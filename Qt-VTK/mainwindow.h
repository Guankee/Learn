#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QMimeData>
#include <QPushButton>
#include <QUrl>

// VTK
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <QVTKInteractor.h>
#include <QVTKOpenGLNativeWidget.h>
//
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
// stl
#include <iostream>
#include <string>
#include <vector>

#include "PlanAreaInteractorStyle.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  // void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void initVTK();
  void addPolyData(vtkSmartPointer<vtkPolyData> polydata, bool append = false);
  void cleanMeshData();
  void render();
 public Q_SLOTS:
  void slotPlanStyle(bool state);
  void slotCleanPoly();
  void slotEdgeOn(bool state);
  // void slotShowNormal(bool state);
 private:
  vtkSmartPointer<vtkRenderer> curVtkRenderer{nullptr};
  vtkSmartPointer<vtkRenderWindow> curentVtkWindow{nullptr};
  vtkSmartPointer<vtkCamera> curVtkcamera{nullptr};
  vtkSmartPointer<QVTKInteractor> qvtkInteractor{nullptr};
  vtkSmartPointer<vtkLight> lightFront{nullptr};
  vtkSmartPointer<vtkLight> lightBack{nullptr};
  // std::vector<vtkPolyData*>polys;
  vtkSmartPointer<vtkPolyData> curPolyData{nullptr};
  vtkSmartPointer<vtkActor> curactor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<PlanAreaInteractorStyle> planStyle =
      vtkSmartPointer<PlanAreaInteractorStyle>::New();
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> cameraStyle =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

 private:
  Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
   