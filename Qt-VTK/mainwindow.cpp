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
  connect(ui->Btn1, &QPushButton::clicked, this, &MainWindow::slotBtn1);
  connect(ui->Btn2, &QPushButton::toggled, this, &MainWindow::slotBtn2);
  connect(ui->Btn3, &QPushButton::clicked, this, &MainWindow::slotBtn3);
  connect(ui->Btn4, &QPushButton::toggled, this, &MainWindow::slotBtn4);
  connect(ui->Btn5, &QPushButton::clicked, this, &MainWindow::slotBtn5);
  connect(ui->Btn6, &QPushButton::toggled, this, &MainWindow::slotBtn6);
  connect(ui->Btn7, &QPushButton::clicked, this, &MainWindow::slotBtn7);
  connect(ui->Btn8, &QPushButton::clicked, this, &MainWindow::slotBtn8);
  connect(ui->Btn9, &QPushButton::clicked, this, &MainWindow::slotBtn9);
  connect(ui->Btn10, &QPushButton::clicked, this, &MainWindow::slotBtn10);
  connect(ui->Btn11, &QPushButton::clicked, this, &MainWindow::slotBtn11);
  connect(ui->Btn12, &QPushButton::clicked, this, &MainWindow::slotBtn12);

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
  // qvtkInteractor->SetPicker();

  curVtkcamera->Zoom(0.6);
}
void MainWindow::loadData() {
  auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
  open3d::io::ReadTriangleMeshOptions read;
  open3d::io::ReadTriangleMeshFromOBJ(
      "C:\\Users\\A015240\\Desktop\\mesh\\meshwithglb.obj", *mesh, read);
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

  create3DMarkWidget();
  curPolyData = polydata;
  double* center = polydata->GetCenter();
  curVtkcamera->SetFocalPoint(center[0], center[1], center[2]);  // 设焦点

  auto mapper = vtkSmartPointer<
      vtkPolyDataMapper>::New();  // 映射器，可以根据点、线、面构造图形
  mapper->SetInputData(curPolyData);
  mapper->Update();

  curActor->SetMapper(mapper);
  curVtkRenderer->AddActor(curActor);
}
void MainWindow::addTextureMesh(
    vtkSmartPointer<vtkActor> actor,
    std::shared_ptr<open3d::geometry::TriangleMesh> mesh, bool append) {
  if (!append) {
    cleanMeshData();
  }
  create3DMarkWidget();
  open3d::geometry::PointCloud cloud;
  cloud.points_ = mesh->vertices_;
  Eigen::Vector3d center = cloud.GetCenter();
  curVtkcamera->SetFocalPoint(center[0], center[1], center[2]);
  curActor = actor;
  curVtkRenderer->AddActor(curActor);
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
  vtkSmartPointer<vtkProperty> frontProperty =
      vtkSmartPointer<vtkProperty>::New();

  frontProperty->LightingOff();
  actor->SetProperty(frontProperty);

  vtkSmartPointer<vtkProperty> backfaceProperty =
      vtkSmartPointer<vtkProperty>::New();
  backfaceProperty->SetAmbient(0.25);
  backfaceProperty->SetDiffuse(0.2);
  backfaceProperty->SetSpecular(0.1);

  actor->SetBackfaceProperty(backfaceProperty);
  return actor;
}
void MainWindow::create3DMarkWidget() {
  if (nullptr == axesActor) {
    axesActor = vtkSmartPointer<vtkAxesActor>::New();
    axesActor->SetPosition(0, 0, 0);
    axesActor->SetTotalLength(2.0, 2.0, 2.0);
    axesActor->SetShaftTypeToCylinder();
    axesActor->SetCylinderRadius(0.04);
    axesActor->SetCylinderResolution(20);
    axesActor->SetConeRadius(0.8);
    axesActor->SetConeResolution(20);

    axesActor->SetXAxisLabelText("X");
    axesActor->SetYAxisLabelText("Y");
    axesActor->SetZAxisLabelText("Z");
    // 修改vtkAxesActor默认的字体颜色，Axes为vtkAxesActor的对象指针
    auto tpropX = vtkSmartPointer<vtkTextProperty>::New();
    // tpropX->ItalicOn();
    tpropX->BoldOn();
    tpropX->SetColor(1, 0, 0);
    // tpropX->ShadowOn();	//阴影
    tpropX->SetFontFamilyToArial();
    // tpropX->SetFontFamilyToTimes();
    axesActor->GetXAxisCaptionActor2D()->SetCaptionTextProperty(tpropX);
    //
    auto tpropY = vtkSmartPointer<vtkTextProperty>::New();
    tpropY->ShallowCopy(tpropX);
    tpropY->SetColor(0, 1, 0);
    axesActor->GetYAxisCaptionActor2D()->SetCaptionTextProperty(tpropY);
    //
    auto tpropZ = vtkSmartPointer<vtkTextProperty>::New();
    tpropZ->ShallowCopy(tpropX);
    tpropZ->SetColor(0, 0, 1);
    axesActor->GetZAxisCaptionActor2D()->SetCaptionTextProperty(tpropZ);
    auto cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
    cube->SetXPlusFaceText("R");
    cube->SetXMinusFaceText("L");
    cube->SetYPlusFaceText("F");
    cube->SetYMinusFaceText("H");
    cube->SetZPlusFaceText("I");
    cube->SetZMinusFaceText("S");
    // cube->SetXFaceTextRotation(180);
    // cube->SetYFaceTextRotation(180);
    // cube->SetZFaceTextRotation(-90);

    cube->SetFaceTextScale(0.80);
    cube->GetCubeProperty()->SetColor(1, 1, 1);

    cube->GetTextEdgesProperty()->SetLineWidth(1);
    cube->GetTextEdgesProperty()->SetDiffuse(0);
    cube->GetTextEdgesProperty()->SetAmbient(1);
    cube->GetTextEdgesProperty()->SetColor(0, 0, 0);

    //
    cube->GetXPlusFaceProperty()->SetColor(0, 0, 0);
    cube->GetXPlusFaceProperty()->SetInterpolationToFlat();

    cube->GetXMinusFaceProperty()->SetColor(0, 0, 0);
    cube->GetXMinusFaceProperty()->SetInterpolationToFlat();
    //
    cube->GetYPlusFaceProperty()->SetColor(0, 0, 0);
    cube->GetYPlusFaceProperty()->SetInterpolationToFlat();
    //
    cube->GetYMinusFaceProperty()->SetColor(0, 0, 0);
    cube->GetYMinusFaceProperty()->SetInterpolationToFlat();
    //
    cube->GetZPlusFaceProperty()->SetColor(0, 0, 0);
    cube->GetZPlusFaceProperty()->SetInterpolationToFlat();
    //
    cube->GetZMinusFaceProperty()->SetColor(0, 0, 0);
    cube->GetZMinusFaceProperty()->SetInterpolationToFlat();

    auto assembly = vtkSmartPointer<vtkPropAssembly>::New();
    assembly->AddPart(cube);
    assembly->AddPart(axesActor);

    // 控制坐标系,使之随视角共同变化
    if (orientationMarkerWidget == nullptr) {
      orientationMarkerWidget =
          vtkSmartPointer<vtkOrientationMarkerWidget>::New();
      orientationMarkerWidget->SetOrientationMarker(assembly);
      orientationMarkerWidget->SetInteractive(false);
      orientationMarkerWidget->SetInteractor(qvtkInteractor);
      orientationMarkerWidget->SetEnabled(true);
      orientationMarkerWidget->InteractiveOff();
    }
  }
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
    } else if (urlList.size() == 1 &&
               urlList[0].toLocalFile().endsWith(".obj")) {
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
      open3d::io::ReadTriangleMeshFromOBJ(filePath.toStdString().c_str(), *mesh,
                                          read);
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
 /*   curVtkRenderer->Render();*/
    ui->qVTKOpenglWidget->repaint();
  }
}

void MainWindow::slotPlanStyle(bool state) {
  if (state) {
    //planStyle->setPolyData(polyData);
    planStyle->setActor(curActor);
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
  vtkSmartPointer<vtkActorCollection> actors = curVtkRenderer->GetActors();
  actors->InitTraversal();
  for (int i = 0; i < actors->GetNumberOfItems(); i++) {
    curVtkRenderer->RemoveActor(actors->GetNextActor());
  }
  actors->RemoveAllItems();
  curVtkRenderer->GetRenderWindow()->Render();
}
void MainWindow::slotEdgeOn(bool state) {
  if (state) {
    curActor->GetProperty()->EdgeVisibilityOn();
    //     // curActor->GetProperty()->e
    curActor->GetProperty()->SetEdgeColor(0.0, 0.0,
                                          0.0);  // 设置边界线颜色为黑色
    curActor->GetProperty()->SetLineWidth(1.0);  // 设置边界线宽度

  } else {
    curActor->GetProperty()->EdgeVisibilityOff();
  }

  // if (state) {
  //   // curActor->GetProperty()->LightingOff();
  //   // curActor->GetProperty()->BackfaceCullingOff();
  //   // curActor->GetBackfaceProperty()->LightingOn();
  // } else {
  //   // curActor->GetProperty()->LightingOn();
  //   // curActor->GetProperty()->BackfaceCullingOn();
  //   // curActor->GetBackfaceProperty()->LightingOff();
  // }

  curVtkRenderer->GetRenderWindow()->Render();
}

void MainWindow::slotBtn1() {
 //   vtkNew<vtkCubeSource>cubSource;
 //   cubSource->SetXLength(0.01);
 //   cubSource->SetYLength(0.01);
 //   cubSource->SetZLength(0.01);
 //   cubSource->SetCenter(0.638563, 0.0476707, 0.14582);
 //   //cubSource->SetCenter(0.635908,0.0306695,0.158201);
 //   cubSource->Update();

 //   vtkNew<vtkTriangleFilter>filer;
 //   filer->SetInputData(cubSource->GetOutput());
 //   filer->Update();

 //   vtkNew<vtkPolyDataMapper>mapper;
 //   mapper->SetInputData(filer->GetOutput());

 ////   vtkNew<vtkActor>cubActor;
 ////   cubActor->SetMapper(mapper);
 ////   cubActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
	////curVtkRenderer->AddActor(cubActor);
	////curentVtkWindow->Render();
 //   vtkPolyData*curPoly = vtkPolyData::SafeDownCast(curActor->GetMapper()->GetInput());
 //   vtkNew<vtkPolyData>copy;
 //   copy->DeepCopy(curPoly);

 //   VtkUtillity::fixPolyData(copy);
 //   vtkNew<vtkBooleanOperationPolyDataFilter>booleanFilter;
 //   booleanFilter->SetInputData(0, copy);
 //   booleanFilter->SetInputData(1, filer->GetOutput());
	//booleanFilter->SetOperationToIntersection();
	//booleanFilter->Update();



	//vtkSmartPointer<vtkPolyData> resultPolyData = booleanFilter->GetOutput();
	//vtkSmartPointer<vtkPolyDataMapper> resultmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
 //   resultmapper->SetInputData(resultPolyData);

	//vtkSmartPointer<vtkActor> resActor = vtkSmartPointer<vtkActor>::New();
 //   resActor->SetMapper(resultmapper);
 //   resActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
 //   

 //   vtkNew<vtkPolyDataMapper>copyMapper;
 //   copyMapper->SetInputData(copy);
 //   vtkNew<vtkActor>copyActor;
 //   copyActor->SetMapper(copyMapper);
 //   vtkNew<vtkRenderer>render;
 //   //render->AddActor(copyActor);
 //   render->AddActor(resActor);
 //   vtkNew<vtkRenderWindow>widow;
 //   widow->AddRenderer(render);
 //   vtkNew<vtkRenderWindowInteractor>intor;
 //   intor->SetRenderWindow(widow);

	//curVtkRenderer->AddActor(resActor);
	//curentVtkWindow->Render();

    //widow->Render();
    //intor->Start();
  if (!cutActorBox) {
    cutActorBox = vtkSmartPointer<CutActorBoxWidget>::New();
  }

  cutActorBox->setIneractor(qvtkInteractor);
  cutActorBox->setRenderer(curVtkRenderer);
    cutActorBox->setActor(curActor);
    cutActorBox->on();
    //VtkUtillity::booleanTest();
}
void MainWindow::slotBtn2(bool state) {}
void MainWindow::slotBtn3() { cutActorBox->off(); }
void MainWindow::slotBtn4(bool state) {}
void MainWindow::slotBtn5() {


  // cleanMeshData();
  // vtkSmartPointer<vtkPolyData> polyData = cutActorBox->GetSelectPolyData();
  // int num = polyData->GetNumberOfCells();
  // vtkNew<vtkDataSetMapper>mapper;
  // mapper->SetInputData(polyData);
  // vtkNew<vtkActor>actor;
  // actor->SetMapper(mapper);
  // vtkNew<vtkRenderer>render;
  // render->AddActor(actor);
  // vtkNew<vtkRenderWindow>window;
  // window->AddRenderer(render);
  // window->SetSize(800, 600);
  // vtkNew<vtkRenderWindowInteractor>intor;
  // intor->SetRenderWindow(window);
  // window->Render();
  // intor->Start();
  // addPolyData(polyData);
  // render();
}
void MainWindow::slotBtn6(bool state) {}

void MainWindow::slotBtn7()
{

    //cutActorBox->SetActor(curActor);
    //curentVtkWindow->Render();
	//size = 0.01;
	//cub->SetXLength(0.03);
	//cub->SetXLength(0.05);
	//cub->SetXLength(0.08);
	//cub->Update();
	//curentVtkWindow->Render();
}

void MainWindow::slotBtn8()
{
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateZ(45.0);
    transformFilter1->SetInputConnection(cub->GetOutputPort());
    transformFilter1->SetTransform(transform);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter1->GetOutputPort());
    vtkNew<vtkActor>actor;
    actor->SetMapper(mapper);
    curVtkRenderer->AddActor(actor);
    curentVtkWindow->Render();
}

void MainWindow::slotBtn9()
{
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateZ(45.0);
	transformFilter1->SetInputData(cub->GetOutput());
	transformFilter1->SetTransform(transform);
    transformFilter1->Update();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter1->GetOutputPort());
	vtkNew<vtkActor>actor;
	actor->SetMapper(mapper);
	curVtkRenderer->AddActor(actor);
	curentVtkWindow->Render();
}

void MainWindow::slotBtn10()
{

	cub->SetXLength(0.03+size);
	cub->SetXLength(0.05+size);
	cub->SetXLength(0.08+size);
    size += 0.01;
	//cub->Update();
    curentVtkWindow->Render();
}

void MainWindow::slotBtn11()
{
}

void MainWindow::slotBtn12()
{
    loadData();
}
