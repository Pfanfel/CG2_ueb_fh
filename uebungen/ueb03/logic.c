/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Positionen und Bewegungen der Gegenstände und
 * die Behandlung verschiedener Ereignisse während des Spiels.
 * Die Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 *
 * @author Michael Smirnov & Len Harmsen
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "debugGL.h"
#include "scene.h"
#include "io.h"
#include "util.h"

/* ---- Globale Daten ---- */

/** Status der Lichtberechnung (an/aus) */
static GLboolean g_lightingStatus = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0Status = GL_TRUE;

/* Winkel des Lichtes */
float g_light0Angle = 0.0f;

/* Position der Kamera beim Kameraflug */
CGVector3f g_thirdPersonCameraPosition = {0};

/* Array fuer Kontrollpunkte */
LogicVertex *g_controlPoints = NULL;

/*Array fuer die Loecher*/
CGVector2f *g_holes = NULL;
/* Anzahl der Loecher */
int g_holesAmount = INIT_HOLE_AMOUNT;

/* Ausgewaehlte Barriere */
int g_selectedBarrier = 0;

/* Array fuer die Barrieren */
CGVector2f g_barriers[BARRIER_COUNT] = {0};

/* T Position des Ziels */
float g_targetT = 0;

/* Anzahl der Kontrollpunkte */
GLint g_controlPointAmount = (INITIAL_SURFACES_IN_MESH + 1) * (INITIAL_SURFACES_IN_MESH + 1);

/*Interpolationsmatrix fuer Spline*/
float g_splineInterpolation[16] = {-(1.0f / 6.0f), (1.0f / 2.0f), -(1.0f / 2.0f), (1.0f / 6.0f),
                                   (1.0f / 2.0f), -1.0f, (1.0f / 2.0f), 0,
                                   -(1.0f / 2.0f), 0.0f, (1.0f / 2.0f), 0,
                                   (1.0f / 6.0f), (2.0f / 3.0f), (1.0f / 6.0f), 0};
/*Interpolationsmatrix fuer Bezier*/
float g_bezierInterpolation[16] = {-1.0f, 3.0f, -3.0f, 1.0f,
                                   3.0f, -6.0f, 3.0f, 0.0f,
                                   -3.0f, 3.0f, 0.0f, 0.0f,
                                   1.0f, 0.0f, 0.0f, 0.0f};

/* Bezier Kontrollpunktarray */
CGVector3f g_bezierControlPoints[4] = {0};

/* Subpart in dem Interpoliert wird */
int g_currentSplineSubPartT = 0;
int g_currentSplineSubPartS = 0;

/* Kameraposition */
float g_cameraT = 0.0f;

/* Boolean fuer Kamerafahrt */
GLboolean g_cameraFlight = GL_FALSE;

/* Soll Murmel angestossen werdeen ? */
GLboolean g_pokeMarble = GL_FALSE;

/* Boolean fuer die Murmelbewegung */
GLboolean g_marbelCalculation = GL_FALSE;

/* Array fuer die Murmeln */
Marble g_marbles[MARBLE_COUNT] = {0};

/* Booleans ob Spiel gewonnen oder verloren ist */
GLboolean g_gameWon = GL_FALSE;
GLboolean g_gameLost = GL_FALSE;

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
 * Aendert die Hoehe des gepickten Vertices.
 * 
 * @param vertexIndex Index des zu veraendernden Vertexes
 * @param vertexHeightChange true = erhoehen, false = verringern
 */
void changeControlPointHeight(GLuint vertexIndex, GLboolean vertexHeightChange)
{
    g_controlPoints[vertexIndex][LY] = vertexHeightChange
                                           ? g_controlPoints[vertexIndex][LY] + HEIGHT_CHANGE
                                           : g_controlPoints[vertexIndex][LY] - HEIGHT_CHANGE;
    calculateInterpolatedVertexArray();
}

/**
 * Berechnet den Monomvektor
 * Entweder abgleitet oder nicht
 * @param val der Wert fuer den der Monomvektor berechnet werden soll.
 * @param res der berechnete Monomvektor
 */
static void calculateMonomVector(float val, float *res, GLboolean derivate)
{
    for (int i = 0; i < 4; i++)
    {
        float exp = 3.0f - i;
        if (derivate)
        {
            if (i != 3)
            {
                res[i] = exp * powf(val, exp - 1.0f);
            }
            else
            {
                res[i] = 0.0f;
            }
        }
        else
        {
            res[i] = powf(val, exp);
        }
    }
}

/**
 * Setzt die Geometriewerte der umliegenden Punkte in die Geometriematrix.
 * @param dimension in welcher Dimension interpoliert wird.
 * @param geometryMatrix die Matrix die mit geometriewerten besetzt werden soll.
 */
static void setGeometryMatrix(int dimension, float *geometryMatrix)
{
    int cols = (int)sqrt(g_controlPointAmount);
    int rows = (int)sqrt(g_controlPointAmount);
    int geometryMatrixIndex = 0;
    int controlPointIndex = 0;
    for (int z = 0; z < rows; z++)
    {
        for (int x = 0; x < cols; x++)
        {
            //Pruefung ob im richtigen Unterbereich der Kontrollpunkte
            if (x >= g_currentSplineSubPartT && x < g_currentSplineSubPartT + CONTROL_POINTS_PER_SPLINE_SUBPART &&
                z >= g_currentSplineSubPartS && z < g_currentSplineSubPartS + CONTROL_POINTS_PER_SPLINE_SUBPART)
            {
                geometryMatrix[geometryMatrixIndex] = g_controlPoints[controlPointIndex][dimension];
                //Falls ein Punkt der 4X4 Geometriematrix gefunden wurde
                geometryMatrixIndex++;
            }
            //Weil alle Kontrollpunkte abgeleufen werden
            controlPointIndex++;
        }
    }
}

/**
 * Konvertiert groß T oder S zu klein t oder s. Setzt dabei den aktuellen Teilbereich des Splines.
 * @param T Gross T oder S der zu konvertieren ist
 * @param isT ob es sich um T oder S handelt
 * @return klein t oder s
 */
static float convertTInTAndSubPart(float T, GLboolean isT)
{
    int controlPointPerRow = (int)sqrt(g_controlPointAmount);
    int subParts = 0;
    //Gesamte subParts am Anfang zaehlen -> Da quadratisch nur in eine Dimension
    while (subParts < controlPointPerRow - 4)
    {
        subParts++;
    }
    subParts += 1;
    float splineSubPartWidth = 1.0f / subParts;
    int splineSubPartCounter = 0;
    float t = 0.0f;

    //Fuer t > 1.0f also Objekte die ausserhalb der Splineflaeche gezeichnet werden sollen
    // muss die Schleife abbrechen sonst zaehlt der SubPartCounter zu hoch.
    while (t + splineSubPartWidth < T - DELTA && t + splineSubPartWidth <= 1.0f - DELTA)
    {
        t += splineSubPartWidth;
        splineSubPartCounter++;
    }
    if (isT)
    {
        g_currentSplineSubPartT = splineSubPartCounter;
    }
    else
    {
        g_currentSplineSubPartS = splineSubPartCounter;
    }
    return (T - t) / splineSubPartWidth;
}

/**
 * Uebernimmt die Berechnung des interpolierten Wertes
 * oder die Berechnung der Steigung des interpolierten Wertes
 * @param monomVectorS der zur berechnung benoetigte Monomvektor s
 * @param monomVectorT der zur berechnung benoetigte Monomvektor t
 * @param dimension die zu berechnende Dimension (x=0, y=1, z=2)

 */
static float calculateSplineInterpolationPlain(float *monomVectorS, float *monomVectorT, int dimension)
{
    float multipliedMWithG[16] = {0};
    float multipliedMonomSWithMG[16] = {0};
    float multipliedTransposedInterpolationWithMonomT[16] = {0};
    float geometryMatrix[16] = {0};
    float *transposedInterpolation = transpose(g_splineInterpolation, 4, 4);
    setGeometryMatrix(dimension, geometryMatrix);
    multiply4x4With4x4Matrix(g_splineInterpolation, geometryMatrix, multipliedMWithG);
    multiply1x4With4x4Matrix(monomVectorS, multipliedMWithG, multipliedMonomSWithMG);
    multiply4x4With4x1Matrix(transposedInterpolation, monomVectorT, multipliedTransposedInterpolationWithMonomT);
    free(transposedInterpolation);
    return multiply1x4With4x1Matrix(multipliedMonomSWithMG, multipliedTransposedInterpolationWithMonomT);
}

/**
 * Berechnet die Steigung fuer einen Punkt an der uebergebene Dimension
 * @param S die Zeit in X Dimenstion [0-1]
 * @param T die Zeit in Z Dimenstion [0-1]
 * @param dimension die zu berechnende Dimension (x=0, y=1, z=2)
 * @param isDerivativeS ob nach s oder t abgeleitet wird
 * @return die Steigung des interpolierten Wertes 
 */
float calcGradient(float S, float T, int dimension, GLboolean isDerivativeS)
{
    float t = convertTInTAndSubPart(T, GL_TRUE);
    float s = convertTInTAndSubPart(S, GL_FALSE);
    float monomVectorS[4] = {0};
    float monomVectorT[4] = {0};
    //Benoetigte Monomvektoren berechnen
    if (isDerivativeS)
    {
        calculateMonomVector(s, monomVectorS, GL_TRUE);
        calculateMonomVector(t, monomVectorT, GL_FALSE);
    }
    else
    {
        calculateMonomVector(t, monomVectorT, GL_TRUE);
        calculateMonomVector(s, monomVectorS, GL_FALSE);
    }
    return calculateSplineInterpolationPlain(monomVectorS, monomVectorT, dimension);
}

/**
 * Liefert eine zufaellige Zahl zwischen 0 und 1 inklusive
 * @return die zufaellige Zahl
 */
double getRandomNumber()
{
    return (double)rand() / (double)RAND_MAX;
}

/**
 * Berechnet die Koordinaten fuer eine uebergebene Dimension
 * @param S die Zeit in Z Dimenstion [0-1]
 * @param T die Zeit in X Dimenstion [0-1]
 * @param dimension die zu berechnende Dimension (x=0, y=1, z=2)
 * @return der interpolierte Wert
 */
float interpolate(float S, float T, int dimension)
{
    float t = convertTInTAndSubPart(T, GL_TRUE);
    float s = convertTInTAndSubPart(S, GL_FALSE);
    float monomVectorS[4] = {0};
    float monomVectorT[4] = {0};
    calculateMonomVector(s, monomVectorS, GL_FALSE);
    calculateMonomVector(t, monomVectorT, GL_FALSE);
    return calculateSplineInterpolationPlain(monomVectorS, monomVectorT, dimension);
}

/**
 * Erhoehet die Anzahl der schwarzen Loecher
 */
void increaseHoles(void)
{
    if (g_holesAmount + 1 <= MAX_HOLES)
    {
        g_holesAmount++;
        CGVector2f *tempHoles = malloc(sizeof(CGVector2f) * g_holesAmount);
        if (tempHoles != NULL)
        {
            for (int i = 0; i < g_holesAmount; i++)
            {
                if (i == g_holesAmount - 1)
                {
                    tempHoles[i][LT] = getRandomNumber();
                    tempHoles[i][LS] = getRandomNumber();
                }
                else
                {
                    tempHoles[i][LT] = g_holes[i][LT];
                    tempHoles[i][LS] = g_holes[i][LS];
                }
            }
            free(g_holes);
            g_holes = NULL;
            g_holes = tempHoles;
        }
        else
        {
            free(g_holes);
            printf("Kein virtueller RAM mehr verfügbar ...\n");
            exit(1);
        }
    }
}

/**
 * Verringert die Anzahl der schwarzen Loecher
 */
void decreaseHoles(void)
{
    if (g_holesAmount - 1 >= MIN_HOLES)
    {
        g_holesAmount--;
        CGVector2f *tempHoles = malloc(sizeof(CGVector2f) * g_holesAmount);
        if (tempHoles != NULL)
        {
            for (int i = 0; i < g_holesAmount; i++)
            {
                tempHoles[i][LT] = g_holes[i][LT];
                tempHoles[i][LS] = g_holes[i][LS];
            }
            free(g_holes);
            g_holes = NULL;
            g_holes = tempHoles;
        }
        else
        {
            free(tempHoles);
            printf("Kein virtueller RAM mehr verfügbar ...\n");
            exit(1);
        }
    }
}

/**
 * Initialisiert die Murmeln
 */
void initMarbles(void)
{
    for (int i = 0; i < MARBLE_COUNT; i++)
    {
        float fieldWidth = getInterpolatedMeshWidth();
        float minMarbleSpacing = MARBLE_RADIUS / fieldWidth;
        float currPossiblePositionT = i + getRandomNumber();
        g_marbles[i].center[LT] = clip((currPossiblePositionT / 10.0f), (i / 10.0f) + minMarbleSpacing, ((i + 1.0f) / 10.0f) - minMarbleSpacing);
        float S = (MARBLE_RADIUS) / fieldWidth;
        g_marbles[i].center[LS] = S;
        g_marbles[i].velocity[LX] = 0.0f;
        g_marbles[i].velocity[LY] = 0.0f;
        g_marbles[i].velocity[LZ] = 0.0f;
        g_marbles[i].mass = getRandomNumber() + 1.0f;
        g_marbles[i].isVisible = GL_TRUE;
    }
}

/**
 * Initialisiert die schwarzen Loecher
 */
void initHoles(void)
{
    g_holes = malloc(sizeof(CGVector2f) * g_holesAmount);
    if (g_holes != NULL)
    {
        for (int i = 0; i < g_holesAmount; i++)
        {
            g_holes[i][LT] = getRandomNumber();
            g_holes[i][LS] = getRandomNumber();
        }
    }
    else
    {
        free(g_holes);
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
}

/**
 * Initialisiert die Barrieren
 */
static void initBarriers(void)
{
    for (int i = 0; i < BARRIER_COUNT; i++)
    {
        g_barriers[i][LT] = getRandomNumber();
        g_barriers[i][LS] = getRandomNumber();
    }
}

/**
 * Initialisiert das Ziel an einem Random t-Wert
 */
static void initTarget(void)
{
    g_targetT = getRandomNumber();
}

/**
 * Initialisiert die Logic beim Start.
 */
void initLogic(void)
{
    //Initialisierung der "zufaelligen" Zahlen
    srand((unsigned)time(NULL));
    /* Initialisiert Positionen der Objekte auf dem Mesh */
    initBarriers();
    initHoles();
    initTarget();
    // Initialisieren des Logic Vertex Arrays
    initControlPointArray();
    /* Initialisiert das Vertex Array der Scene */
    calculateInterpolatedVertexArray();
    /* Init Marbles zuletzt, da abhaengig von der InterpolatedMeshWidth */
    initMarbles();
}

/**
 * Initialisiert das Vertex Arrays in der Logik.
 */
void initControlPointArray(void)
{
    g_controlPoints = realloc(g_controlPoints, sizeof(LogicVertex) * g_controlPointAmount);

    int cols = (int)sqrt(g_controlPointAmount);
    int rows = (int)sqrt(g_controlPointAmount);
    int vertexIndex = 0;
    for (int z = 0; z < rows; z++)
    {
        for (int x = 0; x < cols; x++)
        {
            g_controlPoints[vertexIndex][LX] = (-(FIELD_WIDTH / 2) + (x * FIELD_WIDTH / (float)(cols - 1)));
            g_controlPoints[vertexIndex][LY] = getRandomNumber() - (((float)z) * SLOPE_FACTOR);
            g_controlPoints[vertexIndex][LZ] = (-(FIELD_HEIGHT / 2) + (z * FIELD_HEIGHT / (float)(rows - 1)));
            vertexIndex++;
        }
    }
}

/**
 * Liefert die Anzahl der Kontrollpunkte
 * @return die Anzahl
 */
int getControlPointAmount(void)
{
    return g_controlPointAmount;
}

/**
 * Liefert die X Koord. des Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die X Koord.
 */
float getControlPointX(int idx)
{
    return g_controlPoints[idx][LX];
}

/**
 * Liefert die Y Koord. des Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die Y Koord.
 */
float getControlPointY(int idx)
{
    return g_controlPoints[idx][LY];
}

/**
 * Liefert die Z Koord. des Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die Z Koord.
 */
float getControlPointZ(int idx)
{
    return g_controlPoints[idx][LZ];
}

/**
 * Liefert die X Koord. des Bezier Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die X Koord.
 */
float getBezierControlPointX(int idx)
{
    return g_bezierControlPoints[idx][LX];
}

/**
 * Liefert die Y Koord. des Bezier Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die Y Koord.
 */
float getBezierControlPointY(int idx)
{
    return g_bezierControlPoints[idx][LY];
}

/**
 * Liefert die Z Koord. des Bezier Kontrollpunktes
 * @param idx der Index des Kontrollpunktes
 * @return die Z Koord.
 */
float getBezierControlPointZ(int idx)
{
    return g_bezierControlPoints[idx][LZ];
}

/**
 * Liefert die Anzahl der schwarzen Loecher
 * @return die Anzahl
 */
float getHoleAmount(void)
{
    return g_holesAmount;
}

/**
 * Liefert die t-Position eines schwarzen Loches
 * @param i Index des Loches
 * @return das T des Loches
 */
float getHoleT(int i)
{
    return g_holes[i][LT];
}

/**
 * Liefert die s-Position eines schwarzen Loches
 * @param i Index des Loches
 * @return das S des Loches
 */
float getHoleS(int i)
{
    return g_holes[i][LS];
}
/**
 * Kopiert die Inhalte aus dem Globalen Vertices Array in das Temporaere fuer das veraendern der groesse
 * @param dest temporaeres Vertice Array
 * @param vertIndex Index im temporaeren Vertice Array
 * @param vertexIndexGlobal Index im globalen Vertice Array
 */
static void copyIntoTemp(LogicVertex *dest, int vertexIndex, int vertexIndexGlobal, int x, int z, int cols, int rows)
{
    dest[vertexIndex][LX] = (-(FIELD_WIDTH / 2) + (x * FIELD_WIDTH / (float)(cols - 1)));
    dest[vertexIndex][LY] = g_controlPoints[vertexIndexGlobal][LY];
    dest[vertexIndex][LZ] = (-(FIELD_HEIGHT / 2) + (z * FIELD_HEIGHT / (float)(rows - 1)));
}

/**
 * Veraendert die Groesse des Vertex Arrays in der Logik.
 * @param vertexCount Anzahl der Vertices
 * @param inc ob erhoeht oder verringert wird
 */
void resizeLogicVertexArray(GLboolean inc)
{
    // Laufvariablen
    int vertexIndex = 0;
    int vertexIndexGlobal = 0;
    int cols = (int)sqrt(g_controlPointAmount);
    int rows = (int)sqrt(g_controlPointAmount);

    LogicVertex *tempVerticeArray = malloc(sizeof(LogicVertex) * g_controlPointAmount);

    if (tempVerticeArray != NULL)
    {
        //Alle Punke neu berechnen da diese verschoben
        for (int z = 0; z < rows; z++)
        {
            for (int x = 0; x < cols; x++)
            {
                // Increase Vertices
                if (inc)
                {
                    //Am Rand neuer Vertex init.
                    if (z == rows - 1 || x == cols - 1)
                    {
                        tempVerticeArray[vertexIndex][LX] = (-(FIELD_WIDTH / 2) + (x * FIELD_WIDTH / (float)(cols - 1)));
                        tempVerticeArray[vertexIndex][LY] = getRandomNumber() - (((float)z) * SLOPE_FACTOR);
                        tempVerticeArray[vertexIndex][LZ] = (-(FIELD_HEIGHT / 2) + (z * FIELD_HEIGHT / (float)(rows - 1)));
                    }
                    //Vorher schon vorhanden
                    else
                    {
                        copyIntoTemp(tempVerticeArray, vertexIndex, vertexIndexGlobal, x, z, cols, rows);
                        //Nur erhoehen wenn aus globalen Array kopieren
                        vertexIndexGlobal++;
                    }
                    //Jedes mal
                    vertexIndex++;
                }
                // Decrease Vertices
                else
                {
                    //temp ist nun kleiner als global
                    copyIntoTemp(tempVerticeArray, vertexIndex, vertexIndexGlobal, x, z, cols, rows);
                    vertexIndexGlobal++;
                    vertexIndex++;
                    //Wenn x am Rand
                    if (x == cols - 1)
                    {
                        //Ueberspringen aka. loeschen
                        vertexIndexGlobal++;
                    }
                }
            }
        }
        free(g_controlPoints);
        g_controlPoints = NULL;
        g_controlPoints = tempVerticeArray;
    }
    else
    {
        free(tempVerticeArray);
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
}

/**
 * Berechnet die Position der Kamera aus Kugelkoordinaten in Karthesische Koord.
 * @return Array mit x,y und z Position
 */
CGVector3f *calculateThridPersonCameraPosition(void)
{
    float phi = getPhi();
    float theta = getTheta();
    float radius = getRadius();
    /* Winkel der Kamera in Radiant umrechnen */
    float thetaInRad = degreeToRad(90 - theta);
    float phiInRad = degreeToRad(phi);

    float thirdPersonEyeX = radius * sin(thetaInRad) * cos(phiInRad);
    float thirdPersonEyeY = radius * cos(thetaInRad);
    float thirdPersonEyeZ = radius * sin(thetaInRad) * sin(phiInRad);

    g_thirdPersonCameraPosition[0] = thirdPersonEyeX;
    g_thirdPersonCameraPosition[1] = thirdPersonEyeY;
    g_thirdPersonCameraPosition[2] = thirdPersonEyeZ;
    return &g_thirdPersonCameraPosition;
}

/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
GLboolean getLightingStatus(void)
{
    return g_lightingStatus;
}

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingStatus(GLboolean status)
{
    g_lightingStatus = status;
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
GLboolean getLight0Status(void)
{
    return g_light0Status;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0Status(GLboolean status)
{
    g_light0Status = status;
}

/**
 * Setzt den Status des Kamerafluges
 * @param status Status des Kamerafluges (an/aus).
 */
void setCameraFlightStatus(GLboolean status)
{
    g_cameraFlight = status;
}

/**
 * Liefert den Status des Kemarafluges
 * @return der Status des Kamerafluges
 */
GLboolean getCameraFlightStatus(void)
{
    return g_cameraFlight;
}

/**
 * Berechnet das T beim Kameraflug und resettet die Werte nach einem Durchlauf.
 * @param interval Verstrichene Zeit in millisekunden.
 */
static void calcCameraFlightT(float interval)
{
    if ((g_cameraT + (CAMERA_MOVEMENT_STEP * interval)) <= 1.0f)
    {
        g_cameraT += CAMERA_MOVEMENT_STEP * interval;
    }
    else
    {
        g_cameraT = 0.0f;
        g_cameraFlight = GL_FALSE;
    }
}

/**
 * Liefert das Zeitintervall des Kamerafluges.
 * @return das Zeitintervall [0..1].
 */
float getCameraT(void)
{
    return g_cameraT;
}

/**
 * Berechnet den naechsten Step des Lichtes vom Interval abhaengig
 * @param interval Verstrichene Zeit in millisekunden.
 */
void calcLight0RotationAngle(float interval)
{
    g_light0Angle += interval * LIGHT_ROTATION_STEP;
    if (g_light0Angle > 360)
    {
        g_light0Angle -= 360;
    }
}

/**
 * Gibt den Winkel des Lichtes zurueck
 * @return Winkel des Lichtes
 */
float getLight0Angle(void)
{
    return g_light0Angle;
}

/**
 * Verschiebt die Kugel mittels Euler Integration.
 * @param interval die verstrichen Zeit
 * @param i Index der Kugel
 * @param penaltyAccelaration die Gegenbeschleunigung, falls Kugel kollidiert 
 */
static void moveMarble(double interval, int i, float *penaltyAccelaration)
{
    //Position bestimmen
    CGVector2f oldPosition = {g_marbles[i].center[LT], g_marbles[i].center[LS]};
    float worldX = interpolate(oldPosition[LS], oldPosition[LT], LX);
    float worldZ = interpolate(oldPosition[LS], oldPosition[LT], LZ);

    //Vektoren zur Berechnung erstellen
    CGVector3f oldVelocity = {g_marbles[i].velocity[LX], g_marbles[i].velocity[LY], g_marbles[i].velocity[LZ]};
    CGVector3f normal = {0};
    CGVector3f gravity = {0.0f, GRAVITY, 0.0f};
    CGVector3f l = {0};
    CGVector3f force = {0};
    CGVector3f accelaration = {0};
    CGVector3f accelarationMultipliedWithInterval = {0};
    CGVector3f velocityMultipliedWithInterval = {0};

    //Normale der Flaeche holen
    calcVertexNormal(oldPosition[LS], oldPosition[LT], normal);

    float mass = g_marbles[i].mass;
    // g * n
    float gravityProjectedOnNegativNormal = calcDotProduct(gravity, normal);
    // l = n * (g * n)
    multiplyVectorWithScalar(normal, gravityProjectedOnNegativNormal, l);
    // f = g - l
    subtractVectos(gravity, l, force);
    // a = f / m
    divideVectorWithScalar(force, mass, accelaration);
    //Beschleunigungen zusammenrechnen
    addVectors(accelaration, penaltyAccelaration, accelaration);
    // delta(t) * a
    multiplyVectorWithScalar(accelaration, interval, accelarationMultipliedWithInterval);
    // v = v + delta(t) * a
    addVectors(oldVelocity, accelarationMultipliedWithInterval, oldVelocity);
    // Reibung dazumultiplizieren
    multiplyVectorWithScalar(oldVelocity, FRICTION, g_marbles[i].velocity);
    // delta(t) * v
    multiplyVectorWithScalar(g_marbles[i].velocity, interval, velocityMultipliedWithInterval);
    // s = s + delta(t) * v
    float newWorldX = worldX + velocityMultipliedWithInterval[LX];
    float newWorldZ = worldZ + velocityMultipliedWithInterval[LZ];
    //Werte in S und T umrechnen
    float interpolatedWidth = getInterpolatedMeshWidth();
    convertGlobalCoorinatesToInterpolationInterval(newWorldX, newWorldZ, interpolatedWidth, &g_marbles[i].center[LS], &g_marbles[i].center[LT]);
}

/**
 * Berechnet die Gegenbeschleunigung nach der Penalty-Methode
 * @param penetrationDepth die Eindringungstiefe in das Objekt
 * @param penaltyAccelaration die berechnete Gegenbeschleunigung
 * @param normal die normale des Objekts mit dem Kollidiert wurde
 * @param i index der Murmel
 */
static void calculatePenaltyAccelaration(float penetrationDepth, float *penaltyAccelaration, float *normal, int i)
{
    //Falls mehere kraefte innerhalb einer kollision auf die Murmeln wirken
    CGVector3f tempPenaltyAcceleration = {0};
    // f = k * d
    float fPenalty = SPRING_CONSTANT * penetrationDepth;
    CGVector3f fPenaltyVector = {0};
    // fpenalty = f * n
    multiplyVectorWithScalar(normal, fPenalty, fPenaltyVector);
    // a = fpenalty / m
    divideVectorWithScalar(fPenaltyVector, g_marbles[i].mass, tempPenaltyAcceleration);
    addVectors(penaltyAccelaration, tempPenaltyAcceleration, penaltyAccelaration);
}

/**
 * Kuemmert sich um die Kollisionen der Murmel mit den Waenden
 * @param i der Index der Murmel
 * @param worldX die X Koord. der Murmel in Weltkoord.
 * @param worldZ die Z Koord. der Murmel in Weltkoord.
 * @param penaltyAccelaration die berechnete Gegenbeschleunigung
 */
static void handleCollisionWithWall(int i, float worldX, float worldZ, float *penaltyAccelaration)
{
    float interpolatedMeshWidth = getInterpolatedMeshWidth() / 2.0f;

    float distanceToLeftWall = (worldX - MARBLE_RADIUS) - (-interpolatedMeshWidth);
    //Umgedreht da Distanz innerhalb des Spielfeldes immer positiv ist.
    float distanceToRightWall = interpolatedMeshWidth - (worldX + MARBLE_RADIUS);
    float distanceToUpperWall = (worldZ - MARBLE_RADIUS) - (-interpolatedMeshWidth);
    //Umgedreht da Distanz innerhalb des Spielfeldes immer positiv ist.
    float distanceToLowerWall = interpolatedMeshWidth - (worldZ + MARBLE_RADIUS);

    CGVector3f normal = {0};
    //Linke Wand beruehrt
    if (distanceToLeftWall <= 0.0f - DELTA)
    {
        setVector(1.0f, 0.0f, 0.0f, normal);

        calculatePenaltyAccelaration(-distanceToLeftWall, penaltyAccelaration, normal, i);
    }
    //Rechte Wand beruehrt
    if (distanceToRightWall <= 0.0f - DELTA)
    {
        setVector(-1.0f, 0.0f, 0.0f, normal);
        calculatePenaltyAccelaration(-distanceToRightWall, penaltyAccelaration, normal, i);
    }
    //Obere Wand beruehrt
    if (distanceToUpperWall <= 0.0f - DELTA)
    {
        setVector(0.0f, 0.0f, 1.0f, normal);
        calculatePenaltyAccelaration(-distanceToUpperWall, penaltyAccelaration, normal, i);
    }
    //Untere Wand beruehrt
    if (distanceToLowerWall <= 0.0f + DELTA)
    {
        setVector(0.0f, 0.0f, -1.0f, normal);
        calculatePenaltyAccelaration(-distanceToLowerWall, penaltyAccelaration, normal, i);
    }
}

/**
 * Prueft ob die Murmel eine Barriere getroffen hat.
 * @param px x-Wert der Kollisions der Murmel
 * @param py y-Wert der Kollisions der Murmel
 * @param cx x-Position der Barriere
 * @param cz z-Position der Barriere
 * @param w Breite der Barriere
 * @param h Hoehe der Barriere
 * @return GL_TRUE wenn Kollision
 */
static GLboolean isPointInBox(float px, float py, float cx, float cz, float w, float h)
{
    return px >= cx - w / 2.0f && px <= cx + w / 2.0f && py >= cz - h / 2.0f && py <= cz + h / 2.0f;
}

/**
 * Kuemmert sich die Kollision der Murmel mit den Barrieren
 * @param i der Index der Murmel
 * @param worldX die X Koord. der Murmel in Weltkoord.
 * @param worldY die Y Koord. der Murmel in Weltkoord.
 * @param worldZ die Z Koord. der Murmel in Weltkoord.
 * @param penaltyAccelaration die berechnete Gegenbeschleunigung
 */
static void handleCollisionWithBarriers(int i, float worldX, float worldY, float worldZ, float *penaltyAccelaration)
{
    for (int i = 0; i < BARRIER_COUNT; i++)
    {
        float S = g_barriers[i][LS];
        float T = g_barriers[i][LT];
        float x = interpolate(S, T, LX);
        float z = interpolate(S, T, LZ);
        float barrierWidth;
        float barrierHeight;
        //Vier Stueck sind rotiert
        if (i < ROTATED_BARRIER_COUNT)
        {
            barrierWidth = BARRIER_HEIGHT;
            barrierHeight = BARRIER_WIDTH;
        }
        else
        {
            barrierWidth = BARRIER_WIDTH;
            barrierHeight = BARRIER_HEIGHT;
        }
        CGVector3f normal = {0};
        float marbleLeft = worldX - MARBLE_RADIUS * 2;
        float marbleRight = worldX + MARBLE_RADIUS * 2;
        float marbleUp = worldZ - MARBLE_RADIUS * 2;
        float marbleDown = worldZ + MARBLE_RADIUS * 2;

        //Rechte Wand beruehrt
        if (isPointInBox(marbleLeft, worldZ, x, z, barrierWidth, barrierHeight))
        {
            setVector(1.0f, 0.0f, 0.0f, normal);
            float distanceToRightWall = marbleLeft - (x + barrierWidth / 2.0f);
            calculatePenaltyAccelaration(-distanceToRightWall, penaltyAccelaration, normal, i);
        }
        //Linke Wand beruehrt
        if (isPointInBox(marbleRight, worldZ, x, z, barrierWidth, barrierHeight))
        {
            setVector(-1.0f, 0.0f, 0.0f, normal);
            float distanceToLeftWall = (x - barrierWidth / 2.0f) - marbleRight;
            calculatePenaltyAccelaration(-distanceToLeftWall, penaltyAccelaration, normal, i);
        }
        //Untere Wand beruehrt
        if (isPointInBox(worldX, marbleUp, x, z, barrierWidth, barrierHeight))
        {
            setVector(0.0f, 0.0f, 1.0f, normal);
            float distanceToLowerWall = marbleUp - (z + barrierHeight / 2.0f);
            calculatePenaltyAccelaration(-distanceToLowerWall, penaltyAccelaration, normal, i);
        }
        //Obere Wand beruehrt
        if (isPointInBox(worldX, marbleDown, x, z, barrierWidth, barrierHeight))
        {
            setVector(0.0f, 0.0f, -1.0f, normal);
            float distanceToUpperWall = (z - barrierHeight / 2.0f) - marbleDown;
            calculatePenaltyAccelaration(-distanceToUpperWall, penaltyAccelaration, normal, i);
        }
    }
}

/**
 * Kuemmert sich um die Kollisionen der Murmeln untereinder
 * @param i der Index der Murmel
 * @param worldX die X Koord. der Murmel in Weltkoord.
 * @param worldY die Y Koord. der Murmel in Weltkoord.
 * @param worldZ die Z Koord. der Murmel in Weltkoord.
 * @param penaltyAccelaration die berechnete Gegenbeschleunigung
 */
static void handleCollisionWithMarbles(int i, float worldX, float worldY, float worldZ, float *penaltyAccelaration)
{
    for (int j = 0; j < MARBLE_COUNT; j++)
    {
        if (j != i)
        {
            float S = g_marbles[j].center[LS];
            float T = g_marbles[j].center[LT];
            float centerCollisionMarbleX = interpolate(S, T, LX);
            float centerCollisionMarbleY = interpolate(S, T, LY);
            float centerCollisionMarbleZ = interpolate(S, T, LZ);
            CGVector3f distanceVector = {worldX - centerCollisionMarbleX, worldY - centerCollisionMarbleY, worldZ - centerCollisionMarbleZ};
            float distanceBetweenMarbles = calcVectorLength(distanceVector);
            //TODO: Klaeren ob in ordnung
            if (distanceBetweenMarbles <= (MARBLE_RADIUS * 2) + DELTA)
            {
                CGVector3f normal = {0};
                divideVectorWithScalar(distanceVector, distanceBetweenMarbles, normal);
                float penetrationDepth = ((MARBLE_RADIUS * 2) + DELTA) - distanceBetweenMarbles;
                calculatePenaltyAccelaration(penetrationDepth, penaltyAccelaration, normal, i);
            }
        }
    }
}

/**
 * Prueft ob eine Murmel innerhab eine Sphaere liegt
 * @param i der Index der Murmel
 * @param s die s Komponente der zu ueberpuefenden Sphaere
 * @param t die t Komponente der zu ueberpuefenden Sphaere
 * @param raduis der Radius der zu ueberpuefenden Sphaere
 * @return ob die Murmel innerhalb der Sphare liegt oder nicht
 */
static GLboolean checkMarbleInSphere(int i, float s, float t, float radius)
{
    GLboolean ret = GL_FALSE;
    CGVector2f marblePosition = {g_marbles[i].center[LT], g_marbles[i].center[LS]};
    float worldX = interpolate(marblePosition[LS], marblePosition[LT], LX);
    float worldY = interpolate(marblePosition[LS], marblePosition[LT], LY);
    float worldZ = interpolate(marblePosition[LS], marblePosition[LT], LZ);
    float targetX = interpolate(s, t, LX);
    float targetY = interpolate(s, t, LY);
    float targetZ = interpolate(s, t, LZ);
    CGVector3f distanceVector = {worldX - targetX, worldY - targetY, worldZ - targetZ};
    float distanceToTarget = calcVectorLength(distanceVector);
    if (distanceToTarget + MARBLE_RADIUS <= radius - DELTA)
    {
        ret = GL_TRUE;
    }
    return ret;
}

/**
 * Kuemmert sich um die Anziehung bei den Schwarzen loechern
 * @param i der Index der Kugel
 * @param worldX die x Koord. der Murmel in Weltkoord.
 * @param worldY die y Koord. der Murmel in Weltkoord.
 * @param worldZ die z Koord. der Murmel in Weltkoord.
 * @param penaltyAccelaration die berechnete Gegenbeschleunigung
 */
static void handleAttraction(int i, float worldX, float worldY, float worldZ, float *penaltyAccelaration)
{
    for (int j = 0; j < g_holesAmount; j++)
    {
        if (j != i)
        {
            float S = g_holes[j][LS];
            float T = g_holes[j][LT];
            float centerHoleX = interpolate(S, T, LX);
            float centerHoleY = interpolate(S, T, LY);
            float centerHoleZ = interpolate(S, T, LZ);
            CGVector3f distanceVector = {centerHoleX - worldX, centerHoleY - worldY, centerHoleZ - worldZ};
            float distanceBetweenMarbleAndHole = calcVectorLength(distanceVector);
            if (distanceBetweenMarbleAndHole <= ATTRACTION_DISTANCE - DELTA)
            {
                float attractionValue = ATTRACTION_DISTANCE - DELTA - distanceBetweenMarbleAndHole;
                multiplyVectorWithScalar(distanceVector, attractionValue * ATTRACTION_FACTOR, distanceVector);
                addVectors(penaltyAccelaration, distanceVector, penaltyAccelaration);
            }
            //Murmel wurde verschluckt
            if (checkMarbleInSphere(i, S, T, HOLE_RADIUS))
            {
                g_marbles[i].isVisible = GL_FALSE;
            }
        }
    }
}

/**
 * Kuemmert sich um die Kollisionen der Kugeln mit den Gegenstaenden
 * @param i der Index der Kugel
 * @param penaltyAccelaration die Gegenbeschleunigung die gesetzt wird, falls eine Kollision vorliegt
 */
static void handleMarbleCollision(int i, float *penaltyAccelaration)
{
    CGVector2f oldPosition = {g_marbles[i].center[LT], g_marbles[i].center[LS]};
    float worldX = interpolate(oldPosition[LS], oldPosition[LT], LX);
    float worldY = interpolate(oldPosition[LS], oldPosition[LT], LY);
    float worldZ = interpolate(oldPosition[LS], oldPosition[LT], LZ);

    handleCollisionWithWall(i, worldX, worldZ, penaltyAccelaration);
    handleCollisionWithBarriers(i, worldX, worldY, worldZ, penaltyAccelaration);
    handleCollisionWithMarbles(i, worldX, worldY, worldZ, penaltyAccelaration);
    handleAttraction(i, worldX, worldY, worldZ, penaltyAccelaration);
}

/**
 * Kuemmert sich um das Anstupsen einer Kugel
 * @param i der Index der anzustupsenden Kugel
 * @param penaltyAccelaration der berechnete Vektor in den die Kugel weggestossen wird.
 */
static void handleMarblePoke(float *penaltyAccelaration)
{
    //Random Vektor
    float pokeX = getRandomNumber() - 0.5f;
    float pokeY = getRandomNumber() - 0.5f;
    float pokeZ = getRandomNumber() - 0.5f;
    CGVector3f pokeVector = {pokeX, pokeY, pokeZ};
    //Normieren
    float vectorLength = calcVectorLength(pokeVector);
    divideVectorWithScalar(pokeVector, vectorLength, pokeVector);
    //Anstupskraft hinzufuegen
    multiplyVectorWithScalar(pokeVector, POKE_FACTOR, pokeVector);
    //Addieren
    addVectors(penaltyAccelaration, pokeVector, penaltyAccelaration);
    //Damit nur die erste Kugel angestupst wird
    g_pokeMarble = GL_FALSE;
}

/**
 * Startet die Murmeln
 */
void startMarbles(void)
{
    g_marbelCalculation = GL_TRUE;
}

/**
 * Kuemmert sich um die Animation/Bewegung der Murmeln auf der Splineflaeche
 * @param interval das verstrichene Intervall seid dem letzten Zeichen
 */
static void handleMarbleMovement(double interval)
{

    for (int i = 0; i < MARBLE_COUNT; i++)
    {
        if (g_marbles[i].isVisible)
        {
            CGVector3f penaltyAccelaration = {0.0f, 0.0f, 0.0f};
            handleMarbleCollision(i, penaltyAccelaration);
            if (g_pokeMarble)
            {
                handleMarblePoke(penaltyAccelaration);
            }
            moveMarble(interval, i, penaltyAccelaration);
        }
    }
}

/**
 * Prueft ob das Spiel gewonnen wurde.
 */
static void checkGameWon(void)
{
    for (int i = 0; i < MARBLE_COUNT && !g_gameWon; i++)
    {
        if (checkMarbleInSphere(i, 1.0f, g_targetT, TARGET_RADIUS))
        {
            g_gameWon = GL_TRUE;
        }
    }
}

/**
 * Prueft ob das Spiel verloren wurde.
 */
static void checkGameLost(void)
{
    GLboolean res = GL_TRUE;
    for (int i = 0; i < MARBLE_COUNT; i++)
    {
        res &= !g_marbles[i].isVisible;
    }

    g_gameLost = res;
}

/**
 * Kuemmert sich um die Berechnungen in der Logik
 * @param interval Verstrichene Zeit in millisekunden.
 */
void handleLogicCalculations(double interval)
{
    if (!g_gameWon)
    {
        calcLight0RotationAngle(interval);
        if (g_cameraFlight)
        {
            calcCameraFlightT(interval);
        }
        if (g_marbelCalculation)
        {
            while (interval >= UPDATE_CALL)
            {
                handleMarbleMovement(UPDATE_CALL);
                interval -= UPDATE_CALL;
            }
        }
        checkGameWon();
        checkGameLost();
    }
    if (g_gameLost)
    {
        resetGame();
    }
}

/**
 * Startet das Spiel von vorne
 */
void resetGame(void)
{
    g_selectedBarrier = 0;
    g_targetT = 0;
    g_marbelCalculation = GL_FALSE;
    g_gameWon = GL_FALSE;
    initLogic();
}

/**
 * Gibt den Speicher der dynamisch allozierten Array in der logic frei  
 */
void freeArraysLogic(void)
{
    free(g_controlPoints);
    free(g_holes);
}

/**
 * Erhoeht die Anzahl an Vertices.
 */
void increaseVertices(void)
{
    int controlPointPerRow = (int)sqrt(g_controlPointAmount);
    if (controlPointPerRow + 1 <= MAX_CONTROLPOINTS_IN_MESH)
    {
        g_controlPointAmount = (controlPointPerRow + 1) * (controlPointPerRow + 1);
        resizeLogicVertexArray(GL_TRUE);
        calculateInterpolatedVertexArray();
    }
}

/**
 * Verringert die Anzahl an Vertices.
 */
void decreaseVertices(void)
{
    int controlPointPerRow = (int)sqrt(g_controlPointAmount);
    if (controlPointPerRow - 1 >= MIN_CONTROLPOINTS_IN_MESH)
    {
        g_controlPointAmount = (controlPointPerRow - 1) * (controlPointPerRow - 1);
        resizeLogicVertexArray(GL_FALSE);
        calculateInterpolatedVertexArray();
    }
}

/**
 * Verschiebt die aktuelle Barriere einen Schritt nach oben.
 */
void moveBarrierUp(void)
{
    if (g_barriers[g_selectedBarrier][LS] - BARRIER_STEP >= 0.0f + DELTA)
    {
        g_barriers[g_selectedBarrier][LS] -= BARRIER_STEP;
    }
}

/**
 * Verschiebt die aktuelle Barriere einen Schritt nach unten.
 */
void moveBarrierDown(void)
{
    if (g_barriers[g_selectedBarrier][LS] + BARRIER_STEP <= 1.0f - DELTA)
    {
        g_barriers[g_selectedBarrier][LS] += BARRIER_STEP;
    }
}

/**
 * Verschiebt die aktuelle Barriere einen Schritt nach links. 
 */
void moveBarrierLeft(void)
{
    if (g_barriers[g_selectedBarrier][LT] - BARRIER_STEP >= 0.0f + DELTA)
    {
        g_barriers[g_selectedBarrier][LT] -= BARRIER_STEP;
    }
}

/**
 * Verschiebt die aktuelle Barriere einen Schritt nach rechts. 
 */
void moveBarrierRight(void)
{
    if (g_barriers[g_selectedBarrier][LT] + BARRIER_STEP <= 1.0f - DELTA)
    {
        g_barriers[g_selectedBarrier][LT] += BARRIER_STEP;
    }
}

/**
 * Setzt die Kontrollpunkte fuer die Bezier Interpolation
 * @param idx der Index des Kontrollpunktes
 * @param x die X Koord.
 * @param y die Y Koord.
 * @param z die Z Koord.
 */
void setg_bezierControlPoint(int idx, float x, float y, float z)
{
    g_bezierControlPoints[idx][LX] = x;
    g_bezierControlPoints[idx][LY] = y;
    g_bezierControlPoints[idx][LZ] = z;
}

/**
 * Liefert die uebergebene Komponente fuer den uebergeben Zeitpunkt der nach Bizer interpolieten Position.
 * @param T der uebergebener Zeitpunkt
 * @param dimension die zu berechnende Dimension (x=0,y=1,z=2)
 * @return die Koordinate der Dimension zu dem jew. Zeitpunkt in glob. Koord.
 */
float getBezier(float T, int dimension)
{
    float monomVector[4] = {0};
    calculateMonomVector(T, monomVector, GL_FALSE);
    float monomWithInterpolation[4] = {0};
    multiply1x4With4x4Matrix(monomVector, g_bezierInterpolation, monomWithInterpolation);
    return multiply1x4With4x1MatrixByDimension(g_bezierControlPoints, dimension, monomWithInterpolation);
}

/**
 * Liefert die Koordinate der uebergebene Dimension des Punktes, in den die Kamera schauen soll, wenn diese entlang der Bezier Kurve fliegt.
 * @param T der Zeitpunkt der Interpolation (0..1)
 * @param dimension die zu berechnende Dimension (x=0,y=1,z=2)
 * @return die Koordinate der uebergebenen Dimension zu dem Zeipunkt in die die Kamera schauen soll.
 */
float getDirection(float T, int dimension)
{
    float monomVector[4] = {0};
    calculateMonomVector(T, monomVector, GL_TRUE);
    float monomWithInterpolation[4] = {0};
    multiply1x4With4x4Matrix(monomVector, g_bezierInterpolation, monomWithInterpolation);
    return multiply1x4With4x1MatrixByDimension(g_bezierControlPoints, dimension, monomWithInterpolation);
}

/**
 * Setzt den Anstubsstatus auf true
 */
void pokeMarble(void)
{
    g_pokeMarble = GL_TRUE;
}

/**
 * Gibt die t-Position einer Barriere zurueck.
 * @param i index der Barriere
 * @return t-Position
 */
float getBarrierT(int i)
{
    return g_barriers[i][LT];
}

/**
 * Gibt die s-Position einer Barriere zurueck.
 * @param i index der Barriere
 * @return s-Position
 */
float getBarrierS(int i)
{
    return g_barriers[i][LS];
}

/**
 * Liefert den Index der aktuell ausgewaehlten Barriere
 * @return der Index
 */
int getSelectedBarrier(void)
{

    return g_selectedBarrier;
}

/**
 * Selektiert ein schwarzes Loch
 * @param i selektiertes Loch.
 */
void setSelectedBarrierIndex(int i)
{
    g_selectedBarrier = i;
}

/**
 * Liefert die T-Position des Ziels
 * @return die T-Position 
 */
float getTargetT(void)
{
    return g_targetT;
}

/**
 * Liefert die S-Position einer Kugel.
 * @param i index der Kugel
 * @return die S-Position
 */
float getMarbleS(int i)
{
    return g_marbles[i].center[LS];
}

/**
 * Liefert die T-Position einer Kugel.
 * @param i index der Kugel
 * @return die T-Position
 */
float getMarbleT(int i)
{
    return g_marbles[i].center[LT];
}

/**
 * Liefert den boolischen Wert ob die Murmel sichtbar ist
 * @param i Index der Kugel
 * @return Sichtbarkeit true wenn sichtabr
 */
GLboolean isMarbleVisible(int i)
{
    return g_marbles[i].isVisible;
}

/**
 * Liefert den boolischen Wert, ob das Spiel gewonnen wurde
 * @return True, wenn das Spiel gewonnen wurde
 */
GLboolean getGameWonStatus(void)
{
    return g_gameWon;
}