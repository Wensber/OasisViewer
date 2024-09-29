// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef __GLWIDGET_HPP__
#define __GLWIDGET_HPP__

#include "geometryengine.hpp"

#include <QBasicTimer>
#include <QColor>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#ifdef USE_QPAINTER
#include <QPainter>
#include <QPaintEvent>
#else
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#endif

#include "layoutManager.hpp"

extern float fov;


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    enum MouseModes {
        Zoom = 0,
        Pan,
        Rotate,
        Move
    };

    typedef double coord_type;

    GLWidget(const layout::dLayoutManager& layM, QWidget* parent);

    virtual ~GLWidget();

    const layout::dLayoutManager& getLayoutManager() const {
        return layM;
    }

    QColor getBackground() const {
        return background;
    }

#ifdef USE_QPAINTER
    void paintLayout(QPainter *painter, QPaintEvent *event);
#endif
    const layout::dBox& getViewBox();

public slots:
    void onZoomFull() {
        setZoomFactor(1.0);
        setPannedDist(QVector2D(0, 0));
        setPanDiff(QVector2D(0, 0));
        rotationAxis = QVector3D();
        angularSpeed = 0;
        rotation = QQuaternion();
        update();
    }
    void onZoomInHalf() {
        setZoomFactor(getZoomFactor()*2.0); //sqrt(2.0));
        update();
    }
    void onZoomOut2X() {
        setZoomFactor(getZoomFactor()*0.5); //sqrt(0.5));
        update();
    }

    void onZoomIn(bool bChecked) {
        printf("onZoomIn: bChecked=%d.\n", bChecked);
        if (bChecked) {
            setMouseMode(Zoom);
        } else {
            setMouseMode(Move);
        }
    }

    void onPan(bool bChecked) {
        if (bChecked) {
            setMouseMode(Pan);
        } else {
            setMouseMode(Move);
        }
    }

    void onRotate(bool bChecked) {
        if (bChecked) {
            setMouseMode(Rotate);
        } else {
            setMouseMode(Move);
        }
    }

    GeometryEngine* getGeometry() {
        return geometries;
    }

    const GeometryEngine* getGeometry() const {
        return geometries;
    }

    void update() {
        if(getGeometry()) {
            getGeometry()->initLayoutGeometries();
        }
        QOpenGLWidget::update();
    }

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

    QVector3D getEyeCoords() {
        const layout::dBox& vbox = getViewBox();
        layout::dPoint ctr = vbox.center();
        QVector2D panned = getPannedDist() + getPanDiff();
        double dist = std::max(vbox.getWidth(), vbox.getHeight());
        return QVector3D(-ctr.x()+dist*panned.x()/width(),
                         -ctr.y()-dist*panned.y()/height(),
                         -dist*0.5 / std::tan((fov/2.0)*PI/180.0));
    }

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();
    void initTextures();

    double getZoomFactor() const {
        return zoomFactor;
    }
    void setZoomFactor(double factor) {
        zoomFactor = factor;
        printf("setZoomFactor = %.3lf\n", getZoomFactor());
    }

    MouseModes getMouseMode() const {
        return mouseMode;
    }
    void setMouseMode(MouseModes m) {
        mouseMode = m;
    }

    const QVector2D& getPannedDist() const {
        return pannedDist;
    }
    void setPannedDist(const QVector2D& dist) {
        pannedDist = dist;
    }

    const QVector2D& getPanDiff() const {
        return panDiff;
    }
    void setPanDiff(const QVector2D& diff) {
        panDiff = diff;
    }

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries = nullptr;

    QOpenGLTexture *texture = nullptr;

    QMatrix4x4 projection;

    QPointF mousePressPos;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;

    const layout::dLayoutManager& layM;
    QColor background;
    layout::dBox viewBBox{std::numeric_limits<coord_type>::min(),
                          std::numeric_limits<coord_type>::min(),
                          std::numeric_limits<coord_type>::min(),
                          std::numeric_limits<coord_type>::min()};
    double zoomFactor = 1.0;
    MouseModes mouseMode = Move;
    QVector2D pannedDist;
    QVector2D panDiff;
};


#endif
