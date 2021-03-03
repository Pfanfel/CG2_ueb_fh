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

/** Status der der Lichtberechnung (an/aus) */
static GLboolean g_lightingStatus = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0Status = GL_TRUE;

float g_light0Angle = 0.0f;

CGVector3f g_thirdPersonCameraPosition = {0};

/**Array fuer Kontrollpunkte */
LogicVertex *g_controlPoints = NULL;

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

int g_currentSplineSubPartT = 0;
int g_currentSplineSubPartS = 0;

float g_cameraT = 0.0f;

GLboolean g_cameraFlight = GL_FALSE;

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
 * Berechnet den Monomvector
 * Entweder abgleitet oder nicht
 * @param val der Wert fuer den der Monomvector berechnet werden soll.
 * @param res der berechnete Monomvector
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

    while (t + splineSubPartWidth < T - DELTA)
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
 * Berechnet die Koordinaten fuer eine uebergebene Dimension
 * @param S die Zeit in X Dimenstion [0-1]
 * @param T die Zeit in Z Dimenstion [0-1]
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
 * Initialisiert die Logic beim Start.
 */
void initLogic(void)
{
    // Initialisieren des Logic Vertex Arrays
    initControlPointArray();
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
 * Initialisiert das Vertex Arrays in der Logik.
 */
void initControlPointArray(void)
{
    srand((unsigned)time(NULL));
    g_controlPoints = realloc(g_controlPoints, sizeof(LogicVertex) * g_controlPointAmount);

    int cols = (int)sqrt(g_controlPointAmount);
    int rows = (int)sqrt(g_controlPointAmount);
    int vertexIndex = 0;
    for (int z = 0; z < rows; z++)
    {
        for (int x = 0; x < cols; x++)
        {
            g_controlPoints[vertexIndex][LX] = (-(FIELD_WIDTH / 2) + (x * FIELD_WIDTH / (float)(cols - 1)));
            g_controlPoints[vertexIndex][LY] = getRandomNumber();
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
                        tempVerticeArray[vertexIndex][LY] = getRandomNumber();
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
 * Kuemmert sich um die Berechnungen in der Logik
 * @param interval Verstrichene Zeit in millisekunden.
 */
void handleLogicCalculations(double interval)
{
    calcLight0RotationAngle(interval);
    if (g_cameraFlight)
    {
        calcCameraFlightT(interval);
    }
}

/**
 * Gibt den Speicher des Logic Arrays frei.
 */
void freeArraysLogic(void)
{
    free(g_controlPoints);
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