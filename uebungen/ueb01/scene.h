#ifndef __SCENE_H__
#define __SCENE_H__
/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * 
 *
 * @author Michael Smirnov & Len Harmsen
 */

#include "types.h"

/**
 * Zeichen-Funktion.
 * Stellt die das Spielfeld dar.
 */
void drawScene(void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

/**
 * Initialisiert die Displaylisten und fuellt diese mit Objekten
 */
void initDisplayLists(void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void);

/**
 * Toggelt die Normalen.
 */
void toggleNormals(void);

float calcPlaneRotation(float *n);

void calcPlaneNormal(float *n);

/**
 * Gibt den Speicher der Scene Arrays frei.
 */
void freeArraysScene(void);

void updateVertexArray(GLboolean resetPlane);

void increaseResolution(void);

void decreaseResolution(void);

/**
 * Berechnet das Kreuzprodukt zweiter Vektoren
 * @param a Vektor a
 * @param Vektor b
 * @return das Kreuzprodukt zwischen den beiden Vektoren
 */
float calcDotProduct(float *a, float *b);

/**
 * Berechnet Betrag/Laenge eines Vektors
 * @param a ein Vektor
 * @return der Betrag des Vektors
 */
float calcVectorLength(float *a);

#endif
