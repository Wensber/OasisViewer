#ifndef __LAYOUT_CIRCLE_HPP__
#define __LAYOUT_CIRCLE_HPP__


#include "trapezoid.hpp"


namespace layout {

template<typename pointT>
class Circle : public iShape<pointT> {
public:
    typedef typename pointT::coord_type coord_type;

protected:
    pointT center;
    coord_type radius;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    Circle(const pointT& c, coord_type r)
        : center(c), radius(r)
    {
        computeBBox();
    }

    virtual ~Circle() {}

    void print(std::string prefix) {

        std::cout << prefix << "Circle" << std::endl;
        std::cout << prefix << "(" << bg::get<0>(center) << ", " << bg::get<1>(center) << ") " << radius << std::endl;

    }

    pointT& getCenter() {
        return center;
    }
    const pointT& getCenter() const {
        return center;
    }

    coord_type getRadius() const {
        return radius;
    }

    const Box<pointT>& computeBBox() {
        coord_type c_x = getCenter().x();
        coord_type c_y = getCenter().y();
        bbox.setMinX(c_x - getRadius());
        bbox.setMinY(c_y - getRadius());
        bbox.setMaxX(c_x + getRadius());
        bbox.setMaxY(c_y + getRadius());
        return bbox;
    }
    /// return bounding box. const version.
    const Box<pointT>& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return circle_vertex_count;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw Circle: 256 vertices...\n");
        coord_type x = bg::get<0>(getCenter());
        coord_type y = bg::get<1>(getCenter());
        double theta = 2*PI / circle_vertex_count;
        // approximate the circle with 256-vertex ring
        for (int i=0; i<circle_vertex_count; ++i) {
            vertices.emplace_back(x+std::cos(i*theta)*getRadius(),
                                  y+std::sin(i*theta)*getRadius(),
                                  pointZval);
        }
        vertexCnts.emplace_back(circle_vertex_count);
    }

    int getShapeType() {
        return CIRCLE;
    }
}; // class Circle

typedef Circle<dPoint> dCircle;

} // namespace layout

//BOOST_GEOMETRY_REGISTER_RING_TEMPLATED(layout::Circle)

#endif // __LAYOUT_CIRCLE_HPP__






















