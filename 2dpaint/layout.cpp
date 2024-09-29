// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

// modified from Qt OpenGL cube example

#include "layoutManager.hpp"

namespace layout
{

void initializeManager(dLayoutManager& layMan) {
    dLayout* a = layMan.newLayout("A");

    dCell* a_top = a->newCell("top");

    dLayer* a_0 = a_top->newLayer(0, 0, QColor("red"));
    a_0->addShape(new dBox(dPoint(0.0, 0.0), dPoint(1.0, 2.0)));
#if 0
    a_0->addShape( new dTrapezoid(dPoint(2.0, 0.0), dPoint(2.0, 3.0), 5.0, 3.0, 0));
    a_0->addShape( new dCircle(dPoint(4.0, 2.0), 1.0));
    std::vector<dPoint> pts1 = {{2,3}, {4,3}, {6,5}, {4,7}, {2,7}, {0,5}};
    a_0->addShape( new dPolygon(pts1));

    dLayer* a_1 = a_top->newLayer(1, 0, QColor("green"));
    a_1->addShape( new dBox(dPoint(0.0, 10.0), dPoint(1.0, 12.0)));
    a_1->addShape( new dTrapezoid(dPoint(2.0, 10.0), dPoint(2.0, 14.0), 3.0, 10.0, 0));
    //a_1->addShape( new dCircle(pointT(3.0, 15.0), 1.0) );
    std::vector<dPoint> pts2 = {{2,13}, {4,13}, {6,15}, {4,17}, {2,17}, {0,15}};
    a_1->addShape( new dPolygon(pts2));

    dLayout* b = layMan.newLayout("B");
    dCell* b_top = b->newCell("top");

    dLayer* b_0 = b_top->newLayer(0, 0, QColor("red"));
    dLayer* b_1 = b_top->newLayer(1, 0, QColor("green"));

    b_0->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 20.0)) );
    b_0->addShape( new Trapezoid<pointT>(pointT(1.0, 20.0), 1.0, 2.0, 3) );
    b_0->addShape( new Circle<pointT>(pointT(3.0, 20.0), 1.0) );

    b_1->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 30.0)) );
    b_1->addShape( new Trapezoid<pointT>(pointT(1.0, 30.0), 1.0, 2.0, 3) );
    b_1->addShape( new Circle<pointT>(pointT(3.0, 30.0), 1.0) );
#endif
} // inititalizeManager

} // namespace layout
