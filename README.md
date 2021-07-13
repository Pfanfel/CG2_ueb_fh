# Computergafik 2 Übung

Zur erstellung wurde C99 zusammen mit openGl verwendet. 

Die Details der Übung sind auf der internen Seite des Übungsleiters [Lars Neuman](https://intern.fh-wedel.de/mitarbeiter/ne/praktikum-geometrische-modellierung-und-computeranimation/) zu finden:


## Programme ausführen

Zum Ausführen der Programme muss das Makefile in dem jeweiligen Ordner mit dem Befehl `make` ausgeführt werden.

Um die `*.o` Files zu entfernen kann `make clean` verwendet werden.

## Erste Aufgabe Cloudy (ueb01)

Spline- und Bézierkurven

Die erste Übung beschäftigt sich mit Kurven, genauer gesagt mit Uniform B-Splines, aus denen wir eine vereinfachte Version des Spiels "Cloudy" erstellen: Anhand von jeweils mindestens vier gegebenen Punkten soll ein Splinestück erzeugt werden, bei dem alle vorhandenen Punkte außer dem ersten und letzten mit der Maus selektiert und in ihrer Position frei verschoben werden können. Somit läßt sich eine Gesamtkurve erstellen, auf der ein Papierflieger entlangfliegt und dabei eine Reihe von Sternen einsammeln soll.

Als Beispielprogramm dient diesmal dieser Youtube-Walkthrough. Die dort gezeigte Funktionalität muß allerdings nicht komplett nachgebildet werden.
<https://www.youtube.com/watch?v=DNMXOI9sURA>

![Aufgabe 1](./gifs/CG2_cloudy.gif)

## Zweite Aufgabe Landschaftsgenerator (ueb02)

Splineflächen

In der zweiten Übung wollen wir den Schritt von 2D nach 3D gehen. Aus der Splinekurve wird nun eine Splinefläche. Somit werden die Geometriewerte nicht mehr direkt als Kontrollpunkte angegeben, sondern selbst aus jeweils vier anderen Punkten interpoliert.
Optisch soll eine Landschaft entstehen, auf die verschiedene Texturen aufgebracht werden können und für die man sich u.a. ihre Normalen, Kontroll- und interpolierten Punkte anzeigen lassen kann.

![Aufgabe 2](./gifs/CG2_landschaftsgenerator.gif)

## Dritte Aufgabe Kugelspiel (ueb03)

Animation, Physik und Kollisionserkennung

In dieser Übung erweitern wir die zweite Aufgabe und richten das Augenmerk stärker auf die Physik. Über die Splinefläche sollen nun (relativ) physikalisch korrekt Kugeln rollen, die dabei mit bestimmten Bereichen in der Landschaft interagieren können und schließlich ein definiertes Ziel erreichen sollen.

![Aufgabe 3](./gifs/CG2_kollision.gif)

## Vierte Aufgabe Partikelsimulation (ueb04)

Partikelsimulation

In der vierten Aufgabe widmen wir uns einer Partikelsimulation. Rund um zwei Zielobjekte fliegen etliche Partikel herum, deren Flugbahnen sich dabei mittels Euler-Integration einem realistischen Flugverhalten annähern.

![Aufgabe 4](./gifs/CG2_partikel.gif)

## Fünfte Aufgabe Raytracer (ueb05)

Raytracing, Bounding-Boxes

In der letzten Aufgabe ist ein einfacher Raytracer zu erstellen, der eine raumähnliche Szene mit wenigen Objekten darin zeigt.

![Aufgabe 5](./gifs/CG2_raytracer.gif)
