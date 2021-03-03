#ifndef __UTIL_H__
#define __UTIL_H__

/**
 * @file
 * Nuetzliche Funktionen-Modul.
 * Dieses Modul beinhaltet nuetzliche Funktionen.
 *
 * 
 *
 * @author Michael Smirnov & Len Harmsen
 */

/* Utility Funktion zum setzen von 3 Farbwerten
 * @param dst worauf gesetzt wird
 * @param src wovon gesetzt wird
 */
void setColor(CGColor3f dst, CGColor3f src);

/**
 * Utility Funktion zum setzen der Farbe,
 * sowie die Materialeigenschaften des zu Zeichnenden Objektes
 * @param red Der Intensitaetswert fuer den roten Kanal
 * @param green Der Intensitaetswert fuer den gruenen Kanal
 * @param blue Der Intensitaetswert fuer den blauen Kanal
 * @param alpha Der Intensitaetswert fuer den alpha Kanal beim Material
 * @param face die Seite welche beim Material gesetzt werden soll
 * @param materialAttribute Materialeigenschaft der Flaeche die geaendert werden soll 
 * 
 */
void setMaterialAndColor(float red, float green, float blue, float alpha, GLenum face, GLenum materialAttribute);

/**
 * Berechnet den Richtungsvektor zwischen zwei Punkten im 3-D Raum
 * @param startPtr der Startpunkt
 * @param endPtr der Endpunkt
 * @param resPtr der Ergebnissvektor
 */
void calcVectorBetweenPoints(GLfloat *startPtr, GLfloat *endPtr, GLfloat *resPtr);

/**
 * Hilfsfunktion zum ausgaben einer 4x4 Matrix
 * @param m Zeiger auf das erste Element dieser Matrix
 */
void printMatrix(GLfloat *m);

/**
 * Hilfsfunktion um von Grad zu Radian zu konvertieren
 * @param degree Grad
 * @return Radian
 */
float degreeToRad(float degree);

/**
 * Hilfsfunktion zum begrenzen eines Wertes innerhalb eines Wertebereichs
 * @param value des Wert des zu Begrenzen ist
 * @param lower die untere Grenze
 * @param upper die obere Grenze
 * @return der potenziell korrigierte Wert
 */
float clip(float value, float lower, float upper);

/**
 * Hilfsfunktion welche zwei Strings zusammenfuegt
 * @param *s1 Zeiger auf den ersten String
 * @param *s2 Zeiger auf den zweiten String
 * @return Zeiger auf zusammengefuegten String (muss wieder gefreet werden!)
 */
char *concat(char *s1, char *s2);

/**
 * Bestimmt die Inverse einer 4x4 Matrix
 * Quelle :https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
 * @param m die zu invertierende Matrix
 * @param invOut die invertierte Matrix
 * @return ob die determinante == 0 (nicht loesbar) oder nicht
 */
GLboolean gluInvertMatrix(const GLfloat m[16], GLfloat invOut[16]);

/**
 * Hilfsfunktion welche zwei 4x4 Matrixen welche als 16-Elementige Arrays representiert
 * werden miteinander multipliziert. mat1*mat2 = res
 * @param mat1 Zeiger auf die ertste Matrix
 * @param mat2 Zeiger auf die zweite Matrix
 * @param res Zeiger auf die Ergebnissmatrix
 */
void multiply4x4With4x4Matrix(GLfloat *mat1, GLfloat *mat2, GLfloat *res);

/**
 * Berechnet aus der Modelmatrix dessen Position in Weltkoordinaten
 * @param viewMatrixPtr ein Zeiger auf die Aktuelle Viewmatrix
 * @param objectMatrixPtr ein Zeiger auf die Modelviewmatrix des zu bestimmenden Objektes
 * @param worldCoordinatesMatrixPtr Zeiger auf den Vektor in den das Ergebniss geschrieben werden soll (x,y,z)
 */
void calcWorldCoordinates(GLfloat *viewMatrixPtr, GLfloat *objectMatrixPtr, GLfloat *worldCoordinatesPtr);

/**
 * Berechnet das Kreuzprodukt zwischen zwei 3D Vektoren und schreibt das normalisierte Ergebnis auf res.
 * @param a Zeiger auf den ersten Vektor
 * @param b Zeiger auf den zweiten Vektor
 * @param res Zeiger auf das Arrayelemet in das das Erg. geschieben wird
 */
void calcCrossProduct(GLfloat *a, GLfloat *b, GLfloat *res);

/**
 * https://stackoverflow.com/questions/31225431/transpose-a-matrix-via-pointer-in-c
 * Transponiert eine Matrix beliebiger Groesse und gibt diese zurueck
 * @param n sind Zeilen
 * @param m sind Spalten
 */
float *transpose(float *matrix, int n, int m);

/**
 * Multipliziert dem Monomvektor S [1X4] mit der uebergebenen Matrix [4X4]
 * @param monomVectorS der Monomvektor S
 * @param mat4x4 die zu multiplizierende [4X4] Matrix
 * @param result das Ergebniss der multiplikation
 */
void multiply1x4With4x4Matrix(float *monomVectorS , float *mat4x4, float *result);

/**
 * Multipliziert den Monomvektor T mit der Interpolationsmatrix.
 * @param interpolation die Interpolationsmatrix
 * @param monomVectorT der Monomvektor T
 * @param result das Ergebniss des Multiplikation
 */
void multiply4x4With4x1Matrix(float* interpolation , float *monomVectorT, float *result);

/**
 * Multipiliert die uebergebenen Matrixen miteinander
 * @param m1x4 Zeiger auf die 1X4 Matrix
 * @param m4x1 Zeiger auf die 4X1 Matrix
 */
float multiply1x4With4x1Matrix(float *m1x4, float *m4x1);

/**
 * Multipiliert die uebergebenen Matrixen miteinander
 * @param m1x4 Zeiger auf die 1X4 Matrix
 * @param dimension die x, y oder z dimension der 1x4 Matrix welche zu berechnen ist.
 * @param m4x1 Zeiger auf die 4X1 Matrix
 */
float multiply1x4With4x1MatrixByDimension(CGVector3f *m1x4, int dimension, float *m4x1);

#endif