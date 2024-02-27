#include "GLWidget.h"
GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  // ReadMeshOBJ("C:\\Users\\A015240\\Desktop\\tt\\1.obj");
  setAcceptDrops(true);
}

GLWidget::~GLWidget() {}

void GLWidget::dragEnterEvent(QDragEnterEvent* event) {
  std::cout<<"dragEnterEvent "<<std::endl;
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void GLWidget::dropEvent(QDropEvent* event) {
  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
      return;
    }
    //目前只写放入单个obj文件的
    QString path=urls.first().toLocalFile();
    // std::cout<<path.toStdString()<<std::endl;
    bool ret=ReadMeshOBJ(path);

    // ui->textEdit->setText(urls.first().toLocalFile());
  }
}

bool GLWidget::ReadMeshOBJ(const QString& path) {
  if (!path.endsWith("obj")) {
    int ret = QMessageBox::warning(this, tr("QT-Open3d"),
                                   tr("Only obj files are supported!"));
    return false;
  }
  if (nullptr == obj_mesh_) {
    obj_mesh_ = std::make_shared<open3d::geometry::TriangleMesh>();
  }
  open3d::io::ReadTriangleMeshOptions params;
  open3d::io::ReadTriangleMeshFromOBJ(path.toStdString(), *obj_mesh_, params);
  open3d::visualization::DrawGeometries({obj_mesh_}, "open3d", 720, 500, 600,
                                        800, false, false, false);

  return true;
}