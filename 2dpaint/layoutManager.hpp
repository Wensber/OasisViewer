#ifndef __LAYOUT_LAYOUT_MANAGER_HPP__
#define __LAYOUT_LAYOUT_MANAGER_HPP__

#include "layout.hpp"


namespace layout {

template<typename pointT>
class LayoutManager {
public:
    typedef typename pointT::coord_type coord_type;
    typedef std::unordered_map<std::string, Layout<pointT>* > tLayouts;

protected:
    tLayouts layouts;
    Layout<pointT>* activeLayout;
    Box<pointT> bbox{std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min(),
                     std::numeric_limits<coord_type>::min()};

public:
    LayoutManager()
        : activeLayout(nullptr)
    {
        layouts.clear();
        bbox.makeInvalid();
    }

    ~LayoutManager() {
        for_each(getLayout().begin(), getLayouts().end(),
            [] (typename decltype(layouts)::value_type const& p) {
                delete p.second;
            }
        );
        getLayout().clear();
    }

    Layout<pointT>* newLayout(const std::string& name) {
        typename tLayouts::iterator it = getLayouts().find(name);
        if (it == getLayouts().end()) {
            Layout<pointT>* layout = new Layout<pointT>(name);
            getLayouts().insert(std::make_pair(name, layout));
            activeLayout = layout;
            bbox.makeInvalid();
        } else {
            activeLayout = it->second;
        }
        return activeLayout;
    }

    void closeLayout(const std::string& name) {
        typename tLayouts::iterator it = getLayouts().find(name);
        if (it != getLayouts().end()) {
            if (activeLayout == it->second)
                activeLayout = nullptr;
            getLayouts().erase(it);
            bbox.makeInvalid();
        }
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
        typename tLayouts::iterator it = getLayout().find(name);
        if (it != getLayouts().end()) {
            if (activeLayout == it->second)
                activeLayout = nullptr;
            delete it->second;
            layouts.erase(it);
            bbox.makeInvalid();
        }
    }

    Layout<pointT>* getLayout(const std::string& name) {
        typename tLayouts::iterator it = getLayouts().find(name);
        if (it != getLayouts().end())
            return it->second;
        return nullptr;
    }

    const Layout<pointT>* getLayout(const std::string& name) const {
        typename tLayouts::const_iterator it = getLayouts().find(name);
        if (it != getLayouts().end())
            return it->second;
        return nullptr;
    }

    const Box<pointT>& getBBox() const {
        if (activeLayout) {
            bbox = activeLayout()->getBBox();
        } else {
            bbox.makeInvalid();
        }
        return bbox;
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

typedef LayoutManager<dPoint> dLayoutManager;

void initializeManager(dLayoutManager& layM);

} // namespace layout

#endif // __LAYOUT_LAYOUT_HPP__






















