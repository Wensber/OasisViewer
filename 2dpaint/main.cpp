
#include "mainwindow.hpp"

#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include <QTranslator>


void createOasisFile() {

    std::string name = "Test";

    layout::Layout<layout::dPoint> layout(name);

    layout::Cell<layout::dPoint>* cell1 = layout.newCell("A");

    //layout::Layer<layout::dPoint>* layer1 = cell1->newLayer(1, 1, QColor("red"));
    //layer1->addShape(new layout::Box<layout::dPoint>(0, 0, 2, 11));
    //layer1->addShape(new layout::Box<layout::dPoint>(0, 0, 11, 2));
    //layer1->addShape(new layout::Box<layout::dPoint>(0, 0, 6, 11));

    layout::Layer<layout::dPoint>* layer2 = cell1->newLayer(2, 1, QColor("green"));
    //std::vector<layout::dPoint> pts1 = {{2, 6}, {6, 10}, {10, 6}, {6, 2}};
    //std::vector<layout::dPoint> pts1 = {{-162, -89}, {-187, -87}, {-170, -64}};
    std::vector<layout::dPoint> pts1 = {{0, 30}, {0, 60}, {30, 90}, {60, 90}, {90, 60}, {90, 30}, {60, 0}, {30, 0}};
    layer2->addShape(new layout::Polygon<layout::dPoint>(pts1));

    layout::Layer<layout::dPoint>* layer3 = cell1->newLayer(3, 1, QColor("yellow"));
    //layout::dPoint cntr1 = {6, 6};
    //layer3->addShape(new layout::Circle<layout::dPoint>(cntr1, 2));
    layout::dPoint cntr1 = {15, 15};
    layer3->addShape(new layout::Circle<layout::dPoint>(cntr1, 15));
    layout::dPoint cntr2 = {75, 15};
    layer3->addShape(new layout::Circle<layout::dPoint>(cntr2, 15));
    layout::dPoint cntr3 = {15, 75};
    layer3->addShape(new layout::Circle<layout::dPoint>(cntr3, 15));
    layout::dPoint cntr4 = {75, 75};
    layer3->addShape(new layout::Circle<layout::dPoint>(cntr4, 15));

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

#elif 0

    createOasisFile();

    std::string name = "Test.oas";
    oasisio::OasisFileManager<layout::dPoint> ofm;

    layout::Layout<layout::dPoint> output(name);
    std::ifstream ifs;
    ifs.open(name);
    ofm.readOasisFile(ifs, output);
    ifs.close();

    output.print();

#elif 0

    createOasisFile();

    std::string name = "Test.oas";
    std::string name2 = "C:/Users/seben/Downloads/poly.oas";

    std::ifstream ifs;

    ifs.open(name);
    oasisio::byte bytes[1] = {0};
    while(!ifs.eof()) {
        ifs.read((char *)bytes, 1);
        //std::cout << bytes[0];
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)(bytes[0]) << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    ifs.close();

    ifs.open(name2);
    while(!ifs.eof()) {
        ifs.read((char *)bytes, 1);
        //std::cout << bytes[0];
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)(bytes[0]) << " ";
    }
    std::cout << std::endl;
    ifs.close();

#elif 0

    std::string name = "table.oas";

    oasisio::TableOffsets table( 1, 107,
                                 1, 0,
                                 1, 0,
                                 1, 80,
                                 1, 0,
                                 1, 0 );

    std::ofstream ofs;
    ofs.open(name);
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getCellnameFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getCellnameOffset());

    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getTextstringFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getTextstringOffset());

    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getPropnameFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getPropnameOffset());

    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getPropstringFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getPropstringOffset());

    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getLayernameFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getLayernameOffset());

    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getXnameFlag());
    oasisio::OasisWriter::toBytesUnsigned(ofs, table.getXnameOffset());
    ofs.close();

    std::ifstream ifs;
    ifs.open(name);

    unsigned int cellnameFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int cellnameOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int textstringFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int textstringOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int propnameFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int propnameOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int propstringFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int propstringOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int layernameFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int layernameOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int xnameFlag = oasisio::OasisReader::fromBytesUnsigned(ifs);
    unsigned int xnameOffset = oasisio::OasisReader::fromBytesUnsigned(ifs);

    oasisio::TableOffsets out(cellnameFlag, cellnameOffset,
                       textstringFlag, textstringOffset,
                       propnameFlag, propnameOffset,
                       propstringFlag, propstringOffset,
                       layernameFlag, layernameOffset,
                       xnameFlag, xnameOffset);

    std::cout << out << std::endl;

#else

    std::string name = "unsigned.oas";

    std::ofstream ofs;
    oasisio::OasisWriter ow(&ofs);
    ofs.open(name);

    int count = 1;
    ow.toBytesUnsigned(10);

    ofs.close();
    std::ifstream ifs;
    ifs.open(name);

    int i;
    int next;
    for(i=0; i<count; ++i) {
        next = oasisio::OasisReader::fromBytesUnsigned(ifs);
        std::cout << std::dec << next << std::endl;
    }

#endif

}












