#include "GLWidget.h"
GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  ReadObj("D:\\textures\\texturemeshwithopt.obj");
   initializeOpenGLFunctions();
}

GLWidget::~GLWidget() {}
void GLWidget::ReadObj(const std::string &path) {
  std::string file_path = "D:\\textures\\texturemeshwithopt.obj";
  // std::shared_ptr<open3d::geometry::TriangleMesh> pcd(
  //     new open3d::geometry::TriangleMesh);
  // if (mesh_ == nullptr) {
  //   mesh_ = std::make_shared<open3d::geometry::TriangleMesh>();
  // }
  open3d::io::ReadTriangleMeshOptions read;
  open3d::io::ReadTriangleMeshFromOBJ(file_path, mesh_, read);
  // 显示PLY文件
  // open3d::visualization::DrawGeometries({pcd}, "Registration result", 1028,
  // 768,
  //                                       50, 50, false, false, true);
}
void GLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  Compile();
  BindGeometry();

  glEnable(GL_CULL_FACE);
  // glDepthFunc(GLenum(option.GetGLDepthFunc()));
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void GLWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}
void GLWidget::paintGL() {
// view_matrix_ = open3d::visualization::gl_util::LookAt(eye_, lookat_, up_);  
// MVP_matrix_ = projection_matrix_ * view_matrix_ * model_matrix_;
// MVP_matrix_=Eigen::
MVP_matrix_.setIdentity();
RenderGeometry();

}
bool GLWidget::CompileShaders(const char *const vertex_shader_code,
                              const char *const geometry_shader_code,
                              const char *const fragment_shader_code) {
  if (compiled_) {
    return true;
  }
  bool success;
  shaderProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                 vertex_shader_code);
  shaderProgram.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                 fragment_shader_code);
  success = shaderProgram.link();
  compiled_ = true;
  return success;
}
//glew是gl的一种扩展
// bool GLWidget::CompileShaders(const char *const vertex_shader_code,
//                               const char *const geometry_shader_code,
//                               const char *const fragment_shader_code) {
//   if (compiled_) {
//     return true;
//   }
//   if (vertex_shader_code != NULL) {
//     vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
//     const GLchar *vertex_shader_code_buffer = vertex_shader_code;
//     glShaderSource(vertex_shader_, 1, &vertex_shader_code_buffer, NULL);
//     glCompileShader(vertex_shader_);
//     if (!ValidateShader(vertex_shader_)) {
//       return false;
//     }
//   }
//   if (geometry_shader_code != NULL) {
//     geometry_shader_ = glCreateShader(GL_GEOMETRY_SHADER);
//     const GLchar *geometry_shader_code_buffer = geometry_shader_code;
//     glShaderSource(geometry_shader_, 1, &geometry_shader_code_buffer, NULL);
//     glCompileShader(geometry_shader_);
//     if (!ValidateShader(geometry_shader_)) {
//       return false;
//     }
//   }

//   if (fragment_shader_code != NULL) {
//     fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
//     const GLchar *fragment_shader_code_buffer = fragment_shader_code;
//     glShaderSource(fragment_shader_, 1, &fragment_shader_code_buffer, NULL);
//     glCompileShader(fragment_shader_);
//     if (!ValidateShader(fragment_shader_)) {
//       return false;
//     }
//   }
//   program_ = glCreateProgram();
//   if (vertex_shader_code != NULL) {
//     glAttachShader(program_, vertex_shader_);
//   }
//   if (geometry_shader_code != NULL) {
//     glAttachShader(program_, geometry_shader_);
//   }
//   if (fragment_shader_code != NULL) {
//     glAttachShader(program_, fragment_shader_);
//   }
//   glLinkProgram(program_);
//   // if (!ValidateProgram(program_)) {
//   //   return false;
//   // }
//   if (vertex_shader_code != NULL) {
//     glDeleteShader(vertex_shader_);
//   }
//   if (geometry_shader_code != NULL) {
//     glDeleteShader(geometry_shader_);
//   }
//   if (fragment_shader_code != NULL) {
//     glDeleteShader(fragment_shader_);
//   }

//   compiled_ = true;
//   return true;
// }
// bool GLWidget::ValidateShader(GLuint shader_index) {
//   GLint result = GL_FALSE;
//   int info_log_length;
//   glGetShaderiv(shader_index, GL_COMPILE_STATUS, &result);
//   if (result == GL_FALSE) {
//     glGetShaderiv(shader_index, GL_INFO_LOG_LENGTH, &info_log_length);
//     if (info_log_length > 0) {
//       std::vector<char> error_message(info_log_length + 1);
//       glGetShaderInfoLog(shader_index, info_log_length, NULL,
//                          &error_message[0]);
//       // utility::LogWarning("Shader error: {}", &error_message[0]);
//       std::cout << "Shader error:" << &error_message[0] << std::endl;
//     }
//     return false;
//   }
//   return true;
// }
bool GLWidget::Compile() {
  if (!CompileShaders(TextureSimpleVertexShader, NULL,
                      TextureSimpleFragmentShader)) {
    std::cout << "Compiling shaders failed." << std::endl;
    return false;
  }
  // vertex_position_ = glGetAttribLocation(program_, "vertex_position");
  // vertex_uv_ = glGetAttribLocation(program_, "vertex_uv");
  // texture_ = glGetUniformLocation(program_, "diffuse_texture");
  // MVP_ = glGetUniformLocation(program_, "MVP");
  return true;
}
void GLWidget::UnbindGeometry() {
  if (bound_) {
    makeCurrent();
    for (auto buf : vertex_position_buffers_) {
      glDeleteBuffers(1, &buf);
    }
    for (auto buf : vertex_uv_buffers_) {
      glDeleteBuffers(1, &buf);
    }
    for (auto buf : texture_buffers_) {
      glDeleteTextures(1, &buf);
    }
    doneCurrent();
    vertex_position_buffers_.clear();
    vertex_uv_buffers_.clear();
    texture_buffers_.clear();
    draw_array_sizes_.clear();
    array_offsets_.clear();
    num_materials_ = 0;
    bound_ = false;
  }
}
bool GLWidget::PrepareBinding() {
  std::vector<std::vector<Eigen::Vector3f>> tmp_points;
  std::vector<std::vector<Eigen::Vector2f>> tmp_uvs;

  num_materials_ = (int)mesh_.textures_.size();
  array_offsets_.resize(num_materials_);
  draw_array_sizes_.resize(num_materials_);
  vertex_position_buffers_.resize(num_materials_);
  vertex_uv_buffers_.resize(num_materials_);
  texture_buffers_.resize(num_materials_);

  tmp_points.resize(num_materials_);
  tmp_uvs.resize(num_materials_);

  // Bind vertices and uvs per material
  for (size_t i = 0; i < mesh_.triangles_.size(); i++) {
    // triangle 里面存的是vertices_的绝对索引啊
    const auto &triangle = mesh_.triangles_[i];
    int mi = mesh_.triangle_material_ids_[i];

    for (size_t j = 0; j < 3; j++) {
      size_t idx = i * 3 + j;
      size_t vi = triangle(j);
      tmp_points[mi].push_back(mesh_.vertices_[vi].cast<float>());
      //triangle_uvs_与vertices_本来就是对应上的 但是这不是有纹理要给分开成成快吗
      //顶点的位置确定了就可以直接来一个uv的绝对索引
      tmp_uvs[mi].push_back(mesh_.triangle_uvs_[idx].cast<float>());
    }
  }

  // Bind textures
  for (int mi = 0; mi < num_materials_; ++mi) {
    //给gpu传纹理 索引存起来  后面传给clsl的程序才是需要用到那个纹理单元呢
    glGenTextures(1, &texture_buffers_[mi]);
    glBindTexture(GL_TEXTURE_2D, texture_buffers_[mi]);

    GLenum format, type;
    auto it =
        open3d::visualization::gl_util::texture_format_map_.find(mesh_.textures_[mi].num_of_channels_);
    if (it == open3d::visualization::gl_util::texture_format_map_.end()) {
      // utility::LogWarning("Unknown texture format, abort!");
      return false;
    }
    format = it->second;

    it = open3d::visualization::gl_util::texture_type_map_.find(mesh_.textures_[mi].bytes_per_channel_);
    if (it == open3d::visualization::gl_util::texture_type_map_.end()) {
      // utility::LogWarning("Unknown texture type, abort!");
      return false;
    }
    type = it->second;

    glTexImage2D(GL_TEXTURE_2D, 0, format, mesh_.textures_[mi].width_,
                 mesh_.textures_[mi].height_, 0, format, type,
                 mesh_.textures_[mi].data_.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // Point separations
  array_offsets_[0] = 0;
  draw_array_sizes_[0] = static_cast<int>(tmp_points[0].size());
  for (int mi = 1; mi < num_materials_; ++mi) {
    draw_array_sizes_[mi] = static_cast<int>(tmp_points[mi].size());
    array_offsets_[mi] = array_offsets_[mi - 1] + draw_array_sizes_[mi - 1];
  }

  // Prepared chunk of points and uvs
  points_.clear();
  uvs_.clear();
  for (int mi = 0; mi < num_materials_; ++mi) {
    points_.insert(points_.end(), tmp_points[mi].begin(), tmp_points[mi].end());
    uvs_.insert(uvs_.end(), tmp_uvs[mi].begin(), tmp_uvs[mi].end());
  }

  draw_arrays_mode_ = GL_TRIANGLES;
  return true;
}
bool GLWidget::BindGeometry() {
  void UnbindGeometry();

  // std::vector<Eigen::Vector3f> points;
  // std::vector<Eigen::Vector2f> uvs;
  if (!PrepareBinding()) {
    std::cout << "Binding failed when preparing data." << std::endl;
    return false;
  }

  // Create buffers and bind the geometry
  for (int mi = 0; mi < num_materials_; ++mi) {
    glGenBuffers(1, &vertex_position_buffers_[mi]);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_position_buffers_[mi]);
    glBufferData(GL_ARRAY_BUFFER,
                 draw_array_sizes_[mi] * sizeof(Eigen::Vector3f),
                 points_.data() + array_offsets_[mi], GL_STATIC_DRAW);

    glGenBuffers(1, &vertex_uv_buffers_[mi]);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_uv_buffers_[mi]);
    glBufferData(GL_ARRAY_BUFFER,
                 draw_array_sizes_[mi] * sizeof(Eigen::Vector2f),
                 uvs_.data() + array_offsets_[mi], GL_STATIC_DRAW);
  }
  bound_ = true;
  return true;
}
bool GLWidget::RenderGeometry() {
  glUseProgram(program_);
  for (int mi = 0; mi < num_materials_; ++mi) {

    glUniformMatrix4fv(MVP_, 1, GL_FALSE, MVP_matrix_.data());

    glUniform1i(texture_, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_buffers_[mi]);

    glEnableVertexAttribArray(vertex_position_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_position_buffers_[mi]);
    glVertexAttribPointer(vertex_position_, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(vertex_uv_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_uv_buffers_[mi]);
    glVertexAttribPointer(vertex_uv_, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glDrawArrays(draw_arrays_mode_, 0, draw_array_sizes_[mi]);

    glDisableVertexAttribArray(vertex_position_);
    glDisableVertexAttribArray(vertex_uv_);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  return true;
}