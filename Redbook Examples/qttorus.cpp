/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */
/*  torus.c
 *  This program demonstrates the creation of a display list.
 */

//#include <GL/glut.h>
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>
//#include <qglcolormap.h>
//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>

#define PI_ 3.14159265358979323846

static GLuint thetorus;

class torus : public QGLWidget
{
public:
	torus( QWidget *parent=0, const char *name=0 ):
		QGLWidget( parent, name ) {};
protected:
	void initializeGL(void);
	void paintGL();
	void resizeGL(int w, int h);
	void keyPressEvent (QKeyEvent *e);	
	void drawtorus(int numc, int numt);
};
/* Create display list with Torus and initialize state */
void torus::initializeGL(void)
{
	
	thetorus = glGenLists (1);
	glNewList(thetorus, GL_COMPILE);
	this->drawtorus(8, 25);
	glEndList();
	
	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

/* Clear window and draw torus */
void torus::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glCallList(thetorus);
   glFlush();
}

/* Handle window resize */
void torus::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 100.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
}

/* Rotate about x-axis when "x" typed; rotate about y-axis
   when "y" typed; "i" returns torus to original view */
void torus::keyPressEvent(QKeyEvent *e /*unsigned char key, int x, int y*/)
{
   switch (e->key()) {
   case Key_X:  //R key is pressed independent of Shift
      glRotatef(30.,1.0,0.0,0.0);
      updateGL();
      break;
   case Key_Y:
      glRotatef(30.,0.0,1.0,0.0);
      updateGL();
      break;
   case Key_I:
      glLoadIdentity();
      gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
      updateGL();
      break;
   case Key_Escape:
      exit(0);
      break;
   default:
      break;
   }
   e->accept();	// Don't pass any key events to parent
}
/* Draw a torus */
void torus::drawtorus(int numc, int numt)
{
   int i, j, k;
   double s, t, x, y, z, twopi;

   twopi = 2 * PI_;
   for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;

            x = (1+.1*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (1+.1*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = .1 * sin(s * twopi / numc);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
   }
}
int main(int argc, char **argv)
{
	QApplication::setColorSpec( QApplication::CustomColor );
	QApplication a(argc, argv);
	if ( !QGLFormat::hasOpenGL() ) {
		qWarning("This system has no OpenGL support. exiting." );
		return -1;
	}
	
	torus w;
	a.setMainWidget( &w);
	w.show();
	return a.exec();

}
