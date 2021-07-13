
# Aufgabe 4: Partikelsimulation

### Neue Techniken:

Partikelsimulation

###  Beschreibung:

![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS1415_Screenshot5.jpg)

In der vierten Aufgabe widmen wir uns einer Partikelsimulation. Rund um
zwei Zielobjekte fliegen etliche Partikel herum, deren Flugbahnen sich
dabei mittels Euler-Integration einem realistischen Flugverhalten
annähern. 

Auch zu dieser Aufgabe gibt es ein Beispielprogramm
für [Windows](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_particle.exe) und [Linux](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_particle).
Beide benötigen jeweils
zwei [Texturen](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/WS2021_Textures.rar) im
Unterordner "Textures".

###  Kernanforderungen:

-   Es gibt mindestens ein **Partikel**, das sich an den Zielobjekten
    mittels Euler-Integration ausrichtet
-   **Zielobjekt** können zwei Kugeln, ein anderes Partikel oder das
    Zentrum aller Partikel sein (umschaltbar)
-   Das Partikel wird korrekt **orientiert** dargestellt

###  Weitere Funktionalitätsanforderungen:

-   Das Programm zeigt eine **3D-Szene** mit Beleuchtung, die einen
    Würfel aus Boden, 4 Wänden und einer Decke beinhaltet. Der Boden
    soll dabei anders als die Wände und die Decke aussehen, damit man
    sich grob orientieren kann. An den Wänden und der Decke können zwei
    verschiedene Texturen dargestellt werden (Taste t/T zum Wechseln),
    der Boden soll immer eine feste Textur haben.
-   Die erste **Kamera** kann auf einer Kugeloberfläche um den
    Mittelpunkt der Szene herumbewegt werden (linke Maustaste +
    Mausbewegung) und bietet überdies eine Zoomfunktion (mittlere
    Maustaste).
-   In der Szene befinden sich **zwei Kugeln als Zielobjekte**. Initial
    ruhen diese fest im Raum. Mit m/M können sie in Bewegung versetzt
    werden, wobei dann jeweils per Zufall der Startzeitpunkt der
    Bewegung und die neue Zielposition (innerhalb des Würfels) ermittelt
    werden sollen. Zwischen der aktuellen und der neuen Position wird
    die Bewegung linear interpoliert dargestellt.
-   Um die Zielobjekte herum fliegen **diverse Partikel**, deren Anzahl
    mittels der Pfeiltasten regelbar ist. Ihre Bewegung wird mit der
    **Euler-Integration** berechnet (s. Tips), wobei sie sich stets an
    beiden Zielobjekten ausrichten (benutzt dabei bitte eine Gewichtung
    nach Gauß, wie in den Folien beschrieben). Jedes Partikel hat nicht
    nur einen Ort, sondern auch eine Orientierung im Raum.
-   Mit d/D kann man zwischen zwei **Darstellungsformen** für die
    Partikel hin- und herschalten: einer Liniendarstellung und einer
    Dreiecksdarstellung. In beiden Varianten sollen die Partikel jeweils
    zweifarbig dargestellt werden, damit man ihre Lage besser erkennen
    kann. In der Dreiecksdarstellung fliegen die Partikel mit der
    Dreiecksspitze voraus.
-   Die Taste s/S erzeugt auf dem Boden für jedes Partikel einen
    "**Schatten**". Dieser soll allerdings nicht aufwendig berechnet
    werden, sondern einfach als Projektion entlang der y-Achse knapp
    über dem Boden in einem dunkleren Grau-/Farbton gezeichnet werden.
-   Zusätzlich zur Kugelkamera soll es noch eine **zweite Kamera**
    geben, mit der man quasi auf einem Partikel "mitfliegen" kann. Dazu
    ist die o.g. Orientierung des Partikels wichtig, damit man im LookAt
    alle Werte angeben kann. Die Orientierung des Partikels ergibt sich
    dabei aus der berechneten und gewichteten Beschleunigung in Richtung
    der beiden Zielobjekte (siehe Material, doppeltes Kreuzprodukt auf
    der letzten Folie). Mit n/N kann man jeweils mit der Kamera zum
    nächsten Partikel wechseln. Die Partikelkamera befindet sich dabei
    leicht über dem hinteren Rand (bzw. Ende bei Liniendarstellung) des
    Partikels und schaut über die Partikelspitze nach vorne.  
    Das aktuell ausgewählte Partikel soll mit anderen Farben dargestellt
    werden, damit man es leicht im gesamten Schwarm ausfindig machen
    kann.
-   Alternativ zu den Kugeln als Zielobjekten soll es auch möglich sein,
    das **Zentrum aller Partikel** oder **ein** **Partikel als
    Zielobjekt** zu definieren. Dieses eine Partikel (immer dasjenige,
    das auch die o.g. Partikelkamera "trägt") bewegt sich weiterhin zu
    den beiden Kugeln hin, alle anderen Partikel verfolgen aber dann
    dieses "Führungspartikel". Das Wechseln mittels n/N zum nächsten
    Partikel muß hier natürlich ebenfalls funktionieren. Das verfolgte
    Partikel soll dabei temporär einen höheren Wert für k_(v) bekommen,
    weil es sonst leicht im Schwarm der Verfolger verschwindet und nicht
    mehr wirklich sichtbar ist. Mit der Taste z/Z kann das Zielobjekt
    hier zyklisch zwischen Kugeln, Zentrum und Partikel tauschen.
-   Drückt man p/P, frieren alle Partikel und die Zielobjekte ein. Die
    Kugelkamera soll aber nach wie vor funktionieren, so daß man sich
    durch die **erstarrten Partikel** hindurchbewegen kann.
-   Beachtet die allgemeinen Anforderungen!

###  Tips:

1\) Die physikalischen Hintergründe zu Partikelsystemen finden sich noch
einmal in den [Folien der
Einführung](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Partikel.pdf).