#ifndef __LAYOUT_LAYER_HPP__
#define __LAYOUT_LAYER_HPP__


#include "box.hpp"

#include <unordered_set>
#include <string>
#include <QColor>


namespace layout {

template<typename pointT>
class Layer {
public:
    typedef typename pointT::coord_type coord_type;
    typedef std::unordered_set<iShape<pointT>*> tShapes;

protected:
    tShapes shapes;
    int layerNum;
    int dataT;
    QColor lyrColor;
    std::string layerName;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};
    int vertexCnt;

public:
    Layer(int n, int d, const QColor& clr)
        : layerNum(n)
        , dataT(d)
        , lyrColor(clr)
        , layerName(makeLayerName(n, d))
        , vertexCnt(0) {
    }

    virtual ~Layer() {
        typename tShapes::iterator it = getShapes().begin();
        for(; it != getShapes().end(); ++it) {
            delete *it;
        }
    }

    void print(std::string prefix) {

        std::cout << prefix << layerName << std::endl;
        typename std::unordered_set<iShape<pointT>*>::const_iterator it = shapes.begin();
        for(; it != shapes.end(); ++it) {
            (*it)->print(prefix + "  ");
        }
    }

    tShapes& getShapes() {
        return shapes;
    }
    const tShapes& getShapes() const {
        return shapes;
    }

    int getLayerNum() const {
        return layerNum;
    }
    int setLayerNum(int n) {
        layerNum = n;
    }

    int getDataType() const {
        return dataT;
    }
    void setDataType(int type) {
        dataT = type;
    }

    static std::string makeLayerName(int lyrNo, int dataT) {
        std::stringstream ss;
        ss << lyrNo << ":" << dataT;
        return ss.str();
    }

    std::string getName() const {
        if (layerName.size() == 0)
            return makeLayerName(layerNum, dataT);
        return layerName;
    }

    void setName(const std::string& name) {
        layerName = name;
        std::size_t pos = name.find_first_of(":");
        layerNum = std::stoi(name.substr(0, pos));
        dataT = std::stoi(name.substr(pos+1));
    }

    void addShape(iShape<pointT>* shape) {
        getShapes().insert(shape);
        vertexCnt += shape->getVertexCount();
        bbox.makeInvalid();
    }

    void deleteShape(iShape<pointT>* shape) {
        typename tShapes::iterator it = getShapes().find(shape);
        if (it != getShapes().end()) {
            vertexCnt -= (*it)->getVertexCount();
            delete *it;
            getShapes().erase(it);
            bbox.makeInvalid();
        }
    }

    const Box<pointT>& computeBBox() {
        bbox.makeInvalid();
        typename tShapes::const_iterator it = getShapes().begin();
        if (it != getShapes().end()) {
            bbox = (*it)->getBBox();
            ++it;
            for (; it != getShapes().end(); ++it) {
                bbox.expand((*it)->getBBox());
            }
        }
        return bbox;
    }
    const Box<pointT>& getBBox() const {
        if (!bbox.isValid()) {
            Layer<pointT>* This = const_cast<Layer<pointT>*>(this);
            return This->computeBBox();
        }
        return bbox;
    }

    QColor getColor() const {
        return lyrColor;
    }
    void setColor(const QColor& color) {
        lyrColor = color;
    }

    int getVertexCount() const {
        return vertexCnt;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw Layer %s: %d vertices...\n", getName().c_str(), getVertexCount());
        typename tShapes::const_iterator it = getShapes().begin();
        for (; it != getShapes().end(); ++it) {
            (*it)->getVertices(vertices, vertexCnts);
        }
    }
}; // class Layer

typedef Layer<dPoint> dLayer;

} // namespace layout

#endif // __LAYOUT_LAYER_HPP__






















