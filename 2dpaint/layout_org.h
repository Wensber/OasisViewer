#ifndef __LAYOUT_H__
#define __LAYOUT_H__


#include <boost/geometry.hpp>

#include <QColor>
#include <QVector2D>
#include <QVector3D>

#include <vector>
#include <sstream>
#include <string>

namespace bg = boost::geometry;

extern const double PI;
extern const int circle_vertex_count;
extern float pointZval;

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
    /// construct shape's bounding box.
    virtual const tBox& computeBBox() = 0;
    /// return shape's bounding box.
    virtual const tBox& getBBox() const = 0;
    /// return shape's vertex count.
    virtual int getVertexCount() const = 0;
    /// collect all shapes' verices and each shape's vertex count. returns the # of shapes.
    virtual void getVertices(std::vector<QVector3D>& vertices,
                             std::vector<int>& vertexCnts) const = 0;

    static void makeBoxInvalid(tBox& box) {
        bg::set<0>(box.min_corner(), 0.0);
        bg::set<1>(box.min_corner(), 0.0);
        bg::set<0>(box.max_corner(), 0.0);
        bg::set<1>(box.max_corner(), 0.0);
    }

    static bool isBoxValid(const tBox& box) {
        return (bg::get<0>(box.max_corner()) - bg::get<0>(box.min_corner())) != 0 ||
               (bg::get<1>(box.max_corner()) - bg::get<1>(box.min_corner())) != 0;
    }
};


template<class pointT>
class Box : public iShape<pointT>, public bg::model::box<pointT> {
public:
    typedef boost::geometry::box_tag type;
    typedef bg::model::box<pointT> base_type;
    typedef typename bg::coordinate_type<pointT>::type coordT;
    using tBox = typename iShape<pointT>::tBox;
    using bg::model::box<pointT>::max_corner;
    using bg::model::box<pointT>::min_corner;

    Box()
        : base_type() {
    }

    Box(const pointT& min, const pointT& max)
        : base_type(min, max) {
    }

    Box(coordT minx, coordT miny, coordT maxx, coordT maxy)
        : base_type() {
        bg::set<0>(min_corner(), minx);
        bg::set<1>(min_corner(), miny);
        bg::set<0>(max_corner(), maxx);
        bg::set<1>(max_corner(), maxy);
    }

    virtual ~Box() {}

    coordT getMinX() const {
        return bg::get<0>(min_corner());
    }
    void setMinX(coordT x) {
        bg::set<0>(min_corner(), x);
    }
    coordT getMaxX() const {
        return bg::get<0>(max_corner());
    }
    void setMaxX(coordT x) {
        bg::set<0>(max_corner(), x);
    }

    coordT getMinY() const {
        return bg::get<1>(min_corner());
    }
    void setMinY(coordT y) {
        bg::set<1>(min_corner(), y);
    }
    coordT getMaxY() const {
        return bg::get<1>(max_corner());
    }
    void setMaxY(coordT y) {
        bg::set<1>(max_corner(), y);
    }

    Box& operator=(const tBox& box) {
        setMinX(bg::get<0>(box.min_corner()));
        setMinY(bg::get<1>(box.min_corner()));
        setMaxX(bg::get<0>(box.max_corner()));
        setMaxY(bg::get<1>(box.max_corner()));
        return *this;
    }

    const tBox& computeBBox() {
        // nothing to do.
        return *this;
    }
    const tBox& getBBox() const {
        return *this;
    }

    const coordT getWidth() const {
        return getMaxX() - getMinX();
    }

    const coordT getHeight() const {
        return getMaxY() - getMinY();
    }

    int getVertexCount() const {
        return 4;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                    std::vector<int>& vertexCnts) const {
        printf("draw Box 4 vertices...\n");
        vertices.emplace_back(getMinX(), getMinY(), pointZval);
        vertices.emplace_back(getMaxX(), getMinY(), pointZval);
        vertices.emplace_back(getMaxX(), getMaxY(), pointZval);
        vertices.emplace_back(getMinX(), getMaxY(), pointZval);
        vertexCnts.emplace_back(4);
    }
};

template<class pointT>
class Trapezoid : public iShape<pointT> {
public:
    typedef boost::geometry::polygon_tag type;
    typedef iShape<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    pointT p0, p1;
    int tType;
    coordT length0 = 0;
    coordT length1 = 0;
    tBox bbox;

public:
    Trapezoid(const pointT& _p0, const pointT& _p1, coordT _length0, coordT _length1, int _type)
        : p0(_p0), p1(_p1)
        ,length0(_length0), length1(_length1)
        , tType(_type) {
        computeBBox();
    }

    virtual ~Trapezoid() {}

    /// compute bounding box. non const version
    const tBox& computeBBox() {
        if (tType == 0) {
            coordT x0 = bg::get<0>(p0);
            coordT y0 = bg::get<1>(p0);
            coordT x1 = bg::get<0>(p1);
            coordT y1 = bg::get<1>(p1);
            bg::set<0>(bbox.min_corner(), x0);
            bg::set<1>(bbox.min_corner(), y0);
            bg::set<0>(bbox.max_corner(),
                       std::max(x0+length0, x1+length1));
            bg::set<1>(bbox.max_corner(),
                       std::max(y0+length0, y1+length1));
        }
        return bbox;
    }
    /// return bounding box. const version.
    const tBox& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return 4;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw Trapezoid: 4 vertices...\n");
        if (tType == 0) {
            coordT x = bg::get<0>(p0);
            coordT y = bg::get<1>(p0);
            vertices.emplace_back(x, y, pointZval);
            vertices.emplace_back(x+length0, y, pointZval);
            x = bg::get<0>(p1);
            y = bg::get<1>(p1);
            vertices.emplace_back(x+length1, y, pointZval);
            vertices.emplace_back(x, y, pointZval);
            vertexCnts.emplace_back(4);
        }
    }

}; // class Trapezoid

template<class pointT>
class Circle : public iShape<pointT> {
public:
    typedef boost::geometry::polygon_tag type;
    typedef iShape<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    pointT center;
    coordT radius;
    tBox bbox;

public:
    Circle(const pointT& c, coordT r)
        : center(c), radius(r)
    {
        computeBBox();
    }

    virtual ~Circle() {}

    /// construct bounding box. non-const version.
    const tBox& computeBBox() {
        coordT c_x = bg::get<0>(center);
        coordT c_y = bg::get<1>(center);
        bg::set<0>(bbox.min_corner(), c_x - radius);
        bg::set<1>(bbox.min_corner(), c_y - radius);
        bg::set<0>(bbox.max_corner(), c_x + radius);
        bg::set<1>(bbox.max_corner(), c_y + radius);
        return bbox;
    }
    /// return bounding box. const version.
    const tBox& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return circle_vertex_count;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw Circle: 256 vertices...\n");
        coordT x = bg::get<0>(center);
        coordT y = bg::get<1>(center);
        double theta = 2*PI / circle_vertex_count;
        // approximate the circle with 256-vertex ring
        for (int i=0; i<circle_vertex_count; ++i) {
            vertices.emplace_back(x+std::cos(i*theta)*radius,
                                  y+std::sin(i*theta)*radius,
                                  pointZval);
        }
        vertexCnts.emplace_back(circle_vertex_count);
    }
}; // class Circle


template<class pointT>
class Polygon : public iShape<pointT>, public bg::model::polygon<pointT> {
public:
    typedef boost::geometry::polygon_tag type;
    typedef typename bg::model::polygon<pointT> base_type;
    using coordT = typename iShape<pointT>::coordT;
    using tBox = typename iShape<pointT>::tBox;

protected:
    tBox bbox;

public:
    Polygon(std::vector<pointT>& points) {
        typename std::vector<pointT>::iterator it = points.begin();
        for (; it != points.end(); ++it) {
            bg::append(this->outer(), *it);
        }
        computeBBox();
    }

    virtual ~Polygon() {}

    const tBox& computeBBox() {
        bg::envelope(this->outer(), bbox);
        return bbox;
    }
    const tBox& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return bg::num_points(this->outer());
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        bg::for_each_point(this->outer(),
            [&vertices](const pointT& pt) {
                vertices.emplace_back(bg::get<0>(pt),
                                      bg::get<1>(pt),
                                      pointZval);
            }
        );
        vertexCnts.emplace_back(getVertexCount());
    }
}; // class Polygon


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
    tBox bbox;
    QColor lyrColor;
    std::string layerName;
    int vertexCnt;

public:
    Layer(int n, int d, const QColor& clr, const std::string& name = "")
        : layerNum(n)
        , dataT(d)
        , lyrColor(clr)
        , layerName(name)
        , vertexCnt(0) {
        iShape<pointT>::makeBoxInvalid(bbox);
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

    std::string getLayerName() const {
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
        shapes.push_back(shape);
        vertexCnt += shape->getVertexCount();
        bg::expand(bbox, shape->getBBox());
    }

    void deleteShape(iShape<pointT>* shape) {
        typename tShapes::iterator it = std::find(shapes.begin(), shapes.end(), shape);
        if (it != shapes.end()) {
            vertexCnt -= (*it)->getVertexCount();
            delete *it;
            shapes.erase(it);

            bbox = tBox();
            computeBBox();
        }
    }

    const tBox& computeBBox() {
        typename tShapes::const_iterator it = getShapes().begin();
        if (it != getShapes().end()) {
            bg::envelope(*it, bbox);
            ++it;
        }
        for (; it != getShapes().end(); ++it) {
            bg::expand(bbox, *it);
        }
        return bbox;
    }
    const tBox& getBBox() const {
        if (!iShape<pointT>::isBoxValid(bbox)) {
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
        printf("draw Layer: %d vertices...\n", getVertexCount());
        typename tShapes::const_iterator it = shapes.begin();
        for (; it != shapes.end(); ++it) {
            (*it)->getVertices(vertices, vertexCnts);
        }
    }
}; // class Layer


template<class pointT>
class Cell {
public:
    using tLayers = std::unordered_map<std::string, Layer<pointT>* >;
    typedef bg::model::box<pointT> tBox;

protected:
    std::string cellName;
    tLayers layers;
    Layer<pointT>* activeLayer;
    tBox bbox;

public:
    Cell(const std::string& n)
        : cellName(n)
        , activeLayer(nullptr)
    {
        iShape<pointT>::makeBoxInvalid(bbox);
    }

    virtual ~Cell() {
        for_each(layers.begin(), layers.end(),
            [] (typename decltype(layers)::value_type const& p) {
                delete p.second;
            }
        );
        layers.clear();
    }

    tLayers getLayers() {
        return layers;
    }
    const tLayers getLayers() const {
        return layers;
    }

    const tBox computeBBox() {
        typename tLayers::const_iterator it = getLayers().begin();
        if (it != getLayers().end()) {
            bbox = it->second->getBBox();
            ++it;
        }
        for (; it != getLayers().end(); ++it) {
            bg::expand(bbox, it->second->computeBBox());
        }
        return bbox;
    }

    tBox getBBox() const {
        if (!iShape<pointT>::isBoxValid(bbox))
            return computeBBox();
        return bbox;
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
    void delLayer(const std::string& name) {
        typename tLayers::iterator it = layers.find(name);
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

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw cell=%s. %d vertices...\n", getName().c_str(), getVertexCount());
        typename tLayers::const_iterator it = layers.begin();
        for (; it != layers.end(); ++it) {
            it->second->getVertices(vertices, vertexCnts);
        }
    }
}; // class Cell


template<class pointT>
class Layout {
public:
    using tCells = std::unordered_map<std::string, Cell<pointT>* >;
    typedef bg::model::box<pointT> tBox;

protected:
    std::string layoutName;
    tCells cells;
    Cell<pointT>* activeCell;
    tBox bbox;

public:
    Layout(const std::string& name)
        : layoutName(name)
    {
        iShape<pointT>::makeBoxInvalid(bbox);
    }

    virtual ~Layout() {
        for_each(cells.begin(), cells.end(),
            [] (typename decltype(cells)::value_type const& p) {
                delete p.second;
            }
        );
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
        return layoutName;
    }

    void setName(const std::string& name) {
        layoutName = name;
    }

    const tBox& computeBBox() {
        typename tCells::const_iterator it = getCells().begin();
        if (it != getCells().end()) {
            bbox = it->second->computeBBox();
            ++it;
        }
        for (; it != getCells().end(); ++it) {
            bg::expand(bbox, it->second->computeBBox());
        }
        return bbox;
    }

    const tBox& getBBox() const {
        if (!iShape<pointT>::isBoxValid(bbox)) {
            Layout<pointT>* This = const_cast<Layout<pointT>*>(this);
            return This->computeBBox();
        }
        return bbox;
    }

    int getVertexCount() const {
        int cnt = 0;
        typename tCells::const_iterator it = cells.begin();
        for (; it != cells.end(); ++it) {
            cnt += it->second->getVertexCount();
        }
        return cnt;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw layout=%s. %d vertices...", getName().c_str(), getVertexCount());
        typename tCells::const_iterator it = cells.begin();
        for (; it != cells.end(); ++it) {
            it->second->getVertices(vertices, vertexCnts);
        }
    }
}; // class Layout


template<class pointT>
class LayoutManager {
public:
    using tLayouts = std::unordered_map<std::string, Layout<pointT>* >;
    typedef bg::model::box<pointT> tBox;

protected:
    tLayouts layouts;
    Layout<pointT>* activeLayout;

public:
    LayoutManager()
        : activeLayout(nullptr)
    {}

    ~LayoutManager() {
        for_each(layouts.begin(), layouts.end(),
            [] (typename decltype(layouts)::value_type const& p) {
                delete p.second;
            }
        );
        layouts.clear();
    }

    Layout<pointT>* newLayout(const std::string& name) {
        Layout<pointT>* layout = new Layout<pointT>(name);
        typename tLayouts::const_iterator it = getLayouts().find(name);
        if (it == getLayouts().end()) {
            layouts[name] = layout;
            activeLayout = layout;
        } else {
            activeLayout = it->second;
        }
        return activeLayout;
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

    const tBox& getBBox() const {
        if (activeLayout)
            return activeLayout()->getBBox();
        return iShape<pointT>::invalidBox();
    }

    int getVertexCount() const {
        if (activeLayout)
            return activeLayout->getVertexCount();
        return 0;
    }
    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        if (activeLayout) {
            activeLayout->getVertices(vertices, vertexCnts);
        }
    }

}; // class LayoutManager

typedef bg::model::point<double, 2, bg::cs::cartesian> dPoint;
typedef bg::model::box<dPoint> bgdBox;
typedef Box<dPoint> dBox;
typedef Trapezoid<dPoint> dTrapezoid;
typedef Circle<dPoint> dCircle;
typedef Polygon<dPoint> dPolygon;
typedef Layer<dPoint> dLayer;
typedef Cell<dPoint> dCell;
typedef Layout<dPoint> dLayout;
typedef LayoutManager<dPoint> dLayoutManager;

void initializeManager(dLayoutManager& layM);

} // namespace layout

#endif // __LAYOUT_H__






















