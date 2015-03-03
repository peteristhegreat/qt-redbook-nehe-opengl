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

/*
 *  cube.cpp
 *  This program demonstrates a single modeling transformation,
 *  glScalef() and a single viewing transformation, gluLookAt().
 *  A wireframe cube is rendered.
 */
//  converted to Qt by Jared Haslam with a slightly larger cube
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>




class CubeWidget : public QGLWidget
{
public:
	CubeWidget( QWidget *parent=0, const char *name=0):
		QGLWidget(parent,name){};
protected:
	void initializeGL(void);
	void resizeGL (int w, int h);
	void paintGL();
	void keyPressEvent (QKeyEvent *e);
}; 



void CubeWidget::initializeGL(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void CubeWidget::paintGL(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glLoadIdentity ();             /* clear the matrix */
           /* viewing transformation  */
   gluLookAt (0 ,0 , 5.0,0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glScalef (1.0, 2.0, 1.0);      /* modeling transformation */ 
   glBegin (GL_LINE_LOOP);
	glVertex3f(1, 1, 0);
	glVertex3f(-1,1,0);
	glVertex3f(-1,-1,0);
	glVertex3f(1,-1,0);
   glEnd();
   glBegin (GL_LINE_LOOP);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
   glEnd();
   glBegin (GL_LINES);
	glVertex3f(1,1,0);
	glVertex3f(1,1,1);
	glVertex3f(-1,1,0);
	glVertex3f(-1,1,1);
	glVertex3f(-1,-1,0);
	glVertex3f(-1,-1,1);
	glVertex3f(1,-1,0);
	glVertex3f(1,-1,1);
   glEnd();
   //glutWireCube (1.0);
   glFlush ();
}

void CubeWidget::resizeGL (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
   glMatrixMode (GL_MODELVIEW);
}

void CubeWidget::keyPressEvent(QKeyEvent *e)
{
   switch (e->key()) {
      case Key_Escape:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
   QApplication::setColorSpec( QApplication::CustomColor);
   QApplication a( argc, argv );  

   if (!QGLFormat::hasOpenGL() ){
	qWarning( "This system has no OpenGL support....Exiting.");
	return -1;
   }

   CubeWidget C;
   a.setMainWidget ( &C );
   C.show();
   return a.exec();
}

