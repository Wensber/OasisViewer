#ifndef __LAYOUT_BOX_HPP__
#define __LAYOUT_BOX_HPP__


#include "ishape.hpp"

#if 1
#include <boost/geometry/geometries/register/box.hpp>
#endif

namespace layout {

template<typename pointT>
class Box : public iShape<pointT>, public bg::model::box<pointT> {
public:
    typedef bg::model::box<pointT> base_type;
    typedef typename pointT::coord_type coord_type;

public:
    Box() = default;
    Box(const pointT& min, const pointT& max)
        : base_type(min, max) {
    }
    Box(coord_type minx, coord_type miny, coord_type maxx, coord_type maxy)
        : base_type(pointT(minx, miny), pointT(maxx, maxy)) {
    }

    virtual ~Box() {}

    void print(std::string prefix) {

        std::cout << prefix << "Box" << std::endl;
        std::cout << prefix << "(" << getMinX() << ", " << getMinY() << ") "
                            << "(" << getMaxX() << ", " << getMaxY() << ") " << std::endl;

    }

    pointT& min_corner() {
        return base_type::min_corner();
    }
    const pointT& min_corner() const {
        return base_type::min_corner();
    }

    pointT& max_corner() {
        return base_type::max_corner();
    }
    const pointT& max_corner() const {
        return base_type::max_corner();
    }

    pointT center() const {
        pointT ctr = min_corner();
        ctr += max_corner();
        ctr /= 2.0;
        return ctr;
    }

    coord_type getMinX() const {
        return bg::get<0>(base_type::min_corner());
    }
    void setMinX(coord_type x) {
        bg::set<0>(base_type::min_corner(), x);
    }

    coord_type getMaxX() const {
        return bg::get<0>(base_type::max_corner());
    }
    void setMaxX(coord_type x) {
        bg::set<0>(base_type::max_corner(), x);
    }

    coord_type getMinY() const {
        return bg::get<1>(base_type::min_corner());
    }
    void setMinY(coord_type y) {
        bg::set<1>(base_type::min_corner(), y);
    }

    coord_type getMaxY() const {
        return bg::get<1>(base_type::max_corner());
    }
    void setMaxY(coord_type y) {
        bg::set<1>(base_type::max_corner(), y);
    }

    coord_type getWidth() const {
        return getMaxX() - getMinX();
    }
    coord_type getHeight() const {
        return getMaxY() - getMinY();
    }

    Box& operator=(const bg::model::box<pointT>& box) {
        setMinX(bg::get<0>(box.min_corner()));
        setMinY(bg::get<1>(box.min_corner()));
        setMaxX(bg::get<0>(box.max_corner()));
        setMaxY(bg::get<1>(box.max_corner()));
        return *this;
    }

    Box operator-=(const pointT& pt) {
        min_corner() -= pt;
        max_corner() -= pt;
    }
    Box& operator*=(coord_type factor) {
        pointT ctr = center();
        pointT dist(getWidth()*factor, getHeight()*factor);
        min_corner() = ctr;
        min_corner() -= dist;
        max_corner() = ctr;
        max_corner() += dist;
        return *this;
    }

    void makeInvalid() {
        setMinX(std::numeric_limits<coord_type>::min());
        setMinY(std::numeric_limits<coord_type>::min());
        setMaxX(std::numeric_limits<coord_type>::min());
        setMaxY(std::numeric_limits<coord_type>::min());
    }

    bool isValid() const {
        if (getWidth() > 0.0 || getHeight() > 0.0)
            return true;
        return false;
    }

    void expand(const Box& box) {
        if (box.getMinX() < getMinX())
            setMinX(box.getMinX());
        if (box.getMinY() < getMinY())
            setMinY(box.getMinY());
        if (box.getMaxX() > getMaxX())
            setMaxX(box.getMaxX());
        if (box.getMaxY() > getMaxY())
            setMaxY(box.getMaxY());
    }

    const Box& computeBBox() {
        // nothing to do.
        return *this;
    }
    const Box& getBBox() const {
        return *this;
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

    int getShapeType() {
        return BOX;
    }

}; // class Box

typedef Box<dPoint> dBox;

} // namespace layout

#if 0
namespace boost { namespace geometry { namespace traits
{

template <typename pointT> struct tag<layout::Box<pointT>> { using type = box_tag; };
template <typename pointT> struct point_type<layout::Box<pointT>> { using type = pointT; };

template <typename pointT, std::size_t Corner, std::size_t Index>
struct indexed_access<layout::Box<pointT>, Corner, Index> {
    static_assert(Corner == min_corner || Corner == max_corner, "Out of range");
    static_assert(Index < pointT::D, "Out of range");
    using CoordinateType = typename coordinate_type<layout::Box<pointT>>::type;
    static inline CoordinateType get(layout::Box<pointT> const& b) {
        return bg::get<Index>(b.Corner);
    }
    static inline CoordinateType set(layout::Box<pointT> const& b, CoordinateType value) {
        bg::set<Index>(b.Corner, value);
    }
}; // indexed_access

}}} // namespace boost::geometry::traits
#else
BOOST_GEOMETRY_REGISTER_BOX_TEMPLATED(layout::Box, min_corner(), max_corner())
#endif

#endif // __LAYOUT_BOX_HPP__
