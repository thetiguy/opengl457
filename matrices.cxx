/********************************************************************* 457                                                        *
 *  Template code for HW 3                                           *
 *  March 2, 2010                                                    *
 *  Charlie Crandall                                                 *
 *  cdc5064@psu.edu                                                  *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  Implementation file for matrix manipulation                      *
 *  This is not a full blown-up implementation of matrices           *
 *  It contains just enough stuffs for this assignment               *
 *********************************************************************/  


#include <iostream>
#include "matrices.h"
#include <math.h>

using std::cerr;


// appends a 1.0 to a Point3 to generate HPoint3
HPoint3 Pt3toHPt3(Point3 pt)
{
  HPoint3 temp;
  temp.x = pt.x;  temp.y = pt.y;  temp.z = pt.z;  temp.w = 1.0;
  return temp;
}


// returns a homogenized Point3, given an HPoint3 as input
Point3 HPt3toPt3(HPoint3 pt)
{
  Point3 temp;
  HPoint3 temp1 = Homogenize(pt);
  temp.x = temp1.x;  temp.y = temp1.y;  temp.z = temp1.z;
  return temp;
}


// returns a 4x4 identity matrix
Matrix4 IdentMatrix()
{
  Matrix4 m;
  for (int i=0; i<4; i++)   // set all elements to zero
    for (int j=0; j<4; j++)
      m.elem[i][j] = 0.0;

  for (int i=0; i<4; i++)   // set diagonal elements to one
    m.elem[i][i] = 1.0;

  return m;
}
 

// returns the product of two 4x4 matrices
Matrix4 Mult4(Matrix4 a, Matrix4 b)
{
  Matrix4 m;
  register double sum;

  for (int j=0; j<4;  j++) 
    for (int i=0; i<4; i++) {
      sum = 0.0;
      for (int k=0; k<4; k++)
	sum +=  a.elem[j][k] * b.elem[k][i];
      m.elem[j][i] = sum;
    }

  return m;
}


// returns the homogeneous 3d point as a result of
// multiplying a 4x4 matrix with a homogeneous point
HPoint3 TransHPoint3(Matrix4 m, HPoint3 p)
{
  HPoint3 temp;
  temp.x = m.elem[0][0]*p.x + m.elem[0][1]*p.y + m.elem[0][2]*p.z + m.elem[0][3]*p.w;
  temp.y = m.elem[1][0]*p.x + m.elem[1][1]*p.y + m.elem[1][2]*p.z + m.elem[1][3]*p.w;
  temp.z = m.elem[2][0]*p.x + m.elem[2][1]*p.y + m.elem[2][2]*p.z + m.elem[2][3]*p.w;
  temp.w = m.elem[3][0]*p.x + m.elem[3][1]*p.y + m.elem[3][2]*p.z + m.elem[3][3]*p.w;
  return temp;
}


// returns the result of homogenization of the input point
HPoint3 Homogenize(HPoint3 a)
{
  HPoint3 p;
  if ((a.w) != 0.0) {
    p.x = a.x /(a.w);  p.y = a.y /(a.w); 
    p.z = a.z /(a.w);  p.w = 1.0;
  }
  else {
    cerr << "Cannot Homogenize, returning original point\n";
    p.x = a.x;  p.y = a.y;  p.z = a.z;  p.w = a.w;
  }
  return p;
}





// returns a 4x4 scale matrix, given sx,sy,sz as inputs 
Matrix4 SetScaleMatrix(double sx, double sy, double sz)
{
  Matrix4 m;
  m = IdentMatrix();
  m.elem[0][0] = sx;
  m.elem[1][1] = sy;
  m.elem[2][2] = sz;
  return m;
}
    
// returns a 4x4 translation matrix, given tx,ty,tz as inputs 
Matrix4 SetTransMatrix(double tx, double ty, double tz)
{
  Matrix4 m;
  m = IdentMatrix();
  m.elem[0][3] = tx; m.elem[1][3] = ty; m.elem[2][3] = tz;
  return m;
}


// returns a 4x4 rotation matrix, given an axis and an angle 
Matrix4 SetRotMatrix(Vector3 n, double angle)
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* to set up the rotation matrix according to   */
  /* the specification in the handout             */
  /************************************************/
  Matrix4 m;
  m = IdentMatrix();
  
  m.elem[0][0] = cos(angle) + ( n.x*n.x )*( 1-cos(angle) ); m.elem[0][1] = n.x*n.y*( 1-cos(angle) )-n.z*sin(angle); m.elem[0][2] = n.x*n.z*( 1-cos(angle) )+ n.y*sin(angle); m.elem[0][3] = 0;
  
  m.elem[1][0] = n.y*n.x*( 1-cos(angle) )+n.z*sin(angle); m.elem[1][1] = cos(angle) + ( n.y*n.y )*( 1-cos(angle) ); m.elem[1][2] = n.y*n.z*( 1-cos(angle) )-n.x*sin(angle); m.elem[1][3] = 0;
  
  m.elem[2][0] = n.z*n.x*( 1-cos(angle) )-n.y*sin(angle); m.elem[2][1] = n.z*n.y*( 1-cos(angle) )+n.x*sin(angle); m.elem[2][2] = cos(angle) + ( n.z*n.z )*( 1-cos(angle) ); m.elem[2][3] = 0;
  
  m.elem[3][0] = 0; m.elem[3][1] = 0; m.elem[3][2] = 0; m.elem[3][3] = 1;
  
  
  
  return m;
}







//
// for your convenience when debugging
//

// prints a 4x4 matrix
void PrintMat(Matrix4 m)
{
   for (int i=0;i<4;i++) {
     for (int j=0;j<4;j++) {
       std::cerr << m.elem[i][j] << " ";
     }
     std::cerr << std::endl;
   }
}

// prints a homogeneous 3d point / vector
void PrintHPoint(HPoint3 p)
{
  std::cerr << "("
	    << p.x << " "
	    << p.y << " "
	    << p.z << " "
	    << p.w << ")" << std::endl;
}

// prints a 3d point / vector
void PrintPoint(Point3 p) {
  std::cerr << "("
	    << p.x << " "
	    << p.y << " "
	    << p.z << " " << std::endl;
}

Vector3 crossVector(Vector3 v1, Vector3 v2){
    Vector3 val = {v1.y*v2.z-v1.z*v2.y, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x};
    return val;
}
