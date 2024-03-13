#include <GL/glew.h>
#include <iostream>

int main() {


	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// GLEW初始化失败
		std::cerr << "GLEW: " << glewGetErrorString(err) << std::endl;
		return 1;
	}

	std::cout << "GLEW: " << glewGetString(GLEW_VERSION) << std::endl;
	// 输出GLEW版本信息
	if (!GLEW_VERSION_3_3) {
		std::cerr << "OpenGL 3.3" << std::endl;
		return 1;
	}

	//// 检查OpenGL核心功能


	//// 检查特定扩展功能
	//if (!GLEW_ARB_vertex_program) {
	//	std::cerr << "扩展功能ARB_vertex_program不受支持" << std::endl;
	//	return 1;
	//}

	// ...

	return 0;
}