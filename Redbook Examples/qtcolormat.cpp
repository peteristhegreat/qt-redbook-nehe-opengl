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
 *  colormat.c
 *  After initialization, the program will be in
 *  ColorMaterial mode.  Interaction:  pressing the 
 *  mouse buttons will change the diffuse reflection values.
 */
#include<qgl.h>
#include<qapplication.h>
#include<qkeycode.h>
#include<qevent.h>

GLUquadricObj *GLOBAL_sphereObj;

GLfloat diffuseMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };

GLfloat vBLUE[]    = {0.0, 0.0, 1.0};
GLfloat vDKGREEN[] = {0.0, 0.6, 0.0};
GLfloat vMAGENTA[] = {1.0, 0.0, 1.0};
GLfloat vRED[]     = {1.0, 0.0, 0.0};
GLfloat vGRAY[]    = {0.6, 0.6, 0.6};
GLfloat vYELLOW[]  = {1.0, 1.0, 0.0};
GLfloat vWHITE[]   = {1.0, 1.0, 1.0};
GLfloat vGREEN[]   = {0.0, 1.0, 0.0};
GLfloat vCYAN[]    = {0.0, 1.0, 1.0};
GLfloat vBLACK[]   = {0.0, 0.0, 0.0};

#define GLOBE    1

class ColormatWidget : public QGLWidget
{
public:
	ColormatWidget(QWidget *parent=0, const char *name=0):
		QGLWidget(parent, name) {};
		
protected:
	void initializeGL(void);
	void resizeGL( int w, int h);
	void keyPressEvent( QKeyEvent *e);
	void paintGL();
	void mousePressEvent(QMouseEvent *e);

};


/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
void ColormatWidget::initializeGL(void) 
{
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   
   glNewList(GLOBE, GL_COMPILE); 
        //glColor3fv(vBLUE);
        //glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vBLUE );
        GLOBAL_sphereObj = gluNewQuadric (); 
        //gluQuadricDrawStyle (GLOBAL_sphereObj, GLU_SMOOTH); 
        gluSphere (GLOBAL_sphereObj, 1.0, 40, 40); 
   glEndList();
}

void ColormatWidget::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glCallList(GLOBE);
   //glutSolidSphere(1.0, 20, 16);
   glFlush ();
}

void ColormatWidget::resizeGL (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
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

 void ColormatWidget::mousePressEvent(QMouseEvent *e)
{
   switch (e->button() ) {
      case LeftButton:
            diffuseMaterial[0] += 0.1;
            if (diffuseMaterial[0] > 1.0)
               diffuseMaterial[0] = 0.0;
            glColor4fv(diffuseMaterial);
	    update();
         break;
      case MidButton:
            diffuseMaterial[1] += 0.1;
            if (diffuseMaterial[1] > 1.0)
               diffuseMaterial[1] = 0.0;
            glColor4fv(diffuseMaterial);
	    update();
         break;
      case RightButton:
            diffuseMaterial[2] += 0.1;
            if (diffuseMaterial[2] > 1.0)
               diffuseMaterial[2] = 0.0;
            glColor4fv(diffuseMaterial);
	    update();
         break;
      default:
         break; 
   } 
}


void ColormatWidget::keyPressEvent(QKeyEvent *e)
{
   switch (e->key() ) {
      case Key_Escape:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
	QApplication::setColorSpec( QApplication::CustomColor );
   	QApplication a( argc, argv);
   
   	if ( !QGLFormat::hasOpenGL())
     	{
     		qWarning( "This system has no OpenGL support.  Exiting...");
     		return -1;
     	}
	
	ColormatWidget c;
	a.setMainWidget( &c);
	c.show();
	return a.exec();
}
