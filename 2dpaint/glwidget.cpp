// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "glwidget.hpp"

#include <QMouseEvent>
#include <QRubberBand>

float fov = 60.0;
QRubberBand *rubberBand = nullptr;


GLWidget::GLWidget(const layout::dLayoutManager &_layM, QWidget *parent)
    : QOpenGLWidget(parent)
    , layM(_layM)
    , background(QColor(0, 0, 0, 0))
    , zoomFactor(1.0)
    , mouseMode(Move)
    , pannedDist(0, 0)
    , panDiff(0, 0) {
    setMinimumSize(400, 200);
    viewBBox.makeInvalid();
    //setAutoFillBackground(false);
}
//! [0]

GLWidget::~GLWidget() {
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}

const layout::dBox& GLWidget::getViewBox() {
    const layout::dLayout* activeLayout = getLayoutManager().getActiveLayout();
    if (activeLayout) {
        viewBBox = activeLayout->getBBox();
        double w = width();
        w = (w) ?w :1.0;
        double h = height();
        // adjust viewBBox's width according to window's aspece ratio
        double vw = viewBBox.getHeight()*w/h;
        layout::dPoint center = viewBBox.center();
        viewBBox.setMinX(center.x()-vw/2);
        viewBBox.setMaxX(center.x()+vw/2);
        // add some margin.
        viewBBox *= 1.01; // * getZoomFactor());
    } else {
        viewBBox.makeInvalid();
    }
    return viewBBox;
}

#if 0
void GLWidget::paintLayout(QPainter *painter, QPaintEvent *event)
{
    // paint the background
    painter->fillRect(event->rect(), getBackground());
    getLayoutManager()->draw(painter);
}

//! [1]
void GLWidget::paintEvent(QPaintEvent *event)
{
#if 0
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paintLayout(&painter, event);
    painter.end();
#else
    QPainter paint(this);
    paint.beginNativePainting();
#endif

}
#else
void GLWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPos = e->position();
    mousePressModel = model;
    if (getMouseMode() == Zoom) {
        if (!rubberBand) {
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        }
        rubberBand->setGeometry(
            QRect(QPoint(round(mousePressPos.x()), round(mousePressPos.y())),
            QSize()));
        rubberBand->show();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e) {
    // Mouse move position - mouse press position
    MouseModes mode = getMouseMode();
    QPointF pos = e->position();
    const layout::dBox& vbox = getViewBox();
    if (mode == Zoom) {
        rubberBand->resize(pos.x()-mousePressPos.x(), pos.y()-mousePressPos.y());
    } else if (mode == Pan) {
        model = mousePressModel;
        QRectF mapped = model.mapRect(QRectF(vbox.getMinX(), vbox.getMinY(),
                                             vbox.getWidth(), vbox.getHeight()));
        double scale = mapped.width() / vbox.getWidth();
        QVector2D diff = QVector2D(e->position()-mousePressPos);
        model.translate(vbox.getWidth()*diff.x()/(scale*width()),
                        -vbox.getHeight()*diff.y()/(scale*height()),
                        0.0);
        update();
    }
    return;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    MouseModes mode = getMouseMode();
    printf("mouseReleaseEvent: mode=%d.\n", mode);

    const layout::dBox& vbox = getViewBox();

    if (mode == Zoom) {
        QRectF mapped = model.mapRect(QRectF(vbox.getMinX(), vbox.getMinY(),
                                             vbox.getWidth(), vbox.getHeight()));

        rubberBand->hide();
        QRect band = rubberBand->geometry();
        // factor to scale up the view.
        double factor = (width()>height())
                            ?height()/band.height()
                            :width()/band.width();
#if 0
        QVector3D bcenter;
        bcenter.setX(mapped.left()+band.center().x()*mapped.width()/width());
        bcenter.setY(mapped.bottom()+band.center().y()*mapped.height()/height());
        bcenter.setZ(layout::pointZval);

        QVector3D mcenter;
        mcenter.setX(mapped.center().x());
        mcenter.setY(mapped.center().y());
        mcenter.setZ(layout::pointZval);

        // find the matrix mapping bcenter to mapped vbox center.
        model.setToIdentity();
        // set the target point as the new origin
        model.translate(-mcenter.x(), mcenter.y());
        // translate to bcenter
        model.translate(bcenter.x(), -bcenter.y());
#else
        double scale = mapped.width() / vbox.getWidth();
        QVector2D diff(width()/2.0 - band.center().x(),
                       height()/2.0 - band.center().y());

        //model.scale(factor/scale, factor/scale);
        mapped = model.mapRect(QRectF(vbox.getMinX(), vbox.getMinY(),
                                      vbox.getWidth(), vbox.getHeight()));
        /* adjust rubberband's box to match window's aspect ratio
         * band' width/height = window's window/height;
         * band.width() = width()*band.height() / height();
         * if window's width > height:
         *   factor = width() / band.width()
         *          = width()*height() / (width()*band.height())
         *          = height() / band.height();
         * else if window's height > width:
         *   factor = height() / band.height()
         *          = width() / band.width();
         */
        model.translate(mapped.width()*diff.x()/width(),
                        -mapped.height()*diff.y()/height(),
                        0.0);
#endif
        //model.scale(factor/scale, factor/scale);
        update();
    } else if (false) { //(mode == Pan){
        model = mousePressModel;
        QRectF mapped = model.mapRect(QRectF(vbox.getMinX(), vbox.getMinY(),
                                             vbox.getWidth(), vbox.getHeight()));
        double scale = mapped.width() / vbox.getWidth();
        QVector2D diff = QVector2D(e->position() - mousePressPos);
        model.translate(vbox.getWidth()*diff.x()/(scale*width()),
                        -vbox.getHeight()*diff.y()/(scale*height()),
                        0.0);
        update();

    } else if (mode == Pan){
        model = mousePressModel;
        QRectF mapped = model.mapRect(QRectF(vbox.getMinX(), vbox.getMinY(),
                                             vbox.getWidth(), vbox.getHeight()));
        double scale = mapped.width() / vbox.getWidth();
        printf("pan scale=%.3lf\n", scale);
        QVector2D diff = QVector2D(e->position() - mousePressPos);
        model.translate(mapped.width()*diff.x()/width(),
                        -mapped.height()*diff.y()/height(),
                        0.0);
        update();
    } else if (mode == Rotate) {
        // Rotation axis is perpendicular to the mouse position difference
        // vector
        QVector2D diff = QVector2D(e->position() - mousePressPos);
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

        // Accelerate angular speed relative to the length of the mouse sweep
        qreal acc = diff.length() / 100.0;

        // Calculate new rotation axis as weighted sum
        rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

        // Increase angular speed
        angularSpeed += acc;
    }
}

void GLWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    // enable depth buffer
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

#if 0
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

    geometries = new GeometryEngine(layM);

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void GLWidget::initShaders()
{

    // Compile vertex shader
#if 0
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, "C:/Users/LPLung/projects/qt/2dpaint/vshader.glsl"))
        close();
#else
    const char vShaderSource[] = R"glsl(
    #version 330
    uniform mat4 mvp_matrix;
    attribute vec4 a_position;
    attribute vec4 a_color;
    out vec4 attrib_fragment_color;
    void main()
    {
        gl_Position = mvp_matrix * a_position;
        attrib_fragment_color = a_color;
    }
    )glsl";
    if (!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vShaderSource))
        close();
#endif

    // Compile fragment shader
#if 0
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, "C:/Users/LPLung/projects/qt/2dpaint/fshader.glsl"))
        close();
#else
    const char fShaderSource[] = R"glsl(
    #version 330
    in vec4 attrib_fragment_color;
    void main()
    {
        gl_FragColor = attrib_fragment_color;
    }
    )glsl";
    if (!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fShaderSource))
        close();
#endif
    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void GLWidget::initTextures()
{
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void GLWidget::resizeGL(int w, int h)
{
    const float zNear = 0.1f;
    const float zFar = getEyeCoords().z()*-10.0;
    // Reset projection
    projection.setToIdentity();

#if 1
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
#else
    projection.ortho(0, w, 0, h, zNear, zFar);
#endif
}

void GLWidget::paintGL()
{
    printf("paintGL...\n");
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();
    program.bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    QVector3D eye = getEyeCoords();
    matrix.translate(eye.x(), eye.y(), eye.z());
    matrix *= model;

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw layout
    geometries->drawLayoutGeometries(&program);

}
#endif
