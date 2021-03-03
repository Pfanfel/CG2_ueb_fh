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
GLboolean g_controlPointsAreVisible = GL_TRUE;
GLboolean g_interpolatedPointsAreVisible = GL_TRUE;

/* Vertex Array */
Vertex *g_vertices = NULL;
/* Indizes zum Ausgeben */
GLuint *g_indices = NULL;

GLuint g_ListIdSphere;             //Sphere der Kontrollpunkte
GLuint g_ListIdSpherePicked;       //gepickte Sphere
GLuint g_ListIdSphereInterpolated; //Sphere der interpolierten Punkte

/* ---- Funktionsprototypen innerhalb ---- */

static int getInterpolatedVerticeCount(void);

/* ---- Funktionen ---- */
/**
 * Initialisiert die Displaylisten und fuellt diese mit Objekten
 */
void initDisplayLists(void)
{ /* IDs fuerzwei Displaylisten anfordern */
    g_ListIdSphere = glGenLists((GLsizei)3);
    if (g_ListIdSphere != 0)
    {
        glNewList(g_ListIdSphere, GL_COMPILE);
        setMaterialAndColor(WHITE, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawSphere();
        glEndList();

        g_ListIdSpherePicked = g_ListIdSphere + 1;
        glNewList(g_ListIdSpherePicked, GL_COMPILE);
        setMaterialAndColor(RED, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawSphere();
        glEndList();

        g_ListIdSphereInterpolated = g_ListIdSphere + 2;
        glNewList(g_ListIdSphereInterpolated, GL_COMPILE);
        setMaterialAndColor(GREEN, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        drawSphere();
        glEndList();
    }
    else /* glGenLists liefert 0 zurueck-> Fehler */
    {
        CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
    }
}

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp()
{
    int size = 19;

    float color[3] = {LIGHT_BLUE};

    char *help[] = {"Hilfe:",
                    "w,a,s,d - Kamera",
                    "./, - Kamera rein/raus Zoomen",
                    "+/- - Anzahl Interpolationspunkte erhoehen/verringern",
                    "H/h - Hilfe an/aus",
                    "i, I - Pausiert bzw. setzt Simulation fort",
                    "b, B - Interpolierte Bezierkurve an/aus",
                    "c, C - Kameraflug entlang Bezier",
                    "v, V - Orientierungshilfe in 3D an/aus",
                    "z, Z - Textur der Flaeche wechseln",
                    "l, L - Punktlichtquelle (Sonne) an/aus",
                    "F1 - Wireframe an/aus",
                    "F2 - Vollbild an/aus",
                    "F3 - Normalen an/aus",
                    "F4 - Texturen an/aus",
                    "F5 - Kontrollpunkte an/aus",
                    "F6 - Interpolierte Punkte an/aus",
                    "F7 - Lichtberechnung an/aus",
                    "ESC/Q/q - Ende"};

    drawString(0.2f, 0.15f, color, help[0]);

    for (int i = 1; i < size; ++i)
    {
        drawString(0.2f, 0.15f + i * 0.04f, color, help[i]);
    }
}

/**
 * Schreibt die Information ueber FPS und vertices in den Fenstertitel
 */
static void drawInfoInWindowTitle(void)
{
    //Name
    char *name = "CG2 Landschaftsgenerator |";

    //Vertices
    //Holt sich die Kontrollpunkte und wandelt sie in ein String um
    int controlPointAmount = (int)sqrt(getControlPointAmount());
    char controlPointAmountString[4];
    sprintf(controlPointAmountString, "%d", controlPointAmount);
    char *controlPointsString = concat(" Kontrollpunkte: ", controlPointAmountString);
    char *controlPointsIntermediateString = concat(controlPointsString, "x");
    char *controlPointsFinalString = concat(controlPointsIntermediateString, controlPointAmountString);

    free(controlPointsString);
    controlPointsString = NULL;
    free(controlPointsIntermediateString);
    controlPointsIntermediateString = NULL;

    int resolutionAmount = (int)sqrt(getInterpolatedVerticeCount());
    char resolutionString[8];
    sprintf(resolutionString, "%d", resolutionAmount);
    char *resolutionAsString = concat(" | Interpolationsaufloesung: ", resolutionString);
    char *resolutionIntermediateString = concat(resolutionAsString, "x");
    char *resolutionFinalString = concat(resolutionIntermediateString, resolutionString);

    free(resolutionAsString);
    resolutionAsString = NULL;
    free(resolutionIntermediateString);
    resolutionIntermediateString = NULL;

    //FPS
    float fps = getFps();
    char fpsString[6];
    sprintf(fpsString, "%.2f ", fps);
    char *fpsStringOut = concat(" | FPS: ", fpsString);

    char *intermediateTitle = concat(name, controlPointsFinalString);
    char *intermediateTitle2 = concat(intermediateTitle, resolutionFinalString);
    char *title = concat(intermediateTitle2, fpsStringOut);

    glutSetWindowTitle(title);

    //Zwischenstrings freeen

    free(controlPointsFinalString);
    controlPointsFinalString = NULL;
    free(resolutionFinalString);
    resolutionFinalString = NULL;
    free(fpsStringOut);
    fpsStringOut = NULL;
    free(intermediateTitle);
    intermediateTitle = NULL;
    free(intermediateTitle2);
    intermediateTitle2 = NULL;
    free(title);
    title = NULL;
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
            CGVector3f sunInWorldCoordinates = {0};
            calcWorldCoordinates(getVirtualCameraMatrix(), getSunPosMatrix(), sunInWorldCoordinates);
            /*Postion des Lichtes in abhaengigkeit von der Sonne*/
            CGPoint4f lightPos0 = {sunInWorldCoordinates[0], sunInWorldCoordinates[1], sunInWorldCoordinates[2], 1.0f};
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
 * Berechnet die Normalen der Ebene anhand der paritellen Ableitungen.
 * @param S intervall [0-1]
 * @param T intervall [0-1]
 * @param normal die berechnete Normale
 */
static void calcVertexNormals(float S, float T, float *normal)
{
    //Vektoren die durch den Punkt und entlang der s und t Achse Verlaufen
    float vS[3] = {0};
    float vT[3] = {0};
    for (int i = 0; i < DIMENSIONS; i++)
    {
        vS[i] = calcGradient(S, T, i, GL_TRUE);
        vT[i] = calcGradient(S, T, i, GL_FALSE);
    }
    calcCrossProduct(vS, vT, normal);
}

/**
 * Liefert die Anzahl der interpolierten Punkte
 * @return die Anzahl.
 */
static int getInterpolatedVerticeCount(void)
{
    int interpolationResolution = getInterpolationResolution();
    return (interpolationResolution * interpolationResolution);
}

/**
 * Zeichnet die Normalen
 */
static void drawVertexNormals(void)
{
    setMaterialAndColor(YELLOW, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    //Anzahl so berechnen weil sonst nicht alle Normalen gezeichnet werden (obere Schranke zu klein)
    int verticesRequired = getInterpolatedVerticeCount();
    //Die Anzahl der Interpolierten Punkte
    for (long i = 0; i < verticesRequired; i++)
    {
        drawLineInBetween(g_vertices[i][CX],
                          g_vertices[i][CY],
                          g_vertices[i][CZ],
                          g_vertices[i][CX] + g_vertices[i][NX],
                          g_vertices[i][CY] + g_vertices[i][NY],
                          g_vertices[i][CZ] + g_vertices[i][NZ]);
    }
}

/**
 * Zeichnet die Kugeln auf den interpolierten Vertices.
 */
void drawInterpolatedPoints(void)
{
    //Anzahl so berechnen weil sonst nicht alle Normalen gezeichnet werden (obere Schranke zu klein)
    int verticesRequired = getInterpolatedVerticeCount();

    for (int i = 0; i < verticesRequired; i++)
    {
        glPushMatrix();
        {
            glTranslatef(g_vertices[i][CX], g_vertices[i][CY], g_vertices[i][CZ]);
            glScalef(0.025f, 0.025f, 0.025f);
            glCallList(g_ListIdSphereInterpolated);
        }
        glPopMatrix();
    }
}

/**
 * Zeichnet die Kugeln auf den Kontrollpunkten.
 */
void drawControlPoints(void)
{
    int controlPointsAmount = getControlPointAmount();
    int selectedControlPointIdx = getSelectedControlPointIdx();
    for (int i = 0; i < controlPointsAmount; i++)
    {
        glPushMatrix();
        {
            float x = getControlPointX(i);
            float y = getControlPointY(i);
            float z = getControlPointZ(i);
            glTranslatef(x, y, z);
            glScalef(0.05f, 0.05f, 0.05f);
            if (i == selectedControlPointIdx)
            {
                glCallList(g_ListIdSpherePicked);
            }
            else
            {
                glCallList(g_ListIdSphere);
            }
        }
        glPopMatrix();
    }
}

/**
 * Zeichnet das Feld mit den dazugehoerigen Normalen und Texturen
 */
static void drawField(void)
{
    if (g_normals)
    {
        drawVertexNormals();
    }
    int interpolationResolution = getInterpolationResolution();
    // Benoetigte Groessen zum Speicher reservieren
    int indicesRequired = ((interpolationResolution - 1) * (interpolationResolution - 1)) * VERTICES_PER_SQUARE;
    //Textur abhaengig von der z/Z Taste
    int textureIdx = getTextureIdx();
    bindTexture(textureIdx);
    //Material setzen bei ausgeschalteter Textur
    //Color abheangig von der Hoehe setzen
    glDrawElements(GL_TRIANGLES, indicesRequired, GL_UNSIGNED_INT, g_indices);

    unbindTexture(textureIdx);

    if (g_controlPointsAreVisible)
    {
        drawControlPoints();
    }
    if (g_interpolatedPointsAreVisible)
    {
        drawInterpolatedPoints();
    }
}

/**
 * Berechnet den niedrigsten Kontrollpunkt.
 * @return Index des niedrigsten Kontrollpunktes
 */
int getIndexOfLowestInterpolatedPoint(void)
{
    float lowest = FLT_MAX;
    int idx = -1;
    int verticesRequired = getInterpolatedVerticeCount();
    for (int i = 0; i < verticesRequired; i++)
    {
        float current = g_vertices[i][LY];
        if (current < lowest - DELTA)
        {
            lowest = current;
            idx = i;
        }
    }
    return idx;
}

/**
 * Berechnet den hoechsten Kontrollpunkt.
 * @return Index des hoechsten Kontrollpunktes
 */
int getIndexOfHighestInterpolatedPoint(void)
{
    float highest = FLT_MIN;
    int idx = -1;
    int verticesRequired = getInterpolatedVerticeCount();
    for (int i = 0; i < verticesRequired; i++)
    {
        float current = g_vertices[i][LY];
        if (current > highest + DELTA)
        {
            highest = current;
            idx = i;
        }
    }
    return idx;
}

/**
 * Zeichnet die Interpolierte Bezierkurve zwischen dem hoehsten und niedrigsem Punkt der Splineflaeche
 */
void drawInterpolatedBezierCurve(void)
{
    setMaterialAndColor(PINK, 0.0f, GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    for (int i = 0; i <= BEZIER_CURVE_RESOLUTION; i++)
    {
        float cameraPositionX = getBezier(i / (float)BEZIER_CURVE_RESOLUTION, LX);
        float cameraPositionY = getBezier(i / (float)BEZIER_CURVE_RESOLUTION, LY);
        float cameraPositionZ = getBezier(i / (float)BEZIER_CURVE_RESOLUTION, LZ);
        glBegin(GL_POINTS);
        glVertex3f(cameraPositionX, cameraPositionY, cameraPositionZ);
        glEnd();
    }
    //Kontrollpunkte
    for (int j = 0; j < 4; j++)
    {
        glPushMatrix();
        {
            glTranslatef(getBezierControlPointX(j), getBezierControlPointY(j), getBezierControlPointZ(j));
            glScalef(0.025f, 0.025f, 0.025f);
            drawSphere();
        }
        glPopMatrix();
    }
}

/**
 * Berechnet den Index des hoehesten und niedrigsten interpolierten Punktes
 */
void calculateLineBetweenLowestAndHighest(void)
{
    int lowestIdx = getIndexOfLowestInterpolatedPoint();
    int highestIdx = getIndexOfHighestInterpolatedPoint();
    float vHighestToLowest[2] = {g_vertices[lowestIdx][TX] - g_vertices[highestIdx][TX],
                                 g_vertices[lowestIdx][TY] - g_vertices[highestIdx][TY]};

    CGVector3f firstControlpoint = {0};
    CGVector3f secondControlpoint = {0};
    float S = g_vertices[highestIdx][TX] + (vHighestToLowest[LX] * (1.0f / 3.0f));
    float T = g_vertices[highestIdx][TY] + (vHighestToLowest[LY] * (1.0f / 3.0f));
    firstControlpoint[LX] = interpolate(S, T, LX);
    firstControlpoint[LY] = interpolate(S, T, LY) + CAMERA_DISTANCE_BEZIER;
    firstControlpoint[LZ] = interpolate(S, T, LZ);
    S = g_vertices[highestIdx][TX] + (vHighestToLowest[LX] * (2.0f / 3.0f));
    T = g_vertices[highestIdx][TY] + (vHighestToLowest[LY] * (2.0f / 3.0f));
    secondControlpoint[LX] = interpolate(S, T, LX);
    secondControlpoint[LY] = interpolate(S, T, LY) + CAMERA_DISTANCE_BEZIER;
    secondControlpoint[LZ] = interpolate(S, T, LZ);
    //Kontrollpunkte fuer Bezier-Interpol. setzten
    setg_bezierControlPoint(0, g_vertices[highestIdx][LX], g_vertices[highestIdx][LY], g_vertices[highestIdx][LZ]);
    setg_bezierControlPoint(1, firstControlpoint[LX], firstControlpoint[LY], firstControlpoint[LZ]);
    setg_bezierControlPoint(2, secondControlpoint[LX], secondControlpoint[LY], secondControlpoint[LZ]);
    setg_bezierControlPoint(3, g_vertices[lowestIdx][LX], g_vertices[lowestIdx][LY], g_vertices[lowestIdx][LZ]);
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
        drawSun();

        if (getDrawBezierStatus() && !getCameraFlightStatus())
        {
            calculateLineBetweenLowestAndHighest();
            drawInterpolatedBezierCurve();
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
 * Toggelt die Kugeln.
 */
void toggleControlPoints(void)
{
    g_controlPointsAreVisible = !g_controlPointsAreVisible;
}

/**
 * Toggelt die interpolierten Kugeln.
 */
void toggleInterpolatedPoints(void)
{
    g_interpolatedPointsAreVisible = !g_interpolatedPointsAreVisible;
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
 * Liefert die Farbe abhaengig von der Hoehe
 * @param height die hoehe des interpol. Punktes
 * @param color die dazugehoerige Farbe
 */
void getColorThroughHeight(float height, float *color)
{
    float grey[3] = {GREY};
    float white[3] = {WHITE};
    float green[3] = {GREEN};
    float blue[3] = {LIGHT_BLUE};
    color[0] = white[0];
    color[1] = white[1];
    color[2] = white[2];
    if (height <= UPPER_BOUND_BLUE + DELTA)
    {
        color[0] = blue[0];
        color[1] = blue[1];
        color[2] = blue[2];
    }
    else if (height <= UPPER_BOUND_GREEN + DELTA)
    {
        color[0] = green[0];
        color[1] = green[1];
        color[2] = green[2];
    }
    else if (height <= UPPER_BOUND_GREY + DELTA)
    {
        color[0] = grey[0];
        color[1] = grey[1];
        color[2] = grey[2];
    }
}

/**
 * Initialisiert das Vertex Array.
 */
void calculateInterpolatedVertexArray(void)
{
    int interpolationResolution = getInterpolationResolution();
    float sampleStepWidth = 1.0f / (interpolationResolution - 1);
    // Benoetigte Groessen zum Speicher reservieren
    int verticesRequired = ((interpolationResolution) * (interpolationResolution));
    int indicesRequired = ((interpolationResolution - 1) * (interpolationResolution - 1)) * VERTICES_PER_SQUARE;
    // Speicher reservieren
    g_vertices = realloc(g_vertices, sizeof(Vertex) * verticesRequired);
    g_indices = realloc(g_indices, sizeof(GLuint) * indicesRequired);
    if (g_vertices != NULL && g_indices != NULL)
    {
        // Laufvariablen
        int vertexIndex = 0;
        int indexBufferIndex = 0;
        //S in z-Richtung
        float S;
        float T;
        for (int i = 0; i < interpolationResolution; i++)
        {
            //T in x-Richtung
            for (int j = 0; j < interpolationResolution; j++)
            {
                S = i * sampleStepWidth;
                T = j * sampleStepWidth;
                g_vertices[vertexIndex][CX] = interpolate(S, T, LX);
                g_vertices[vertexIndex][CY] = interpolate(S, T, LY);
                g_vertices[vertexIndex][CZ] = interpolate(S, T, LZ);
                float color[3] = {1.0f, 1.0f, 1.0f};
                if (!getTexturingStatus())
                {
                    getColorThroughHeight(g_vertices[vertexIndex][CY], color);
                }
                g_vertices[vertexIndex][CR] = color[0];
                g_vertices[vertexIndex][CG] = color[1];
                g_vertices[vertexIndex][CB] = color[2];
                float normal[3] = {0};
                calcVertexNormals(S, T, normal);
                g_vertices[vertexIndex][NX] = normal[0];
                g_vertices[vertexIndex][NY] = normal[1];
                g_vertices[vertexIndex][NZ] = normal[2];
                g_vertices[vertexIndex][TX] = S;
                g_vertices[vertexIndex][TY] = T;
                vertexIndex++;
            }
        }
        for (int z = 0; z < interpolationResolution - 1; z++)
        {
            for (int x = 0; x < interpolationResolution - 1; x++)
            {
                g_indices[indexBufferIndex++] = ((z * (interpolationResolution)) + x);
                g_indices[indexBufferIndex++] = (((z + 1) * (interpolationResolution)) + x);
                g_indices[indexBufferIndex++] = ((z * (interpolationResolution)) + x + 1);
                g_indices[indexBufferIndex++] = (((z + 1) * (interpolationResolution)) + x);
                g_indices[indexBufferIndex++] = (((z + 1) * (interpolationResolution)) + x + 1);
                g_indices[indexBufferIndex++] = ((z * (interpolationResolution)) + x + 1);
            }
        }
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(g_vertices[0][CX]));
        glColorPointer(3, GL_FLOAT, sizeof(Vertex), &(g_vertices[0][CR]));
        glNormalPointer(GL_FLOAT, sizeof(Vertex), &(g_vertices[0][NX]));
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(g_vertices[0][TX]));
    }
    else
    {
        printf("Kein virtueller RAM mehr verfügbar ...\n");
        exit(1);
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

    /* Initialisiert die Displaylisten */
    initDisplayLists();

    /*Logic init.*/
    initLogic();

    /* Initialisiert das Vertex Array */
    calculateInterpolatedVertexArray();

    /* Linienbreite */

    glLineWidth(1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
}

/**
 * Gibt den Speicher der Scene Arrays frei.
 */
void freeArraysScene(void)
{
    free(g_vertices);
    free(g_indices);
}