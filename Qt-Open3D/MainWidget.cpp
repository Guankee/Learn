#include "MainWidget.h"

MainWindow::MainWindow(QMainWindow* parent)
    : QMainWindow(parent), ui(new Ui::GL3D) {
  ui->setupUi(this);
   setCentralWidget(ui->openGLWidget);
  // setAcceptDrops(true);
  //  widget =new GLWidget;
}
// void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
// 	std::cout << " dragEnterEvent " << std::endl;
// 	event->acceptProposedAction();
// }
// //接受了之后 放下触发
// void MainWindow::dropEvent(QDropEvent* event) {
// 	std::cout << " dropEvent " << std::endl;
// }
MainWindow::~MainWindow() { delete ui; }
