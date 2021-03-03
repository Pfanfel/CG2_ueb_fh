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
#include "debugGL.h"
#include "stringOutput.h"
#include "debugGL.h"
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
        CG_ERROR(("Could not create Quadric\n"));
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
 * Zeichnet eine schwarze Sphaere an der uebergebenen Position
 * @param x die X Pos.
 * @param y die Y Pos.
 * @param z die Z Pos.
 */
void drawHole(float x, float y, float z)
{
    glPushMatrix();
    {
        glTranslatef(x, y, z);
        glScalef(HOLE_RADIUS * 2, HOLE_RADIUS * 2, HOLE_RADIUS * 2);
        setMaterialAndColor(BLACK, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawSphere();
    }
    glPopMatrix();
}

/**
 * Zeichnet eine pinke Murmel an der uebergebenen Position
 * @param x die X Pos.
 * @param y die Y Pos.
 * @param z die Z Pos.
 */
void drawMarble(float x, float y, float z)
{
    glPushMatrix();
    {
        glTranslatef(x, y, z);
        glScalef(MARBLE_RADIUS * 2.0f, MARBLE_RADIUS * 2.0f, MARBLE_RADIUS * 2.0f);
        setMaterialAndColor(ORANGE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawSphere();
    }
    glPopMatrix();
}

/**
 * Zeichnet eine gruene Sphaere an der uebergebenen Position
 * @param x die X Pos.
 * @param y die Y Pos.
 * @param z die Z Pos.
 */
void drawTarget(float x, float y, float z)
{
    glPushMatrix();
    {
        glTranslatef(x, y, z);
        glScalef(TARGET_RADIUS * 2.0f, TARGET_RADIUS * 2.0f, TARGET_RADIUS * 2.0f);
        glColor4f(GREEN, 0.5f);
        drawSphere();
    }
    glPopMatrix();
}

/**
 * Zeichnet ein Einheitsrechteck
 * @param drawNormals ob normalen gezeichent werden muessen.
 */
static void drawRect(GLboolean drawNormals)
{
    glRecti(-1, -1, 1, 1);
    if (drawNormals)
    {
        drawLineInBetween(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }
}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 * @param drawNormals ob normalen gezeichent werden muessen.
 */
static void drawCube(GLboolean drawNormals)
{
    /* Vorderseite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();

    /* rechte Seitenfläche */
    glPushMatrix();
    {
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();

    /* hintere Seitenfläche */
    glPushMatrix();
    {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();

    /* linke Seitenflaeche */
    glPushMatrix();
    {
        glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();

    /* Bodenseite */
    glPushMatrix();
    {
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();

    /* Oberseite */
    glPushMatrix();
    {
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();
}

/**
 * Zeichnet eine Barriere in Form eines laenglichen Quaders
 * In x-Richtung skaliert
 * @param drawNormals ob normalen gezeichent werden muessen.
 */
void drawBarrier(GLboolean drawNormals)
{
    glPushMatrix();
    {
        glScalef(BARRIER_WIDTH, BARRIER_HEIGHT, BARRIER_HEIGHT);
        drawCube(drawNormals);
    }
    glPopMatrix();
}

/**
 * Zeichnet eine Wand
 * @param drawNormals ob normalen gezeichnet werden muessen.
 */
static void drawWall(GLboolean drawNormals)
{
    float widthAndHeight = getInterpolatedMeshWidth();
    //Transparenz
    glColor4f(GREY, 0.1f);
    glPushMatrix();
    {
        glScalef(widthAndHeight / 2.0f, widthAndHeight / 2.0f, 1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();
}

/**
 * Zeichnet Waende um das Spielfeld herum
 * @param drawNormals
 */
void drawWalls(GLboolean drawNormals)
{
    float widthAndHeight = getInterpolatedMeshWidth();
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            //Um 0.5 nach oben translatieren, da Kontrollpunkte zwischen 0 und 1 generiert werden
            glTranslatef(0.0f, 0.5f, 0.0f);
            glRotatef((90.0f * i), 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -(widthAndHeight / 2.0f));
            drawWall(drawNormals);
        }
        glPopMatrix();
        glPushMatrix();
        {
            //Mitte muss bei y = 0.5f sein
            glTranslatef(0.0f, 0.5f, 0.0f);
            glRotatef((90.0f * i), 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, (widthAndHeight / 2.0f));
            drawWall(drawNormals);
        }
        glPopMatrix();
    }
}

/**
 * Liefert die Modellmatrix der Sonne
 */
GLfloat *getSunPosMatrix(void)
{
    return g_sunPosMatrix;
}
