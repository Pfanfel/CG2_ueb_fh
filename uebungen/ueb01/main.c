/**
 * @file
 * Hauptprogramm. Initialisierung und Starten der Ergeignisverarbeitung.
 *
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Michael Smirnov & Len Harmsen
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "debugGL.h"

/**
 * Hauptprogramm.
 * Initialisierung und Starten der Ereignisbehandlung.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int main(int argc, char **argv)
{
    /* Initialisierung des I/O-Sytems
       (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
    if (!initAndStartIO("CG2 Cloudy", 500, 500))
    {
        fprintf(stderr, "Initialisierung fehlgeschlagen!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}