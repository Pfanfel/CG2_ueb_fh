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

#endif

#include <string.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "io.h"
#include "logic.h"
#include "types.h"
#include "debugGL.h"
#include "texture.h"
#include "stringOutput.h"
/* ---- Globale Variablen ---- */

/*Status ob Normalen angezeigt werden sollen*/
GLboolean g_normals = GL_FALSE;
/* Anzahl der Punkte in der Kurve */
int g_resolutionVerticesCount = 0;
/* Aufloesung der Kurve */
float g_resolution = DEFAULT_RESOLUTION;
/* Vertex Array fuer Kurve */
Vertex *g_curveVertices = NULL;
/* Indize Array fuer Kurve */
GLuint *g_curveIndices = NULL;

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp()
{
    int size = 15;

    float color[3] = {LIGHT_BLUE};

    char *help[] = {"Hilfe:",
                    "+/- - Anzahl der Vertices erhöhen",
                    "h,H - Hilfe an/aus",
                    "b,B - Spline und Bezier umschalten",
                    "s,S - Flieger starten",
                    "c,C - Konvexe Huelle zeichnen umschalten",
                    "p, P - Pausiert bzw. setzt Fliegen fort",
                    "n, N - Normalen an/aus",
                    "1 - Level 1",
                    "2 - Level 2",
                    "3 - Level 3",
                    "F1 - Wireframe an/aus",
                    "F2 - Vollbild an/aus",
                    "F3 - Texturen an/aus",
                    "ESC/Q/q - Ende"};

    drawString(0.25f, 0.2f, color, help[0]);

    for (int i = 1; i < size; ++i)
    {
        drawString(0.25f, 0.2f + i * 0.04f, color, help[i]);
    }
}

/**
 * Hilfsmethode welche eine Linie zwischen 2 Punkten zeichnet
 * @param x1 x Koordinate des ersten Punktes
 * @param y1 y Koordinate des ersten Punktes
 * @param z1 z Koordinate des ersten Punktes
 * @param x2 x Koordinate des zweiten Punktes
 * @param y2 y Koordinate des zweiten Punktes
 * @param z2 z Koordinate des zweiten Punktes
 */
static void drawLineInBetween(float x1, float y1, float z1, float x2, float y2, float z2)
{
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

/**
 * Berechnet Betrag/Laenge eines Vektors
 * @param a ein Vektor
 * @return der Betrag des Vektors
 */
float calcVectorLength(float *a)
{
    float res = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        res = res + pow(a[i], 2);
    }
    return sqrtf(res);
}

/**
 * Berechnet das Kreuzprodukt zwischen zwei 3D Vektoren und schreibt das normalisierte Ergebnis auf res.
 * @param a Zeiger auf den ersten Vektor
 * @param b Zeiger auf den zweiten Vektor
 * @param res Zeiger auf das Arrayelemet in das das Erg. geschieben wird
 */
static void calcCrossProduct(GLfloat *a, GLfloat *b, GLfloat *res)
{
    float length;
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
    length = calcVectorLength(res);
    res[0] *= 0.1f / length; //Normaliserter Vektor länge 0.1 statt 1
    res[1] *= 0.1f / length;
    res[2] *= 0.1f / length;
}

/**
 * Berechnet das Kreuzprodukt zweiter Vektoren
 * @param a Vektor a
 * @param Vektor b
 * @return das Kreuzprodukt zwischen den beiden Vektoren
 */
float calcDotProduct(float *a, float *b)
{
    float res = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        res = res + a[i] * b[i];
    }
    return res;
}

/**
 * Zeichnet einen Einheitskreis.
 */
static void drawCircle(void)
{
    GLUquadricObj *circle = gluNewQuadric();
    gluDisk(circle, 0, 1.0f, 50, 1);
    gluDeleteQuadric(circle);
}

/**
 * Zeichnet die Kontrollpunkte
 */
void drawControlPoints(void)
{
    //Das Aktuelle Level bestimmen
    int controlPointsAmount = getg_currentLevel() + LOWEST_AMOUNT_CONTROL_POINTS;
    for (int i = 0; i < controlPointsAmount; i++)
    {
        float currPointX = getControlpointX(i);
        float currPointY = getControlpointY(i);
        glPushMatrix();
        {
            //fprintf(stdout, "x: %f, y: %f\n", currPointX, currPointY);
            //An den Punkt aus dem Array translaten
            glTranslatef(currPointX, currPointY, 0.0);
            glScalef(CONTROL_POINT_RADIUS, CONTROL_POINT_RADIUS, 1.0);
            //Farbe setzen (erste und letze sind rot)
            if (i == 0 || i == controlPointsAmount - 1)
            {
                glColor3f(RED);
            }
            else
            {
                glColor3f(GREEN);
            }
            drawCircle();
        }
        glPopMatrix();
    }
}

/**
 * Erstellt ein Einheitsdreieck.
 */
static void drawTriangle(void)
{
    GLUquadricObj *circle = gluNewQuadric();
    gluDisk(circle, 0, 1.0f, 3, 1);
    gluDeleteQuadric(circle);
}

/**
 * Berechnet die Rotation des Fliegers um die Z-Achse ausgehend von der uebergebenen Normale 
 * @param n Die Normale der Kurve unter dem Flieger
 * @return der Winkel um den Rotiert werden soll
 */
float calcPlaneRotation(float *n)
{
    GLfloat w[3] = {1.0f, 0.0f, 0.0f};
    // Winkel wird berechnet
    float alpha = acosf(calcDotProduct(n, w) / (calcVectorLength(n) * calcVectorLength(w)));
    // wenn y < 0 ist dann muss winkel * (-1) gerechnet werden sonst hat man komische ergebnisse und kann hoch bzw runter fliegen nicht dirrefernzieren
    if (n[CY] < 0.0f)
    {
        alpha *= (-1.0f);
    }
    alpha = radToDegree(alpha);
    return alpha;
}

/**
 * Berechnet die Normale des Fliegers
 * @param n Out-Parameter die Normale
 */
void calcPlaneNormal(float *n)
{
    GLfloat v[3] = {0.0f, 0.0f, 1.0f};
    GLfloat u[3] = {0};
    //holt sich die Position vom Plane und berechnet die Positionen links und rechts mit einem OFFSET um die Normale zu berechnen
    float planePosition = getPlaneT();
    if (planePosition - PLANE_NORMAL_OFFSET >= 0.0f + DELTA && planePosition + PLANE_NORMAL_OFFSET <= 1.0f - DELTA)
    {
        //Linker und rechter Punkt neben der aktuellen Position des Fliegers
        u[CX] = calculateX(planePosition - PLANE_NORMAL_OFFSET) - calculateX(planePosition + PLANE_NORMAL_OFFSET);
        u[CY] = calculateY(planePosition - PLANE_NORMAL_OFFSET) - calculateY(planePosition + PLANE_NORMAL_OFFSET);
        u[CZ] = 0.0f;
    }
    else if (planePosition - PLANE_NORMAL_OFFSET < 0.0f + DELTA)
    {
        u[CX] = calculateX(planePosition) - calculateX(planePosition + PLANE_NORMAL_OFFSET);
        u[CY] = calculateY(planePosition) - calculateY(planePosition + PLANE_NORMAL_OFFSET);
        u[CZ] = 0.0f;
    }
    else if (planePosition + PLANE_NORMAL_OFFSET > 1.0f - DELTA)
    {
        u[CX] = calculateX(planePosition - PLANE_NORMAL_OFFSET) - calculateX(planePosition);
        u[CY] = calculateY(planePosition - PLANE_NORMAL_OFFSET) - calculateY(planePosition);
        u[CZ] = 0.0f;
    }
    // Normale wird berechnet
    calcCrossProduct(u, v, n);
}

/**
 * Zeichnet den Flieger an einer bestimmten Position
 * @param x x-Position
 * @param y y-Position
 */
void drawPlane(float x, float y)
{
    GLfloat n[3] = {0};
    calcPlaneNormal(n);
    float rotation = calcPlaneRotation(n);
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    {
        //Verschiebt den Flieger auf die momentane Position
        //Translatiert ihn in richtung des Normalenvektors,
        //sodass Flieger ueber der Kurve schwebt
        //Rotiert ihn, je nach Winkel von Normale zur x-Achse
        glTranslatef(x + n[CX], y + n[CY], 0.0f);
        glRotatef(-180.0f + rotation, 0.0f, 0.0f, 1.0f);
        glScalef(PLANE_DIAMETER / 2.0f, PLANE_DIAMETER, 1.0f);
        drawTriangle();
    }
    glPopMatrix();
}

/**
* Zeichnet einen Stern an einen bestimmten Position mit einem bestimmten Radius
* @param x x-Position
* @param y y-Position
* @param radius der radius
*/
void drawStar(float x, float y, float radius)
{
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    {
        glTranslatef(x, y, 0.0f);
        glScalef(radius, radius, 1.0f);
        drawTriangle();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(x, y, 0.0f);
        glRotatef(-180, 0.0f, 0.0f, 1.0f);
        glScalef(radius, radius, 1.0f);
        drawTriangle();
    }
    glPopMatrix();
}

/**
 * Zeichnet je nach aktuellem Level die Sterne
 */
void drawStars(void)
{
    int starCount = getStarCount();
    for (int i = 0; i < starCount; i++)
    {
        Star star = getStar(i);
        if (star.isVisible)
        {
            drawStar(star.x, star.y, STAR_DIAMETER);
        }
    }
}

/**
 * Zeichnet eine Wolke.
 */
void drawCloud(void)
{
    Cloud cloud = getCloud();
    if (cloud.isVisible)
    {
        float x = cloud.x;
        float y = cloud.y;
        glColor3f(0.5f, 0.5f, 0.5f);
        glPushMatrix();
        {
            glTranslatef(x, y, 0.0f);
            glScalef(CLOUD_DIAMETER, CLOUD_DIAMETER, 1.0f);
            drawCircle();
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(x + 0.05f, y - 0.0125f, 0.0f);
            glScalef(CLOUD_DIAMETER - 0.025f, CLOUD_DIAMETER - 0.025f, 1.0f);
            drawCircle();
        }
        glPopMatrix();
        glPushMatrix();
        {
            glTranslatef(x - 0.05f, y - 0.0125f, 0.0f);
            glScalef(CLOUD_DIAMETER - 0.025f, CLOUD_DIAMETER - 0.025f, 1.0f);
            drawCircle();
        }
        glPopMatrix();
    }
}

/*Zeichnet eine Hintergrundtextur*/
void drawBackground(void)
{
    bindTexture(background);

    if (getTexturingStatus())
    {
        glColor3f(WHITE);
    }
    else
    {
        glColor3f(BLACK);
    }
    glPushMatrix();
    {
        glRotatef(180, 0.0f, 0.0f, 1.0f);
        glTranslatef(-1.0f, -1.0f, 0);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(2.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(2.0f, 2.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0.0f, 2.0f);
        }
        glEnd();
    }
    glPopMatrix();
    unbindTexture();
}

/**
 * Zeichnet das Feld mit den dazugehoerigen Normalen und Texturen
 */
static void drawField(void)
{
    drawBackground();
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(g_curveVertices[0][CX]));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), &(g_curveVertices[0][NX]));
    glColor3f(WHITE);
    glLineWidth(3.0f);
    glDrawElements(GL_LINE_STRIP, g_resolutionVerticesCount, GL_UNSIGNED_INT, g_curveIndices);
    glLineWidth(1.0f);
    drawPlane(getPlaneX(), getPlaneY());
    drawStars();
    drawCloud();
}

/**
 * Zeichnet die Normalen
 */
static void drawVertexNormals(void)
{
    for (int i = 0; i < g_resolutionVerticesCount; i++)
    {
        drawLineInBetween(g_curveVertices[i][CX],
                          g_curveVertices[i][CY],
                          g_curveVertices[i][CZ],
                          g_curveVertices[i][CX] + g_curveVertices[i][NX],
                          g_curveVertices[i][CY] + g_curveVertices[i][NY],
                          g_curveVertices[i][CZ] + g_curveVertices[i][NZ]);
    }
}

void drawConvexHull(void)
{
    int length = getConvexHullLength();
    glBegin(GL_POLYGON);
    {
        for (int i = 0; i < length; i++)
        {
            glColor4f(0.0f, 1.0f, 0.4f, 0.2f);
            glVertex3f(getConvexHullPointXByIndex(i), getConvexHullPointYByIndex(i), 0.0f);
        }
    }
    glEnd();
}

/**
 * Zeichen-Funktion.
 * Stellt die das Spielfeld dar.
 */
void drawScene(void)
{
    /* Hilfe zeichnen */
    if (getHelpStatus())
    {
        drawHelp();
    }
    else
    {
        if (getTexturingStatus())
        {
            /* Texturierung aktivieren */
            glEnable(GL_TEXTURE_2D);
        }
        else
        {
            /* Texturierung deaktivieren */
            glDisable(GL_TEXTURE_2D);
        }
        drawField();
        if (getConvexHullStatus())
        {
            calcConvexHull();
            drawConvexHull();
        }
        drawControlPoints();
        if (g_normals)
        {
            drawVertexNormals();
        }
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
 * Berechnet die Normalen der Linie anhand der Nachbarpunkte.
 */
static void calcVertexNormals(int z)
{
    GLfloat v[3] = {0.0f, 0.0f, 1.0f};
    GLfloat u[3] = {0};
    GLfloat n[3] = {0};
    //Links ausserhalb
    if (z - 1 < 0)
    {
        u[CX] = g_curveVertices[z][CX] - g_curveVertices[z + 1][CX];
        u[CY] = g_curveVertices[z][CY] - g_curveVertices[z + 1][CY];
        u[CZ] = 0.0f;
    }
    else
        //Rechts ausserhalb
        if (z + 1 >= g_resolutionVerticesCount)
    {
        u[CX] = g_curveVertices[z - 1][CX] - g_curveVertices[z][CX];
        u[CY] = g_curveVertices[z - 1][CY] - g_curveVertices[z][CY];
        u[CZ] = 0.0f;
    }
    else
    {
        //Links und rechts vom Aktuellen
        u[CX] = g_curveVertices[z - 1][CX] - g_curveVertices[z + 1][CX];
        u[CY] = g_curveVertices[z - 1][CY] - g_curveVertices[z + 1][CY];
        u[CZ] = 0.0f;
    }

    calcCrossProduct(u, v, n);

    g_curveVertices[z][NX] = n[CX];
    g_curveVertices[z][NY] = n[CY];
    g_curveVertices[z][NZ] = n[CZ];
}

/**
 * Veraendert das Vertex Array.
 */
void updateVertexArray(GLboolean resetPlane)
{
    //Punkte pro Splinesubbereich berechnen
    int verticesPerSubPart = ((int)((1.0f / g_resolution) + DELTA));
    //Punkte insgesamt Berechnen (in abh. von Level)
    g_resolutionVerticesCount = 1 + verticesPerSubPart;

    //Den benötigten Speicher reservieren
    g_curveVertices = realloc(g_curveVertices, sizeof(Vertex) * g_resolutionVerticesCount);
    g_curveIndices = realloc(g_curveIndices, sizeof(GLuint) * g_resolutionVerticesCount);

    //Pruefen ob reservieren geklappt hat
    if (g_curveVertices != NULL && g_curveIndices != NULL)
    {
        float T = 0.0f;
        //Ueber alle interpolierten Punke der Kurve laufen
        for (int i = 0; i < g_resolutionVerticesCount; i++)
        {
            //Beim letzen Punkt muss T == 1 sein.
            if (i == g_resolutionVerticesCount - 1)
            {
                T = 1.0f;
            }
            g_curveVertices[i][CX] = calculateX(T);
            g_curveVertices[i][CY] = calculateY(T);
            g_curveVertices[i][CZ] = 0.0f;
            g_curveVertices[i][NX] = 0.0f;
            g_curveVertices[i][NY] = 0.0f;
            g_curveVertices[i][NZ] = 0.0f;
            g_curveIndices[i] = i;
            //Um einen Aufloesungsschritt erhohen (0 - 0.999)
            T += g_resolution;
        }
    }
    else
    {
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
    }
    //Fuer alle interpolierten Punkte die Normalen berechnen
    for (int z = 0; z < g_resolutionVerticesCount; z++)
    {
        calcVertexNormals(z);
    }
    if (resetPlane)
    {
        updatePlanePosition(0.0f);
    }
}

/*Verringert die Aufluesung der dargestellten Kurve*/
void decreaseResolution(void)
{
    if (g_resolution + DEFAULT_RESOLUTION < 1.0f + DELTA)
    {
        g_resolution += DEFAULT_RESOLUTION;
        updateVertexArray(GL_FALSE);
    }
}

/*Erhoeht die Aufluesung der dargestellten Kurve*/
void increaseResolution(void)
{
    if (g_resolution - DEFAULT_RESOLUTION > 0.0f + DELTA)
    {
        g_resolution -= DEFAULT_RESOLUTION;
        updateVertexArray(GL_FALSE);
    }
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

    initLogic();

    /* Linienbreite */
    glLineWidth(1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    /* Alles in Ordnung? */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    return (GLGETERROR == GL_NO_ERROR);
}

/**
 * Gibt den Speicher der Scene Arrays frei.
 */
void freeArraysScene(void)
{
    free(g_curveVertices);
    free(g_curveIndices);
}