#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkPointPicker.h>
//this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()
#include <vtkRendererCollection.h> 
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>  //vtkStandardNewMacro();
#include <vtkProperty.h>

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
/**************************************************************************************************/
class PointPickerInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
	static PointPickerInteractorStyle* New();
	vtkTypeMacro(PointPickerInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown() {
		//打印鼠标左键像素位置
		std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0]
			<< " " << this->Interactor->GetEventPosition()[1] << std::endl;
		//注册拾取点函数
		this->Interactor->GetPicker()->Pick(
			this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1], 0,  // always zero.
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()
		);
		//打印拾取点空间位置
		double picked[3];
		this->Interactor->GetPicker()->GetPickPosition(picked);
		std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
		//对拾取点进行标记
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->SetPosition(picked);
		actor->SetScale(0.05);
		actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
		//调父类的  不调用父类的直接转不了了 
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
};
/**************************************************************************************************/

vtkStandardNewMacro(PointPickerInteractorStyle);

int main() {
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(1, 1, 1);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->Render();
	renderWindow->SetWindowName("PointPicker");
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkPointPicker> pointPicker =
		vtkSmartPointer<vtkPointPicker>::New();

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetPicker(pointPicker);
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<PointPickerInteractorStyle> style =
		vtkSmartPointer<PointPickerInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	vtkSmartPointer<vtkAxesActor>  Axes = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget>  widget =
		vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	widget->SetInteractor(renderWindowInteractor);
	widget->SetOrientationMarker(Axes);
	widget->SetOutlineColor(1, 1, 1);
	widget->SetViewport(0, 0, 0.2, 0.2);
	widget->SetEnabled(1);
	widget->InteractiveOn();

	renderWindow->Render();
	renderWindowInteractor->Start();

	return 0;
}