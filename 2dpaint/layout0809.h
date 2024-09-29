#ifndef LAYOUT_H
#define LAYOUT_H


#include <boost/geometry.hpp>
#include <QBrush>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <vector>
#include <sstream>
#include <string>

namespace bg = boost::geometry;

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
    using coord = pointT;
    using tBox = bg::model::box<pointT>;
    virtual tBox getBBox() = 0;

protected:
    pointT origin;

public:
    iShape(pointT o)
        : origin(o)
    {}

    pointT& getOrigin() {
        return origin;
    }
    const pointT& getOrigin() const {
        return origin;
    }

    virtual void draw(QPainter* painter) const = 0;
};



template<class pointT>
class Box : public iShape<pointT>, public bg::model::box<pointT> {
public:

    typedef typename bg::coordinate_type<pointT>::type coordT;
    using base_type = bg::model::box<pointT>;
    using bg::model::box<pointT>::max_corner;
    using bg::model::box<pointT>::min_corner;
    using tBox = typename iShape<pointT>::tBox;

    Box(pointT min, pointT max, pointT o)
        : bg::model::box<pointT>(min, max)
        , iShape<pointT>(o)
    {}


    tBox getBBox() {
        return tBox(min_corner(), max_corner());
    }

    void draw(QPainter* painter) const {
        std::cout << "drawing box...\n";
        coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
        coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
        painter->translate(c_x, c_y);

        double x1 = bg::get<0>(min_corner());
        double y1 = bg::get<1>(min_corner());
        painter->drawRect(int(x1),
                          int(y1),
                          int(bg::get<0>(max_corner())-x1),
                          int(bg::get<1>(max_corner())-y1));
    }
};

template<class pointT>
class Trapezoid : public iShape<pointT> {
protected:
    typedef typename bg::coordinate_type<pointT>::type coordT;
    using tBox = typename iShape<pointT>::tBox;

    coordT width;
    coordT height;
    int trapT;

    tBox bb;
    bool dirty = true;

public:
    Trapezoid(const pointT& o, coordT w, coordT h, int t)
        : iShape<pointT>(o)
        , width(w)
        , height(h)
        , trapT(t)
    {}

    tBox getBBox() {
        if(dirty) {
            coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
            coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
            bb = tBox(iShape<pointT>::getOrigin(), pointT(c_x + width, c_y + height));
            dirty = false;
        }
        return bb;
    }

    void draw(QPainter* painter) const {
        std::cout << "drawing trapezoid...\n";
        coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
        coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
        painter->translate(c_x, c_y);

        static const QPointF points[4] = {
            QPointF(0, 0),
            QPointF(width * 0.25, height),
            QPointF(width, height),
            QPointF(width * 0.75, height)
        };
        painter->drawPolygon(points, 4);
    }
};

template<class pointT>
class Circle : public iShape<pointT> {
protected:
    typedef typename bg::coordinate_type<pointT>::type coordT;
    using tBox = typename iShape<pointT>::tBox;
    coordT radius;

    tBox bb;
    bool dirty = true;

public:
    Circle(const pointT& o, coordT dis)
        : iShape<pointT>(o)
        , radius(dis)
    {}

    ~Circle() {
        delete bb;
    }

    tBox getBBox() {
        if(dirty) {
            coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
            coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
            bb = tBox(pointT(c_x - radius, c_y - radius), pointT(c_x + radius, c_y + radius));
            dirty = false;
        }
        return bb;
    }

    void draw(QPainter* painter) const {
        std::cout << "drawing circle...\n";
        coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
        coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
        painter->translate(c_x, c_y);
        painter->drawEllipse(bg::get<0>(-radius),
                             bg::get<1>(-radius), radius*2, radius*2);
    }
}; // class Circle



template<class pointT>
class Polygon : public iShape<pointT>, public bg::model::polygon<pointT> {
public:
    typedef typename bg::coordinate_type<pointT>::type coordT;
    using bg::model::polygon<pointT>::polygon;
    using bg::model::polygon<pointT>::outer;
    using tBox = bg::model::box<pointT>;

protected:
    tBox bb;
    bool dirty = true;

public:
    Polygon(std::vector<pointT> points, pointT o)
        : bg::model::polygon<pointT>::polygon(points)
        , iShape<pointT>(o)
    {}

    tBox getBBox() {
        if(dirty){
            bg::envelope(this, bb);
            dirty = false;
        }
        return bb;
    }

    ~Polygon() {
        delete bb;
    }

    void draw(QPainter* painter) const {
        std::cout << "drawing polygon...\n";
        coordT c_x = bg::get<0>(iShape<pointT>::getOrigin());
        coordT c_y = bg::get<1>(iShape<pointT>::getOrigin());
        painter->translate(c_x, c_y);

        QPointF points[outer().size()];
        int i;
        for(i=0; i<outer().size(); ++i) {
            pointT next = outer()[i];
            points[i] = QPointF(bg::get<0>(next), bg::get<1>(next));
        }
        painter->drawPolygon(points, outer().size());
    }
}; // class Polygon


template<class pointT>
class Layer {
public:
    using tBox = bg::model::box<pointT>;
    typedef typename bg::coordinate_type<pointT>::type coordT;
    using tShapes = std::vector< iShape<pointT>* >;

protected:
    tShapes m_s;
    int layerNum;
    int dataT;

    tBox bb;
    bool hasBB = false;

    QColor lyrColor;

public:
    Layer(int n, const int s)
        : layerNum(n)
        , dataT(s)
    {}

    ~Layer() {
        int i;
        for(i = 0; i < m_s.size(); i++) {
            delete m_s[i];
        }
    }

    tShapes getShapes() {
        return m_s;
    }

    const tShapes getShapes() const {
        return m_s;
    }

    void addShape(iShape<pointT>* shape) {
        m_s.push_back(shape);
        updateBBox(shape->getBBox());
    }

    tBox getBBox() {
        return bb;
    }

    void updateBBox(const tBox& b) {

        coordT min_x = bg::get<bg::min_corner, 0>(b);
        coordT min_y = bg::get<bg::min_corner, 1>(b);
        coordT max_x = bg::get<bg::max_corner, 0>(b);
        coordT max_y = bg::get<bg::max_corner, 1>(b);

        if(!hasBB) {

            bb = tBox(pointT(min_x, min_y), pointT(max_x, max_y));
            hasBB = true;

        } else {

            coordT bb_min_x = bg::get<bg::min_corner, 0>(bb);
            coordT bb_min_y = bg::get<bg::min_corner, 1>(bb);
            coordT bb_max_x = bg::get<bg::max_corner, 0>(bb);
            coordT bb_max_y = bg::get<bg::max_corner, 1>(bb);

            bg::set<bg::min_corner, 0>(bb, min(min_x, bb_min_x));
            bg::set<bg::min_corner, 1>(bb, min(min_y, bb_min_y));
            bg::set<bg::max_corner, 0>(bb, max(max_x, bb_max_x));
            bg::set<bg::max_corner, 1>(bb, max(max_y, bb_max_y));

        }

    }

    /*
    tBox initialBBox(const std::vector< iShape<pointT> >& shapes) {
        coordT min_x = bg::get<bg::min_corner, 0>(shapes.front().getBBox());
        coordT min_y = bg::get<bg::min_corner, 1>(shapes.front().getBBox());
        coordT max_x = bg::get<bg::max_corner, 0>(shapes.front().getBBox());
        coordT max_y = bg::get<bg::max_corner, 1>(shapes.front().getBBox());

        int i;
        for(i = 1; i < shapes.size(); i++) {
            min_x = min(min_x, shapes.at(i).getBBox().min_corner().getX());
            min_y = min(min_y, shapes.at(i).getBBox().min_corner().getY());
            max_x = max(max_x, shapes.at(i).getBBox().max_corner().getX());
            max_y = max(max_y, shapes.at(i).getBBox().max_corner().getY());
        }

        hasBB = true;

        return tBox(pointT(min_x, min_y), pointT(max_x, max_y));
    }
    */

    std::string getName() {
        std::stringstream ss;

        ss << layerNum << ":" << dataT;
        return ss.str();
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
}; // class Layer


template<class pointT>
class Cell {
protected:
    std::string name;

    using tLayers = std::unordered_map<std::string, Layer<pointT>* >;
    tLayers m_l;
    Layer<pointT>* activeLayer;

public:
    Cell(const std::string& n)
        : name(n)
    {}

    ~Cell() {
        for_each(m_l.begin(), m_l.end(), [] (typename decltype(m_l)::value_type const& p) { delete p.second; });
        m_l.clear();
    }

    tLayers getLayers() {
        return m_l;
    }

    const tLayers getLayers() const {
        return m_l;
    }

    Layer<pointT>* newLayer(int i, const std::string& n) {
        Layer<pointT>* layer = new Layer<pointT>(i, n);
        m_l[n] = layer;
        activeLayer = layer;
        return layer;
    }


    Layer<pointT>* getLayer(const std::string& n) {
        return m_l[n];
    }

    const Layer<pointT>* getLayer(const std::string& n) const {
        return m_l[n];
    }

    void delLayer(const std::string& n) {
        m_l.erase(n);
    }

    Layer<pointT>* getActiveLayer() {
        return activeLayer;
    }
    const Layer<pointT>* getActiveLayer() const {
        return activeLayer;
    }
    void setActiveLayer(const std::string& n) {
        activeLayer = n;
    }

    std::string getName() {
        return name;
    }

    void setName(const std::string& n) {
        name = n;
    }

    void draw(QPainter* painter) const {
        const Layer<pointT>* lyr = getActiveLayer();
        if (lyr) {
            lyr->draw(painter);
        }
    }
}; // class Cell


template<class pointT>
class Layout {
protected:
    std::string name;

    using tCells = std::unordered_map<std::string, Cell<pointT>* >;
    tCells m_c;
    Cell<pointT>* activeCell;

public:
    Layout(const std::string& n)
        : name(n)
    {}

    ~Layout() {
        for_each(m_c.begin(), m_c.end(), [] (typename decltype(m_c)::value_type const& p) { delete p.second; });
        m_c.clear();
    }

    tCells getCells() {
        return m_c;
    }

    const tCells getCells() const {
        return m_c;
    }

    Cell<pointT>* newCell(const std::string& n) {
        Cell<pointT>* cell = new Cell<pointT>(n);
        m_c[n] = cell;
        activeCell = cell;
        return cell;
    }

    Cell<pointT>* getCell(const std::string& n) {
        activeCell = m_c[n];
        return m_c[n];
    }

    const Cell<pointT>* getCell(const std::string& n) const {
        activeCell = m_c[n];
        return m_c[n];
    }

    void delCell(const std::string& n) {
        m_c.erase(n);
    }

    Cell<pointT>* getActiveCell() {
        return activeCell;
    }
    const Cell<pointT>* getActiveCell() const {
        return activeCell;
    }
    void setActiveCell(const std::string& n) {
        activeCell = m_c[n];
    }

    std::string getName() {
        return name;
    }

    void setName(const std::string& n) {
        name = n;
    }

    void draw(QPainter* painter) const {
        const Cell<pointT>* cell = getActiveCell();
        if (cell) {
            cell->draw(painter);
        }
    }
}; // class Layout


template<class pointT>
class LayoutManager {
protected:
    using tLayouts = std::unordered_map<std::string, Layout<pointT>* >;
    tLayouts m_ls;
    Layout<pointT>* activeLayout;

public:
    LayoutManager()
    {}

    ~LayoutManager() {
        for_each(m_ls.begin(), m_ls.end(), [] (typename decltype(m_ls)::value_type const& p) { delete p.second; });
        m_ls.clear();
    }

    Layout<pointT>* newLayout(const std::string& n) {
        Layout<pointT>* layout = new Layout<pointT>(n);
        m_ls[n] = layout;
        activeLayout = layout;
        return layout;
    }

    tLayouts& getLayouts() {
        return m_ls;
    }

    const tLayouts& getLayouts() const {
        return m_ls;
    }

    void setActiveLayout(const std::string& n) {
        typename tLayouts::iterator cit = m_ls.find(n);
        if(cit != m_ls.end()) {
            activeLayout = cit->second;
        }
    }

    Layout<pointT>* getActiveLayout() {
        return activeLayout;
    }
    const Layout<pointT>* getActiveLayout() const {
        return activeLayout;
    }

    void delLayout(const std::string& n) {
        m_ls.erase(n);
    }

    Layout<pointT>* getLayout(const std::string& n) {
        activeLayout = m_ls[n];
        return m_ls[n];
    }

    const Layout<pointT>* getLayout(const std::string& n) const {
        activeLayout = m_ls[n];
        return m_ls[n];
    }

    void draw(QPainter* painter) const {
        const Layout<pointT>* lyt = getActiveLayout();
        if (lyt) {
            lyt->draw(painter);
        }
    }

}; // class LayoutManager

template<class pointT>
void initializeManager(LayoutManager<pointT>& layMan) {

    Layout<pointT>* a = layMan.newLayout("A");
    Layout<pointT>* b = layMan.newLayout("B");

    Cell<pointT>* a_top = a->newCell("top");
    Cell<pointT>* b_top = b->newCell("top");

    Layer<pointT>* a_0 = a_top->newLayer(0, "0");
    Layer<pointT>* a_1 = a_top->newLayer(1, "0");

    Layer<pointT>* b_0 = b_top->newLayer(0, "0");
    Layer<pointT>* b_1 = b_top->newLayer(1, "0");

    a_0->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 0.0)) );
    a_0->addShape( new Trapezoid<pointT>(pointT(1.0, 0.0), 1.0, 2.0, 3) );
    a_0->addShape( new Circle<pointT>(pointT(3.0, 0.0), 1.0) );

    a_1->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 10.0)) );
    a_1->addShape( new Trapezoid<pointT>(pointT(1.0, 10.0), 1.0, 2.0, 3) );
    a_1->addShape( new Circle<pointT>(pointT(3.0, 10.0), 1.0) );

    b_0->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 20.0)) );
    b_0->addShape( new Trapezoid<pointT>(pointT(1.0, 20.0), 1.0, 2.0, 3) );
    b_0->addShape( new Circle<pointT>(pointT(3.0, 20.0), 1.0) );

    b_1->addShape( new Box<pointT>(pointT(0.0, 0.0), pointT(1.0, 2.0), pointT(0.0, 30.0)) );
    b_1->addShape( new Trapezoid<pointT>(pointT(1.0, 30.0), 1.0, 2.0, 3) );
    b_1->addShape( new Circle<pointT>(pointT(3.0, 30.0), 1.0) );

}

typedef bg::model::point<double, 2, bg::cs::cartesian> dPoint;
typedef Box<dPoint> dBox;
typedef Polygon<dPoint> dPolygon;
typedef Layer<dPoint> dLayer;
typedef Cell<dPoint> dCell;
typedef Layout<dPoint> dLayout;
typedef LayoutManager<dPoint> dLayoutManager;

}

#endif // LAYOUT_H






















