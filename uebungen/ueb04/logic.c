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

/* Position der Kamera beim Kameraflug */
CGVector3f g_thirdPersonCameraPosition = {0};

/* Array fuer die Sphaeren */
CGVector3f g_balls[BALL_COUNT] = {0};

/*Postionen Paar fuer alte und neue Position des gerade zu animierenden Balls*/
CGVector3f g_ballMovementPair[2] = {0};

GLboolean g_interpolationFinished = GL_TRUE;

float g_timeToPass = 0.0f;
float g_passedTimeNewPos = 0.0f;
float g_T = 0.0f;
int g_currentAnimatedBallIdx = 0;

/* Array fuer Partikel */
Particle *g_particles = NULL;
/* Anzahl der Partikel */
GLint g_particleAmount = INITIAL_PARTICLES;

/*Das gerade ausgewahlte Partikel*/
GLint g_pickedParticle = 0;

/*Modus des Targets 0-> Baelle, 1-> Ein Partikel, 2->Zentrum aller Partikel*/
TargetMode g_targetMode = targetModeBalls;

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
 * Liefert eine zufaellige Zahl zwischen -1 und 1 inklusive
 * @return die zufaellige Zahl
 */
static float getRandomNumber()
{
    return (float)rand() / ((float)RAND_MAX / 2.0f) - 1.0f;
}

/**
 * Setzt einen Ball an eine zufaellige Position innerhalb des Wuerfels.
 * @param i der Index des Balls
 */
static void setBallToRandomPos(int i)
{
    //0 ist alte 1 ist neue Position
    g_ballMovementPair[0][LX] = g_balls[i][LX];
    g_ballMovementPair[0][LY] = g_balls[i][LY];
    g_ballMovementPair[0][LZ] = g_balls[i][LZ];
    g_ballMovementPair[1][LX] = getRandomNumber();
    g_ballMovementPair[1][LY] = getRandomNumber();
    g_ballMovementPair[1][LZ] = getRandomNumber();
}

/**
 * Setzt die Baelle an eine zufaellige Position innerhalb des Wuerfels.
 */
static void initBallsPos(void)
{
    for (int i = 0; i < BALL_COUNT; i++)
    {
        g_balls[i][LX] = getRandomNumber();
        g_balls[i][LY] = getRandomNumber();
        g_balls[i][LZ] = getRandomNumber();
    }
}

/**
 * Initialisiert das Partikel Array.
 */
void initParticleArray(void)
{
    g_particles = realloc(g_particles, sizeof(Particle) * g_particleAmount);
    if (g_particles != NULL)
    {
        for (int i = 0; i < INITIAL_PARTICLES; i++)
        {
            g_particles[i].center[LX] = getRandomNumber();
            g_particles[i].center[LY] = getRandomNumber();
            g_particles[i].center[LZ] = getRandomNumber();
            g_particles[i].velocity[LX] = getRandomNumber();
            g_particles[i].velocity[LY] = getRandomNumber();
            g_particles[i].velocity[LZ] = getRandomNumber();
            normalizeVector(g_particles[i].velocity);
            multiplyVectorWithScalar(g_particles[i].velocity, K_V, g_particles[i].velocity);
            g_particles[i].up[LX] = 0.0f;
            g_particles[i].up[LY] = 0.0f;
            g_particles[i].up[LZ] = 0.0f;
            g_particles[i].accelaration[LX] = 0.0f;
            g_particles[i].accelaration[LY] = 0.0f;
            g_particles[i].accelaration[LZ] = 0.0f;
            //Erstellt Wert zwischen 0.5f und 10.5f
            g_particles[i].k_weak = (float)((rand() % 100) / 10.0f) + 0.5f;
        }
    }
    else
    {
        free(g_particles);
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
}

/**
 * Kopiert die Inhalte aus dem Globalen Partikel Array in das Temporaere fuer das veraendern der groesse
 * @param dest temporaeres Partikel Array
 * @param idx Index im temporaeren Array
 */
static void copyIntoTemp(Particle *dest, int idx)
{

    dest[idx].center[LX] = g_particles[idx].center[LX];
    dest[idx].center[LY] = g_particles[idx].center[LY];
    dest[idx].center[LZ] = g_particles[idx].center[LZ];
    dest[idx].velocity[LX] = g_particles[idx].velocity[LX];
    dest[idx].velocity[LY] = g_particles[idx].velocity[LY];
    dest[idx].velocity[LZ] = g_particles[idx].velocity[LZ];
    dest[idx].up[LX] = g_particles[idx].up[LX];
    dest[idx].up[LY] = g_particles[idx].up[LY];
    dest[idx].up[LZ] = g_particles[idx].up[LZ];
    dest[idx].accelaration[LX] = g_particles[idx].accelaration[LX];
    dest[idx].accelaration[LY] = g_particles[idx].accelaration[LY];
    dest[idx].accelaration[LZ] = g_particles[idx].accelaration[LZ];
    dest[idx].k_weak = g_particles[idx].k_weak;
}

/**
 * Veraendert die Groesse des Partikel Arrays.
 * @param inc ob erhoeht oder verringert wird
 */
void resizeParticleArray(GLboolean inc)
{
    Particle *tempParticleArray = malloc(sizeof(Particle) * g_particleAmount);

    if (tempParticleArray != NULL)
    {
        //Alle Punke neu berechnen da diese verschoben
        for (int i = 0; i < g_particleAmount; i++)
        {
            // Increase Vertices
            if (inc)
            {
                //Am Rand neuer Vertex init.
                if (i == g_particleAmount - 1)
                {
                    tempParticleArray[i].center[LX] = getRandomNumber();
                    tempParticleArray[i].center[LY] = getRandomNumber();
                    tempParticleArray[i].center[LZ] = getRandomNumber();
                    tempParticleArray[i].velocity[LX] = getRandomNumber();
                    tempParticleArray[i].velocity[LY] = getRandomNumber();
                    tempParticleArray[i].velocity[LZ] = getRandomNumber();
                    normalizeVector(tempParticleArray[i].velocity);
                    multiplyVectorWithScalar(tempParticleArray[i].velocity, K_V, tempParticleArray[i].velocity);
                    tempParticleArray[i].up[LX] = 0.0f;
                    tempParticleArray[i].up[LY] = 0.0f;
                    tempParticleArray[i].up[LZ] = 0.0f;
                    tempParticleArray[i].accelaration[LX] = 0.0f;
                    tempParticleArray[i].accelaration[LY] = 0.0f;
                    tempParticleArray[i].accelaration[LZ] = 0.0f;
                    //Erstellt Wert zwischen 0.5f und 10.5f
                    tempParticleArray[i].k_weak = (float)((rand() % 100) / 10.0f) + 0.5f;
                }
                //Vorher schon vorhanden
                else
                {
                    copyIntoTemp(tempParticleArray, i);
                }
            }
            // Decrease Vertices
            else
            {
                copyIntoTemp(tempParticleArray, i);
            }
        }
        free(g_particles);
        g_particles = NULL;
        g_particles = tempParticleArray;
    }
    else
    {
        free(tempParticleArray);
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
}

/**
 * Initialisiert die Logic beim Start.
 */
void initLogic(void)
{
    //Initialisierung der "zufaelligen" Zahlen
    srand((unsigned)time(NULL));
    // Initialisieren des Particle Arrays
    initParticleArray();
    initBallsPos();
    g_interpolationFinished = GL_TRUE;
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
 * Berechnet die Beschleunigung eines Partikels
 * @param idx der Index des Partikels
 * @param target die Position des Targets
 * @param singleAcceleration die berechnete Beschleunigung des Partikels (out-param)
 */
void calculateSingleAccelatation(int idx, float *target, float *singleAcceleration)
{
    //Beschleunigung berechnen
    CGVector3f distanceParticleToTarget = {0};
    CGVector3f particleToTargetLengthNormalized = {0};
    // (t - s)
    subtractVectos(target, g_particles[idx].center, distanceParticleToTarget);
    // ||t - s||
    float particleToTargetLength = calcVectorLength(distanceParticleToTarget);
    // (t - s) / ||t - s||
    divideVectorWithScalar(distanceParticleToTarget, particleToTargetLength, particleToTargetLengthNormalized);
    // ... * K_WEAK
    multiplyVectorWithScalar(particleToTargetLengthNormalized, g_particles[idx].k_weak, singleAcceleration);
}

/**
 * Berechnet die Gewichtung fuer ein uebergebenes Target
 * @param idx der Index der Partikels
 * @param target die Position des Targets
 * @return Gewichuntg, welche mit der Beschleunigung multipliziert wird \n
 * hoeher je naeher Partikel an Target
 */
float calculateWeighting(int idx, float *target)
{
    CGVector3f distanceParticleToTarget = {0};
    // (t - s)
    subtractVectos(target, g_particles[idx].center, distanceParticleToTarget);
    // ||t - s||
    float particleToTargetLength = calcVectorLength(distanceParticleToTarget);
    // ||t - s||^2
    float TminusS_2 = pow(particleToTargetLength, 2.0f);
    // ||t - s||^2 / const
    float exp = TminusS_2 / GAUSS_CONST;
    // e^(-||t - s||^2)
    return expf((-exp));
}

/**
 * Berechnet die gewichtete Beschleunigung zu allen Baellen hin
 * @param idx der Index der Partikels
 * @param acceleration Beschleunigung
 */
static void calculateAccelerationToBalls(int idx, float *acceleration)
{
    //Ueber alle Baelle
    for (int i = 0; i < BALL_COUNT; i++)
    {
        CGVector3f singleAcceleration = {0.0f, 0.0f, 0.0f};
        // ai
        calculateSingleAccelatation(idx, g_balls[i], singleAcceleration);
        // gi
        float weighting = calculateWeighting(idx, g_balls[i]);
        // gi * ai
        multiplyVectorWithScalar(singleAcceleration, weighting, singleAcceleration);
        // Summe ueber alle Beschleunigungen mit Gewichtung
        addVectors(acceleration, singleAcceleration, acceleration);
    }
}

/**
 * Berechnet das arethmetische Mittel von allen Partikeln
 * @param mean das arethmetische Mittel in 3-D
 */
static void calculateMean(float *mean)
{
    for (int i = 0; i < g_particleAmount; i++)
    {
        mean[LX] += g_particles[i].center[LX];
        mean[LY] += g_particles[i].center[LY];
        mean[LZ] += g_particles[i].center[LZ];
    }
    for (int i = 0; i < 3; i++)
    {
        mean[i] /= g_particleAmount;
        mean[i] = clip(mean[i], -1.0f, 1.0f);
    }
}

/**
 * Berechnet die gewichtete Beschleunigung zu allen Baellen hin
 * @param idx der Index der Partikels
 * @param acceleration Beschleunigung
 */
static void calculateAccelarationToCenterOfParticles(int idx, float *acceleration)
{

    CGVector3f mean = {0.0f, 0.0f, 0.0f};
    calculateMean(mean);
    calculateSingleAccelatation(idx, mean, acceleration);
}

/**
 * Berechnet die Beschleunigung der Partikel.
 * @param idx Index des Partikels
 * @param acceleration Beschleunigung
 */
static void calculateAcceleration(int idx, float *acceleration)
{
    switch (g_targetMode)
    {
    case targetModeBalls:
        calculateAccelerationToBalls(idx, acceleration);
        break;
    case targetModeSelectedParticle:
        if (idx == g_pickedParticle) //Das ausgewahlte bewegt sich weiterhin zu den Baellen nur schneller
        {
            calculateAccelerationToBalls(idx, acceleration);
        }
        else //Die anderen bewegen sich auf das ausgewahlte zu
        {
            calculateSingleAccelatation(idx, g_particles[g_pickedParticle].center, acceleration);
        }
        break;
    case targetModeCenterOfParticles:
        calculateAccelarationToCenterOfParticles(idx, acceleration); //Zum median
        break;
    }
}

/**
 * Verschiebt die Partikel mittels Euler Integration.
 * @param interval die verstrichen Zeit
 * @param i Index des Partikels
 */
static void moveParticle(double interval, int i)
{
    CGVector3f acceleration = {0.0f, 0.0f, 0.0f};
    //Vektoren zur Berechnung erstellen
    CGVector3f oldPosition = {g_particles[i].center[LX], g_particles[i].center[LY], g_particles[i].center[LZ]};
    CGVector3f newPosition = {0};
    CGVector3f oldVelocity = {g_particles[i].velocity[LX], g_particles[i].velocity[LY], g_particles[i].velocity[LZ]};

    CGVector3f accelarationMultipliedWithInterval = {0};
    CGVector3f velocityMultipliedWithInterval = {0};

    calculateAcceleration(i, acceleration);
    // delta(t) * a
    multiplyVectorWithScalar(acceleration, interval, accelarationMultipliedWithInterval);
    // v = v + delta(t) * a
    addVectors(oldVelocity, accelarationMultipliedWithInterval, oldVelocity);
    //eulerV/||eulerV|| -> Normieren des Richtungsvektors
    float velocityLength = calcVectorLength(oldVelocity);
    divideVectorWithScalar(oldVelocity, velocityLength, oldVelocity);
    //eulerV(normiert) * kv
    if (g_targetMode == targetModeSelectedParticle && i == g_pickedParticle) //Wenn es sich um das Ausgewahlte handelt -> schneller
    {
        multiplyVectorWithScalar(oldVelocity, K_V * PICKED_PARTICLE_SPEED_FAKTOR, oldVelocity);
    }
    else
    {
        multiplyVectorWithScalar(oldVelocity, K_V, oldVelocity);
    }
    // delta(t) * v
    multiplyVectorWithScalar(oldVelocity, interval, velocityMultipliedWithInterval);
    // s = s + delta(t) * v
    addVectors(oldPosition, velocityMultipliedWithInterval, newPosition);

    g_particles[i].center[LX] = newPosition[LX];
    g_particles[i].center[LY] = newPosition[LY];
    g_particles[i].center[LZ] = newPosition[LZ];
    //oldVelocity enthelt nun die neue Geschwindikeit
    g_particles[i].velocity[LX] = oldVelocity[LX];
    g_particles[i].velocity[LY] = oldVelocity[LY];
    g_particles[i].velocity[LZ] = oldVelocity[LZ];
    CGVector3f up = {0};
    CGVector3f velocityCrossAcceleration = {0};

    calcCrossProduct(oldVelocity, acceleration, velocityCrossAcceleration);
    calcCrossProduct(velocityCrossAcceleration, oldVelocity, up);

    g_particles[i].up[LX] = up[LX];
    g_particles[i].up[LY] = up[LY];
    g_particles[i].up[LZ] = up[LZ];

    g_particles[i].accelaration[LX] = acceleration[LX];
    g_particles[i].accelaration[LY] = acceleration[LY];
    g_particles[i].accelaration[LZ] = acceleration[LZ];
}

/**
 * Kuemmert sich um die Animation/Bewegung der Partikel
 * @param interval das verstrichene Intervall seid dem letzten Zeichen
 */
static void handleParticleMovement(double interval)
{

    for (int i = 0; i < g_particleAmount; i++)
    {
        moveParticle(interval, i);
    }
}

/**
 * Interpoliert die Position des uebergebenen Balls linear
 * @param intervall das verstrichene Intervall seid dem letzten Zeichen-
 * @param idx der Index des Balls
 */
void interpolateBall(double interval, int idx)
{
    CGVector3f movementVector = {g_ballMovementPair[1][LX] - g_ballMovementPair[0][LX],
                                 g_ballMovementPair[1][LY] - g_ballMovementPair[0][LY],
                                 g_ballMovementPair[1][LZ] - g_ballMovementPair[0][LZ]};
    g_balls[idx][LX] = g_ballMovementPair[0][LX] + movementVector[LX] * g_T;
    g_balls[idx][LY] = g_ballMovementPair[0][LY] + movementVector[LY] * g_T;
    g_balls[idx][LZ] = g_ballMovementPair[0][LZ] + movementVector[LZ] * g_T;
    //Interpolation zu Ende
    if (g_T + interval * ANIMATION_FACTOR > 1.0f - DELTA)
    {
        g_interpolationFinished = GL_TRUE;
        g_T = 0.0f;
    }
    //Unter 1
    else
    {
        g_T += interval * ANIMATION_FACTOR;
    }
}

/**
 * Berechnet in einem zufaelligen Zeitintervall eine neue zufaellige
 * Position des Ziel-Balls
 * @param interval das verstrichene Intervall seid dem letzten Zeichen.
 * @param i der Index des Balles
 */
static void calcNewRandomBallPosition(double interval)
{
    if (g_timeToPass > g_passedTimeNewPos + DELTA)
    {
        g_passedTimeNewPos += interval;
    }
    else if (g_interpolationFinished) // Intervall ist abgelaufen und die Animation ist zuende
    {
        /*Neue zufaellige Pos. und Intervall berechen*/

        //Entweder 0 oder 1
        g_currentAnimatedBallIdx = fabsf(getRandomNumber() + 1);
        setBallToRandomPos(g_currentAnimatedBallIdx);
        g_passedTimeNewPos = 0.0f;
        //Zwischen 1 und 9
        g_timeToPass = (getRandomNumber() + 2.0f) * (getRandomNumber() + 2.0f);
        g_interpolationFinished = GL_FALSE;
    }
    else //Intervall ist abgelaufen aber die Animation noch nicht
    {
        interpolateBall(interval, g_currentAnimatedBallIdx);
    }
}

/**
 * Kuemmert sich um die Animation/Bewegung der Zielkugeln
 * @param interval das verstrichene Intervall seid dem letzten Zeichen
 */
static void handleBallMovement(double interval)
{
    calcNewRandomBallPosition(interval);
}

/**
 * Kuemmert sich um die Berechnungen in der Logik
 * @param interval Verstrichene Zeit in millisekunden.
 */
void handleLogicCalculations(double interval)
{
    //Euler integration sollte genauer sein, da kleinere Intervalle als FPS
    while (interval >= UPDATE_CALL)
    {
        handleParticleMovement(UPDATE_CALL);
        if (getBallMovementStatus())
        {
            handleBallMovement(UPDATE_CALL);
        }
        interval -= UPDATE_CALL;
    }
}

/**
 * Gibt den Speicher der dynamisch allozierten Array in der logic frei  
 */
void freeArraysLogic(void)
{
    free(g_particles);
}

/**
 * Erhoeht die Anzahl an Partikel.
 */
void increaseParticles(void)
{
    if (g_particleAmount + 1 <= MAX_PARTICLES)
    {
        g_particleAmount++;
        resizeParticleArray(GL_TRUE);
    }
}

/**
 * Verringert die Anzahl an Partikel.
 */
void decreaseParticles(void)
{
    if (g_particleAmount - 1 >= MIN_PARTICLES)
    {
        g_particleAmount--;
        resizeParticleArray(GL_FALSE);
        if (g_pickedParticle >= g_particleAmount)
        {
            g_pickedParticle = g_particleAmount - 1;
        }
    }
}

/**
 * Liefert die Anzahl an Partikeln.
 * @return Partikelanzahl
 */
int getParticleAmount(void)
{
    return g_particleAmount;
}

/**
 * Liefert die x-Koordinate des Partikels i.
 * @param i Index des Partikels
 * @return x-Koordinate des Partikels
 */
float getParticleX(int i)
{
    return g_particles[i].center[LX];
}
/**
 * Liefert die y-Koordinate des Partikels i.
 * @param i Index des Partikels
 * @return y-Koordinate des Partikels
 */
float getParticleY(int i)
{
    return g_particles[i].center[LY];
}

/**
 * Liefert die z-Koordinate des Partikels i.
 * @param i Index des Partikels
 * @return z-Koordinate des Partikels
 */
float getParticleZ(int i)
{
    return g_particles[i].center[LZ];
}

/**
 * Liefert die x-Koordinate der Geschwindigkeit.
 * @param i Index des Partikels
 * @return x-Koordinate der Geschwindigkeit
 */
float getParticleVelocityX(int i)
{
    return g_particles[i].velocity[LX];
}
/**
 * Liefert die y-Koordinate der Geschwindigkeit.
 * @param i Index des Partikels
 * @return y-Koordinate der Geschwindigkeit
 */
float getParticleVelocityY(int i)
{
    return g_particles[i].velocity[LY];
}

/**
 * Liefert die z-Koordinate der Geschwindigkeit.
 * @param i Index des Partikels
 * @return z-Koordinate der Geschwindigkeit
 */
float getParticleVelocityZ(int i)
{
    return g_particles[i].velocity[LZ];
}
/**
 * Liefert die x-Koordinate der Beschleunigung.
 * @param i Index des Partikels
 * @return x-Koordinate der Beschleunigung
 */
float getParticleAccelarationX(int i)
{
    return g_particles[i].accelaration[LX];
}
/**
 * Liefert die y-Koordinate der Beschleunigung.
 * @param i Index des Partikels
 * @return y-Koordinate der Beschleunigung
 */
float getParticleAccelarationY(int i)
{
    return g_particles[i].accelaration[LY];
}

/**
 * Liefert die z-Koordinate der Beschleunigung.
 * @param i Index des Partikels
 * @return z-Koordinate der Beschleunigung
 */
float getParticleAccelarationZ(int i)
{
    return g_particles[i].accelaration[LZ];
}
/**
 * Liefert die x-Koordinate des UP-Vektors.
 * @param i Index des Partikels
 * @return x-Koordinate des UP-Vektors
 */
float getParticleUpX(int i)
{
    return g_particles[i].up[LX];
}
/**
 * Liefert die y-Koordinate des UP-Vektors.
 * @param i Index des Partikels
 * @return y-Koordinate des UP-Vektors
 */
float getParticleUpY(int i)
{
    return g_particles[i].up[LY];
}

/**
 * Liefert die z-Koordinate des UP-Vektors.
 * @param i Index des Partikels
 * @return z-Koordinate des UP-Vektors
 */
float getParticleUpZ(int i)
{
    return g_particles[i].up[LZ];
}
/**
 * Liefert die x-Koordinate des Balls i.
 * @param i Index des Balls
 * @return x-Koordinate des Balls
 */
float getBallX(int i)
{
    return g_balls[i][LX];
}
/**
 * Liefert die y-Koordinate des Balls i.
 * @param i Index des Balls
 * @return y-Koordinate des Balls
 */
float getBallY(int i)
{
    return g_balls[i][LY];
}

/**
 * Liefert die z-Koordinate des Balls i.
 * @param i Index des Balls
 * @return z-Koordinate des Balls
 */
float getBallZ(int i)
{
    return g_balls[i][LZ];
}

/**
 * Liefert das ausgewaehlte Partikel.
 */
int getPickedParticle(void)
{
    return g_pickedParticle;
}

/**
 * Erhoeht den Index des gepickten Partikels.
 */
void increasePickedParticle(void)
{
    g_pickedParticle = (g_pickedParticle + 1) % g_particleAmount;
}

/**
 * Aendert das Zielobjekt zyklisch.
 */
void changeTargetMode(void)
{
    g_targetMode = (g_targetMode + 1) % 3;
}