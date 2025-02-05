#ifndef CUT_ACTOR_BOX_WIDGET_H
#define CUT_ACTOR_BOX_WIDGET_H

#include <QVTKInteractor.h>
#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class CutActorBoxWidgetCommand : public vtkCommand {
 public:
  static CutActorBoxWidgetCommand* New();
  vtkTypeMacro(CutActorBoxWidgetCommand, vtkCommand);

  CutActorBoxWidgetCommand() = default;
  ~CutActorBoxWidgetCommand() = default;
  void Execute(vtkObject* caller, unsigned long eventId,
               void* calldata) override;
  // void SetActorProperty(vtkSmartPointer<vtkProperty>actorFrontProperty,
  //	                  vtkSmartPointer<vtkProperty>actorBackProperty);
  void SetRenderer(vtkSmartPointer<vtkRenderer> renderer) {
    curRenderer = renderer;
  }
  void setActor(vtkSmartPointer<vtkActor> actor) { curActor = actor; }
  vtkSmartPointer<vtkPolyData> GetSelectPolyData() { return selectPolyData; }
  vtkSmartPointer<vtkActor> GetSelectActor() { return selectActor; }

 private:
  vtkSmartPointer<vtkActor> curActor = nullptr;
  vtkSmartPointer<vtkRenderer> curRenderer = nullptr;
  // vtkSmartPointer<vtkProperty>actorFrontProperty = nullptr;
  // vtkSmartPointer<vtkProperty>actorBackProperty = nullptr;
  vtkSmartPointer<vtkPolyData> selectPolyData = nullptr;
  vtkSmartPointer<vtkActor> selectActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkDataSetMapper> selectMapper =
      vtkSmartPointer<vtkDataSetMapper>::New();
  vtkSmartPointer<vtkPlanes> planes = vtkSmartPointer<vtkPlanes>::New();
  vtkSmartPointer<vtkExtractPolyDataGeometry> clipper =
      vtkSmartPointer<vtkExtractPolyDataGeometry>::New();
};

class CutActorBoxWidget : public vtkObject {
 public:
  static CutActorBoxWidget* New();
  vtkTypeMacro(CutActorBoxWidget, vtkObject);
  void SetIneractor(vtkSmartPointer<QVTKInteractor> interactor);
  void SetRenderer(vtkSmartPointer<vtkRenderer> render);
  void SetActor(vtkSmartPointer<vtkActor> actor);
  void On();
  void Off();
  vtkSmartPointer<vtkPolyData> GetSelectPolyData() {
    return command->GetSelectPolyData();
  }
  CutActorBoxWidget();
  ~CutActorBoxWidget();

 private:
  bool IsHasPolyData(vtkSmartPointer<vtkActor> actor);

 private:
  vtkSmartPointer<QVTKInteractor> curInteractor = nullptr;
  vtkSmartPointer<vtkProperty> actorFrontProperty = nullptr;
  vtkSmartPointer<vtkProperty> actorBackProperty = nullptr;
  vtkSmartPointer<vtkBoxWidget> boxWidget = nullptr;
  vtkSmartPointer<CutActorBoxWidgetCommand> command = nullptr;
};
#endif  // CUT_ACTOR_BOX_WIDGET_H