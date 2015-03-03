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

float GLWidget::colors[12][3] = { // 12 different colors
    {1.0f, 0.5f, 0.5f}, {1.0f, 0.75f, 0.5f}, {1.0f, 1.0f, 0.5f}, {0.75f, 1.0f, 0.5f},
    {0.5f, 1.0f, 0.5f}, {0.5f, 1.0f, 0.75f}, {0.5f, 1.0f, 1.0f}, {0.5f, 0.75f, 1.0f},
    {0.5f, 0.5f, 1.0f}, {0.75f, 0.5f, 1.0f}, {1.0f, 0.5f, 1.0f}, {1.0f, 0.5f, 0.75f}
};

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("NeHe's OpenGL Lesson 19");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    colorChanged = rainbowChanged = false;
    changeAmount = 0;
    zoom = -40.0f;
    slowdown = 0.5f;
    rainbow = true;
    randomColors = false;
    col = 0;
    keepColorDuration = 200;
    keepColorCount = 0;
    xspeed = yspeed = 0.0f;

    colorName[0] = "first color";
    colorName[1] = "second color";
    colorName[2] = "third color";
    colorName[3] = "fourth color";
    colorName[4] = "fifth color";
    colorName[5] = "sixth color";
    colorName[6] = "seventh color";
    colorName[7] = "eighth color";
    colorName[8] = "ninth color";
    colorName[9] = "tenth color";
    colorName[10] = "eleventh color";
    colorName[11] = "twelfth color";
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
    glDisable(GL_DEPTH_TEST); // Disable depth testing

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    // Set up texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture = bindTexture(QPixmap(":/img/Particle.bmp"));
    glEnable(GL_TEXTURE_2D);

    // Set up blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    // Initialize particles
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        particle[i].life = 1.0f;
        particle[i].fade = (qrand() % 100) / 1000.0f + 0.003f;
        particle[i].r = colors[i / (MAX_PARTICLES / 12)][0];
        particle[i].g = colors[i / (MAX_PARTICLES / 12)][1];
        particle[i].b = colors[i / (MAX_PARTICLES / 12)][2];
        particle[i].xi = (qrand() % 50 - 25.0f) * 10.0f;
        particle[i].yi = (qrand() % 50 - 25.0f) * 10.0f;
        particle[i].zi = (qrand() % 50 - 25.0f) * 10.0f;
        particle[i].xg = 0.0f;
        particle[i].yg = -1.5f;
        particle[i].zg = 0.0f;
        particle[i].x = 0.0f;
        particle[i].y = 0.0f;
        particle[i].z = 0.0f;
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

    gluPerspective(45.0f, static_cast<GLfloat>(width)/height, 0.1f, 200.0f); // Set up perspective

    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity(); // Reset modelview matrix
}

// OpenGL painting code goes here
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    // Draw particles
    float x, y, z;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        x = particle[i].x;
        y = particle[i].y;
        z = particle[i].z + zoom;
        glColor4f(particle[i].r, particle[i].g, particle[i].b, particle[i].life);

        // Use triangle strip to draw particle
        glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
            glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
            glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
            glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
        glEnd();

        // Update particle position
        particle[i].x += particle[i].xi / (slowdown * 1000.0f);
        particle[i].y += particle[i].yi / (slowdown * 1000.0f);
        particle[i].z += particle[i].zi / (slowdown * 1000.0f);

        // Take gravity into account
        particle[i].xi += particle[i].xg;
        particle[i].yi += particle[i].yg;
        particle[i].zi += particle[i].zg;

        // Decrease particle life
        particle[i].life -= particle[i].fade;

        // Reincarnate particle if it has died . . .
        if (particle[i].life <= 0.0f) {
            if (rainbow) {
                ++keepColorCount;
                if (keepColorCount >= keepColorDuration) {
                    keepColorCount = 0;
                    col = (col + 1) % 12;
                }
            } else if (randomColors)
                col = qrand() % 12;

            particle[i].life = 1.0f;
            particle[i].fade = (qrand() % 100) / 1000.0f + 0.003f;
            particle[i].x = 0.0f;
            particle[i].y = 0.0f;
            particle[i].z = 0.0f;
            particle[i].xi = xspeed + qrand() % 40 - 20.0f;
            particle[i].yi = yspeed + qrand() % 40 - 20.0f;
            particle[i].zi = qrand() % 40 - 20.0f;
            particle[i].r = colors[col][0];
            particle[i].g = colors[col][1];
            particle[i].b = colors[col][2];
        }

        // Update gravity
        if (activeKeys.contains(Qt::Key_8) && (particle[i].yg < 1.5f))
            particle[i].yg += 0.01f;
        if (activeKeys.contains(Qt::Key_2) && (particle[i].yg > -1.5f))
            particle[i].yg -= 0.01f;
        if (activeKeys.contains(Qt::Key_6) && (particle[i].xg < 1.5f))
            particle[i].xg += 0.01f;
        if (activeKeys.contains(Qt::Key_4) && (particle[i].xg > -1.5f))
            particle[i].xg -= 0.01f;

        // Simulate an explosion if wanted
        if (activeKeys.contains(Qt::Key_Tab)) {
            particle[i].xi = (qrand() % 50 - 25.0f) * 10.0f;
            particle[i].yi = (qrand() % 50 - 25.0f) * 10.0f;
            particle[i].zi = (qrand() % 50 - 25.0f) * 10.0f;
            particle[i].x = 0.0f;
            particle[i].y = 0.0f;
            particle[i].z = 0.0f;
        }
    }
    if (activeKeys.contains(Qt::Key_Tab)) // Make the explosion effect a bit nicer when the key is held too long
        activeKeys.removeAll(Qt::Key_Tab);

    // Show message when an enabled OpenGL feature has changed
    glLoadIdentity();
    if ((colorChanged || rainbowChanged) && changeAmount > 0) {
        QString str;
        if (colorChanged && randomColors)
            str = QString("Color cycling has been disabled.");
        else if (colorChanged && !randomColors)
            str = QString("Color has cycled to ") + colorName[col];
        else if (rainbowChanged)
            str = QString("Rainbow effect has been ") + (rainbow ? "enabled" : "disabled");
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
    case Qt::Key_Plus:
        if (slowdown > 0.1f)
            slowdown -= 0.01f;
        break;
    case Qt::Key_Minus:
        if (slowdown < 2.0f)
            slowdown += 0.01f;
        break;
    case Qt::Key_PageUp:
        zoom += 0.5f;
        break;
    case Qt::Key_PageDown:
        zoom -= 0.5f;
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        rainbow = !rainbow;
        randomColors = true;
        changeAmount = 140;
        rainbowChanged = true;
        colorChanged = false;
        break;
    case Qt::Key_Space:
        if (rainbow || randomColors) { // Initialize
            col = 0;
            rainbow = false;
            randomColors = false;
        } else // Change color
            ++col;

        if (col == 12) {
            randomColors = true;
            col = 0;
        } else
            randomColors = false;
        changeAmount = 140;
        colorChanged = true;
        rainbowChanged = false;
        break;
    case Qt::Key_Up:
        if (yspeed < 300.0f)
            yspeed += 1.0f;
        break;
    case Qt::Key_Down:
        if (yspeed > -300.0f)
            yspeed -= 1.0f;
        break;
    case Qt::Key_Left:
        if (xspeed < 300.0f)
            xspeed += 1.0f;
        break;
    case Qt::Key_Right:
        if (xspeed > -300.0f)
            xspeed -= 1.0f;
        break;
    default:
        QGLWidget::keyPressEvent(event); // Let base class handle the other keys
    }
    // Modify list of active keys
    if (!activeKeys.contains(event->key()))
        activeKeys.append(event->key());
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    // Modify list of active keys
    activeKeys.removeAll(event->key());
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
