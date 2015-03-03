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
    setWindowTitle("NeHe's OpenGL Lesson 9");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    zoom = -15.0f;
    tilt = 90.0f;
    spin = 0.0f;
    twinkle = false;
    twinkleChanged = false;
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteTextures(1, &texture);
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glClearDepth(1.0f); // Depth buffer setup
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture = bindTexture(QPixmap(":/img/Star.bmp"));
    glEnable(GL_TEXTURE_2D);

    // Set up blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // Set up stars
    for (int i = 0; i < NUM_OF_STARS; ++i) {
        star[i].angle = 0.0f;
        star[i].dist = (static_cast<GLfloat>(i)/NUM_OF_STARS) * 5.0f;
        star[i].r = qrand() % 256;
        star[i].g = qrand() % 256;
        star[i].b = qrand() % 256;
    }
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

    for (int i = 0; i < NUM_OF_STARS; ++i) {
        glLoadIdentity(); // Reset view
        glTranslatef(0.0f, 0.0f, zoom); // Zoom into screen
        glRotatef(tilt, 1.0f, 0.0f, 0.0f); // Tilt view
        glRotatef(star[i].angle, 0.0f, 1.0f, 0.0f); // Rotate to star angle
        glTranslatef(star[i].dist, 0.0f, 0.0f); // Move forward on the X plane
        glRotatef(-star[i].angle, 0.0, 1.0f, 0.0f); // Cancel rotation
        glRotatef(-tilt, 1.0f, 0.0f, 0.0f); // Cancel tilt

        if (star[i].a < 255)
            star[i].a += 3;

        if (twinkle) { // If twinkling stars are enabled
            glColor4ub(qrand() % 256, qrand() % 256, qrand() % 256, star[i].a);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
            glEnd();
        }

        glRotatef(spin, 0.0f, 0.0f, 1.0f); // Rotate star on Z-axis
        glColor4ub(star[i].r, star[i].g, star[i].b, star[i].a);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();

        spin += 0.01f;
        star[i].angle += static_cast<GLfloat>(i) / NUM_OF_STARS;
        star[i].dist -= 0.02f;

        if (star[i].dist < 0.0f) { // Star reached the center
            star[i].dist += 5.0f;
            star[i].r = qrand() % 256;
            star[i].g = qrand() % 256;
            star[i].b = qrand() % 256;
            star[i].a = 0;
        }
    }

    // Show message when twinkle feature has changed
    glLoadIdentity();
    if (twinkleChanged) {
        QString str = QString("Twinkling stars are now ") + (twinkle ? "enabled" : "disabled");
        QFont font;
        QFontMetrics metrics(font);
        font.setPixelSize(20);
        font.setBold(true);
        if (twinkleChanged < 100)
            glColor4f(1.0, 1.0, 1.0, static_cast<GLfloat>(twinkleChanged) / 100);
        else
            glColor3f(1.0, 1.0, 1.0);
        this->renderText(10, 10 + metrics.height(), str, font);
        twinkleChanged--;
    }

    // Framerate control
    int delay = time.msecsTo(QTime::currentTime());
    if (delay == 0)
        delay = 1;
    time = QTime::currentTime();
    timer->start(qMax(0, 10 - delay));
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
    case Qt::Key_T:
        twinkle = !twinkle;
        twinkleChanged = 200;
        break;
    case Qt::Key_PageUp:
        zoom -= 0.2f;
        break;
    case Qt::Key_PageDown:
        zoom += 0.2f;
        break;
    case Qt::Key_Up:
        tilt -= 0.5f;
        break;
    case Qt::Key_Down:
        tilt += 0.5f;
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
