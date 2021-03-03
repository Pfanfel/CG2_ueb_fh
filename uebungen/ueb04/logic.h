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

#define UPDATE_CALL 0.005f

/*Kamera*/
#define RADIUS_SCROLL_STEPS 0.03
#define CAMERA_ROTATION_SPEED 1.0f

#define INIT_PHI 90
#define INIT_THETA 45
#define INIT_CAMERA_RADIUS 5

#define UPPER_BOUND_PHI 360
#define LOWER_BOUND_PHI 0

#define UPPER_BOUND_THETA 90
#define LOWER_BOUND_THETA -90

#define INITIAL_PARTICLES 20
#define MAX_PARTICLES 1000
#define MIN_PARTICLES 1
#define GAUSS_CONST 50
#define K_V 0.5f

#define WING_WIDTH 4.0

#define PICKED_PARTICLE_SPEED_FAKTOR 1.2f

#define DELTA 0.0001f

#define BALL_COUNT 2

#define ANIMATION_FACTOR 0.5f

#define VECTOR_LENGTH_FACTOR 0.05f

#define CAMERA_DISTANCE_TO_PARTICLE 2.0f
#define CAMERA_DISTANCE_BEHIND_PARTICLE 4.0f

#define SHADOW_DISTANCE_TO_GROUND -0.999f

void handleLogicCalculations(double interval);

void setLightingStatus(GLboolean status);

void setLight0Status(GLboolean status);

GLboolean getLightingStatus(void);

GLboolean getLight0Status(void);

CGVector3f *calculateThridPersonCameraPosition(void);

void freeArraysLogic(void);

void initLogic(void);

int getParticleAmount(void);

float getParticleX(int i);

float getParticleY(int i);

float getParticleZ(int i);

float getParticleVelocityX(int i);

float getParticleVelocityY(int i);

float getParticleVelocityZ(int i);

float getParticleAccelarationX(int i);

float getParticleAccelarationY(int i);

float getParticleAccelarationZ(int i);

float getParticleUpX(int i);

float getParticleUpY(int i);

float getParticleUpZ(int i);

float getBallX(int i);

float getBallY(int i);

float getBallZ(int i);

void decreaseParticles(void);

void increaseParticles(void);

int getPickedParticle(void);

void increasePickedParticle(void);

void changeTargetMode(void);
