/**
 * @file
 * Nuetzliche Funktionen-Modul.
 * Dieses Modul beinhaltet nuetzliche Funktionen.
 *
 * 
 *
 * @author Michael Smirnov & Len Harmsen
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "debugGL.h"

/* ---- Eigene Header einbinden ---- */

#include "types.h"

/* Utility Funktion zum setzen von 3 Farbwerten
 * @param dst worauf gesetzt wird
 * @param src wovon gesetzt wird
 */
void setColor(CGColor3f dst, CGColor3f src)
{
    memcpy(dst, src, sizeof(CGColor3f));
}

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
void setMaterialAndColor(float red, float green, float blue, float alpha, GLenum face, GLenum materialAttribute)
{
    glColor3f(red, green, blue);
    CGColor4f material = {red, green, blue, alpha};
    glMaterialfv(face, materialAttribute, material);
}

/**
 * Berechnet den Richtungsvektor zwischen zwei Punkten im 3-D Raum
 * @param startPtr der Startpunkt
 * @param endPtr der Endpunkt
 * @param resPtr der Ergebnissvektor
 */
void calcVectorBetweenPoints(GLfloat *startPtr, GLfloat *endPtr, GLfloat *resPtr)
{
    resPtr[0] = endPtr[0] - startPtr[0];
    resPtr[1] = endPtr[1] - startPtr[1];
    resPtr[2] = endPtr[2] - startPtr[2];
}

/**
 * Hilfsfunktion zum ausgaben einer 4x4 Matrix
 * @param m Zeiger auf das erste Element dieser Matrix
 */
void printMatrix(GLfloat *m)
{
    for (int i = 0; i < 16; i += 4)
    {
        fprintf(stdout, "%f,%f,%f,%f\n", m[i], m[i + 1], m[i + 2], m[i + 3]);
    }
    printf("\n");
}

/**
 * Hilfsfunktion zum ausgaben einen 3x1 Vektor
 * @param m Zeiger auf das erste Element dieses Vectors
 */
void printVector(GLfloat *m)
{
    for (int i = 0; i < 3; i++)
    {
        fprintf(stdout, "%f\n", m[i]);
    }
    printf("\n");
}

/**
 * Berechnet Betrag/Laenge eines Vektors
 * @param a ein Vektor
 * @return der Betrag des Vektors
 */
float calcVectorLength(float *a)
{
    float res = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        res = res + pow(a[i], 2);
    }
    return sqrtf(res);
}

/**
 * Hilfsfunktion um von Grad zu Radian zu konvertieren
 * @param degree Grad
 * @return Radian
 */
float degreeToRad(float degree)
{
    return degree * (M_PI / 180);
}

/**
 * Hilfsfunktion zum begrenzen eines Wertes innerhalb eines Wertebereichs
 * @param value des Wert des zu Begrenzen ist
 * @param lower die untere Grenze
 * @param upper die obere Grenze
 * @return der potenziell korrigierte Wert
 */
float clip(float value, float lower, float upper)
{
    return fmin(upper, fmax(lower, value));
}

/**
 * Hilfsfunktion welche zwei Strings zusammenfuegt
 * @param *s1 Zeiger auf den ersten String
 * @param *s2 Zeiger auf den zweiten String
 * @return Zeiger auf zusammengefuegten String (muss wieder gefreet werden!)
 */
char *concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 fuer terminalsymbol
    if (result == NULL)
    {
        fprintf(stdout, "Fehler beim Malloc fuer concat");
        exit(1);
    }
    else
    {
        strcpy(result, s1);
        strcat(result, s2);
        return result;
    }
}

/**
 * Bestimmt die Inverse einer 4x4 Matrix
 * Quelle :https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
 * @param m die zu invertierende Matrix
 * @param invOut die invertierte Matrix
 * @return ob die determinante == 0 (nicht loesbar) oder nicht
 */
GLboolean gluInvertMatrix(const GLfloat m[16], GLfloat invOut[16])
{
    GLfloat inv[16], det;
    int i;

    inv[0] = m[5] * m[10] * m[15] -
             m[5] * m[11] * m[14] -
             m[9] * m[6] * m[15] +
             m[9] * m[7] * m[14] +
             m[13] * m[6] * m[11] -
             m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] +
             m[4] * m[11] * m[14] +
             m[8] * m[6] * m[15] -
             m[8] * m[7] * m[14] -
             m[12] * m[6] * m[11] +
             m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] -
             m[4] * m[11] * m[13] -
             m[8] * m[5] * m[15] +
             m[8] * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] +
              m[4] * m[10] * m[13] +
              m[8] * m[5] * m[14] -
              m[8] * m[6] * m[13] -
              m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] +
             m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] -
             m[9] * m[3] * m[14] -
             m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
             m[0] * m[11] * m[14] -
             m[8] * m[2] * m[15] +
             m[8] * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] +
             m[0] * m[11] * m[13] +
             m[8] * m[1] * m[15] -
             m[8] * m[3] * m[13] -
             m[12] * m[1] * m[11] +
             m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
              m[0] * m[10] * m[13] -
              m[8] * m[1] * m[14] +
              m[8] * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] -
             m[1] * m[7] * m[14] -
             m[5] * m[2] * m[15] +
             m[5] * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
             m[0] * m[7] * m[14] +
             m[4] * m[2] * m[15] -
             m[4] * m[3] * m[14] -
             m[12] * m[2] * m[7] +
             m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
              m[0] * m[7] * m[13] -
              m[4] * m[1] * m[15] +
              m[4] * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] +
              m[0] * m[6] * m[13] +
              m[4] * m[1] * m[14] -
              m[4] * m[2] * m[13] -
              m[12] * m[1] * m[6] +
              m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
             m[1] * m[7] * m[10] +
             m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] +
             m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
              m[0] * m[7] * m[9] +
              m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] +
              m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return GL_FALSE;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return GL_TRUE;
}

/**
 * https://stackoverflow.com/questions/31225431/transpose-a-matrix-via-pointer-in-c
 * Transponiert eine Matrix beliebiger Groesse und gibt diese zurueck
 * @param n sind Zeilen
 * @param m sind Spalten
 */
float *transpose(float *matrix, int n, int m)
{
    int i = 0;
    int j = 0;
    float num;
    float *transposed = malloc(sizeof(float) * n * m);
    while (i < n)
    {
        j = 0;
        while (j < m)
        {
            num = *(matrix + i * m + j);
            *(transposed + i + n * j) = num; // I changed how you index the transpose
            j++;
        }
        i++;
    }

    return transposed;
}

/**
 * Hilfsfunktion welche zwei 4x4 Matrixen welche als 16-Elementige Arrays representiert
 * werden miteinander multipliziert. mat1*mat2 = res
 * @param mat1 Zeiger auf die ertste Matrix
 * @param mat2 Zeiger auf die zweite Matrix
 * @param res Zeiger auf die Ergebnissmatrix
 */
void multiply4x4With4x4Matrix(GLfloat *mat1, GLfloat *mat2, GLfloat *res)
{
    for (int k = 0; k <= 12; k += 4)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0, mat2Count = 0; j < 4; j++, mat2Count += 4)
            {
                res[k + i] += mat1[k + j % 4] * mat2[mat2Count + i % 4];
            }
        }
    }
}

/**
 * Multipliziert dem Monomvektor S [1X4] mit der uebergebenen Matrix [4X4]
 * @param monomVectorS der Monomvektor S
 * @param mat4x4 die zu multiplizierende [4X4] Matrix
 * @param result das Ergebniss der multiplikation
 */
void multiply1x4With4x4Matrix(float *monomVectorS, float *mat4x4, float *result)
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        //Zur sicherheit mit 0 ueberschreiben
        result[i] = 0.0f;
        for (int j = 0; j < 16; j = j + 4)
        {
            result[i] += monomVectorS[count % 4] * mat4x4[j + i];
            count++;
        }
    }
}

/**
 * Multipliziert den Monomvektor T mit der Interpolationsmatrix.
 * @param interpolation die Interpolationsmatrix
 * @param monomVectorT der Monomvektor T
 * @param result das Ergebniss des Multiplikation
 */
void multiply4x4With4x1Matrix(float *interpolation, float *monomVectorT, float *result)
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        //Zur sicherheit mit 0 ueberschreiben
        result[i] = 0.0f;
        for (int j = 0; j < 4; j++)
        {
            result[i] += interpolation[count] * monomVectorT[count % 4];
            count++;
        }
    }
}

/**
 * Multipiliert die uebergebenen Matrixen miteinander
 * @param m1x4 Zeiger auf die 1X4 Matrix
 * @param m4x1 Zeiger auf die 4X1 Matrix
 */
float multiply1x4With4x1Matrix(float *m1x4, float *m4x1)
{
    float res = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        res += m1x4[i] * m4x1[i];
    }
    return res;
}

/**
 * Multipiliert die uebergebenen Matrixen miteinander
 * @param m1x4 Zeiger auf die 1X4 Matrix
 * @param dimension die x, y oder z dimension der 1x4 Matrix welche zu berechnen ist.
 * @param m4x1 Zeiger auf die 4X1 Matrix
 */
float multiply1x4With4x1MatrixByDimension(CGVector3f *m1x4, int dimension, float *m4x1)
{
    float res = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        res += m1x4[i][dimension] * m4x1[i];
    }
    return res;
}

/**
 * Berechnet aus der Modelmatrix dessen Position in Weltkoordinaten
 * @param viewMatrixPtr ein Zeiger auf die Aktuelle Viewmatrix
 * @param objectMatrixPtr ein Zeiger auf die Modelviewmatrix des zu bestimmenden Objektes
 * @param worldCoordinatesMatrixPtr Zeiger auf den Vektor in den das Ergebniss geschrieben werden soll (x,y,z)
 */
void calcWorldCoordinates(GLfloat *viewMatrixPtr, GLfloat *objectMatrixPtr, GLfloat *worldCoordinatesPtr)
{
    GLfloat viewMatrix[16] = {0};
    GLfloat invertedViewMatrix[16] = {0};
    GLfloat objectMatrix[16] = {0};
    GLfloat worldCoordinatesMatrix[16] = {0};
    /*Daten von Ptr. kopieren*/
    memcpy(viewMatrix, viewMatrixPtr, sizeof(GLfloat) * 16);
    memcpy(objectMatrix, objectMatrixPtr, sizeof(GLfloat) * 16);
    /*Inverse Berechnen*/
    if (gluInvertMatrix(viewMatrix, invertedViewMatrix))
    {
        multiply4x4With4x4Matrix(objectMatrix, invertedViewMatrix, worldCoordinatesMatrix);
        memcpy(worldCoordinatesPtr, &worldCoordinatesMatrix[12], sizeof(GLfloat) * 3);
    }
    else
    {
        printf("FELHER BEI BERECHNUNG DER INVERSE!");
    }
}

/**
 * Konvertiert die globalen Koordinaten eines Obj auf der Flaeche der Szene in S und T.
 * @param x Die X Koord.
 * @param z die Z Koord.
 * @param fieldWidth die aktuelle Breite in globalen Koordinaten
 * @param S Zeiger auf den Speicher wo die berechnete S Position geschieben wird.
 * @param T Zeiger auf den Speicher wo die berechnete T Position geschieben wird. 
 */
void convertGlobalCoorinatesToInterpolationInterval(float x, float z, float fieldWidth, float *S, float *T)
{
    *S = (z + (fieldWidth / 2)) / fieldWidth;
    *T = (x + (fieldWidth / 2)) / fieldWidth;
}

/**
 * Berechnet das Kreuzprodukt zwischen zwei 3D Vektoren und schreibt das normalisierte Ergebnis auf res.
 * @param a Zeiger auf den ersten Vektor
 * @param b Zeiger auf den zweiten Vektor
 * @param res Zeiger auf das Arrayelemet in das das Erg. geschieben wird
 */
void calcCrossProduct(GLfloat *a, GLfloat *b, GLfloat *res)
{
    float length;
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
    length = sqrt(res[0] * res[0] + res[1] * res[1] + res[2] * res[2]);
    res[0] /= length;
    res[1] /= length;
    res[2] /= length;
}

/**
 * Berechnet das Skalarprodukt
 * @param a erster Vektor
 * @param b zweiter Vektor
 * @return das Ergebniss
 */
float calcDotProduct(float *a, float *b)
{
    float res = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        res = res + a[i] * b[i];
    }
    return res;
}

/**
 * Multipliziert einen Skalar mit einem Vector
 * (1x3 Vektor)
 * @param v der zu multiplizierende Vector.
 * @param scalar der scalar.
 * @param res das Erbeniss der Berechnung.
 */
void multiplyVectorWithScalar(float *v, float scalar, float *res)
{
    res[0] = v[0] * scalar;
    res[1] = v[1] * scalar;
    res[2] = v[2] * scalar;
}

/**
 * Dividiert einen Vektor mit einem Scalar
 * (1x3 Vektor)
 * @param v der zu dividierende Vector.
 * @param scalar der scalar.
 * @param res das Erbeniss der Berechnung.
 */
void divideVectorWithScalar(float *v, float scalar, float *res)
{
    res[0] = v[0] / scalar;
    res[1] = v[1] / scalar;
    res[2] = v[2] / scalar;
}

/**
 * Kuemmert sich um die subtraktion zweier Vektoren.
 * b wird von a abgezogen. (b ist das Ziel)
 * @param a erster Vektor von dem subtrahiert wird
 * @param b zweiter Vektor der subtrahiert wird
 * @param res Ergbeniss Vektor auf den das Ergbeniss geschrieben wird
 */
void subtractVectos(float *a, float *b, float *res)
{
    res[0] = a[0] - b[0];
    res[1] = a[1] - b[1];
    res[2] = a[2] - b[2];
}

/**
 * Addiert zwei Vektoren
 * @param a erster Vektor auf den addiert wird
 * @param b zweiter Vektor der addiert wird
 * @param res Ergbeniss Vektor auf den das Ergbeniss geschrieben wird
 */
void addVectors(float *a, float *b, float *res)
{
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
}

/**
 * Setzt die Komponenten eines 3D Vektors
 * @param x die x Komponente
 * @param y die y Komponente
 * @param z die z Komponente
 * @param res der beschriebe Vektor
 */
void setVector(float x, float y, float z, float *res)
{
    res[0] = x;
    res[1] = y;
    res[2] = z;
}