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
// THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
// AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
// INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
// FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
// GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
// SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
// KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
// LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
// THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
// ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
// POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
// 
// US Government Users Restricted Rights 
// Use, duplication, or disclosure by the Government is subject to
// restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
// (c)(1)(ii) of the Rights in Technical Data and Computer Software
// clause at DFARS 252.227-7013 and/or in similar or successor
// clauses in the FAR or the DOD or NASA FAR Supplement.
// Unpublished-- rights reserved under the copyright laws of the
// United States.  Contractor/manufacturer is Silicon Graphics,
// Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
//
// OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
//
////  bezsurf.c
 //  This program renders a wireframe Bezier surface,
 //  using two-dimensional evaluators.
 //
 //		Modified by Mark Andersen 10/17/2001 converted to QT from glut
 
#include <stdlib.h>
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>

//	specify control mesh for bezier surface

GLfloat ctrlpoints[4][4][3] = {
   {{-1.5, -1.5, 4.0}, {-0.5, -1.5, 2.0}, 
    {0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}}, 
   {{-1.5, -0.5, 1.0}, {-0.5, -0.5, 3.0}, 
    {0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}}, 
   {{-1.5, 0.5, 4.0}, {-0.5, 0.5, 0.0}, 
    {0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}}, 
   {{-1.5, 1.5, -2.0}, {-0.5, 1.5, -2.0}, 
    {0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}}
};

//	declare "BezierSurface" class (subclass of QGLWidget class)

class BezierSurface : public QGLWidget
{
public:
	BezierSurface( QWidget *parent=0, const char *name=0):
		QGLWidget(parent, name) {};

protected:
	void paintGL();
	void initializeGL(void);
	void resizeGL( int w, int h);
	void keyPressEvent( QKeyEvent *e);
};

//implement "paintGL" function

void BezierSurface::paintGL(void)
{
   int i, j;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glColor3f(1.0, 1.0, 1.0);
   glPushMatrix ();
   glRotatef(85.0, 1.0, 1.0, 1.0);
   for (j = 0; j <= 8; j++) {
      glBegin(GL_LINE_STRIP);
      for (i = 0; i <= 30; i++)
         glEvalCoord2f((GLfloat)i/30.0, (GLfloat)j/8.0);
      glEnd();
      glBegin(GL_LINE_STRIP);
      for (i = 0; i <= 30; i++)
         glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/30.0);
      glEnd();
   }
   glPopMatrix ();
   glFlush();
}

void BezierSurface::initializeGL(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
           0, 1, 12, 4, &ctrlpoints[0][0][0]);
   glEnable(GL_MAP2_VERTEX_3);
   glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_FLAT);
}

void BezierSurface::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho(-4.0, 4.0, -4.0*(GLfloat)h/(GLfloat)w, 
              4.0*(GLfloat)h/(GLfloat)w, -4.0, 4.0);
   else
      glOrtho(-4.0*(GLfloat)w/(GLfloat)h, 
              4.0*(GLfloat)w/(GLfloat)h, -4.0, 4.0, -4.0, 4.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void BezierSurface::keyPressEvent( QKeyEvent *e)
{
   switch (e->ascii()) {
      case 27:
         exit(0);
         break;
   }
}

//	main loop

int main(int argc, char** argv)
{
	QApplication::setColorSpec( QApplication::CustomColor );
	QApplication a(argc, argv );
	
	if ( !QGLFormat::hasOpenGL() ) {
		qWarning( "This system does not support OpenGL.  Exiting.");
		return -1;
	}
	
	BezierSurface s;
	a.setMainWidget( &s );
	s.show();
	return a.exec();
}
