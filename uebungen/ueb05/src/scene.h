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
  * Zeichnet die Scene.
  */
void drawScene(GLint width, GLint height);

void setLight0Status(GLboolean status);

GLboolean getLight0Status(void);

void setLight1Status(GLboolean status);

GLboolean getLight1Status(void);

int initScene(GLint width, GLint height);

void freeFrameBuffer(void);

void setG_rendered(GLboolean val);

void toggleG_numLightsStartIndex(void);

void toggleG_Vignette(void);

#endif