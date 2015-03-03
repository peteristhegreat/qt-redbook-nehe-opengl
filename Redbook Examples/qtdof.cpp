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
 *  dof.c
 *  This program demonstrates use of the accumulation buffer to
 *  create an out-of-focus depth-of-field effect.  The teapots
 *  are drawn several times into the accumulation buffer.  The
 *  viewing volume is jittered, except at the focal point, where
 *  the viewing volume is at the same position, each time.  In
 *  this case, the gold teapot remains in focus.
 */
// converted to qt by Jared Haslam


#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <stdlib.h>
#include <math.h>
#include "jitter.h"
//#include <qgcolormap.h>
//#include "glteapots.h"

#ifdef WIN32
#define near zNear
#define far zFar
#endif

#define PI_ 3.14159265358979323846

class TeaWidget : public QGLWidget
{
public:
	TeaWidget(QWidget *parent=0, const char *name=0):
		QGLWidget(parent,name){};
protected:
	void renderTeapot(GLfloat x, GLfloat y, GLfloat z, 
   GLfloat ambr, GLfloat ambg, GLfloat ambb, 
   GLfloat difr, GLfloat difg, GLfloat difb, 
   GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine);
	void teapot();
	void accFrustum(GLdouble left, GLdouble right, GLdouble bottom,
   GLdouble top, GLdouble near, GLdouble far, GLdouble pixdx,
   GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus);
	void accPerspective(GLdouble fovy, GLdouble aspect, 
   GLdouble near, GLdouble far, GLdouble pixdx, GLdouble pixdy, 
   GLdouble eyedx, GLdouble eyedy, GLdouble focus);
	void initializeGL(void);
	void resizeGL(int w, int h);
	void paintGL();
	void keyPressEvent (QKeyEvent *e);
};

GLuint teapotList;

/* accFrustum()
 * The first 6 arguments are identical to the glFrustum() call.
 *  
 * pixdx and pixdy are anti-alias jitter in pixels. 
 * Set both equal to 0.0 for no anti-alias jitter.
 * eyedx and eyedy are depth-of field jitter in pixels. 
 * Set both equal to 0.0 for no depth of field effects.
 *
 * focus is distance from eye to plane in focus. 
 * focus must be greater than, but not equal to 0.0.
 *
 * Note that accFrustum() calls glTranslatef().  You will 
 * probably want to insure that your ModelView matrix has been 
 * initialized to identity before calling accFrustum().
 */
void TeaWidget::accFrustum(GLdouble left, GLdouble right, GLdouble bottom, 
   GLdouble top, GLdouble near, GLdouble far, GLdouble pixdx, 
   GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
   GLdouble xwsize, ywsize; 
   GLdouble dx, dy;
   GLint viewport[4];

   glGetIntegerv (GL_VIEWPORT, viewport);
	
   xwsize = right - left;
   ywsize = top - bottom;
	
   dx = -(pixdx*xwsize/(GLdouble) viewport[2] + eyedx*near/focus);
   dy = -(pixdy*ywsize/(GLdouble) viewport[3] + eyedy*near/focus);
	
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum (left + dx, right + dx, bottom + dy, top + dy, near, far);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef (-eyedx, -eyedy, 0.0);
}

/*  accPerspective()
 * 
 *  The first 4 arguments are identical to the gluPerspective() call.
 *  pixdx and pixdy are anti-alias jitter in pixels. 
 *  Set both equal to 0.0 for no anti-alias jitter.
 *  eyedx and eyedy are depth-of field jitter in pixels. 
 *  Set both equal to 0.0 for no depth of field effects.
 *
 *  focus is distance from eye to plane in focus. 
 *  focus must be greater than, but not equal to 0.0.
 *
 *  Note that accPerspective() calls accFrustum().
 */
void TeaWidget::accPerspective(GLdouble fovy, GLdouble aspect, 
   GLdouble near, GLdouble far, GLdouble pixdx, GLdouble pixdy, 
   GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
   GLdouble fov2,left,right,bottom,top;

   fov2 = ((fovy*PI_) / 180.0) / 2.0;

   top = near / (cos(fov2) / sin(fov2));
   bottom = -top;

   right = top * aspect;
   left = -right;

   accFrustum (left, right, bottom, top, near, far,
               pixdx, pixdy, eyedx, eyedy, focus);
}

void TeaWidget::initializeGL(void)
{
   GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat position[] = { 0.0, 3.0, 3.0, 0.0 };
    
   GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
   GLfloat local_view[] = { 0.0 };

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
    
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

   glFrontFace (GL_CW);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);
   glEnable(GL_DEPTH_TEST);

   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearAccum(0.0, 0.0, 0.0, 0.0); 
/*  make teapot display list */
   teapot();
}

void TeaWidget::renderTeapot (GLfloat x, GLfloat y, GLfloat z, 
   GLfloat ambr, GLfloat ambg, GLfloat ambb, 
   GLfloat difr, GLfloat difg, GLfloat difb, 
   GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine)
{
   GLfloat mat[4];

   glPushMatrix();
   glTranslatef (x, y, z);
   mat[0] = ambr; mat[1] = ambg; mat[2] = ambb; mat[3] = 1.0;	
   glMaterialfv (GL_FRONT, GL_AMBIENT, mat);
   mat[0] = difr; mat[1] = difg; mat[2] = difb;	
   glMaterialfv (GL_FRONT, GL_DIFFUSE, mat);
   mat[0] = specr; mat[1] = specg; mat[2] = specb;
   glMaterialfv (GL_FRONT, GL_SPECULAR, mat);
   glMaterialf (GL_FRONT, GL_SHININESS, shine*128.0);
   glCallList(teapotList);
   glPopMatrix();
}

/*  display() draws 5 teapots into the accumulation buffer 
 *  several times; each time with a jittered perspective.
 *  The focal point is at z = 5.0, so the gold teapot will 
 *  stay in focus.  The amount of jitter is adjusted by the
 *  magnitude of the accPerspective() jitter; in this example, 0.33.
 *  In this example, the teapots are drawn 8 times.  See jitter.h
 */
void TeaWidget::paintGL()
{
   int jitter;
   GLint viewport[4];

   glGetIntegerv (GL_VIEWPORT, viewport);
   glClear(GL_ACCUM_BUFFER_BIT);
   glColor3f(1,1,1);
   glPushMatrix();
   glPopMatrix();
   for (jitter = 0; jitter < 8; jitter++) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      accPerspective (45.0, 
         (GLdouble) viewport[2]/(GLdouble) viewport[3], 
         1.0, 15.0, 0.0, 0.0,
         0.33*j8[jitter].x, 0.33*j8[jitter].y, 5.0);

	//ruby, gold, silver, emerald, and cyan teapots	
      renderTeapot (-1.1, -0.5, -4.5, 0.1745, 0.01175, 
                    0.01175, 0.61424, 0.04136, 0.04136, 
                    0.727811, 0.626959, 0.626959, 0.6);
      renderTeapot (-0.5, -0.5, -5.0, 0.24725, 0.1995, 
                    0.0745, 0.75164, 0.60648, 0.22648, 
                    0.628281, 0.555802, 0.366065, 0.4);
      renderTeapot (0.2, -0.5, -5.5, 0.19225, 0.19225, 
                    0.19225, 0.50754, 0.50754, 0.50754, 
                    0.508273, 0.508273, 0.508273, 0.4);
      renderTeapot (1.0, -0.5, -6.0, 0.0215, 0.1745, 0.0215, 
                    0.07568, 0.61424, 0.07568, 0.633, 
                    0.727811, 0.633, 0.6);
      renderTeapot (1.8, -0.5, -6.5, 0.0, 0.1, 0.06, 0.0, 
                    0.50980392, 0.50980392, 0.50196078, 
                    0.50196078, 0.50196078, .25);
      glAccum (GL_ACCUM, 0.125);
   }
   glAccum (GL_RETURN, 1.0);
   glFlush();
}

void TeaWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void TeaWidget::keyPressEvent(QKeyEvent *e)
{
   switch (e->key()) {
      case Key_Escape:
         exit(0);
         break;
   }
}

void TeaWidget::teapot()
{
    static long patchdata[][16] = {
        {102,103,104,105,4,5,6,7,8,9,10,11,12,13,14,15},
        {12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27},
        {24,25,26,27,29,30,31,32,33,34,35,36,37,38,39,40},
        {96,96,96,96,97,98,99,100,101,101,101,101,0,1,2,3,},
        {0,1,2,3,106,107,108,109,110,111,112,113,114,115,116,117},
        {118,118,118,118,124,122,119,121,123,126,125,120,40,39,38,37},
        {41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56},
        {53,54,55,56,57,58,59,60,61,62,63,64,28,65,66,67},
        {68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83},
        {80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}
    };
    static float cpdata[][3] = {
        {0.2,0,2.7},{0.2,-0.112,2.7},{0.112,-0.2,2.7},{0,-0.2,2.7},
        {1.3375,0,2.53125},{1.3375,-0.749,2.53125},{0.749,-1.3375,2.53125},
        {0,-1.3375,2.53125},{1.4375,0,2.53125},{1.4375,-0.805,2.53125},
        {0.805,-1.4375,2.53125},{0,-1.4375,2.53125},{1.5,0,2.4},
        {1.5,-0.84,2.4},{0.84,-1.5,2.4},{0,-1.5,2.4},{1.75,0,1.875},
        {1.75,-0.98,1.875},{0.98,-1.75,1.875},{0,-1.75,1.875},{2,0,1.35},
        {2,-1.12,1.35},{1.12,-2,1.35},{0,-2,1.35},{2,0,0.9},{2,-1.12,0.9},
        {1.12,-2,0.9},{0,-2,0.9},{-2,0,0.9},{2,0,0.45},{2,-1.12,0.45},
        {1.12,-2,0.45},{0,-2,0.45},{1.5,0,0.225},{1.5,-0.84,0.225},
        {0.84,-1.5,0.225},{0,-1.5,0.225},{1.5,0,0.15},{1.5,-0.84,0.15},
        {0.84,-1.5,0.15},{0,-1.5,0.15},{-1.6,0,2.025},{-1.6,-0.3,2.025},
        {-1.5,-0.3,2.25},{-1.5,0,2.25},{-2.3,0,2.025},{-2.3,-0.3,2.025},
        {-2.5,-0.3,2.25},{-2.5,0,2.25},{-2.7,0,2.025},{-2.7,-0.3,2.025},
        {-3,-0.3,2.25},{-3,0,2.25},{-2.7,0,1.8},{-2.7,-0.3,1.8},{-3,-0.3,1.8},
        {-3,0,1.8},{-2.7,0,1.575},{-2.7,-0.3,1.575},{-3,-0.3,1.35},{-3,0,1.35},
        {-2.5,0,1.125},{-2.5,-0.3,1.125},{-2.65,-0.3,0.9375},{-2.65,0,0.9375},
        {-2,-0.3,0.9},{-1.9,-0.3,0.6},{-1.9,0,0.6},{1.7,0,1.425},
        {1.7,-0.66,1.425},{1.7,-0.66,0.6},{1.7,0,0.6},{2.6,0,1.425},
        {2.6,-0.66,1.425},{3.1,-0.66,0.825},{3.1,0,0.825},{2.3,0,2.1},
        {2.3,-0.25,2.1},{2.4,-0.25,2.025},{2.4,0,2.025},{2.7,0,2.4},
        {2.7,-0.25,2.4},{3.3,-0.25,2.4},{3.3,0,2.4},{2.8,0,2.475},
        {2.8,-0.25,2.475},{3.525,-0.25,2.49375},{3.525,0,2.49375},
        {2.9,0,2.475},{2.9,-0.15,2.475},{3.45,-0.15,2.5125},{3.45,0,2.5125},
        {2.8,0,2.4},{2.8,-0.15,2.4},{3.2,-0.15,2.4},{3.2,0,2.4},{0,0,3.15},
        {0.8,0,3.15},{0.8,-0.45,3.15},{0.45,-0.8,3.15},{0,-0.8,3.15},
        {0,0,2.85},{1.4,0,2.4},{1.4,-0.784,2.4},{0.784,-1.4,2.4},{0,-1.4,2.4},
        {0.4,0,2.55},{0.4,-0.224,2.55},{0.224,-0.4,2.55},{0,-0.4,2.55},
        {1.3,0,2.55},{1.3,-0.728,2.55},{0.728,-1.3,2.55},{0,-1.3,2.55},
        {1.3,0,2.4},{1.3,-0.728,2.4},{0.728,-1.3,2.4},{0,-1.3,2.4},{0,0,0},
        {1.425,-0.798,0},{1.5,0,0.075},{1.425,0,0},{0.798,-1.425,0},
        {0,-1.5,0.075},{0,-1.425,0},{1.5,-0.84,0.075},{0.84,-1.5,0.075}
    };
    static float tex[2][2][2] = {{{0, 0},{1, 0}},{{0, 1},{1, 1}}};

    if ( !glIsList( teapotList ) ) {
        float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
        long grid = 14;

        teapotList = glGenLists( 1 );
        glNewList( teapotList, GL_COMPILE );
        glPushMatrix();
        glRotatef( 270.0, 1.0, 0.0, 0.0 );
        glScalef( 0.25, 0.25, 0.25 );
        glTranslatef( 0.0, 0.0, 0.0 );
        for ( long i = 0; i < 10; i++ ) {
            for ( long j = 0; j < 4; j++ ) {
                for ( long k = 0; k < 4; k++ ) {
                    for ( long l = 0; l < 3; l++ ) {
                        p[j][k][l] = cpdata[patchdata[i][j*4+k]][l];
                        q[j][k][l] = cpdata[patchdata[i][j*4+(3-k)]][l];
                        if ( l == 1 )
                            q[j][k][l] *= -1.0;
                        if ( i < 6 ) {
                            r[j][k][l] = cpdata[patchdata[i][j*4+(3-k)]][l];
                            if ( l == 0 )
                                r[j][k][l] *= -1.0;
                            s[j][k][l] = cpdata[patchdata[i][j*4+k]][l];
                            if ( l == 0 )
                                s[j][k][l] *= -1.0;
                            if ( l == 1 )
                                s[j][k][l] *= -1.0;
                        }
                    }
                }
            }
            glMap2f( GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
                    &tex[0][0][0] );
            glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0] );
            glEnable( GL_MAP2_VERTEX_3);
            glEnable(GL_MAP2_TEXTURE_COORD_2 );
            glMapGrid2f( grid, 0.0, 1.0, grid, 0.0, 1.0 );
            glEvalMesh2( GL_FILL, 0, grid, 0, grid );
            glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0] );
            glEvalMesh2( GL_FILL, 0, grid, 0, grid );
            if ( i < 6 ) {
                glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
                         &r[0][0][0]);
                glEvalMesh2( GL_FILL, 0, grid, 0, grid );
                glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
                         &s[0][0][0] );
                glEvalMesh2( GL_FILL, 0, grid, 0, grid );
            }
        }
        glDisable(GL_MAP2_VERTEX_3);
        glDisable(GL_MAP2_TEXTURE_COORD_2);
        glPopMatrix();
        glEndList();
    }

    glCallList( teapotList );
}



/*  Main Loop
 *  Be certain you request an accumulation buffer.
 */
int main(int argc, char** argv)
{
   QApplication a( argc, argv );

   if (!QGLFormat::hasOpenGL() ){
	qWarning( "This system has no OpenGL support....Exiting.");
	return -1;
   }

   QGLFormat format;

   format.setAccum(true);
   format.setDoubleBuffer(false);
   QGLFormat::setDefaultFormat(format);

   TeaWidget c;
   c.setGeometry(100,100,400,400);
   a.setMainWidget ( &c);
   c.show();
   return a.exec();
}


