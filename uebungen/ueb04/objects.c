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
 * Erstellt einen Flieger
 * @param position Position im Raum
 * @param velocity Geschwindigkeitsvektor fuer die Orientation
 * @param rightVector VxA fuer die Orientation
 * @param isTarget Wahrheitswert ob es sich um das ausgewaehlte Partikel handelt
 */
void drawPlane(float *position, float *velocity, float *rightVector, GLboolean isTarget)
{
    CGVector3f front = {position[LX], position[LY], position[LZ]};
    CGVector3f leftWing = {position[LX] - velocity[LX] - (rightVector[LX] / WING_WIDTH),
                           position[LY] - velocity[LY] - (rightVector[LY] / WING_WIDTH),
                           position[LZ] - velocity[LZ] - (rightVector[LZ] / WING_WIDTH)};
    CGVector3f rightWing = {position[LX] - velocity[LX] + (rightVector[LX] / WING_WIDTH),
                            position[LY] - velocity[LY] + (rightVector[LY] / WING_WIDTH),
                            position[LZ] - velocity[LZ] + (rightVector[LZ] / WING_WIDTH)};
    CGVector3f backMid = {position[LX] - velocity[LX],
                          position[LY] - velocity[LY],
                          position[LZ] - velocity[LZ]};

    if (isTarget) //Ausgewahlter Flieger in einer anderen Farbe
    {
        glBegin(GL_TRIANGLES);
        {
            setMaterialAndColor(RED, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            //Oben links
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(leftWing[LX], leftWing[LY], leftWing[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            //Unten links
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            glVertex3f(leftWing[LX], leftWing[LY], leftWing[LZ]);

            setMaterialAndColor(BLUE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            //Unten rechts
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(rightWing[LX], rightWing[LY], rightWing[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            //Oben rechts
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            glVertex3f(rightWing[LX], rightWing[LY], rightWing[LZ]);
        }
        glEnd();
    }
    else //Normale Flieger in anderer Farbe
    {
        glBegin(GL_TRIANGLES);
        {
            setMaterialAndColor(GREEN, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            //Oben links
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(leftWing[LX], leftWing[LY], leftWing[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            //Unten links
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            glVertex3f(leftWing[LX], leftWing[LY], leftWing[LZ]);

            setMaterialAndColor(YELLOW, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            //Oben rechts
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
            glVertex3f(rightWing[LX], rightWing[LY], rightWing[LZ]);
            //Unten rechts
            glVertex3f(front[LX], front[LY], front[LZ]);
            glVertex3f(rightWing[LX], rightWing[LY], rightWing[LZ]);
            glVertex3f(backMid[LX], backMid[LY], backMid[LZ]);
        }
        glEnd();
    }
}

/**
 * Zeichnet das Partikel als Linie.
 * @param position Position des Partikels
 * @param velocity Geschwindigkeit des Partikels
 * @param isTarget Wahrheitswert ob es sich um das ausgewaehlte Partikel handelt
 */
void drawPlaneAsLine(float *position, float *velocity, GLboolean isTarget)
{
    if (isTarget) //Andere Farbe fuer ausgewahlten
    {
        glBegin(GL_LINES);
        setMaterialAndColor(RED, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glVertex3f(position[LX], position[LY], position[LZ]);
        setMaterialAndColor(BLUE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glVertex3f(position[LX] - velocity[LX], position[LY] - velocity[LY], position[LZ] - velocity[LZ]);
        glEnd();
    }
    else
    {
        glBegin(GL_LINES);
        setMaterialAndColor(GREEN, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glVertex3f(position[LX], position[LY], position[LZ]);
        setMaterialAndColor(YELLOW, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glVertex3f(position[LX] - velocity[LX], position[LY] - velocity[LY], position[LZ] - velocity[LZ]);
        glEnd();
    }
}

/**
 * Erstellt einen Schatten
 * @param position Position im Raum
 * @param velocity Geschwindigkeitsvektor fuer die Orientation
 * @param rightVector VxA fuer die Orientation
 */
void drawShadow(float *position, float *velocity, float *rightVector)
{
    CGVector3f front = {position[LX], position[LY], position[LZ]};
    CGVector3f leftWing = {position[LX] - velocity[LX] - (rightVector[LX] / WING_WIDTH),
                           position[LY] - velocity[LY] - (rightVector[LY] / WING_WIDTH),
                           position[LZ] - velocity[LZ] - (rightVector[LZ] / WING_WIDTH)};
    CGVector3f rightWing = {position[LX] - velocity[LX] + (rightVector[LX] / WING_WIDTH),
                            position[LY] - velocity[LY] + (rightVector[LY] / WING_WIDTH),
                            position[LZ] - velocity[LZ] + (rightVector[LZ] / WING_WIDTH)};
    glBegin(GL_TRIANGLES);
    {
        //Oben
        glVertex3f(front[LX], SHADOW_DISTANCE_TO_GROUND, front[LZ]);
        glVertex3f(leftWing[LX], SHADOW_DISTANCE_TO_GROUND, leftWing[LZ]);
        glVertex3f(rightWing[LX], SHADOW_DISTANCE_TO_GROUND, rightWing[LZ]);
        //Unten
        glVertex3f(front[LX], SHADOW_DISTANCE_TO_GROUND, front[LZ]);
        glVertex3f(rightWing[LX], SHADOW_DISTANCE_TO_GROUND, rightWing[LZ]);
        glVertex3f(leftWing[LX], SHADOW_DISTANCE_TO_GROUND, leftWing[LZ]);
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

/**
 * Zeichnet ein Einheitsrechteck
 * @param drawNormals ob normalen gezeichent werden muessen.
 */
static void drawRect(GLboolean drawNormals)
{
    glBegin(GL_QUADS);
    {
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
    }
    glEnd();
    if (drawNormals)
    {
        drawLineInBetween(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }
}

/**
 * Zeichnet eine Wand
 * @param drawNormals ob normalen gezeichnet werden muessen.
 */
static void drawWall(GLboolean drawNormals)
{
    //Transparenz
    glColor3f(WHITE);
    glPushMatrix();
    {
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
    if (getTexturingStatus())
    {
        /* Texturierung aktivieren */
        glEnable(GL_TEXTURE_2D);
    }
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            glRotatef((90.0f * i), 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -1.0f);
            drawWall(drawNormals);
        }
        glPopMatrix();
    }
    /* Oberseite */
    glPushMatrix();
    {
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -1.0f);
        drawWall(drawNormals);
    }
    glPopMatrix();
    if (getTexturingStatus())
    {
        /* Texturierung aktivieren */
        glDisable(GL_TEXTURE_2D);
    }
    /* Bodenseite */
    glColor3f(BLUE);
    glPushMatrix();
    {
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -1.0f);
        drawRect(drawNormals);
    }
    glPopMatrix();
}