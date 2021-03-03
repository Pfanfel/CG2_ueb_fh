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
#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/* ---- Konstanten ---- */

/* Konstanten und Typen zur besseren Lesbarkeit der Vertex Arrays */
#define CX (0)
#define CY (1)
#define CZ (2)
#define NX (3)
#define NY (4)
#define NZ (5)

/*Konstanten zur besseren Lesbarkeit des Arrays in der Logik*/
#define LX (0)
#define LY (1)

//Anzahl an Lvl
#define LEVEL_AMOUNT 3

#define DELTA 0.0001f

#define DEFAULT_RESOLUTION 0.01f

#define CONTROL_POINT_RADIUS 0.035f
#define STAR_DIAMETER 0.05f
#define CLOUD_DIAMETER 0.05f
#define PLANE_DIAMETER 0.1f

#define LOWEST_AMOUNT_CONTROL_POINTS 4

#define INIT_VELOCITY 0.002f
#define LOWEST_VELOCITY 0.001f
#define HIGHEST_VELOCITY 0.01f
#define VELOCITY_STEPS 0.00003f

#define PLANE_SPEED 100

#define PLANE_NORMAL_OFFSET 0.01f

int getg_currentLevel(void);

float getControlpointX(int index);

float getControlpointY(int index);

float calculateX(float T);

float calculateY(float T);

void toggleSpline(void);

float radToDegree(float rad);

void checkCircleHit(float x, float y);

void moveObject(float hitX, float hitY);

void setCurrentLevel(int numCurrLevel);

void initLogic(void);

float getPlaneX(void);

float getPlaneY(void);

float getPlaneT(void);

void updatePlanePosition(float T);

void startPlane(void);

void handleLogicCalculations(double interval);

int getStarCount(void);

void calcConvexHull(void);

int getConvexHullLength(void);

float getConvexHullPointXByIndex(int index);

float getConvexHullPointYByIndex(int index);

Star getStar(int i);

Cloud getCloud(void);

GLboolean isPlaneMoving(void);