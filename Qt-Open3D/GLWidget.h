#pragma once
#include <open3d/Open3D.h>

#include <iostream>
#include <string>
#include <vector>
// #include<Eigen/Core>
// class QOpenGLWidget;
// #include <QOpenGLFunctions_3_3_Core>
// #include <QOpenGLFunctions_4_5_Core>
// #include <QOpenGLShaderProgram>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QOpenGLWidget>
#include <QString>
#include <QUrl>
#include <QtWidgets/QMainWindow>

const char *const TextureSimpleFragmentShader =
    "#version 330\n"
    "\n"
    "in vec2 fragment_uv;\n"
    "out vec4 FragColor;\n"
    "\n"
    "uniform sampler2D diffuse_texture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(diffuse_texture, fragment_uv);\n"
    "}\n";

const char *const TextureSimpleVertexShader =
    "#version 330\n"
    "\n"
    "in vec3 vertex_position;\n"
    "in vec2 vertex_uv;\n"
    "uniform mat4 MVP;\n"
    "\n"
    "out vec2 fragment_uv;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vertex_position, 1);\n"
    "    fragment_uv = vertex_uv;\n"
    "}\n";

class GLWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  GLWidget(QWidget *parent = nullptr);
  ~GLWidget();
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  bool ReadMeshOBJ(const QString &path);

 private:
  std::shared_ptr<open3d::geometry::TriangleMesh> obj_mesh_{nullptr};
};
