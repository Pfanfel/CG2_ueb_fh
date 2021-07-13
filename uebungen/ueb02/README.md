
# Aufgabe 2: Landschaftsgenerator

## Neue Techniken:

Splineflächen

##  Beschreibung:

![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS1617_Splineflaeche.png)

In der zweiten Übung wollen wir den Schritt von 2D nach 3D gehen. Aus der
Splinekurve wird nun eine Splinefläche. Somit werden die Geometriewerte
nicht mehr direkt als Kontrollpunkte angegeben, sondern selbst aus
jeweils vier anderen Punkten interpoliert.  
Optisch soll eine Landschaft entstehen, auf die verschiedene Texturen
aufgebracht werden können und für die man sich u.a. ihre Normalen,
Kontroll- und interpolierten Punkte anzeigen lassen kann.

Hier gibt es ein Beispielprogramm für
[Linux](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Splineflaeche).
Zum Ablauf benötigt es eine Reihe von
[Texturen](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Splineflaeche_Texturen.rar)
im Nebenordner "content".

##  Kernanforderungen:

-   Eine **Splinefläche aus 4x4 Kontrollpunkten** interpoliert wird
    dargestellt.
-   Die **Zeichengenauigkeit** der Splinefläche läßt sich mittels
    Tastendruck verändern, was die Anzahl der interpolierten Punkte
    verändert, die durch Dreiecke verbunden werden.
-   Die Normalen der Splinefläche werden korrekt mittels **partieller
    Ableitung** berechnet.

##  Weitere Funktionalitätsanforderungen:

-   Nach dem Starten des Programms sieht man bereits eine
    **Splinefläche** (bzw. einen Flächenteil der Landschaft) mit
    mindestens 16 (=4x4) Kontrollpunkten. Die y-Komponenten der
    Kontrollpunkte können dabei zunächst in einem definierten Bereich
    zufällig gewählt oder aus den umliegenden Kontrollpunkten gemittelt
    werden (+ evtl. eine leichte Zufallskomponente, damit es nicht zu
    einheitlich wird). Dasselbe gilt auch für alle im weiteren
    Programmverlauf ggf. noch erzeugten neuen Kontrollpunkte.
-   Mit der linken und rechten Pfeiltaste kann jeweils einer der
    Kontrollpunkte selektiert werden (durch einen zyklischen Durchlauf
    aller Kontrollpunkte, beginnend bei dem 1., der zu Beginn bereits
    selektiert ist). Der aktuell selektierte Kontrollpunkt soll zur
    besseren Erkennbarkeit in einer anderen Farbe dargestellt werden.
    Mit den Pfeiltasten nach oben und unten kann dann seine **Höhe
    verändert** werden, woran sich die entstehende Splinefläche sofort
    anpasst.
-   Über die Tasten p/P und o/O können weitere **Flächenteile ergänzt
    bzw. wieder entfernt** werden (entweder, indem neue Kontrollpunkte
    zwischen den bestehenden ergänzt werden oder durch einfaches
    Hinzufügen bzw. Entfernen einer zusätzlichen "Spalte" und "Zeile" am
    Rand der Fläche, wobei die bestehenden Flächenteile unverändert
    bleiben).
-   Für alle vorhandenen Kontrollpunkte werden jeweils die zugehörigen
    Splineflächen interpoliert. Dabei muß nur die **y-Koordinate
    interpoliert** werden (es ist also keine höherwertige Interpolation
    gefordert, wir empfehlen diese aber, weil man sich damit später
    einige Probleme bei der Interpolation erspart. Im "höherwertigen"
    Fall ist danach ggf. eine "globale Skalierung" der Splinefläche
    sinnvoll, um sie zu vergrößern). Die x- und z-Koordinaten bekommt
    man ansonsten z.B. durch eine lineare Abbildung der Werte von s und
    t auf x und z (man interpoliert also anhand von s und t jeweils
    zwischen den x- bzw. z-Werten der jeweils ersten und letzten
    Kontrollpunkte dieses Flächenabschnitts).
-   Es handelt sich um eine **3D-Szene** (**Beleuchtung** mit Normalen
    aus einem Normalarray ist erforderlich!), in der man rudimentär mit
    der **Kamera** das Geschehen betrachten und sich darin bewegen kann
    (bewegen, rotieren und zoomen soll möglich sein). Die Lichtquelle
    soll sich ständig als **Punktlichtquelle auf einer Kreisbahn** um
    den Mittelpunkt der Szene (aber natürlich deutlich höher als die
    Fläche und mit einem nicht zu kleinen Radius) bewegen und als gelbe
    Kugel dargestellt werden.
-   Die Normalen müssen dieses mal wirklich korrekt über die
    **partiellen Ableitungen** der Funktion q(s,t) berechnet werden. Es
    reicht nicht mehr aus, die Normalen mittels der umliegenden Punkte
    grob zu interpolieren!
-   Die Farbe eines Punktes der Splinefläche soll **höhenabhängig** sein
    (damit sind NICHT die interpolierten Punkte - dargestellt durch
    Kugeln - gemeint, sondern die Splinefläche selbst, die gefärbt
    werden soll!). Definiert Euch dazu bitte verschiedene Konstanten,
    die mindestens vier verschiedene Höhenbereiche und die zugehörigen
    Farben festlegen.
-   Es sind **Vertexarrays** zu verwenden, welche die Vertizes der
    interpolierten Splinefläche und die der Normalen beinhalten und
    diese auch daraus zeichnen. Für das Zeichnen der Splinefläche ist
    glDrawElements zu benutzen.
-   Die **Genauigkeit der Splinefläche** (also wie "weich" sie
    gezeichnet wird) läßt sich über die Tasten + und - beeinflussen.
    Damit wird die Anzahl der Punkte, die auf der Splinefläche
    interpoliert werden (und zwischen denen man mittels Vertexarray und
    GL_TRIANGLES Dreiecke zeichnet) verändert.
-   Mittels verschiedener **F-Tasten** lassen sich die Normalen, die
    interpolierten Punkte und die Kontrollpunkte ein- und ausblenden.
    Die interpolierten und die Kontrollpunkte werden dabei als Spheres
    dargestellt.
-   Alternativ zur Farbe je nach Höhe eines interpolierten Punktes soll
    es auch möglich sein, die gesamte Splinefläche mit einer einzigen
    **Textur** zu belegen. Wenn die Splinefläche ihre Größe ändert, soll
    sich folglich auch die Textur strecken oder stauchen (siehe Tips).
    Die Texturierung soll mit der Taste t/T ein- und ausschaltbar sein
    und mit z/Z soll zwischen den einzelnen Texturen (mindestens 3
    verschiedene sollen vorhanden sein) gewechselt werden können.
-   Mit der Taste c/C kann ein kurzer **Kameraflug** gestartet werden.
    Dieser führt entlang einer Beziérkurve vom höchsten zum tiefsten
    interpolierten Punkt der gesamten Splinefläche. Diese beiden Punkte
    bilden dabei den ersten und letzten Kontrollpunkt der Beziérkurve,
    die mittleren beiden Kontrollpunkte ergeben sich wie folgt: Zwischen
    dem ersten und letzten Punkt wird eine Gerade erstellt und
    gleichmäßig in drei Teile geteilt. Jeweils nach 1/3 bzw. 2/3 dieser
    Geraden befinden sich (was die x- und z-Koordinate angeht) die
    mittleren beiden Kontrollpunkte. Der Höhenwert (die y-Koordinate)
    der mittleren beiden Kontrollpunkte ergibt sich dann jeweils aus dem
    dortigen Höhenwert der Splinefläche (berechnet als exaktes q(s,t)) +
    ein per Konstante einstellbarer Abstand nach oben (so entsteht
    annähernd ein der Höhe der Fläche angepaßter Kurs). Dieser Abstand
    ist möglichst so zu wählen, daß die Kamera nicht mit der
    Splinefläche kollidiert.  
    Die Kamera soll dabei mit Blickrichtung in Flugrichtung entsprechend
    der Kurve ausgerichtet sein wie im Beispiel zu sehen.  
-   Beachtet die allgemeinen Anforderungen!

##  Tips:

1\) Die interpolierten Punkte verbindet Ihr am einfachsten mittels
GL_TRIANGLES bzw. GL_TRIANGLE_STRIP zu einer Fläche. Dabei generiert Ihr
am besten immer eine über die ganze Fläche laufende Spalte (oder Zeile)
von Dreiecken und beginnt dann wieder am gleichen Rand wie zuvor.
Ansonsten entstehen am gegenüberliegenden Rand wahrscheinlich zu Linien
entartete Dreiecke, was nicht besonders schön (und auch nicht erlaubt)
ist.

2\) Da es sich um eine 3D-Szene handelt und Beleuchtung erforderlich
ist, benötigt man natürlich Normalen. Damit diese auch bei der Benutzung
von Vertex-Arrays korrekt einbezogen werden, müssen zusätzlich zu den
Vertex-Arrays noch Normal-Arrays benutzt werden. Dies läuft fast
identisch zu den Vertex-Arrays ab: Die Benutzung von Normal-Arrays muß
einmalig mittels *glEnableClientState(GL_NORMAL_ARRAY)* aktiviert
werden; danach können - analog zu den Vertexdaten - mit
*glNormalPointer(...)* die Normalen angegeben werden. Vertex- und
Normal-Array nutzen dabei dasselbe Indexarray. Beim Aufruf von
*glDrawElements(...)* werden die Normalen dann automatisch mit benutzt.
Bedenkt dabei, daß Normalen in OpenGL immer pro Vertex und nicht pro
Fläche vergeben werden! Als Materialangaben werden - wenn vorhanden -
die zugehörigen Werte aus dem Color-Array benutzt. Dies muß allerdings
einmalig durch ein *glEnable(GL_COLOR_MATERIAL)* eingeschaltet werden.

3\) Zum Texturieren der gesamten Fläche braucht Ihr auf jeden Fall den
Parameter T zusätzlich zu den diversen t jedes einzelnen Splines. T
erstreckt sich dabei über die gesamte Kurve bzw. Fläche und nicht nur
über ein Kurven- bzw. Flächenteil. Wie bei den vielen t läuft auch bei T
der freie Parameter von 0..1 (hier allerdings inklusive beider
Grenzen).  
Wie schon die Normalen werden auch Texturkoordinaten in einem eigenen
Array abgelegt und beim Aufruf von *glDrawElements(...)* dann mit
benutzt. Der Befehl zur Aktivierung lautet hier
*glEnableClientState(GL_TEXTURE_COORD_ARRAY).*  
Auf [TextureHaven](https://texturehaven.com/textures/ "TextureHaven")
z.B. findet Ihr verschiedenste Texturen in hinreichender Größe.

4\) Das Material zur Einführung der
[Splineflächen](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Splineflaechen.pdf)
könnte auch weiterhelfen :-)
