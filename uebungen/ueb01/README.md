
# Aufgabe 1: Cloudy

![](https://intern.fh-wedel.de/typo3temp/pics/bd0779df5c.jpg)

## Neue Techniken:

Spline- und Bézierkurven

##  Beschreibung:

Die erste Übung beschäftigt sich mit Kurven, genauer gesagt mit Uniform
B-Splines, aus denen wir eine vereinfachte Version des Spiels "Cloudy"
erstellen: Anhand von jeweils mindestens vier gegebenen Punkten soll ein
Splinestück erzeugt werden, bei dem alle vorhandenen Punkte außer dem
ersten und letzten mit der Maus selektiert und in ihrer Position frei
verschoben werden können. Somit läßt sich eine Gesamtkurve erstellen,
auf der ein Papierflieger entlangfliegt und dabei eine Reihe von Sternen
einsammeln soll.

Als Beispielprogramm dient diesmal dieser
Youtube-[Walkthrough](https://www.youtube.com/watch?v=DNMXOI9sURA). Die
dort gezeigte Funktionalität muß allerdings nicht komplett nachgebildet
werden.


##  Kernanforderungen:

-   Eine **Uniform B-Splinekurve** wird dargestellt, interpoliert aus
    genau 4 Kontrollpunkten.
-   Die **Zeichengenauigkeit** der Splinekurve läßt sich mittels
    Tastendruck verändern, was die Anzahl der auf der idealen Kurve
    interpolierten Punkte verändert, die durch einen Linienzug verbunden
    werden.
-   Die mittleren beiden Punkte sind **mit der Maus frei im Fenster
    verschiebbar**, wobei sich die Splinekurve direkt mit anpaßt.

##  Weitere Funktionalitätsanforderungen:

-   Es gibt mindestens drei verschiedene, aufeinanderfolgende **Level**,
    die im Schwierigkeitsgrad ansteigen: Im 1. Level soll es eine
    Splinekurve aus 4 Punkten und einen Stern zum Einsammeln geben,
    im 2. Level 5 Punkte und 3 Sterne und im 3. Level 6 Punkte, 2 Sterne
    und eine fest platzierte Wolke (s.u.).  
    Für alle vorhandenen Kontrollpunkte werden jeweils die zugehörigen
    **Splinestücke** interpoliert: Die ersten vier Punkte bilden das
    erste Teilstück, die Punkte zwei bis fünf das zweite Teilstück usw.
    Beachtet dabei bitte, daß durch die Splineinterpolation die Kurve in
    unserem Programm im allgemeinen Fall NICHT direkt durch die vier
    Kontrollpunkte verlaufen, sondern sich (deutlich kleiner) irgendwo
    im Bereich der konvexen Hülle befindet (s.u.).
-   **Anfangs- und Endkontrollpunkt** der Splinekurve sind jeweils fest
    am linken und rechten Rand in der Mitte platziert, alle weiteren
    Kontrollpunkte (mindestens 2) können vom Spieler mit der linken
    Maustaste selektiert und frei in x- und y-Richtung verschoben werden
    (falls noch genug Platz ist also auch komplett um Anfangs- und
    Endpunkt herum). Die Kurve und der Flieger passen sich dabei
    umgehend an. Es handelt sich folglich um eine höherwertige Funktion,
    bei der x- und y-Komponente separat interpoliert werden!
-   Die ganze Szene ist **orthografisch** projiziert in 2D und aus
    2D-Objekten aufgebaut, die **Kamera** ist unbeweglich,
    **Beleuchtung** ist nicht erforderlich.  
    Durch die feste Kamera ist beim Verschieben von Punkten der Platz
    natürlich begrenzt. Es muß aber trotzdem möglich sein, alle Sterne
    mit der Kurve bzw. dem Papierflieger zu erreichen.  
    Hinweis: Zur Selektion muß kein Picking umgesetzt werden, da wir ja
    nur eine 2D-Szene haben. Eine einfache Koordinatenumrechnung reicht
    aus!
-   Der Papierflieger ist zu Beginn jedes Levels am linken Ende der
    Kurve platziert und wartet dort, bis der Spieler ihn mit der Taste
    s/S losfliegen läßt. Dann bewegt der Flieger sich entlang der
    Splinekurve, wobei er sich jeweils entsprechend des gerade
    überflogenen Splinestückes orientiert (s. dazu auch die Normalen
    weiter unten). Der Flieger befindet sich dabei nicht direkt *auf*
    der Kurve, sondern wie im Video stets ein kleines Stück *darüber*!
    Wo im Raum genau "darüber" ist und wie die **Orientierung** des
    Fliegers aussieht, richtet sich dabei nach der Normalen der Kurve
    zwischen den beiden jeweils gerade überflogenen interpolierten
    Punkten und nicht nur nach den 4-6 Kontrollpunkten.  
    Die **Geschwindigkeit** des Fliegers soll dabei auf Basis der
    aktuellen Steigung der Kurve an dieser Stelle variabel sein: Runter
    geht es schneller als rauf. Dabei darf eine Mindestgeschwindigkeit
    nicht unterschritten werden (der Flieger bleibt also nie stehen) und
    er darf sich auch nicht unendlich schnell abwärts bewegen :-)  
    Hinweis: Durch verschieden lange einzelne Kurvenabschnitte der
    Gesamtkurve wird ein Punkt bei einer animierten Bewegung auf der
    Kurve selbst ohne die zuvor genannte Geschwindigkeitsanpassung
    meistens nicht ganz gleich schnell die gesamte Kurve entlanglaufen.
    Dies ist ok und muß von Euch nicht "korrigiert" werden.
-   Kommt der Flieger einem **Stern** ausreichend nahe (Abstand der
    Mittelpunkte \<= Summe der Radien), gilt dieser Stern als
    eingesammelt und verschwindet daraufhin. Wurden bei einem Flug alle
    Sterne eingesammelt und der Flieger hat das rechte Ende der Kurve
    erreicht, gilt das Level als gelöst und das nächste wird angezeigt.
    Falls nicht, wird der Flieger wieder am linken Ende der Kurve
    platziert und der Spieler kann es erneut versuchen.
-   Die **Wolke** aus dem 3. Level gilt als unbewegliches Hindernis, das
    der Papierflieger nicht berühren darf. Tut er es doch, ist das Level
    verloren und der Spieler muß von vorne beginnen. Hier soll dieselbe
    "Kollisionserkennung" wie bei den Sternen genutzt werden.  
    Die Wolke muß sich weder bewegen, noch Regen oder Blitze fallen
    lassen, der/die den Flieger ebenfalls zerstören würde(n). Auch muß
    es keine Zeitnahme und Punktberechnung und auch keinen schönen
    Hintergrund und keine Musik wie im Original geben. Allerdings ist
    dies natürlich auch nicht verboten :-)
-   Die Objekte müssen nicht so komplex wie im Beispiel **modelliert**
    sein. Für den Flieger reicht ein Dreieck (das aber korrekt
    entsprechend der Kurve orientiert sein muß und es muß deutlich sein,
    wo "vorne" ist), für die Wolke könnt Ihr z.B. ein paar Kreise nehmen
    und für den Stern einen Triangle-Fan mit zwei verschiedenen Radien.
-   Die **Genauigkeit der Kurve** (also wie "rund" sie gezeichnet wird)
    läßt sich über die Tasten + und - beeinflussen. Damit wird die
    Anzahl der Punkte, die auf der Kurve interpoliert werden (und
    zwischen denen man mittels Vertexarray mit Typ GL_LINE_STRIP die
    Geradenstücke zeichnet) verändert. Unabhängig davon soll sich der
    Flieger immer auf der idealen Kurve bewegen. Er wird folglich bei
    einer sehr groben Unterteilung weiter vom gezeichneten Linienzug
    abweichen, als bei einer detaillierteren Darstellung.
-   Durch einen Tastendruck auf n/N lassen sich die **Normalen** in den
    Punkten der interpolierten Kurve ein- und ausblenden. Berechnet die
    Normale eines Punktes der Kurve dabei bitte über das Kreuzprodukt
    aus dem Verbindungsvektor der beiden direkt benachbarten Punkte und
    dem Vektor (0,0,1), also der z-Achse. Für die beiden Endpunkte (wo
    es keine 2 benachbarten Punkte gibt) soll die Verbindung des
    Endpunktes selbst mit dem einen vorhandenen Nachbarpunkt benutzt
    werden.
-   Es sind **Vertexarrays** zu verwenden, die mindestens die Vertices
    der interpolierten Kurve und die der Normalen beinhalten und diese
    auch daraus zeichnen.
-   Mit der Taste b/B soll man zwischen der **Spline- und der
    Bézier-Interpolation** hin- und herschalten können. Dies muß
    allerdings - um die Sache nicht zu schwierig zu machen - nur
    funktionieren, wenn genau vier Punkte angezeigt werden (also nur
    im 1. Level). Die sonstige Funktionalität (Normalen, Genauigkeit,
    Flug des Papierfliegers, ...) soll auch bei Bézier weiterhin
    vorhanden sein! Beachtet dabei bitte insbesondere, daß durch die
    (bei Spline gegenüber Bézier) verkürzte Kurve evtl. nicht mehr alle
    Punkte erreichbar sind, wenn man hin- und herschaltet.
-   Mit der Taste c/C soll die **konvexe Hülle** für Spline bzw. Bézier
    ein- und ausgeblendet werden können. Welche Kontrollpunkte die
    konvexe Hülle beschreiben läßt sich am einfachsten mit dem in der
    Einführung beschriebenen Algorithmus berechnen. Bei Änderungen der
    Kontrollpunkte soll sich auch die konvexe Hülle direkt mit anpassen.
-   Beachtet die allgemeinen Anforderungen!

##  Tips:

1\) Die Normalen und die Hüllkurve zeichnet Ihr am einfachsten mit einem
GL_LINES bzw. GL_LINE_STRIP als Primitivtyp im *glDrawElements()*. Auch
für die Hüllkurve könnt Ihr das Vertexarray (in Verbindung mit einem
\[weiteren\] Indexarray) benutzen (z.B., falls Ihr die Kontrollpunkte
ohnehin schon mit ins Vertexarray gespeichert habt).

2\) Zur konvexen Hülle gibt es ein separates kleines Beispielprogramm
für
[Linux](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS1617_convexhull)
und
[Windows](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS1617_convexhull.exe).

3\) Das [Material zur
Einführungsveranstaltung](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/SplinesUndKonvexeHuelle.pdf "Splines")
könnte auch hilfreich sein.
