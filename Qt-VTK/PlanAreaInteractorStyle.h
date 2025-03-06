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
#include <vtkSphereSource.h>
#include <vtkGenericCell.h>
#include <vtkPolyDataNormals.h>
#include <vtkDataArray.h>
#include <vtkTransform.h>
#include <vtkLineSource.h>
#include<vtkTransformPolyDataFilter.h>
#include <queue>
#include <unordered_set>
#include <vector>
class PlanAreaInteractorStyle : public vtkInteractorStyleTrackballCamera {
 public:
  static PlanAreaInteractorStyle* New();
  void setRenderer(vtkSmartPointer<vtkRenderer> renderer);
  void setPolyData(vtkSmartPointer<vtkPolyData> polyData);
  void setActor(vtkSmartPointer<vtkActor> actor);
  virtual void OnLeftButtonDown() override;
  //virtual void OnRightButtonDown() override;
  // virtual void OnMouseMove() override;
  // virtual void OnLeftButtonUp() override;
  virtual void OnMiddleButtonDown() override;
  std::vector<vtkSmartPointer<vtkActor>> highlightedSquares;

 private:
  vtkSmartPointer<vtkRenderer> curRenderer = nullptr;
  vtkSmartPointer<vtkPolyData> curPolyData = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPolyData> curPolyataWithNor = nullptr;
  vtkSmartPointer<vtkActor> curSquareActor = nullptr;
  vtkSmartPointer<vtkActor> curActor=nullptr;
  void drawSquare(const double worldPos[3]);
  void highlightSquareArea(const double worldPos[3]);
  void computeAverageNormal(const double worldPos[3], int range,
                            double normal[3],
                            std::unordered_set<vtkIdType>& visitedCells);
  void projectSquareToSurface(vtkPolyData* squarePolyData,
                              vtkPolyData* projectedPolyData);
  void addSquare(double worldPos[3]);
  vtkSmartPointer<vtkTransform>getTransform(double normal[3],double pos[3]);
  bool isDrawingSquare;
};

#endif  // PLANAREAINTERACTORSTYLE_H