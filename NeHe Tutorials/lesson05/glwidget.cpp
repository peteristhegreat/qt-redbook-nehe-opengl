/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#include "glwidget.h"
#include <QKeyEvent>
#include <QTimer>

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 5");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    this->rtri = 0;
    this->rquad = 0;
}

// Empty destructor
GLWidget::~GLWidget() {}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background

    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL); // Set type of depth test

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations
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

// Draw a pyramid using OpenGL
void GLWidget::drawPyramid() {
    glBegin(GL_TRIANGLES);
        // Front face of triangle
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glVertex3f(0.0f, 1.0f, 0.0f); // Top of triangle
        glColor3f(0.0f, 1.0f, 0.0f); // Green color
        glVertex3f(-1.0f, -1.0f, 1.0); // Left of triangle
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        glVertex3f(1.0f, -1.0f, 1.0f); // Right of triangle
        // Right face of triangle
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glVertex3f(0.0f, 1.0f, 0.0f); // Top of triangle
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        glVertex3f(1.0f, -1.0f, 1.0f); // Left of triangle
        glColor3f(0.0f, 1.0f, 0.0f); // Green color
        glVertex3f(1.0f, -1.0f, -1.0f); // Right of triangle
        // Back face of triangle
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glVertex3f(0.0f, 1.0f, 0.0f); // Top of triangle
        glColor3f(0.0f, 1.0f, 0.0f); // Green color
        glVertex3f(1.0f, -1.0f, -1.0f); // Left of triangle
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        glVertex3f(-1.0f, -1.0f, -1.0f); // Right of triangle
        // Left face of triangle
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glVertex3f(0.0f, 1.0f, 0.0f); // Top of triangle
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        glVertex3f(-1.0f, -1.0f, -1.0f); // Left of triangle
        glColor3f(0.0f, 1.0f, 0.0f); // Green color
        glVertex3f(-1.0f, -1.0f, 1.0f); // Right of triangle
    glEnd();
}

// Draw a cube using OpenGL
void GLWidget::drawCube() {
    glBegin(GL_QUADS);
        // Top face of quad
        glColor3f(0.0f, 1.0f, 0.0f); // Green color
        glVertex3f(1.0f, 1.0f, -1.0f); // Top right of quad
        glVertex3f(-1.0f, 1.0f, -1.0f); // Top left of quad
        glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom left of quad
        glVertex3f(1.0f, 1.0f, 1.0f); // Bottom right of quad
        // Bottom face of quad
        glColor3f(1.0f, 0.5f, 0.0f); // Orange color
        glVertex3f(1.0f, -1.0f, 1.0f); // Top right of quad
        glVertex3f(-1.0f, -1.0f, 1.0f); // Top left of quad
        glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom left of quad
        glVertex3f(1.0f, -1.0f, -1.0f); // Bottom right of quad
        // Front face of quad
        glColor3f(1.0f, 0.0f, 0.0f); // Red color
        glVertex3f(1.0f, 1.0f, 1.0f); // Top right of quad
        glVertex3f(-1.0f, 1.0f, 1.0f); // Top left of quad
        glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom left of quad
        glVertex3f(1.0f, -1.0f, 1.0f); // Bottom right of quad
        // Back face of quad
        glColor3f(1.0f,1.0f,0.0f); // Yellow color
        glVertex3f(1.0f, -1.0f, -1.0f); // Top right of quad
        glVertex3f(-1.0f, -1.0f, -1.0f); // Top left of quad
        glVertex3f(-1.0f, 1.0f, -1.0f); // Bottom left of quad
        glVertex3f(1.0f, 1.0f, -1.0f); // Bottom right of quad
        // Left face of quad
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        glVertex3f(-1.0f, 1.0f, 1.0f); // Top right of quad
        glVertex3f(-1.0f, 1.0f, -1.0f); // Top left of quad
        glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom left of quad
        glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom right of quad
        // Right face of quad
        glColor3f(1.0f, 0.0f, 1.0f); // Violet color
        glVertex3f(1.0f, 1.0f, -1.0f); // Top right of quad
        glVertex3f(1.0f, 1.0f, 1.0f); // Top left of quad
        glVertex3f(1.0f, -1.0f, 1.0f); // Bottom left of quad
        glVertex3f(1.0f, -1.0f, -1.0f); // Bottom right of quad
    glEnd();
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    // Draw pyramid in the left side of the window
    glLoadIdentity(); // Reset current modelview matrix
    glTranslatef(-1.5f, 0.0f, -6.0f); // Move 1.5 units to the left and 6 units into the screen
    glRotatef(rtri, 0.0f, 1.0f, 0.0f); // Rotate triangle on Y-axis
    drawPyramid();

    // Draw cube in the right side of the window
    glLoadIdentity(); // Reset current modelview matrix
    glTranslatef(1.5f, 0.0f, -7.0f); // Move 1.5 units to the right and 7 units into the screen
    glRotatef(rquad, 1.0f, 0.0f, 0.0f); // Rotate quad on X-axis
    glColor3f(0.5f, 0.5f, 1.0f); // Blue shade
    drawCube();

    rtri += 1.0f;
    rquad -= 0.60f;

    // Framerate control
    int delay = time.msecsTo(QTime::currentTime());
    if (delay == 0)
        delay = 1;
    time = QTime::currentTime();
    timer->start(qMax(0, 20 - delay));
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
