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
    setWindowTitle("NeHe's OpenGL Lesson 20");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    roll = 0.0f;
    masking = true;
    secondScene = false;

    changeAmount = 0;
    maskChanged = sceneChanged = false;
}

// Destructor
GLWidget::~GLWidget() {
    for (int i = 0; i < 5; ++i)
        deleteTexture(texture[i]);
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Load textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture[0] = bindTexture(QImage(":/img/Logo.bmp"));
    texture[1] = bindTexture(QImage(":/img/Mask1.bmp"));
    texture[2] = bindTexture(QImage(":/img/Image1.bmp"));
    texture[3] = bindTexture(QImage(":/img/Mask2.bmp"));
    texture[4] = bindTexture(QImage(":/img/Image2.bmp"));
    glEnable(GL_TEXTURE_2D); // Enable texture mapping
}

// This is called when the OpenGL window is resized
void GLWidget::resizeGL(int width, int height) {
    // Prevent divide by zero (in the gluPerspective call)
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height); // Reset current viewport

    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Reset projection matrix

    gluPerspective(45.0f, static_cast<GLfloat>(width)/height, 0.1f, 200.0f); // Set up perspective

    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity(); // Reset modelview matrix
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f); // Move into the screen 2 units

    glBindTexture(GL_TEXTURE_2D, texture[0]); // Select Our Logo Texture
    glBegin(GL_QUADS); // Draw textured quad
        glTexCoord2f(0.0f, -roll); glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom left
        glTexCoord2f(3.0f, -roll); glVertex3f(1.1f, -1.1f, 0.0f); // Bottom right
        glTexCoord2f(3.0f, 3.0f - roll); glVertex3f(1.1f, 1.1f, 0.0f); // Top right
        glTexCoord2f(0.0f, 3.0f - roll); glVertex3f(-1.1f, 1.1f, 0.0f); // Top left
    glEnd();

    glEnable(GL_BLEND); // Enable blending
    glDisable(GL_DEPTH_TEST); // Disable depth testing

    if (secondScene) {
        glTranslatef(0.0f, 0.0f, -1.0f);
        glRotatef(roll * 360.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < (masking ? 2 : 1); ++i) {
            if (masking && i == 0) {
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
                glBindTexture(GL_TEXTURE_2D, texture[3]); // Second mask texture
            } else {
                glBlendFunc(GL_ONE, GL_ONE);
                glBindTexture(GL_TEXTURE_2D, texture[4]); // Second image texture
            }
            glBegin(GL_QUADS); // Draw textured quad
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom left
                glTexCoord2f(1.0f, 0.0f); glVertex3f(1.1f, -1.1f, 0.0f); // Bottom right
                glTexCoord2f(1.0f, 1.0f); glVertex3f(1.1f, 1.1f, 0.0f); // Top right
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.1f, 1.1f, 0.0f); // Top left
            glEnd();
        }
    } else {
        for (int i = 0; i < (masking ? 2 : 1); ++i) {
            if (masking && i == 0) {
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
                glBindTexture(GL_TEXTURE_2D, texture[1]); // First mask texture
            } else {
                glBlendFunc(GL_ONE, GL_ONE);
                glBindTexture(GL_TEXTURE_2D, texture[2]); // First image texture
            }
            glBegin(GL_QUADS); // Draw textured quad
                glTexCoord2f(roll, 0.0f); glVertex3f(-1.1f, -1.1f, 0.0f); // Bottom left
                glTexCoord2f(roll + 4.0f, 0.0f); glVertex3f(1.1f, -1.1f, 0.0f); // Bottom right
                glTexCoord2f(roll + 4.0f, 4.0f); glVertex3f(1.1f, 1.1f, 0.0f); // Top right
                glTexCoord2f(roll, 4.0f); glVertex3f(-1.1f, 1.1f, 0.0f); // Top left
            glEnd();
        }
    }

    glDisable(GL_BLEND); // Disable blending
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Update roll variable
    roll += 0.005f;
    if (roll > 1.0f)
        roll -= 1.0f;

    // Show message when an enabled OpenGL feature has changed
    glLoadIdentity();
    if ((maskChanged || sceneChanged) && changeAmount > 0) {
        QString str;
        if (maskChanged)
            str = QString("Mask has been ") + (masking ? "enabled" : "disabled");
        else // sceneChanged
            str = QString("Scene has changed to ") + (secondScene ? "second scene" : "first scene");
        QFont font;
        QFontMetrics metrics(font);
        font.setPixelSize(20);
        font.setBold(true);
        if (changeAmount < 100)
            glColor4f(0.0, 0.0, 0.0, static_cast<GLfloat>(changeAmount) / 100);
        else
            glColor3f(0.0, 0.0, 0.0);
        this->renderText(11, 11 + metrics.height(), str, font);
        if (changeAmount < 100)
            glColor4f(1.0, 1.0, 1.0, static_cast<GLfloat>(changeAmount) / 100);
        else
            glColor3f(1.0, 1.0, 1.0);
        this->renderText(10, 10 + metrics.height(), str, font);
        changeAmount--;
    }

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
    case Qt::Key_Space:
        secondScene = !secondScene;
        maskChanged = false;
        sceneChanged = true;
        changeAmount = 140;
        break;
    case Qt::Key_M:
        masking = !masking;
        sceneChanged = false;
        maskChanged = true;
        changeAmount = 140;
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
