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

void drawLineInBetween(float x1, float y1, float z1, float x2, float y2, float z2);

void drawGrid();

void drawAxes(void);

void drawSphere();

void drawSun(void);

void drawMarble(float x, float y, float z);

void drawHole(float x, float y, float z);

void drawTarget(float x, float y, float z);

void drawBarrier(GLboolean drawNormals);

void drawControlPoints(void);

GLfloat *getSunPosMatrix(void);

void drawWalls(GLboolean drawNormals);

#endif