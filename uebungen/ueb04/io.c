/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Michael Smirnov & Len Harmsen
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glut.h>

#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "logic.h"
#include "scene.h"
#include "debugGL.h"
#include "util.h"
#include "texture.h"

/* Funktionen */

static void moveCam(void);

static void handleRadius(void);

/* ---- Globale Variablen ---- */

Movement g_camMovementStatus = moveNone;

Radius g_radiusStatus = radiusNone;

GLboolean g_firstPersonCam = GL_FALSE;

GLboolean g_togglePause = GL_FALSE;

GLboolean g_drawHelp = GL_FALSE;

GLboolean g_particleLine = GL_FALSE;

GLboolean g_drawAcceleration = GL_FALSE;

GLboolean g_drawVelocity = GL_FALSE;

GLboolean g_drawUpVector = GL_FALSE;

GLboolean g_shadows = GL_FALSE;

GLboolean g_ballMovement = GL_FALSE;

int g_visualHelp = 0;

int g_textureIdx = 0;

/* Kamera Stuff */

float g_phi = INIT_PHI;
float g_theta = INIT_THETA;
float g_radius = INIT_CAMERA_RADIUS;

GLfloat g_virtualCameraMatrix[16] = {0};

GLfloat g_fps = 0.0f;

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60
/* ---- Funktionen ---- */

/**
 * Liefert Winkel Phi der Kamera
 * @return der Winkel Phi
 */
float getPhi(void)
{
    return g_phi;
}

/**
 * Liefert den Winkel Theta der Kamera
 * @return der Winkel Theta
 */
float getTheta(void)
{
    return g_theta;
}

/**
 * Liefert den Radius fuer die Kameraberechnung
 * @return der Radius
 */
float getRadius(void)
{
    return g_radius;
}

/**
 * Erhoeht den Index der Textur
 */
void increaseTextureIdx(void)
{
    g_textureIdx = (g_textureIdx + 1) % 2;
    bindTexture(g_textureIdx);
}

/** Setzt den toggle Status */
static void
toggleGamePause(void)
{
    g_togglePause = !g_togglePause;
}

/**
 * Toggelt ob der Hilfetext angezeigt werden soll oder nicht.
 */
static void toggleHelp()
{
    g_drawHelp = !g_drawHelp;
}

/**
 * Toggelt ob das Partikel als Linie oder als Dreieck dargestellt wird. 
 */
static void toggleParticleAppearance()
{
    g_particleLine = !g_particleLine;
}

/**
 * Toggelt die Schatten der Partikel. 
 */
static void toggleShadows()
{
    g_shadows = !g_shadows;
}

/**
 * Toggelt ob die Beschleunnigung der Partikel gezeichnet werden soll.
 */
static void toggleAcceleration()
{
    g_drawAcceleration = !g_drawAcceleration;
}

/**
 * Toggelt ob die Geschwindigkeit der Partikel gezeichnet werden soll.
 */
static void toggleVelocity()
{
    g_drawVelocity = !g_drawVelocity;
}

/**
 * Toggelt ob der Up-Vektor der Partikel gezeichnet werden soll.
 */
static void toggleUpVector()
{
    g_drawUpVector = !g_drawUpVector;
}

/**
 * Liefert den Status ob die Schatten gezeichnet werden sollen.
 * @return Schatten Status (Wahrheitswert)
 */
GLboolean getDrawShadows(void)
{
    return g_shadows;
}

/**
 * Liefert den Status ob das Partikel als Linie gezeichent werden soll oder nicht
 * @return den Wahrheitswert.
 */
GLboolean getParticleLineStatus(void)
{
    return g_particleLine;
}

/**
 * Liefert den Status ob der Geschwindikeitsvektor des Partikels als Linie gezeichent werden soll oder nicht
 * @return den Wahrheitswert.
 */
GLboolean getDrawParticleVelocityStatus(void)
{
    return g_drawVelocity;
}

/**
 * Liefert den Status ob die Baelle bewegt werden sollen oder nicht
 * @return den Wahrheitswert.
 */
GLboolean getBallMovementStatus(void)
{
    return g_ballMovement;
}

/**
 * Liefert den Status ob der Up-Vektor des Partikels als Linie gezeichent werden soll oder nicht
 * @return den Wahrheitswert.
 */
GLboolean getDrawParticleUpVectorStatus(void)
{
    return g_drawUpVector;
}

/**
 * Liefert den Status ob die Geschwindigkeit des Partikels als Linie gezeichent werden soll oder nicht
 * @return den Wahrheitswert.
 */
GLboolean getDrawParticleAccelerationStatus(void)
{
    return g_drawAcceleration;
}

/**
 * Liefert den Status ob die Hilfe gezeichnet werden soll oder nicht
 * @return den Status
 */
GLboolean getHelpStatus()
{
    return g_drawHelp;
}

/**
 * Getter fuer Visuelle Hilfe
 * @return ob Hilfe angezeigt werden soll
 */
int getVisualHelp()
{
    return g_visualHelp;
}

/**
 * Liefert die aktuellen FPS
* @return die aktuellen FPS
 */
float getFps()
{
    return g_fps;
}

/**
 * Berechnung der Frames pro Sekunde.
 * @return aktuelle Framerate.
 */
static float
frameRate(void)
{
    /* Zeitpunkt der letzten Neuberechnung */
    static int timebase = 0;
    /* Anzahl der Aufrufe seit letzter Neuberechnung */
    static int frameCount = 0;
    /* Zuletzt berechneter FPS-Wert */
    static float fps = 0.0f;
    /* aktuelle "Zeit" */
    static int time = 0;

    /* Diesen Aufruf hinzuzaehlen */
    frameCount++;

    /* seit dem Start von GLUT vergangene Zeit ermitteln */
    time = glutGet(GLUT_ELAPSED_TIME);

    /* Eine Sekunde ist vergangen */
    if (time - timebase > 1000)
    {
        /* FPS-Wert neu berechnen */
        fps = frameCount * 1000.0f / (time - timebase);

        /* Zureuecksetzen */
        timebase = time;
        frameCount = 0;
    }

    /* Aktuellen FPS-Wert zurueckgeben */
    return fps;
}

/**
 * Umschalten zw. Vollbild- und Fenstermodus.
 * Beim Wechsel zum Fenstermodus wird vorherige Position und Groesse
 * wiederhergestellt. HINWEIS: Fenster wird nicht korrekt auf alte Position
 * gesetzt, da GLUT_WINDOW_WIDTH/HEIGHT verfaelschte Werte liefert.
 */
static void
toggleFullscreen(void)
{
    /* Flag: Fullscreen: ja/nein */
    static GLboolean fullscreen = GL_FALSE;
    /* Zwischenspeicher: Fensterposition */
    static CGPoint2i windowPos;
    /* Zwischenspeicher: Fenstergroesse */
    static CGDimensions2i windowSize;

    /* Modus wechseln */
    fullscreen = !fullscreen;

    if (fullscreen)
    {
        /* Fenstereinstellungen speichern */
        windowPos[0] = glutGet(GLUT_WINDOW_X);
        windowPos[1] = glutGet(GLUT_WINDOW_Y);
        windowSize[0] = glutGet(GLUT_WINDOW_WIDTH);
        windowSize[1] = glutGet(GLUT_WINDOW_HEIGHT);
        /* In den Fullscreen-Modus wechseln */
        glutFullScreen();
    }
    else
    {
        /* alte Fenstereinstellungen wiederherstellen */
        glutReshapeWindow(windowSize[0], windowSize[1]);

        /* HINWEIS:
         Auskommentiert, da es sonst Probleme mit der Vollbildarstellung bei
         Verwendung von FreeGlut gibt */
        glutPositionWindow(windowPos[0], windowPos[1]);
    }
}

/**
 * Debug-Ausgabe eines Tasturereignisses.
 * Ausgabe der Taste, die das Ereignis ausgeloest hat, des Status der Modifier
 * und der Position des Mauszeigers.
 * Die Ausgabe erfolgt nur, wenn das Makro DEBUG definiert ist.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
debugPrintKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
                        int y)
{
    (void)isSpecialKey;
    (void)key;
    (void)status;
    (void)x;
    (void)y;
#ifdef DEBUG
    /* Bitmask mit Status der Modifier-Tasten */
    int modifierMask = glutGetModifiers();

    /* Spezialtaste */
    if (isSpecialKey)
    {
        printf("Spezialtaste");

        switch (key)
        {
        case GLUT_KEY_F1:
            printf(" F1");
            break;
        case GLUT_KEY_F2:
            printf(" F2");
            break;
        case GLUT_KEY_F3:
            printf(" F3");
            break;
        case GLUT_KEY_F4:
            printf(" F4");
            break;
        case GLUT_KEY_F5:
            printf(" F5");
            break;
        case GLUT_KEY_F6:
            printf(" F6");
            break;
        case GLUT_KEY_F7:
            printf(" F7");
            break;
        case GLUT_KEY_F8:
            printf(" F8");
            break;
        case GLUT_KEY_F9:
            printf(" F9");
            break;
        case GLUT_KEY_F10:
            printf(" F10");
            break;
        case GLUT_KEY_F11:
            printf(" F11");
            break;
        case GLUT_KEY_F12:
            printf(" F12");
            break;
        case GLUT_KEY_LEFT:
            printf(" LEFT");
            break;
        case GLUT_KEY_UP:
            printf(" UP");
            break;
        case GLUT_KEY_RIGHT:
            printf(" RIGHT");
            break;
        case GLUT_KEY_DOWN:
            printf(" DOWN");
            break;
        case GLUT_KEY_PAGE_UP:
            printf(" PAGE_UP");
            break;
        case GLUT_KEY_PAGE_DOWN:
            printf(" PAGE_DOWN");
            break;
        case GLUT_KEY_HOME:
            printf(" HOME");
            break;
        case GLUT_KEY_END:
            printf(" END");
            break;
        case GLUT_KEY_INSERT:
            printf(" INSERT");
            break;
        }
    }
    /* keine Spezialtaste */
    else
    {
        printf("Taste %c (ASCII-Code: %u)", key, key);
    }

    /* Status ausgeben */
    if (status == GLUT_DOWN)
    {
        printf(" gedrueckt.\n");
    }
    else
    {
        printf(" losgelassen.\n");
    }

    /* Status der Modifier ausgeben */
    printf("  Status der Shift-Taste: %d, Status der Strg-Taste: %d, Status der Alt-Taste: %d\n",
           modifierMask & GLUT_ACTIVE_SHIFT, (modifierMask & GLUT_ACTIVE_CTRL) >> 1,
           (modifierMask & GLUT_ACTIVE_ALT) >> 2);

    printf("  Position der Maus: (%d,%d)\n\n", x, y);

#endif
}

/**
 * Setzt einen Viewport für 3-dimensionale Darstellung
 * mit perspektivischer Projektion und legt eine Kamera fest.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 * 
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Höhe des Viewports
 * @param lookAt enthält die für glLookAt benötigten Daten zur Kamera (Augpunkt, Zentrum, Up-Vektor)
 */
static void
set3DViewport(GLint x, GLint y, GLint width, GLint height, GLdouble lookAt[9])
{
    /* Seitenverhältnis bestimmen */
    double aspect = (double)width / height;

    /* Folge Operationen beeinflussen die Projektionsmatrix */
    glMatrixMode(GL_PROJECTION);

    /* Einheitsmatrix laden */
    glLoadIdentity();

    /* Viewport-Position und -Ausdehnung bestimmen */
    glViewport(x, y, width, height);

    /* Perspektivische Darstellung */
    gluPerspective(40,     /* Öffnungswinkel */
                   aspect, /* Seitenverhältnis */
                   0.1,    /* nahe Clipping-Ebene */
                   200);   /* ferne Clipping-Ebene */

    /* Folge Operationen beeinflussen die Modelviewmatrix */
    glMatrixMode(GL_MODELVIEW);

    /* Einheitsmatrix laden */
    glLoadIdentity();

    /* Kameraposition */
    gluLookAt(lookAt[0], lookAt[1], lookAt[2],  /* Augpunkt */
              lookAt[3], lookAt[4], lookAt[5],  /* Zentrum */
              lookAt[6], lookAt[7], lookAt[8]); /* Up-Vektor */

    /*Aktuelle View Matrix speichern fuer spaetere Berechnung von Weltkoodinaten von obj.*/
    glGetFloatv(GL_MODELVIEW_MATRIX, g_virtualCameraMatrix);

    /* Szene zeichnen lassen in 3D */

    drawScene();
}

/**
 * Setzt einen Viewport für 2-dimensionale Darstellung.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 *
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Höhe des Viewports
 */
static void
set2DViewport(GLint x, GLint y, GLint width, GLint height)
{
    /* Seitenverhältnis bestimmen */
    double aspect = (double)width / height;

    /* Folge Operationen beeinflussen die Projektionsmatrix */
    glMatrixMode(GL_PROJECTION);

    /* Einheitsmatrix laden */
    glLoadIdentity();

    /* Viewport-Position und -Ausdehnung bestimmen */
    glViewport(x, y, width, height);

    /* Koordinatensystem bleibt quadratisch */
    if (aspect <= 1)
    {
        gluOrtho2D(-1.0, 1.0,                    /* links, rechts */
                   -1.0 / aspect, 1.0 / aspect); /* unten, oben */
    }
    else
    {
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, /* links, rechts */
                   -1.0, 1.0);                  /* unten, oben */
    }

    /* Folge Operationen beeinflussen die Modelviewmatrix */
    glMatrixMode(GL_MODELVIEW);

    /* Einheitsmatrix laden */
    glLoadIdentity();

    /* Szene zeichnen lassen in 2D*/
    drawScene();
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
    /* Seit dem Programmstart vergangene Zeit in Millisekunden */
    int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

    /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
    double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;

    /* Pausiert das Spiel */
    if (g_togglePause || g_drawHelp)
    {
        interval = 0.0;
    }

    /*Kamerabewegung durch Tastendruecke*/
    moveCam();
    /*Kameraradius durch Tastendruecke*/
    handleRadius();

    /*Kuemmert sich um die Berechnungen in der Logik (zeitgesteuert)*/
    handleLogicCalculations(interval);

    /* Wieder als Timer-Funktion registrieren */
    glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

    /* Neuzeichnen anstossen */
    glutPostRedisplay();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer. Du bist toll :)
 */
static void
cbDisplay(void)
{

    /* Fensterdimensionen auslesen */
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    /* Kamera von der Seite */
    GLdouble lookAtThirdPerson[9] = {(*calculateThridPersonCameraPosition())[0],
                                     (*calculateThridPersonCameraPosition())[1],
                                     (*calculateThridPersonCameraPosition())[2], /* Augpunkt */
                                     0.0, 0.0,
                                     0.0, /* Zentrum */
                                     0.0, 1.0, 0.0};
    //Velocity und Up Vector des gepickten Partikels holen
    int pickedParticle = getPickedParticle();
    CGVector3f particleVelocity = {getParticleVelocityX(pickedParticle), getParticleVelocityY(pickedParticle), getParticleVelocityZ(pickedParticle)};
    CGVector3f particleUp = {getParticleUpX(pickedParticle), getParticleUpY(pickedParticle), getParticleUpZ(pickedParticle)};
    //Vektoren normalisieren und auf die richtige Laenge bringen
    normalizeVector(particleVelocity);
    normalizeVector(particleUp);
    multiplyVectorWithScalar(particleVelocity, VECTOR_LENGTH_FACTOR, particleVelocity);
    multiplyVectorWithScalar(particleUp, VECTOR_LENGTH_FACTOR, particleUp);
    //Position der Kamera ist 1/2 * Up ueber dem Partikel und 3 * Velocity hinter dem Partikel
    float cameraPositionX = getParticleX(pickedParticle) + (particleUp[LX] / CAMERA_DISTANCE_TO_PARTICLE) - (CAMERA_DISTANCE_BEHIND_PARTICLE * particleVelocity[LX]);
    float cameraPositionY = getParticleY(pickedParticle) + (particleUp[LY] / CAMERA_DISTANCE_TO_PARTICLE) - (CAMERA_DISTANCE_BEHIND_PARTICLE * particleVelocity[LY]);
    float cameraPositionZ = getParticleZ(pickedParticle) + (particleUp[LZ] / CAMERA_DISTANCE_TO_PARTICLE) - (CAMERA_DISTANCE_BEHIND_PARTICLE * particleVelocity[LZ]);
    //Ausrichtung der Kamera ist in Richtung der Velocity vor der Kameraposition
    float cameraDirectionX = cameraPositionX + particleVelocity[LX];
    float cameraDirectionY = cameraPositionY + particleVelocity[LY];
    float cameraDirectionZ = cameraPositionZ + particleVelocity[LZ];
    GLdouble lookAtFirstPerson[9] = {cameraPositionX, cameraPositionY, cameraPositionZ,
                                     cameraDirectionX, cameraDirectionY, cameraDirectionZ,
                                     //Up Vector als Up Vector
                                     particleUp[LX], particleUp[LY], particleUp[LZ]};

    /* Framewbuffer und z-Buffer zuruecksetzen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_drawHelp)
    {
        set2DViewport(0, 0,           /* x, y */
                      width, height); /* breite, hoehe */
    }
    else //Keine Hilfe
    {

        /* 3D-Viewports setzen */
        if (!g_firstPersonCam)
        {
            set3DViewport(0, 0,               /* x, y */
                          width, height,      /* breite, hoehe */
                          lookAtThirdPerson); /* Kamera */
        }
        else
        {
            set3DViewport(0, 0,               /* x, y */
                          width, height,      /* breite, hoehe */
                          lookAtFirstPerson); /* Kamera */
        }
    }

    /* Objekt anzeigen */
    glutSwapBuffers();

    /* Framerate berechnen */
    g_fps = frameRate();
}

/**
 * Verarbeitung eines Tasturereignisses.
 * Pfeiltasten steuern die Position des angezeigten Rechtecks.
 * F1-Taste (de-)aktiviert Wireframemodus.
 * F2-Taste schaltet zwischen Fenster und Vollbilddarstellung um.
 * ESC-Taste und q, Q beenden das Programm.
 * Falls Debugging aktiviert ist, wird jedes Tastaturereignis auf stdout
 * ausgegeben.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
                    int y)
{
    /** Keycode der ESC-Taste */
#define ESC 27

    /* temporaere Variable fuer Zustaende */
    GLboolean state = GL_FALSE;

    /* Debugausgabe */
    debugPrintKeyboardEvent(key, status, isSpecialKey, x, y);

    /* Taste gedrueckt */
    if (status == GLUT_DOWN)
    {
        /* Spezialtaste gedrueckt */
        if (isSpecialKey)
        {
            switch (key)
            {
            case GLUT_KEY_UP:
                increaseParticles();
                break;
            case GLUT_KEY_DOWN:
                decreaseParticles();
                break;
                /* (De-)Aktivieren des Wireframemode */
            case GLUT_KEY_F1:
                toggleWireframeMode();
                glutPostRedisplay();
                break;
                /* Umschalten zwischen Fenster- und Vollbilddarstellung */
            case GLUT_KEY_F2:
                toggleFullscreen();
                break;
                /* Normalen anzeigen */
            case GLUT_KEY_F3:
                toggleNormals();
                break;
                /* Texturen anzeigen */
            case GLUT_KEY_F4:
                state = !getTexturingStatus();
                setTexturingStatus(state);
                glutPostRedisplay();
                break;
            case GLUT_KEY_F5:
                /* Belichtung an und aus */
                state = !getLightingStatus();
                setLightingStatus(state);
                break;
            case GLUT_KEY_F6:
                /*Punktlichtquelle an/aus*/
                state = !getLight0Status();
                setLight0Status(state);
                //Neuzeichnen anstossen
                glutPostRedisplay();
                break;
            }
        }
        /* normale Taste gedrueckt */
        else
        {
            switch (key)
            {
                //Zahl 1
            case 49:
                toggleAcceleration();
                break;
                //Zahl 2
            case 50:
                toggleVelocity();
                break;
            //Zahl 3
            case 51:
                toggleUpVector();
                break;
            /* veraendert den Radius */
            case '.':
                g_radiusStatus = radiusClose;
                break;
            case ',':
                g_radiusStatus = radiusFar;
                break;

                /* Bewegen der Kamera in entsprechende Richtung starten */
            case 'w':
            case 'W':
                g_camMovementStatus = moveUp;
                break;
            case 'a':
            case 'A':
                g_camMovementStatus = moveLeft;
                break;
            case 's':
            case 'S':
                g_camMovementStatus = moveDown;
                break;
            case 'd':
            case 'D':
                g_camMovementStatus = moveRight;
                break;
                /* Programm beenden */
            case 'q':
            case 'Q':
            case ESC:
                freeArraysLogic();
                exit(0);
                break;
                /* Hilfe */
            case 'H':
            case 'h':
                toggleHelp();
                break;
            /*Pausiert das Spiel*/
            case 'P':
            case 'p':
                toggleGamePause();
                break;
            /*Dastellungsform der Partikel aendern*/
            case 'E':
            case 'e':
                toggleParticleAppearance();
                break;
            /*Kamera togglen (First Person oder Third Person)*/
            case 'C':
            case 'c':
                g_firstPersonCam = !g_firstPersonCam;
                break;
            /*Schatten der Partikel toggeln*/
            case 'R':
            case 'r':
                toggleShadows();
                break;
                /*Achsen und Grid zum debuggen*/
            case 'v':
            case 'V':
                g_visualHelp = !g_visualHelp;
                break;
                /* Zielobjekt aendern */
            case 'z':
            case 'Z':
                changeTargetMode();
                break;
            case 'm':
            case 'M':
                g_ballMovement = !g_ballMovement;
                break;
            case 'n':
            case 'N':
                increasePickedParticle();
                break;
                /* Textur aendern */
            case 't':
            case 'T':
                increaseTextureIdx();
                break;
            }
        }
    }
    /* Taste losgelassen */
    else
    {
        switch (key)
        {
        /* veraendern des Radius bei loslassen stoppen */
        case '.':
            g_radiusStatus = radiusNone;
            break;
        case ',':
            g_radiusStatus = radiusNone;
            break;

            /* Bewegen der Kamera in entsprechende Richtung stoppen */
        case 'w':
        case 'W':
            g_camMovementStatus = moveNone;
            break;
        case 'a':
        case 'A':
            g_camMovementStatus = moveNone;
            break;
        case 's':
        case 'S':
            g_camMovementStatus = moveNone;
            break;
        case 'd':
        case 'D':
            g_camMovementStatus = moveNone;
            break;
        }
    }
}

/**
 * Bewegt die Kamera durch Tastendruecke.
 */
static void moveCam(void)
{
    switch (g_camMovementStatus)
    {
    case moveUp:
        g_theta += CAMERA_ROTATION_SPEED;
        g_theta = clip(g_theta, LOWER_BOUND_THETA + DELTA, UPPER_BOUND_THETA - DELTA); //Damit Camera != UP-Vector
        break;
    case moveDown:
        g_theta -= CAMERA_ROTATION_SPEED;
        g_theta = clip(g_theta, LOWER_BOUND_THETA + DELTA, UPPER_BOUND_THETA - DELTA); //Damit Camera != UP-Vector
        break;
    case moveLeft:
        g_phi += CAMERA_ROTATION_SPEED;
        g_phi = fmod(g_phi, UPPER_BOUND_PHI); //Nicht zu groß werden lassen
        break;
    case moveRight:
        g_phi -= CAMERA_ROTATION_SPEED;
        if (g_phi < LOWER_BOUND_PHI)
        {
            g_phi += UPPER_BOUND_PHI;
        }
        break;
    default:
        break;
    }
}

/**
 * Veraendert den Radius.
 */
static void handleRadius(void)
{
    switch (g_radiusStatus)
    {
    case radiusClose:

        if ((g_radius - RADIUS_SCROLL_STEPS) > 0.1)
        {
            g_radius -= RADIUS_SCROLL_STEPS;
        }
        break;
    case radiusFar:
        g_radius += RADIUS_SCROLL_STEPS;
        break;
    default:
        break;
    }
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y)
{
    handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Tastenloslassen.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbKeyboardUp(unsigned char key, int x, int y)
{
    handleKeyboardEvent(key, GLUT_UP, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial(int key, int x, int y)
{
    handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void)
{

    /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
    glutKeyboardFunc(cbKeyboard);

    /* Tasten-Loslass-Callback - wird ausgefuehrt, wenn eine Taste losgelassen
   * wird */
    glutKeyboardUpFunc(cbKeyboardUp);

    /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
   * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
   * Einfuegen) gedrueckt wird */
    glutSpecialFunc(cbSpecial);

    /* Automat. Tastendruckwiederholung ignorieren */
    glutIgnoreKeyRepeat(0);

    /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
    glutTimerFunc(1000 / TIMER_CALLS_PS,       /* msecs - bis Aufruf von func */
                  cbTimer,                     /* func  - wird aufgerufen    */
                  glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
                                                   func aufgerufen wird */

    /* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
     * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
    glutDisplayFunc(cbDisplay);
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height)
{
    int windowID = 0;

    /* Kommandozeile immitieren */
    int argc = 1;
    char *argv = "cmd";

    /* Glut initialisieren */
    glutInit(&argc, &argv);

    /* DEBUG-Ausgabe */
    INFO(("Erzeuge Fenster...\n"));

    /* Initialisieren des Fensters */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(width, height);
    glutInitWindowPosition(2500, 0);

    /* Fenster erzeugen */
    windowID = glutCreateWindow(title);

    if (windowID)
    {

        /* DEBUG-Ausgabe */
        INFO(("...fertig.\n\n"));

        /* DEBUG-Ausgabe */
        INFO(("Initialisiere Szene...\n"));
        if (initScene())
        {
            /* DEBUG-Ausgabe */
            INFO(("...fertig.\n\n"));

            /* DEBUG-Ausgabe */
            INFO(("Lade und initialisiere Texturen...\n"));

            if (initTextures())
            {
                bindTexture(0);
                /* DEBUG-Ausgabe */
                INFO(("...fertig.\n\n"));

                /* DEBUG-Ausgabe */
                INFO(("Registriere Callbacks...\n"));

                registerCallbacks();

                /* DEBUG-Ausgabe */
                INFO(("...fertig.\n\n"));

                /* DEBUG-Ausgabe */
                INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

                glutMainLoop();
            }
            else
            {
                /* DEBUG-Ausgabe */
                INFO(("...fehlgeschlagen.\n\n"));

                glutDestroyWindow(windowID);
                windowID = 0;
            }
        }
        else
        {
            /* DEBUG-Ausgabe */
            INFO(("...fehlgeschlagen.\n\n"));

            glutDestroyWindow(windowID);
            windowID = 0;
        }
    }
    else
    {
        /* DEBUG-Ausgabe */
        INFO(("...fehlgeschlagen.\n\n"));
    }

    return windowID;
}
