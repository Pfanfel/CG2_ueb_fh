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

void drawWalls(GLboolean drawNormals);

void drawPlane(float *position, float *velocity, float *rightVector, GLboolean isTarget);

void drawShadow(float *position, float *velocity, float *rightVector);

void drawPlaneAsLine(float *position, float *velocity, GLboolean isTarget);

#endif