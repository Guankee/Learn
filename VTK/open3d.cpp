#include<iostream>
#include<open3d/Open3D.h>
// #include<open3d/core/CUDAUtils.h>
int main(){
  std::string file_path = "D:\\text2\\texture.obj";

  std::shared_ptr<open3d::geometry::TriangleMesh> mesh =
      std::make_shared<open3d::geometry::TriangleMesh>();
  open3d::io::ReadTriangleMeshOptions read;
  open3d::io::ReadTriangleMeshFromOBJ(file_path, *mesh, read);
  open3d::visualization::DrawGeometries({mesh}, "OpenGL", 1028,
                                        720, 50, 50, false, false, true);

  open3d::io::WriteTriangleMeshToOBJ("C:\\Users\\A015240\\Desktop\\tt\\2.obj",*mesh,
  false,true,true,true,true,true);
  std::string file_path2 = "C:\\Users\\A015240\\Desktop\\tt\\2.obj";

  std::shared_ptr<open3d::geometry::TriangleMesh> mesh2 =
      std::make_shared<open3d::geometry::TriangleMesh>();
  open3d::io::ReadTriangleMeshFromOBJ(file_path2, *mesh2, read);
  open3d::visualization::DrawGeometries({mesh2}, "2", 1028,
                                        720, 50, 50, false, false, true);

  return 0;
}