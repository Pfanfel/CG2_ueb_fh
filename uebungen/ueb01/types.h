#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* Hoehe/Breite des (quadratischen) Levels */
#define LEVELSIZE (12)

/** ---- Farben ---- */
#define BLACK 0.0f, 0.0f, 0.0f
#define RED 1.0f, 0.0f, 0.0f
#define GREEN 0.0f, 1.0f, 0.0f
#define BLUE 0.0f, 0.0f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f
#define GREY 0.46, 0.46f, 0.49f
#define YELLOW 1.0f, 1.0f, 0.0f
#define ORANGE 1.0f, 0.5f, 0.0f
#define PURPLE 1.0f, 0.0f, 0.0f
#define BROWN 0.58f, 0.29f, 0.0f
#define LIGHT_BLUE 0.03f, 0.97f, 1.0f
#define LIGHT_GREY 0.658824f, 0.658824f, 0.658824f

/* ---- Typedeklarationen ---- */

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Vektor im 2D-Raum */
typedef GLfloat CGVector2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3f[3];

/** Punkt im 3D-Raum (homogene Koordinaten) */
typedef GLfloat CGPoint4f[4];

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** RGBA-Farbwert */
typedef GLfloat CGColor4f[4];

/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

/* Datentyp fuer die Sterne */
typedef struct
{
    float x;
    float y;
    GLboolean isVisible;
} Star;

/* Datentyp fuer die Wolke */
typedef struct
{
    float x;
    float y;
    GLboolean isVisible;
} Cloud;


/** Mausereignisse. */
enum e_MouseEventType
{
    mouseButton,
    mouseMotion,
    mousePassiveMotion
};

/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseEventType CGMouseEventType;

/** Datentyp fuer Vertexarray. */
typedef GLfloat Vertex[6];

#endif