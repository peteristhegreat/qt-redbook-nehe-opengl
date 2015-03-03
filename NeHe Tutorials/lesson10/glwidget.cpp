/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#define PI_OVER_180 0.0174532925f

#include "glwidget.h"
#include <cmath>
#include <QKeyEvent>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 10");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    sector1.triangle = NULL;
    xpos = yrot = zpos = 0;
    filter = 2;
    walkbias = walkbiasangle = lookupdown = 0.0f;
    lightChanged = filterChanged = blendChanged = false;
    changeAmount = 0;
    filterName[0] = "nearest filter";
    filterName[1] = "linear filter";
    filterName[2] = "mipmaps";
    keyUp = keyDown = keyLeft = keyRight = keyPageUp = keyPageDown = false;
}

// Destructor
GLWidget::~GLWidget() {
    if (sector1.triangle)
        delete[] sector1.triangle;
    glDeleteTextures(3, texture);
}

// Initialize all data (return true on success)
bool GLWidget::init() {
    if (!setupWorld()) {
        qDebug("Could not load world from resource file. Exiting.");
        return false;
    }
    return true;
}

QString GLWidget::getLine(QTextStream &in) {
    QString line;
    do {
        line = in.readLine().trimmed();
    } while (!line.isNull() && (line.startsWith("//") || line.isEmpty()));
    return line;
}

// Setup our world (return true on success)
bool GLWidget::setupWorld() {
    QFile world(":/data/World.txt");
    if (!world.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;

    QTextStream in(&world);
    QStringList lineParts;
    QString line;
    line = getLine(in);
    if (!line.isEmpty()) {
        lineParts = line.split(QRegExp("\\s+"));
        if (lineParts.size() >= 2 && lineParts[0] == "NUMPOLLIES") {
            sector1.numTriangles = lineParts[1].toInt();
            sector1.triangle = new Triangle[sector1.numTriangles];
        } else {
            qDebug("setupWorld(): first line must be polygon count.");
            return false;
        }
        int triLoop = 0, verLoop = 0;
        while (!(line = getLine(in)).isEmpty() && triLoop < sector1.numTriangles) {
            lineParts = line.split(QRegExp("\\s+"));
            if (lineParts.size() >= 5 && triLoop < sector1.numTriangles) {
                sector1.triangle[triLoop].vertex[verLoop].x = lineParts[0].toFloat();
                sector1.triangle[triLoop].vertex[verLoop].y = lineParts[1].toFloat();
                sector1.triangle[triLoop].vertex[verLoop].z = lineParts[2].toFloat();
                sector1.triangle[triLoop].vertex[verLoop].u = lineParts[3].toFloat();
                sector1.triangle[triLoop].vertex[verLoop].v = lineParts[4].toFloat();
                ++verLoop;
                if (verLoop == 3) {
                    verLoop = 0;
                    ++triLoop;
                }
            }
        }
    } else {
        qDebug("setupWorld(): resource file seems to be empty...");
        return false;
    }
    return true;
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up textures
    // Note: Because we need different texture filters, we cannot use Qt's bindTexture()
    //       Qt automatically generates the most appropriate textures using available features in your setup
    //       which means that Qt always generates mipmaps if your system supports them
    //       We will use normal glBindTexture() etc. functions instead

    glEnable(GL_TEXTURE_2D);
    QImage img = convertToGLFormat(QImage(":/img/Mud.bmp"));
    glGenTextures(3, texture);

    // Texture using nearest filter
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    // Texture using linear filter
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    // Texture using mipmaps
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    // Set up lighting
    GLfloat ambLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat diffLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPos[] = {0.0f, 0.0f, 2.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffLight);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    // Set up blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set up depth testing
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);
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
    movePlayer();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    // Player translation
    GLfloat xtrans = -xpos;
    GLfloat ytrans = -walkbias - 0.25f;
    GLfloat ztrans = -zpos;
    GLfloat sceneroty = 360.0f - yrot; // 360° angle for player direction

    glLoadIdentity();
    glRotatef(lookupdown, 1.0f, 0.0f, 0.0f);
    glRotatef(sceneroty, 0.0f, 1.0f, 0.0f); // Rotate to direction player is watching
    glTranslatef(xtrans, ytrans, ztrans); // Translate scene based on player position

    // Draw all triangles
    glBindTexture(GL_TEXTURE_2D, texture[filter]);
    qglColor(Qt::white);
    for (int i = 0; i < sector1.numTriangles; ++i) {
        glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            for (int j = 0; j < 3; ++j) {
                glTexCoord2f(sector1.triangle[i].vertex[j].u, sector1.triangle[i].vertex[j].v);
                glVertex3f(sector1.triangle[i].vertex[j].x, sector1.triangle[i].vertex[j].y, sector1.triangle[i].vertex[j].z);
            }
        glEnd();
    }

    // Show message when an enabled OpenGL feature has changed
    glLoadIdentity();
    if ((lightChanged || filterChanged || blendChanged) && changeAmount > 0) {
        QString str;
        if (filterChanged)
            str = QString("Now filtering using ") + filterName[filter];
        else if (lightChanged)
            str = QString("Lighting is now ") + (glIsEnabled(GL_LIGHTING) ? "on" : "off");
        else if (blendChanged)
            str = QString("Blending is now ") + (glIsEnabled(GL_BLEND) ? "on" : "off");
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

// We don't really need this, but using this way, we bypass the normal autorepeat functionality, which is better for a "game"
void GLWidget::movePlayer() {
    if (keyUp) {
        xpos -= sin(yrot * PI_OVER_180) * 0.03f;
        zpos -= cos(yrot * PI_OVER_180) * 0.03f;
        if (walkbiasangle >= 353.0f)
            walkbiasangle = 0.0f;
        else
            walkbiasangle += 7.0f;
        walkbias = sin(walkbiasangle * PI_OVER_180) / 20.0f;
    } else if (keyDown) {
        xpos += sin(yrot * PI_OVER_180) * 0.03f;
        zpos += cos(yrot * PI_OVER_180) * 0.03f;
        if (walkbiasangle <= 7.0f)
            walkbiasangle = 353.0f;
        else
            walkbiasangle -= 7.0f;
        walkbias = sin(walkbiasangle * PI_OVER_180) / 20.0f;
    }

    if (keyLeft)
        yrot += 1.5f;
    else if (keyRight)
        yrot -= 1.5f;

    if (keyPageUp)
        lookupdown -= 0.5;
    else if (keyPageDown)
        lookupdown += 0.5;
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
    case Qt::Key_L:
        if (glIsEnabled(GL_LIGHTING))
            glDisable(GL_LIGHTING);
        else
            glEnable(GL_LIGHTING);
        changeAmount = 140;
        filterChanged = false;
        blendChanged = false;
        lightChanged = true;
        break;
    case Qt::Key_F:
        ++filter;
        if (filter > 2)
            filter = 0;
        changeAmount = 140;
        lightChanged = false;
        blendChanged = false;
        filterChanged = true;
        break;
    case Qt::Key_B:
        if (glIsEnabled(GL_BLEND)) {
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
        }
        changeAmount = 140;
        lightChanged = false;
        filterChanged = false;
        blendChanged = true;
        break;
    case Qt::Key_PageUp:
        keyPageUp = true;
        break;
    case Qt::Key_PageDown:
        keyPageDown = true;
        break;
    case Qt::Key_Left:
        keyLeft = true;
        break;
    case Qt::Key_Right:
        keyRight = true;
        break;
    case Qt::Key_Up:
        keyUp = true;
        break;
    case Qt::Key_Down:
        keyDown = true;
        break;
    default:
        QGLWidget::keyPressEvent(event); // Let base class handle the other keys
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_PageUp:
        keyPageUp = false;
        break;
    case Qt::Key_PageDown:
        keyPageDown = false;
        break;
    case Qt::Key_Left:
        keyLeft = false;
        break;
    case Qt::Key_Right:
        keyRight = false;
        break;
    case Qt::Key_Up:
        keyUp = false;
        break;
    case Qt::Key_Down:
        keyDown = false;
        break;
    default:
        QGLWidget::keyReleaseEvent(event); // Let base class handle the other keys
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
