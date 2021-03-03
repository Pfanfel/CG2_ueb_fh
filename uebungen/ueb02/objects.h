#ifndef __OBJECTS_H__
#define __OBJECTS_H__
/**
 * @file
 * Objekte-Modul.
 * Das Modul kapselt vorallem die Objekte welche gezeichnet werden sollen.
 *
 * 
 *
 * @author Michael Smirnov & Len Harmsen
 */

/**
 * Hilfsmethode welche eine Linie zwischen 2 Punkten zeichnet
 * @param x1 x Koordinate des ersten Punktes
 * @param y1 y Koordinate des ersten Punktes
 * @param z1 z Koordinate des ersten Punktes
 * @param x2 x Koordinate des zweiten Punktes
 * @param y2 y Koordinate des zweiten Punktes
 * @param z2 z Koordinate des zweiten Punktes
 */
void drawLineInBetween(float x1, float y1, float z1, float x2, float y2, float z2);

/**
 * Zeichnet ein Gitter Auf der Z Ebene zur Orienteirung
 */
void drawGrid();

/**
 * Zeichnet Koordinatenachsen (inklusive Beschriftung).
 */
void drawAxes(void);

/**
 * Zeichnet eine Kugel mit dem Durchmesser 1 und dem Mittelpunkt (0/0/0).
 * @param color Hoehe der Sphere (verantwortlich fuer die Farbe)
 */
void drawSphere();

void drawSun(void);

void drawControlPoints(void);

/**
 * Liefert die Modellmatrix der Sonne
 */
GLfloat *getSunPosMatrix(void);

#endif