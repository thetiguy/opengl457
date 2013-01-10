/*********************************************************************
 *  CMPSC 457                                                        *
 *  Template code for HW 3                                           *
 *  March 29, 2010                                                   *
 *  Charlie Cranall                                                  *
 *  cdc5064@psu.edu                                                  *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  This is a template code for homework 3.                          *
 *  It has been modified to meet the problems in the assignment      *
 *********************************************************************/  


#include <GL/glut.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "matrices.h"


// for your convenience while debugging
using std::cout;
using std::cerr;
using std::endl;


// glut callbacks
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);


// helpers
void init();
void initObj();
void initCam();
void drawFaces();
void DeviceToWorld(double u, double v, double& x, double& y);


// transformations
void Rotate(double dx, double dy);
void Translate_xy(double tx, double ty);
void Translate_xz(double tx, double ty);
void Scale(double s);


// projection
void SetViewMatrix();
void SetOrthoMatrix();
void SetPerspMatrix();


// initial device window size
int win_w = 512;
int win_h = 512;


// for tracking mouse events
struct MouseTracker
{
  int modifiers;
  int button;
  double initx, inity;
  double finalx, finaly;
};

MouseTracker mtracker;


// for camera parameters
struct Camera
{
  bool perspective;               /* projection method */
  double l, r, b, t, n, f;        /* view volume */
  Point3 eye;                     /* eye position */
  Vector3 u, v, w;                /* eye coordinate system */
  Matrix4 Mo;                     /* orthographic projection */
  Matrix4 Mv;                     /* view matrix for arbitrary view*/
  Matrix4 Mp;                     /* perspective matrix */
};
Camera cam;



// for objects
const int MAXNAMESTRING = 20;
const int MAXVERTICES = 1000;
const int MAXEDGES = 500;
const int MAXFACES = 50;

struct Object3D {
  char name[MAXNAMESTRING];       /* The name of object for printing */
  int Nvertices;                  /* number of vertices */
  int Nfaces ;                    /* number of faces */
  Matrix4 frame;                  /* the local to world coord transform */
  Point3 center;                  /* center of mass */
  HPoint3 vertices[MAXVERTICES];  /* coodrdinates of each vertex */
  int faces[MAXFACES][6];         /* If face has N vertices, give N + 1 
			  	     numbers -> first the number of vertices
			  	     in the face, then the index numbers of
				     each vertex as it appears in the 
				     "vertices"  array. */
};

bool drawHidden;


// Note: We will keep the initial coordinates of the vertices
//       as originally given.  In other words, we will not change
//       the given coordinates of the vertices, even after any
//       transformation.  All the transformation will be recorded
//       in frame.  This way, you can reset the object to the
//       original position at any time, even after applying many
//       transformations.
Object3D obj = {
  "house", 10, 7,
  
  // initially identity matrix (no transformations applied yet)
  {{ {1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0},
     {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}  }},
  
  // center of the object is at origin
  {0.0, 0.0, 0.0},
  
  // vertices of the object in no particular order
  {  {0.0, 1.0, 2.0, 1.0},    {-1.0, 0.5, 2.0, 1.0},
     {-1.0, -1.0, 2.0, 1.0},  {1.0, -1.0, 2.0, 1.0},
     {1.0, 0.5, 2.0, 1.0},    {0.0, 1.0, -2.0, 1.0},
     {-1.0, 0.5, -2.0, 1.0},  {-1.0, -1.0, -2.0, 1.0},
     {1.0, -1.0, -2.0, 1.0},  {1.0, 0.5, -2.0, 1.0}   },
  
  // faces
  {  {5,   0, 1, 2, 3, 4},
     {5,   9, 8, 7, 6, 5},
     {4,   4, 3, 8, 9},
     {4,   0, 4, 9, 5},
     {4,   1, 0, 5, 6},
     {4,   2, 1, 6, 7},
     {4,   3, 2, 7, 8}    }
};



// OpenGL/glut programs will have the structure shown here
//    although with different args andcallbacks.
//
// You should not modify main().
// If you really want to modify it, do it at your own risk.
//
// For complete description of each glut functions used, see
// glut manual page on class website.
int main(int argc, char *argv[])
{
  // initialize glut
  glutInit(&argc, argv);

  // use double buffering with RGB colors
  // double buffer removes most of the flickering
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // set window size and position
  glutInitWindowSize(win_w, win_h);
  glutInitWindowPosition(100, 100);

  // now, create window with title "Viewing"
  glutCreateWindow("Viewing");


  // other stuffs like background color, viewing, etc will be
  // set up in this function.
  init();
  
  // initialize (arrange) the object
  initObj();

  // initialize the camera
  initCam();

  
  // register callbacks for glut
  glutDisplayFunc(display);   // for display
  glutReshapeFunc(reshape);   // for window move/resize
  glutMouseFunc(mouse);       // for mouse buttons
  glutMotionFunc(motion);     // for mouse movement while mouse button pressed
  glutKeyboardFunc(keyboard); // for keyboard


  // start event processing, i.e., accept user inputs
  glutMainLoop();

  return 0;
}


//
// implementation for glut callbacks
//

// called when the window is resized/moved (and some other cases)
void reshape(int w, int h)
{
  // change window size
  win_w = w;
  win_h = h;

  // set the new viewport
  glViewport(0, 0, (GLint)win_w, (GLint)win_h);

  // we will use orthographic projection when drawing the object.
  //
  // NOTE: This has nothing to do with the projections you are
  //       to implement in this assignment.  We only need this
  //       when you draw 2D lines.  In other words, find the 2D
  //       projections of the end points of a given 3D line using
  //       the projection matrices you implemented and then draw
  //       a 2D line between the projected end-points.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 511.0, 0.0, 511.0, -1.0, 1.0);
}


// called when the window needs to be redrawn
void display()
{
  // clear the buffer with bg color set in init()
  // you can think of the buffer as a raster array provided by GL
  glClear(GL_COLOR_BUFFER_BIT);

  // draw the object on the buffer you just cleared
  drawFaces();

  // swap the buffers.
  // we are using 2 buffers provided by GL (see main) -- double buffer.
  // they are called front / back buffers.
  // what you see on the screen is the content of front buffer
  // what you clear/draw above is done on back buffer
  // once drawing is done on the back buffer, 
  //       you need to display the content of the back buffer.
  // swapping buffers means swapping back buffer with front buffer
  //       so that front buffer becomes back buffer and
  //       back buffer becomes front buffer.
  // once back buffer becomes front buffer, the content of it will be
  //       drawn on the screen, so you can see it.
  glutSwapBuffers();
}


// called when a mouse event (button pressed/released) occurs
// in glut, 
//     mouse buttons are represented as
//           GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON
//     status mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
// 
void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {  // mouse pressed.  record the details
    // which button?
    mtracker.button = button;
    // any modifiers (keys like shift/ctrl/alt) pressed?
    mtracker.modifiers = glutGetModifiers();
    // mouse position in world
    DeviceToWorld(double(x), double(y), mtracker.initx, mtracker.inity);
  }
}


// called when a mouse moves with a button pressed
void motion(int x, int y)
{
  // get the mouse position in world
  DeviceToWorld(double(x), double(y), mtracker.finalx, mtracker.finaly);

  // I'm printing the following information just for you to see that things are ok.
  // This will work after you complete the function DeviceToWorld() correctly; otherwise,
  //     mtracker's initx, inity, finalx, and finaly will not show correct values.
  // You may want to get rid of this output when you are sure things work fine.
  cout << '(' << x << ',' << y << ',' << mtracker.initx << ',' << mtracker.inity << ")  ("
       << x << ',' << y << ',' << mtracker.finalx << ',' << mtracker.finaly << ")" << endl;

  // now, process the user input, i.e., mouse movement
  switch (mtracker.button) {
  case GLUT_LEFT_BUTTON:
    if (mtracker.modifiers & GLUT_ACTIVE_SHIFT) {
      // shift + left button
      Translate_xy(mtracker.finalx - mtracker.initx,
		   mtracker.finaly - mtracker.inity);
    }
    else {
      // left button
      Translate_xz(mtracker.finalx - mtracker.initx,
		   mtracker.finaly - mtracker.inity);
    }
    break;
  case GLUT_RIGHT_BUTTON:
    Scale(mtracker.finalx - mtracker.initx);
    break;
  case GLUT_MIDDLE_BUTTON:
    Rotate(mtracker.finalx - mtracker.initx,
	   mtracker.finaly - mtracker.inity);
    break;
  }
  
  // redisplay after transformation
  glutPostRedisplay();

  // reset the mouse position
  mtracker.initx = mtracker.finalx;
  mtracker.inity = mtracker.finaly;
}  


// called when a keyboard event (key typed) occurs
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'Q':  // quit the program
  case 'q':
    exit(0);
  case 'B': //toggle backface culling
  case 'b':
    drawHidden = !drawHidden;
    glutPostRedisplay();
    break;
  case 'P':  // toggle the projection method
  case 'p':  // between orthographic and perspective projections
    cam.perspective = !cam.perspective;
    SetPerspMatrix();
    glutPostRedisplay();
    break;
  case 'R':
  case 'r':
	obj.frame = IdentMatrix();
	initObj();
	glutPostRedisplay();
	break;
  }
}


//
// implementation for helpers
//

void init()
{
  // set background color to black
  glClearColor(0.0, 0.0, 0.0, 0.0);
  drawHidden = true;
}


// arrange the object to its initial position
// NOTE: you may change the parameters as you like
void initObj()
{
  Vector3 n;

  // rotate around y-axis
  n.x = 0.0;  n.y = 1.0;  n.z = 0.0;
  double angle = M_PI / 6.0;
  Matrix4 m1 = SetRotMatrix(n, angle);
  
  // rotate around x-axis
  n.x = 1.0;  n.y = 0.0;  n.z = 0.0;
  angle = M_PI / 6.0;
  Matrix4 m2 = SetRotMatrix(n, angle);

  // translate so that the object is inside view volume
  // (see initCam() for the view volume)
  Matrix4 m3 = SetTransMatrix(0.0, 0.0, -5.0);

  // notice the order of the transformations applied
  //  i.e., Ry -> Rx -> T  becomes  TRxRy in matrix multiplication
  obj.frame = Mult4(m3, Mult4(m2, Mult4(m1, obj.frame)));
}


// initialize camera parameters
// NOTE: you may change the parameters as you like
void initCam()
{
  // use orthographic projection as default
  cam.perspective = false;

  // camera position
  cam.eye.x = 0.0;
  cam.eye.y = 0.0;
  cam.eye.z = 0.0;

  // view volume
  cam.l = -5.0;  cam.r = 5.0;
  cam.b = -5.0;  cam.t = 5.0;
  cam.n = -1.0;  cam.f = -6.0;

  // camera coordinate system
  cam.u.x = 1.0;  cam.u.y = 0.0;  cam.u.z = 0.0;
  cam.v.x = 0.0;  cam.v.y = 1.0;  cam.v.z = 0.0;
  cam.w.x = 0.0;  cam.w.y = 0.0;  cam.w.z = 1.0;

  // set Mv, Mp, Mo
  SetViewMatrix(); 
  SetPerspMatrix();
  SetOrthoMatrix();
}


// draw object faces
// IMPORTANT: you are only allowed to use glVertex2d.
void drawFaces()
{
  /**************************************************/
  /* Remove the lines below that hardcoded vertices */
  /* and put your code here to draw each face of    */
  /* the given object.  You should first compute    */
  /* the overall projection matrix.  Then, apply    */
  /* the matrix to each vertex when you draw the    */
  /* faces.  DO NOT forget to homogenize vertices   */
  /* after transformation.                          */
  /*                                                */
  /* NOTE: You are only allowed to use glVertex2d   */
  /*       with GL_LINE_LOOP primitive.             */
  /**************************************************/
  
  //ortho-proj = Mo * Mp * Mv * Pobj
  Matrix4 Mproj = Mult4(Mult4(Mult4(cam.Mv, cam.Mo), cam.Mp), obj.frame);
  HPoint3 temp;
  
  //find scaling values
  double xScale = (win_w);
  double yScale = (win_h);
  double xShift = (win_w)/2;
  double yShift = (win_h)/2;
  
  //draw each face
  for(int i=0; i < obj.Nfaces; i++){
	cerr << "face:" << i << endl;
	//find an outward facing normal for this face
	HPoint3 p0 = Homogenize(TransHPoint3( Mproj, obj.vertices[ obj.faces[i][1] ] ));
	HPoint3 p1 = Homogenize(TransHPoint3( Mproj, obj.vertices[ obj.faces[i][2] ] ));
	HPoint3 p2 = Homogenize(TransHPoint3( Mproj, obj.vertices[ obj.faces[i][3] ] ));
	Vector3 v1 = {p1.x-p0.x, p1.y-p0.y, p1.z-p0.z};
	Vector3 v2 = {p2.x-p0.x, p2.y-p0.y, p2.z-p0.z};
	Vector3 nv = crossVector(v1, v2);
	//find a vector to the eye
	HPoint3 ep = {cam.eye.x, cam.eye.y, cam.eye.z, 1.0};
	ep = TransHPoint3(Mproj, ep );
	Vector3 ev = { ep.x - p0.x, ep.y - p0.y, ep.z - p0.z + (cam.n - cam.f)*2 };
	//dot normal with eye to find visiblity
	double  result = ev.x*nv.x + ev.y*nv.y + ev.z*nv.z;
	cerr << result << endl;
	//check for backfaces?
	if( drawHidden || result > 0 ){
		glBegin(GL_LINE_LOOP);
		for(int j = 1; j <= obj.faces[i][0]; j++){
			temp = Homogenize(TransHPoint3( Mproj, obj.vertices[ obj.faces[i][j] ] ));
			glVertex2d(temp.x*xScale + xShift, temp.y*yScale + yShift );
		}
		glEnd();
	}
  }
  
}


// convert device coordinate to world coordinate
void DeviceToWorld(double u, double v, double& x, double& y)
{
  /***********************************************/
  /* Put code here to convert device coordinates */
  /* to world coordinate                         */
  /***********************************************/
  //device top left = 0, 0, lower right = win+w, win_h
  //world, center = 0,0;
  x = (u-0)/(win_w-0)*(cam.r-cam.l)+cam.l;
  y = (v-win_h)/(0-win_h)*(cam.t-cam.b)+cam.b;
}




//
// implementation for transformations
//


// rotation using the Rolling Ball transformation
void Rotate(double dx, double dy)
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* for rolling ball rotation according to the   */
  /* specification in the handout                 */
  /************************************************/
  
  Matrix4 m = IdentMatrix();
  double dr = sqrt( dx*dx + dy*dy);
  Vector3 n = { -dy/dr, dx/dr, 0.0};
  double angle = atan( dr / 5);//set R to 1-10
  
  obj.frame = Mult4(obj.frame, SetRotMatrix( n, angle) ) ;
}


// translation in xy-plane
void Translate_xy(double tx, double ty)
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* for translation in xy plane according to the */
  /* specification in the handout                 */
  /************************************************/
  obj.frame = Mult4(obj.frame, SetTransMatrix( tx, ty, 0.0));
}


// translation in xz-plane
void Translate_xz(double tx, double ty)
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* for translation in xz plane according to the */
  /* specification in the handout                 */
  /************************************************/
  obj.frame = Mult4(obj.frame, SetTransMatrix( tx, 0.0, ty));
}


// uniform scale
void Scale(double sx)
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* for scaling in all dimensions according to   */
  /* the specification in the handout             */
  /************************************************/
  
  obj.frame = Mult4(obj.frame, SetScaleMatrix( 1+sx, 1+sx, 1+sx));
}



//
// implementation for projection
//

// Mv
void SetViewMatrix()
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* to set up the matrix Mv for arbitrary view   */
  /* point as specified in the textbook (p147)    */
  /************************************************/
  
  Matrix4 me = IdentMatrix();
  me.elem[0][3] = -cam.eye.x;
  me.elem[1][3] = -cam.eye.y;
  me.elem[2][3] = -cam.eye.z;
  
  Matrix4 mc = IdentMatrix();
  mc.elem[0][0] = cam.u.x; mc.elem[0][1] = cam.u.y; mc.elem[0][2] = cam.u.z;
  mc.elem[1][0] = cam.v.x; mc.elem[1][1] = cam.v.y; mc.elem[1][2] = cam.v.z;
  mc.elem[2][0] = cam.w.x; mc.elem[2][1] = cam.w.y; mc.elem[2][2] = cam.w.z;
  
  cam.Mv = Mult4( mc, me);
}

// Mo
void SetOrthoMatrix()
{
  /************************************************/
  /* Remove the lines below that hardcode Mo and  */
  /* put your code here to set up the matrix Mo   */
  /* for orthographic projection as specified in  */
  /* the textbook (p145)                          */
  /************************************************/

  Matrix4 m;
  m.elem[0][0] = 2/(cam.r - cam.l);  m.elem[0][1] =  0.0;  m.elem[0][2] = 0.0;  m.elem[0][3] = 0-(cam.r+cam.l)/(cam.r-cam.l);
  m.elem[1][0] =  0.0;  m.elem[1][1] = 2/(cam.t - cam.b);  m.elem[1][2] = 0.0;  m.elem[1][3] = 0-(cam.t+cam.b)/(cam.t-cam.b);
  m.elem[2][0] =  0.0;  m.elem[2][1] =  0.0;  m.elem[2][2] = 2/(cam.n - cam.f);  m.elem[2][3] = 0-(cam.n+cam.f)/(cam.n-cam.f);
  m.elem[3][0] =  0.0;  m.elem[3][1] =  0.0;  m.elem[3][2] = 0.0;  m.elem[3][3] = 1.0;
  cam.Mo = m;
}


// Mp
void SetPerspMatrix()
{
  /************************************************/
  /* Remove the line below and put your code here */
  /* to set up the matrix Mp for perspective      */
  /* projection as specified in the textbook      */
  /* (p152)                                       */
  /*                                              */
  /* Note: Mp is just an identity matrix for      */
  /*       orthographic projection.               */
  /************************************************/
  
  Matrix4 m = IdentMatrix();
  m.elem[0][0] = cam.n;
  m.elem[1][1] = cam.n;
  m.elem[2][2] = cam.n+cam.f;
  m.elem[2][3] = -cam.f*cam.n;
  m.elem[3][2] = 1.0;
  m.elem[3][3] = 0.0;
  
  cam.Mp = (cam.perspective)?m:IdentMatrix();
}

