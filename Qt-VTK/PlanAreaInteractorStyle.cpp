#include "PlanAreaInteractorStyle.h"

#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>
#include <vtkRenderWindow.h>

vtkStandardNewMacro(PlanAreaInteractorStyle);

void PlanAreaInteractorStyle::setRenderer(
    vtkSmartPointer<vtkRenderer> renderer) {
  curRenderer = renderer;
  curSquareActor = vtkSmartPointer<vtkActor>::New();
  curRenderer->AddActor(curSquareActor);
}
void PlanAreaInteractorStyle::setPolyData(
    vtkSmartPointer<vtkPolyData> polyData) {
  curPolyData = polyData;
}
void PlanAreaInteractorStyle::OnMouseMove() {
  int* mousePos = GetInteractor()->GetEventPosition();
  vtkSmartPointer<vtkPointPicker> picker =
      vtkSmartPointer<vtkPointPicker>::New();
  double pickedPosition[3];
  if (picker->Pick(mousePos[0], mousePos[1], 0, curRenderer)) {
    picker->GetPickPosition(pickedPosition);
    std::cout << "Picked position: " << pickedPosition[0] << ", "
              << pickedPosition[1] << ", " << pickedPosition[2] << std::endl;
    drawSquare(pickedPosition);
  } else {
    std::cout << "No actor picked at the mouse position." << std::endl;
    std::cout << "Picked position: " << pickedPosition[0] << ", "
              << pickedPosition[1] << ", " << pickedPosition[2] << std::endl;
  }
  vtkInteractorStyleTrackballCamera::OnMouseMove();
}
void PlanAreaInteractorStyle::OnLeftButtonDown() {
  int* clickPos = this->GetInteractor()->GetEventPosition();
  vtkSmartPointer<vtkPointPicker> picker =
      vtkSmartPointer<vtkPointPicker>::New();
  picker->Pick(clickPos[0], clickPos[1], 0, this->curRenderer);
  double pickedPosition[3];
  picker->GetPickPosition(pickedPosition);
  this->highlightSquareArea(pickedPosition);
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void PlanAreaInteractorStyle::drawSquare(const double worldPos[3]) {
  vtkSmartPointer<vtkCubeSource> cubeSource =
      vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->SetCenter(worldPos);
  cubeSource->SetXLength(0.005);
  cubeSource->SetYLength(0.005);
  cubeSource->SetZLength(0.001);
  cubeSource->Update();

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  this->curSquareActor->SetMapper(mapper);
  this->curSquareActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  this->curRenderer->GetRenderWindow()->Render();
}

void PlanAreaInteractorStyle::highlightSquareArea(const double worldPos[3]) {
  vtkSmartPointer<vtkCubeSource> cubeSource =
      vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->SetCenter(worldPos);
  cubeSource->SetXLength(0.005);
  cubeSource->SetYLength(0.005);
  cubeSource->SetZLength(0.001);
  cubeSource->Update();

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkSmartPointer<vtkActor> newSquareActor = vtkSmartPointer<vtkActor>::New();
  newSquareActor->SetMapper(mapper);
  newSquareActor->GetProperty()->SetColor(0.0, 1.0, 0.0);  // 设置颜色为绿色

  this->highlightedSquares.push_back(newSquareActor);
  this->curRenderer->AddActor(newSquareActor);
  this->curRenderer->GetRenderWindow()->Render();
}