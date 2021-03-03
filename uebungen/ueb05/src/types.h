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

/** ---- Farben ---- */
#define BLACK 0.0f, 0.0f, 0.0f
#define RED 1.0f, 0.0f, 0.0f
#define GREEN 0.0f, 1.0f, 0.0f
#define BLUE 0.0f, 0.0f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f
#define GREY 0.46f, 0.46f, 0.49f
#define YELLOW 1.0f, 1.0f, 0.0f
#define ORANGE 1.0f, 0.5f, 0.0f
#define PURPLE 1.0f, 0.0f, 0.0f
#define BROWN 0.58f, 0.29f, 0.0f
#define LIGHT_BLUE 0.03f, 0.97f, 1.0f
#define LIGHT_GREY 0.658824f, 0.658824f, 0.658824f
#define PINK 1.0f, 0.07843137254f, 0.57647058823f

#define X 0
#define Y 1
#define Z 2

#define R 0
#define G 1
#define B 2

/* ---- Typedeklarationen ---- */

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Vektor im 2D-Raum */
typedef GLfloat CGVector2f[2];

/** Punkt im 2D-Raum */
typedef GLint CGPoint2i[2];

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3f[3];

/** Vektor bestehend aus drei int Werten */
typedef GLint CGVector3i[3];

/** Punkt im 3D-Raum (homogene Koordinaten) */
typedef GLfloat CGPoint4f[4];

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** RGBA-Farbwert */
typedef GLfloat CGColor4f[4];

/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

/** Bewegungsrichtungen der Kamera. */
typedef enum
{
    moveUp,
    moveDown,
    moveLeft,
    moveRight,
    moveNone,
} Movement;

/** Bewegungsrichtungen fuer Radius. */
typedef enum
{
    radiusClose,
    radiusFar,
    radiusNone,
} Radius;

typedef enum
{
    boundingBoxNone,
    boundingBoxAABB,
    boundingBoxOBB,
} BoundingBoxType;

typedef enum
{
    objectSphere,
    objectCube,
    objectBunny,
    objectBoundingBoxBunny,
    objectMirror,
    objectWall,
    objectNone,
} SceneObject;

typedef struct
{
    CGVector3f origin;
    CGVector3f direction;
} Ray;

typedef struct MaterialPropertys
{
    float materialAmbient;
    float materialDiffuse;
    float materialSpecular;
    float kReflection;
    float kTransmission;

} MaterialProperties;

typedef struct Sphere
{
    CGVector3f center;
    float radius;
    CGColor3f color;
    MaterialProperties material;

} Sphere;

typedef struct LightSource
{
    CGVector3f center;
    CGColor3f color;
} LightSource;

typedef struct Cuboid
{
    CGVector3f bottomLeftFront;
    CGVector3f bottomRightFront;
    CGVector3f topLeftFront;
    CGVector3f topRightFront;
    CGVector3f topLeftBehind;
    CGVector3f topRightBehind;
    CGVector3f bottomLeftBehind;
    CGVector3f bottomRightBehind;
} Cuboid;

typedef struct ObjObject
{
    int vertexCount;
    int faceCount;
    CGVector3f *vertices;
    CGVector3i *faces;
    CGColor3f color;
    MaterialProperties material;
} ObjObject;

#endif