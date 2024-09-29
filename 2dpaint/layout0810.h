#ifndef __LAYOUT_H__
#define __LAYOUT_H__


#include <boost/geometry.hpp>
#ifdef USE_QPAINTER
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QPoint>
#else
#include <QColor>
#include <QVector2D>
#include <QVector3D>
#endif

#include <vector>
#include <sstream>
#include <string>

namespace bg = boost::geometry;

#if 0 // directly use QVector3D instead.
struct VertexData
{
    QVector3D position;
    //QVector2D texCoord;
};
#endif

namespace layout {

/*
template<class T>
class Point : public bg::model::point<T, 2, bg::cs::cartesian>{
public:
    using bg::model::point<T, 2, bg::cs::cartesian>::point;
    using type = T;
};
*/


template<class pointT>
class iShape {
public:
    typedef typename bg::coordinate_type<pointT>::type coordT;
    typedef bg::model::box<pointT> tBox;

public:
    virtual tBox getBBox() = 0;

#ifdef _USE_QPAINTER
    virtual void draw(QPainter* painter) const = 0;
#else
    virtual int getVertexCount() const = 0;
    virtual void getVertices(QVector3D* vertices) const = 0;
#endif

};


template<class pointT>
class Box : public iShape<pointT>, public bg::model::box<pointT> {
public:
    typedef bg::model::box<pointT> base_type;
    using tBox = typename iShape<pointT>::tBox;
    using bg::model::box<pointT>::max_corner;
    using bg::model::box<pointT>::min_corner;

    Box(const pointT& min, const pointT& max)
        : base_type(min, max)
    {}

    virtual ~Box() {}

    tBox getBBox() {
        return *this;
    }

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        double x1 = bg::get<0>(min_corner());
        double y1 = bg::get<1>(min_corner());
        painter->drawRect(int(x1),
                          int(y1),
                          int(bg::get<0>(max_corner())-x1),
                          int(bg::get<1>(max_corner())-y1));
    }
#else
    int getVertexCount() const {
        return 4;
    }
    void getVertices(QVector3D* vertices) const {
        printf("draw Box 4 vertices...\n");
        *vertices = QVector3D(bg::get<0>(min_corner()),
                              bg::get<1>(min_corner()), 1.0f);
        *(vertices+1) = QVector3D(bg::get<0>(max_corner()),
                                  bg::get<1>(min_corner()), 1.0f);
        *(vertices+2) = QVector3D(bg::get<0>(max_corner()),
                                  bg::get<1>(max_corner()), 1.0f);
        *(vertices+3) = QVector3D(bg::get<0>(min_corner()),
                                  bg::get<1>(max_corner()), 1.0f);
        vertices += 4;
    }
#endif
};

template<class pointT>
class Trapezoid : public iShape<pointT> {
public:
    typedef iShape<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    pointT p0, p1, p2, p3;
    int trapT;
    coordT width = 0;
    coordT height = 0;

    tBox bb;
    bool dirty = true;

public:
    Trapezoid(const pointT& _p0, const pointT& _p1,
              const pointT& _p2, const pointT& _p3, int type)
        : p0(_p0), p1(_p1), p2(_p2), p3(_p3), trapT(type)
    {
        if (trapT == 0) {
            width = bg::get<0>(p1) - bg::get<0>(p0);
            height = bg::get<1>(p2) - bg::get<1>(p1);
        }
    }

    virtual ~Trapezoid() {}

    tBox getBBox() {
        if(dirty) {
            coordT x = bg::get<0>(p0);
            coordT y = bg::get<1>(p0);
            bg::set<0>(bb.min_corner(), x);
            bg::set<1>(bb.min_corner(), y);
            bg::set<0>(bb.max_corner(), x+width);
            bg::set<1>(bb.max_corner(), y+height);
            dirty = false;
        }
        return bb;
    }

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        if (trapT == 0) {
            std::cout << "drawing trapezoid...\n";
            static const QPointF points[4] = {
                QPointF(bg::get<0>(p0), bg::get<1>(p0)),
                QPointF(bg::get<0>(p1), bg::get<1>(p1)),
                QPointF(bg::get<0>(p2), bg::get<1>(p2)),
                QPointF(bg::get<0>(p3), bg::get<1>(p3)),
            };
        }
        painter->drawPolygon(points, 4);
    }
#else
    int getVertexCount() const {
        return 4;
    }
    void getVertices(QVector3D* vertices) const {
        printf("draw Trapezoid: 4 vertices...\n");
        if (trapT == 0) {
            *vertices = QVector3D(bg::get<0>(p0), bg::get<1>(p0), 1.0f);
            *(vertices+1) = QVector3D(bg::get<0>(p1), bg::get<1>(p1), 1.0f);
            *(vertices+2) = QVector3D(bg::get<0>(p1), bg::get<1>(p1), 1.0f);
            *(vertices+3) = QVector3D(bg::get<0>(p1), bg::get<1>(p1), 1.0f);
        }
        vertices += 4;
    }

#endif
}; // class Trapezoid

template<class pointT>
class Circle : public iShape<pointT> {
public:
    typedef iShape<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    pointT center;
    coordT radius;
    tBox bb;
    bool dirty = true;

public:
    Circle(const pointT& c, coordT r)
        : center(c), radius(r)
    {}

    virtual ~Circle() {}

    tBox getBBox() {
        if(dirty) {
            coordT c_x = bg::get<0>(center);
            coordT c_y = bg::get<1>(center);
            bg::set<0>(bb.min_corner(), c_x - radius);
            bg::set<1>(bb.min_corner(), c_y - radius);
            bg::set<0>(bb.max_corner(), c_x + radius);
            bg::set<1>(bb.max_corner(), c_y + radius);
            dirty = false;
        }
        return bb;
    }

    int getVertexCount() const {
        return 256;
    }
    void getVertices(QVector3D* vertices) const {
        printf("draw Circle: 256 vertices...\n");
        // approximate the circle with 256-vertex ring
    }
}; // class Circle



template<class pointT>
class Polygon : public iShape<pointT>, public bg::model::polygon<pointT> {
public:
    typedef typename bg::model::polygon<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    tBox bb;
    bool dirty = true;

public:
    Polygon(std::vector<pointT>& points) {
        typename std::vector<pointT>::iterator it = points.begin();
        for (; it != points.end(); ++it) {
            bg::append(this->outer(), *it);
        }
    }

    tBox getBBox() {
        if(dirty){
            bg::envelope(this, bb);
            dirty = false;
        }
        return bb;
    }

    virtual ~Polygon() {}

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        std::cout << "drawing ring...\n";
        QPointF points[bg::num_points(*this)];
        bg::for_each_point(*this,
                           [&painter, &points](const pointT& pt) {
                                points[i] = QPointF(bg::get<0>(pt), bg::get<1>(pt));
                           });
        painter->drawPolygon(points, bg::num_points(*this));
    }
#else
    int getVertexCount() const {
        int cnt = bg::num_points(*this);
        return cnt;
    }

    void getVertices(QVector3D* vertices) const {
        printf("draw Ring: %d vertices...\n", getVertexCount());
        bg::for_each_point(*this,
                           [&vertices](const pointT& pt) {
                               *(vertices++) = QVector3D(bg::get<0>(pt), bg::get<1>(pt), 1.0f);
                           });
    }
#endif
}; // class Ring


template<class pointT>
class Layer {
public:
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;
    using tShapes = std::vector< iShape<pointT>* >;

protected:
    tShapes shapes;
    int layerNum;
    int dataT;
    tBox bb;
    QColor lyrColor;
    int vertexCnt;

public:
    Layer(int n, int d, const QColor& clr)
        : layerNum(n)
        , dataT(d)
        , lyrColor(clr)
        , vertexCnt(0)
    {
    }

    virtual ~Layer() {
        int i;
        for(i = 0; i < shapes.size(); ++i) {
            delete shapes[i];
        }
    }

    tShapes& getShapes() {
        return shapes;
    }

    const tShapes& getShapes() const {
        return shapes;
    }

    void addShape(iShape<pointT>* shape) {
        shapes.push_back(shape);
        vertexCnt += shape->getVertexCount();
        // delay updating bbox until getBBox().
    }

    void deleteShape(iShape<pointT>* shape) {
        typename tShapes::iterator it = std::find(shapes.begin(), shapes.end(), shape);
        if (it != shapes.end()) {
            vertexCnt -= (*it)->getVertexCount();
            shapes.erase(it);
        }
    }
    const tBox& getBBox() {
        if (shapes.size() > 0) {
            typename tShapes::const_iterator it = shapes.begin();
            bb = (*(it++))->envelop();
            for (; it != shapes.end(); ++it) {
                bg::expand(bb, (*it)->getBBox());
            }
        } else {
            bg::set<0>(bb.min_corner(), 0.0);
            bg::set<1>(bb.min_corner(), 0.0);
            bg::set<0>(bb.max_corner(), 0.0);
            bg::set<1>(bb.max_corner(), 0.0);
        }
        return bb;
    }

    static std::string makeLayerName(int lyrNo, int dataT) {
        std::stringstream ss;
        ss << lyrNo << ":" << dataT;
        return ss.str();
    }

    std::string getName() {
        return makeLayerName(layerNum, dataT);
    }

    void setName(const std::string& name) {
        std::size_t pos = name.find_first_of(":");
        layerNum = std::stoi(name.substr(0, pos));
        dataT = std::stoi(name.substr(pos+1));
    }

    QColor getColor() const {
        return lyrColor;
    }
    void setColor(const QColor& color) {
        lyrColor = color;
    }

#ifdef USE_QPAINTER
    QBrush getBrush() const {
        return QBrush(lyrColor);
    }
    QPen getPen() const {
        return QPen(getBrush(), 1);
    }

    void draw(QPainter* painter) const {
        painter->save();
        painter->setBrush(getBrush());
        painter->setPen(getPen());
        typename tShapes::const_iterator sit = getShapes().begin();
        for (; sit != getShapes().end(); ++sit) {
            (*sit)->draw(painter);
        }
        painter->restore();
    }
#else
    int getVertexCount() const {
        return vertexCnt;
    }

    void getVertices(QVector3D* vertices) const {
        printf("draw Layer: %d vertices...\n", getVertexCount());
        typename tShapes::const_iterator it = shapes.begin();
        for (; it != shapes.end(); ++it) {
            (*it)->getVertices(vertices);
        }
    }
#endif
}; // class Layer


template<class pointT>
class Cell {
protected:
    std::string name;

    using tLayers = std::unordered_map<std::string, Layer<pointT>* >;
    tLayers layers;
    Layer<pointT>* activeLayer;

public:
    Cell(const std::string& n)
        : name(n)
        , activeLayer(nullptr)
    {}

    virtual ~Cell() {
        for_each(layers.begin(), layers.end(),
                 [] (typename decltype(layers)::value_type const& p) { delete p.second; });
        layers.clear();
    }

    tLayers getLayers() {
        return layers;
    }

    const tLayers getLayers() const {
        return layers;
    }

    Layer<pointT>* newLayer(int lyrNo, int dataT, const QColor& clr) {
        std::string lyrName = Layer<pointT>::makeLayerName(lyrNo, dataT);
        typename tLayers::iterator it = layers.find(lyrName);
        if (it == layers.end()) {
          Layer<pointT>* layer = new Layer<pointT>(lyrNo, dataT, clr);
          layers[lyrName] = layer;
          activeLayer = layer;
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

    std::string getName() const {
        return name;
    }

    void setName(const std::string& _name) {
        name = _name;
    }

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it)
            it->second->draw(painter);
        }
    }
#else
    int getVertexCount() const {
        int cnt = 0;
        typename tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it) {
            cnt += it->second->getVertexCount();
        }
        return cnt;
    }
    void getVertices(QVector3D* vertices) const {
        printf("draw cell=%s. %d vertices...\n", getName().c_str(), getVertexCount());
        typename tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it) {
            it->second->getVertices(vertices);
        }
    }
#endif
}; // class Cell


template<class pointT>
class Layout {
public:
    using tCells = std::unordered_map<std::string, Cell<pointT>* >;

protected:
    std::string name;
    tCells cells;
    Cell<pointT>* activeCell;

public:
    Layout(const std::string& n)
        : name(n)
    {}

    virtual ~Layout() {
        for_each(cells.begin(), cells.end(),
                 [] (typename decltype(cells)::value_type const& p) { delete p.second; });
        cells.clear();
    }

    tCells getCells() {
        return cells;
    }

    const tCells getCells() const {
        return cells;
    }

    Cell<pointT>* newCell(const std::string& name) {
        Cell<pointT>* cell = new Cell<pointT>(name);
        cells[name] = cell;
        activeCell = cell;
        return cell;
    }

    Cell<pointT>* getCell(const std::string& name) {
        typename tCells::iterator it = cells.find(name);
        if (it != cells.end())
            return it->second;
        return nullptr;
    }

    const Cell<pointT>* getCell(const std::string& name) const {
        typename tCells::const_iterator it = cells.find(name);
        if (it != cells.end())
            return it->second;
        return nullptr;
    }

    void delCell(const std::string& name) {
        typename tCells::iterator it = cells.find(name);
        if (it != cells.end()) {
            if (activeCell == it->second)
                activeCell = nullptr;
            delete it->secind;
            cells.erase(it);
        }
    }

    Cell<pointT>* getActiveCell() {
        return activeCell;
    }
    const Cell<pointT>* getActiveCell() const {
        return activeCell;
    }
    void setActiveCell(const std::string& name) {
        typename tCells::iterator it = cells.find(name);
        // only set activeCell if we find cell by name.
        if (it != cells.end())
            activeCell = it->second;
        else
            activeCell = nullptr;
    }

    std::string getName() const {
        return name;
    }

    void setName(const std::string& _name) {
        name = _name;
    }

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        const Cell<pointT>* cell = getActiveCell();
        if (cell) {
            cell->draw(painter);
        }
    }
#else
    int getVertexCount() const {
        int cnt = 0;
        typename tCells::const_iterator it = cells.begin();
        for (; it != cells.end(); ++it) {
            cnt += it->second->getVertexCount();
        }
        return cnt;
    }
    void getVertices(QVector3D* vertices) const {
        printf("draw layout=%s. %d vertices...", getName().c_str(), getVertexCount());
        typename tCells::const_iterator it = cells.begin();
        for (; it != cells.end(); ++it) {
            it->second->getVertices(vertices);
        }
    }
#endif
}; // class Layout


template<class pointT>
class LayoutManager {
public:
    using tLayouts = std::unordered_map<std::string, Layout<pointT>* >;

protected:
    tLayouts layouts;
    Layout<pointT>* activeLayout;

public:
    LayoutManager()
        : activeLayout(nullptr)
    {}

    ~LayoutManager() {
        for_each(layouts.begin(), layouts.end(),
                 [] (typename decltype(layouts)::value_type const& p) { delete p.second; });
        layouts.clear();
    }

    Layout<pointT>* newLayout(const std::string& name) {
        Layout<pointT>* layout = new Layout<pointT>(name);
        layouts[name] = layout;
        activeLayout = layout;
        return layout;
    }

    tLayouts& getLayouts() {
        return layouts;
    }

    const tLayouts& getLayouts() const {
        return layouts;
    }

    void setActiveLayout(const std::string& name) {
        typename tLayouts::iterator it = layouts.find(name);
        if (it != layouts.end()) {
            activeLayout = it->second;
        } else {
            activeLayout = nullptr;
        }
    }

    Layout<pointT>* getActiveLayout() {
        return activeLayout;
    }
    const Layout<pointT>* getActiveLayout() const {
        return activeLayout;
    }

    void delLayout(const std::string& name) {
        typename tLayouts::iterator it = layouts.find(name);
        if (it != layouts.end()) {
            if (activeLayout == it->second)
                activeLayout = nullptr;
            delete it->second;
            layouts.erase(it);
        }
    }

    Layout<pointT>* getLayout(const std::string& name) {
        typename tLayouts::iterator it = layouts.find(name);
        if (it != layouts.end())
            return it->second;
        return nullptr;
    }

    const Layout<pointT>* getLayout(const std::string& name) const {
        typename tLayouts::const_iterator it = layouts.find(name);
        if (it != layouts.end())
            return it->second;
        return nullptr;
    }

#ifdef USE_QPAINTER
    void draw(QPainter* painter) const {
        const Layout<pointT>* lyt = getActiveLayout();
        if (lyt) {
            lyt->draw(painter);
        }
    }
#else
    int getVertexCount() const {
        if (activeLayout)
            return activeLayout->getVertexCount();
        return 0;
    }
    void getVertices(QVector3D* vertices) const {
        if (activeLayout) {
            activeLayout->getVertices(vertices);
        }
    }
#endif

}; // class LayoutManager

typedef bg::model::point<double, 2, bg::cs::cartesian> dPoint;
typedef Box<dPoint> dBox;
typedef Trapezoid<dPoint> dTrapezoid;
typedef Circle<dPoint> dCircle;
typedef Ring<dPoint> dRing;
typedef Layer<dPoint> dLayer;
typedef Cell<dPoint> dCell;
typedef Layout<dPoint> dLayout;
typedef LayoutManager<dPoint> dLayoutManager;

void initializeManager(dLayoutManager& layM);

} // namespace layout

#endif // __LAYOUT_H__






















