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
#include "scene.h"
#include "io.h"
#include "util.h"

/* ---- Globale Daten ---- */


/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
 * Berechnet die Position der Kamera aus Kugelkoordinaten in Karthesische Koord.
 * @param result scheibt das Egebniss
 */
void calculateCameraPosition(CGVector3f result)
{
    float phi = getPhi();
    float theta = getTheta();
    float radius = getRadius();
    /* Winkel der Kamera in Radiant umrechnen */
    //Gegenwinkel von Theta da 90 - theta
    float thetaInRad = degreeToRad(90 - theta);
    float phiInRad = degreeToRad(phi);

    float thirdPersonEyeX = radius * sin(thetaInRad) * cos(phiInRad);
    float thirdPersonEyeY = radius * cos(thetaInRad);
    float thirdPersonEyeZ = radius * sin(thetaInRad) * sin(phiInRad);

    result[0] = thirdPersonEyeX;
    result[1] = thirdPersonEyeY;
    result[2] = thirdPersonEyeZ;
}

/**
 * Berechnet der UP-Vekor ausgehend von der aktuellen Kameraposition, welche sich auf einer halbkugel um den Weltmittelpunkt bewegt
 * @param result der UP-Vektor (out)
 */
void calculateUpVector(CGVector3f result)
{
    //Init zu y - Achse
    result[X] = 0.0f;
    result[Y] = 1.0f;
    result[Z] = 0.0f;
    //Um Z Achse um -Theta grad richtung X Achse kippen
    rotateZ(result, -getTheta());
    //Um Y Achse drehen um 180 - Phi grad
    rotateY(result, 180 - getPhi());
    //Result zeigt jetzt orthogonal zu dem look vector der Kamera.
}
/**
 * Berechnet den Ortsvektor der Projektionsflaeche (unten links/0,0 in Screen Space) ausgehend von der aktuellen Kameraposition
 * @param camera Position der Kamera
 * @param lookAt Blickrichtung
 * @param projectionDistance Abstand zwischen Kamera und Projektionsflaeche
 * @param u Richtungsvektor der Projektionsflaeche in y Richtung (Screen Space/Nach oben)
 * @param v Richtungsvektor der Projektionsflaeche in x Richtung (Screen Space/Nach rechts)
 * @param aspect Seitenverhhaeldnniss des Fensters
 * @param s der Ortsvektor der Projektionsflaeche (out)
 */
void calculateProjectionBaseVector(CGVector3f camera, CGVector3f lookAt, float projectionDistance, CGVector3f u, CGVector3f v, float aspect, CGVector3f s)
{
    //Hoehe der Projektionsebene (in World Space)
    float ch = CW / aspect;

    CGVector3f temp = {0};

    //Vektor von Auge zu Projektionsflaeche
    multiplyVectorWithScalar(lookAt, projectionDistance, temp);
    // C + l * d
    addVectors(camera, temp, s);
    //u * (CW / 2)
    multiplyVectorWithScalar(u, CW / 2.0f, temp);
    //C + l * d - u * (CW / 2)
    subtractVectos(s, temp, s);
    //v * (CH / 2)
    multiplyVectorWithScalar(v, ch / 2.0f, temp);
    //C + l * d - u * (CW / 2) - v * (CH / 2)
    subtractVectos(s, temp, s);
}

