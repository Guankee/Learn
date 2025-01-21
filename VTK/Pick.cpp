#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTriangle.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

// bool IsBoundaryCell(vtkPolyData* polyData, vtkIdType cellId) {
//     vtkSmartPointer<vtkIdList> cellPoints =
//     vtkSmartPointer<vtkIdList>::New(); polyData->GetCellPoints(cellId,
//     cellPoints);

//     // 检查面片的每一条边
//     for (vtkIdType i = 0; i < cellPoints->GetNumberOfIds(); ++i) {
//         vtkIdType ptId1 = cellPoints->GetId(i);
//         vtkIdType ptId2 = cellPoints->GetId((i + 1) %
//         cellPoints->GetNumberOfIds());

//         vtkSmartPointer<vtkIdList> edgeNeighborCells =
//         vtkSmartPointer<vtkIdList>::New();
//         polyData->GetCellEdgeNeighbors(cellId, ptId1, ptId2,
//         edgeNeighborCells);

//         // 如果任何一条边的相邻面片数少于两个，那么它就是边界边
//         if (edgeNeighborCells->GetNumberOfIds() < 2) {
//             return true;
//         }
//     }

//     return false;
// }

void FixPolyData(vtkSmartPointer<vtkPolyData> polyData) {
  if (!polyData) {
    std::cerr << "Invalid vtkPolyData input!" << std::endl;
    return;
  }

  // Step 1: Merge duplicate points
  vtkSmartPointer<vtkCleanPolyData> cleaner =
      vtkSmartPointer<vtkCleanPolyData>::New();
  cleaner->SetInputData(polyData);
  cleaner->SetTolerance(1e-6);  // Adjust tolerance as needed
  cleaner->Update();

  vtkSmartPointer<vtkPolyData> cleanedPolyData = cleaner->GetOutput();

  // Step 2: Rebuild Links to ensure connectivity
  cleanedPolyData->BuildLinks();

  // Replace input polyData with cleaned polyData
  polyData->DeepCopy(cleanedPolyData);

  std::cout << "PolyData cleaned and fixed!" << std::endl;
}
std::vector<vtkIdType> GetCellNeighbors(vtkSmartPointer<vtkPolyData> poly,
                                        vtkIdType cellId) {
  std::vector<vtkIdType> cells;
  vtkNew<vtkIdList> cellPoints;
  vtkNew<vtkIdList> neighborCellIds;
  poly->GetCellPoints(cellId, cellPoints);
  poly->BuildLinks();
  for (vtkIdType i = 0; i < cellPoints->GetNumberOfIds(); ++i) {
    vtkSmartPointer<vtkIdList> edgePointIds = vtkSmartPointer<vtkIdList>::New();
    vtkIdType ptId1 = cellPoints->GetId(i);
    vtkIdType ptId2 = cellPoints->GetId((i + 1) % cellPoints->GetNumberOfIds());
    edgePointIds->InsertNextId(ptId1);
    edgePointIds->InsertNextId(ptId2);
    vtkSmartPointer<vtkIdList> neighborCellIds =
        vtkSmartPointer<vtkIdList>::New();
    poly->GetCellNeighbors(cellId, edgePointIds, neighborCellIds);
    double bonds[6];
    poly->GetCellBounds(cellId, bonds);
    int ne = neighborCellIds->GetNumberOfIds();
    for (vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); ++j) {
      cells.push_back(neighborCellIds->GetId(j));
    }
  }
  return cells;
}

bool IsMeshBoundary(vtkSmartPointer<vtkPolyData> poly, vtkIdType cellId,
                    int maxIter, std::unordered_set<vtkIdType>& set) {
  std::queue<vtkIdType> queue;
  queue.push(cellId);
  int num = 0;
  while (num < maxIter && !queue.empty()) {
    int size = queue.size();
    for (int i = 0; i < size; ++i) {
      vtkIdType curId = queue.front();
      queue.pop();

      if (set.find(curId) != set.end()) {
        continue;
      }
      set.insert(curId);

      std::vector<vtkIdType> neighbors = GetCellNeighbors(poly, curId);
      if (neighbors.size() < 3) {
        return true;
      }

      for (auto id : neighbors) {
        if (set.find(id) == set.end()) {
          queue.push(id);
        }
      }
    }
    num++;
  }
  return false;
}

bool checkPointInsidePolyData(vtkSmartPointer<vtkPolyData> poly,
                              std::vector<double> points) {
  double point[3];
  std::copy(points.begin(), points.end(), point);
  auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
  cellLocator->SetDataSet(poly);
  cellLocator->BuildLocator();
  auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
  double closestPoint[3];    // the coordinates of the closest point will be
                             // returned here
  double closestPointDist2;  // the squared distance to the closest point will
                             // be returned here
  vtkIdType cellId;  // the cell id of the cell containing the closest point
                     // will be returned here
  int subId = -1;
  cellLocator->FindClosestPoint(point, closestPoint, assistCell, cellId, subId,
                                closestPointDist2);
  // vtkTriangle::ComputeNormal();

  // if (-1 != subId && closestPointDist2 < 0.001)
  //{
  //	return true;
  // }
  std::unordered_set<vtkIdType> visited;
  bool isBoundary = IsMeshBoundary(poly, cellId, 3, visited);

  if (isBoundary) {
    std::cout << "------------The initial cell or its neighbors are on the "
                 "boundary.------------"
              << std::endl;
  } else {
    std::cout << "The initial cell and its neighbors are not on the boundary."
              << std::endl;
  }

  // // Highlight the visited cells
  vtkSmartPointer<vtkPolyData> highlightPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPolyData> PolyData1 = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPolyData> PolyData2 = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPolyData> PolyData3 = vtkSmartPointer<vtkPolyData>::New();
  highlightPolyData->SetPoints(poly->GetPoints());

  PolyData1->SetPoints(poly->GetPoints());
  PolyData2->SetPoints(poly->GetPoints());
  PolyData3->SetPoints(poly->GetPoints());

  vtkSmartPointer<vtkCellArray> highlightCells =
      vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> cell1 = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> cell2 = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkCellArray> cell3 = vtkSmartPointer<vtkCellArray>::New();
  for (const auto& id : visited) {
    vtkSmartPointer<vtkGenericCell> cell =
        vtkSmartPointer<vtkGenericCell>::New();
    poly->GetCell(id, cell);
    highlightCells->InsertNextCell(cell);
  }
  highlightPolyData->SetPolys(highlightCells);

  // Visualize
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(poly);

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();
  //     // curactor->GetProperty()->e
  actor->GetProperty()->SetEdgeColor(0.0, 0.0,
                                     0.0);  // 设置边界线颜色为黑色
  actor->GetProperty()->SetLineWidth(1.0);  // 设置边界线宽度
  vtkSmartPointer<vtkPolyDataMapper> highlightMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  highlightMapper->SetInputData(highlightPolyData);

  vtkSmartPointer<vtkActor> highlightActor = vtkSmartPointer<vtkActor>::New();
  highlightActor->SetMapper(highlightMapper);
  highlightActor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // Red color
  highlightActor->GetProperty()->SetLineWidth(3);
  highlightActor->GetProperty()->EdgeVisibilityOn();
  //     // curactor->GetProperty()->e
  highlightActor->GetProperty()->SetEdgeColor(0.0, 0.0,
                                              0.0);  // 设置边界线颜色为黑色
  highlightActor->GetProperty()->SetLineWidth(1.0);  // 设置边界线宽度
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);
  renderer->AddActor(highlightActor);
  renderer->SetBackground(0.1, 0.2, 0.4);

  vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(800, 600);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle(style);

  // 设置相机位置和焦点
  // renderer->GetActiveCamera()->SetFocalPoint(pointArray);
  // renderer->GetActiveCamera()->SetPosition(pointArray[0] + 1, pointArray[1] +
  // 1, pointArray[2] + 1); renderer->GetActiveCamera()->Zoom(8);

  renderWindow->Render();
  renderWindowInteractor->Start();

  if (-1 == subId || closestPointDist2 > 0.001) {
    std::cout << "cell  is out" << std::endl;
    return false;
  }
  std::cout << "Closest cell ID: " << cellId << std::endl;
  vtkSmartPointer<vtkIdList> Points = vtkSmartPointer<vtkIdList>::New();
  poly->GetCellPoints(cellId, Points);
  std::cout << "Cell points: ";
  for (vtkIdType i = 0; i < Points->GetNumberOfIds(); ++i) {
    std::cout << Points->GetId(i) << " ";
  }
  std::cout << std::endl;

  // vtkSmartPointer<vtkIdList> cellPoints = vtkSmartPointer<vtkIdList>::New();
  // vtkNew<vtkIdList> neighborCellIds;
  // poly->GetCellPoints(cellId, cellPoints);
  // poly->BuildLinks();
  //// 检查单元的每条边
  // for (vtkIdType i = 0; i < cellPoints->GetNumberOfIds(); ++i) {
  //	vtkSmartPointer<vtkIdList> edgePointIds =
  // vtkSmartPointer<vtkIdList>::New(); 	vtkIdType ptId1 =
  // cellPoints->GetId(i); 	vtkIdType ptId2 = cellPoints->GetId((i + 1) %
  // cellPoints->GetNumberOfIds()); 	edgePointIds->InsertNextId(ptId1);
  //	edgePointIds->InsertNextId(ptId2);

  //	std::cout << "Edge points: " << ptId1 << " " << ptId2 << std::endl;

  //	vtkSmartPointer<vtkIdList> neighborCellIds =
  // vtkSmartPointer<vtkIdList>::New();

  //	// 调用 GetCellNeighbors 方法
  //	poly->GetCellNeighbors(cellId, edgePointIds, neighborCellIds);
  //	int num = neighborCellIds->GetNumberOfIds();
  //	// 检查 neighborCellIds 的有效性
  //	if (!neighborCellIds || neighborCellIds->GetNumberOfIds() < 1) {
  //		//allEdgesHaveNeighbors = false;
  //		std::cerr << "Edge " << ptId1 << "-" << ptId2 << "-------------
  // has no neighbor cells-----------." << std::endl; 		return false;
  //	}
  //	else {
  //		std::cout << "Neighbor cells for edge " << ptId1 << "-" << ptId2
  //<< ": "; 		for (vtkIdType j = 0; j <
  //neighborCellIds->GetNumberOfIds(); ++j) { 			std::cout <<
  //neighborCellIds->GetId(j) << " ";
  //		}
  //		std::cout << std::endl;
  //	}
  //}

  return true;
}

int main(int, char*[]) {
  // std::string filename = "D:\\ply\\6\\output_with_color.ply";
  // vtkSmartPointer<vtkPLYReader> reader =
  // vtkSmartPointer<vtkPLYReader>::New();
  // reader->SetFileName(filename.c_str());
  // reader->Update();

  vtkSmartPointer<vtkPolyDataReader> reader =
      vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(
      "C:\\Users\\A015240\\Desktop\\cutData.vtk");  // 设置文件路径
  reader->Update();                                 // 读取文件

  // 获取读取的 polyData
  vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
  // bool flag = CheckIfAllTriangles(polyData);
  std::cout << "Number of points: " << polyData->GetNumberOfPoints()
            << std::endl;
  std::cout << "Number of cells: " << polyData->GetNumberOfCells() << std::endl;
  FixPolyData(polyData);
  // 打印点和单元的数量
  std::cout << "Number of points: " << polyData->GetNumberOfPoints()
            << std::endl;
  std::cout << "Number of cells: " << polyData->GetNumberOfCells() << std::endl;

  // std::vector<double> points{ 0.71438033083643981,
  // 0.00081929035976531506,0.056970278379591427 };
  std::vector<double> points{0.653973, -0.00992344, 0.134852};
  checkPointInsidePolyData(polyData, points);

  // 可视化读取的 polyData
  // vtkSmartPointer<vtkPolyDataMapper> mapper =
  // vtkSmartPointer<vtkPolyDataMapper>::New(); mapper->SetInputData(polyData);

  // vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  // actor->SetMapper(mapper);

  // vtkSmartPointer<vtkRenderer> renderer =
  // vtkSmartPointer<vtkRenderer>::New(); renderer->AddActor(actor);
  // renderer->SetBackground(0.1, 0.2, 0.3);

  // vtkSmartPointer<vtkRenderWindow> renderWindow =
  // vtkSmartPointer<vtkRenderWindow>::New();
  // renderWindow->AddRenderer(renderer);
  // renderWindow->SetSize(800, 600);

  // vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
  // vtkSmartPointer<vtkRenderWindowInteractor>::New();
  // renderWindowInteractor->SetRenderWindow(renderWindow);

  // renderWindow->Render();
  // renderWindowInteractor->Start();

  // // 创建一个球体
  // vtkSmartPointer<vtkSphereSource> sphereSource =
  //     vtkSmartPointer<vtkSphereSource>::New();
  // sphereSource->Update();

  // // 创建一个映射器
  // vtkSmartPointer<vtkPolyDataMapper> mapper =
  //     vtkSmartPointer<vtkPolyDataMapper>::New();
  // mapper->SetInputConnection(sphereSource->GetOutputPort());

  // // 创建一个actor
  // vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  // actor->SetMapper(mapper);

  // // 创建一个渲染器
  // vtkSmartPointer<vtkRenderer> renderer =
  // vtkSmartPointer<vtkRenderer>::New(); renderer->AddActor(actor);

  // // 创建一个渲染窗口
  // vtkSmartPointer<vtkRenderWindow> renderWindow =
  //     vtkSmartPointer<vtkRenderWindow>::New();
  // renderWindow->AddRenderer(renderer);

  // // 创建一个交互窗口
  // vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
  //     vtkSmartPointer<vtkRenderWindowInteractor>::New();
  // renderWindowInteractor->SetRenderWindow(renderWindow);

  // // 创建并设置自定义交互样式
  // vtkSmartPointer<vtkInteractorStyleRubberBandPick> style =
  //     vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();
  // renderWindowInteractor->SetInteractorStyle(style);

  // // 初始化交互
  // renderWindowInteractor->Initialize();
  // renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
