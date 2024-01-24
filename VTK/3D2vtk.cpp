#include <open3d/Open3D.h>
// #include<open3d/io/
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkTexture.h>
#include<vtkOpenGLActor.h>
#include<vtkOpenGLPolyDataMapper.h>
#include<vtkDoubleArray.h>
#include <iostream>
int main() {
  std::string file_path = "D:\\text2\\texture.obj";

  std::shared_ptr<open3d::geometry::TriangleMesh> mesh =
      std::make_shared<open3d::geometry::TriangleMesh>();
  open3d::io::ReadTriangleMeshOptions read;
  open3d::io::ReadTriangleMeshFromOBJ(file_path, *mesh, read);
  // open3d::visualization::DrawGeometries({mesh}, "Registration result", 1028,
  // 720,
  //                                       50, 50, false, false, true);
  int num_materials = (int)mesh->textures_.size();
  //  std::cout<<num_materials<<std::endl;
  std::vector<std::vector<Eigen::Vector3d>> points;
  std::vector<std::vector<Eigen::Vector2d>> uvs;
  points.resize(num_materials);
  uvs.resize(num_materials);
  for (size_t i = 0; i < mesh->triangles_.size(); i++) {
    // triangle 里面存的是vertices_的绝对索引啊
    const auto &triangle = mesh->triangles_[i];
    int mi = mesh->triangle_material_ids_[i];

    for (size_t j = 0; j < 3; j++) {
      size_t idx = i * 3 + j;
      size_t vi = triangle(j);
      // points[mi].push_back(mesh->vertices_[vi].cast<float>());
      points[mi].push_back(mesh->vertices_[vi]);
      // triangle_uvs_与vertices_本来就是对应上的
      // 但是这不是有纹理要给分开成成快吗
      // 顶点的位置确定了就可以直接来一个uv的绝对索引
      uvs[mi].push_back(mesh->triangle_uvs_[idx]);
    }
  }

  // std::cout << points.size() << std::endl;
  // std::cout << uvs.size() << std::endl;
  // for (int i = 0; i < num_materials; ++i) {
  //   std::cout << points[i].size() << std::endl;
  //   std::cout << uvs[i].size() << std::endl;
  // }
  //   vtkRenderer* renderer = vtkRenderer::New();
  //  vtkRenderWindow* renderWindow = vtkRenderWindow::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  for (int i = 0; i < num_materials; ++i) {
    vtkSmartPointer<vtkImageData> vtkImg_data_ =
        vtkSmartPointer<vtkImageData>::New();
    vtkNew<vtkInformation> info;
    vtkImg_data_->SetDimensions(mesh->textures_[i].width_,
                                mesh->textures_[i].height_, 1);
    vtkImg_data_->SetScalarType(VTK_UNSIGNED_CHAR, info);
    vtkImg_data_->SetNumberOfScalarComponents(
        mesh->textures_[i].num_of_channels_, info);
    vtkImg_data_->AllocateScalars(info);
    unsigned char *ptr_vtk =
        static_cast<unsigned char *>(vtkImg_data_->GetScalarPointer());
    // auto it=mesh->textures_[i].PointerAs();
    for (int j = 0; j < mesh->textures_[i].data_.size(); ++j) {
      *ptr_vtk++ = mesh->textures_[i].data_[j];
    }
    // vtkImageActor* actor = vtkImageActor::New();
    // actor->SetInputData(vtkImg_data_);
    // renderer->AddActor(actor);
    // renderWindow->AddRenderer(renderer);
    // renderWindow->Render();
    vtkNew<vtkDoubleArray> textureCoordinates;
   textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");

    vtkSmartPointer<vtkPoints> mesh_point = vtkSmartPointer<vtkPoints>::New();
    for (int j = 0; j < points[i].size(); ++j) {
      mesh_point->InsertNextPoint(points[i][j].x(), points[i][j].y(),
                                  points[i][j].z());

       double uv[2]={uvs[i][j].x(),uvs[i][j].y()};
       textureCoordinates->InsertNextTuple(uv); 

    }
    std::cout << "points  " << points[i].size() << std::endl;
    std::cout << "mesh_point " << mesh_point->GetNumberOfPoints() << std::endl;
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    for (int j = 0; j < points[i].size() / 3; j++) {
      vtkSmartPointer<vtkTriangle> triangle =
          vtkSmartPointer<vtkTriangle>::New();
      triangle->GetPointIds()->SetId(0, 3 * j);
      triangle->GetPointIds()->SetId(1, 3 * j + 1);
      triangle->GetPointIds()->SetId(2, 3 * j + 2);
      cells->InsertNextCell(triangle);
    }

 

    std::cout << "triangle: " << cells->GetNumberOfCells() << std::endl;
    vtkSmartPointer<vtkPolyData> polygonPolyData =
        vtkSmartPointer<vtkPolyData>::New();
    polygonPolyData->SetPoints(mesh_point);
    polygonPolyData->SetPolys(cells);
   polygonPolyData->GetPointData()->SetTCoords(textureCoordinates);


    	vtkSmartPointer< vtkTexture > texture = 
		vtkSmartPointer< vtkTexture >::New();
    // vtkImg_data_->Get
	 texture->SetInputData( vtkImg_data_);
	 texture->InterpolateOn();


    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polygonPolyData);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    renderer->AddActor(actor);
  }

   
  renderer->SetBackground(1.0, 1.0, 1.0); 
  vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(800, 600 );
  renderWindow->Render();
  renderWindow->SetWindowName("PolyDataNew");

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return 0;
}