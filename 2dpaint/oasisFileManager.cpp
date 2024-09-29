#include <oasisFileManager.hpp>


std::ostream& oasisio::operator<<(std::ostream& o, const oasisio::TableOffsets& to){
    o << "Cellname   : " << to.getCellnameFlag() << ", " << to.getCellnameOffset() << std::endl;
    o << "Textstring : " << to.getTextstringFlag() << ", " << to.getTextstringOffset() << std::endl;
    o << "Propname   : " << to.getPropnameFlag() << ", " << to.getPropnameOffset() << std::endl;
    o << "Propstring : " << to.getPropstringFlag() << ", " << to.getPropstringOffset() << std::endl;
    o << "Layername  : " << to.getLayernameFlag() << ", " << to.getLayernameOffset() << std::endl;
    o << "Xname      : " << to.getXnameFlag() << ", " << to.getXnameOffset();
    return o;
}

std::ostream& oasisio::operator<<(std::ostream& o, const oasisio::PointList& pl){
    int i;
    for(i=0; i<pl.getList().size()-1; ++i) {
        o << "(" << pl.getList()[i].getDeltaX() << ", " << pl.getList()[i].getDeltaY() << "), ";
    }
    o << "(" << pl.getList()[pl.getList().size()-1].getDeltaX() << ", " << pl.getList()[pl.getList().size()-1].getDeltaY() << ")";
    return o;
}

std::ostream& operator<<(std::ostream& o, const layout::dLayout& l){
    o << l.getName() << std::endl;
    for (auto const& cell : l.getCells()) {
        o << "  " << cell.first << std::endl;
        for (auto const& layer : cell.second->getLayers()) {
            o << "  " << layer.first << std::endl;
            for (const layout::iShape<layout::dPoint>* shape : layer.second->getShapes()) {
                o << "  " << shape->getVertexCount() << std::endl;
            }
        }
    }
    return o;
}
