#ifndef __LAYOUT_CELL_HPP__
#define __LAYOUT_CELL_HPP__


#include "layer.hpp"

#include <map>


namespace layout {

template<typename pointT>
class Cell {
public:
    typedef typename pointT::coord_type coord_type;
    typedef std::map<std::string, Layer<pointT>* > tLayers;

protected:
    std::string cellName;
    tLayers layers;
    Layer<pointT>* activeLayer;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    Cell(const std::string& n)
        : cellName(n)
        , activeLayer(nullptr)
    {
        layers.clear();
        bbox.makeInvalid();
    }

    virtual ~Cell() {
        for_each(getLayers().begin(), getLayers().end(),
            [] (typename decltype(layers)::value_type const& p) {
                delete p.second;
            }
        );
        layers.clear();
    }

    void print(std::string prefix) {

        std::cout << prefix << cellName << std::endl;
        typename std::map<std::string, Layer<pointT>* >::const_iterator it = layers.begin();
        for(; it != layers.end(); ++it) {
            ((Layer<pointT>*)(it->second))->print(prefix + "  ");
        }
    }

    tLayers& getLayers() {
        return layers;
    }
    const tLayers& getLayers() const {
        return layers;
    }

    const Box<pointT>& computeBBox() {
        typename tLayers::const_iterator it = getLayers().begin();
        if (it != getLayers().end()) {
            bbox = it->second->getBBox();
            ++it;
        }
        for (; it != getLayers().end(); ++it) {
            bbox.expand(it->second->getBBox());
        }
        return bbox;
    }

    const Box<pointT>& getBBox() const {
        if (!bbox.isValid()) {
            Cell<pointT>* This = const_cast<Cell*>(this);
            return This->computeBBox();
        }
        return bbox;
    }

    Layer<pointT>* newLayer(int lyrNo, int dataT, const QColor& clr) {
        std::string lyrName = Layer<pointT>::makeLayerName(lyrNo, dataT);
        typename tLayers::iterator it = layers.find(lyrName);
        if (it == layers.end()) {
            Layer<pointT>* layer = new Layer<pointT>(lyrNo, dataT, clr);
            layers[lyrName] = layer;
            activeLayer = layer;
            bbox.makeInvalid();
        } else {
            activeLayer = it->second;
        }
        return activeLayer;
    }

    Layer<pointT>* getLayer(int lyrNo, int dataT) {
        std::string lyrName = Layer<pointT>::makeLayerName(lyrNo, dataT);
        typename tLayers::iterator it = layers.find(lyrName);
        if (it == layers.end())
            return nullptr;
        else
            return it->second;
    }

    const Layer<pointT>* getLayer(int lyrNo, int dataT) const {
        std::string lyrName = Layer<pointT>::makeLayerName(lyrNo, dataT);
        typename tLayers::const_iterator it = layers.find(lyrName);
        if (it == layers.end())
            return nullptr;
        else
            return it->second;
    }

    void delLayer(int lyrNo, int dataT) {
        typename tLayers::iterator it = layers.find(Layer<pointT>::makeLayerName(lyrNo, dataT));
        if (it != layers.end()) {
            if (activeLayer == it->second)
                activeLayer = nullptr;
            delete it->second;
            layers.erase(it);
            bbox.makeInvalid();
        }
    }
    void delLayer(const std::string& name) {
        typename tLayers::iterator it = layers.find(name);
        if (it != layers.end()) {
            bbox.makeInvalid();
            if (activeLayer == it->second)
                activeLayer = nullptr;
            delete it->second;
            layers.erase(it);
            bbox.makeInvalid();
        }
    }

    Layer<pointT>* getActiveLayer() {
        return activeLayer;
    }
    const Layer<pointT>* getActiveLayer() const {
        return activeLayer;
    }
    void setActiveLayer(int lyrNo, int dataT) {
        typename tLayers::iterator it = layers.find(Layer<pointT>::makeLayerName(lyrNo, dataT));
        if (it != layers.end())
            activeLayer = it->second;
    }
    void setActiveLayer(const std::string& name) {
        typename tLayers::iterator it = layers.find(name);
        if (it != layers.end())
            activeLayer = it->second;
    }

    std::string getName() const {
        return cellName;
    }
    void setName(const std::string& name) {
        cellName = name;
    }

    int getVertexCount() const {
        int cnt = 0;
        typename tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it) {
            cnt += it->second->getVertexCount();
        }
        return cnt;
    }

    void getVerticesAndColors(std::vector<QVector3D>& vertices,
                     std::vector<QVector4D>& colors,
                     std::vector<int>& vertexCnts) const {
        printf("draw cell=%s. %d vertices...\n", getName().c_str(), getVertexCount());
        typename tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it) {
            it->second->getVerticesAndColors(vertices, colors, vertexCnts);
        }
    }
}; // class Cell

typedef Cell<dPoint> dCell;

} // namespace layout

#endif // __LAYOUT_CELL_HPP__






















