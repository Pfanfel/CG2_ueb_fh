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
#include "scene.h"
#include "io.h"
#include "logic.h"
#include "types.h"
#include "debugGL.h"
#include "stringOutput.h"
#include "objects.h"
#include "util.h"
#include "texture.h"
#include "float.h"
/* ---- Globale Variablen ---- */

GLboolean g_normals = GL_FALSE;

GLuint g_ListIdSphere; //Sphere

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp()
{
    int size = 24;

    float color[3] = {LIGHT_BLUE};

    char *help[] = {"Hilfe:",
                    "w,a,s,d - Kamera bewegen",
                    "./, - Kamera rein/raus Zoomen",
                    "Pfeiltasten oben/unten - Partikel Anzahl erhoehen/verringern",
                    "H/h - Hilfe an/aus",
                    "E/e - Darstellung der Partikel aendern",
                    "M/m - Bewegung der Zielpunkte an/aus",
                    "N/n - Verfolger Partikel weiterschalten",
                    "R/r - Schattenwurf an/aus",
                    "c, C - Kamera wechseln",
                    "v, V - Orientierungshilfe in 3D an/aus",
                    "t, T - Textur der Flaeche wechseln",
                    "P/p - Pausiert bzw. setzt Simulation fort",
                    "Z/z - Wechsel den Verfolgungsmodus",
                    "1 - Beschleunigungsvektor des Partikels an/aus",
                    "2 - Geschwindigkeitsvektor des Partikels an/aus",
                    "3 - Up-Vekor des Partikels an/aus",
                    "F1 - Wireframe an/aus",
                    "F2 - Vollbild an/aus",
                    "F3 - Normalen an/aus",
                    "F4 - Texturen an/aus",
                    "F5 - Lichtberechnung an/aus",
                    "F6 - Punktlichtquelle an/aus",
                    "ESC/Q/q - Ende"};

    drawString(0.2f, 0.10f, color, help[0]);

    for (int i = 1; i < size; ++i)
    {
        drawString(0.2f, 0.10f + i * 0.032f, color, help[i]);
    }
}

/**
 * Schreibt die Information ueber FPS und partikel in den Fenstertitel
 */
static void drawInfoInWindowTitle(void)
{
    //Name
    char *name = "CG2 Partikelsystem |";

    //Vertices
    //Holt sich die Kontrollpunkte und wandelt sie in ein String um
    int particleAmount = getParticleAmount();
    char particleAmountString[4];
    sprintf(particleAmountString, "%d", particleAmount);
    char *particleAmountStringFinal = concat(" Anzahl Partikel: ", particleAmountString);

    //FPS
    float fps = getFps();
    char fpsString[6];
    sprintf(fpsString, "%.2f ", fps);
    char *fpsStringOut = concat(" | FPS: ", fpsString);

    char *title = concat(name, particleAmountStringFinal);
    char *titleFinal = concat(title, fpsStringOut);

    glutSetWindowTitle(titleFinal);

    //Zwischenstrings freeen
    free(particleAmountStringFinal);
    particleAmountStringFinal = NULL;
    free(fpsStringOut);
    fpsStringOut = NULL;
    free(title);
    title = NULL;
    free(titleFinal);
    titleFinal = NULL;
}

/**
 * Kuemmert sich um das setzten der Lichtberechnung und der Lichtquellen
 *
 */
static void handleLight()
{
    if (getLightingStatus())
    {
        /* Lichtberechnung aktivieren */
        glEnable(GL_LIGHTING);

        if (getLight0Status())
        {
            /*Postion des Lichtes*/
            CGPoint4f lightPos0 = {0.0f, 1.0f, 0.0f, 0.0f};
            /* Position der ersten Lichtquelle setzen */
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
            /* Erste Lichtquelle aktivieren */
            glEnable(GL_LIGHT0);
        }
        else
        {
            glDisable(GL_LIGHT0);
        }
    }
    else
    {
        /* Lichtberechnung deaktivieren */
        glDisable(GL_LIGHTING);
    }
}

/**
 * Zeichnet alle Partikel.
 */
static void drawParticles(void)
{
    int particleAmount = getParticleAmount();
    for (int i = 0; i < particleAmount; i++)
    {
        //Partikel Vektoren holen
        CGVector3f position = {getParticleX(i), getParticleY(i), getParticleZ(i)};
        CGVector3f velocity = {getParticleVelocityX(i), getParticleVelocityY(i), getParticleVelocityZ(i)};
        CGVector3f acceleration = {getParticleAccelarationX(i), getParticleAccelarationY(i), getParticleAccelarationZ(i)};
        CGVector3f upVector = {getParticleUpX(i), getParticleUpY(i), getParticleUpZ(i)};
        CGVector3f velocityCrossAccelaration = {0};
        calcCrossProduct(velocity, acceleration, velocityCrossAccelaration);
        //Vektoren normalisieren
        normalizeVector(velocity);
        normalizeVector(acceleration);
        normalizeVector(upVector);
        normalizeVector(velocityCrossAccelaration);
        //Vektoren zum Zeichnen noch verkleinern
        multiplyVectorWithScalar(acceleration, VECTOR_LENGTH_FACTOR, acceleration);
        multiplyVectorWithScalar(velocity, VECTOR_LENGTH_FACTOR, velocity);
        multiplyVectorWithScalar(upVector, VECTOR_LENGTH_FACTOR, upVector);
        multiplyVectorWithScalar(velocityCrossAccelaration, VECTOR_LENGTH_FACTOR, velocityCrossAccelaration);

        //Beschleunigungvektor
        if (getDrawParticleAccelerationStatus())
        {
            setMaterialAndColor(RED, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            drawLineInBetween(position[LX], position[LY], position[LZ],
                              position[LX] + acceleration[LX],
                              position[LY] + acceleration[LY],
                              position[LZ] + acceleration[LZ]);
        }
        //Geschwindigkeitsvektor
        if (getDrawParticleVelocityStatus())
        {
            setMaterialAndColor(BLUE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            drawLineInBetween(position[LX], position[LY], position[LZ],
                              position[LX] + velocity[LX],
                              position[LY] + velocity[LY],
                              position[LZ] + velocity[LZ]);
        }
        //Up-Vektor
        if (getDrawParticleUpVectorStatus())
        {
            setMaterialAndColor(WHITE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            drawLineInBetween(position[LX], position[LY], position[LZ],
                              position[LX] + upVector[LX],
                              position[LY] + upVector[LY],
                              position[LZ] + upVector[LZ]);
        }

        //Dreiecke zeichnen
        if (getParticleLineStatus())
        {
            if (i == getPickedParticle())
            {
                drawPlane(position, velocity, velocityCrossAccelaration, GL_TRUE);
            }
            else
            {
                drawPlane(position, velocity, velocityCrossAccelaration, GL_FALSE);
            }
        }
        else
        //Linien zeichnen
        {
            if (i == getPickedParticle())
            {
                drawPlaneAsLine(position, velocity, GL_TRUE);
            }
            else
            {
                drawPlaneAsLine(position, velocity, GL_FALSE);
            }
        }
    }
}

/**
 * Zeichnet alle Partikel.
 */
static void drawShadows(void)
{
    int particleAmount = getParticleAmount();
    for (int i = 0; i < particleAmount; i++)
    {
        //Partikel Vektoren holen
        CGVector3f position = {getParticleX(i), getParticleY(i), getParticleZ(i)};
        CGVector3f velocity = {getParticleVelocityX(i), getParticleVelocityY(i), getParticleVelocityZ(i)};
        CGVector3f acceleration = {getParticleAccelarationX(i), getParticleAccelarationY(i), getParticleAccelarationZ(i)};
        CGVector3f velocityCrossAccelaration = {0};
        calcCrossProduct(velocity, acceleration, velocityCrossAccelaration);
        //Vektoren normalisieren
        normalizeVector(velocity);
        normalizeVector(velocityCrossAccelaration);
        //Vektoren zum Zeichnen noch verkleinern
        multiplyVectorWithScalar(velocity, VECTOR_LENGTH_FACTOR, velocity);
        multiplyVectorWithScalar(velocityCrossAccelaration, VECTOR_LENGTH_FACTOR, velocityCrossAccelaration);

        setMaterialAndColor(BLACK, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        if (getParticleLineStatus())
        {
            drawShadow(position, velocity, velocityCrossAccelaration);
        }
        else
        {
            drawLineInBetween(position[LX], SHADOW_DISTANCE_TO_GROUND, position[LZ],
                              position[LX] - velocity[LX],
                              SHADOW_DISTANCE_TO_GROUND,
                              position[LZ] - velocity[LZ]);
        }
    }
}

/**
 * Zeichnet die Baelle um die sich die Partikel bewegen.
 */
static void drawBalls(void)
{
    for (int i = 0; i < BALL_COUNT; i++)
    {
        float x = getBallX(i);
        float y = getBallY(i);
        float z = getBallZ(i);

        if (i == 0)
        {
            glColor3f(BLUE);
        }
        else
        {
            glColor3f(PINK);
        }
        glPushMatrix();
        {
            glTranslatef(x, y, z);
            glScalef(0.2f, 0.2f, 0.2f);
            drawSphere();
        }
        glPopMatrix();
    }
}

/**
 * Zeichnet das Spielfeld mit den dazugehoerigen Elementen
 */
static void drawGameField(void)
{
    drawWalls(g_normals);
    drawParticles();
    if (getDrawShadows())
    {
        drawShadows();
    }
    drawBalls();
}

/**
 * Zeichen-Funktion.
 * Stellt die das Spielfeld dar.
 */
void drawScene(void)
{
    drawInfoInWindowTitle();
    /* Hilfe zeichnen */
    if (getHelpStatus())
    {
        drawHelp();
    }
    else
    {
        handleLight();
        if (getVisualHelp())
        {
            drawGrid();
            drawAxes();
        }
        drawGameField();
    }
}

/**
 * Toggelt die Normalen.
 */
void toggleNormals(void)
{
    g_normals = !g_normals;
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * Initialisierung der Lichtquellen.
 * Setzt Eigenschaften der Lichtquellen (Farbe, Oeffnungswinkel, ...)
 */
static void
initLight(void)
{
    /* Farbe der ersten Lichtquelle */
    CGPoint4f lightColor0[3] =
        {{0.1f, 0.1f, 0.1f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.5f, 1.5f, 1.5f, 1.0f}};

    /* Farbe der ersten Lichtquelle setzen */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor0[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0[1]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0[2]);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    /* Z-Buffer-Test aktivieren */
    glEnable(GL_DEPTH_TEST);

    /* Polygonrueckseiten nicht anzeigen */
    glCullFace(GL_BACK);

    glEnable(GL_CULL_FACE);

    /* Normalen nach Transformationen wieder auf die
     * Einheitslaenge bringen */
    glEnable(GL_NORMALIZE);

    /*Fuer den Farbverlauf der Wassers ohne Textur aber mit Beleuchtung*/
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    /* Lichtquellen aktivieren */
    initLight();

    /*Logic init.*/
    initLogic();

    /* Linienbreite */

    glLineWidth(2.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    /* Blending aktivieren */
    glEnable(GL_BLEND);

    /* Blending Funktion setzen
       * Die Blending-Funktion legt fest, wie die Farbwerte
       * der Quelle und des Ziels berechnet werden.
       * (Hier nur die Alphawerte).
       *
       * der erste Parameter legt dies für die Quelle
       * der zweite Parameter für das Ziel fest.
       *
       * So würde z.B. GL_SRC_ALPHA, GL_ONE als Parameterpaar
       * additiver Lichtmischung entsprechen.  */

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
}
