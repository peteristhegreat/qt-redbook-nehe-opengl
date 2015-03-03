/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#include "glwidget.h"
#include <cmath>
#include <QKeyEvent>
#include <QTimer>

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 13");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    cnt1 = cnt2 = 0.0f;
}

// Empty destructor
GLWidget::~GLWidget() {}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up depth testing
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
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

    glLoadIdentity(); // Reset view matrix
    glTranslatef(0.0f, 0.0f, -1.0f); // Move into screen 1 unit

    // Pulsing colors based on text position
    glColor3f(qMax(0.0, 1.0f * cos(cnt1)), qMax(0.0, 1.0 * sin(cnt2)), qMin(1.0, 1.0f - 0.5f * cos(cnt1 + cnt2)));

    /* As you can see, drawing untransformed text in OpenGL in Qt is VERY simple
     * compared to the "normal" platform-independant ways of doing it! */

    // Print text on the screen
    QFont font;
    font.setBold(true);
    font.setPixelSize(18);
    renderText(-0.45 + 0.05 * cos(cnt1), 0.35 * sin(cnt2), 0.0,
               "Active OpenGL Text With NeHe - " + QString::number(cnt1, 'f', 2), font);

    // Print some text without taking the transformation into account
    QFontMetrics metrics(font); // Very useful class to align some text in OpenGL at the absolute borders of the screen!
    QString bottomRightText("I am always at the bottom right.");
    renderText(width() - metrics.width(bottomRightText) - 10, height() - metrics.height() + 10, bottomRightText, font);

    // Increase counters
    cnt1 += 0.102f;
    cnt2 += 0.010f;

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
