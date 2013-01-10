/*********************************************************************
 *  CMPSC 457                                                        *
 *  Template code for HW 3                                           *
 *  March 29, 2010                                                    *
 *  Charlie Crandallv                                                *
 *  cdc5064@psu.edu                                                  *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  Header file for matrix manipulation                              *
 *  This is not a full blown-up implementation of matrices           *
 *  It contains just enough stuffs for this assignment               *
 *********************************************************************/  


#ifndef _MATRICES_H
#define _MATRICES_H



// 3D points / vectors
struct Vector3
{
  double x, y, z;
};
typedef Vector3 Point3;


// 3D points / vectors in Homogeneous coordinates
struct HVector3
{
  double x, y, z, w;
};
typedef HVector3 HPoint3;


// 4-by-4 matrix
struct Matrix4
{
  double elem[4][4];
};



//
// function prototypes
//

HPoint3 Pt3toHPt3(Point3 pt);  // Point3 to HPoint3
Point3  HPt3toPt3(HPoint3 pt); // HPoint3 to Point3

Vector3 crossVector(Vector3 v1, Vector3 v2);

Matrix4 IdentMatrix();                // returns 4x4 identity matrix
Matrix4 Mult4(Matrix4 a, Matrix4 b);  // returns ab

HPoint3 TransHPoint3(Matrix4 m, HPoint3 p);  // returns mp
HPoint3 Homogenize(HPoint3 a);               // returns homogenized HPoint3

Matrix4 SetScaleMatrix(double sx,double sy,double sz); // 4x4 scale matrix
Matrix4 SetTransMatrix(double tx,double ty,double tz); // 4x4 translation matrix
Matrix4 SetRotMatrix(Vector3 n,double angle);          // 4x4 rotation matrix

void PrintMat(Matrix4 m);     // print Matrix4
void PrintHPoint(HPoint3 p);  // print HPoint3
void PrintPoint(Point3 p);    // print Point3


#endif
