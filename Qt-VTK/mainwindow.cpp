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
  //   ouraud 插值 (SetInterpolationToGouraud)：
  // 光照计算在每个顶点进行，然后在多边形内部使用线性插值计算光照值。这会产生较为平滑的光照过渡。
  curactor->GetProperty()->SetInterpolationToGouraud();

  curVtkRenderer->AddActor(curactor);
  render();
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
