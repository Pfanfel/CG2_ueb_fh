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
    CGColor4f material = { red, green, blue, alpha };
    glMaterialfv(face, materialAttribute, material);
}

/**
 * Berechnet den Richtungsvektor zwischen zwei Punkten im 3-D Raum
 * @param startPtr der Startpunkt
 * @param endPtr der Endpunkt
 * @param resPtr der Ergebnissvektor
 */
void calcVectorBetweenPoints(GLfloat* startPtr, GLfloat* endPtr, GLfloat* resPtr)
{
    resPtr[0] = endPtr[0] - startPtr[0];
    resPtr[1] = endPtr[1] - startPtr[1];
    resPtr[2] = endPtr[2] - startPtr[2];
}

/**
 * Hilfsfunktion zum ausgaben einer 4x4 Matrix
 * @param m Zeiger auf das erste Element dieser Matrix
 */
void printMatrix(GLfloat* m)
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
void printVector(GLfloat* m)
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
float calcVectorLength(float* a)
{
    float res = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        res = res + a[i] * a[i];
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
    return degree * (M_PI / 180.0f);
}

/**
 * Hilfsfunktion um von Radian zu Grad zu konvertieren
 * @param rad Radian
 * @return Grad
 */
float radToDegree(float rad)
{
    return (rad * 180.0f) / M_PI;
}

// struct Quaternion {
//     double w, x, y, z;
// };

// struct EulerAngles {
//     double roll, pitch, yaw;
// };

// EulerAngles ToEulerAngles(Quaternion q) {
//     EulerAngles angles;

//     // roll (x-axis rotation)
//     double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
//     double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
//     angles.roll = std::atan2(sinr_cosp, cosr_cosp);

//     // pitch (y-axis rotation)
//     double sinp = 2 * (q.w * q.y - q.z * q.x);
//     if (std::abs(sinp) >= 1)
//         angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
//     else
//         angles.pitch = std::asin(sinp);

//     // yaw (z-axis rotation)
//     double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
//     double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
//     angles.yaw = std::atan2(siny_cosp, cosy_cosp);

//     return angles;
// }

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
char* concat(char* s1, char* s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 fuer terminalsymbol
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
float* transpose(float* matrix, int n, int m)
{
    int i = 0;
    int j = 0;
    float num;
    float* transposed = malloc(sizeof(float) * n * m);
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
void multiply4x4With4x4Matrix(GLfloat* mat1, GLfloat* mat2, GLfloat* res)
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
void multiply1x4With4x4Matrix(float* monomVectorS, float* mat4x4, float* result)
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
void multiply4x4With4x1Matrix(float* interpolation, float* monomVectorT, float* result)
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
float multiply1x4With4x1Matrix(float* m1x4, float* m4x1)
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
float multiply1x4With4x1MatrixByDimension(CGVector3f* m1x4, int dimension, float* m4x1)
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
void calcWorldCoordinates(GLfloat* viewMatrixPtr, GLfloat* objectMatrixPtr, GLfloat* worldCoordinatesPtr)
{
    GLfloat viewMatrix[16] = { 0 };
    GLfloat invertedViewMatrix[16] = { 0 };
    GLfloat objectMatrix[16] = { 0 };
    GLfloat worldCoordinatesMatrix[16] = { 0 };
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
void convertGlobalCoorinatesToInterpolationInterval(float x, float z, float fieldWidth, float* S, float* T)
{
    *S = (z + (fieldWidth / 2)) / fieldWidth;
    *T = (x + (fieldWidth / 2)) / fieldWidth;
}

/**
 * Berechnet das Kreuzprodukt zwischen zwei 3D Vektoren und schreibt das Ergebnis auf res.
 * @param a Zeiger auf den ersten Vektor
 * @param b Zeiger auf den zweiten Vektor
 * @param res Zeiger auf das Arrayelemet in das das Erg. geschrieben wird
 */
void calcCrossProduct(GLfloat* a, GLfloat* b, GLfloat* res)
{
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * Berechnet das Skalarprodukt
 * @param a erster Vektor
 * @param b zweiter Vektor
 * @return das Ergebniss
 */
float calcDotProduct(float* a, float* b)
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
void multiplyVectorWithScalar(float* v, float scalar, float* res)
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
void divideVectorWithScalar(float* v, float scalar, float* res)
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
void subtractVectos(float* a, float* b, float* res)
{
    res[0] = a[0] - b[0];
    res[1] = a[1] - b[1];
    res[2] = a[2] - b[2];
}

/**
 * Addiert zwei Vektoren
 * @param a erster Vektor auf den addiert wird
 * @param b zweiter Vektor der addiert wird
 * @param res Ergbenis Vektor auf den das Ergbeniss geschrieben wird
 */
void addVectors(float* a, float* b, float* res)
{
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];
    res[2] = a[2] + b[2];
}

/**
 * Mult. zwei Vektoren
 * @param a erster Vektor auf den addiert wird
 * @param b zweiter Vektor der addiert wird
 * @param res Ergbenis Vektor auf den das Ergbeniss geschrieben wird
 */
void multiplyVectors(float* a, float* b, float* res)
{
    res[0] = a[0] * b[0];
    res[1] = a[1] * b[1];
    res[2] = a[2] * b[2];
}

/**
 * Setzt die Komponenten eines 3D Vektors
 * @param x die x Komponente
 * @param y die y Komponente
 * @param z die z Komponente
 * @param res der beschriebe Vektor
 */
void setVector(float x, float y, float z, float* res)
{
    res[0] = x;
    res[1] = y;
    res[2] = z;
}

/**
 * Kopier einen Vektor in einen anderen
 * @param dst Ziel
 * @param src Source
 */
void copyVector(CGVector3f dst, CGVector3f src)
{
    dst[X] = src[X];
    dst[Y] = src[Y];
    dst[Z] = src[Z];
}

/**
 * Kopiert Materialeigenschaften
 * @param dst Ziel
 * @param src Source
 */
void copyMaterial(MaterialProperties* dst, MaterialProperties* src)
{
    (*dst).kReflection = (*src).kReflection;
    (*dst).kTransmission = (*src).kTransmission;
    (*dst).materialAmbient = (*src).materialAmbient;
    (*dst).materialDiffuse = (*src).materialDiffuse;
    (*dst).materialSpecular = (*src).materialSpecular;
}
/**
 * Normalisiert einen Vektor.
 * @param vec zu normalisiernder Vektor
 */
void normalizeVector(float* vec)
{
    float length = calcVectorLength(vec);
    divideVectorWithScalar(vec, length, vec);
}

/**
 * Rotiert den vector um den Winkel um die Z Achse
 */
void rotateZ(CGVector3f vec, float angle)
{
    float cosAngle = cos(degreeToRad(angle));
    float sinAngle = sin(degreeToRad(angle));
    float tempX = vec[X] * cosAngle - vec[Y] * sinAngle;
    float tempY = vec[X] * sinAngle + vec[Y] * cosAngle;
    vec[X] = tempX;
    vec[Y] = tempY;
}

/**
 * Rotiert den vector um den Winkel um die Y Achse
 */
void rotateY(CGVector3f vec, float angle)
{
    float cosAngle = cos(degreeToRad(angle));
    float sinAngle = sin(degreeToRad(angle));
    float tempX = vec[X] * cosAngle + vec[Z] * sinAngle;
    float tempZ = -vec[X] * sinAngle + vec[Z] * cosAngle;
    vec[X] = tempX;
    vec[Z] = tempZ;
}

/**
 * Reflektiert einen Vector um eine Normale
 * @param vec Eintreffender Vektor
 * @param normal Normale um die reflektiert wird
 * @param res der reflektierte vektor
 */
void reflectVector(float* vec, float* normal, float* res)
{
    CGColor3f projected = { 0 };
    float projFactor = calcDotProduct(vec, normal);
    multiplyVectorWithScalar(normal, projFactor, projected);
    multiplyVectorWithScalar(projected, 2.0f, projected);
    subtractVectos(projected, vec, res);
}

/**
 * Verschiebt ein Objekt in x y und z Richtung.
 * @param x Verschiebung in x-Richtung
 * @param y Verschiebung in y-Richtung
 * @param z Verschiebung in z-Richtung
 * @param object ein Vertice des Objekts welches verschoben werden soll
 *
 */
void translateObject3f(float x, float y, float z, float* object)
{
    float v[4] = { object[X], object[Y], object[Z], 1 };
    float res[4] = { 0 };
    float M[16] = { 1.0f, 0.0f, 0.0f, x,
                   0.0f, 1.0f, 0.0f, y,
                   0.0f, 0.0f, 1.0f, z,
                   0.0f, 0.0f, 0.0f, 1.0f };

    multiply4x4With4x1Matrix(M, v, res);
    object[X] = res[X];
    object[Y] = res[Y];
    object[Z] = res[Z];
}

/**
 * Skaliert ein Objekt.
 * @param x x-Skalierung
 * @param y y-Skalierung
 * @param z z-Skalierung
 * @param object ein Vertice des Objekts welches skaliert werden soll
 *
 */
void scaleObject3f(float x, float y, float z, float* object)
{
    float v[4] = { object[X], object[Y], object[Z], 1 };
    float res[4] = { 0 };
    float M[16] = { x, 0.0f, 0.0f, 0.0f,
                   0.0f, y, 0.0f, 0.0f,
                   0.0f, 0.0f, z, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f };

    multiply4x4With4x1Matrix(M, v, res);
    object[X] = res[X];
    object[Y] = res[Y];
    object[Z] = res[Z];
}

/**
 * Rotiert ein Objekt um die x y und z Achse.
 * @param angle der Winkel um den Rotiert wird
 * @param x Rotation um die x-Achse
 * @param y Rotation um die y-Achse
 * @param z Rotation um die z-Achse
 */
void rotateObject3f(float angle, float u, float v, float w, float* object)
{
    float L = (u * u + v * v + w * w);
    angle = angle * M_PI / 180.0; //converting to radian value
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;
    float rotationMatrix[16] = { 0 };
    rotationMatrix[0] = (u2 + (v2 + w2) * cos(angle)) / L;
    rotationMatrix[1] = (u * v * (1 - cos(angle)) - w * sqrt(L) * sin(angle)) / L;
    rotationMatrix[2] = (u * w * (1 - cos(angle)) + v * sqrt(L) * sin(angle)) / L;
    rotationMatrix[3] = 0.0;
    rotationMatrix[4] = (u * v * (1 - cos(angle)) + w * sqrt(L) * sin(angle)) / L;
    rotationMatrix[5] = (v2 + (u2 + w2) * cos(angle)) / L;
    rotationMatrix[6] = (v * w * (1 - cos(angle)) - u * sqrt(L) * sin(angle)) / L;
    rotationMatrix[7] = 0.0;
    rotationMatrix[8] = (u * w * (1 - cos(angle)) - v * sqrt(L) * sin(angle)) / L;
    rotationMatrix[9] = (v * w * (1 - cos(angle)) + u * sqrt(L) * sin(angle)) / L;
    rotationMatrix[10] = (w2 + (u2 + v2) * cos(angle)) / L;
    rotationMatrix[11] = 0.0;
    rotationMatrix[12] = 0.0;
    rotationMatrix[13] = 0.0;
    rotationMatrix[14] = 0.0;
    rotationMatrix[15] = 1.0;

    float vec[4] = { object[X], object[Y], object[Z], 1 };
    float res[4] = { 0 };
    multiply4x4With4x1Matrix(rotationMatrix, vec, res);
    object[X] = res[X];
    object[Y] = res[Y];
    object[Z] = res[Z];
}