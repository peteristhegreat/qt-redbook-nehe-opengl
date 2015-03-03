//
// Copyright (c) 1993-1997, Silicon Graphics, Inc.
// ALL RIGHTS RESERVED 
// Permission to use, copy, modify, and distribute this software for 
// any purpose and without fee is hereby granted, provided that the above
// copyright notice appear in all copies and that both the copyright notice
// and this permission notice appear in supporting documentation, and that 
// the name of Silicon Graphics, Inc. not be used in advertising
// or publicity pertaining to distribution of the software without specific,
// written prior permission. 
//
// File:
//  aaindex.c
//
// Description:
//  This program draws shows how to draw anti-aliased lines in color
//  index mode. It draws two diagonal lines to form an X; when 'r' 
//  is typed in the window, the lines are rotated in opposite 
//  directions.
//
// Author:
//   KGM    Modified use Qt instead of glut.
//
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QKeyEvent>
#include <stdlib.h>

#define RAMPSIZE 16
#define RAMP1START 32
#define RAMP2START 48

static float rotAngle = 0.;


class AntiAliasWidget : public QGLWidget
{
  public:
    AntiAliasWidget( QWidget *parent=0 ) : QGLWidget(parent) {};
  protected:
    void initializeGL(void);
    void resizeGL( int w, int h );
    void paintGL();
    void keyPressEvent( QKeyEvent *e);
};

/*  Initialize antialiasing for color index mode,
 *  including loading a green color ramp starting
 *  at RAMP1START, and a blue color ramp starting
 *  at RAMP2START. The ramps must be a multiple of 16.
 */
void AntiAliasWidget::initializeGL(void)
{
  int i;
  QGLColormap colormap;

  for (i = 0; i < RAMPSIZE; i++) 
  {
    //
    // qRgb uses colors between 0..255
    //
    int shade = (int) ((GLfloat) i/(GLfloat) RAMPSIZE) * 255;
    colormap.setEntry ( (int) (RAMP1START+i), qRgb(0, shade, 0));
    colormap.setEntry ( (int) (RAMP2START+i), qRgb(0, 0, shade));
    //glutSetColor(RAMP1START+(GLint)i, 0., shade, 0.);
    //glutSetColor(RAMP2START+(GLint)i, 0., 0., shade);
   }

  this->setColormap (colormap); 

  glEnable (GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glLineWidth (1.5);

  glClearIndex ((GLfloat) RAMP1START);
}

/*  Draw 2 diagonal lines to form an X
*/
void AntiAliasWidget::paintGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  glIndexi(RAMP1START);
  glPushMatrix();
    glRotatef(-rotAngle, 0.0, 0.0, 0.1);
    glBegin (GL_LINES);
      glVertex2f (-0.5, 0.5);
      glVertex2f (0.5, -0.5);
    glEnd ();
  glPopMatrix();

  glIndexi(RAMP2START);
  glPushMatrix();
    glRotatef(rotAngle, 0.0, 0.0, 0.1);
    glBegin (GL_LINES);
      glVertex2f (0.5, 0.5);
      glVertex2f (-0.5, -0.5);
    glEnd ();
  glPopMatrix();

  glFlush();
}

void AntiAliasWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h) 
    gluOrtho2D (-1.0, 1.0, 
      -1.0*(GLfloat)h/(GLfloat)w, 1.0*(GLfloat)h/(GLfloat)w);
  else 
    gluOrtho2D (-1.0*(GLfloat)w/(GLfloat)h, 
      1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void AntiAliasWidget::keyPressEvent(QKeyEvent *e)
{
  switch (e->key()) 
  {
    case Qt::Key_R:  //R key is pressed independent of Shift
      rotAngle += 20.;
      if (rotAngle >= 360.) 
        rotAngle = 0.;
      updateGL();      
      break;
    case Qt::Key_Escape:  /*  Escape Key */
     exit(0);
      break;
    default:
      break;
  }
  e->accept();  // Don't pass any key events to parent
}

/*  Main Loop
*  Open window with initial window size, title bar, 
*  color index display mode, and handle input events.
*/
int main( int argc, char **argv )
{
  QApplication::setColorSpec( QApplication::CustomColor );
  QApplication a( argc, argv );

  AntiAliasWidget w;
  w.show();
  return a.exec();
}

