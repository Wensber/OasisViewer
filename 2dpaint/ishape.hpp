#ifndef __LAYOUT_I_SHAPE_HPP__
#define __LAYOUT_I_SHAPE_HPP__


#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#include <array>
#include <vector>

#include <QVector3D>
#include <QVector4D>


namespace bg = boost::geometry;

extern const double PI;

namespace layout {


extern const int circle_vertex_count;
extern float pointZval;

template<typename T, std::size_t D>
class Point : public virtual std::array<T, D> {
public:
    typedef T coord_type;

    Point& operator=(const Point& p) {
        for (int i=0; i<D; ++i) {
            (*this)[i] = p[i];
        }
        return *this;
    }

    Point operator+(const Point& p) {
        Point pt;
        for (int i=0; i<D; ++i) {
            pt[i] = (*this)[i] + p[i];
        }
        return pt;
    }
    Point& operator+=(const Point& p) {
        for (int i=0; i<D; ++i) {
            (*this)[i] += p[i];
        }
        return *this;
    }

    Point operator-(const Point& p) {
        Point pt;
        for (int i=0; i<D; ++i) {
            pt[i] = (*this)[i] - p[i];
        }
        return pt;
    }
    Point& operator-=(const Point& p) {
        for (int i=0; i<D; ++i) {
            (*this)[i] -= p[i];
        }
        return *this;
    }

    Point operator*(coord_type val) {
        Point pt;
        for (int i=0; i<D; ++i) {
            pt[i] = (*this)[i] * val;
        }
        return pt;
    }
    Point& operator*=(coord_type factor) {
        for (int i=0; i<D; ++i) {
            (*this)[i] *= factor;
        }
        return *this;
    }

    Point operator/(coord_type val) {
        Point pt;
        for (int i=0; i<D; ++i) {
            pt[i] = (*this)[i] / val;
        }
        return pt;
    }
    Point& operator/=(coord_type factor) {
        for (int i=0; i<D; ++i) {
            (*this)[i] /= factor;
        }
        return *this;
    }
}; // class Point

template<typename T>
class Point2 : public Point<T, 2> {
public:
    typedef T coord_type;

    Point2() {
        (*this)[0] = std::numeric_limits<T>::min();
        (*this)[1] = std::numeric_limits<T>::min();
    }

    Point2(coord_type x, coord_type y) {
        (*this)[0] = x;
        (*this)[1] = y;
    }

    coord_type x() const {
        return (*this)[0];
    }
    void setX(coord_type v) {
        (*this)[0] = v;
    }

    coord_type y() const {
        return (*this)[1];
    }
    void setY(coord_type v) {
        (*this)[1] = v;
    }

}; // class Point2

typedef Point2<double> dPoint;

} // namespace layout

#if 0
namespace boost { namespace geometry { namespace traits
{

template <typename T, std::size_t D> struct tag<layout::Point<T, D>> { using type = point_tag; };
template <typename T, std::size_t D> struct dimension<layout::Point<T, D>> : boost::mpl::int_<D> {};
template <typename T, std::size_t D> struct coordinate_type<layout::Point<T, D>> { using type = T; };
template <typename T, std::size_t D> struct coordinate_system<layout::Point<T, D>> { using type = boost::geometry::cs::cartesian; };

template <typename T, std::size_t D, std::size_t Index>
struct access<layout::Point<T, D>, Index> {
    static_assert(Index < D, "Out of range");
    using CoordinateType = typename coordinate_type<layout::Point<T, D>>::type;
    static inline CoordinateType get(layout::Point<T, D> const& p) { return p[Index]; }
    static inline void set(layout::Point<T, D>& p, CoordinateType const& value) { p[Index] = value; }
}; // access

template <typename T> struct tag<layout::Point2<T>> { using type = point_tag; };
template <typename T> struct dimension<layout::Point2<T>> : boost::mpl::int_<2> {};
template <typename T> struct coordinate_type<layout::Point2<T>> { using type = T; };
template <typename T> struct coordinate_system<layout::Point2<T>> { using type = boost::geometry::cs::cartesian; };

template <typename T, std::size_t Index>
struct access<layout::Point2<T>, Index> {
    static_assert(Index < 2, "Out of range");
    using CoordinateType = typename coordinate_type<layout::Point2<T>>::type;
    static inline CoordinateType get(layout::Point2<T> const& p) { return p[Index]; }
    static inline void set(layout::Point2<T>& p, CoordinateType const& value) { p[Index] = value; }
}; // access

}}} // namespace boost::geometry::traits
#else
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(layout::dPoint, double, bg::cs::cartesian, x, y, setX, setY)
#endif

namespace layout {

template <typename pointT>
class Box;

#define BOX 1
#define CIRCLE 2
#define POLYGON 3
#define TRAPEZOID 4

template<typename pointT>
class iShape {
public:
    typedef typename pointT::coord_type coord_type;
    typedef Box<pointT> tBox;

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
    virtual int getShapeType() = 0;

    virtual void print(std::string prefix) = 0;

};

typedef iShape<dPoint> dShape;

} // namespace layout

#endif // __LAYOUT_I_SHAPE_HPP__






















