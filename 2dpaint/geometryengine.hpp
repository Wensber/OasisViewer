// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef __GEOMETRYENGINE_H
#define __GEOMETRYENGINE_H

#include "layoutManager.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine(const layout::dLayoutManager& _layM);
    virtual ~GeometryEngine();

    const layout::dLayoutManager& getLayoutManager() const {
        return layM;
    }

    std::vector<int>& getVertexCounts() {
        return vertexCnts;
    }
    const std::vector<int>& getVertexCounts() const {
        return vertexCnts;
    }
    void drawLayoutGeometries(QOpenGLShaderProgram *program);

    void initLayoutGeometries();

private:
    QOpenGLBuffer arrayBuf;
    std::vector<int> vertexCnts;
    const layout::dLayoutManager& layM;
};

#endif // __GEOMETRYENGINE_H
