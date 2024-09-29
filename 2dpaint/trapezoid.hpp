#ifndef __LAYOUT_TRAPEZOID_HPP__
#define __LAYOUT_TRAPEZOID_HPP__


#include "box.hpp"
#include <cassert>

#include <boost/geometry/geometries/register/ring.hpp>


namespace layout {

template<typename pointT>
class Trapezoid : public iShape<pointT>, public bg::model::ring<pointT> {
public:
    typedef bg::model::ring<pointT> base_type;
    typedef typename pointT::coord_type coord_type;

protected:
    int tType;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    Trapezoid(const std::vector<pointT>& pts, int type)
        : base_type(pts.begin(), pts.end())
        , tType(type) {
        assert(pts.size() == 4);
        computeBBox();
    }

    void print(std::string prefix) {

        std::cout << prefix << "Trapezoid" << std::endl;

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

    virtual ~Trapezoid() {}

    /// compute bounding box. non const version
    const Box<pointT>& computeBBox() {
        std::set<coord_type> xs;
        std::set<coord_type> ys;
        typename std::vector<pointT>::const_iterator it = this->begin();
        for (; it != this->end(); ++it) {
            xs.emplace(bg::get<0>(*it));
            ys.emplace(bg::get<1>(*it));
        }
        bbox.setMinX(*(xs.begin()));
        bbox.setMinY(*(ys.begin()));
        bbox.setMaxX(*(xs.rbegin()));
        bbox.setMaxY(*(ys.rbegin()));
        return bbox;
    }
    /// return bounding box. const version.
    const Box<pointT>& getBBox() const {
        return bbox;
    }

    int getVertexCount() const {
        return 4;
    }

    void getVertices(std::vector<QVector3D>& vertices,
                     std::vector<int>& vertexCnts) const {
        printf("draw Trapezoid: 4 vertices...\n");
        typename std::vector<pointT>::const_iterator it = this->begin();
        for (; it != this->end(); ++it) {
            vertices.emplace_back(bg::get<0>(*it), bg::get<1>(*it), pointZval);
        }
        vertexCnts.emplace_back(4);
    }

    int getShapeType() {
        return TRAPEZOID;
    }

}; // class Trapezoid

typedef Trapezoid<dPoint> dTrapezoid;

} // namespace layout

//BOOST_GEOMETRY_REGISTER_RING_TEMPLATED(layout::Trapezoid)

#endif // __LAYOUT_TRAPEZOID_HPP__






















