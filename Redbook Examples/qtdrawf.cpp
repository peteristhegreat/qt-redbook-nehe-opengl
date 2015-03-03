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
 */

/*
 *  drawf.c
 *  Draws the bitmapped letter F on the screen (several times).
 *  This demonstrates use of the glBitmap() call.
 */

#include <stdlib.h>
#include <qgl.h>
#include <qapplication.h>

GLubyte rasters[24] = {
   0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00,
   0xff, 0x00, 0xff, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00,
   0xff, 0xc0, 0xff, 0xc0};

class DrawF : public QGLWidget
{
public:
    DrawF( QWidget *parent=0, const char *name=0 ) :
	QGLWidget(parent, name) {};
protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
};

void DrawF::initializeGL()
{
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
   glClearColor (0.0, 0.0, 0.0, 0.0);
}

void DrawF::paintGL(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glRasterPos2i (20, 20);
   glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
   glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
   glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
   glFlush();
}

void DrawF::resizeGL(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho (0, w, 0, h, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   //glLoadIdentity();
}

void DrawF::keyPressEvent( QKeyEvent *key )
{
   switch (key->ascii()) {
      case 27:	// Escape Key
          exit(0);
	  break;
      default:
          break;
   }
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   QApplication a( argc, argv );
   
   if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support.  Exiting . . ." );
	return -1;
   }
   
   DrawF w;
   w.setGeometry(100,100,100,100);
   a.setMainWidget( &w );
   w.show();
   return a.exec();
}
