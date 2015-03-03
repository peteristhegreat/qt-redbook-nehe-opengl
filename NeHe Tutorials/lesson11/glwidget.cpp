/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#define PI 3.141592654f

#include "glwidget.h"
#include <cmath>
#include <QKeyEvent>
#include <QTimer>

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 11");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    waveWait = 3;
    xrot = yrot = zrot = 0.0f;

    // Loop through X plane
    for (int x = 0; x < 45; ++x) {
        // Loop through Y plane
        for (int y = 0; y < 45; ++y) {
            // Apply wave to mesh
            points[x][y][0] = x/5.0f - 4.5f;
            points[x][y][1] = y/5.0f - 4.5f;
            points[x][y][2] = sin(x/5.0f * 40.0f / 360.0f * PI * 2.0f);
        }
    }
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteTextures(1, &texture);
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture = bindTexture(QPixmap(":/img/Tim.bmp"));
    glEnable(GL_TEXTURE_2D);

    // Set up depth testing
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    // Set up fill mode for polygons
    glPolygonMode(GL_BACK, GL_FILL); // Draw back face filled in
    glPolygonMode(GL_FRONT, GL_LINE); // Draw front face using lines (wireframe)
}

// This is called when the OpenGL window is resized
void GLWidget::resizeGL(int width, int height) {
    // Prevent divide by zero (in the gluPerspective call)
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height); // Reset current viewport

    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Reset projection matrix

    gluPerspective(45.0f, static_cast<GLfloat>(width)/height, 0.1f, 100.0f); // Calculate aspect ratio

    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity(); // Reset modelview matrix
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    glLoadIdentity(); // Reset current matrix
    glTranslatef(0.0f, 0.0f, -12.0f); // Translate 12 units into the screen

    // Rotate on axises
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(zrot, 0.0f, 0.0f, 1.0f);

    // Draw quads
    glBegin(GL_QUADS);
    // Loop through X plane
    for (int x = 0; x < 44; ++x) {
        // Loop through Y plane
        for (int y = 0; y < 44; ++y) {
            glTexCoord2f(x/44.0f, y/44.0f); // First texture coordinate (bottom left)
            glVertex3f(points[x][y][0], points[x][y][1], points[x][y][2]);
            glTexCoord2f(x/44.0f, (y+1)/44.0f); // Second texture coordinate (top left)
            glVertex3f(points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);
            glTexCoord2f((x+1)/44.0f, (y+1)/44.0f); // Third texture coordinate (top right)
            glVertex3f(points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2]);
            glTexCoord2f((x+1)/44.0f, y/44.0f); // Fourth texture coordinate (bottom right)
            glVertex3f(points[x+1][y][0], points[x+1][y][1], points[x+1][y][2]);
        }
    }
    glEnd();

    // Do the wave!
    if (!waveWait) {
        float first;
        for (int y = 0; y < 45; ++y) { // This loop rotates the wave
            first = points[0][y][2];
            for (int x = 0; x < 44; ++x)
                points[x][y][2] = points[x+1][y][2];
            points[44][y][2] = first;
        }
        waveWait = 3;
    }
    --waveWait;

    // Update rotation
    xrot += 0.3f;
    yrot += 0.2f;
    zrot += 0.4f;

    // Framerate control
    int delay = time.msecsTo(QTime::currentTime());
    if (delay == 0)
        delay = 1;
    time = QTime::currentTime();
    timer->start(qMax(0, 15 - delay));
}

// Key handler
void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        close(); // Quit on Escape
        break;
    case Qt::Key_F1:
        setWindowState(windowState() ^ Qt::WindowFullScreen); // Toggle fullscreen on F1
        break;
    default:
        QGLWidget::keyPressEvent(event); // Let base class handle the other keys
    }
}

void GLWidget::changeEvent(QEvent *event) {
    switch (event->type()) {
    case QEvent::WindowStateChange:
        // Hide cursor if the window is fullscreen, otherwise show it
        if (windowState() == Qt::WindowFullScreen)
            setCursor(Qt::BlankCursor);
        else
            unsetCursor();
        break;
    default:
        break;
    }
}
