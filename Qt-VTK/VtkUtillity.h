#ifndef VTK_UTILLITY_H
#define VTK_UTILLITY_H
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkCellLocator.h>
#include <vtkCell.h>

#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkOutlineFilter.h>
#include <vtkTriangleFilter.h>
class VtkUtillity {
 public:
  static void Test();
  static void fixPolyData(vtkSmartPointer<vtkPolyData>polyData);
  static void booleanTest();
};
#endif  // VTK_UTILLITY_H