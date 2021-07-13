
# Aufgabe 3: Kugelspiel

## Neue Techniken:

Animation, Physik und Kollisionserkennung


![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Kugelspiel.png)
 

## Beschreibung:

In dieser Übung erweitern wir die zweite Aufgabe und richten das
Augenmerk stärker auf die Physik. Über die Splinefläche sollen
nun (relativ) physikalisch korrekt Kugeln rollen, die dabei mit
bestimmten Bereichen in der Landschaft interagieren können und
schließlich ein definiertes Ziel erreichen sollen.

Auch zu dieser Aufgabe gibt es Beispielprogramme für
[Windows](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Kugelspiel.exe)
und
[Linux](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Kugelspiel),
die in einem Nebenordner content/textures ein paar
[Texturen](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Kugelspiel_textures.rar)
benötigen.

 

## Kernanforderungen:

-   Die Kernanforderungen aus **Aufgabe 2** werden erfüllt
-   Es gibt mindestens eine Kugel, die mittels **Euler-Integration**
    korrekt über die Splinefläche rollt
-   Die Kugel kann anhand der **Penalty-Methode** mit mindestens einer
    Wand kollidieren 

 

## Weitere Funktionalitätsanforderungen:

-   Die Splinefläche ist wie in der 2. Übung aufgebaut. Es gelten
    generell alle **Anforderungen der vorherigen Übung**, was die
    Normalenberechnung, die Beleuchtung, Vertexarrays, die
    Erweiterung/Verkleinerung und die Genauigkeit der Fläche sowie die
    Höhenänderung angeht. Auch die Tastenbelegungen sollen
    sinnvollerweise dieselben sein.  
    Der Kameraflug und die Texturierung sind nicht zwingend
    erforderlich.  
    Entgegen der 2. Aufgabe soll die gesamte Splinefläche diesmal eine
    leichte Neigung entlang einer Achse aufweisen, die - zusätzlich zu
    den lokalen, zufälligen Hügeln und Tälern - somit eine grobe
    Struktur vorgibt, damit die Kugeln von einer Seite zur anderen
    rollen.
-   An allen Seitenrändern der Landschaft befinden sich (sichtbare,
    unsichtbare oder halbtransparente) **Wände** (senkrechte,
    achsparallele Flächen), welche die Splinefläche dort abschließen und
    dafür sorgen, daß eine rollende Kugel diese nicht verlassen kann.
    Zwischen den Kugeln und den Wänden kommt es dabei zu Kollisionen.
-   Innerhalb der Fläche befinden sich zufällig verteilt 6 weitere
    kleine **Wandstücke** in Form länglicher Quader: 4 davon sollen
    achsparallel und orthogonal zur Hauptrollrichtung der Kugeln sein, 2
    achsparallel und parallel zur Hauptrollrichtung. Die Wände können
    vom Nutzer mit den Zahltasten selektiert werden, wobei die aktuell
    selektierte Wand durch eine leicht andere Farbe dargestellt werden
    soll. Die aktuell selektierte Wand kann vom Nutzer mit den Tasten
    L(eft), R(ight), U(p) und D(own) entlang der beiden Achsen auf der
    Fläche verschoben werden (gemeint sind Buchstaben-, nicht
    Pfeiltasten). Der Mittelpunkt des Quaders soll dabei jeweils genau
    auf Höhe der Fläche dort sein (q(s,t) ist also auszuwerten). Mehrere
    Wände dürfen sich gegenseitig durchdringen. Die rollenden Kugeln
    prallen von den Wänden ab und können somit in Richtung des Ziels
    (s.u.) gelenkt werden.
-   Bei einer **Kollision** mit einer Wand gilt grob das bekannte
    "Einfallswinkel = Ausfallswinkel"-Prinzip, das durch die
    **Penalty-Methode** umgesetzt werden soll (siehe Tips). Kollisionen
    erzeugen keine zusätzliche Reibung.
-   Am höchsten Ende der Fläche liegen zu Beginn zufällig verteilt
    direkt auf der Fläche in Ruheposition eine Anzahl von 10 (Konstante
    nutzen!) Kugeln, die bei einem Tastendruck auf S(tart) von dort aus
    entsprechend der Euler-Integration losrollen. Achtet darauf, daß
    eine neue Kugel nicht dort erscheint, wo sich schon eine andere
    befindet (ansonsten kommt es durch die Penalty-Methode zu einer
    großen Beschleunigung).  
    Die **Bewegung einer Kugel** hängt dabei von folgenden Einflüssen
    ab:  
    -   bisherige Bewegung
    -   Neigung der Fläche unter der Kugel
    -   Reibungsverluste
    -   Kollisionen

    Genaueres zur Berechnung der Einflüsse durch Euler-Integration und
    Penalty-Methode findet sich in den Tips.  
    Der Auflagepunkt der Kugeln befindet sich dabei immer auf der
    idealen Splinefläche, unabhängig von der aktuellen
    Zeichengenauigkeit.
-   Alle Kugeln **kollidieren** ggf. während des Rollens mit allen
    anderen Kugeln und den Wänden.
-   Die erste noch auf der Fläche vorhandene (siehe nächster Punkt zu
    schwarzen Löchern) Kugel kann vom Spieler mit der Taste a/A zu jedem
    Zeitpunkt (also egal, ob sie still liegt oder sich bewegt) einen
    **Anstoß** bekommen. Dieser wird in eine zufällige Richtung in der
    x-z-Ebene ausgeführt und soll von der Stärke her so eingestellt
    sein, daß er die Kugel deutlich sichtbar bewegt, aber auch nicht
    gleich aus dem Fenster herausschießt ;-)
-   In der Landschaft befinden sich eine Reihe von **schwarzen Löchern**
    in Form dunkler Kugeln mit Mittelpunkt direkt in der Splinefläche.
    Wenn eine rollende Kugel sich auf ihrem Weg über die Splinefläche
    einem schwarzen Loch zu sehr nähert (also seinen Wirkungsradius
    unterschreitet), dann wird sie vom Loch (genauer: von seinem
    Mittelpunkt) angezogen, so daß sich ihre Rollrichtung sichtbar
    ändert oder (bei zu direktem Kurs auf den Mittelpunkt) die Kugel
    sogar "verschluckt" wird und somit verschwindet. Diese Anziehung
    wird dabei mit der Beschleunigung aus dem Euler verrechnet.  
    Die schwarzen Löcher werden zufällig über die Splinefläche verteilt.
    Mit der Taste X kann man ein weiteres hinzufügen, Y entfernt das
    zuletzt hinzugefügte.
-   Am unteren Ende der Splinefläche befindet sich ein **Zielbereich**,
    dargestellt durch eine grüne halbtransparente Kugel. Sobald
    mindestens eine Kugel diesen Zielbereich erreicht, gilt das Spiel
    als gewonnen, was dem Nutzer durch eine Textausgabe mitgeteilt wird.
    Sind zwischenzeitlich alle Kugeln in schwarzen Löchern verschwunden,
    beginnt das Spiel mit 10 neuen Kugeln von vorne. Alternativ kann der
    Nutzer mit N(ew) jederzeit von sich aus neu beginnen.

 

## Tips:

1\) Es gelten natürlich weiterhin die Tips aus [Übung
2](https://intern.fh-wedel.de/mitarbeiter/ne/praktikum-geometrische-modellierung-und-computeranimation/aufgaben/aufgabe-2/).

2\) Auch die [Folien der
Einführung](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Euler-IntegrationUndPenalty-Methode.pdf)
könnten ganz hilfreich sein. Diese gibt es auch noch einmal in einer
"[extended
Version](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Euler-IntegrationUndPenalty-MethodeVerbessert.pdf)",
in der noch letzte kleine physikalische Ungenauigkeiten (siehe Tip 3)
behoben werden (dieser letzte Teil ist zum Bestehen der Aufgabe nicht
relevant und das genannte Problem kann u.U. auch durch eine einfache
Verringerung des Zeitintervalls zwischen zwei Berechnungsschritten
umgangen oder zumindest verringert werden).

3\) Beachtet, daß die Euler-Integration als Ergebnis einen Ort s
liefert, der im allgemeinen Fall *nicht* direkt auf der Fläche, sondern
leicht darüber bzw. darunter liegen wird (es geht ja nur der alte Ort
der Kugel sowie die Normale in diesem Punkt ein und nicht das Aussehen
der Fläche in der Umgebung!). Daher ist es nach der Euler-Integration
erforderlich, vom berechneten neuen Ort s die y-Komponente zu verwerfen
und die korrekte Höhe anhand der x- und z-Komponente des neuen Ortes zu
berechnen, die in die Flächengleichung q(s,t) des betroffenen
Flächenstücks (umgerechnet in s und t) eingesetzt werden.

4\) Sollten das Kugelrollverhalten oder die Kollisionen nicht gut genug
aussehen, probiert am besten etwas mit den Werten für Reibung,
Gravitation, Federkonstante etc. herum, bis Ihr eine Konstellation
gefunden habt, die für Eure Landschaft und Eure sonstigen Einstellungen
"echt" aussieht. Hier gilt es, eine gute Balance zwischen den diversen
Werten zu finden, die mit der verwendeten Splinefläche und ihrer
Umsetzung zusammenpaßt.
