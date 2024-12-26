#include "mainwindow.h"

#include <vector>

#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initVTK();
  setAcceptDrops(true);  // 启用拖放功能
  connect(ui->planBtn, &QPushButton::toggled, this, &MainWindow::slotPlanStyle);
  connect(ui->cleanBtn, &QPushButton::clicked, this,
          &MainWindow::slotCleanPoly);
  connect(ui->edgeBtn, &QPushButton::toggled, this, &MainWindow::slotEdgeOn);
  loadData();
}

MainWindow::~MainWindow() { delete ui; }
void MainWindow::initVTK() {
  if (nullptr == curVtkcamera) {
    curVtkcamera = vtkSmartPointer<vtkCamera>::New();
  }
  if (nullptr == curVtkRenderer) {
    curVtkRenderer = vtkSmartPointer<vtkRenderer>::New();
  }
  if (nullptr == qvtkInteractor) {
    qvtkInteractor = vtkSmartPointer<QVTKInteractor>::New();
  }
  curVtkRenderer->SetBackground2(0.529, 0.8078,
                                 0.92157);       // 设置页面底部颜色值
  curVtkRenderer->SetBackground(1.0, 1.0, 1.0);  // 设置页面顶部颜色值
  curVtkRenderer->SetGradientBackground(true);

  qvtkInteractor = ui->qVTKOpenglWidget->interactor();
  curVtkcamera = curVtkRenderer->GetActiveCamera();
  curentVtkWindow = ui->qVTKOpenglWidget->renderWindow();
  curentVtkWindow->AddRenderer(curVtkRenderer);

  // lightFront = vtkSmartPointer<vtkLight>::New();
  // lightFront->SetLightTypeToSceneLight();
  // curVtkRenderer->AddLight(lightFront);

  // lightBack = vtkSmartPointer<vtkLight>::New();
  // lightBack->SetLightTypeToSceneLight();
  // lightBack->SetIntensity(0.5);
  // // lightBack->SetColor(195.0/255, 176.0/255, 145.0/255);
  // curVtkRenderer->AddLight(lightBack);

  qvtkInteractor->SetRenderWindow(curentVtkWindow);
  qvtkInteractor->SetInteractorStyle(cameraStyle);
  qvtkInteractor->Initialize();
}
void MainWindow::loadData()
{
	auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
	open3d::io::ReadTriangleMeshOptions read;
	open3d::io::ReadTriangleMeshFromOBJ("C:\\Users\\A015240\\Desktop\\Obj\\meshwithoutglb.obj",
		*mesh, read);
	vtkSmartPointer<vtkActor> actor =
		textureMeshO3d2Vtk(mesh, Eigen::Matrix4d::Identity());
	addTextureMesh(actor, mesh);
	render();
}
void MainWindow::addPolyData(vtkSmartPointer<vtkPolyData> polydata,
                             bool append) {
  if (!append) {
    cleanMeshData();
  }
  if (nullptr == curPolyData) {
    curPolyData = vtkSmartPointer<vtkPolyData>::New();
  }
  curPolyData = polydata;
  double* center = polydata->GetCenter();
  curVtkcamera->SetFocalPoint(center[0], center[1], center[2]);  // 设焦点

  auto mapper = vtkSmartPointer<
      vtkPolyDataMapper>::New();  // 映射器，可以根据点、线、面构造图形
  mapper->SetInputData(curPolyData);
  mapper->Update();

  curactor->SetMapper(mapper);
  // curactor->GetProperty()->EdgeVisibilityOn();
  //     // curactor->GetProperty()->e
  // curactor->GetProperty()->SetEdgeColor(0.0, 0.0, 0.0);  //
  // 设置边界线颜色为黑色 curactor->GetProperty()->SetLineWidth(1.0);  //
  // 设置边界线宽度

  //   ouraud 插值 (SetInterpolationToGouraud)：
  // 光照计算在每个顶点进行，然后在多边形内部使用线性插值计算光照值。这会产生较为平滑的光照过渡。
  // curactor->GetProperty()->SetInterpolationToGouraud();

  curVtkRenderer->AddActor(curactor);
}
void MainWindow::addTextureMesh(
    vtkSmartPointer<vtkActor> actor,
    std::shared_ptr<open3d::geometry::TriangleMesh> mesh, bool append) {
  if (!append) {
    cleanMeshData();
  }
  open3d::geometry::PointCloud cloud;
  cloud.points_ = mesh->vertices_;
  Eigen::Vector3d center = cloud.GetCenter();
  curVtkcamera->SetFocalPoint(center[0], center[1], center[2]);
  curactor = actor;
  curVtkRenderer->AddActor(curactor);
  return;
}
vtkSmartPointer<vtkActor> MainWindow::textureMeshO3d2Vtk(
    std::shared_ptr<open3d::geometry::TriangleMesh> mesh,
    const Eigen::Matrix4d& transMatrix) {
  if (mesh == nullptr || mesh->IsEmpty()) {
    return nullptr;
  }

  mesh->Transform(transMatrix);

  int materialNum = static_cast<int>(mesh->textures_.size());

  if (materialNum != 1) {
    return nullptr;
  }
  std::vector<Eigen::Vector3d> points;
  std::vector<Eigen::Vector2d> uvs;
  for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
    const auto& triangle = mesh->triangles_[i];
    int mi = mesh->triangle_material_ids_[i];
    for (size_t j = 0; j < 3; ++j) {
      size_t idx = i * 3 + j;
      size_t vi = triangle(j);
      points.push_back(mesh->vertices_[vi]);
      uvs.push_back(mesh->triangle_uvs_[idx]);
    }
  }

  vtkSmartPointer<vtkImageData> textureImage =
      vtkSmartPointer<vtkImageData>::New();
  vtkNew<vtkInformation> info;
  textureImage->SetDimensions(mesh->textures_[0].width_,
                              mesh->textures_[0].height_, 1);
  textureImage->SetScalarType(VTK_UNSIGNED_CHAR, info);
  textureImage->SetNumberOfScalarComponents(mesh->textures_[0].num_of_channels_,
                                            info);
  textureImage->AllocateScalars(info);
  unsigned char* ptr_vtk =
      static_cast<unsigned char*>(textureImage->GetScalarPointer());
  std::copy(mesh->textures_[0].data_.begin(), mesh->textures_[0].data_.end(),
            ptr_vtk);

  vtkSmartPointer<vtkDoubleArray> textureCoordinates =
      vtkSmartPointer<vtkDoubleArray>::New();
  textureCoordinates->SetNumberOfComponents(2);
  textureCoordinates->SetName("TextureCoordinates");
  textureCoordinates->SetNumberOfTuples(points.size());

  vtkSmartPointer<vtkPoints> meshPoints = vtkSmartPointer<vtkPoints>::New();
  meshPoints->SetNumberOfPoints(points.size());
  for (size_t j = 0; j < points.size(); ++j) {
    meshPoints->InsertPoint(j, points[j].x(), points[j].y(), points[j].z());
    double uv[2] = {uvs[j].x(), uvs[j].y()};
    textureCoordinates->InsertTuple(j, uv);
  }

  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
  size_t numTriangles = points.size() / 3;
  std::vector<vtkIdType> trianglePts(3);
  for (size_t j = 0; j < numTriangles; ++j) {
    trianglePts[0] = 3 * j;
    trianglePts[1] = 3 * j + 1;
    trianglePts[2] = 3 * j + 2;
    cells->InsertNextCell(3, trianglePts.data());
  }

  vtkSmartPointer<vtkPolyData> polygonPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  polygonPolyData->SetPoints(meshPoints);
  polygonPolyData->SetPolys(cells);
  polygonPolyData->GetPointData()->SetTCoords(textureCoordinates);

  vtkSmartPointer<vtkTexture> vtktexture = vtkSmartPointer<vtkTexture>::New();
  vtktexture->SetInputData(textureImage);
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
  vtkSmartPointer<vtkProperty> frontProperty = vtkSmartPointer<vtkProperty>::New();

  frontProperty->LightingOff();
  //frontProperty->SetShading(false);
  actor->SetProperty(frontProperty);

  vtkSmartPointer<vtkProperty> backfaceProperty = vtkSmartPointer<vtkProperty>::New();
  //backfaceProperty->LightingOff();
  backfaceProperty->SetAmbient(0.2);  
  backfaceProperty->SetDiffuse(0.2); 
  backfaceProperty->SetSpecular(0.1);

  actor->SetBackfaceProperty(backfaceProperty);
  //vtkSmartPointer<vtkProperty> backfaceProperty = curactor->GetBackfaceProperty();
  //backfaceProperty->LightingOn();
 /* actor->GetProperty()->BackfaceCullingOn();*/
  return actor;
}

void MainWindow::cleanMeshData() {
  vtkSmartPointer<vtkActorCollection> actors = curVtkRenderer->GetActors();
  actors->InitTraversal();
  for (int i = 0; i < actors->GetNumberOfItems(); i++) {
    curVtkRenderer->RemoveActor(actors->GetNextActor());
  }
  actors->RemoveAllItems();
}
void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urlList = event->mimeData()->urls();
    if (urlList.size() == 1 && urlList[0].toLocalFile().endsWith(".ply")) {
      event->acceptProposedAction();
    }else if (urlList.size() == 1 && urlList[0].toLocalFile().endsWith(".obj")) {
        event->acceptProposedAction();
    }

  }
}

void MainWindow::dropEvent(QDropEvent* event) {
  QList<QUrl> urlList = event->mimeData()->urls();
  if (urlList.size() == 1) {
    QString filePath = urlList[0].toLocalFile();
    if (filePath.endsWith(".ply")) {
      // 在这里处理 .ply 文件，比如显示文件路径
      vtkSmartPointer<vtkPLYReader> reader =
          vtkSmartPointer<vtkPLYReader>::New();
      reader->SetFileName(filePath.toStdString().c_str());
      reader->Update();
      polyData = reader->GetOutput();
      addPolyData(polyData);
      render();
    } else if (filePath.endsWith(".obj")) {
        // std::shared_ptr<open3d::geometry::TriangleMesh> mesh =
        //     std::make_shared<open3d::geometry::TriangleMesh>();

        auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
        open3d::io::ReadTriangleMeshOptions read;
        open3d::io::ReadTriangleMeshFromOBJ(filePath.toStdString().c_str(),
                                            *mesh, read);
        vtkSmartPointer<vtkActor> actor =
            textureMeshO3d2Vtk(mesh, Eigen::Matrix4d::Identity());
        addTextureMesh(actor, mesh);
        render();
      }
  }
}
void MainWindow::render() {
  if (curVtkRenderer) {
    curVtkRenderer->GetRenderWindow()->Render();
  }
}

void MainWindow::slotPlanStyle(bool state) {
  if (state) {
    planStyle->setPolyData(polyData);
    planStyle->setRenderer(curVtkRenderer);
    qvtkInteractor->SetRenderWindow(curentVtkWindow);
    qvtkInteractor->SetInteractorStyle(planStyle);
    qvtkInteractor->Initialize();

  } else {
    qvtkInteractor->SetRenderWindow(curentVtkWindow);
    qvtkInteractor->SetInteractorStyle(cameraStyle);
    qvtkInteractor->Initialize();
  }
}
void MainWindow::slotCleanPoly() {
  for (auto actor : planStyle->highlightedSquares) {
    curVtkRenderer->RemoveActor(actor);
  }
  planStyle->highlightedSquares.clear();
  curVtkRenderer->GetRenderWindow()->Render();
}
void MainWindow::slotEdgeOn(bool state) {
  //if (state) {
  //  curactor->GetProperty()->EdgeVisibilityOn();
  //  //     // curactor->GetProperty()->e
  //  curactor->GetProperty()->SetEdgeColor(0.0, 0.0,
  //                                        0.0);  // 设置边界线颜色为黑色
  //  curactor->GetProperty()->SetLineWidth(1.0);  // 设置边界线宽度

  //} else {
  //  curactor->GetProperty()->EdgeVisibilityOff();
  //}

    if (state) {
        //curactor->GetProperty()->LightingOff();
        //curactor->GetProperty()->BackfaceCullingOff();
        //curactor->GetBackfaceProperty()->LightingOn();
    }
    else{
		//curactor->GetProperty()->LightingOn();
        //curactor->GetProperty()->BackfaceCullingOn();
        //curactor->GetBackfaceProperty()->LightingOff();
    }


  curVtkRenderer->GetRenderWindow()->Render();
}
