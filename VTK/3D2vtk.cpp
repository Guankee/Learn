#include <open3d/Open3D.h>
// #include<open3d/io/
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkTriangle.h>
#include <vtkVersion.h>

#include <iostream>

// 注意为什么这可以快这么多
bool ImageProcessingUtility::textureMeshO3d2Vtk(
    const O3dTriangleMeshPtr mesh, const Eigen::Matrix4d transMatrix,
    std::vector<vtkSmartPointer<vtkActor>>& actors) {
  if (mesh == nullptr || mesh->IsEmpty()) {
    Logger::writeErrorLog(0, "textureMeshO3d2Vtk",
                          "O3dTriangleMeshPtr is null");
    return false;
  }

  mesh->Transform(transMatrix);

  int materialNum = (int)mesh->textures_.size();
  Logger::writeInfoLog(
      0, "textureMeshO3d2Vtk",
      std::string("materialNum is " + std::to_string(materialNum)).c_str());

  if (0 == materialNum) {
    Logger::writeErrorLog(0, "textureMeshO3d2Vtk", "materialNum is 0");
    return false;
  }

  actors.clear();
  std::vector<std::vector<Eigen::Vector3d>> points;
  std::vector<std::vector<Eigen::Vector2d>> uvs;
  std::vector<vtkSmartPointer<vtkImageData>> textures;
  points.resize(materialNum);
  uvs.resize(materialNum);
  textures.resize(materialNum);
  // 拆解对应上点坐标和纹理坐标
  // 事先不知道这个三角面片对应哪个纹理
  // 三个三个的是一个面片
  auto start_time = clock();
  for (size_t i = 0; i < mesh->triangles_.size(); i++) {
    const auto& triangle = mesh->triangles_[i];
    int mi = mesh->triangle_material_ids_[i];
    for (size_t j = 0; j < 3; j++) {
      size_t idx = i * 3 + j;
      size_t vi = triangle(j);
      points[mi].push_back(mesh->vertices_[vi]);
      uvs[mi].push_back(mesh->triangle_uvs_[idx]);
    }
  }
  auto end_time = clock();
  std::cout << " time for 1 triangles_: "
            << (double(end_time - start_time) / CLOCKS_PER_SEC) << " s"
            << std::endl;
  start_time = clock();
  for (int i = 0; i < materialNum; ++i) {
    vtkNew<vtkInformation> info;
    textures[i] = vtkSmartPointer<vtkImageData>::New();
    textures[i]->SetDimensions(mesh->textures_[i].width_,
                               mesh->textures_[i].height_, 1);
    textures[i]->SetScalarType(VTK_UNSIGNED_CHAR, info);
    textures[i]->SetNumberOfScalarComponents(
        mesh->textures_[i].num_of_channels_, info);
    textures[i]->AllocateScalars(info);
    unsigned char* ptr_vtk =
        static_cast<unsigned char*>(textures[i]->GetScalarPointer());
    std::copy(mesh->textures_[i].data_.begin(), mesh->textures_[i].data_.end(),
              ptr_vtk);
  }
  end_time = clock();
  std::cout << " time for 2 textures: "
            << (double(end_time - start_time) / CLOCKS_PER_SEC) << " s"
            << std::endl;
  actors.resize(materialNum);

  for (int i = 0; i < materialNum; ++i) {
    vtkNew<vtkDoubleArray> textureCoordinates;
    textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");
    textureCoordinates->SetNumberOfTuples(points[i].size());

    vtkSmartPointer<vtkPoints> meshPoint = vtkSmartPointer<vtkPoints>::New();
    meshPoint->SetNumberOfPoints(points[i].size());
    start_time = clock();
    for (int j = 0; j < points[i].size(); ++j) {
      meshPoint->InsertPoint(j, points[i][j].x(), points[i][j].y(),
                             points[i][j].z());
      double uv[2] = {uvs[i][j].x(), uvs[i][j].y()};
      textureCoordinates->InsertTuple(j, uv);
    }
    end_time = clock();
    std::cout << " time for 3 meshPoint: "
              << (double(end_time - start_time) / CLOCKS_PER_SEC) << " s"
              << std::endl;

    std::cout << "----------points  " << points[i].size() << std::endl;
    std::cout << "----------mesh_point " << meshPoint->GetNumberOfPoints()
              << std::endl;
    std::cout << "----------uv " << textureCoordinates->GetNumberOfTuples()
              << std::endl;
    start_time = clock();

    // 这一步和之前的方法差了特别多
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    size_t numTriangles = points[i].size() / 3;

    std::vector<vtkIdType> trianglePts(3);
    for (size_t j = 0; j < numTriangles; ++j) {
      trianglePts[0] = 3 * j;
      trianglePts[1] = 3 * j + 1;
      trianglePts[2] = 3 * j + 2;
      cells->InsertNextCell(3, trianglePts.data());
    }

    // vtkSmartPointer<vtkCellArray> cells =
    // vtkSmartPointer<vtkCellArray>::New(); size_t numTriangles =
    // points[i].size() / 3;

    // for (size_t j = 0; j < numTriangles; j += 4) {  //
    // 以4个三角形为一组进行处理
    //	// 使用 AVX 或 SSE 加载 4 个三角形的顶点索引
    //	__m128i indices0 = _mm_set_epi32(3 * (j + 3) + 2, 3 * (j + 3) + 1, 3 *
    //(j + 3), 3 * j + 2);
    //	__m128i indices1 = _mm_set_epi32(3 * (j + 2) + 2, 3 * (j + 2) + 1, 3 *
    //(j + 2), 3 * j + 1);
    //	__m128i indices2 = _mm_set_epi32(3 * (j + 1) + 2, 3 * (j + 1) + 1, 3 *
    //(j + 1), 3 * j);

    //	// 用来存储4个三角形的顶点索引
    //	vtkIdType *trianglePts=new vtkIdType[12];  //
    //每次处理四个三角形，12个顶点

    //	// 将索引加载到内存
    //	_mm_storeu_si128(reinterpret_cast<__m128i*>(trianglePts), indices0);
    //	_mm_storeu_si128(reinterpret_cast<__m128i*>(trianglePts + 4), indices1);
    //	_mm_storeu_si128(reinterpret_cast<__m128i*>(trianglePts + 8), indices2);

    //	// 将这些四个三角形的数据插入 vtkCellArray
    //	cells->InsertNextCell(3, &trianglePts[0]);  // 第一个三角形
    //	cells->InsertNextCell(3, &trianglePts[3]);  // 第二个三角形
    //	cells->InsertNextCell(3, &trianglePts[6]);  // 第三个三角形
    //	cells->InsertNextCell(3, &trianglePts[9]);  // 第四个三角形
    //}

    end_time = clock();
    std::cout << " time for 4 vtkTriangle: "
              << (double(end_time - start_time) / CLOCKS_PER_SEC) << " s"
              << std::endl;

    vtkSmartPointer<vtkPolyData> polygonPolyData =
        vtkSmartPointer<vtkPolyData>::New();
    polygonPolyData->SetPoints(meshPoint);
    polygonPolyData->SetPolys(cells);
    polygonPolyData->GetPointData()->SetTCoords(textureCoordinates);

    vtkSmartPointer<vtkTexture> vtktexture = vtkSmartPointer<vtkTexture>::New();
    vtktexture->SetInputData(textures[i]);
    vtktexture->InterpolateOn();
    vtktexture->SetMipmap(true);
    vtktexture->SetQualityTo32Bit();
    vtktexture->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polygonPolyData);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->SetTexture(vtktexture);
    actor->GetProperty()->LightingOff();
    actors[i] = actor;
  }

  return true;
}

int main() {
  const char* gg = vtkVersion::GetVTKVersion();

  // 输出 vtk 的版本号
  std::cout << "vtk版本" << gg << std::endl;

  // std::string file_path =
  // "C:\\Users\\A015240\\Desktop\\88\\sigletexture.obj";
  std::string file_path = "C:\\Users\\A015240\\Desktop\\obj3\\sigletexture.obj";
  // std::string file_path = "D:\\ply\\1\\1.obj";

  std::shared_ptr<open3d::geometry::TriangleMesh> mesh =
      std::make_shared<open3d::geometry::TriangleMesh>();
  open3d::io::ReadTriangleMeshOptions read;
  open3d::io::ReadTriangleMeshFromOBJ(file_path, *mesh, read);
  // open3d::visualization::DrawGeometries({ mesh }, "Open3D", 1028,
  //	720, 50, 50, false, false, true);
  int num_materials = (int)mesh->textures_.size();

  //  std::cout<<num_materials<<std::endl;
  std::vector<std::vector<Eigen::Vector3d>> points;
  std::vector<std::vector<Eigen::Vector2d>> uvs;
  points.resize(num_materials);
  uvs.resize(num_materials);
  // 拆解对应上点坐标和纹理坐标
  // 事先不知道这个三角面片对应哪个纹理
  // 三个三个的是一个面片

  for (size_t i = 0; i < mesh->triangles_.size(); i++) {
    // triangle 里面存的是vertices_的绝对索引啊
    const auto& triangle = mesh->triangles_[i];
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

  vtkNew<vtkNamedColors> colors;

  //   vtkRenderer* renderer = vtkRenderer::New();
  vtkRenderWindow* renderWindowImage = vtkRenderWindow::New();
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
    unsigned char* ptr_vtk =
        static_cast<unsigned char*>(vtkImg_data_->GetScalarPointer());
    // auto it=mesh->textures_[i].PointerAs();
    for (int j = 0; j < mesh->textures_[i].data_.size(); ++j) {
      *ptr_vtk++ = mesh->textures_[i].data_[j];
    }

    //     vtkImageActor* actor = vtkImageActor::New();
    //     actor->SetInputData(vtkImg_data_);
    //     renderer->AddActor(actor);
    //     renderWindowImage->AddRenderer(renderer);
    //     renderWindowImage->Render();
    //     // 创建交互器
    //     vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    //         vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //             vtkSmartPointer<vtkInteractorStyleImage> Style =
    //         vtkSmartPointer<vtkInteractorStyleImage>::New();
    //     interactor->SetRenderWindow(renderWindowImage);
    // interactor->SetInteractorStyle(Style);
    //     // 渲染并启动窗口事件循环
    //     renderWindowImage->Render();
    //     interactor->Start();
    vtkNew<vtkDoubleArray> textureCoordinates;
    textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");

    vtkSmartPointer<vtkPoints> mesh_point = vtkSmartPointer<vtkPoints>::New();
    for (int j = 0; j < points[i].size(); ++j) {
      mesh_point->InsertNextPoint(points[i][j].x(), points[i][j].y(),
                                  points[i][j].z());

      double uv[2] = {uvs[i][j].x(), uvs[i][j].y()};
      textureCoordinates->InsertNextTuple(uv);
    }
    std::cout << "points  " << points[i].size() << std::endl;
    std::cout << "mesh_point " << mesh_point->GetNumberOfPoints() << std::endl;
    std::cout << "uv " << textureCoordinates->GetNumberOfTuples() << std::endl;
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

    vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
    // vtkImg_data_->Get
    texture->SetInputData(vtkImg_data_);
    texture->InterpolateOn();
    texture->SetMipmap(true);
    texture->SetQualityTo32Bit();
    texture->Update();
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polygonPolyData);
    // mapper->SetInterpolationToPhong();
    // mapper->SeamlessUOn();

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->SetTexture(texture);
    actor->GetProperty()->LightingOff();

    // actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
    renderer->AddActor(actor);
  }

  renderer->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(800, 600);
  renderWindow->Render();
  renderWindow->SetWindowName("VTK");

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return 0;
}
