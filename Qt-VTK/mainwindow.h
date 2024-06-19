#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QUrl>
#include <QMimeData>

//VTK
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <QVTKOpenGLNativeWidget.h>
#include <QVTKInteractor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkLight.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkActorCollection.h>
#include <vtkProperty.h>
#include <vtkPLYReader.h>
//stl
#include<vector>
#include<string>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
   void dragEnterEvent(QDragEnterEvent *event) override;
    // void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

  void initVTK();
  void addPolyData(vtkSmartPointer<vtkPolyData> polydata,bool append=false);
  void cleanMeshData();
  void  render();
private:
  vtkSmartPointer<vtkRenderer> curVtkRenderer{nullptr};
  vtkSmartPointer<vtkRenderWindow>curentVtkWindow{nullptr};
  vtkSmartPointer<vtkCamera>curVtkcamera{nullptr};
  vtkSmartPointer<QVTKInteractor>qvtkInteractor{nullptr};
  vtkSmartPointer<vtkLight>lightFront{nullptr};
  vtkSmartPointer<vtkLight>lightBack{nullptr};
  // std::vector<vtkPolyData*>polys;
  vtkSmartPointer<vtkPolyData>curPolyData{nullptr};
  vtkSmartPointer<vtkActor> curactor = vtkSmartPointer<vtkActor>::New();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
