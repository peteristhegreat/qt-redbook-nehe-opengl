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
 *  movelight.c
 *  This program demonstrates when to issue lighting and
 *  transformation commands to render a model with a light
 *  which is moved by a modeling transformation (rotate or
 *  translate).  The light position is reset after the modeling
 *  transformation is called.  The eye position does not change.
 *
 *  A sphere is drawn using a grey material characteristic.
 *  A single light source illuminates the object.
 *
 *  Interaction:  pressing the left mouse button alters
 *  the modeling transformation (x rotation) by 30 degrees.
 *  The scene is then redrawn with the light in a new position.
 */
 
 //	Modified by Alma Oliphant for qt
 //	October 24, 2001

#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>

#define SQUARE   6
#define BOX      7
#define SPHERE	  1 

static int spin = 0;

class MoveLight : public QGLWidget
{
public:
    MoveLight( QWidget *parent=0, const char *name=0 ) :
        QGLWidget(parent, name) {};
    void MakeCube(float side);
    void MakeSphere(int obj, float r, int sec, int slice);

protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
    void mousePressEvent( QMouseEvent *e );
};


void MoveLight::initializeGL(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

//	Initialize the wire cube and sphere
   MakeCube(.1);
   MakeSphere(SPHERE,.8,40,38);//Obj, radius, orange slices, horizontal slices

}


void MoveLight::paintGL(void)
{
   GLfloat position[] = { 0.0, 0.0, 1.5, 1.0 };

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix ();
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   glPushMatrix ();
   glRotated ((GLdouble) spin, 1.0, 0.0, 0.0);
   glLightfv (GL_LIGHT0, GL_POSITION, position);

   glTranslated (0.0, 0.0, 1.5);
   glDisable (GL_LIGHTING);
   glColor3f (0.0, 1.0, 1.0);
	
   glCallList(BOX);
//   glutWireCube (0side/2); /*make a cube*/
	
   glEnable (GL_LIGHTING);
   glPopMatrix ();

//	The original glut used a Torus (donut)
//	I've got a wire sphere here, mainly because it's easier
//	to program	
   glCallList(SPHERE);
//   glutSolidTorus (0.275, 0.85, 8, 15); /*do a circle*/
   glPopMatrix ();
   glFlush ();
}

void MoveLight::MakeCube(float side)
{
   glNewList(BOX, GL_COMPILE);
   	//	Make one wire box
      glBegin(GL_LINE_LOOP);
			//	Side/2 to center the box on the origin
         glVertex3f(-side/2, -side/2, -side/2);
         glVertex3f( side/2, -side/2, -side/2);
         glVertex3f( side/2,  side/2, -side/2);
      	glVertex3f(-side/2,  side/2, -side/2);
   	glEnd();
      //	Make the other wire box
      glBegin(GL_LINE_LOOP);
         glVertex3f(-side/2, -side/2, side/2);
         glVertex3f( side/2, -side/2, side/2);
         glVertex3f( side/2,  side/2, side/2);
      	glVertex3f(-side/2,  side/2, side/2);
   	glEnd();
      //	Connect the two boxes together
      glBegin(GL_LINES);
         glVertex3f(-side/2, -side/2, -side/2);
         glVertex3f(-side/2, -side/2, side/2);
         
         glVertex3f( side/2, -side/2, -side/2);
         glVertex3f( side/2, -side/2, side/2);
         
         glVertex3f( side/2,  side/2, -side/2);
         glVertex3f( side/2,  side/2, side/2);
         
         glVertex3f(-side/2,  side/2, -side/2);
         glVertex3f(-side/2,  side/2, side/2);
      glEnd();
         
	glEndList();
}

void MoveLight::MakeSphere(int obj, float r, int sec, int slice)
{
	//	Used a Sphere.  Future should include the torus
    glNewList(obj, GL_COMPILE); 
        glColor3f(0,0,1);
        GLUquadricObj* sphereObj = gluNewQuadric (); 
        //gluQuadricDrawStyle (sphereObj, GLU_FLAT);
        gluQuadricDrawStyle (sphereObj, GLU_LINE); 
        gluQuadricNormals (sphereObj, GLU_SMOOTH);
        gluSphere (sphereObj, r, sec, slice); 
    glEndList();
}

void MoveLight::resizeGL(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

//void MoveLight::mouse(int button, int state, int x, int y)
void MoveLight::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == QMouseEvent::LeftButton )
    {
    	//The original glut had a state of the button (up or down) call
      //	here we just say if the button does anything, spin.
         //if (e->type() == QMouseEvent::MouseButtonPress)
        {
        		spin = (spin + 30) % 360;
         	updateGL();
        }
    }
}

void MoveLight::keyPressEvent(QKeyEvent *e)
{
   //	You can put more ascii values in here
   switch (e->ascii()) {
      case 27:
         exit(0);
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

    MoveLight w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
