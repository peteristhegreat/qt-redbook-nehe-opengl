//
// Copyright (c) 1993-1997, Silicon Graphics, Inc.
// ALL RIGHTS RESERVED 
// Permission to use, copy, modify, and distribute this software for 
// any purpose and without fee is hereby granted, provided that the above
// copyright notice appear in all copies and that both the copyright notice
// and this permission notice appear in supporting documentation, and that 
// the name of Silicon Graphics, Inc. not be used in advertising
// or publicity pertaining to distribution of the software without specific,
// written prior permission. 
//
// File:
//  tess_trans.cpp
/*  
 *  This program demonstrates polygon tessellation.
 *  Two tesselated objects are drawn.  The first is a
 *  rectangle with a triangular hole.  The second is a
 *  smooth shaded, self-intersecting star.
 *
 *  Note the exterior rectangle is drawn with its vertices
 *  in counter-clockwise order, but its interior clockwise.
 *  Note the combineCallback is needed for the self-intersecting
 *  star.  Also note that removing the TessProperty for the 
 *  star will make the interior unshaded (WINDING_ODD).
 */
// Author:
//   DCD    Modified use Qt instead of glut.
//
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>

#ifndef CALLBACK
#define CALLBACK
#endif

GLdouble currentWinding = GLU_TESS_WINDING_ODD;
GLUtesselator *tobj;

typedef void (*TessFuncPtr)();
GLuint startList;

class Qt_Tess : public QGLWidget
{
public:
    Qt_Tess( QWidget *parent=0, const char *name=0 ):
        QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
//    void CALLBACK beginCallback(GLenum which);
//    void CALLBACK errorCallback(GLenum errorCode);
//    void CALLBACK endCallback(void);
//    void CALLBACK combineCallback(GLdouble coords[3], GLdouble *data[4], GLfloat weight[4], GLdouble **dataOut );

};


void CALLBACK beginCallback(GLenum which)
{
   glBegin(which);
}


void CALLBACK errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf(stderr, "Tessellation Error: %s\n", estring);
   exit(0);
}

void CALLBACK endCallback(void)
{
   glEnd();
}

void CALLBACK vertexCallback(GLdouble *vertex)
{
   const GLdouble *pointer;

   pointer = (GLdouble *) vertex;
   glColor3dv(pointer+3);
   glVertex3dv(vertex);
}


void CALLBACK combineCallback(GLdouble coords[3], 
                     GLdouble *vertex_data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
   GLdouble *vertex;
   int i;

   vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));

   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];
//   for (i = 3; i < 7; i++)
//      vertex[i] = weight[0] * vertex_data[0][i] 
//                  + weight[1] * vertex_data[1][i]
//                  + weight[2] * vertex_data[2][i] 
//                  + weight[3] * vertex_data[3][i];
   *dataOut = vertex;
}



/*  Initialize antialiasing for color index mode,
 *  including loading a green color ramp starting
 *  at RAMP1START, and a blue color ramp starting
 *  at RAMP2START. The ramps must be a multiple of 16.
 */
void Qt_Tess::initializeGL(void)
{
   GLUtesselator *tobj;
   GLdouble rect[4][3] = {
   			  {50.0, 50.0, 0.0},
                          {200.0, 50.0, 0.0},
                          {200.0, 200.0, 0.0},
                          {50.0, 200.0, 0.0}
			 };
   GLdouble tri[3][3] = {
   			 {75.0, 75.0, 0.0},
                         {125.0, 175.0, 0.0},
                         {175.0, 75.0, 0.0}
			};
   GLdouble star[5][6] = {
   			  {250.0, 50.0, 0.0, 1.0, 0.0, 1.0},
                          {325.0, 200.0, 0.0, 1.0, 1.0, 0.0},
                          {400.0, 50.0, 0.0, 0.0, 1.0, 1.0},
                          {250.0, 150.0, 0.0, 1.0, 0.0, 0.0},
                          {400.0, 150.0, 0.0, 0.0, 1.0, 0.0}
			 };

   glClearColor(0.0, 0.0, 0.0, 0.0);

   startList = glGenLists(2);

   tobj = gluNewTess();
   gluTessCallback(tobj, GLU_TESS_VERTEX, (TessFuncPtr)glVertex3dv);
   gluTessCallback(tobj, GLU_TESS_BEGIN, (TessFuncPtr)beginCallback);
   gluTessCallback(tobj, GLU_TESS_END, (TessFuncPtr)endCallback);
   gluTessCallback(tobj, GLU_TESS_ERROR, (TessFuncPtr)errorCallback);

   /*  rectangle with triangular hole inside  */
   glNewList(startList, GL_COMPILE);
   glShadeModel(GL_FLAT);    
   gluTessBeginPolygon(tobj, NULL);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, rect[0], rect[0]);
         gluTessVertex(tobj, rect[1], rect[1]);
         gluTessVertex(tobj, rect[2], rect[2]);
         gluTessVertex(tobj, rect[3], rect[3]);
      gluTessEndContour(tobj);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, tri[0], tri[0]);
         gluTessVertex(tobj, tri[1], tri[1]);
         gluTessVertex(tobj, tri[2], tri[2]);
      gluTessEndContour(tobj);
   gluTessEndPolygon(tobj);
   glEndList();

   gluTessCallback(tobj, GLU_TESS_VERTEX, (TessFuncPtr)vertexCallback);
   gluTessCallback(tobj, GLU_TESS_BEGIN, (TessFuncPtr)beginCallback);
   gluTessCallback(tobj, GLU_TESS_END, (TessFuncPtr)endCallback);
   gluTessCallback(tobj, GLU_TESS_ERROR, (TessFuncPtr)errorCallback);
   gluTessCallback(tobj, GLU_TESS_COMBINE, (TessFuncPtr)combineCallback);

   /*  smooth shaded, self-intersecting star  */
   glNewList(startList + 1, GL_COMPILE);
   glShadeModel(GL_SMOOTH);    
   gluTessProperty(tobj, GLU_TESS_WINDING_RULE,
                   GLU_TESS_WINDING_POSITIVE);
   gluTessBeginPolygon(tobj, NULL);
      gluTessBeginContour(tobj);
         gluTessVertex(tobj, star[0], star[0]);
         gluTessVertex(tobj, star[1], star[1]);
         gluTessVertex(tobj, star[2], star[2]);
         gluTessVertex(tobj, star[3], star[3]);
         gluTessVertex(tobj, star[4], star[4]);
      gluTessEndContour(tobj);
   gluTessEndPolygon(tobj);
   glEndList();
   gluDeleteTess(tobj);
}

/*  Draw 2 diagonal lines to form an X
 */
void Qt_Tess::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(1.0, 1.0, 1.0);
   glCallList(startList);
   glCallList(startList + 1);
   glFlush();
}


void Qt_Tess::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
}



void Qt_Tess::keyPressEvent(QKeyEvent *e)
{
   switch (e->ascii()) 
   {
      case 27:
         exit(0);
         break;
   }
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  color index display mode, and handle input events.
 */
int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    Qt_Tess w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
