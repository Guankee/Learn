#include "CutActorBoxWidget.h"

vtkStandardNewMacro(CutActorBoxWidgetCommand);

// void CutActorBoxWidgetComnand::SetActorProperty(vtkSmartPointer<vtkProperty>
// frontProperty, vtkSmartPointer<vtkProperty> backProperty){ 	actorFrontProperty
//= frontProperty; 	actorBackProperty = backProperty;
// }

void CutActorBoxWidgetCommand::Execute(vtkObject* caller, unsigned long eventId,
                                       void* calldata) {
  if (!curActor || !curRenderer) {
    return;
  }
  vtkBoxWidget* widget = vtkBoxWidget::SafeDownCast(caller);
  widget->GetPlanes(planes);
  vtkPolyData* data =
      vtkPolyData::SafeDownCast(curActor->GetMapper()->GetInput());
  clipper->SetInputData(data);
  clipper->SetImplicitFunction(planes);
  clipper->Update();
  if (clipper->GetOutput()->GetNumberOfCells() != 0) {
    curRenderer->RemoveActor(curActor);
  }
  selectPolyData = clipper->GetOutput();
  selectMapper->SetInputData(clipper->GetOutput());
  selectActor->SetMapper(selectMapper);
  if (curActor->GetTexture()) {
    selectActor->SetTexture(curActor->GetTexture());
    selectActor->SetProperty(curActor->GetProperty());
    selectActor->SetBackfaceProperty(curActor->GetBackfaceProperty());
  }
  curRenderer->AddActor(selectActor);
  curRenderer->Render();
}

vtkStandardNewMacro(CutActorBoxWidget);

CutActorBoxWidget::CutActorBoxWidget() {
  if (!boxWidget) {
    boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
  }
  if (!command) {
    command = vtkSmartPointer<CutActorBoxWidgetCommand>::New();
  }
  boxWidget->AddObserver(vtkCommand::InteractionEvent, command);
  vtkProperty*pr=boxWidget->GetOutlineProperty();
  vtkProperty*spr=boxWidget->GetSelectedOutlineProperty();
  pr->SetColor(0,0,1.0);
  spr->SetColor(1.0,0,0);
  pr=boxWidget->GetHandleProperty();
  spr=boxWidget->GetSelectedHandleProperty();
  pr->SetColor(0.3,1.0,0);
  pr->SetOpacity(0.6);
  spr->SetColor(1.0,0,0);
  spr->SetOpacity(0.6);
}
CutActorBoxWidget::~CutActorBoxWidget() {}
bool CutActorBoxWidget::IsHasPolyData(vtkSmartPointer<vtkActor> actor) {
  if (nullptr == actor) {
    return false;
  }
  vtkMapper* mapper = actor->GetMapper();
  if (!mapper) {
    return false;
  }
  vtkPolyData* polyData = vtkPolyData::SafeDownCast(mapper->GetInput());
  if (!polyData) {
    return false;
  }

  vtkCellArray* cells = polyData->GetPolys();
  if (!cells) {
    return false;
  }
  int numCells = cells->GetNumberOfCells();
  return numCells > 0;
}
void CutActorBoxWidget::SetIneractor(
    vtkSmartPointer<QVTKInteractor> interactor) {
  if (!interactor) {
    return;
  }
  curInteractor = interactor;
  boxWidget->SetInteractor(curInteractor);
}

void CutActorBoxWidget::SetRenderer(vtkSmartPointer<vtkRenderer> render) {
  command->SetRenderer(render);
}

void CutActorBoxWidget::SetActor(vtkSmartPointer<vtkActor> actor) {
  boxWidget->SetProp3D(actor);
  boxWidget->SetPlaceFactor(1.0);
  boxWidget->PlaceWidget();
  command->setActor(actor);
}

void CutActorBoxWidget::On() {
  if (IsHasPolyData(command->GetSelectActor())) {
    boxWidget->SetProp3D(command->GetSelectActor());
    boxWidget->SetPlaceFactor(1.0);
    boxWidget->PlaceWidget();
  }

  boxWidget->On();
}

void CutActorBoxWidget::Off() { boxWidget->Off(); }
