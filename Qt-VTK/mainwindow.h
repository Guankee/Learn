#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QMimeData>
#include <QPushButton>
#include <QUrl>
// open3d
#include <open3d/Open3D.h>

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
#include <vtkAnnotatedCubeActor.h>
#include <vtkAreaPicker.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPLYReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>
#include <vtkAreaPicker.h>
#include <vtkBoxWidget.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkPlanes.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkDataSetMapper.h>
// stl
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "PlanAreaInteractorStyle.h"

namespace Ui {
class MainWindow;
}

namespace {
	class vtkMyCallback : public vtkCommand
	{
	public:
		static vtkMyCallback* New()
		{
			return new vtkMyCallback;
		}
		void SetData(vtkSmartPointer<vtkRenderer>renderer, vtkSmartPointer<vtkActor>actor) {
			curRenderer = renderer;
			curActor = actor;
		}
		virtual void Execute(vtkObject* caller, unsigned long, void*)
		{
			// Here we use the vtkBoxWidget to transform the underlying coneActor
			// (by manipulating its transformation matrix).
			//vtkNew<vtkTransform> t;
			vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
			//vtkPlanes* frustum;
			vtkNew<vtkPlanes>frustum;
			widget->GetPlanes(frustum);
			vtkNew<vtkExtractPolyDataGeometry> clipper;
			vtkPolyData* data = vtkPolyData::SafeDownCast(curActor->GetMapper()->GetInput());
			clipper->SetInputData(data);
			clipper->SetImplicitFunction(frustum);
			clipper->Update();

			if (clipper->GetOutput()->GetNumberOfCells() != 0){
				curRenderer->RemoveActor(curActor);
			}
			selectPloyData = clipper->GetOutput();
			selectMapper->SetInputData(clipper->GetOutput());
			selectActor->SetMapper(selectMapper);
			if (curActor->GetTexture()){
				selectActor->SetTexture(curActor->GetTexture());
				selectActor->GetProperty()->LightingOff();
			}
			curRenderer->AddActor(selectActor);
			curRenderer->Render();
			

			//widget->GetTransform(t);
			//widget->GetProp3D()->SetUserTransform(t);


		}
	private:
		vtkSmartPointer<vtkRenderer>curRenderer;
		vtkSmartPointer<vtkActor>curActor;
		vtkSmartPointer<vtkPolyData>selectPloyData;
		vtkSmartPointer<vtkDataSetMapper>selectMapper=vtkSmartPointer<vtkDataSetMapper>::New();
		vtkSmartPointer<vtkActor>selectActor = vtkSmartPointer<vtkActor>::New();
	};
} // namespace

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
  void loadData();
  void addPolyData(vtkSmartPointer<vtkPolyData> polydata, bool append = false);
  void addTextureMesh(vtkSmartPointer<vtkActor> actor,
                      std::shared_ptr<open3d::geometry::TriangleMesh> mesh,
                      bool append = false);
  vtkSmartPointer<vtkActor> textureMeshO3d2Vtk(
      std::shared_ptr<open3d::geometry::TriangleMesh> mesh,
      const Eigen::Matrix4d &transMatrix);
  void create3DMarkWidget();
  void cleanMeshData();
  void render();
 public Q_SLOTS:
  void slotPlanStyle(bool state);
  void slotCleanPoly();
  void slotEdgeOn(bool state);
  void slotBtn1();
  void slotBtn2(bool state);
  void slotBtn3();
  void slotBtn4(bool state);
  void slotBtn5();
  void slotBtn6(bool state);


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
  vtkSmartPointer<vtkActor> curActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<PlanAreaInteractorStyle> planStyle =
      vtkSmartPointer<PlanAreaInteractorStyle>::New();
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> cameraStyle =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  vtkSmartPointer<vtkOrientationMarkerWidget> orientationMarkerWidget = nullptr;
  vtkSmartPointer<vtkAxesActor> axesActor = nullptr;
  vtkSmartPointer<vtkBoxWidget> boxWidget = nullptr;
 private:
  Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
