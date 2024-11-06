
#include "glwidget.hpp"
#include "layoutManager.hpp"
#include "mainwindow.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  QFrame* frame = new QFrame();
  frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  frame->setLineWidth(5);
  setCentralWidget(frame);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->setContentsMargins(5, 5, 5, 5);

  m_layM = new layout::dLayoutManager();
  layout::initializeManager(*m_layM);

  m_glw = new GLWidget(*m_layM, frame);
  // set glw resizeable.
  m_glw->setMinimumSize(400, 400);
  m_glw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  hbox->addWidget(m_glw);
  frame->setLayout(hbox);

  createMenus();
  createToolBars();

  QStatusBar* sbar = new QStatusBar();
  setStatusBar(sbar);
  sbar->showMessage("Ready");

  oasisio::OasisFileManager<layout::dPoint> ofm;
  std::string name = "TestLayout";

  layout::Layout<layout::dPoint> layout(name);
  layout.newCell("Cell 1");
  layout.newCell("Cell 2");

  //ofm.writeOasisFile(&layout, name+".oasis");

}

void MainWindow::createFileMenu() {
  QMenuBar* mbar = menuBar();
  QMenu* fileMenu = mbar->addMenu(tr("&File"));
  fileMenu->setObjectName("File");

  QAction* newAct = new QAction(tr("&New..."), fileMenu);
  newAct->setObjectName("File.New");
  newAct->setShortcut(QKeySequence(tr("Ctrl+N")));
  fileMenu->addAction(newAct);

  QAction* openAct = new QAction("&Open...", fileMenu);
  openAct->setObjectName("File.Open");
  openAct->setShortcut(QKeySequence(tr("Ctrl+O")));
  fileMenu->addAction(openAct);

  fileMenu->addSeparator();

  QAction* saveAct = new QAction(tr("&Save"), fileMenu);
  saveAct->setObjectName("File.Save");
  saveAct->setShortcut(QKeySequence(tr("Ctrl+S")));
  saveAct->setEnabled(true);
  fileMenu->addAction(saveAct);

  QAction* saveAsAct = new QAction(tr("Save &As"), fileMenu);
  saveAsAct->setObjectName("File.SaveAs");
  saveAsAct->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
  saveAsAct->setEnabled(true);
  fileMenu->addAction(saveAsAct);

  fileMenu->addSeparator();

  QAction* exitAct = new QAction(tr("E&xit"), fileMenu);
  exitAct->setObjectName("File.Exit");
  exitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));
  fileMenu->addAction(exitAct);

  connect(newAct, &QAction::triggered, this, &MainWindow::onNew);
  connect(openAct, &QAction::triggered, this, &MainWindow::onOpen);
  connect(saveAct, &QAction::triggered, this, &MainWindow::onSave);
  connect(saveAsAct, &QAction::triggered, this, &MainWindow::onSaveAs);
  connect(exitAct, &QAction::triggered, this, &MainWindow::onExit);

#if 0
  connect(newAct, &QAction::triggered, saveAct, &QAction::setEnabled);
  connect(newAct, &QAction::triggered, saveAsAct, &QAction::setEnabled);
#endif

}

void MainWindow::createViewMenu() {
    QMenuBar* mbar = menuBar();
    QMenu* viewMenu = mbar->addMenu(tr("&View"));
    viewMenu->setObjectName("View");

    QAction* zoomFullAct = new QAction(tr("Zoom Full"), viewMenu);
    zoomFullAct->setObjectName("View.ZoomFull");
    zoomFullAct->setShortcut(QKeySequence(tr("Ctrl+F")));
    viewMenu->addAction(zoomFullAct);

    QAction* zoomInHalfAct = new QAction(tr("Zoom In Half"), viewMenu);
    zoomInHalfAct->setObjectName("View.Zoom.ZoomInHalf");
    zoomInHalfAct->setShortcut(QKeySequence(tr("Ctrl+I")));
    viewMenu->addAction(zoomInHalfAct);

    QAction* zoomOut2XAct = new QAction(tr("Zoom Out 2X"), viewMenu);
    zoomOut2XAct->setObjectName("View.Zoom.ZoomOut2X");
    zoomOut2XAct->setShortcut(QKeySequence(tr("Ctrl+Shift+O")));
    viewMenu->addAction(zoomOut2XAct);

    QActionGroup* mouseGroup = new QActionGroup(viewMenu);
    mouseGroup->setObjectName("View.mouseGroup");
    mouseGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    QAction* zoomInAct = new QAction(tr("Zoom In"), mouseGroup);
    zoomInAct->setObjectName("View.ZoomIn");
    zoomInAct->setCheckable(true);
    zoomInAct->setShortcut(QKeySequence(tr("Ctrl+Z")));
    viewMenu->addAction(zoomInAct);

    QAction* panAct = new QAction(tr("Pan"), mouseGroup);
    panAct->setObjectName("View.Pan");
    panAct->setCheckable(true);
    panAct->setShortcut(QKeySequence(tr("Ctrl+P")));
    viewMenu->addAction(panAct);

    QAction* rotateAct = new QAction(tr("Rotate"), mouseGroup);
    rotateAct->setObjectName("View.Rotate");
    rotateAct->setCheckable(true);
    rotateAct->setShortcut(QKeySequence(tr("Ctrl+R")));
    //viewMenu->addAction(rotateAct);

    connect(zoomFullAct, &QAction::triggered, getGLWidget(), &GLWidget::onZoomFull);
    connect(zoomInHalfAct, &QAction::triggered, getGLWidget(), &GLWidget::onZoomInHalf);
    connect(zoomOut2XAct, &QAction::triggered, getGLWidget(), &GLWidget::onZoomOut2X);
    connect(zoomInAct, &QAction::triggered, getGLWidget(), &GLWidget::onZoomIn);
    connect(panAct, &QAction::triggered, getGLWidget(), &GLWidget::onPan);
    connect(rotateAct, &QAction::triggered, getGLWidget(), &GLWidget::onRotate);
}

void MainWindow::createMenus() {
  printf("in MainWindow::createMenus()...\n");
  QMenuBar* mbar = new QMenuBar();
  setMenuBar(mbar);

  // create File menu.
  createFileMenu();

  createViewMenu();

}

void MainWindow::createToolBars() {
  QToolBar* toolBar = new QToolBar(tr("OASIS Viewer"));
  this->addToolBar(toolBar);
}

void MainWindow::onNew(bool bChecked) {
  static int cnt = 0;
  printf("MainWindow::onNew: bChecked=%d...\n", bChecked);

  std::string msg("onNew: cnt=");
  msg += std::to_string(++cnt) + "...";
  m_label->setText(msg.c_str());

  QAction* act = findChild<QAction*>("File.Save");
  act->setEnabled(true);
  act = findChild<QAction*>("File.SaveAs");
  act->setEnabled(true);
}

void MainWindow::onOpen(bool bChecked) {
  QString fn = QFileDialog::getOpenFileName(this,
                 tr("Open File"), ".", tr("OASIS Files (*.oas *.oasis)"));
  std::string name = fn.toStdString();
  printf("Opening layout: %s...\n", fn.toStdString().c_str());

  // load this oasis file.
  layout::Layout<layout::dPoint>* output =
    getLayoutManager().newLayout(name);
  std::ifstream ifs;
  ifs.open(name);
  oasisio::OasisFileManager<layout::dPoint> ofm;
  ofm.readOasisFile(ifs, *output);

  //std::cout << *output << std::endl;
  output->print();

  QAction* act = findChild<QAction*>("File.Save");
  act->setEnabled(true);
  act = findChild<QAction*>("File.SaveAs");
  act->setEnabled(true);

  getGLWidget()->update();
}

void MainWindow::onSave(bool bChecked) {
  // write the current layout to fn.
  std::string name("");
  if (m_layM->getActiveLayout()) {
      name = m_layM->getActiveLayout()->getName();
      oasisio::OasisFileManager<layout::dPoint> ofm;
      //std::cout << m_layM->getActiveLayout() << std::endl;
      ofm.writeOasisFile(m_layM->getActiveLayout(), name);
  }
}

void MainWindow::onSaveAs(bool bChecked) {
    if (m_layM->getActiveLayout()) {
        QString fn = QFileDialog::getSaveFileName(this,
                        tr("Save As"), ".", tr("OASIS Files (*.oas *.oasis)"));
        std::string name = fn.toStdString();
        m_layM->getActiveLayout()->setName(name);
        onSave(false);
    }
}

void MainWindow::onExit(bool bChecked) {
  printf("MainWindow::onExit: bChecked=%d...\n", bChecked);
  qApp->quit();
}

#if 0
void MainWindow::onZoomFull(bool bChecked) {
    printf("MainWindow::onZoomFull: bChecked=%d...\n", bChecked);
    getGLWidget()->onZoomFull();
}

void MainWindow::onZoomInHalf(bool bChecked) {
    printf("MainWindow::onZoomInHalf: bChecked=%d...\n", bChecked);
    getGLWidget()->onZoomInHalf();
}

void MainWindow::onZoomOut2X(bool bChecked) {
    printf("MainWindow::onZoomOut2X: bChecked=%d...\n", bChecked);
    getGLWidget()->onZoomOut2X();
}

void MainWindow::onZoomIn(bool bChecked) {
    printf("MainWindow::onZoomIn: bChecked=%d...\n", bChecked);
    getGLWidget()->onZoomIn(bChecked);
}

void MainWindow::onPan(bool bChecked) {
    printf("MainWindow::onPan: bChecked=%d...\n", bChecked);
    getGLWidget()->onPan(bChecked);
}
#endif
