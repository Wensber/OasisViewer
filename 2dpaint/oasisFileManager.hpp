#ifndef OASISFILEMANAGER_H
#define OASISFILEMANAGER_H

#include <oasisIO.hpp>
#include <iostream>
#include <layout.hpp>
#include <bitset>
#include "polygon.hpp"
#include "circle.hpp"
#include "trapezoid.hpp"

namespace oasisio {


class TableOffsets {
protected:
    unsigned int cellnameFlag;
    unsigned int cellnameOffset;

    unsigned int textstringFlag;
    unsigned int textstringOffset;

    unsigned int propnameFlag;
    unsigned int propnameOffset;

    unsigned int propstringFlag;
    unsigned int propstringOffset;

    unsigned int layernameFlag;
    unsigned int layernameOffset;

    unsigned int xnameFlag;
    unsigned int xnameOffset;


    std::map<unsigned int, std::string> cellnames;
    unsigned int cellReferences = 0;

    std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string> layernames;

public:
    TableOffsets(unsigned int cnf, unsigned int cno, unsigned int tsf, unsigned int tso, unsigned int pnf, unsigned int pno,
                 unsigned int psf, unsigned int pso, unsigned int lnf, unsigned int lno, unsigned int xnf, unsigned int xno)
        : cellnameFlag(cnf)
        , cellnameOffset(cno)
        , textstringFlag(tsf)
        , textstringOffset(tso)
        , propnameFlag(pnf)
        , propnameOffset(pno)
        , propstringFlag(psf)
        , propstringOffset(pso)
        , layernameFlag(lnf)
        , layernameOffset(lno)
        , xnameFlag(xnf)
        , xnameOffset(xno)
    {}


    void extractCellnames(std::ifstream& ifs) {

        if(cellnameOffset == 0) {
            return;
        }

        if(cellnameFlag == 1) { //Strict

            int curPos = ifs.tellg();
            ifs.seekg(cellnameOffset, ifs.beg);

            while(true) {
                unsigned int recordID = OasisReader::fromBytesUnsigned(ifs);
                if(recordID != 4) {
                    break;
                }
                std::string cellname = OasisReader::fromBytesString(ifs);
                unsigned int reference = OasisReader::fromBytesUnsigned(ifs);
                std::cout << "Cellname : " << cellname << std::endl;
                std::cout << "Reference : " << reference << std::endl;
                cellnames[reference] = cellname;
            }

            ifs.seekg(curPos, ifs.beg);

        }

    }

    void extractLayernames(std::ifstream& ifs) {

        if(layernameOffset == 0) {
            return;
        }

        if(layernameFlag == 1) {

            int curPos = ifs.tellg();
            ifs.seekg(layernameOffset, ifs.beg);

            while(true) {
                unsigned int recordID = OasisReader::fromBytesUnsigned(ifs);
                if(recordID != 11) {
                    break;
                }
                unsigned int li1, li2, di1, di2;
                std::string layername = OasisReader::fromBytesString(ifs);
                unsigned int layer_interval_type = OasisReader::fromBytesUnsigned(ifs);
                switch(layer_interval_type) {
                case 3: //Only supported case atm
                    li1 = OasisReader::fromBytesUnsigned(ifs);
                    li2 = li1;
                    break;
                }
                unsigned int data_interval_type = OasisReader::fromBytesUnsigned(ifs);
                switch(data_interval_type) {
                case 3: //Only supported case atm
                    di1 = OasisReader::fromBytesUnsigned(ifs);
                    di2 = di1;
                    break;
                }
                addLayername(layername, li1, li2, di1, di2);
            }

            ifs.seekg(curPos, ifs.beg);

        }

    }

    const unsigned int getCellnameFlag() const {
        return cellnameFlag;
    }
    void setCellnameFlag(unsigned int i) {
        cellnameFlag = i;
    }

    const unsigned int getCellnameOffset() const {
        return cellnameOffset;
    }
    void setCellnameOffset(unsigned int i) {
        cellnameOffset = i;
    }

    const unsigned int getTextstringFlag() const {
        return textstringFlag;
    }
    void setTextstringFlag(unsigned int i) {
        textstringFlag = i;
    }

    const unsigned int getTextstringOffset() const {
        return textstringOffset;
    }
    void setTextstringOffset(unsigned int i) {
        textstringOffset = i;
    }

    const unsigned int getPropnameFlag() const {
        return propnameFlag;
    }
    void setPropnameFlag(unsigned int i) {
        propnameFlag = i;
    }

    const unsigned int getPropnameOffset() const {
        return propnameOffset;
    }
    void setPropnameOffset(unsigned int i) {
        propnameOffset = i;
    }

    const unsigned int getPropstringFlag() const {
        return propstringFlag;
    }
    void setPropstringFlag(unsigned int i) {
        propstringFlag = i;
    }

    const unsigned int getPropstringOffset() const {
        return propstringOffset;
    }
    void setPropstringOffset(unsigned int i) {
        propstringOffset = i;
    }

    const unsigned int getLayernameFlag() const {
        return layernameFlag;
    }
    void setLayernameFlag(unsigned int i) {
        layernameFlag = i;
    }

    const unsigned int getLayernameOffset() const {
        return layernameOffset;
    }
    void setLayernameOffset(unsigned int i) {
        layernameOffset = i;
    }

    const unsigned int getXnameFlag() const {
        return xnameFlag;
    }
    void setXnameFlag(unsigned int i) {
        xnameFlag = i;
    }

    const unsigned int getXnameOffset() const {
        return xnameOffset;
    }
    void setXnameOffset(unsigned int i) {
        xnameOffset = i;
    }


    unsigned int addCellname(std::string s) {
        ++cellReferences;
        cellnames[cellReferences] = s;
        return cellReferences;
    }
    std::map<unsigned int, std::string>& getCellnames() {
        return cellnames;
    }

    void addLayername(std::string s, unsigned int li1, unsigned int li2, unsigned int di1, unsigned int di2) {
        layernames[std::make_tuple(li1, li2, di1, di2)] = s;
    }
    std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string>& getLayernames() {
        return layernames;
    }


};

std::ostream& operator<<(std::ostream& o, const TableOffsets& to);



template<class pointT>
class OasisFileManager {
public:
    OasisFileManager()
    {}

    void readOasisFile(std::ifstream& ifs, layout::Layout<pointT>& outLayout) {

        std::string magic = OasisReader::fromBytesString(ifs);
        if(magic != MAGIC) {
            throw std::exception("Failed to find magic bytes.");
        }


        TableOffsets table = readStartRecord(ifs);
        std::cout << table << std::endl;
        std::cout << "Current Pos : " << (int)ifs.tellg() << std::endl;
        table.extractCellnames(ifs);
        table.extractLayernames(ifs);
        std::map<unsigned int, std::string>& cellnames = table.getCellnames();


        //Read in Records
        bool done = false;
        while(!done) {

            unsigned int recordID = OasisReader::fromBytesUnsigned(ifs);
            std::cout << "Record ID : " << recordID << std::endl;

            switch(recordID) {

            case 2: //End Record
            {
                done = true;
                //Potentially do something with the validation scheme
                break;
            }

            case 4: //Cellname with reference number
            {
                if(table.getCellnameFlag() == 1) { //Strict
                    char len[1]; //String length
                    ifs.read(len, 1);
                    std::cout << "Skip " << len[0]+4 << " bytes" << std::endl;
                    ifs.seekg(len[0]+4, ifs.cur);
                } else {
                    std::string cellname = OasisReader::fromBytesString(ifs);
                    unsigned int reference = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Cellname : " << cellname << std::endl;
                    std::cout << "Reference : " << reference << std::endl;
                    cellnames[reference] = cellname;
                }
                break;
            }

            case 11:
            {
                if(table.getLayernameFlag() == 1) {
                    char len[1]; //String length
                    ifs.read(len, 1);
                    std::cout << "Skip " << len[0]+16 << " bytes" << std::endl;
                    ifs.seekg(len[0]+16, ifs.cur);
                } else {
                    unsigned int li1, li2, di1, di2;
                    std::string layername = OasisReader::fromBytesString(ifs);
                    unsigned int layer_interval_type = OasisReader::fromBytesUnsigned(ifs);
                    switch(layer_interval_type) {
                    case 3: //Only supported case atm
                        li1 = OasisReader::fromBytesUnsigned(ifs);
                        li2 = li1;
                        break;
                    }
                    unsigned int data_interval_type = OasisReader::fromBytesUnsigned(ifs);
                    switch(data_interval_type) {
                    case 3: //Only supported case atm
                        di1 = OasisReader::fromBytesUnsigned(ifs);
                        di2 = di1;
                        break;
                    }
                    table.addLayername(layername, li1, li2, di1, di2);
                }
                break;
            }

            case 13: //Cell with reference number
            {
                unsigned int reference = OasisReader::fromBytesUnsigned(ifs);
                std::string cellname = cellnames.find(reference)->second;
                //Add functionality to look up cellname table if the reference hasn't been added yet
                std::cout << "Cellname : " << cellname << std::endl;
                std::cout << "Reference : " << reference << std::endl;

                layout::Cell<pointT>* cell = outLayout.newCell(cellname);
                readCellRecord(ifs, table, cell);

                break;
            }


            }

            if(ifs.eof()) {
                done = true;
                throw std::exception("Failed to find End Record.");
            }

        }

    }

    void writeOasisFile(const layout::Layout<pointT>* layout, std::string name) {

        std::ofstream ofs;
        ofs.open(name, std::ios::trunc);

        //magic bytes
        OasisWriter::toBytesString(ofs, MAGIC);

        //Start Record

        //Record ID
        OasisWriter::toBytesUnsigned(ofs, 1);
        OasisWriter::toBytesString(ofs, VERSION);
        //Unit - Placeholder, figure out method of determining unit
        OasisWriter::toBytesReal(ofs, SINGLE_PRECISION_FLOAT, 1.0f);
        //offset-flag, Table Offsets stored in End Record
        OasisWriter::toBytesUnsigned(ofs, 1);

        TableOffsets table( 1, 0,
                           1, 0,
                           1, 0,
                           1, 0,
                           1, 0,
                           1, 0 );

        //Cells
        typename std::map<std::string, layout::Cell<pointT>*>::const_iterator it;
        for(it = layout->getCells().begin(); it != layout->getCells().end(); ++it) {

            const layout::Cell<pointT>* cell = it->second;

            unsigned int ref = table.addCellname(cell->getName());

            //Record ID
            OasisWriter::toBytesUnsigned(ofs, 13);
            //Reference Number
            OasisWriter::toBytesUnsigned(ofs, ref);

            writeCellRecord(ofs, table, cell);

        }


        //Cellname Records
        if(table.getCellnames().size() > 0) {

            unsigned int pos = ofs.tellp();
            table.setCellnameOffset(pos);

            typename std::map<unsigned int, std::string>::const_iterator it2 = table.getCellnames().begin();
            for(; it2 != table.getCellnames().end(); ++it2) {

                //Record ID
                OasisWriter::toBytesUnsigned(ofs, 4);
                //Name String
                OasisWriter::toBytesString(ofs, it2->second);
                //Reference Number
                OasisWriter::toBytesUnsigned(ofs, it2->first);

            }

        }

        //Layername Records
        if(table.getLayernames().size() > 0) {

            unsigned int pos = ofs.tellp();
            table.setLayernameOffset(pos);

            typename std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string>::const_iterator it = table.getLayernames().begin();
            for(; it != table.getLayernames().end(); ++it) {

                //Only handling Type 3 intervals right now

                //Record ID
                OasisWriter::toBytesUnsigned(ofs, 11);
                //Layer Name
                OasisWriter::toBytesString(ofs, it->second);

                //Layer Name Interval Type
                OasisWriter::toBytesUnsigned(ofs, 3);
                //Bound
                OasisWriter::toBytesUnsigned(ofs, std::get<0>(it->first));

                //Datatype Interval Type
                OasisWriter::toBytesUnsigned(ofs, 3);
                //Bound
                OasisWriter::toBytesUnsigned(ofs, std::get<2>(it->first));

            }

        }

        //Other Name Records go here



        //End Record

        //Record ID
        OasisWriter::toBytesUnsigned(ofs, 2);

        //Table Offsets
        OasisWriter::toBytesUnsigned(ofs, table.getCellnameFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getCellnameOffset());

        OasisWriter::toBytesUnsigned(ofs, table.getTextstringFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getTextstringOffset());

        OasisWriter::toBytesUnsigned(ofs, table.getPropnameFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getPropnameOffset());

        OasisWriter::toBytesUnsigned(ofs, table.getPropstringFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getPropstringOffset());

        OasisWriter::toBytesUnsigned(ofs, table.getLayernameFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getLayernameOffset());

        OasisWriter::toBytesUnsigned(ofs, table.getXnameFlag());
        OasisWriter::toBytesUnsigned(ofs, table.getXnameOffset());

        unsigned int numPadding = 256 - 4 - (4 * 12) - 4;
        unsigned int i;
        for(i=0; i<numPadding; ++i) {
            OasisWriter::toBytesChar(ofs, 0);
        }

        //Validation Scheme - None
        OasisWriter::toBytesUnsigned(ofs, 0);

        ofs.close();

    }



protected:
    TableOffsets readStartRecord(std::ifstream& ifs) {

        unsigned int recordID = OasisReader::fromBytesUnsigned(ifs);
        if(recordID != 1) {
            throw std::exception("Failed to find Start Record.");
        }
        std::cout << "Start Record ID : " << recordID << std::endl;

        std::string version = OasisReader::fromBytesString(ifs);
        if(version != VERSION) {
            throw std::exception("Incorrect Oasis version.");
        }
        std::cout << "Version : " << version << std::endl;

        //Not yet used for anything
        float unit = OasisReader::fromBytesReal(ifs);
        std::cout << "Unit : " << unit << std::endl;

        unsigned int offsetFlag = OasisReader::fromBytesUnsigned(ifs);
        std::cout << "Offset Flag : " << offsetFlag << std::endl;

        int curPos = ifs.tellg();
        std::cout << "Current Pos : " << curPos << std::endl;
        if(offsetFlag == 1) {
            ifs.seekg(-252, ifs.end);
        }

        unsigned int cellnameFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int cellnameOffset = OasisReader::fromBytesUnsigned(ifs);
        unsigned int textstringFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int textstringOffset = OasisReader::fromBytesUnsigned(ifs);
        unsigned int propnameFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int propnameOffset = OasisReader::fromBytesUnsigned(ifs);
        unsigned int propstringFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int propstringOffset = OasisReader::fromBytesUnsigned(ifs);
        unsigned int layernameFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int layernameOffset = OasisReader::fromBytesUnsigned(ifs);
        unsigned int xnameFlag = OasisReader::fromBytesUnsigned(ifs);
        unsigned int xnameOffset = OasisReader::fromBytesUnsigned(ifs);

        TableOffsets table(cellnameFlag, cellnameOffset,
                           textstringFlag, textstringOffset,
                           propnameFlag, propnameOffset,
                           propstringFlag, propstringOffset,
                           layernameFlag, layernameOffset,
                           xnameFlag, xnameOffset);

        if(offsetFlag == 1) {
            ifs.seekg(curPos, ifs.beg);
        }

        return table;

    }


    void writeCellRecord(std::ofstream& ofs, TableOffsets& table, const layout::Cell<pointT>* cell) {

        typename std::map<std::string, layout::Layer<pointT>*>::const_iterator it;
        for(it = cell->getLayers().begin(); it != cell->getLayers().end(); ++it) {

            const layout::Layer<pointT>* layer = it->second;
            table.addLayername(layer->getName(), layer->getLayerNum(), layer->getLayerNum(), layer->getDataType(), layer->getDataType());

            std::cout << "Reading Shapes" << std::endl;
            for(layout::iShape<pointT>* shape : layer->getShapes()) {

                if(shape->getShapeType() == BOX) {

                    layout::Box<pointT>* box = (layout::Box<pointT>*) shape;
                    unsigned char rectangle_info = 0;
                    unsigned int height = box->getHeight();
                    unsigned int width = box->getWidth();
                    bool square = height == width;

                    //Layer Number
                    rectangle_info += 1;
                    //Datatype
                    rectangle_info += 2;
                    //Repetition, not considered yet
                    rectangle_info += 0;//4
                    //Y
                    rectangle_info += 8;
                    //X
                    rectangle_info += 16;
                    //H
                    rectangle_info += (square ? 0 : 32);
                    //W
                    rectangle_info += 64;
                    //S
                    rectangle_info += (square ? 128 : 0);

                    //Record ID
                    OasisWriter::toBytesUnsigned(ofs, 20);
                    //Rectangle Info
                    OasisWriter::toBytesChar(ofs, rectangle_info);
                    //Layer Number
                    OasisWriter::toBytesUnsigned(ofs, layer->getLayerNum());
                    //Datatype
                    OasisWriter::toBytesUnsigned(ofs, layer->getDataType());
                    //Width
                    OasisWriter::toBytesUnsigned(ofs, width);
                    //Height
                    if(!square) {
                        OasisWriter::toBytesUnsigned(ofs, height);
                    }
                    //X
                    OasisWriter::toBytesSigned(ofs, box->getMinX());
                    //Y
                    OasisWriter::toBytesSigned(ofs, box->getMinY());

                } else if(shape->getShapeType() == POLYGON) {

                    layout::Polygon<pointT>* polygon = (layout::Polygon<pointT>*) shape;
                    std::vector<QVector3D> vertices;
                    std::vector<int> count;
                    polygon->getVertices(vertices, count);
                    oasisio::PointList pointList(POINT_LIST_4);
                    fillPointList(pointList, vertices);

                    std::cout << "Write Point List" << std::endl;
                    int i;
                    for(i=0; i<pointList.getList().size(); ++i) {
                        std::cout << "(" << pointList.getList().at(i).getDeltaX() << ", " <<
                                            pointList.getList().at(i).getDeltaY() << ") ";
                    }
                    std::cout << std::endl;

                    unsigned char polygon_info = 0;

                    //Layer Number
                    polygon_info += 1;
                    //Datatype
                    polygon_info += 2;
                    //Repetition, not considered yet
                    polygon_info += 0;//4
                    //Y
                    polygon_info += 8;
                    //X
                    polygon_info += 16;
                    //Point List
                    polygon_info += 32;

                    //Record ID
                    OasisWriter::toBytesUnsigned(ofs, 21);
                    //Polygon Info
                    OasisWriter::toBytesChar(ofs, polygon_info);
                    //Layer Number
                    OasisWriter::toBytesUnsigned(ofs, layer->getLayerNum());
                    //Datatype
                    OasisWriter::toBytesUnsigned(ofs, layer->getDataType());
                    //Point List
                    OasisWriter::toBytesPointList(ofs, pointList);
                    //X
                    OasisWriter::toBytesSigned(ofs, (int)vertices.at(0).x());
                    //Y
                    OasisWriter::toBytesSigned(ofs, (int)vertices.at(0).y());

                } else if(shape->getShapeType() == CIRCLE) {

                    layout::Circle<pointT>* circle = (layout::Circle<pointT>*) shape;
                    unsigned int radius = circle->getRadius();
                    int x = bg::get<0>(circle->getCenter());
                    int y = bg::get<1>(circle->getCenter());

                    unsigned char circle_info = 0;

                    //Layer Number
                    circle_info += 1;
                    //Datatype
                    circle_info += 2;
                    //Repetition, not considered yet
                    circle_info += 0;//4
                    //Y
                    circle_info += 8;
                    //X
                    circle_info += 16;
                    //r
                    circle_info += 32;

                    std::cout << "Writing Circle" << std::endl;
                    std::cout << "Circle Info " << std::bitset<8>((int)circle_info).to_string() << std::endl;
                    std::cout << "Layer Num " << layer->getLayerNum() << std::endl;
                    std::cout << "Data Type " << layer->getDataType() << std::endl;
                    std::cout << "Radius " << radius << std::endl;
                    std::cout << "X " << x << std::endl;
                    std::cout << "Y " << y << std::endl;

                    //Record ID
                    OasisWriter::toBytesUnsigned(ofs, 27);
                    //Circle Info
                    OasisWriter::toBytesChar(ofs, circle_info);
                    //Layer Number
                    OasisWriter::toBytesUnsigned(ofs, layer->getLayerNum());
                    //Datatype
                    OasisWriter::toBytesUnsigned(ofs, layer->getDataType());
                    //Radius
                    OasisWriter::toBytesUnsigned(ofs, radius);
                    //X
                    OasisWriter::toBytesSigned(ofs, x);
                    //Y
                    OasisWriter::toBytesSigned(ofs, y);

                }

            }

        }

    }

    void fillPointList(oasisio::PointList& pointList, const std::vector<QVector3D>& vertices) {

        pointList.addDelta(DELTA_G2, 0, 0);

        int i;
        for(i=1; i<vertices.size(); ++i) {
            pointList.addDelta(DELTA_G2, (int) (vertices.at(i).x() - vertices.at(i-1).x()),
                                         (int) (vertices.at(i).y() - vertices.at(i-1).y()));
        }

    }

    void fillPointVector(std::vector<pointT>& points, const oasisio::PointList& pointList) {
        int i;
        for(i=0; i<pointList.getList().size(); ++i) {
            pointT next;
            bg::set<0>(next, pointList.getList().at(i).getDeltaX() + bg::get<0>(points.back()));
            bg::set<1>(next, pointList.getList().at(i).getDeltaY() + bg::get<1>(points.back()));
            points.emplace_back(next);
        }
    }

    void readCellRecord(std::ifstream& ifs, TableOffsets& table, layout::Cell<pointT>* cell) {

        while(true) {

            unsigned int recordID = OasisReader::fromBytesUnsigned(ifs);
            std::cout << "Record ID " << recordID << std::endl;
            switch(recordID) {
            case 20:
            {
                unsigned char rectangle_info = OasisReader::fromBytesChar(ifs);
                std::cout << "Rectangle Info " << std::bitset<8>((int)rectangle_info).to_string() << std::endl;
                bool S = rectangle_info & 128;
                bool W = rectangle_info & 64;
                bool H = rectangle_info & 32;
                bool X = rectangle_info & 16;
                bool Y = rectangle_info & 8;
                bool R = rectangle_info & 4; //Currently unused
                bool D = rectangle_info & 2;
                bool L = rectangle_info & 1;
                unsigned int layernum, datatype, width, height;
                int x, y;
                if(L) {
                    layernum = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Layer Num " << layernum << std::endl;
                }
                if(D) {
                    datatype = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Datatype " << datatype << std::endl;
                }
                if(W) {
                    width = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Width " << width << std::endl;
                }
                if(H) {
                    height = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Height " << height << std::endl;
                }
                if(X) {
                    x = OasisReader::fromBytesSigned(ifs);
                    std::cout << "X " << x << std::endl;
                }
                if(Y) {
                    y = OasisReader::fromBytesSigned(ifs);
                    std::cout << "Y " << y << std::endl;
                }
                if(S) {
                    height = width;
                }

                layout::Layer<pointT>* layer = cell->getLayer(layernum, datatype);
                if(layer == nullptr) {
                    QColor red(255, 0, 0);
                    layer = cell->newLayer(layernum, datatype, red);
                    typename std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string>::const_iterator it = table.getLayernames().begin();
                    for(; it != table.getLayernames().end(); ++it) {
                        if(layernum >= std::get<0>(it->first) && layernum <= std::get<1>(it->first) &&
                            datatype >= std::get<2>(it->first) && datatype <= std::get<3>(it->first)) {
                            layer->setName(it->second);
                            break;
                        }
                    }
                }
                layout::Box<pointT>* box = new layout::Box<pointT>(x, y, x+width, y+height);
                layer->addShape(box);

                break;
            }
            case 21:
            {
                unsigned char polygon_info = OasisReader::fromBytesChar(ifs);
                std::cout << "Polygon Info " << std::bitset<8>((int)polygon_info).to_string() << std::endl;
                bool P = polygon_info & 32;
                bool X = polygon_info & 16;
                bool Y = polygon_info & 8;
                bool R = polygon_info & 4; //Currently unused
                bool D = polygon_info & 2;
                bool L = polygon_info & 1;

                unsigned int layernum, datatype;
                int x, y;
                oasisio::PointList pointList(POINT_LIST_4);
                if(L) {
                    layernum = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Layer Num " << layernum << std::endl;
                }
                if(D) {
                    datatype = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Datatype " << datatype << std::endl;
                }
                if(P) {
                    OasisReader::fromBytesPointList(ifs, pointList);
                    std::cout << "Read Point List" << std::endl;
                    int i;
                    for(i=0; i<pointList.getList().size(); ++i) {
                        std::cout << "(" << pointList.getList().at(i).getDeltaX() << ", " <<
                            pointList.getList().at(i).getDeltaY() << ") ";
                    }
                    std::cout << std::endl;
                }
                if(X) {
                    x = OasisReader::fromBytesSigned(ifs);
                    std::cout << "X " << x << std::endl;
                }
                if(Y) {
                    y = OasisReader::fromBytesSigned(ifs);
                    std::cout << "Y " << y << std::endl;
                }

                layout::Layer<pointT>* layer = cell->getLayer(layernum, datatype);
                if(layer == nullptr) {
                    QColor red(255, 0, 0);
                    layer = cell->newLayer(layernum, datatype, red);
                    typename std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string>::const_iterator it = table.getLayernames().begin();
                    for(; it != table.getLayernames().end(); ++it) {
                        if(layernum >= std::get<0>(it->first) && layernum <= std::get<1>(it->first) &&
                            datatype >= std::get<2>(it->first) && datatype <= std::get<3>(it->first)) {
                            layer->setName(it->second);
                            break;
                        }
                    }
                }

                std::vector<pointT> points;
                pointT first;
                bg::set<0>(first, x);
                bg::set<1>(first, y);
                points.emplace_back(first);
                fillPointVector(points, pointList);

                layout::Polygon<pointT>* polygon = new layout::Polygon<pointT>(points);
                layer->addShape(polygon);

                break;
            }
            case 27:
            {
                unsigned char circle_info = OasisReader::fromBytesChar(ifs);
                std::cout << "Circle Info " << std::bitset<8>((int)circle_info).to_string() << std::endl;
                bool r = circle_info & 32;
                bool X = circle_info & 16;
                bool Y = circle_info & 8;
                bool R = circle_info & 4; //Currently unused
                bool D = circle_info & 2;
                bool L = circle_info & 1;

                unsigned int radius, layernum, datatype;
                int x, y;

                if(L) {
                    layernum = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Layer Num " << layernum << std::endl;
                }
                if(D) {
                    datatype = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Datatype " << datatype << std::endl;
                }
                if(r) {
                    radius = OasisReader::fromBytesUnsigned(ifs);
                    std::cout << "Radius " << radius << std::endl;
                }
                if(X) {
                    x = OasisReader::fromBytesSigned(ifs);
                    std::cout << "X " << x << std::endl;
                }
                if(Y) {
                    y = OasisReader::fromBytesSigned(ifs);
                    std::cout << "Y " << y << std::endl;
                }

                layout::Layer<pointT>* layer = cell->getLayer(layernum, datatype);
                if(layer == nullptr) {
                    QColor red(255, 0, 0);
                    layer = cell->newLayer(layernum, datatype, red);
                    typename std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::string>::const_iterator it = table.getLayernames().begin();
                    for(; it != table.getLayernames().end(); ++it) {
                        if(layernum >= std::get<0>(it->first) && layernum <= std::get<1>(it->first) &&
                            datatype >= std::get<2>(it->first) && datatype <= std::get<3>(it->first)) {
                            layer->setName(it->second);
                            break;
                        }
                    }
                }

                pointT center;
                bg::set<0>(center, x);
                bg::set<1>(center, y);
                layer->addShape(new layout::Circle<pointT>(center, radius));

                break;
            }
            case 13:
            case 2:
            default:
                ifs.seekg(-4, ifs.cur);
                return;
            }

        }


    }


};









}


#endif // OASISFILEMANAGER_H














