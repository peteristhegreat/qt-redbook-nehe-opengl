/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#define NUM_OF_STARS 50

#include <QGLWidget>
#include <QTime>
#include <GL/GLU.h>

class QTimer;

typedef struct star {
    GLubyte r, g, b, a;
    GLfloat dist, angle;
} Star;

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget();
    ~GLWidget();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);

private:
    QTimer *timer;
    QTime time;
    bool twinkle;
    int twinkleChanged;
    Star star[NUM_OF_STARS];
    GLfloat zoom, tilt, spin;
    GLuint texture;
};

#endif // GLWIDGET_H
