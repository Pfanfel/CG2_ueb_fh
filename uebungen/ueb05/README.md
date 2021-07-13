
# Aufgabe 5: Raytracer

### Neue Techniken:

Raytracing, Bounding-Boxes

###  Beschreibung:

![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Raytracer-Beispielszene.png)

In der letzten Aufgabe ist ein einfacher Raytracer zu erstellen, der eine
raumähnliche Szene mit wenigen Objekten darin zeigt.

Der Screenshot rechts zeigt eine mögliche Ergebnisansicht.

###  Kernanforderungen:

-   Mittels **Raytracing** wird ein Bild der Szene erzeugt
-   Dargestellt wird ein würfelförmiger Raum mit einer opaken Kugel,
    einem teiltransparenten Würfel und einem geladenen **3D-Modell**
-   Das 3D-Modell wird korrekt mit einer minimalen **Axis Aligned
    Bounding Box** versehen

###  Weitere Funktionalitätsanforderungen:

-   Zu modellieren ist wie in der 4. Aufgabe eine 3D-Szene, die einen
    **würfelförmigen Raum** zeigt (alle Seiten weiß/grau), auf dessen
    Boden sich eine blaue opake Kugel, ein roter teiltransparenter
    Würfel (leicht rotiert um die y-Achse und somit nicht parallel zur
    y-z-Ebene) und ein geladenes grünes **3D-Modell** des Stanford Bunny
    befinden.
-   Das 3D-Modell könnt Ihr Euch aus einer beliebigen Quelle in einem
    selbstgewählten Format lokal abspeichern und dann beim Programmstart
    laden. Wir empfehlen Euch eine einfache Variante des
    **obj-Formates** (nur mit Vertices und Faces) und stellen Euch das
    Modell auch in verschiedenen Auflösungen darin zur Verfügung
    ([grob](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/bunny152v300f.obj),
    [mittel](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/bunny1355v2641f.obj),
    [fein](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/bunny2503v4968f.obj),
    [sehr
    fein](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/bunny35947v69451f.obj)
    (evtl. fehlerhaft), [extrem
    fein](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/bunny72027v144046f.obj)).
    Zeilen mit einem \# stellen darin Kommentare dar, ein v am Anfang
    deutet auf einen Vertex hin, ein f auf ein Face aus Vertices der
    genannten Zeilen. Andere Formate sind auch zulässig, aber vermutlich
    schwieriger zu parsen.  
    Orientiert den Bunny so, daß er annähernd im 45°-Winkel steht und
    nicht achsparallel (siehe Screenshot).
-   Zusätzlich soll sich an der Rückwand ein **Spiegel** (ideal
    reflektierend) befinden, der so platziert ist, daß man die Objekte
    auf dem Boden zumindest teilweise darin sehen kann. Einen Rahmen
    braucht der Spiegel nicht, er kann einfach nur aus der
    "Spiegelfläche" bestehen.
-   Der **Augpunkt und die Projektionsfläche** sollen anhand der Tasten
    v/h/l/r/o/u so positioniert und orientiert werden können, daß der
    Raum mit allen Objekten darin jeweils "durch" eine der 6 Seiten des
    Würfels gut zu sehen ist. Die genauen Werte dazu sollt Ihr selbst
    festlegen. Der Spiegel ist logischerweise nur beim Blick durch die
    Vorderseite wirklich brauchbar. Ggf. ist es (je nach Position der
    Kamera) dazu erforderlich, daß die Primärstrahlen die eine
    Würfelwand von außen ungehindert passieren können (Sonderfall),
    damit man etwas sieht.
-   Es soll innerhalb des Raumes **zwei Punktlichtquellen** an deutlich
    unterschiedlichen Positionen geben, die mit der Tastatur einzeln
    ein- und ausgeschaltet werden können. Sie müssen nicht wie im
    Screenshot oben visualisiert werden (da die Lichtquellen dann durch
    die Visualisierung um sie herum immer verdeckt wären).
-   Das **Raytracing** ist anhand des in den Folien (siehe Tips)
    beschriebenen Verfahrens durchzuführen. Als lokales
    Beleuchtungsmodell soll das bekannte **Phong** verwendet werden
    (siehe Tips). Teiltransparente Objekte sollen den transmittierten
    Strahl nicht brechen. Wir gehen also davon aus, daß alle Objekte der
    Szene dieselbe Dichte haben. Teiltransparente Objekte werfen wie
    opake Objekte auch einen Schatten, dieser muß allerdings nicht
    abhängig von der Stärke der Transparenz oder der Intensität des
    Objektes eingefärbt werden. Objekte innerhalb von teiltransparenten
    Objekten werden mit Phong beleuchtet und werfen auch selbst einen
    Schatten (der Schattenstrahl "ignoriert" teiltransparente Objekte im
    Weg somit und stellt dann keine Verdeckung fest).
-   Legt selbst eine sinnvolle maximale **Rekursionstiefe** fest, die
    einen brauchbaren Kompromiß zwischen Qualität des Ergebnisses und
    Berechnungsdauer darstellt. Auch die Werte für den **Reflexions- und
    den Brechungskoeffizienten** sowie für eine minimale beigetragene
    Intensität, ab der sonst das Rekursionsverfahren abgebrochen wird,
    sind von Euch sinnvoll zu setzen. Ebenso muß eine
    **Abschwächungsfunktion** je nach Abstand definiert werden.
-   Macht Euch rechtzeitig ausführliche Gedanken um die **Schnittests**
    zwischen einem Strahl und den vorhandenen Objekten. Da hier der
    Hauptberechnungsaufwand steckt, ist ein effizientes Verfahren
    immanent wichtig! Unnötig aufwendige oder langsame Verfahren können
    zu Punktabzügen führen!
-   Für den Bunny soll zudem eine Bounding Box genutzt werden, um den
    Rechenaufwand zu verringern. Im einfachsten Fall ist dies eine
    **Axis Aligned Bounding Box** (AABB). Hier sind also einfach die
    jeweils kleinsten und größten Koordinaten des Bunnys entlang der
    drei Achsen zu bestimmen und die Box entsprechend zu positionieren.
    Durch einen Tastendruck auf b soll die Box teiltransparent ein- und
    ausgeblendet werden können.
-   Alternativ zur AABB soll es auch möglich sein, eine **Oriented
    Bounding Box** (OBB) um den Bunny zu erstellen und zu nutzen. Diese
    steht zwar ebenfalls direkt auf dem Boden (also auf der x-z-Ebene),
    ist aber so um die y-Achse rotiert, daß das Boxvolumen minimal ist.
    Hierzu soll nicht die (komplizierte) Hauptkomponentenanalyse
    (Principal Component Analysis, PCA) benutzt werden, sondern
    folgendes einfacheres
    Verfahren:
    
    ![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/OBB-Beispielbild.png)
    -   es wird der Mittelpunkt des 3D-Modells berechnet
        (komponentenweise Mittelung aller Vertexkoordinaten)
    -   durch den Mittelpunkt wird eine Gerade gelegt, die sich in der
        x-z-Ebene befindet. Diese Gerade wird in 5°-Schritten rotiert
        (von 0° bis 175°)
    -   für jeden Rotationsschritt werden alle Vertices des 3D-Modells
        auf die Gerade "projiziert" (siehe Tips) und es werden somit die
        beiden "Extremwerte" (mit den größten Entfernungen zum
        Mittelpunkt) auf der Geraden bestimmt
    -   dasselbe passiert in jedem Rotationsschritt für eine weitere
        Gerade, die rechtwinklig auf der ersten steht: Auch hier werden
        durch Projektion aller Vertices des 3D-Modells die Extremwerte
        bestimmt
    -   zusammen ergibt sich so ein Rechteck (als Produkt der Strecken
        auf beiden Geraden zwischen den jeweiligen Extremwerten). Das
        Rechteck mit dem kleinsten Flächeninhalt führt offensichtlich
        zur optimalen OBB

    Auch die OBB soll mit der Taste b teiltransparent ein- und
    ausgeblendet werden können, zusätzlich soll die Taste n ein
    Umschalten zwischen \<kein Bounding Volume verwendet>, AABB und OBB
    ermöglichen.
-   Die **Auflösung des Fensters** soll über Konstanten im Code änderbar
    sein (um insb. den erforderlichen Rechenaufwand beeinflussen zu
    können).
-   Beachtet die allgemeinen Anforderungen! Abweichend davon gilt in
    dieser Aufgabe:  
    -   die Objekte müssen nicht aus Einheitsobjekten erzeugt werden
    -   die Fenstergröße muß nicht zur Laufzeit änderbar sein
    -   es muß keine Wireframeansicht geben
    -   die Hilfe soll aus Zeitgründen in der Konsole statt im Fenster
        ausgegeben werden
    -   die Normalen müssen nicht angezeigt werden können

###  Tips:

1\) Als Einstieg sind sicherlich die Folien zum
[Raytracing](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Raytracing.pdf)
selbst und zu [Bounding
Volumes](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Bounding_Volumes.pdf)
hilfreich.

2\) Achtet bei den obj-Modellen auf korrekte Indizes: Die aus den Faces
referenzierten Vertices beginnen bei 1, nicht bei 0!

3\) Zum Zeichnen gibt es zwei Möglichkeiten:

-   als Primitivtyp GL_POINTS nutzen und mittels glVertex2f die
    berechneten Intensitäten Punkt für Punkt setzen
-   die Berechnungsergebnisse in ein Array schreiben und dies mit
    glDrawPixels direkt in den Framebuffer kopieren

4\) Effiziente Schnittests zwischen Strahl und Dreieck bzw. Strahl und
Kugel sind immens wichtig, um ein performantes Programm zu erstellen.
Investiert hier vorab ausreichend Zeit, um Euch ein gutes mathematisches
Verfahren zu suchen!

5\) Ihr müßt nicht ständig (rechenaufwendig) neu zeichnen. Da alle
Objekte in der Szene statisch sind reicht es, nur bei einer Änderung der
Ansicht einmal neu zu rendern.

![](https://intern.fh-wedel.de/fileadmin/mitarbeiter/ne/CG2/Skalarprodukt_Punkt_auf_Gerade.png)

6\) Die "Projektion" eines Punktes auf eine Gerade kann z.B. durch ein
Skalarprodukt erfolgen. Die Gerade g in der aktuellen Rotation wird als
ein normierter Vektor dargestellt, der zweite Vektor ergibt sich als
Verbindung des zu prüfenden Punktes P_(i) zum berechneten Mittelpunkt M
(nicht normiert!!). Bildet man nun das Skalarprodukt, erhält man als
Ergebnis den Abstand des "projizierten" Punktes vom Mittelpunkt
(Beispiel siehe rechts).

7\) Durch Fließkommaungenauigkeiten kann es passieren, daß ein von einem
Schnittpunkt rekursiv weiterverfolgter Strahl das "gerade verlassene"
Objekt angeblich noch einmal schneidet und so wieder derselbe
Schnittpunkt als nähester zurückgeliefert wird. Bedenkt dies ggf. bei
Euren Schnitttests und schließt diesen dann ggf. aus.

8\) Falls wer von Euch im Makefile beim gcc das Flag -g (für den Debug
Build) nutzt: Laßt das sein, das ist deutlich langsamer :o)  
Stattdessen empfiehlt sich das Flag -O3, was die Berechnungen durch
Optimierung teilweise um die Hälfte beschleunigt.