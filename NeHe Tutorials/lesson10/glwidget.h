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
#include <QTime>
#include <QTextStream>
#include <QHash>
#include <GL/GLU.h>

class QTimer;

typedef struct vertex {
    float x, y, z;
    float u, v;
} Vertex;

typedef struct triangle {
    Vertex vertex[3];
} Triangle;

typedef struct sector {
    int numTriangles;
    Triangle *triangle;
} Sector;

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget();
    ~GLWidget();
    bool init();

protected:
    QString getLine(QTextStream &in);
    bool setupWorld();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);

private:
    void movePlayer();
    QTimer *timer;
    QTime time;
    GLuint filter, texture[3];
    Sector sector1;
    GLfloat xpos, yrot, zpos, lookupdown;
    GLfloat walkbias, walkbiasangle;
    bool lightChanged, filterChanged, blendChanged;
    int changeAmount;
    QHash<GLuint, QString> filterName;
    bool keyUp, keyDown, keyLeft, keyRight, keyPageUp, keyPageDown;
};

#endif // GLWIDGET_H
