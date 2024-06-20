#ifndef PLANAREAINTERACTORSTYLE_H
#define PLANAREAINTERACTORSTYLE_H
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vector>
#include <queue>
#include<unordered_set>
class PlanAreaInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
	static PlanAreaInteractorStyle* New();
	void setRenderer(vtkSmartPointer<vtkRenderer> renderer);
	void setPolyData(vtkSmartPointer<vtkPolyData> polyData);
	// virtual void OnLeftButtonDown() override;
  virtual void OnRightButtonDown()override;
	virtual void OnMouseMove() override;
	// virtual void OnLeftButtonUp() override;
	std::vector<vtkSmartPointer<vtkActor>> highlightedSquares;

private:
	vtkSmartPointer<vtkRenderer> curRenderer;
	vtkSmartPointer<vtkPolyData> curPolyData;
	vtkSmartPointer<vtkActor> curSquareActor;
	void drawSquare(const double worldPos[3]);
	void highlightSquareArea(const double worldPos[3]);
	void computeAverageNormal(const double worldPos[3],int range, double normal[3]);
	void projectSquareToSurface(vtkPolyData* squarePolyData,
								vtkPolyData* projectedPolyData);
	bool isDrawingSquare;
};

#endif  // PLANAREAINTERACTORSTYLE_H