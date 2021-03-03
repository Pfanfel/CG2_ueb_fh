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

void setColor(CGColor3f dst, CGColor3f src);

void setMaterialAndColor(float red, float green, float blue, float alpha, GLenum face, GLenum materialAttribute);

void calcVectorBetweenPoints(GLfloat *startPtr, GLfloat *endPtr, GLfloat *resPtr);

void rotateZ(CGVector3f vec, float angle);

void rotateY(CGVector3f vec, float angle);

void printMatrix(GLfloat *m);

void printVector(GLfloat *m);

float calcVectorLength(float *a);

float degreeToRad(float degree);

float radToDegree(float rad);

float clip(float value, float lower, float upper);

char *concat(char *s1, char *s2);

GLboolean gluInvertMatrix(const GLfloat m[16], GLfloat invOut[16]);

void multiply4x4With4x4Matrix(GLfloat *mat1, GLfloat *mat2, GLfloat *res);

void multiply1x4With4x4Matrix(float *monomVectorS, float *mat4x4, float *result);

void calcWorldCoordinates(GLfloat *viewMatrixPtr, GLfloat *objectMatrixPtr, GLfloat *worldCoordinatesPtr);

void calcCrossProduct(GLfloat *a, GLfloat *b, GLfloat *res);

float *transpose(float *matrix, int n, int m);

void multiply4x4With4x1Matrix(float *interpolation, float *monomVectorT, float *result);

float multiply1x4With4x1Matrix(float *m1x4, float *m4x1);

float multiply1x4With4x1MatrixByDimension(CGVector3f *m1x4, int dimension, float *m4x1);

void convertGlobalCoorinatesToInterpolationInterval(float x, float z, float fieldWidth, float *S, float *T);

float calcDotProduct(float *a, float *b);

void multiplyVectorWithScalar(float *v, float scalar, float *res);

void multiplyVectors(float *a, float *b, float *res);

void divideVectorWithScalar(float *v, float scalar, float *res);

void subtractVectos(float *a, float *b, float *res);

void addVectors(float *a, float *b, float *res);

void setVector(float x, float y, float z, float *res);

void normalizeVector(float *vec);

void copyVector(CGVector3f dst, CGVector3f src);

void copyMaterial(MaterialProperties *dst, MaterialProperties *src);

void reflectVector(float *vec, float *normal, float *res);

void scaleObject3f(float x, float y, float z, float *object);

void translateObject3f(float x, float y, float z, float *object);

void rotateObject3f(float angle, float u, float v, float w, float *object);
#endif