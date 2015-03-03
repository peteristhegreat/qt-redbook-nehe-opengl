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
#include <QHash>

#define MAX_PARTICLES 1000

class QTimer;

typedef struct particle {
    float life; // Life
    float fade; // Fade speed
    float r, g, b; // Color
    float x, y, z; // Position
    float xi, yi, zi; // Movement
    float xg, yg, zg; // Gravity
} Particle;

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
    void keyReleaseEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);

private:
    QTimer *timer;
    QTime time;
    GLuint texture;
    bool colorChanged, rainbowChanged;
    int changeAmount;
    QHash<GLuint, QString> colorName;
    Particle particle[MAX_PARTICLES];
    static float colors[12][3];
    float zoom, slowdown;
    bool rainbow, randomColors;
    GLuint col;
    unsigned int keepColorDuration, keepColorCount;
    QList<int> activeKeys;
    GLfloat xspeed, yspeed;
};

#endif // GLWIDGET_H
