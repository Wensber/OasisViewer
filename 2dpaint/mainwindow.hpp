#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "layoutManager.hpp"
#include "oasisfilemanager.hpp"

class GLWidget;


class MainWindow : public QMainWindow
{
  Q_OBJECT

protected:
  QLabel* m_label;

  layout::dLayoutManager* m_layM;
  GLWidget* m_glw;

public:
  MainWindow(QWidget *parent = nullptr);
  virtual ~MainWindow() {}

  void createFileMenu();
  void createViewMenu();

  void createMenus();
  void createToolBars();

  layout::dLayoutManager& getLayoutManager() {
      return *m_layM;
  }
  const layout::dLayoutManager& getLayoutManager() const {
      return *m_layM;
  }

  GLWidget* getGLWidget() {
      return m_glw;
  }
public slots:
  void onOpen(bool bChecked);
    const GLWidget* getGLWidget() const {
        return m_glw;
    }

  void onNew(bool bChecked);
  void onSave(bool bChecked);
  void onSaveAs(bool bChecked);
  void onExit(bool bChecked);

#if 0
  void onZoomFull(bool bChecked);
  void onZoomIn2X(bool bChecked);
  void onZoomOutHalf(bool bChecked);
  void onZoomIn(bool bChecked);
  void onPan(bool bChecked);
#endif

};
#endif // __MAINWINDOW_H




















