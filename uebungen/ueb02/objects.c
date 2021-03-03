/**
 * @file
 * Objekte-Modul.
 * Das Modul kapselt vorallem die Objekte welche gezeichnet werden sollen.
 * 
 *
 * @author Michael Smirnov & Len Harmsen
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else

#include <GL/gl.h>
#include <math.h>
#include <GL/glut.h>

#endif

#include <string.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "types.h"
#include "stringOutput.h"
#include "util.h"
#include "io.h"
#include "texture.h"
#include "scene.h"

/* ---- Globale Variablen ---- */

GLfloat g_sunPosMatrix[16] = {0};

/* ---- Funktionsprototypen innerhalb ---- */

/**
 * Hilfsmethode welche eine Linie zwischen 2 Punkten zeichnet
 * @param x1 x Koordinate des ersten Punktes
 * @param y1 y Koordinate des ersten Punktes
 * @param z1 z Koordinate des ersten Punktes
 * @param x2 x Koordinate des zweiten Punktes
 * @param y2 y Koordinate des zweiten Punktes
 * @param z2 z Koordinate des zweiten Punktes
 */
void drawLineInBetween(float x1, float y1, float z1, float x2, float y2, float z2)
{
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

/**
 * Zeichnet ein Gitter Auf der Z Ebene zur Orienteirung
 */
void drawGrid()
{
    setMaterialAndColor(LIGHT_BLUE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    for (int i = -500; i <= 500; i += 1)
    {
        glBegin(GL_LINES);
        glColor3ub(150, 190, 150);
        glVertex3f(-500, 0, i);
        glVertex3f(500, 0, i);
        glVertex3f(i, 0, -500);
        glVertex3f(i, 0, 500);
        glEnd();
    }
}

/**
 * Zeichnet einen Pfeil mit Laenge 1 entlang der positiven z-Achse.
 */
static void drawArrow(void)
{
    glBegin(GL_LINE_STRIP);
    {
        /* Achse */
        glVertex3f(0.0f, 0.0f, -0.5f);
        glVertex3f(0.0f, 0.0f, 0.5f);
        /* erster Pfeil */
        glVertex3f(0.05f, 0.0f, 0.45f);
        glVertex3f(-0.05f, 0.0f, 0.45f);
        glVertex3f(0.0f, 0.0f, 0.5f);
        /* zweiter Pfeil */
        glVertex3f(0.0f, 0.05f, 0.45f);
        glVertex3f(0.0f, -0.05f, 0.45f);
        glVertex3f(0.0f, 0.0f, 0.5f);
    }
    glEnd();
}

/**
 * Zeichnet Koordinatenachsen (inklusive Beschriftung).
 */
void drawAxes(void)
{
    /* Farben der Koordinatenachsen */
    CGColor3f axesTextColor = {1.0f, 1.0f, 0.0f};

    setMaterialAndColor(ORANGE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glPushMatrix();
    {
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 2.0f);

        /* x-Achse */
        drawArrow();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
        glScalef(1.0f, 1.0f, 2.0f);

        /* y-Achse zeichnen */
        drawArrow();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glScalef(1.0f, 1.0f, 2.0f);

        /* z-Achse zeichnen */
        drawArrow();
    }
    glPopMatrix();

    /* Beschriftungen der Koordinatenachsen */

    glColor3fv(axesTextColor);
    glRasterPos3f(1.1f, 0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
    glRasterPos3f(0.0f, 1.1f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
    glRasterPos3f(0.0f, 0.0f, 1.1f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
}

/**
 * Zeichnet eine Kugel mit dem Durchmesser 1 und dem Mittelpunkt (0/0/0).
 */
void drawSphere()
{
    /* Quadric erzuegen */
    GLUquadricObj *qobj = gluNewQuadric();
    if (qobj != 0)
    {
        /* Normalen fuer Quadrics berechnen lassen */
        gluQuadricNormals(qobj, GLU_SMOOTH);

        /* Kugel zeichen */
        gluSphere(qobj, 0.5, 20, 20);

        /* Loeschen des Quadrics nicht vergessen */
        gluDeleteQuadric(qobj);
    }
    else
    {
        //CG_ERROR(("Could not create Quadric\n"));
    }
}

void drawSun(void)
{
    float sunAngle = getLight0Angle();
    glPushMatrix();
    {
        glRotatef(sunAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(SUN_RADIUS, SUN_HEIGHT, 0.0f);
        setMaterialAndColor(YELLOW, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        //Die Koordinaten der Sonne fuer das Licht bestimmen
        glGetFloatv(GL_MODELVIEW_MATRIX, g_sunPosMatrix);
        drawSphere();
    }
    glPopMatrix();
}

/**
 * Liefert die Modellmatrix der Sonne
 */
GLfloat *getSunPosMatrix(void)
{
    return g_sunPosMatrix;
}
