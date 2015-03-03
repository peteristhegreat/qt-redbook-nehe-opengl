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
//  tesswind.cpp
//
// Description:
//  
//  This program demonstrates the winding rule polygon 
//  tessellation property.  Four tessellated objects are drawn, 
//  each with very different contours.  When the w key is pressed, 
//  the objects are drawn with a different winding rule.

// Author:
//   DCD    Modified use Qt instead of glut.
//
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>
//#include <qglcolormap.h>
//#include <stdlib.h>

#ifndef CALLBACK
#define CALLBACK
#endif

GLdouble currentWinding = GLU_TESS_WINDING_ODD;
int currentShape = 0;
GLUtesselator *tobj;
GLuint ulist;

typedef void (*TessFuncPtr)();
static float rotAngle = 0.;


class TesswindWidget : public QGLWidget
{
public:
    TesswindWidget( QWidget *parent=0, const char *name=0 ):
        QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
    void makeNewList (void);
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

void CALLBACK combineCallback(GLdouble coords[3], GLdouble *data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
   GLdouble *vertex;
   vertex = (GLdouble *) malloc(3 * sizeof(GLdouble));

   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];
   *dataOut = vertex;
}


void TesswindWidget::makeNewList (void) 
{
   int i;
   static GLdouble rects[12][3] = 
   {
       { 50.0,  50.0, 0.0},{300.0, 50.0, 0.0},
       {300.0, 300.0, 0.0},{50.0, 300.0, 0.0},
       {100.0, 100.0, 0.0},{250.0, 100.0, 0.0},
       {250.0, 250.0, 0.0},{100.0, 250.0, 0.0},
       {150.0, 150.0, 0.0},{200.0, 150.0, 0.0},
       {200.0, 200.0, 0.0},{150.0, 200.0, 0.0}
   };   


//   static GLdouble rects[12][3] = 
//      {50.0, 50.0, 0.0, 300.0, 50.0, 0.0, 
//       300.0, 300.0, 0.0, 50.0, 300.0, 0.0,
//       100.0, 100.0, 0.0, 250.0, 100.0, 0.0, 
//       250.0, 250.0, 0.0, 100.0, 250.0, 0.0,
//       150.0, 150.0, 0.0, 200.0, 150.0, 0.0, 
//       200.0, 200.0, 0.0, 150.0, 200.0, 0.0};
   static GLdouble spiral[16][3] =
   { 
       {400.0, 250.0, 0.0}, {400.0, 50.0, 0.0}, 
       {50.0, 50.0, 0.0},   {50.0, 400.0, 0.0}, 
       {350.0, 400.0, 0.0}, {350.0, 100.0, 0.0}, 
       {100.0, 100.0, 0.0}, {100.0, 350.0, 0.0}, 
       {300.0, 350.0, 0.0}, {300.0, 150.0, 0.0}, 
       {150.0, 150.0, 0.0}, {150.0, 300.0, 0.0}, 
       {250.0, 300.0, 0.0}, {250.0, 200.0, 0.0}, 
       {200.0, 200.0, 0.0}, {200.0, 250.0, 0.0}
   };
   static GLdouble quad1[4][3] = 
   {
      {50.0, 150.0, 0.0}, {350.0, 150.0, 0.0}, 
      {350.0, 200.0, 0.0}, {50.0, 200.0, 0.0}
   };
   static GLdouble quad2[4][3] =
   {
      {100.0, 100.0, 0.0}, {300.0, 100.0, 0.0}, 
       {300.0, 350.0, 0.0}, {100.0, 350.0, 0.0}
   };
   static GLdouble tri[3][3] =
   {
      {200.0, 50.0, 0.0}, {250.0, 300.0, 0.0},
      {150.0, 300.0, 0.0}
   };
 
   gluTessProperty(tobj, GLU_TESS_WINDING_RULE, currentWinding);

   glNewList(ulist, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 4; i < 8; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 8; i < 12; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(ulist+1, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 7; i >= 4; i--)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 11; i >= 8; i--)
            gluTessVertex(tobj, rects[i], rects[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(ulist+2, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 16; i++)
            gluTessVertex(tobj, spiral[i], spiral[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();

   glNewList(ulist+3, GL_COMPILE);
      gluTessBeginPolygon(tobj, NULL);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, quad1[i], quad1[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 0; i < 4; i++)
            gluTessVertex(tobj, quad2[i], quad2[i]);
         gluTessEndContour(tobj);
         gluTessBeginContour(tobj);
         for (i = 0; i < 3; i++)
            gluTessVertex(tobj, tri[i], tri[i]);
         gluTessEndContour(tobj);
      gluTessEndPolygon(tobj);
   glEndList();
}


void TesswindWidget::initializeGL(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);    

   tobj = gluNewTess();
   gluTessCallback(tobj, GLU_TESS_VERTEX, (TessFuncPtr)glVertex3dv);
   gluTessCallback(tobj, GLU_TESS_BEGIN,(TessFuncPtr)beginCallback);
   gluTessCallback(tobj, GLU_TESS_END, (TessFuncPtr)endCallback);
   gluTessCallback(tobj, GLU_TESS_ERROR, (TessFuncPtr)errorCallback);
   gluTessCallback(tobj, GLU_TESS_COMBINE, (TessFuncPtr)combineCallback);

   ulist = glGenLists(4);
   makeNewList();
}


void TesswindWidget::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f(1.0, 1.0, 1.0);
   glPushMatrix(); 
   glCallList(ulist);
   glTranslatef(0.0, 500.0, 0.0);
   glCallList(ulist+1);
   glTranslatef(500.0, -500.0, 0.0);
   glCallList(ulist+2);
   glTranslatef(0.0, 500.0, 0.0);
   glCallList(ulist+3);
   glPopMatrix(); 
   glFlush();
}

void TesswindWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      gluOrtho2D(0.0, 1000.0, 0.0, 1000.0 * (GLdouble)h/(GLdouble)w);
   else
      gluOrtho2D(0.0, 1000.0 * (GLdouble)w/(GLdouble)h, 0.0, 1000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void TesswindWidget::keyPressEvent(QKeyEvent *e)
{
   
   switch (e->ascii()) 
   {
      case 'w':
      case 'W':
         if (currentWinding == GLU_TESS_WINDING_ODD)
            currentWinding = GLU_TESS_WINDING_NONZERO;
         else if (currentWinding == GLU_TESS_WINDING_NONZERO)
            currentWinding = GLU_TESS_WINDING_POSITIVE;
         else if (currentWinding == GLU_TESS_WINDING_POSITIVE)
            currentWinding = GLU_TESS_WINDING_NEGATIVE;
         else if (currentWinding == GLU_TESS_WINDING_NEGATIVE)
            currentWinding = GLU_TESS_WINDING_ABS_GEQ_TWO;
         else if (currentWinding == GLU_TESS_WINDING_ABS_GEQ_TWO)
            currentWinding = GLU_TESS_WINDING_ODD;
         makeNewList();
         updateGL();
         break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }

}


int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    TesswindWidget w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
