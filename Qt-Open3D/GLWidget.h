#pragma once
#include<open3d/Open3D.h>
// #include <open3d/visualization/utility/GLHelper.h>
// #include <open3d/geometry/TriangleMesh.h>
// #include <open3d/io/TriangleMeshIO.h>

#include <iostream>
#include <string>
#include <vector>
// #include<Eigen/Core>
// class QOpenGLWidget;
#include <QOpenGLFunctions_3_3_Core>
// #include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QtWidgets/QMainWindow>

// #include <open3d/open3d.h>


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

class GLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
  Q_OBJECT
 public:
  GLWidget(QWidget *parent = nullptr);
  ~GLWidget();
  void ReadObj(const std::string &path);
  // 考虑使用QOpenGLShaderProgram
  bool CompileShaders(const char *const vertex_shader_code,
                      const char *const geometry_shader_code,
                      const char *const fragment_shader_code);
  bool ValidateShader(GLuint shader_index);
  bool Compile();
  void UnbindGeometry();
  bool PrepareBinding();
  bool BindGeometry();
  bool RenderGeometry();

 protected:
  virtual void initializeGL() override;
  virtual void resizeGL(int w, int h) override;
  virtual void paintGL() override;

 private:
  open3d::geometry::TriangleMesh mesh_;
  QOpenGLShaderProgram shaderProgram;


  // 数据相关 gl_index
  GLuint vertex_position_;
  GLuint vertex_uv_;
  GLuint texture_;
  GLuint MVP_;

  int num_materials_;
  std::vector<int> array_offsets_;
  std::vector<GLsizei> draw_array_sizes_;
  std::vector<GLuint> vertex_position_buffers_;
  std::vector<GLuint> vertex_uv_buffers_;
  std::vector<GLuint> texture_buffers_;
  // shared code
  bool compiled_{false};
  GLuint vertex_shader_ = 0;
  GLuint geometry_shader_ = 0;
  GLuint fragment_shader_ = 0;
  GLuint program_ = 0;
  GLenum draw_arrays_mode_ = GL_POINTS;
  GLsizei draw_arrays_size_ = 0;
  // 用于监控vao vbo veo 是否绑定到GPU
  bool bound_{false};
  // Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor>MVP_;
  //控制相机矩阵 
  open3d::visualization::gl_util::GLMatrix4f projection_matrix_;
  open3d::visualization::gl_util::GLMatrix4f view_matrix_;
  open3d::visualization::gl_util::GLMatrix4f model_matrix_;
  open3d::visualization::gl_util::GLMatrix4f MVP_matrix_;

  std::vector<Eigen::Vector3f> points_;
  std::vector<Eigen::Vector2f> uvs_;
};
