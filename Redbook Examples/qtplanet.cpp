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
 *  planet.c
 *  This program shows how to composite modeling transformations
 *  to draw translated and rotated models.
 *  Interaction:  pressing the d and y keys (day and year)
 *  alters the rotation of the planet around the sun.
 */
 
 //	Modified by Alma Oliphant for use with qt
 //	October 24, 2001

#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>


#define SUN 1
#define MOON 2
#define ANGLE 60
#define SUNANGLE 40
static int year = 0, day = 0;

class PlanetWidget : public QGLWidget
{
public:
    PlanetWidget( QWidget *parent=0, const char *name=0 ) :
        QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
    void WireSphere(int obj, float r, int sec, int slice);
};


void PlanetWidget::initializeGL(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
   WireSphere(SUN,1,40,38); /* draw sun */
   WireSphere(MOON,0.2,20,15); /* draw smaller planet */
}


void PlanetWidget::WireSphere(int obj, float r, int sec, int slice)
{
    glNewList(obj, GL_COMPILE); 
        glColor3f(0,0,1);
        GLUquadricObj* sphereObj = gluNewQuadric ();
        //can make the planets solid, wireframe, and points 
        gluQuadricDrawStyle (sphereObj, GLU_POINT); 
        gluSphere (sphereObj, r, sec, slice); 
    glEndList();
}

void PlanetWidget::paintGL(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
	
   glPushMatrix();
//   glutWireSphere(1.0, 20, 16);   /* draw sun */
   	glRotatef(ANGLE,1,0,0);
		glPushMatrix();
      	glRotatef(SUNANGLE,1,0,0);//can change the angle of the sun
         glCallList(SUN);
      glPopMatrix(); 
      glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);
   	glTranslatef (2.0, 0.0, 0.0);
   	glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
//   glutWireSphere(0.2, 10, 8);    /* draw smaller planet */
		glCallList(MOON);
   glPopMatrix();
}

void PlanetWidget::resizeGL(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void PlanetWidget::keyPressEvent(QKeyEvent *e)
{
      switch (e->ascii()) 
      {
      case 'd':
         day = (day + 10) % 360;
         updateGL();
         break;
      case 'D':
         day = (day - 10) % 360;
         updateGL();
         break;
      case 'y':
         year = (year + 5) % 360;
         updateGL();
         break;
      case 'Y':
         year = (year - 5) % 360;
         updateGL();
         break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
   e->accept();  // Don't pass any key events to parent
}

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    PlanetWidget w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
