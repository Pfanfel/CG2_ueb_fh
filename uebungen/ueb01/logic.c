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
#include <string.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "debugGL.h"
#include "scene.h"
#include "io.h"

/* ---- Globale Daten ---- */
GLboolean g_isSpline = GL_TRUE;

int g_currentLevel = 0;

int g_hitPointIdx = 0;

int g_currentSplineSubPart = 0;

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
/*Monomvektor*/
float g_monomVector[4] = {0.0f, 0.0f, 0.0f, 0.0f};
/*Monomvektor mit Interpolationsmatrix multipliziert*/
float g_multipliedMonomWithInterpolation[4] = {0.0f, 0.0f, 0.0f, 0.0f};
//Ausgangslevel
/* ------------------------------------------------------------------------------------- Level_1 --------------------------------------- */
CGPoint2f g_initLevel[LEVEL_AMOUNT][6] = {{{-0.8f, 0.0f}, {-0.4f, -0.25f}, {0.4f, -0.25f}, {0.8f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
                                          /* --------------------------------------------- Level_2 --------------------------------------- */
                                          {{-0.8f, 0.0f}, {-0.4f, -0.25f}, {0.0f, -0.30f}, {0.4f, -0.25f}, {0.8f, 0.0f}, {0.0f, 0.0f}},
                                          /* --------------------------------------------- Level_3 --------------------------------------- */
                                          {{-0.8f, 0.0f}, {-0.4f, -0.25f}, {-0.2f, -0.30f}, {0.2f, -0.30f}, {0.4f, -0.25f}, {0.8f, 0.0f}}};
/*Anzahl der Sterne nach Level indiziert*/
int g_starCount[3] = {1, 3, 2};
/*Initialisierungsdaten der Sterne nach Level*/
Star g_initStars[3][3] = {{{0.0f, 0.75f, GL_TRUE}, {0.0f, 0.0f, GL_FALSE}, {0.0f, 0.0f, GL_FALSE}},
                          {{-0.4f, -0.45f, GL_TRUE}, {0.4f, -0.45f, GL_TRUE}, {0.0f, 0.45f, GL_TRUE}},
                          {{-0.4f, 0.5f, GL_TRUE}, {0.4f, -0.5f, GL_TRUE}, {0.0f, 0.0f, GL_FALSE}}};
/*Sterne im aktuellen Level*/
Star g_stars[3] = {{0}};

/*Die Wolke in LvL 3*/
Cloud g_cloud = {0.0, 0.5f, GL_FALSE};

/*Die Kontrollpunkte im aktuellen Level*/
CGPoint2f g_controlPoints[6] = {0};

/* Die Kontrollpunkte fuer die konvexe Huelle */
CGPoint2f *g_convexHull = NULL;

int g_convexHullLength;

/*Die Position des Fiegers in T [0-1] */
float g_planePosition = 0.0f;

/*Fliegergeschwindigkeit*/
float g_planeVelocity = INIT_VELOCITY;

/*Ob der Flieger sich bewegt*/
GLboolean g_planeIsMoving = GL_FALSE;

/*Ob das Spiel gerade lauft*/
GLboolean g_gameInProgress = GL_TRUE;

/*Ob der Fieger ans ende der Kurve geflgen ist*/
GLboolean g_curseFinished = GL_FALSE;

/*Anzahl der eingesammelten Sterne*/
int g_collectedStars = 0;

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
 * Kuemmert sich um die Bewegung des Flugzeugs 
 * @param interval die verstrichene Zeit seit dem letzen Frame
 */
static void movePlane(float interval)
{
    {
        GLfloat n[3] = {0};
        calcPlaneNormal(n);
        float alpha = calcPlaneRotation(n);
        //Fliegt hoch
        if ((alpha >= 95.0f && alpha <= 180.0f) || (alpha >= -180.0f && alpha <= -95.0f))
        {
            if (!(g_planeVelocity - VELOCITY_STEPS < LOWEST_VELOCITY))
            {
                g_planeVelocity -= VELOCITY_STEPS;
            }
        }
        //Fliegt runter
        else if ((alpha <= 85.0f && alpha >= -85.0f))
        {
            if (!(g_planeVelocity + VELOCITY_STEPS > HIGHEST_VELOCITY))
            {
                g_planeVelocity += VELOCITY_STEPS;
            }
        }
        // 85 - 95 und -85 - -95 keine Geschwindigkeitsaenderung

        // Wenn Plane am Ende is curseFinished -> True
        if (g_planePosition + g_planeVelocity < 1.0f - DELTA)
        {
            g_planePosition += g_planeVelocity * interval * PLANE_SPEED;
        }
        else
        {
            g_curseFinished = GL_TRUE;
            g_planeIsMoving = GL_FALSE;
        }
    }
}

/**
 * Kuemmert sich um die Kollision des Fliegers mit den jeweiligen Objekten.
 */
static void handlePlaneCollision(void)
{
    GLfloat n[3] = {0};
    calcPlaneNormal(n);
    float currentX = calculateX(g_planePosition) + n[LX];
    float currentY = calculateY(g_planePosition) + n[LY];
    int starCount = g_starCount[g_currentLevel];
    for (int i = 0; i < starCount; i++)
    {
        Star currentStar = {g_stars[i].x, g_stars[i].y, g_stars[i].isVisible};
        if (currentStar.isVisible)
        {
            float distanceToPlaneX = currentStar.x - currentX;
            float distanceToPlaneY = currentStar.y - currentY;

            float distanceToPlane = sqrt(pow(distanceToPlaneX, 2) + pow(distanceToPlaneY, 2));
            if (distanceToPlane <= (STAR_DIAMETER / 2.0f) + (PLANE_DIAMETER / 2.0f))
            {
                g_stars[i].isVisible = GL_FALSE;
                g_collectedStars++;
            }
        }
    }
    if (g_cloud.isVisible && g_currentLevel == 2)
    {
        float distanceToPlaneX = g_cloud.x - currentX;
        float distanceToPlaneY = g_cloud.y - currentY;
        float distanceToPlane = sqrt(pow(distanceToPlaneX, 2) + pow(distanceToPlaneY, 2));
        if (distanceToPlane <= (CLOUD_DIAMETER / 2.0f) + (PLANE_DIAMETER / 2.0f))
        {
            g_gameInProgress = GL_FALSE;

            setCurrentLevel(g_currentLevel);
        }
    }
}

/**
 * Prueft ob das Level gewonnen wurde.
 */
static GLboolean gameWon(void)
{
    return g_collectedStars == g_starCount[g_currentLevel] && g_curseFinished;
}

/**
 * Prueft ob das Level nicht gewonnen wurde.
 */
static GLboolean gameLost(void)
{
    return g_collectedStars != g_starCount[g_currentLevel] && g_curseFinished;
}

/**
 * Verantwortlich fuer die Berechnungen des Flugzeugs im Flug
 */
void handleLogicCalculations(double interval)
{

    if (g_gameInProgress)
    {
        //wird bei jedem Frame aufgerufen
        if (g_planeIsMoving)
        {
            movePlane(interval);
            handlePlaneCollision();
        }
        if (gameWon())
        {
            setCurrentLevel((g_currentLevel + 1) % 3);
        }
        if (gameLost())
        {
            setCurrentLevel(g_currentLevel);
        }
    }
}

/**
 * Initialisiert die Kontrollpunkte des aktuellen Levels
 */
static void initControlPoints(void)
{
    for (int i = 0; i <= LOWEST_AMOUNT_CONTROL_POINTS + g_currentLevel; i++)
    {
        g_controlPoints[i][LX] = g_initLevel[g_currentLevel][i][LX];
        g_controlPoints[i][LY] = g_initLevel[g_currentLevel][i][LY];
    }
}

/**
 * Initialisiert die Sterne des aktuellen Levels
 */
static void initStars(void)
{
    for (int i = 0; i < g_starCount[g_currentLevel]; i++)
    {
        g_stars[i] = g_initStars[g_currentLevel][i];
    }
}

/**
 * Initialisiert die Sichtbarkeit der Wolke
 */
static void initCloud(void)
{
    if (g_currentLevel != 2)
    {
        g_cloud.isVisible = GL_FALSE;
    }
    else
    {
        g_cloud.isVisible = GL_TRUE;
    }
}

/**
 * Initialisiert die Position des Fliegers
 */
static void initPlanePosition(void)
{
    g_planePosition = 0.0f;
    g_planeIsMoving = GL_FALSE;
    g_planeVelocity = INIT_VELOCITY;
}

/**
 * Akutalisiert die Position des Fliegers
 * @param T die Position des Fliegers im Zeitverlauf [0-1]
 */
void updatePlanePosition(float T)
{
    g_planePosition = T;
}

/**
 * Initialisiert das Spiel beim starten.
 */
void initLogic(void)
{
    setCurrentLevel(0);
}

/**
 * Berechnet den Monomvektor fuer ein gegebenes t
 * @param t klein t fuer den Monomvektor
 */
static void setg_monomVector(float t)
{
    g_monomVector[0] = powf(t, 3.0f);
    g_monomVector[1] = powf(t, 2.0f);
    g_monomVector[2] = t;
    g_monomVector[3] = 1.0f;
}

/**
 * Setzt das Aktuelle Level und initialisiert neu
 * @param numCurrLevel der Index des zu setzenden Levels
 */
void setCurrentLevel(int numCurrLevel)
{
    g_currentLevel = numCurrLevel;
    g_gameInProgress = GL_TRUE;
    g_curseFinished = GL_FALSE;
    g_collectedStars = 0;
    initControlPoints();
    initPlanePosition();
    initStars();
    initCloud();
    updateVertexArray(GL_TRUE);
}

/**
 * Multipliziert den Monomvektor mit der aktuell gesetzten Interpolationsmatrix
 */
static void multiplyTWithM(void)
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        g_multipliedMonomWithInterpolation[i] = 0.0f;
        for (int j = 0; j < 16; j = j + 4)
        {
            if (g_isSpline)
            {
                g_multipliedMonomWithInterpolation[i] += g_monomVector[count % 4] * g_splineInterpolation[j + i];
            }
            else
            {
                g_multipliedMonomWithInterpolation[i] += g_monomVector[count % 4] * g_bezierInterpolation[j + i];
            }
            count++;
        }
    }
}

/**
 * Berechnet die X oder Y Komponente der aktuellen Kurve
 * @param isX ob die X Komponente berechnet werden soll
 * @return den jew. X oder Y Wert.
 */
static float calcInterpolatedValue(GLboolean isX)
{
    int xOrYIndice = 0;
    float ret = 0.0f;
    if (!isX)
    {
        xOrYIndice = 1;
    }
    //i je nachdem in welchem splineSubpart wir gerade berechnen z.B. 0..3 oder 1..4
    for (int i = g_currentSplineSubPart; i < g_currentSplineSubPart + 4; i++)
    {
        //i - g_currentSplineSubPart um immer von 0 bis 3
        ret += g_controlPoints[i][xOrYIndice] * g_multipliedMonomWithInterpolation[i - g_currentSplineSubPart];
    }
    return ret;
}

/**
 * Konvertiert T zu t. Setzt dabei den aktuellen Teilbereich des Splines.
 * @param T Gross T der zu konvertieren ist
 * @return klein t
 */
static float convertTTot(float T)
{
    float splineSubPartWidth = 1.0f / (g_currentLevel + 1.0f);
    int splineSubPartCounter = 0;
    float t = 0.0f;

    while (t + splineSubPartWidth < T - DELTA)
    {
        t += splineSubPartWidth;
        splineSubPartCounter++;
    }
    g_currentSplineSubPart = splineSubPartCounter;
    return (T - t) / splineSubPartWidth;
}

/**
 * Berechnet die X Komponente fuer ein ubergebenes groß T
 * @param T die Zeit [0-1]
 * @return die X Komponente
 */
float calculateX(float T)
{
    float t = convertTTot(T);
    setg_monomVector(t);
    multiplyTWithM();
    return calcInterpolatedValue(GL_TRUE);
}

/**
 * Berechnet die Y Komponente fuer ein ubergebenes groß T
 * @param T die Zeit [0-1]
 * @return die Y Komponente
 */
float calculateY(float T)
{
    float t = convertTTot(T);
    setg_monomVector(t);
    multiplyTWithM();
    return calcInterpolatedValue(GL_FALSE);
}

/**
 * Hilfsfunktion um von Radian zu Grad zu konvertieren
 * @param rad Radian
 * @return Grad
 */
float radToDegree(float rad)
{
    return rad * 180.0f / M_PI;
}

/**
 * Liefert den Index des akutellen Levels
 * @return der Index des Levels
 */
int getg_currentLevel(void)
{
    return g_currentLevel;
}

/**
 * Schaltet zwischen Bezier und Spline um
 */
void toggleSpline(void)
{
    g_isSpline = !g_isSpline;
}

/**
 * Liefert die X Koordinate des Kontrollpunktes an dem uebergebenen Index.
 * @param index der Index des Kontrollpunktes
 * @return die X Koordinate
 */
float getControlpointX(int index)
{
    return g_controlPoints[index][CX];
}

/**
 * Liefert die Y Koordinate des Kontrollpunktes an dem uebergebenen Index.
 * @param index der Index des Kontrollpunktes
 * @return die Y Koordinate
 */
float getControlpointY(int index)
{
    return g_controlPoints[index][CY];
}
/**
 * Liefert die X Position des Fliegers
 * @return die X Position
 */
float getPlaneX(void)
{
    return calculateX(g_planePosition);
}

/**
 * Liefert die Y Position des Fliegers
 * @return die Y Position
 */
float getPlaneY(void)
{
    return calculateY(g_planePosition);
}

/**
 * Liefert das aktuelle groß T vom Flieger
 * @return Groß T vom Flieger
 */
float getPlaneT(void)
{
    return g_planePosition;
}

/**
 * Ueberfrueft ob ein Kontrollpunkt mit der Maus getroffen wurde
 * @param x Positon des Klicks zwischen -1 und 1 (0,0 ist mitte)
 * @param y Positon des Klicks zwischen -1 und 1 (0,0 ist mitte)
 */
void checkCircleHit(float x, float y)
{
    g_hitPointIdx = 0;
    //Nicht beim Ersten
    for (int i = 1; i < (g_currentLevel + LOWEST_AMOUNT_CONTROL_POINTS) && g_hitPointIdx == 0; i++)
    {
        //Nicht beim Letzen
        if (i < g_currentLevel + LOWEST_AMOUNT_CONTROL_POINTS - 1)
        {
            float d = sqrtf(powf(x - g_controlPoints[i][LX], 2) + pow(y - g_controlPoints[i][LY], 2));
            if (d <= CONTROL_POINT_RADIUS + DELTA)
            {
                g_hitPointIdx = i;
            }
        }
    }
}
/**
 * Bewegt ein Kontrollpunkt bei geklickter Maustaste
 * @param hitX X Positon des Klicks zwischen -1 und 1 (0,0 ist mitte)
 * @param hitY Y Positon des Klicks zwischen -1 und 1 (0,0 ist mitte)
 */
void moveObject(float hitX, float hitY)
{
    if (g_hitPointIdx > 0)
    {
        //Verhindert das rausschieben des Kontrollpunktes ausserhalb des Fensters
        hitX = hitX < -1.0f ? -1.0f : hitX > 1.0f ? 1.0f : hitX;
        hitY = hitY < -1.0f ? -1.0f : hitY > 1.0f ? 1.0f : hitY;
        g_controlPoints[g_hitPointIdx][LX] = hitX;
        g_controlPoints[g_hitPointIdx][LY] = hitY;
        updateVertexArray(GL_FALSE);
    }
}

/**
 * Startet den Flieger
 */
void startPlane(void)
{
    g_planeIsMoving = GL_TRUE;
}

/**
 * Liefert die Anzahl der Sterne im aktuellen Level
 * @return die Anzahl der Sterne.
 */
int getStarCount(void)
{
    return g_starCount[g_currentLevel];
}

/**
 * Liefert den Stern an dem uebergebenen Index
 * @param i der Index
 * @return den Stern
 */
Star getStar(int i)
{
    return g_stars[i];
}

/**
 * Liefert die Wolke in Level 3
 * @return Die Wolke
 */
Cloud getCloud(void)
{
    return g_cloud;
}

/**
 * Normalisiert den ubergebenen Vektor auf laege 1
 * @param v Der zu normalisierende Vektor
 */
static void normalizeVector(float *v)
{
    float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
}

/**
 * Berechnet den Winkel von einem Vektor zwischen zwei Punkten zu der x-Achse.
 * @param points Punkte-Array aus dem die richtigen Punkte gelesen werden
 * @param i index s Ziel-Punktes
 * @param lastX x-Position des Startpunktes
 * @param lastY y-Position des Startpunktes
 * @param currentDistance Laenge des vektors
 * @param x x-Achse
 */
float calculateAlpha(CGPoint2f *points, int i, float lastX, float lastY, float *currentDistance, float *x)
{

    /* -----------Werte holen und Winkel berechnen---------------- */
    float currentY = points[i][LY];
    float currentX = points[i][LX];
    //Vektor vom zuletzt berechnetem Punkt (Startpunkt) zu aktuell betrachtetem Punkt
    float v[3] = {currentX - lastX, currentY - lastY, 0.0f};
    *currentDistance = calcVectorLength(v);
    normalizeVector(v);
    float alpha = acosf(calcDotProduct(v, x));
    /* ------------------------------------------------------------ */
    alpha = radToDegree(alpha);
    //Formel, wenn y vom neuen kleiner als y vom vorherigen
    if (currentY < lastY - DELTA)
    {
        alpha = 360.0f - alpha;
    }
    return alpha;
}

/**
 * Berechnet die konvexe Huelle der Kontrollpunkte
 */
void calcConvexHull(void)
{
    int controlPointsAmount = g_currentLevel + LOWEST_AMOUNT_CONTROL_POINTS;
    //Beide Mengen
    CGPoint2f calculated[controlPointsAmount];
    CGPoint2f notCalculated[controlPointsAmount];
    //Werte der Kontrollpunkte kopieren. NotCalculated mit allen Kontrollpunkten
    for (int i = 0; i < controlPointsAmount; i++)
    {
        notCalculated[i][LX] = g_controlPoints[i][LX];
        notCalculated[i][LY] = g_controlPoints[i][LY];
    }
    //Etwas was hoher als 1 ist
    float lowestY = 1.5f;
    float lowestX = 1.5f;
    //Index fuer das Calculated Array -> zeigt immer auf den zuletzt hinzugefuegten Punkt
    int calculatedPoints = 0;
    //Index in cotCalculates des gefundenen Startpunktes berechnen (untester-links)
    int calculatedPointIdx = 0;
    //Alle Punkte durchgehen und den Startpunkt berechnen (unten links)
    for (int i = 0; i < controlPointsAmount; i++)
    {
        float currentY = notCalculated[i][LY];
        float currentX = notCalculated[i][LX];

        if (currentY < lowestY - DELTA)
        {
            lowestY = currentY;
            lowestX = currentX;
            calculatedPointIdx = i;
        }
        //Wenn Y gleich nimm den Punkt mit niedrigeren X Wert
        else if (fabsf(currentY - lowestY) < DELTA && currentX < lowestX - DELTA)
        {
            lowestX = currentX;
            calculatedPointIdx = i;
        }
    }
    //Den Startpunkt in das Calculated Array schreiben
    calculated[calculatedPoints][LX] = notCalculated[calculatedPointIdx][LX];
    calculated[calculatedPoints][LY] = notCalculated[calculatedPointIdx][LY];
    calculatedPoints++;

    //Iteration um den Punkt mit dem kleinsten Winkel zum letzten Startpunkt zu finden
    //Wenn Winkel gleich dann groesster Abstand
    //UND Winkel vom zugefuegten Punkt muss groesser sein als der Winkel vom zuletzt zugefuegten Punkt zu dem davor
    GLboolean run = GL_TRUE;
    //Winkel zwischen des zuletzt berechneten Punkt und x-Achse
    float lastAlpha = 0.0f;
    while (run && (calculatedPoints < controlPointsAmount))
    {
        //Groesser 360
        float lowestAlpha = 361.0f;
        float biggestDistance = -1.0f;
        float lastY = calculated[calculatedPoints - 1][LY];
        float lastX = calculated[calculatedPoints - 1][LX];
        float x[3] = {1.0f, 0.0f, 0.0f};
        //Alle unberechneten Punkte durchgehen und den neuen Startpunkt berechnen
        for (int i = 0; i < controlPointsAmount; i++)
        {
            float currentDistance;
            float alpha = calculateAlpha(notCalculated, i, lastX, lastY, &currentDistance, x);
            if (alpha < lowestAlpha - DELTA && alpha >= lastAlpha)
            {
                lowestAlpha = alpha;
                biggestDistance = currentDistance;
                calculatedPointIdx = i;
            }
            else if (fabs(alpha - lowestAlpha) < DELTA && currentDistance > biggestDistance + DELTA && alpha >= lastAlpha)
            {
                biggestDistance = currentDistance;
                calculatedPointIdx = i;
            }
        }
        float distanceStartAndCurrent;
        float alpha = calculateAlpha(notCalculated, calculatedPointIdx, lastX, lastY, &distanceStartAndCurrent, x);
        lastAlpha = alpha;

        if (fabsf(notCalculated[calculatedPointIdx][LX] - calculated[0][LX]) < DELTA && fabsf(notCalculated[calculatedPointIdx][LY] - calculated[0][LY]) < DELTA)
        {
            //Abbruch wenn alle Punke betrachtet und wieder beim Ausgangspunkt
            run = GL_FALSE;
        }
        //Noch nicht am ende
        else
        {
            //Neuen Startpunkt in das Calculated Array schreiben
            calculated[calculatedPoints][LX] = notCalculated[calculatedPointIdx][LX];
            calculated[calculatedPoints][LY] = notCalculated[calculatedPointIdx][LY];
            calculatedPoints++;
        }
    }

    //Werte in das ConvexHull Array kopieren
    g_convexHull = realloc(g_convexHull, sizeof(CGPoint2f) * calculatedPoints);
    if (g_convexHull != NULL)
    {
        for (int i = 0; i < calculatedPoints; i++)
        {
            g_convexHull[i][LX] = calculated[i][LX];
            g_convexHull[i][LY] = calculated[i][LY];
        }
        g_convexHullLength = calculatedPoints;
    }
    else
    {
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
}

/**
 * Liefert die Anzahl der Punkte aus der die konvexe Huelle besteht
 * @return die Anzahl der Kontrollpunkte
 */
int getConvexHullLength(void)
{
    return g_convexHullLength;
}

/**
 * Liefert die X Koordinate des Punktes der konvexen Huelle an dem uebergebenen Index
 * @param index der Index des gesuchten Kontrollpunktes
 * @return Die X Koordinate des Kontrollpunktes
 */
float getConvexHullPointXByIndex(int index)
{
    return g_convexHull[index][LX];
}

/**
 * Liefert die Y Koordinate des Punktes der konvexen Huelle an dem uebergebenen Index
 * @param index der Index des gesuchten Kontrollpunktes
 * @return Die Y Koordinate des Kontrollpunktes
 */
float getConvexHullPointYByIndex(int index)
{
    return g_convexHull[index][LY];
}

/**
 * Liefert den Status, ob sich der Flieger bewegt
 * @return der Status ob der Flieger sich bewegt
 */
GLboolean isPlaneMoving(void)
{
    return g_planeIsMoving;
}
