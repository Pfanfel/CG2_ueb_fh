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
#include "util.h"
#include "float.h"
#include "stdio.h"

/* ---- Globale Variablen ---- */

GLboolean g_rendered = GL_FALSE;
GLint g_firstRenderAfterMoveCount = 0;

GLboolean g_vignette = GL_FALSE;

GLboolean g_light0Status = GL_TRUE;

GLboolean g_light1Status = GL_TRUE;

CGColor3f* g_framebuffer = NULL;

LightSource g_lightPos[] = { {.center = {1.3f, 1.5f, 0.0f}, .color = {WHITE}},
                            {.center = {-1.3f, 1.5f, 0.0f}, .color = {WHITE}} };
int g_numLightsInScene = 2;
int g_numLightsStartIndex = 0;

Sphere g_sphere = { .center = {0.25f, -1.489999f, -0.5f},
                   .radius = 0.5f,
                   .color = {BLUE},
                   .material.materialAmbient = 0.2f,
                   .material.materialDiffuse = 0.2f,
                   .material.materialSpecular = 0.6f,
                   .material.kReflection = 0.0f,
                   .material.kTransmission = 0.0f };

ObjObject g_cube = { 0 };

ObjObject g_bunny = { 0 };

ObjObject g_boundingBoxAABB = { .vertexCount = 8,
                               .faceCount = 12 };

ObjObject g_boundingBoxOBB = { .vertexCount = 8,
                              .faceCount = 12 };

ObjObject g_mirror = { 0 };

ObjObject g_walls = { 0 };

CGVector3f g_backgroundColor = { BLACK };

/* ---- Funktionsprototypen innerhalb ---- */

/* ---- Funktionen ---- */

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp(void)
{
    int size = 15;

    char* help[] = { "Hilfe:",
                    "w,a,s,d - Kamera bewegen",
                    "./, - Kamera rein/raus Zoomen",
                    "h - Hilfe an/aus",
                    "V/v - Kamera von vorne Positionieren ",
                    "H - Kamera von hinten Positionieren ",
                    "L/l - Kamera von links Positionieren ",
                    "R/r - Kamera von rechts Positionieren ",
                    "O/o - Kamera von oben Positionieren ",
                    "U/u - Kamera von unten Positionieren ",
                    "B/b - Bounding Box des Hasen an/aus ",
                    "N/n - Umschalten der Art der Bounding Box (kein, AABB, OBB) ",
                    "1 - Lichtquelle 1 an/aus",
                    "2 - Vignette Effekt an/aus",
                    "ESC/Q/q - Ende" };

    for (int i = 1; i < size; ++i)
    {
        fprintf(stdout, "%s\n", help[i]);
    }
}

/**
 * Schreibt die Information ueber FPS und partikel in den Fenstertitel
 */
static void drawInfoInWindowTitle(void)
{
    //Name
    char* name = "CG2 RayTracing |";

    //FPS
    float fps = getFps();
    char fpsString[6];
    sprintf(fpsString, "%.2f ", fps);
    char* fpsStringOut = concat(" | FPS: ", fpsString);

    char* title = concat(name, fpsStringOut);

    glutSetWindowTitle(title);

    //Zwischenstrings freeen
    free(fpsStringOut);
    fpsStringOut = NULL;
    free(title);
    title = NULL;
}

/**
 * Liesst eine Obj Datei ein.
 * @param filename der Pfad zur Datei
 * @param object Ziel in das das Obj Objekt geladen wird
 */
static void loadObjObject(char* filename, ObjObject* object)
{
    FILE* file;
    file = fopen(filename, "r");
    if (file)
    {
        //Verices und Faces Anzal aus Datei lesen
        if (fscanf(file, "# vertex count = %d\n", &(*object).vertexCount))
        {
            if (fscanf(file, "# face count = %d\n", &(*object).faceCount))
            {
                //Speicher fuer Vertices und Faces reservieren
                (*object).vertices = malloc((*object).vertexCount * sizeof(CGVector3f));
                (*object).faces = malloc((*object).faceCount * sizeof(CGVector3i));
                //Vertices einlesen und min und max abspeichern fuer Bounding Box Ecken
                for (int i = 0; i < (*object).vertexCount; i++)
                {
                    if (!fscanf(file, "v %f %f %f\n", &(*object).vertices[i][X], &(*object).vertices[i][Y],
                        &(*object).vertices[i][Z]))
                    {
                        //Sollte nicht auftreten
                        exit(-1);
                    }
                }
                //Flaechen auslesen und in Objekt schreiben
                for (int i = 0; i < (*object).faceCount; i++)
                {
                    if (!fscanf(file, "f %d %d %d\n", &(*object).faces[i][X], &(*object).faces[i][Y], &(*object).faces[i][Z]))
                    {
                        //Sollte nicht auftreten
                        exit(-1);
                    }
                }
            }
        }
    }
    //Datei nach lesen schliessen
    fclose(file);
}

/**
 * Berechnet die min und max Werte eine Objektes in alle Dimensionen.
 * @param obj Objekt fuer das die Werte berechnet werden
 * @param xMin die linkeste Kante
 * @param xMax die rechteste Kante
 * @param yMin die niedrigste Kante
 * @param yMax die hoechste Kante
 * @param zMin die tiefste Kante
 * @param zMax die naechste Kante
 */
static void calculateMinMax(ObjObject obj, float* xMin, float* xMax, float* yMin, float* yMax, float* zMin, float* zMax)
{
    for (int i = 0; i < obj.vertexCount; i++)
    {
        // Pruefen ob das aktuelle Vertice das kleinste/groesste im Objekt ist
        (*xMin) = obj.vertices[i][X] < (*xMin) ? obj.vertices[i][X] : (*xMin);
        (*yMin) = obj.vertices[i][Y] < (*yMin) ? obj.vertices[i][Y] : (*yMin);
        (*zMin) = obj.vertices[i][Z] < (*zMin) ? obj.vertices[i][Z] : (*zMin);
        (*xMax) = obj.vertices[i][X] > (*xMax) ? obj.vertices[i][X] : (*xMax);
        (*yMax) = obj.vertices[i][Y] > (*yMax) ? obj.vertices[i][Y] : (*yMax);
        (*zMax) = obj.vertices[i][Z] > (*zMax) ? obj.vertices[i][Z] : (*zMax);
    }
}

/**
 * Setzt die Werte der Vertices fuer die Boundig Boxes
 * @param xMin die linkeste Kante
 * @param xMax die rechteste Kante
 * @param yMin die niedrigste Kante
 * @param yMax die hoechste Kante
 * @param zMin die tiefste Kante
 * @param zMax die naechste Kante
 * @param boundingBoxObj die Bounding Box die veraendert werden soll
 */
static void setBoundingBoxObj(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, ObjObject* boundingBoxObj)
{
    // Die gleinsten/groessten Vertices im Objekt fuer Boundingbox speichern
    (*boundingBoxObj).vertices = malloc((*boundingBoxObj).vertexCount * sizeof(CGVector3f));
    (*boundingBoxObj).faces = malloc((*boundingBoxObj).faceCount * sizeof(CGVector3i));
    (*boundingBoxObj).vertices[0][X] = xMin;
    (*boundingBoxObj).vertices[0][Y] = yMin;
    (*boundingBoxObj).vertices[0][Z] = zMax;
    (*boundingBoxObj).vertices[1][X] = xMax;
    (*boundingBoxObj).vertices[1][Y] = yMin;
    (*boundingBoxObj).vertices[1][Z] = zMax;
    (*boundingBoxObj).vertices[2][X] = xMin;
    (*boundingBoxObj).vertices[2][Y] = yMax;
    (*boundingBoxObj).vertices[2][Z] = zMax;
    (*boundingBoxObj).vertices[3][X] = xMax;
    (*boundingBoxObj).vertices[3][Y] = yMax;
    (*boundingBoxObj).vertices[3][Z] = zMax;
    (*boundingBoxObj).vertices[4][X] = xMin;
    (*boundingBoxObj).vertices[4][Y] = yMax;
    (*boundingBoxObj).vertices[4][Z] = zMin;
    (*boundingBoxObj).vertices[5][X] = xMax;
    (*boundingBoxObj).vertices[5][Y] = yMax;
    (*boundingBoxObj).vertices[5][Z] = zMin;
    (*boundingBoxObj).vertices[6][X] = xMin;
    (*boundingBoxObj).vertices[6][Y] = yMin;
    (*boundingBoxObj).vertices[6][Z] = zMin;
    (*boundingBoxObj).vertices[7][X] = xMax;
    (*boundingBoxObj).vertices[7][Y] = yMin;
    (*boundingBoxObj).vertices[7][Z] = zMin;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            (*boundingBoxObj).faces[i][j] = g_cube.faces[i][j];
        }
    }
}

/**
 * Berechnet die AABB eines ObjObjects
 * @param obj Objekt fuer das die Bounding Box erstellt werden soll
 * @param boundingBoxObj Objekt in das die Bounding Box geschrieben wird
 */
static void calculateAABB(ObjObject obj, ObjObject* boundingBoxObj)
{
    /* Minimale und maximale Werte holen */
    float xMax = -FLT_MAX;
    float yMax = -FLT_MAX;
    float zMax = -FLT_MAX;
    float xMin = FLT_MAX;
    float yMin = FLT_MAX;
    float zMin = FLT_MAX;
    calculateMinMax(obj, &xMin, &xMax, &yMin, &yMax, &zMin, &zMax);
    //Bounding Box erstellen
    setBoundingBoxObj(xMin, xMax, yMin, yMax, zMin, zMax, boundingBoxObj);
}

/**
 * Berechnet die OBB zu einem ObjObjekt
 * @param obj Objekt zu dem die Bounding Box erstellt werden soll
 * @param boundingBoxObj Objekt in das die Bounding Box geschrieben wird
 */
static void calculateOBB(ObjObject obj, ObjObject* boundingBoxObj)
{
    /* Holen der maximalen und minimalen Werte */
    float xMax = -FLT_MAX;
    float yMax = -FLT_MAX;
    float zMax = -FLT_MAX;
    float xMin = FLT_MAX;
    float yMin = FLT_MAX;
    float zMin = FLT_MAX;
    calculateMinMax(obj, &xMin, &xMax, &yMin, &yMax, &zMin, &zMax);

    //Mittelpunkt des Objekts in Weltkoordinaten erstellen
    CGVector2f middlePoint = { ((xMax - xMin) / 2.0f) + xMin,
                              ((zMax - zMin) / 2.0f) + zMin };

    float minVolume = FLT_MAX;
    float rotation = 0.0f;
    for (int i = 0; i < 180; i += 5)
    {
        //Erste Projektionslinie
        CGVector2f projectionLine = { 1.0f,
                                     0.0f };
        //Neue Projektionslinie berechnen
        CGVector3f rotatedVector3f = { projectionLine[0], 0.0, projectionLine[1] };
        rotateObject3f(i, 0.0f, 1.0f, 0.0f, rotatedVector3f);
        CGVector2f rotatedVector = { rotatedVector3f[X], rotatedVector3f[Z] };
        //Orthogonale bestimmen um -90! Rotiert also in Richtung z Achse
        CGVector3f orthoProjectionLine3f = { projectionLine[0], 0.0, projectionLine[1] };
        rotateObject3f(i - 90.0f, 0.0f, 1.0f, 0.0f, orthoProjectionLine3f);
        CGVector2f orthoProjectionLine = { orthoProjectionLine3f[X], orthoProjectionLine3f[Z] };
        //Minimale und Maximale Entfernung der Punkte zum Mittelpunkt
        float minDist = FLT_MAX;
        float maxDist = -FLT_MAX;
        float orthoMinDist = FLT_MAX;
        float orthoMaxDist = -FLT_MAX;
        for (int i = 0; i < obj.vertexCount; i++)
        {
            /*-----------------------------------------Projektionslinie------------------------------------------*/
            // Pruefen ob das aktuelle Vertice das kleinste/groesste im Objekt ist
            CGVector2f coordsOfVertex = { obj.vertices[i][X] - middlePoint[0],
                                         obj.vertices[i][Z] - middlePoint[1] };
            float sclrPrdct = coordsOfVertex[0] * rotatedVector[0] + coordsOfVertex[1] * rotatedVector[1];
            //Die minimale und maximale Entfernung mit ihren Punkten zwischenspeichern
            minDist = sclrPrdct < minDist ? sclrPrdct : minDist;
            maxDist = sclrPrdct > maxDist ? sclrPrdct : maxDist;
            /*------------------------------------orthogonale Projektionslinie------------------------------------*/
            sclrPrdct = coordsOfVertex[0] * orthoProjectionLine[0] + coordsOfVertex[1] * orthoProjectionLine[1];
            //Die minimale und maximale Entfernung mit ihren Punkten zwischenspeichern
            orthoMinDist = sclrPrdct < orthoMinDist ? sclrPrdct : orthoMinDist;
            orthoMaxDist = sclrPrdct > orthoMaxDist ? sclrPrdct : orthoMaxDist;
        }
        //Distanz in rotated

        float minMaxDist = maxDist - minDist;
        //Distanz orthogonale
        float orthoMinMaxDist = orthoMaxDist - orthoMinDist;

        float volume = minMaxDist * orthoMinMaxDist;
        //MinVolume ueberschreiben und maximale und minimale Werte ueberschreiben
        if (volume < minVolume)
        {
            minVolume = volume;
            rotation = i;
            xMin = minDist;
            xMax = maxDist;
            zMin = orthoMinDist;
            zMax = orthoMaxDist;
        }
    }
    //BoundingBox generieren und verschieben
    setBoundingBoxObj(xMin, xMax, yMin, yMax, zMin, zMax, boundingBoxObj);
    for (int i = 0; i < (*boundingBoxObj).vertexCount; i++)
    {
        rotateObject3f(rotation, 0.0f, 1.0f, 0.0f, (*boundingBoxObj).vertices[i]);
        translateObject3f(middlePoint[0], 0.0f, middlePoint[1], (*boundingBoxObj).vertices[i]);
    }
}

/**
 * Verschiebt und rotiert den Hasen an die richtige Stelle.
 */
static void moveBunny(void)
{
    for (int i = 0; i < g_bunny.vertexCount; i++)
    {
        //Fuer ersten Hasen
        // rotateObject3f(90.0f, -1.0f, 0.0f, 0.0f, g_bunny.vertices[i]);
        // rotateObject3f(30.0f, 0.0f, -1.0f, 0.0f, g_bunny.vertices[i]);
        // translateObject3f(1.0f, -1.899999f, 0.25f, g_bunny.vertices[i]);
        //Fuer zweiten Hasen
        scaleObject3f(0.5f, 0.5f, 0.5f, g_bunny.vertices[i]);
        rotateObject3f(45.0f, 0.0f, 1.0f, 0.0f, g_bunny.vertices[i]);
        translateObject3f(1.0f, -1.489999f, 0.25f, g_bunny.vertices[i]);
    }
}

/**
 * Verschiebt den Wuerfel an die richtige Stelle.
 */
static void moveCube(void)
{
    for (int i = 0; i < g_cube.vertexCount; i++)
    {
        rotateObject3f(15.0f, 0.0f, -1.0f, 0.0f, g_cube.vertices[i]);
        translateObject3f(-1.0f, -1.489999f, 0.0f, g_cube.vertices[i]);
    }
}

/**
 * Setzt die Materialeigenschaften fuer den Hasen
 * @param obj Objekt fuer das das Material gesetzt wird
 * @param color Farbe
 * @param amb ambienter Anteil
 * @param dif diffuser Anteil
 * @param spec spiegelnder Anteil
 * @param kR Reflexionskonstante
 * @param kT Transmissionskonstante
 */
static void setMaterial(ObjObject* obj, float* color, float amb, float dif, float spec, float kR, float kT)
{
    copyVector((*obj).color, color);
    (*obj).material.materialAmbient = amb;
    (*obj).material.materialDiffuse = dif;
    (*obj).material.materialSpecular = spec;
    (*obj).material.kReflection = kR;
    (*obj).material.kTransmission = kT;
}

/**
 * Liesst alle Obj Dateien fuer die Szene ein, setzt ihre Materialien und schiebt sie zurecht.
 */
static void loadObjObjects(void)
{
    // loadObjObject("src/objects/bunny152v300f.obj", &g_bunny); //Hat die Normalen falschrum :O
    loadObjObject("src/objects/bunny1355v2641f.obj", &g_bunny); //Hoch aufgeloester Hase
    loadObjObject("src/objects/cube8v6f.obj", &g_cube);
    loadObjObject("src/objects/mirror4v2f.obj", &g_mirror);
    loadObjObject("src/objects/walls8v6f.obj", &g_walls);
    //Setzt die Materialien
    CGColor3f darkgrey = { 0.1f, 0.1f, 0.1f };
    CGColor3f white = { WHITE };
    CGColor3f green = { GREEN };
    CGColor3f red = { RED };
    CGColor3f pink = { PINK };
    setMaterial(&g_walls, white, 0.7f, 0.3f, 0.0f, 0.0f, 0.0f);
    setMaterial(&g_mirror, darkgrey, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(&g_bunny, green, 0.2f, 0.8f, 0.0f, 0.0f, 0.0f);
    setMaterial(&g_cube, red, 0.3f, 0.7f, 0.0f, 0.0f, 0.8f);
    setMaterial(&g_boundingBoxOBB, pink, 0.1f, 0.1f, 0.8f, 0.0f, 0.9f);
    //Verschiebt den Hasen und den Cube
    moveBunny();
    moveCube();
    //Berechnen der Bounding Boxes nur initial beim Laden der Szene, da diese statisch
    calculateAABB(g_bunny, &g_boundingBoxAABB);
    calculateOBB(g_bunny, &g_boundingBoxOBB);
}

/**
 * Kummert sich um den Schnitttest mit den Sphaeren, angelehnt an
 * Siehe: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
 * @param currRay der aktuelle Strahl fuer den der Test berechnet wird
 * @param t Distanz zu dem bestimmten Schnittpunkt
 * @param sphere die Spaere mit der schnittpunkt berechnet werden soll
 * @return ob die Sphaere getroffen wurde
 */
GLboolean rayIntersectSphere(Ray currRay, float* t, Sphere sphere)
{
    float radius = sphere.radius;
    CGVector3f center = { 0 };
    copyVector(center, sphere.center);
    float t0;
    float t1;

    //Analytische loesung
    //L = Richtungsvektor zum Origin
    CGVector3f L = { 0 };
    subtractVectos(center, currRay.origin, L);
    //tca = Abstand origin zu mittelpunkt
    float tca = calcDotProduct(L, currRay.direction);
    if (tca < 0.0f)
    {
        return GL_FALSE;
    }
    //Richtungsvektor quadrat - abstand quadrat
    float d2 = calcDotProduct(L, L) - tca * tca;
    if (d2 > (radius * radius))
    {
        return GL_FALSE;
    }
    //Abstand Mittelpunt zu Schnittpunkt
    float thc = sqrt((radius * radius) - d2);
    //t0 = abstand zum eintretenden schnittpunkt
    //t1 = abstand zum austretenden schnittpunkt
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1)
    {
        //tauschen
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    //Wenn orign auf kante des Objektes, dann würde der wert an der Kante als schnittpunkt erkannt werden,
    //und die transmission klappt nicht, da dafuer der 2. punkt als ergebniss benoetigt wird
    if (t0 <= 0.0f + DELTA)
    {
        t0 = t1; // Falls t0 nagativ ist wird t1 als Schnittpunkt genommen
        if (t0 < 0.0f)
        {
            return GL_FALSE; // t0 und t1 negativ -> Schnittpunkt hinter dem Augpunkt, kann verworfen werden
        }
    }

    *t = t0;
    return GL_TRUE;
}

/**
 * Moeller Trumbore Intersection mit ObjObject
 * https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
 * Fuers kommentieren https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
 * @param currRay Aktueller Ray
 * @param t Zeitpunkt der Kollision, welcher in der Methode veraendert wird
 * @param normal Normale, welche in der Methode veraendert wird
 * @param faceCount Anzahl der Flaechen
 * @param vertices Array von Vertices auf die zugegriffen wird
 * @param faces Array von Flaechen auf die zugegriffen wird
 * @param isWall boolean, ob es sich um eine Wand handelt
 * @return true, wenn ein Objekt getroffen wurde
 */
static GLboolean rayIntersectObjObject(Ray currRay, float* t, float* normal, int* faceCount, CGVector3f* vertices, CGVector3i* faces, GLboolean isWall)
{
    //Ob Schnittpunkt existiert
    GLboolean ret = GL_FALSE;
    //min und max t fuer den Kollisionszeitpunkt
    float minT = FLT_MAX;
    float maxT = -FLT_MAX;
    //Schleife ueber alle Flaechen
    for (int i = 0; i < *faceCount; i++)
    {
        /*------------------ aufstellen der Flaeche durch zwei Vektoren ------------------*/
        int v0 = faces[i][0] - 1;
        int v1 = faces[i][1] - 1;
        int v2 = faces[i][2] - 1;
        CGVector3f vertex0 = { vertices[v0][X], vertices[v0][Y], vertices[v0][Z] };
        CGVector3f vertex1 = { vertices[v1][X], vertices[v1][Y], vertices[v1][Z] };
        CGVector3f vertex2 = { vertices[v2][X], vertices[v2][Y], vertices[v2][Z] };
        CGVector3f edge1 = { 0 };
        subtractVectos(vertex1, vertex0, edge1);
        CGVector3f edge2 = { 0 };
        subtractVectos(vertex2, vertex0, edge2);
        /*--------------------------------------------------------------------------------*/
        CGVector3f cross_rayDir_edge2 = { 0 };
        //Determinante berechnen  https://en.wikipedia.org/wiki/Triple_product
        calcCrossProduct(currRay.direction, edge2, cross_rayDir_edge2);
        float det = calcDotProduct(edge1, cross_rayDir_edge2);
        //Wenn Determinante 0 ist,
        //ist die RayDirection in der Dreiecksebene das heisst der Ray ist parallel zum und im Dreieck.
        //(coplanar)
        if (!(det > -DELTA && det < DELTA)) //Test auf ungleich 0
        {
            float invDet = 1.0f / det; //Invertierte Determinante um statt zu Teilen multiplizieren zu koennen
            CGVector3f orig_minus_vert0 = { 0 };
            subtractVectos(currRay.origin, vertex0, orig_minus_vert0);
            /*------------------- Testen, ob der Schnittpunkt ausserhalb des Dreiecks liegt -------------------*/
            /*------------------------------- mithilfe von u, v und w -----------------------------------------*/
            /*------------ u, v und w sind die Koordinaten des Schnittpunktes in der Dreiecksebene ------------*/
            /*------------------------------- addiert muessen sie 1 ergeben -----------------------------------*/
            /*Baryzentrische Koordinaten (u,v)dienen in der linearen Algebra und in der Geometrie dazu, die Lage von Punkten in Bezug auf ein gegebenes Dreieck zu beschreiben*/
            //u = Determinante mit Vektor (vertex0 -> origin) und der Normalen des Dreiecks berechnen
            //Diese Determinante mit der Inversen Determinante multiplizieren
            float baryU = invDet * calcDotProduct(orig_minus_vert0, cross_rayDir_edge2);
            if (!(baryU < 0.0f || baryU > 1.0f)) // wenn negativ dann ausserhalb vom Dreieck
                                                 // u v w addiert muessen 1 ergeben -> u < 1
            {
                //v = Determinante mit Kreuzprodukt mit Vektor (vertex0 -> origin) und der edge1
                //und der RayDirection berechnen
                //Diese Determinante mit der Inversen Determinante multiplizieren
                CGVector3f cross_origMinusVert0_edge1 = { 0 };
                calcCrossProduct(orig_minus_vert0, edge1, cross_origMinusVert0_edge1);
                float baryV = invDet * calcDotProduct(currRay.direction, cross_origMinusVert0_edge1);

                if (!(baryV < 0.0f || baryU + baryV > 1.0f)) //v darf nicht negativ sein
                                                             // u v w addiert muessen 1 ergeben -> muessen u+v < 1 damit w noch berechnet werden kann
                {
                    //Jetzt kann t berechnet werden
                    float t0 = invDet * calcDotProduct(edge2, cross_origMinusVert0_edge1);
                    //Wenn eine Wand getroffen wurde soll der Maximale Wert genommen werden,
                    //sodass nie die vordere Wand "im Weg" ist.
                    if (isWall)
                    {
                        if (t0 > DELTA && t0 > maxT) // ray intersection
                        {
                            maxT = t0;
                            ret = GL_TRUE;
                            //Normale fuer den Kollisionspunkt wird berechnet
                            calcCrossProduct(edge2, edge1, normal);
                        }
                    }
                    //Ansonsten den nahesten Punkt nehmen
                    else if (t0 > DELTA && t0 < minT) // ray intersection
                    {
                        minT = t0;
                        ret = GL_TRUE;
                        //Normale fuer den Kollisionspunkt wird berechnet
                        calcCrossProduct(edge1, edge2, normal);
                    }
                }
            }
        }
    }
    //Wenn Kollision vorhanden, dann t setzen
    if (ret)
    {
        if (isWall)
        {
            *t = maxT;
        }
        else
        {
            *t = minT;
        }
    }
    return ret;
}

/**
 * Prueft, ob ein Objekt von einem Ray getroffen wird
 * @param currRay der Ray
 * @param minT minimale Distanz zum getroffenen Objekt
 * @param normal die Normale der getroffenen Flaeche
 * @return ein Enum Wert des Objektes, welches getroffen wurde
 */
static SceneObject checkIntersectionWithObjects(Ray currRay, float* minT, float* normal)
{
    float t = 0;
    SceneObject closestObject = objectNone;
    CGVector3f tempNormal = { 0 };
    // //Kugel intersection
    if (rayIntersectSphere(currRay, &t, g_sphere))
    {
        if (t < *minT)
            *minT = t;
        closestObject = objectSphere;
        CGVector3f currRayPos = { 0 };
        multiplyVectorWithScalar(currRay.direction, *minT, currRayPos);
        addVectors(currRayPos, currRay.origin, currRayPos);
        subtractVectos(currRayPos, g_sphere.center, normal);
        normalizeVector(normal);
    }

    //Cube intersection
    if (rayIntersectObjObject(currRay, &t, tempNormal, &g_cube.faceCount, g_cube.vertices, g_cube.faces, GL_FALSE))
    {
        if (t < *minT)
        {
            *minT = t;
            closestObject = objectCube;
            copyVector(normal, tempNormal);
        }
    }

    // //Bunny intersection
    BoundingBoxType currBoundingBoxType = getBoundingBoxStatus();
    ObjObject boundingBoxObj = { 0 };
    // Pruefen welche BoundingBox eingestellt
    switch (currBoundingBoxType)
    {
    case boundingBoxAABB:
        boundingBoxObj = g_boundingBoxAABB;
        break;
    case boundingBoxOBB:
        boundingBoxObj = g_boundingBoxOBB;
        break;
    default:
        break;
    }

    //Falls eine Boounding Box ausgewaehlt
    GLboolean boundingBoxHit = GL_TRUE;
    //Pruefe die Bounding Box
    if (currBoundingBoxType != boundingBoxNone)
    {
        boundingBoxHit = rayIntersectObjObject(currRay, &t, tempNormal, &boundingBoxObj.faceCount, boundingBoxObj.vertices, boundingBoxObj.faces, GL_FALSE);
        GLboolean drawBoundingBox = getDrawBoundingBoxStatus();
        //Wenn die Bounding Box getroffen wurde und sie gezeichnet werden soll
        if (boundingBoxHit && drawBoundingBox && t < *minT)
        {
            *minT = t;
            closestObject = objectBoundingBoxBunny;
            copyVector(normal, tempNormal);
        }
    }
    //Pruefe den Hasen wenn BoundingBox getroffen wurde
    if (boundingBoxHit)
    {
        if (rayIntersectObjObject(currRay, &t, tempNormal, &g_bunny.faceCount, g_bunny.vertices, g_bunny.faces, GL_FALSE))
        {
            if (t < *minT)
            {
                *minT = t;
                closestObject = objectBunny;
                copyVector(normal, tempNormal);
            }
        }
    }

    //Mirror intersection
    if (rayIntersectObjObject(currRay, &t, tempNormal, &g_mirror.faceCount, g_mirror.vertices, g_mirror.faces, GL_FALSE))
    {
        if (t < *minT)
        {
            *minT = t;
            closestObject = objectMirror;
            copyVector(normal, tempNormal);
        }
    }

    //Wand intersection
    if (rayIntersectObjObject(currRay, &t, tempNormal, &g_walls.faceCount, g_walls.vertices, g_walls.faces, GL_TRUE))
    {
        if (t < *minT)
        {
            *minT = t;
            closestObject = objectWall;
            copyVector(normal, tempNormal);
        }
    }
    return closestObject;
}

/**
 * Berechnet die lokale Beleuchtung an einem Objektpunkt
 * @param color Farbe des Objekts
 * @param intersectionPos die Koordinaten in Weltkoordinaten an der die lokale Beleuchtung berechnet werden soll
 * @param normal die Normnale des Objekts an dem Punkt
 * @param ray der Strahl der das Objekt getroffen hat
 * @param framebufferValue der berechnete Ergebnisswert (out)
 * @param material Materialeigenschaften des Objekts
 */
void shading(CGColor3f color, CGVector3f intersectionPos, CGVector3f normal, Ray ray, CGColor3f framebufferValue,
    MaterialProperties material)
{

    if (g_numLightsStartIndex == g_numLightsInScene)
    {
        //AMBIENT Anteil wenn keine Lichtquelen
        CGColor3f white = { WHITE };
        multiplyVectorWithScalar(white, AMBIENT_LIGHT_INTENSITY, framebufferValue);
        multiplyVectors(color, framebufferValue, framebufferValue);
    }
    else
    { //Ueber alle Lichtquellen laufen
        for (int i = g_numLightsStartIndex; i < g_numLightsInScene; i++)
        {
            CGColor3f lightColor = { 0 };
            copyVector(lightColor, g_lightPos[i].color);

            //Berechne Vekor von Punkt auf Obj zur Lichtquelle
            CGVector3f lightVector = { 0 };
            subtractVectos(g_lightPos[i].center, intersectionPos, lightVector);
            float distanceLightIntersectionPos = calcVectorLength(lightVector);
            normalizeVector(lightVector);

            //AMBIENT

            CGColor3f ambientComponent = { 0 };
            multiplyVectorWithScalar(lightColor, AMBIENT_LIGHT_INTENSITY, ambientComponent);

            //DIFFUSE

            //Skalarprodukt zwischen Normalisiertem Vector vom Pixel zur Lichtquelle
            //und der Normale des Pixels bilden und nur den Positiven Teil beruecksichtigen (0 - 1) da cos auch neg. bei Winkeln >90
            float diffuseIntensity = clip(calcDotProduct(normal, lightVector), 0.0f, 1.0f);
            CGColor3f diffuseComponent = { 0 };
            multiplyVectorWithScalar(lightColor, diffuseIntensity, diffuseComponent);

            //SPEKULAR

            CGVector3f reflectedLightVector = { 0 };
            reflectVector(lightVector, normal, reflectedLightVector);
            normalizeVector(reflectedLightVector);
            CGVector3f fromIntersectionToRayOrigin = { 0 };
            subtractVectos(ray.origin, intersectionPos, fromIntersectionToRayOrigin);
            normalizeVector(fromIntersectionToRayOrigin);
            //Max ->  damit wir bei negativen Werten des Winkels (cos >90) immernoch 0 haben
            float angleToRayOriginAndReflected = fmax(calcDotProduct(fromIntersectionToRayOrigin, reflectedLightVector), 0.0f);
            //Je groesser der Exponent, desto kleiner der Reflektionspunkt/shininess wird hoeher
            float specularConstant = pow(angleToRayOriginAndReflected, SHININESS);
            CGColor3f specularComponent = { 0 };
            multiplyVectorWithScalar(lightColor, specularConstant, specularComponent);

            //DAEMPFUNG abhaengig von der Entfernung zum Licht
            //ax^2+bx+c -> x ist die distanz
            float attenuation = 1.0f / (CONSTANT_ATTENUATION + LINEAR_ATTENUATION * distanceLightIntersectionPos +
                QUADRIC_ATTENUATION * distanceLightIntersectionPos * distanceLightIntersectionPos);

            //PHONG ANWENDEN
            CGColor3f temp = { 0 };
            CGColor3f tempColor = { 0 };
            multiplyVectorWithScalar(diffuseComponent, material.materialDiffuse, temp);
            addVectors(tempColor, temp, tempColor);

            multiplyVectorWithScalar(specularComponent, material.materialSpecular, temp);
            addVectors(tempColor, temp, tempColor);

            multiplyVectorWithScalar(ambientComponent, material.materialAmbient, temp);
            addVectors(tempColor, temp, tempColor);

            multiplyVectors(color, tempColor, tempColor);

            multiplyVectorWithScalar(tempColor, attenuation, tempColor);

            addVectors(tempColor, framebufferValue, framebufferValue);
        }
    }
}

/**
 * Berechnet mithife von Raytracing den Farbwert an dem Pixel
 * @param currRay der Aktuell zu verfolgende Strahl
 * @param framebufferValue der zu beschreibende Farbwert des Framebuffers
 * @param recursionDepth die aktuelle rekursionstiefe
 * @param distanceTravelled die zurueckgelegte Distanz des aktuell verfolgten Strahls
 */
void traceRay(Ray currRay, CGColor3f framebufferValue, int recursionDepth, float distanceTravelled)
{
    if (recursionDepth < MAX_RECURSION_DEPTH)
    {
        //Rekursion erhoehen
        recursionDepth++;
        //Notwendige Variablen erstellen
        float minT = FLT_MAX;
        CGVector3f normal = { 0 };

        //Kollision mit Objekten pruefen
        SceneObject intersectedObject = checkIntersectionWithObjects(currRay, &minT, normal);

        //Je nach kollidiertem Objekt Farbe und Material setzen
        if (intersectedObject == objectNone)
        {
            copyVector(framebufferValue, g_backgroundColor);
        }
        else
        {
            //Kollisionspunkt in Weltkoord.
            CGVector3f currRayPos = { 0 };
            multiplyVectorWithScalar(currRay.direction, minT, currRayPos);
            addVectors(currRayPos, currRay.origin, currRayPos);
            //Farb und Material variable erstellen
            CGVector3f hitObjectColor = { 0 };
            MaterialProperties hitObjectMaterial = { 0 };
            //Normale normalisieren
            normalizeVector(normal);
            switch (intersectedObject)
            {
                //Kugel
            case objectSphere:
                copyVector(hitObjectColor, g_sphere.color);
                copyMaterial(&hitObjectMaterial, &g_sphere.material);
                break;
                //Wuerfel
            case objectCube:
                copyVector(hitObjectColor, g_cube.color);
                copyMaterial(&hitObjectMaterial, &g_cube.material);
                break;
                //Der Hase
            case objectBunny:
                copyVector(hitObjectColor, g_bunny.color);
                copyMaterial(&hitObjectMaterial, &g_bunny.material);
                break;
                //Bunny Hitbox
            case objectBoundingBoxBunny:
                copyVector(hitObjectColor, g_boundingBoxOBB.color);
                copyMaterial(&hitObjectMaterial, &g_boundingBoxOBB.material);
                break;
                //Spiegel
            case objectMirror:
                copyVector(hitObjectColor, g_mirror.color);
                copyMaterial(&hitObjectMaterial, &g_mirror.material);
                break;
                //Wand
            case objectWall:
                copyVector(hitObjectColor, g_walls.color);
                copyMaterial(&hitObjectMaterial, &g_walls.material);
                break;
                //Hintergrundfarbe in den Framebuffer schreiben
            default:
                break;
            }
            CGColor3f shadeFrameBufferValue = { 0 };
            //In das shading den wert mitgeben wie weit der strahl bis jetzt geflogen ist
            shading(hitObjectColor, currRayPos, normal, currRay, shadeFrameBufferValue, hitObjectMaterial);

            //DAEMPFUNG
            //ax^2+bx+c -> x ist die distanz
            distanceTravelled += minT;

            float attenuation = 1.0f / (CONSTANT_ATTENUATION + LINEAR_ATTENUATION * distanceTravelled +
                QUADRIC_ATTENUATION * distanceTravelled * distanceTravelled);
            //Daemfung in abhaengigkeit von der zurueckgelegten Distanz des aktuell betrachteten Strahls berechnen
            multiplyVectorWithScalar(shadeFrameBufferValue, attenuation, shadeFrameBufferValue);

            //Falls Intensitaetswert zu niedrig ist, wird abgebrochen(Keine weitere Rekursion angestossen)
            float frameIntensity = (shadeFrameBufferValue[X] + shadeFrameBufferValue[Y] + shadeFrameBufferValue[Z]) / 3.0f;
            if (frameIntensity > DELTA)
            {
                //Die lokale BRDF (Shading) in den Framebuffer schreiben fuer den aktuellen Abschnitt des Strahls
                copyVector(framebufferValue, shadeFrameBufferValue);

                //Reflexion
                if (hitObjectMaterial.kReflection > 0.0f)
                {
                    Ray reflectedRay = { 0 };
                    //Origin des reflektierten Strahls auf currRayPos setzen (Kollisionspunkt)
                    copyVector(reflectedRay.origin, currRayPos);
                    CGVector3f currRayPosToOrigin = { 0 };
                    //Direction Vektor umdrehen -> Vektor muss fuer das Reflektieren vom Kollisionspunkt ausgehen
                    multiplyVectorWithScalar(currRay.direction, -1.0f, currRayPosToOrigin);
                    //Vektor reflektieren fuer neue RayDirection
                    reflectVector(currRayPosToOrigin, normal, reflectedRay.direction);
                    normalizeVector(reflectedRay.direction);
                    CGColor3f reflectFrameBufferValue = { 0 };
                    //Den von dem getroffenem Objekt ausgehenden reflektieren Ray schiessen
                    traceRay(reflectedRay, reflectFrameBufferValue, recursionDepth, distanceTravelled);
                    //Reflektionsfaktor draufrechnen
                    multiplyVectorWithScalar(reflectFrameBufferValue, hitObjectMaterial.kReflection, reflectFrameBufferValue);
                    addVectors(framebufferValue, reflectFrameBufferValue, framebufferValue);
                }

                //Transmittierenden Strahl schiessen
                if (hitObjectMaterial.kTransmission > 0.0f)
                {
                    Ray transmittedRay = { 0 };
                    //Origin des reflektierten Strahls auf currRayOis setzen (Kollisionspunkt)
                    copyVector(transmittedRay.origin, currRayPos);
                    //Transmittierender Strahl wird nicht abgelenkt, da selbe Dichte angenommen
                    copyVector(transmittedRay.direction, currRay.direction);
                    CGColor3f transmittedFrameBufferValue = { 0 };
                    ///Den von dem getroffenem Objekt ausgehenden transmittierenden Ray schiessen
                    traceRay(transmittedRay, transmittedFrameBufferValue, recursionDepth, distanceTravelled);
                    multiplyVectorWithScalar(transmittedFrameBufferValue, hitObjectMaterial.kTransmission,
                        transmittedFrameBufferValue);
                    //Transmittierenden und Reflekierenden Ray addieren und als ergebniss in framebufferValue schreiben
                    addVectors(framebufferValue, transmittedFrameBufferValue, framebufferValue);
                }
                //Shadow
                for (int i = g_numLightsStartIndex; i < g_numLightsInScene; i++)
                {
                    Ray shadowRay = { 0 };
                    //Origin des schatten Strahls auf currRayPos setzen (Kollisionspunkt)
                    copyVector(shadowRay.origin, currRayPos);
                    //Direction zum LichtPunkt
                    subtractVectos(g_lightPos[i].center, shadowRay.origin, shadowRay.direction);
                    float distanceToLightSource = calcVectorLength(shadowRay.direction);
                    normalizeVector(shadowRay.direction);
                    ///Den von dem getroffenem Objekt ausgehenden transmittierenden Ray schiessen
                    minT = FLT_MAX;
                    SceneObject shadowIntersectedObject = objectNone;
                    shadowIntersectedObject = checkIntersectionWithObjects(shadowRay, &minT, normal);
                    if (shadowIntersectedObject != objectNone && minT < distanceToLightSource)
                    {
                        multiplyVectorWithScalar(framebufferValue, 0.8f, framebufferValue);
                    }
                }
            }
        }
    }
}


static GLboolean isCameraMoving(void)
{
    //Fuer geringere Aufloesung wenn die Kamera bewegt wird
    Movement movement = getCamMovementStatus();
    Radius radiusMovement = getRadiusStatus();
    return (movement != moveNone) || (radiusMovement != radiusNone);
}

/**
 * Zeichnet die Szene
 * @param width die breite des Fenster in Pixeln
 * @param height die hoehe des Fenster in Pixeln
 */
void drawScene(GLint width, GLint height)
{
    drawInfoInWindowTitle();

    if (getHelpStatus())
    {
        drawHelp();
        toggleHelp();
    }

    CGVector3f cameraPos = { 0 };
    calculateCameraPosition(cameraPos);
    //Up Vektor
    CGVector3f v = { 0 };
    //Up Vektor ausgehend von der aktuellen Kameraposition
    calculateUpVector(v);
    normalizeVector(v);
    //In richtung Ursprung
    CGVector3f lookVector = { -cameraPos[X], -cameraPos[Y], -cameraPos[Z] };
    normalizeVector(lookVector);
    CGVector3f u = { 0 };
    //u = look x v (up-vec)
    calcCrossProduct(lookVector, v, u);
    //Abstand zwischen Augpunkt und Projektionsebene nach trigonometrie
    float d = CW / (2.0f * tan(degreeToRad(FOV / 2.0f)));
    float aspect = (float)width / height;

    //Ortsvektor der unteren linken Ecke der Projektionsflaeche
    CGVector3f s = { 0 };
    calculateProjectionBaseVector(cameraPos, lookVector, d, u, v, aspect, s);

    //Abstaende zwischen den Pixeln in world space berechnen
    float pixelSize = CW / (float)width;

    CGVector3f deltaU = { 0 };
    CGVector3f deltaV = { 0 };
    multiplyVectorWithScalar(u, pixelSize, deltaU);
    multiplyVectorWithScalar(v, pixelSize, deltaV);

    //Neu rendern wenn sich bewegt wurde, oder Bounding-Box angezeigt werden soll
    if (isCameraMoving())
    {
        g_rendered = GL_FALSE;
        g_firstRenderAfterMoveCount = 0;
    }
    //Neu alloziieren
    if (!g_rendered)
    {
        g_framebuffer = calloc(height * width, sizeof(CGColor3f));
    }

    if (g_framebuffer != NULL)
    {
        if (!g_rendered || g_firstRenderAfterMoveCount == 0)
        {
            //Framebuffer berechnen mittels Rays die in die Scene geschossen werden
            //Zeilenweise laufen
            for (int j = 0; j < height; j = isCameraMoving() ? j + SKIP_PIXEL_COUNT : j + 1)
            {
                for (int i = 0; i < width; i = isCameraMoving() ? i + SKIP_PIXEL_COUNT : i + 1)
                {
                    CGVector3f tempU = { 0 };
                    CGVector3f tempV = { 0 };

                    multiplyVectorWithScalar(deltaU, (0.5f + i), tempU);
                    multiplyVectorWithScalar(deltaV, (0.5f + j), tempV);

                    //Aktuellen Punkt in der Projektionsflaeche berechnen
                    CGVector3f pixelOnPlane = { 0 };
                    addVectors(s, tempU, pixelOnPlane);
                    addVectors(pixelOnPlane, tempV, pixelOnPlane);

                    //Stahl erzeugen der duch den akuellen Punkt laeuft
                    Ray currRay = { 0 };
                    copyVector(currRay.origin, cameraPos);
                    subtractVectos(pixelOnPlane, cameraPos, currRay.direction);
                    float distanceCamToPixel = calcVectorLength(currRay.direction);
                    normalizeVector(currRay.direction);

                    int idx = i * height + j;
                    if (idx >= width * height)
                    {
                        fprintf(stderr, "FRAMEBUFFERZUGRIFF ERROR i:%d j:%d\n", i, j);
                        exit(-1);
                    }
                    //Raytracing
                    traceRay(currRay, g_framebuffer[j * width + i], 0, 0.0f);
                    //Daempft die Farbwerte, welche weiter am Rand liegen.
                    if (g_vignette)
                    {
                        multiplyVectorWithScalar(g_framebuffer[j * width + i],
                            (1.0f / ((distanceCamToPixel * distanceCamToPixel * distanceCamToPixel) * 7.0f)),
                            g_framebuffer[j * width + i]);
                    }
                }
            }
            g_rendered = GL_TRUE;
            if (!isCameraMoving())
            {
                g_firstRenderAfterMoveCount++;
            }
        }
        glDrawPixels(width, height, GL_RGB, GL_FLOAT, g_framebuffer);
    }
    else
    {
        fprintf(stderr, "KEIN RAM VERFUEGBAR\n");
        exit(1);
    }
}

/**
 * Reserviert den Speicher fuer den Framebuffer
 * @param width Breite des Bildschirmes
 * @param height Hoehe des Bildschirmes
 */
static void initFrameBuffer(GLint width, GLint height)
{
    g_framebuffer = malloc(height * width * sizeof(CGColor3f));
    if (g_framebuffer == NULL)
    {
        fprintf(stderr, "KEIN RAM VERFUEGBAR\n");
        exit(1);
    }
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(GLint width, GLint height)
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

    /* Linienbreite */

    glLineWidth(1.0f);

    //Laedt die Obj Dateien
    loadObjObjects();

    //initialisiert den Frambuffer
    initFrameBuffer(width, height);

    return (glGetError() == GL_NO_ERROR);
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
GLboolean getLight0Status(void)
{
    return g_light0Status;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0Status(GLboolean status)
{
    g_light0Status = status;
}

/**
 * Liefert den Status der zweiten Lichtquelle.
 * @return Status der zweiten Lichtquelle (an/aus).
 */
GLboolean getLight1Status(void)
{
    return g_light1Status;
}

/**
 * Setzt den Status der zweiten Lichtquelle.
 * @param status Status der zweiten Lichtquelle (an/aus).
 */
void setLight1Status(GLboolean status)
{
    g_light1Status = status;
}

/**
 * Toggled den Vignette Status.
 */
void toggleG_Vignette(void)
{
    g_vignette = !g_vignette;
    g_rendered = GL_FALSE;
}

/**
 * Ändert die Anzahl an Lichtquellen.
 */
void toggleG_numLightsStartIndex(void)
{
    g_numLightsStartIndex = (g_numLightsStartIndex + 1) % 3;
    g_rendered = GL_FALSE;
}

/**
 * Setzt den rendered Status auf false sodass neu gezeichnet wird.
 */
void setG_rendered(GLboolean val)
{
    g_rendered = val;
}

/**
 * Gibt den Spiecher fuer den Framebuffer frei
 */
void freeFrameBuffer(void)
{
    free(g_framebuffer);
}
