#pragma once
#include <iostream>
#include <open3d/Open3D.h>


#include <string>
#include <vector>
// #include<Eigen/Core>
// class QOpenGLWidget;
// #include <QOpenGLFunctions_3_3_Core>
// #include <QOpenGLFunctions_4_5_Core>
// #include <QOpenGLShaderProgram>
// #include <GL/glew.h>
// #define GLEW_STATIC
#include<GL/glew.h>
// #include<GL/

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
  bool Compile();  // 获取相关变量
  bool CompileShaders(
      const char *const vertex_shader_code,
      const char *const geometry_shader_code,
      const char *const fragment_shader_code);  // gl代码 编译具体
  bool ValidateShader(GLuint shader_index);
  bool ValidateProgram(GLuint program_index);
  bool UnbindGeometry();
    bool PrepareBinding(std::vector<Eigen::Vector3f> &points,
                      std::vector<Eigen::Vector2f> &uvs);
  bool BindGeometry();


 protected:
  virtual void initializeGL();
  // virtual void resizeGL(int w, int h);
  virtual void paintGL();

 protected:
  bool compiled_{false};  // qt的框架中init可以保证只编译一次
  bool bound_{false};
  GLuint vertex_shader_ = 0;
  GLuint geometry_shader_ = 0;
  GLuint fragment_shader_ = 0;
  GLuint program_ = 0;  // del program 看看 需不需要
  GLenum draw_arrays_mode_ = GL_POINTS;

  GLuint vertex_position_;
  GLuint vertex_uv_;
  GLuint texture_;
  GLuint MVP_;

  int num_materials_;
  //一团的在全部里面的偏移量
  std::vector<int> array_offsets_;
  //一团的数量
  std::vector<GLsizei> draw_array_sizes_;

  std::vector<GLuint> vertex_position_buffers_;
  std::vector<GLuint> vertex_uv_buffers_;
  std::vector<GLuint> texture_buffers_;

 private:
  std::shared_ptr<open3d::geometry::TriangleMesh> obj_mesh_{nullptr};
};
