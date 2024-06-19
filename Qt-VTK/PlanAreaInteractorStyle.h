#ifndef PLANAREAINTERACTORSTYLE_H
#define PLANAREAINTERACTORSTYLE_H
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkCubeSource.h>
#include <vtkProperty.h>
#include <vtkCellPicker.h>
#include <vector>

class PlanAreaInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
	static PlanAreaInteractorStyle* New();
	void setRenderer(vtkSmartPointer<vtkRenderer> renderer);
	void setPolyData(vtkSmartPointer<vtkPolyData> polyData);
	virtual void OnLeftButtonDown() override;
	virtual void OnMouseMove() override;
	// virtual void OnLeftButtonUp() override;
	std::vector<vtkSmartPointer<vtkActor>> highlightedSquares;
private:
	vtkSmartPointer<vtkRenderer> curRenderer;
	vtkSmartPointer<vtkPolyData> curPolyData;
	vtkSmartPointer<vtkActor> curSquareActor;
	void drawSquare(const double worldPos[3]);
	void highlightSquareArea(const double worldPos[3]);
	bool isDrawingSquare;

};

#endif // PLANAREAINTERACTORSTYLE_H