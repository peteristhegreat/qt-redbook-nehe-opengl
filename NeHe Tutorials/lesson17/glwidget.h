/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <GL/GLU.h>
#include <QTime>

class QTimer;

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
    void glPrint(int x, int y, const char *text, int italicSet);
    void buildFont();
    QTimer *timer;
    QTime time;
    GLuint texture[2];
    GLuint base;
    GLfloat aspect;
    GLfloat cnt1, cnt2;
};

#endif // GLWIDGET_H
