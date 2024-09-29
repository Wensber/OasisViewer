
#include "mainwindow.hpp"

#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include <QTranslator>


void createOasisFile() {

    std::string name = "Test";

    layout::Layout<layout::dPoint> layout(name);

    layout::Cell<layout::dPoint>* cell1 = layout.newCell("Cell 1");

    layout::Layer<layout::dPoint>* layer1 = cell1->newLayer(1, 1, QColor("red"));
    layer1->addShape(new layout::Box<layout::dPoint>(0, 0, 10, 20));
    layer1->addShape(new layout::Box<layout::dPoint>(-10, -10, 0, 10));

    layout::Layer<layout::dPoint>* layer2 = cell1->newLayer(2, 1, QColor("red"));
    std::vector<layout::dPoint> pts1 = {{2, 3}, {4, 3}, {6, 4}, {4, 5}, {2, 5}, {0, 4}};
    layer2->addShape(new layout::Polygon<layout::dPoint>(pts1));

    layout::Layer<layout::dPoint>* layer3 = cell1->newLayer(3, 1, QColor("red"));
    layout::dPoint cntr1 = {5, 6};
    layer3->addShape(new layout::Circle<layout::dPoint>(cntr1, 3));

    layout.print();

    oasisio::OasisFileManager<layout::dPoint> ofm;
    ofm.writeOasisFile(&layout, name + ".oas");

}


int main(int argc, char *argv[])
{

#if 1

    createOasisFile();

    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(16);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("OASIS Viewer");
    app.setApplicationVersion("0.1");

    MainWindow w;
    w.show();

    std::cout << "hello world." << std::endl;
    return app.exec();

#else


    createOasisFile();

    std::string name = "Test.oas";
    oasisio::OasisFileManager<layout::dPoint> ofm;


    layout::Layout<layout::dPoint> output(name);
    std::ifstream ifs;
    ifs.open(name);
    ofm.readOasisFile(ifs, output);

    output.print();


#endif

}












