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
#define CR (3)
#define CG (4)
#define CB (5)
#define NX (6)
#define NY (7)
#define NZ (8)
#define TX (9)
#define TY (10)

/*Konstaten zur besseren lesbarkeit des Arrays in der Logik zur bessereren lesbarkeit*/
#define LX (0)
#define LY (1)
#define LZ (2)

#define LT (0)
#define LS (1)

#define SLOPE_FACTOR 0.05f

#define UPDATE_CALL 0.005f

/*Kamera*/
#define RADIUS_SCROLL_STEPS 0.25f
#define CAMERA_ROTATION_SPEED 1.5f

#define INIT_PHI 90
#define INIT_THETA 45
#define INIT_CAMERA_RADIUS 10

#define UPPER_BOUND_PHI 360
#define LOWER_BOUND_PHI 0

#define UPPER_BOUND_THETA 90
#define LOWER_BOUND_THETA 0

#define HEIGHT_CHANGE 0.1f

#define INITIAL_SURFACES_IN_MESH 10

#define MAX_CONTROLPOINTS_IN_MESH 99
#define MIN_CONTROLPOINTS_IN_MESH 4

#define VERTICES_PER_SQUARE 6

#define FIELD_WIDTH 4.0f
#define FIELD_HEIGHT 4.0f

#define LIGHT_ROTATION_STEP 50.0f

#define SUN_HEIGHT 5.0f
#define SUN_RADIUS 10.0f

#define CONTROL_POINTS_PER_SPLINE_SUBPART 4

#define INIT_INTERPOLATION_RESOLUTION 40
#define MIN_INTERPOLATION_RESOLUTION 2
#define MAX_INTERPOLATION_RESOLUTION 500

#define BEZIER_CURVE_RESOLUTION 200

#define DELTA 0.0001f

#define UPPER_BOUND_GREY 0.7f
#define UPPER_BOUND_GREEN 0.5f
#define UPPER_BOUND_BLUE 0.3f

#define DIMENSIONS 3

#define CAMERA_MOVEMENT_STEP 0.25f
#define CAMERA_DISTANCE_BEZIER 0.2f

#define BARRIER_STEP 0.01f
#define BARRIER_COUNT 6
#define BARRIER_WIDTH 0.1f
#define BARRIER_HEIGHT 0.2f
#define ROTATED_BARRIER_COUNT 4

#define TARGET_RADIUS 0.1f

#define HOLE_RADIUS 0.1f
#define INIT_HOLE_AMOUNT 4
#define MAX_HOLES 10
#define MIN_HOLES 1
#define ATTRACTION_DISTANCE HOLE_RADIUS * 3.0f
#define ATTRACTION_FACTOR 150

#define MARBLE_RADIUS 0.025f

#define GRAVITY -10.0f
#define SPRING_CONSTANT 2500
#define FRICTION 0.997f

/* Murmelkonstanten */
#define MARBLE_COUNT 10

#define POKE_FACTOR 500

void changeControlPointHeight(GLuint vertexIndex, GLboolean vertexHeightChange);

float calcGradient(float S, float T, int dimension, GLboolean derivativeS);

float interpolate(float S, float T, int dimension);

void initControlPointArray(void);

void handleLogicCalculations(double interval);

void setLightingStatus(GLboolean status);

void setLight0Status(GLboolean status);

GLboolean getLightingStatus(void);

GLboolean getLight0Status(void);

CGVector3f *calculateThridPersonCameraPosition(void);

float getLight0Angle(void);

void freeArraysLogic(void);

void initLogic(void);

int getControlPointAmount(void);

float getControlPointX(int idx);

float getControlPointY(int idx);

float getControlPointZ(int idx);

float getBezierControlPointX(int idx);

float getBezierControlPointY(int idx);

float getBezierControlPointZ(int idx);

void increaseVertices(void);

void decreaseVertices(void);

void increaseHoles(void);

void decreaseHoles(void);

void setg_bezierControlPoint(int idx, float x, float y, float z);

float getBezier(float T, int dimension);

float getDirection(float T, int dimension);

void setCameraFlightStatus(GLboolean status);

GLboolean getCameraFlightStatus(void);

float getCameraT(void);

float getBarrierT(int i);

float getBarrierS(int i);

float getHoleAmount(void);

float getHoleT(int i);

float getHoleS(int i);

int getSelectedBarrier(void);

float getTargetT(void);

void setSelectedBarrierIndex(int i);

void moveBarrierRight(void);

void moveBarrierLeft(void);

void moveBarrierUp(void);

void moveBarrierDown(void);

float getMarbleS(int i);

float getMarbleT(int i);

void startMarbles(void);

void resetGame(void);

void pokeMarble(void);

GLboolean isMarbleVisible(int i);

GLboolean getGameWonStatus(void);