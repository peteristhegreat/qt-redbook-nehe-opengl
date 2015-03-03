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
 
//////////////////////////////////////////////////////////////////
// Tyler Davis
// ME570
//
// Filename: TdAlpha3D.cpp
//
// Description: This file demonstrates the effects of blending
// 		when using OpenGL to make an object appear translucent. I'm
// 		converting it to use C++ instead of C, and QT instead of glut
//
// Revision history:
// October 16, 2001 - Began the conversion
//		Removed glutIdleFunc by simply making the animate
//		function go until finished.
//		Used gluSphere instead of glutSolidSphere
//		Used gluCylinder instead of glutSolidCube - switch to cube later
//		Used paintGL instead of glutPostRedisplay. - this is not exactly
//			the best method. updateGL() - a QT command - is a more
//			appropriate replacement.
//		Used glFlush instead of glutSwapBuffers. This is confusing
//		because the original code initializes the window in
//		single buffer mode (I think). Still need to prove this works.
// October 18, 2001 - Alright! I fixed the double buffering problem. It turns
//		out that the screen needed to be redrawn, but that's the
//		responsibility of the window system, not OpenGL. So I used
//		the QT command repaint() which repaints immediatedly. Beauty!
//		I just finished the cube. Had to define all the polygons, and
//		their normals. The list lets me define it once, and later some
//		tranformations are done to it. Ok, now I've replaced repaint()
//		with updateGL(). It seems to be more appropriate. I've also
//		learned that QGLWidgets are double-buffered by default, and
//		calling paintGL() swaps buffers automatically.
//		Now I have a timer running the animation so it will be smooth.
//		I put the two objects closer to each other so we don't have to
//		wait so long for the animation. I think it's all good now!
//
//////////////////////////////////////////////////////////////////

/*
 *  alpha3D.c
 *  This program demonstrates how to intermix opaque and
 *  alpha blended polygons in the same scene, by using 
 *  glDepthMask.  Press the 'a' key to animate moving the 
 *  transparent object through the opaque object.  Press 
 *  the 'r' key to reset the scene.
 */

#include <stdio.h>
//#include <GL/glut.h>

#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <stdlib.h>

#define MAXZ 1.0	// These values have been changed to 1 so that the
#define MINZ -1.0	// animation happens faster (less distance to cover)
#define ZINC 0.04

static float solidZ = MAXZ;
static float transparentZ = MINZ;
static GLuint sphereList, cubeList;

class TdAlpha3D : public QGLWidget
{
public:
	TdAlpha3D(QWidget *parent=0, const char *name=0);
	~TdAlpha3D();
	GLUquadricObj* sphere;

protected:
	void initializeGL(void);
	void paintGL(void);
	void resizeGL(int w, int h);
	void animate(void);
	void keyPressEvent(QKeyEvent *e);
	void timerEvent(QTimerEvent*);

};

TdAlpha3D::TdAlpha3D(QWidget *parent, const char *name)
	: QGLWidget(parent, name)
{	
}


TdAlpha3D::~TdAlpha3D()
{	gluDeleteQuadric(sphere);

}

void TdAlpha3D::initializeGL(void)
{
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.15 };
   GLfloat mat_shininess[] = { 100.0 };
   GLfloat position[] = { 0.5, 0.5, 1.0, 0.0 };

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

   sphereList = glGenLists(1);
   // This is my attempt at using GLU to make a sphere:
   sphere = gluNewQuadric();
   gluQuadricDrawStyle(sphere, GLU_FILL);
   gluQuadricNormals(sphere, GLU_SMOOTH);

   glNewList(sphereList, GL_COMPILE);
   //   glutSolidSphere (0.4, 16, 16); // This is the original code
	gluSphere(sphere, .4, 16, 16);
   glEndList();

   cubeList = glGenLists(1);
   glNewList(cubeList, GL_COMPILE);
	// Now I'm going to make my own cube.
	glBegin(GL_QUADS);
		// First the face
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-.3, -.3, .3);
		glVertex3f(-.3, .3, .3);
		glVertex3f(.3, .3, .3);
		glVertex3f(.3, -.3, .3);
		// Now the right side
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(.3, -.3, .3);
		glVertex3f(.3, .3, .3);
		glVertex3f(.3, .3, -.3);
		glVertex3f(.3, -.3, -.3);
		// Now the left side
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-.3, -.3, .3);
		glVertex3f(-.3, .3, .3);
		glVertex3f(-.3, .3, -.3);
		glVertex3f(-.3, -.3, -.3);
		// Next the back
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(-.3, -.3, -.3);
		glVertex3f(-.3, .3, -.3);
		glVertex3f(.3, .3, -.3);
		glVertex3f(.3, -.3, -.3);
		// Next the top
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-.3, .3, .3);
		glVertex3f(-.3, .3, -.3);
		glVertex3f(.3, .3, -.3);
		glVertex3f(.3, .3, .3);
		// Last the bottom
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-.3, -.3, .3);
		glVertex3f(-.3, -.3, -.3);
		glVertex3f(.3, -.3, -.3);
		glVertex3f(.3, -.3, .3);
	glEnd();
   glEndList();
   
}

void TdAlpha3D::paintGL(void)
{
   GLfloat mat_solid[] = { 0.75, 0.75, 0.0, 1.0 };
   GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat mat_transparent[] = { 0.0, 0.8, 0.8, 0.6 };
   GLfloat mat_emission[] = { 0.0, 0.3, 0.3, 0.6 };

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix ();
      glTranslatef (-0.15, -0.15, solidZ);
      glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
      glCallList (sphereList);
   glPopMatrix ();

   glPushMatrix ();
      glTranslatef (0.15, 0.15, transparentZ);
      glRotatef (15.0, 1.0, 1.0, 0.0);
      glRotatef (30.0, 0.0, 1.0, 0.0);
      glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
      glEnable (GL_BLEND);
      glDepthMask (GL_FALSE);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE);
      glCallList (cubeList);
      glDepthMask (GL_TRUE);
      glDisable (GL_BLEND);
   glPopMatrix ();

//   glutSwapBuffers(); // A QGLWidget is by default double buffered, so
			// calling updateGL() will perform the swap.
	
}

void TdAlpha3D::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLint) w, (GLint) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
               1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
               1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void TdAlpha3D::animate(void)
{
   if(solidZ > MINZ && transparentZ < MAXZ)
   {	solidZ -= ZINC;
   	transparentZ += ZINC;
	updateGL();
   }
}

void TdAlpha3D::keyPressEvent(QKeyEvent *e)
{
   switch (e->key())
   {
      case Key_A:
         solidZ = MAXZ;
         transparentZ = MINZ;
//	 glutIdleFunc(animate); // I can't find a similar function in QT, so I
				// just start a timer, which then calls the
				// animation function. (See the next line)
         startTimer(100);
         break;
     case Key_R:
         killTimers();		// This stops all timers, I only have one,and
	 			// resets everything.
         solidZ = MAXZ;
         transparentZ = MINZ;
         updateGL();
         break;
      case Key_Escape:
        exit(0);
    }
    e->accept();
}

void TdAlpha3D::timerEvent(QTimerEvent*)
{	animate();
}

int main(int argc, char** argv)
{
   QApplication app(argc, argv);
   
   if (!QGLFormat::hasOpenGL())
   {	qWarning("This system has no OpenGL support. Exiting.");
   	return -1;
   }
   
   TdAlpha3D goodboy;
   app.setMainWidget(&goodboy);
   goodboy.resize(500, 500);
   goodboy.show();
   return app.exec();
   
}
