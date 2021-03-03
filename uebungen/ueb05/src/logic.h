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

/*Kamera*/
#define RADIUS_SCROLL_STEPS 0.03
#define CAMERA_ROTATION_SPEED 1.0f

//Initiale Kameraposition beim Start des Programms
#define INIT_PHI 90
#define INIT_THETA 0
#define INIT_CAMERA_RADIUS 3

#define INIT_WINDOW_WIDTH 300
#define INIT_WINDOW_HEIGHT 300

//Fuer die Kamerabewegungen
#define UPPER_BOUND_PHI 360
#define LOWER_BOUND_PHI 0

#define UPPER_BOUND_THETA 90
#define LOWER_BOUND_THETA -90

#define SKIP_PIXEL_COUNT 7

//Fuer Spekularen-Lichtanteil nach Phong
#define SHININESS 35

#define AMBIENT_LIGHT_INTENSITY 0.4f

#define MAX_RECURSION_DEPTH 5

//Faktoren des Polynoms 2 Grades fuer die Abschwächungsfunktion des Lichts
#define CONSTANT_ATTENUATION 1.0f
#define LINEAR_ATTENUATION 0.1f
#define QUADRIC_ATTENUATION 0.01f

//Breite der Projektionsebene (in World Space)
#define CW 1.0f
#define FOV 90.0f

//Float unganauigkeiten
#define DELTA 0.0001f

void calculateUpVector(CGVector3f result);

void calculateCameraPosition(CGVector3f result);

void calculateProjectionBaseVector(CGVector3f camera, CGVector3f lookAt, float projectionDistance, CGVector3f u, CGVector3f v, float aspect, CGVector3f s);