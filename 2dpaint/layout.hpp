#ifndef __LAYOUT_LAYOUT_HPP__
#define __LAYOUT_LAYOUT_HPP__


#include "cell.hpp"


namespace layout {

template<typename pointT>
class Layout {
public:
    typedef typename pointT::coord_type coord_type;
    typedef std::map<std::string, Cell<pointT>* > tCells;

protected:
    std::string layoutName;
    tCells cells;
    Cell<pointT>* activeCell;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    Layout(const std::string& name)
        : layoutName(name)
        , activeCell(nullptr)
    {
        cells.clear();
        bbox.makeInvalid();
    }

    virtual ~Layout() {
        for_each(getCells().begin(), getCells().end(),
            [] (typename decltype(cells)::value_type const& p) {
                delete p.second;
            }
        );
        getCells().clear();
    }

    void print() {

        std::cout << layoutName << std::endl;
        typename std::map<std::string, Cell<pointT>* >::const_iterator it = cells.begin();
        for(; it != cells.end(); ++it) {
            ((Cell<pointT>*)(it->second))->print("  ");
        }

    }

    tCells& getCells() {
        return cells;
    }
    const tCells& getCells() const {
        return cells;
    }

    Cell<pointT>* newCell(const std::string& name) {
        std::cout << "Start New Cell" << std::endl;
        typename tCells::const_iterator it = getCells().find(name);
        if (it != cells.end()) {
            //std::stringstream str("Cell ");
            //str << name << " already exists.";
            //throw std::runtime_error(str.str().c_str());
            return it->second;
        } else {
            Cell<pointT>* cell = new Cell<pointT>(name);
            getCells().insert(std::make_pair(name, cell));
            activeCell = cell;
            bbox.makeInvalid();
            return cell;
        }
    }

    Cell<pointT>* getCell(const std::string& name) {
        typename tCells::iterator it = getCells().find(name);
        if (it != cells.end())
            return it->second;
        return nullptr;
    }

    const Cell<pointT>* getCell(const std::string& name) const {
        typename tCells::const_iterator it = getCells().find(name);
        if (it != getCells().end())
            return it->second;
        return nullptr;
    }

    void delCell(const std::string& name) {
        typename tCells::iterator it = getCells().find(name);
        if (it != getCells().end()) {
            if (activeCell == it->second)
                activeCell = nullptr;
            delete it->second;
            getCells().erase(it);
            bbox.makeInvalid();
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

    const Box<pointT>& computeBBox() {
        typename tCells::const_iterator it = cells.begin();
        if (it != getCells().end()) {
            bbox = it->second->getBBox();
            ++it;
        }
        for (; it != getCells().end(); ++it) {
            bbox.expand(it->second->getBBox());
        }
        return bbox;
    }

    const Box<pointT>& getBBox() const {
        if (!bbox.isValid()) {
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

    void getVerticesAndColors(std::vector<QVector3D>& vertices,
                     std::vector<QVector4D>& colors,
                     std::vector<int>& vertexCnts) const {
        printf("draw layout=%s. %d vertices...\n", getName().c_str(), getVertexCount());
        typename tCells::const_iterator it = cells.begin();
        for (; it != cells.end(); ++it) {
            it->second->getVerticesAndColors(vertices, colors, vertexCnts);
        }
    }
}; // class Layout

typedef Layout<dPoint> dLayout;

} // namespace layout

std::ostream& operator<<(std::ostream& o, const layout::dLayout& l);

#endif // __LAYOUT_LAYOUT_HPP__






















