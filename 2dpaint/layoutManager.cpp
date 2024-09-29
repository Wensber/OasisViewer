// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

// modified from Qt OpenGL cube example

#include "trapezoid.hpp"
#include "circle.hpp"
#include "polygon.hpp"
#include "layoutManager.hpp"

namespace layout
{

void initializeManager(dLayoutManager& layMan) {
    dLayout* a = layMan.newLayout("./TestLayout.oas");

    dCell* a_top = a->newCell("top");

    dLayer* a_0 = a_top->newLayer(0, 0, QColor("red"));
    a_0->addShape(new dBox(dPoint(0, 0), dPoint(1, 2)));
    std::vector<dPoint> pts = {{2, 0}, {4, 0}, {3, 1}, {2, 1}};
    a_0->addShape(new dTrapezoid(pts, 0));
    a_0->addShape(new dCircle(dPoint(4.5, 1.5), 1));
    std::vector<dPoint> pts1 = {{2, 3}, {4, 3}, {6, 4}, {4, 5}, {2, 5}, {0, 4}};
    a_0->addShape(new dPolygon(pts1));

    dLayer* a_1 = a_top->newLayer(1, 0, QColor("green"));
    a_1->addShape(new dBox(dPoint(1, 6), dPoint(1, 8)));
    std::vector<dPoint> pts2 = {{3, 6}, {5, 6}, {4, 7}, {3, 7}};
    a_1->addShape(new dTrapezoid(pts2, 0));
    a_1->addShape(new dCircle(dPoint(5.5, 7.5), 1));
    std::vector<dPoint> pts3 = {{3, 9}, {5, 9}, {7, 10}, {5, 11}, {3, 11}, {1, 10}};
    a_1->addShape(new dPolygon(pts3));

#if 0
    dLayout* b = layMan.newLayout("B");
    dCell* b_top = b->newCell("top");

    dLayer* b_0 = b_top->newLayer(0, 0, QColor("red"));
    dLayer* b_1 = b_top->newLayer(1, 0, QColor("green"));

    b_0->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0));
    b_0->addShape( new Trapezoid<pointT>(pointT(1.0, 20.0), 1.0, 2.0, 3) );
    b_0->addShape( new Circle<pointT>(pointT(3.0, 20.0), 1.0) );

    b_1->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 30.0)) );
    b_1->addShape( new Trapezoid<pointT>(pointT(1.0, 30.0), 1.0, 2.0, 3) );
    b_1->addShape( new Circle<pointT>(pointT(3.0, 30.0), 1.0) );
#endif
}

}
