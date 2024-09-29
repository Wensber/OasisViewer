#ifndef __LAYOUT_POLYGON_HPP__
#define __LAYOUT_POLYGON_HPP__


#include "trapezoid.hpp"

#include <boost/geometry/geometries/polygon.hpp>


namespace layout {

template<typename pointT>
class Polygon : public iShape<pointT>, public bg::model::polygon<pointT> {
public:
    typedef bg::model::polygon<pointT> base_type;
    typedef typename pointT::coord_type coord_type;

protected:
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    Polygon()
        : base_type() {
        base_type::outer().clear();
        base_type::inners().clear();
    }

    Polygon(const std::vector<pointT>& pts) {
        this->base_type::outer().reserve(pts.size());
        typename std::vector<pointT>::const_iterator it = pts.begin();
        for (; it != pts.end(); ++it) {
            bg::append(this->base_type::outer(), *it);
        }

        computeBBox();
    }

    virtual ~Polygon() {}

    void print(std::string prefix) {

        std::cout << prefix << "Polygon" << std::endl;

        std::vector<QVector3D> vertices;
        std::vector<int> count;
        getVertices(vertices, count);

        std::cout << prefix;
        int i;
        for(i=0; i<vertices.size(); ++i) {
            std::cout << "(" << vertices.at(i).x() << ", " << vertices.at(i).y() << ") ";
        }
        std::cout << std::endl;

    }

    const Box<pointT>& computeBBox() {
        bg::envelope(this->outer(), bbox);
        return bbox;
    }
    const Box<pointT>& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return (int)this->outer().size();
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

    int getShapeType() {
        return POLYGON;
    }
}; // class Polygon

typedef Polygon<dPoint> dPolygon;

} // namespace layout

#if 1
//BOOST_GEOMETRY_REGISTER_MULTI_POLYGON_TEMPLATED(layout::Polygon)
#else
namespace boost { namespace polygon {
//first register CPolygon as a polygon_concept type
template <>
struct geometry_concept<layout::dPolygon> {
    typedef polygon_concept type;
};

template <>
struct polygon_traits<layout::dPolygon> {
    typedef int coordinate_type;
    typedef layout::dPolygon::outer()::const_iterator iterator_type;
    typedef layout::dPoint point_type;

    // Get the begin iterator
    static inline iterator_type begin_points(const layout::dPolygon& t) {
        return t.outer().begin();
    }

    // Get the end iterator
    static inline iterator_type end_points(const layout::dPolygon& t) {
        return t.outer().end();
    }

    // Get the number of sides of the polygon
    static inline std::size_t size(const layout::dPolygon& t) {
        return t.outer().size();
    }

    // Get the winding direction of the polygon
    static inline winding_direction winding(const layout::dPolygon& t) {
        return unknown_winding;
    }
}; // struct polygon_traits

template <>
struct polygon_mutable_traits<layout::dPolygon> {
    //expects stl style iterators
    template <typename iT>
    static inline layout::dPolygon& set_points(layout::dPolygon& t,
                                       iT input_begin, iT input_end) {
        t.clear();
        t.insert(t.end(), input_begin, input_end);
        return t;
    }
}; // struct polygon_mutable_traits
} }
#endif

#endif // __LAYOUT_POLYGON_HPP__






















