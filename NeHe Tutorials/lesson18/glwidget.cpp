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
    setWindowTitle("NeHe's OpenGL Lesson 18");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    filter = 2;
    lightChanged = filterChanged = objChanged = false;
    changeAmount = 0;
    filterName[0] = "nearest filter";
    filterName[1] = "linear filter";
    filterName[2] = "mipmaps";
    objName[0] = "cube";
    objName[1] = "cylinder";
    objName[2] = "disk";
    objName[3] = "sphere";
    objName[4] = "cone";
    objName[5] = "animated disk";
    xrot = yrot = 0.0f;
    xspeed = yspeed = 0.5f;
    z = -5.0f;
    quadricObj = NULL;
    object = 0;
    part1 = part2 = p1 = 0;
    p2 = 1;
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteTextures(3, texture);
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background

    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL); // Set type of depth test

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up textures
    // Note: Because we need different texture filters, we cannot use Qt's bindTexture()
    //       Qt automatically generates the most appropriate textures using available features in your setup
    //       which means that Qt always generates mipmaps if your system supports them
    //       We will use normal glBindTexture() etc. functions instead

    glEnable(GL_TEXTURE_2D);
    QImage img = convertToGLFormat(QImage(":/img/Wall.bmp"));
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
    GLfloat ambLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPos[] = {0.0f, 0.0f, 2.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffLight);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    // Set up quadric object
    quadricObj = gluNewQuadric();
    gluQuadricNormals(quadricObj, GLU_SMOOTH);
    gluQuadricTexture(quadricObj, GL_TRUE);
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

// Draw a cube using OpenGL
void GLWidget::drawCube() {
    glBegin(GL_QUADS);
        // Front Face
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 1.0f); // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Top Left Of The Texture and Quad
        // Back Face
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f); // Top Left Of The Texture and Quad
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad
        // Top Face
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Texture and Quad
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f); // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f); // Top Right Of The Texture and Quad
        // Bottom Face
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Top Left Of The Texture and Quad
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Right Of The Texture and Quad
        // Right face
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f); // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 1.0f); // Top Left Of The Texture and Quad
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Bottom Left Of The Texture and Quad
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom Right Of The Texture and Quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Top Right Of The Texture and Quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Top Left Of The Texture and Quad
    glEnd();
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    glLoadIdentity(); // Reset current modelview matrix
    glTranslatef(0.0f, 0.0f, z); // Move into the screen
    glRotatef(xrot, 1.0f, 0.0f, 0.0f); // Rotate on X-axis
    glRotatef(yrot, 0.0f, 1.0f, 0.0f); // Rotate on Y-axis
    glBindTexture(GL_TEXTURE_2D, texture[filter]);

    switch (object) {
    case 0:
        drawCube();
        break;
    case 1:
        glTranslatef(0.0f, 0.0f, -1.5f);
        gluCylinder(quadricObj, 1.0f, 1.0f, 3.0f, 32, 32);
        break;
    case 2:
        gluDisk(quadricObj, 0.5f, 1.5f, 32, 32);
        break;
    case 3:
        gluSphere(quadricObj, 1.3f, 32, 32);
        break;
    case 4:
        glTranslatef(0.0f, 0.0f, -1.5f);
        gluCylinder(quadricObj, 1.0f, 0.0f, 3.0f, 32, 32);
        break;
    case 5:
        part1 += p1; // Increase start angle
        part2 += p2; // Increase sweep angle
        if (part1 == 360) {
            p1 = 0;
            part1 = 0;
            p2 = 1;
            part2 = 0;
        } else if (part2 == 360) {
            p1 = 1;
            p2 = 0;
        }
        gluPartialDisk(quadricObj, 0.5f, 1.5f, 32, 32, part1, part2 - part1);
        break;
    }

    xrot += xspeed; // X-axis rotation
    yrot += yspeed; // Y-axis rotation

    // Show message when an enabled OpenGL feature has changed
    glLoadIdentity();
    if ((lightChanged || filterChanged || objChanged) && changeAmount > 0) {
        QString str;
        if (filterChanged)
            str = QString("Now filtering using ") + filterName[filter];
        else if (objChanged)
            str = QString("Object changed to ") + objName[object];
        else // if (lightChanged)
            str = QString("Lighting is now ") + (glIsEnabled(GL_LIGHTING) ? "on" : "off");
        QFont font;
        QFontMetrics metrics(font);
        font.setPixelSize(20);
        font.setBold(true);
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
    case Qt::Key_L:
        if (glIsEnabled(GL_LIGHTING))
            glDisable(GL_LIGHTING);
        else
            glEnable(GL_LIGHTING);
        filterChanged = false;
        objChanged = false;
        lightChanged = true;
        changeAmount = 140;
        break;
    case Qt::Key_Space:
        ++object;
        if (object > 5)
            object = 0;
        filterChanged = false;
        lightChanged = false;
        objChanged = true;
        changeAmount = 140;
        break;
    case Qt::Key_F:
        ++filter;
        if (filter > 2)
            filter = 0;
        lightChanged = false;
        objChanged = false;
        filterChanged = true;
        changeAmount = 140;
        break;
    case Qt::Key_PageUp:
        z -= 0.05f;
        break;
    case Qt::Key_PageDown:
        z += 0.05f;
        break;
    case Qt::Key_Up:
        xspeed -= 0.02f;
        break;
    case Qt::Key_Down:
        xspeed += 0.02f;
        break;
    case Qt::Key_Left:
        yspeed -= 0.02f;
        break;
    case Qt::Key_Right:
        yspeed += 0.02f;
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
