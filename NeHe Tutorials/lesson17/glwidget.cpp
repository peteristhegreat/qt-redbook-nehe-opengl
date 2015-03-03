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
    setWindowTitle("NeHe's OpenGL Lesson 17");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    base = 0;
    cnt1 = cnt2 = 0.0f;
    aspect = 1.0f;
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteLists(base, 256);
    glDeleteTextures(2, texture);
}

void GLWidget::glPrint(int x, int y, const char *text, int italicSet) {
    if (italicSet > 1)
        italicSet = 1;

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glDisable(GL_DEPTH_TEST); // Temporarily disable depth testing

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Save projection matrix
    glLoadIdentity(); // Reset projection matrix
    glOrtho(0, 640, 0, 480, -1, 1); // Set up ortho screen

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Save modelview matrix
    glLoadIdentity(); // Reset modelview matrix
    glTranslated(x, y, 0); // Position the text

    glListBase(base - 32 + 128 * italicSet); // Select font set
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text); // Draw text using display lists

    // Restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
}

// Build our font and save characters in display lists
void GLWidget::buildFont() {
    base = glGenLists(256); // Generate 256 display lists

    // Story every character in a seperate display list
    float cx, cy;
    for (int i = 0; i < 256; ++i) {
        cx = (i % 16) / 16.0f; // X position of current character
        cy = (i / 16) / 16.0f; // Y position of current character
        glNewList(base + i, GL_COMPILE); // Compile a new list
            glBegin(GL_QUADS);
                glTexCoord2f(cx, 1 - cy - 0.0625f);
                glVertex2i(0, 0);
                glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);
                glVertex2i(16, 0);
                glTexCoord2f(cx + 0.0625f, 1 - cy);
                glVertex2i(16, 16);
                glTexCoord2f(cx, 1 - cy);
                glVertex2i(0, 16);
            glEnd();
            glTranslated(10, 0, 0); // Take care of spacing between characters
        glEndList();
    }
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

    // Set up blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set up textures
    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, texture);
    QImage img[2];
    img[0] = convertToGLFormat(QImage(":/img/Bumps.bmp"));
    img[1] = convertToGLFormat(QImage(":/img/Font.bmp"));

    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, img[i].width(), img[i].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img[i].bits());
    }

    buildFont();
}

// This is called when the OpenGL window is resized
void GLWidget::resizeGL(int width, int height) {
    // Prevent divide by zero (in the gluPerspective call)
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height); // Reset current viewport

    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Reset projection matrix

    aspect = static_cast<GLfloat>(width)/height;
    gluPerspective(45.0f, aspect, 0.1f, 100.0f); // Calculate aspect ratio

    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity(); // Reset modelview matrix
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer
    glLoadIdentity(); // Reset view matrix

    // Draw diamond-like thingy
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(30.0f * cnt1, 1.0f, 1.0f, 0.0f);
    glDisable(GL_BLEND);
    qglColor(Qt::white);
    glBegin(GL_QUADS);
        glTexCoord2d(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);
        glTexCoord2d(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2d(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2d(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();
    glRotatef(90.0f, 1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2d(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);
        glTexCoord2d(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2d(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2d(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();
    glEnable(GL_BLEND);

    // Draw text
    glColor3f(1.0f * cos(cnt1), 1.0f * sin(cnt2), 1.0f - 0.5f * cos(cnt1 + cnt2));
    glPrint(int(280 + 250 * cos(cnt1)), int(235 + 200 * sin(cnt2)), "NeHe", 0);
    glColor3f(1.0f * sin(cnt2), 1.0f - 0.5f * cos(cnt1 + cnt2), 1.0f * cos(cnt1));
    glPrint(int(280 + 230 * cos(cnt2)), int(235 + 200 * sin(cnt1)), "OpenGL", 1);
    glColor3f(0.0f, 0.0f, 1.0f);
    glPrint(int(240 + 200 * cos((cnt2 + cnt1) / 5)), 2, "Giuseppe D'Agata", 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPrint(int(242 + 200 * cos((cnt2 + cnt1) / 5)), 2, "Giuseppe D'Agata", 0);

    // Update count values
    cnt1 += 0.04f;
    cnt2 += 0.032f;

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
