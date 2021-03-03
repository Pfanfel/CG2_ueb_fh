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

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 */
int initAndStartIO(char *title, int width, int height);

GLboolean getHelpStatus(void);

GLboolean getPauseStatus(void);

GLboolean getConvexHullStatus(void);

#endif
