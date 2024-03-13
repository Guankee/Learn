#include <iostream>
#include "GLWidget.h"
GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
	ReadMeshOBJ("C:\\Users\\A015240\\Desktop\\tt\\1.obj");
	setAcceptDrops(true);
}

GLWidget::~GLWidget() {}

void GLWidget::dragEnterEvent(QDragEnterEvent* event) {
	std::cout << "dragEnterEvent " << std::endl;
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
		// 目前只写放入单个obj文件的
		QString path = urls.first().toLocalFile();
		// std::cout<<path.toStdString()<<std::endl;
		bool ret = ReadMeshOBJ(path);

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
	// open3d::visualization::DrawGeometries({obj_mesh_}, "open3d", 720, 500, 600,
	//                                       800, false, false, false);

	return true;
}
void GLWidget::initializeGL() {
	if (!compiled_) {
		Compile();
	}
	if (!bound_) {
		BindGeometry();
	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void GLWidget::paintGL() {
	glUseProgram(program_);
	for (int mi = 0; mi < num_materials_; ++mi) {
		// glUniformMatrix4fv(MVP_, 1, GL_FALSE, view.GetMVPMatrix().data());

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

}


bool GLWidget::Compile() {
	if (!CompileShaders(TextureSimpleVertexShader, NULL,
						TextureSimpleFragmentShader)) {
		// std::cout < "Compiling shaders failed";
		return false;
	}
	vertex_position_ = glGetAttribLocation(program_, "vertex_position");
	vertex_uv_ = glGetAttribLocation(program_, "vertex_uv");
	texture_ = glGetUniformLocation(program_, "diffuse_texture");
	MVP_ = glGetUniformLocation(program_, "MVP");
	return true;
}
bool GLWidget::ValidateProgram(GLuint program_index) {
	GLint result = GL_FALSE;
	int info_log_length;
	glGetProgramiv(program_index, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		glGetProgramiv(program_index, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0) {
			std::vector<char> error_message(info_log_length + 1);
			glGetShaderInfoLog(program_index, info_log_length, NULL,
							   &error_message[0]);
			// utility::LogWarning("Shader error: {}", &error_message[0]);
		}
		return false;
	}
	return true;
}
bool GLWidget::CompileShaders(const char* const vertex_shader_code,
							  const char* const geometry_shader_code,
							  const char* const fragment_shader_code) {
	if (compiled_) {
		return true;
	}
	if (vertex_shader_code != NULL) {
		vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* vertex_shader_code_buffer = vertex_shader_code;
		glShaderSource(vertex_shader_, 1, &vertex_shader_code_buffer, NULL);
		glCompileShader(vertex_shader_);
		if (!ValidateShader(vertex_shader_)) {
			return false;
		}
	}

	if (geometry_shader_code != NULL) {
		geometry_shader_ = glCreateShader(GL_GEOMETRY_SHADER);
		const GLchar* geometry_shader_code_buffer = geometry_shader_code;
		glShaderSource(geometry_shader_, 1, &geometry_shader_code_buffer, NULL);
		glCompileShader(geometry_shader_);
		if (!ValidateShader(geometry_shader_)) {
			return false;
		}
	}

	if (fragment_shader_code != NULL) {
		fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
		const GLchar* fragment_shader_code_buffer = fragment_shader_code;
		glShaderSource(fragment_shader_, 1, &fragment_shader_code_buffer, NULL);
		glCompileShader(fragment_shader_);
		if (!ValidateShader(fragment_shader_)) {
			return false;
		}
	}
	program_ = glCreateProgram();
	if (vertex_shader_code != NULL) {
		glAttachShader(program_, vertex_shader_);
	}
	if (geometry_shader_code != NULL) {
		glAttachShader(program_, geometry_shader_);
	}
	if (fragment_shader_code != NULL) {
		glAttachShader(program_, fragment_shader_);
	}
	glLinkProgram(program_);
	if (!ValidateProgram(program_)) {
		return false;
	}
	if (vertex_shader_code != NULL) {
		glDeleteShader(vertex_shader_);
	}
	if (geometry_shader_code != NULL) {
		glDeleteShader(geometry_shader_);
	}
	if (fragment_shader_code != NULL) {
		glDeleteShader(fragment_shader_);
	}

	compiled_ = true;
	return true;
}
bool GLWidget::ValidateShader(GLuint shader_index) {
	GLint result = GL_FALSE;
	int info_log_length;
	glGetShaderiv(shader_index, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		glGetShaderiv(shader_index, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0) {
			std::vector<char> error_message(info_log_length + 1);
			glGetShaderInfoLog(shader_index, info_log_length, NULL,
							   &error_message[0]);
			// utility::LogWarning("Shader error: {}", &error_message[0]);
			// std::cout < "Shader error: " << error_message << std::endl;
		}
		return false;
	}
	return true;
}
bool GLWidget::UnbindGeometry() {
	if (bound_) {
		for (auto buf : vertex_position_buffers_) {
			glDeleteBuffers(1, &buf);
		}
		for (auto buf : vertex_uv_buffers_) {
			glDeleteBuffers(1, &buf);
		}
		for (auto buf : texture_buffers_) {
			glDeleteTextures(1, &buf);
		}

		vertex_position_buffers_.clear();
		vertex_uv_buffers_.clear();
		texture_buffers_.clear();
		draw_array_sizes_.clear();
		array_offsets_.clear();
		num_materials_ = 0;
		bound_ = false;
	}
	return true;
}
bool GLWidget::PrepareBinding(std::vector<Eigen::Vector3f>& points,
							  std::vector<Eigen::Vector2f>& uvs) {
	std::vector<std::vector<Eigen::Vector3f>> tmp_points;
	std::vector<std::vector<Eigen::Vector2f>> tmp_uvs;
	num_materials_ = (int)obj_mesh_->textures_.size();
	array_offsets_.resize(num_materials_);
	draw_array_sizes_.resize(num_materials_);
	vertex_position_buffers_.resize(num_materials_);
	vertex_uv_buffers_.resize(num_materials_);
	texture_buffers_.resize(num_materials_);

	tmp_points.resize(num_materials_);
	tmp_uvs.resize(num_materials_);

	for (size_t i = 0; i < obj_mesh_->triangles_.size(); i++) {
		const auto& triangle = obj_mesh_->triangles_[i];
		int mi = obj_mesh_->triangle_material_ids_[i];

		for (size_t j = 0; j < 3; j++) {
			size_t idx = i * 3 + j;
			size_t vi = triangle(j);
			tmp_points[mi].push_back(obj_mesh_->vertices_[vi].cast<float>());
			tmp_uvs[mi].push_back(obj_mesh_->triangle_uvs_[idx].cast<float>());
		}
	}
	// Bind textures
	for (int mi = 0; mi < num_materials_; ++mi) {
		glGenTextures(1, &texture_buffers_[mi]);
		glBindTexture(GL_TEXTURE_2D, texture_buffers_[mi]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, obj_mesh_->textures_[mi].width_,
					 obj_mesh_->textures_[mi].height_, 0, GL_RGB, GL_UNSIGNED_BYTE,
					 obj_mesh_->textures_[mi].data_.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	// Prepared chunk of points and uvs
	points.clear();
	uvs.clear();
	for (int mi = 0; mi < num_materials_; ++mi) {
		points.insert(points.end(), tmp_points[mi].begin(), tmp_points[mi].end());
		uvs.insert(uvs.end(), tmp_uvs[mi].begin(), tmp_uvs[mi].end());
	}

	draw_arrays_mode_ = GL_TRIANGLES;
	return true;
}
// GL_STATIC_DRAW 用这个可能会慢
bool GLWidget::BindGeometry() {
	UnbindGeometry();
	std::vector<Eigen::Vector3f> points;
	std::vector<Eigen::Vector2f> uvs;
	if (!PrepareBinding(points, uvs)) {
		return false;
	}
	for (int mi = 0; mi < num_materials_; ++mi) {
		glGenBuffers(1, &vertex_position_buffers_[mi]);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_position_buffers_[mi]);
		glBufferData(GL_ARRAY_BUFFER,
					 draw_array_sizes_[mi] * sizeof(Eigen::Vector3f),
					 points.data() + array_offsets_[mi], GL_STATIC_DRAW);

		glGenBuffers(1, &vertex_uv_buffers_[mi]);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_uv_buffers_[mi]);
		glBufferData(GL_ARRAY_BUFFER,
					 draw_array_sizes_[mi] * sizeof(Eigen::Vector2f),
					 uvs.data() + array_offsets_[mi], GL_STATIC_DRAW);
	}
	bound_ = true;
	return true;
}