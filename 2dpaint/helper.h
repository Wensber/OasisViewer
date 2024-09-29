// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef HELPER_H
#define HELPER_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>

//! [0]
class Helper
{
public:
    Helper();

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);

protected:
    std::vector<QBrush> background;
    QBrush boxBrush;
    QBrush polyBrush;
    QBrush trapBrush;

    QPen boxPen;
    QPen polyPen;
    QPen trapPen;

    QFont textFont;
    QPen textPen;
};
//! [0]

#endif
