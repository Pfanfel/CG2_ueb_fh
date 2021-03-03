#ifndef __IO_H__
#define __IO_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
/**
 * @file
 * Schnittstelle des Ein-/Ausgabe-Moduls.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Michael Smirnov & Len Harmsen
 */

int initAndStartIO(char *title, int width, int height);

int getVisualHelp();

int getSelectedControlPointIdx(void);

GLboolean getHelpStatus();

float getPhi(void);

float getTheta(void);

float getRadius(void);

float getFps();

int getTextureIdx(void);

GLboolean getPauseStatus(void);

GLboolean getShipPickingStatus();

GLuint getPickedObjectId();

GLfloat *getVirtualCameraMatrix(void);

int getInterpolationResolution(void);

GLboolean getDrawBezierStatus();

#endif
