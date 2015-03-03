/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#include "glwidget.h"
#include <cmath>
#include <FTGL/ftgl.h>
#include <QKeyEvent>
#include <QTimer>

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 14");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    font = NULL;
    rot = 0.0f;
}

// Destructor
GLWidget::~GLWidget() {
    delete font;
}

// Set up the FTGL library for 3D text rendering
bool GLWidget::initFTGL() {
    font = new FTExtrudeFont("DejaVuSans-Bold.ttf");
    if (font->Error()) {
        qDebug("initFTGL(): Could not load font.");
        return false;
    }
    font->FaceSize(2);
    font->Depth(0.75f);
    return true;
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up depth testing
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    // Set up lighting
    glEnable(GL_LIGHT0); // Assumes that GL_LIGHT0 is predefined
    glEnable(GL_LIGHTING); // Lighting
    glEnable(GL_COLOR_MATERIAL); // Material coloring
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

    glTranslatef(0.0f, 0.0f, -20.0f);

    // Pulsate colors
    glColor3f(1.0f * cos(rot / 20.0f), 1.0f * sin(rot / 25.0f), 1.0f - 0.5f * cos(rot / 17.0f));

    /* Qt does not support rendering 3D fonts by default.
       We use FTGL as our cross-platform library for this particular funtionality. */

    QByteArray cStr = (QString("NeHe - ") + QString::number(rot / 1000.0f, 'f', 2)).toLatin1();
    FTBBox rect = font->BBox(cStr.data());
    FTPoint p = rect.Upper();
    glRotatef(rot, 1.0f, 0.0f, 0.0f);
    glRotatef(rot * 1.5f, 0.0f, 1.0f, 0.0f);
    glRotatef(rot * 1.4f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-p.Xf() / 2.0f, 0.0f, 0.0f);
    font->Render(cStr.data());

    rot += 0.5f;

    // Framerate control
    int delay = time.msecsTo(QTime::currentTime());
    if (delay == 0)
        delay = 1;
    time = QTime::currentTime();
    timer->start(qMax(0, 30 - delay));
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
