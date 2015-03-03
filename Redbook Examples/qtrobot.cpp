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

// File:
//  robot.cpp
//
// Description:
//
// This program shows how to composite modeling transformations
// to draw translated and rotated hierarchical models.
// Interaction:  pressing the s and e keys (shoulder and elbow)
// alters the rotation of the robot arm.
// Author:
//
//   Travis Astle   Modified use Qt instead of glut.
//   Oct 19, 2001
//
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>

static int shoulder = 0, elbow = 0;

// Create Robot class

class Robot : public QGLWidget
{
public:
    Robot( QWidget *parent=0, const char *name=0 ) :
        QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
    void wireCube(float length);
};

// Initialize function adapted from existing code

void Robot::initializeGL(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

// Wire cube program to replace glutWireCube();

void Robot::wireCube(float length)
{
  glBegin(GL_LINE_LOOP);
    glVertex3f(-length/2, -length/2, length/2);
    glVertex3f(length/2, -length/2, length/2);
    glVertex3f(length/2, length/2, length/2);
    glVertex3f(-length/2, length/2, length/2);
  glEnd();

  glBegin(GL_LINE_LOOP);
    glVertex3f(-length/2, -length/2, -length/2);
    glVertex3f(length/2, -length/2, -length/2);
    glVertex3f(length/2, length/2, -length/2);
    glVertex3f(-length/2, length/2, -length/2);
  glEnd();

  glBegin(GL_LINES);
    glVertex3f(-length/2, -length/2, length/2);
    glVertex3f(-length/2, -length/2, -length/2);

    glVertex3f(length/2, -length/2, length/2);
    glVertex3f(length/2, -length/2, -length/2);

    glVertex3f(length/2, length/2, length/2);
    glVertex3f(length/2, length/2, -length/2);

    glVertex3f(-length/2, length/2, length/2);
    glVertex3f(-length/2, length/2, -length/2);
  glEnd();
}

// paint function adapted from original display function

void Robot::paintGL(void)
{
   float len =3;
   glClear (GL_COLOR_BUFFER_BIT);
   glPushMatrix();
   glTranslatef (-1.0, 0.0, 0.0);
   glRotatef ((GLfloat) shoulder, 0.0, 0.0, 1.0);
   glTranslatef (1.0, 0.0, 0.0);
   glPushMatrix();
   glScalef (2.0, 0.4, 1.0);
   wireCube(1.0);
   
   glPopMatrix();

   glTranslatef (1.0, 0.0, 0.0);
   glRotatef ((GLfloat) elbow, 0.0, 0.0, 1.0);
   glTranslatef (1.0, 0.0, 0.0);
   glPushMatrix();
   glScalef (2.0, 0.4, 1.0);
   wireCube(1.0);
   glPopMatrix();

   glPopMatrix();
   //swapBuffers();
}

// Resize function adapted from reshape function

void Robot::resizeGL(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(65.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef (0.0, 0.0, -5.0);
}

// Key press event function to replace keyboard function.  This
// function is modified to only take one argument (the keyboard input)
// instead of receiving three, as in the keyboard function.

void Robot::keyPressEvent(QKeyEvent *e)
{
   switch (e->key()) {
      case 's':
         shoulder = (shoulder + 5) % 360;
         updateGL();
         break;
      case 'S':
         shoulder = (shoulder - 5) % 360;
         updateGL();
         break;
      case 'e':
         elbow = (elbow + 5) % 360;
         updateGL();
         break;
      case 'E':
         elbow = (elbow - 5) % 360;
         updateGL();
         break;
      case 27:
         exit(0);
         break;
      default:
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

    Robot w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}

