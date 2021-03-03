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
#include "util.h"

/* Funktionen */

static void moveCam(void);

static void handleRadius(void);

/* ---- Globale Variablen ---- */

Movement g_camMovementStatus = moveNone;

Radius g_radiusStatus = radiusNone;

BoundingBoxType g_boundingBoxStatus = boundingBoxAABB;
GLboolean g_drawBoundingBox = GL_FALSE;

GLboolean g_drawHelp = GL_FALSE;

/* Kamera Stuff */

float g_phi = INIT_PHI;
float g_theta = INIT_THETA;
float g_radius = INIT_CAMERA_RADIUS;

GLfloat g_fps = 0.0f;

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60
/* ---- Funktionen ---- */

/**
 * Liefert den Zeichenstatus der BoundingBox
 */
GLboolean getDrawBoundingBoxStatus(void)
{
    return g_drawBoundingBox;
}

/**
 * Liefert den BoundingBox Status.
 */
BoundingBoxType getBoundingBoxStatus(void)
{
    return g_boundingBoxStatus;
}

/**
 * Liefert den Bewegungsstatus der Kamera.
 */
Movement getCamMovementStatus(void)
{
    return g_camMovementStatus;
}

/**
 * Liefert den Bewegungsstatus des Radiiuses der Kamera.
 */
Radius getRadiusStatus(void)
{
    return g_radiusStatus;
}

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
 * Toggelt ob der Hilfetext angezeigt werden soll oder nicht.
 */
void toggleHelp()
{
    g_drawHelp = !g_drawHelp;
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
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
    lastCallTime = lastCallTime;
    /* Seit dem Programmstart vergangene Zeit in Millisekunden */
    int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

    /*Kamerabewegung durch Tastendruecke*/
    moveCam();
    /*Kameraradius durch Tastendruecke*/
    handleRadius();

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

    /* Framewbuffer und z-Buffer zuruecksetzen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawScene(width, height);

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
            case GLUT_KEY_F1:
                break;
                /* Umschalten zwischen Fenster- und Vollbilddarstellung */
            case GLUT_KEY_F2:
                toggleFullscreen();
                break;
            case GLUT_KEY_F3:
                break;
            case GLUT_KEY_F4:
                break;
            }
        }
        /* normale Taste gedrueckt */
        else
        {
            switch (key)
            {
                //Lichtquelle aus/einschalten
            case '1':
                toggleG_numLightsStartIndex();
                break;
                //Vignette aus/einschalten
            case '2':
                toggleG_Vignette();
                break;
                /* veraendert den Radius */
            case '.':
                g_radiusStatus = radiusClose;
                break;
            case ',':
                g_radiusStatus = radiusFar;
                break;
                /* Weiterschalten der verschiedenen Bounding Boxes */
            case 'n':
            case 'N':
                g_boundingBoxStatus = g_boundingBoxStatus + 1 >= 3 ? 0 : g_boundingBoxStatus + 1;
                setG_rendered(GL_FALSE);
                break;
                /* BoundingBox zeichnen togglen */
            case 'b':
            case 'B':
                g_drawBoundingBox = !g_drawBoundingBox;
                setG_rendered(GL_FALSE);
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
                //Vorne richtung -Z
            case 'v':
            case 'V':
                g_phi = 90;
                g_theta = 0;
                setG_rendered(GL_FALSE);
                break;
                //Hinten richtung Z
            case 'H':
                g_phi = 270;
                g_theta = 0;
                setG_rendered(GL_FALSE);
                break;
                //Links richtung X
            case 'l':
            case 'L':
                g_phi = 180;
                g_theta = 10;
                setG_rendered(GL_FALSE);
                break;
                //Rechts richtung -X
            case 'r':
            case 'R':
                g_phi = 0;
                g_theta = 0;
                setG_rendered(GL_FALSE);
                break;
                //Oben richtung -Y
            case 'o':
            case 'O':
                g_phi = 90;
                g_theta = 90;
                setG_rendered(GL_FALSE);
                break;
                //Unten richtung Y
            case 'u':
            case 'U':
                g_phi = 90;
                g_theta = -90;
                setG_rendered(GL_FALSE);
                break;
                /* Programm beenden */
            case 'q':
            case 'Q':
            case ESC:
                freeFrameBuffer();
                exit(0);
                /* Hilfe */
            case 'h':
                toggleHelp();
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
            g_camMovementStatus = g_camMovementStatus != moveUp ? g_camMovementStatus : moveNone;
            break;
        case 'a':
        case 'A':
            g_camMovementStatus = g_camMovementStatus != moveLeft ? g_camMovementStatus : moveNone;
            break;
        case 's':
        case 'S':
            g_camMovementStatus = g_camMovementStatus != moveDown ? g_camMovementStatus : moveNone;
            break;
        case 'd':
        case 'D':
            g_camMovementStatus = g_camMovementStatus != moveRight ? g_camMovementStatus : moveNone;
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
 * Zeichnet neu.
 * @param w neue Fensterbreite
 * @param h neue Fenserhoehe
 */
static void cbRedraw(int w, int h)
{
    w = w;
    h = h;
    setG_rendered(GL_FALSE);
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

    glutReshapeFunc(cbRedraw);

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
int initAndStartIO(char* title, int width, int height)
{
    int windowID = 0;

    /* Kommandozeile immitieren */
    int argc = 1;
    char* argv = "cmd";

    /* Glut initialisieren */
    glutInit(&argc, &argv);

    /* Initialisieren des Fensters */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);

    /* Fenster erzeugen */
    windowID = glutCreateWindow(title);

    if (windowID)
    {
        if (initScene(width, height))
        {
            registerCallbacks();

            glutMainLoop();
        }
        else
        {
            fprintf(stderr, "Scene konnte nicht initialisiert werden\n");
            glutDestroyWindow(windowID);
            windowID = 0;
        }
    }
    else
    {
        fprintf(stderr, "Fenster konnte nicht erstellt werden\n");
    }

    return windowID;
}
