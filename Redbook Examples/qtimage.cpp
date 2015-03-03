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
//
//	History:
//	JKH	Modified to use QT instead of glut
//
/*  image.c
 *  This program demonstrates drawing pixels and shows the effect
 *  of glDrawPixels(), glCopyPixels(), and glPixelZoom().
 *  Interaction: moving the mouse while pressing the mouse button
 *  will copy the image in the lower-left corner of the window
 *  to the mouse position, using the current pixel zoom factors.
 *  There is no attempt to prevent you from drawing over the original
 *  image.  If you press the 'r' key, the original image and zoom
 *  factors are reset.  If you press the 'z' or 'Z' keys, you change
 *  the zoom factors.
 */
#include <qgl.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qevent.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

/*	Create checkerboard image	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
GLubyte checkImage[checkImageHeight][checkImageWidth][3];

static GLdouble zoomFactor = 1.0;
static GLint myHeight;
static GLint screeny = 0, screenx = 0;

class ImageWidget : public QGLWidget
{
public:
    ImageWidget( QWidget *parent=0, const char *name=0 );

protected:
    void initializeGL( void );
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e );
    void makeCheckImage( void );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );

private:
    bool down;
};

ImageWidget::ImageWidget( QWidget *parent, const char *name )
     : QGLWidget( parent, name )
{
}

void ImageWidget::mousePressEvent( QMouseEvent *e )
{
    down = TRUE;
    if( down == TRUE )
    {
        screeny = myHeight - (GLint)e->y();
        screenx = (GLint)e->x();
        updateGL();
    }}

void ImageWidget::mouseReleaseEvent( QMouseEvent *e )
{
    down = FALSE;
}

void ImageWidget::mouseMoveEvent( QMouseEvent *e )
{
    if( down == TRUE )
    {
        screeny = myHeight - (GLint)e->y();
        screenx = (GLint)e->x();
        updateGL();
    }
}

void ImageWidget::initializeGL(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void ImageWidget::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   //glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
   glRasterPos2i (screenx, screeny);
   glPixelZoom (zoomFactor, zoomFactor);
   glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
   glFlush ();
}

void ImageWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   myHeight = (GLint) h;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void ImageWidget::keyPressEvent( QKeyEvent *e )
{
   switch ( e->key() )
   {
      case Key_R:
         zoomFactor = 1.0;
         updateGL();
         printf ("zoomFactor reset to 1.0\n");
         break;
      case Key_Z:
         zoomFactor += 0.5;
         if (zoomFactor >= 3.0) 
            zoomFactor = 3.0;
         updateGL();
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case Key_X:
         zoomFactor -= 0.5;
         if (zoomFactor <= 0.5) 
            zoomFactor = 0.5;
         updateGL();
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case Key_Escape:
         exit(0);
         break;
      default:
         break;
   }
}

void ImageWidget::makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = (((i&0x8)==0)^((j&0x8)==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
      }
   }
}

int main(int argc, char** argv)
{   
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() )
    {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    ImageWidget l;
    a.setMainWidget( &l );
    l.show();
    return a.exec();
}
