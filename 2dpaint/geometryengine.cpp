// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "geometryengine.hpp"

#include <QVector2D>
#include <QVector3D>


GeometryEngine::GeometryEngine(const layout::dLayoutManager& _layM)
    : layM(_layM)
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();

    // Initializes layout geometries and transfers it to VBOs
    initLayoutGeometries();
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
}

void GeometryEngine::initLayoutGeometries() {
    const layout::dLayout* activeLayout = getLayoutManager().getActiveLayout();
    if (activeLayout) {
        printf("initLayoutGeometries: activeLayout=%s...\n", activeLayout->getName().c_str());
        int verCnt = activeLayout->getVertexCount();
        std::vector<QVector3D> vertices;
        vertices.reserve(verCnt);
        getVertexCounts().clear();
        activeLayout->getVertices(vertices, getVertexCounts());

        // Transfer vertex data to VBO 0
        arrayBuf.bind();
        //arrayBuf.setUsagePattern(QGLBuffer::DynamicDraw);
        arrayBuf.allocate(vertices.data(), verCnt * sizeof(QVector3D));
    }
}

void GeometryEngine::drawLayoutGeometries(QOpenGLShaderProgram* program) {
    const layout::dLayout* activeLayout = getLayoutManager().getActiveLayout();
    if (!activeLayout)
        return;
    printf("drawLayoutGeometries: activeLayout=%s...\n", activeLayout->getName().c_str());

    // Tell OpenGL which VBOs to use
    arrayBuf.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    // setAttributeBuffer(int location, GLenum type, int offset, int tupleSize, int stride = 0)
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

#if 0
    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    // original: program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 3);
#endif

    // Draw layout geometry using indices from VBO 1
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#if 0
    glDrawArrays(GL_POLYGON, 0, 6);
    glDrawArrays(GL_POLYGON, 6, 4);
#else
    std::vector<int>::const_iterator it = getVertexCounts().begin();
    int first = 0;
    for (; it != getVertexCounts().end(); ++it) {
        glDrawArrays(GL_POLYGON, first, *it);
        first += *it;
    }
#endif
}
