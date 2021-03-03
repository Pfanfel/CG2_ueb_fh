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
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void);

/**
 * Toggelt die Normalen.
 */
void toggleNormals(void);

/**
 * Toggelt die Kontrollpunkte.
 */
void toggleControlPoints(void);


void calcVertexNormal(float S, float T, float *normal);

#endif
