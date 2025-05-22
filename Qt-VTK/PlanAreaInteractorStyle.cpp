#include "PlanAreaInteractorStyle.h"

#include <vtkArrowSource.h>
#include <vtkCellLocator.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolygon.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkClipPolyData.h>
#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
#include <vtkConeSource.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSTLReader.h>

#include <vtkBox.h>
#include <numeric>
#include <vector>

#include"VtkUtillity.h"
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
void PlanAreaInteractorStyle::setActor(vtkSmartPointer<vtkActor> actor)
{
    curActor = actor; 
    vtkPolyData* poly = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInput());
    curPolyData->DeepCopy(poly);
    VtkUtillity::fixPolyData(curPolyData);

    vtkNew<vtkPolyDataNormals>normal;
    normal->SetInputData(curPolyData);
    normal->ComputeCellNormalsOn();
    normal->ComputePointNormalsOn();
    normal->Update();


    curPolyataWithNor = normal->GetOutput();
    if (picker==nullptr){
		picker =vtkSmartPointer<vtkPointPicker>::New();
        //picker->SetTolerance(1e-5);
    }
    picker->InitializePickList();
    picker->AddPickList(actor);
    picker->PickFromListOn();

}
void PlanAreaInteractorStyle::OnLeftButtonDown() {
  int* mousePos = GetInteractor()->GetEventPosition();

  double pickedPosition[3];
  if (picker->Pick(mousePos[0], mousePos[1], 0, curRenderer)) { 
    picker->GetPickPosition(pickedPosition);
    std::cout << "Picked position: " << pickedPosition[0] << ", "
              << pickedPosition[1] << ", " << pickedPosition[2] << std::endl;

    //double* normal = picker->GetPickNormal();
    addSquare(pickedPosition);
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(pickedPosition);
	sphereSource->SetRadius(0.0005);
	sphereSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(sphereSource->GetOutput());


	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

	curRenderer->AddActor(actor);

    curRenderer->GetRenderWindow()->Render();  

  } else {
    std::cout << "No actor picked at the mouse position." << std::endl;
    std::cout << "Picked position: " << pickedPosition[0] << ", "
              << pickedPosition[1] << ", " << pickedPosition[2] << std::endl;
  }
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void PlanAreaInteractorStyle::OnMiddleButtonDown() {
  //int* mousePos = GetInteractor()->GetEventPosition();
  //vtkSmartPointer<vtkPointPicker> picker =
  //    vtkSmartPointer<vtkPointPicker>::New();
  //double pickedPosition[3];
  //std::cout << "mousePos position: " << mousePos[0] << ", " << mousePos[1]
  //          << std::endl;
  //if (picker->Pick(mousePos[0], mousePos[1], 0, curRenderer)) {
  //  picker->GetPickPosition(pickedPosition);
  //  std::cout << "Picked position: " << pickedPosition[0] << ", "
  //            << pickedPosition[1] << ", " << pickedPosition[2] << std::endl;
  //  drawSquare(pickedPosition);
  //  // 创建一个球体
  //  vtkSmartPointer<vtkSphereSource> sphereSource =
  //      vtkSmartPointer<vtkSphereSource>::New();
  //  sphereSource->SetCenter(pickedPosition);
  //  sphereSource->SetRadius(0.0005);  // 设置球体半径
  //  sphereSource->Update();

  //  // 创建映射器
  //  vtkSmartPointer<vtkPolyDataMapper> mapper =
  //      vtkSmartPointer<vtkPolyDataMapper>::New();
  //  mapper->SetInputData(sphereSource->GetOutput());

  //  // 创建演员
  //  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  //  actor->SetMapper(mapper);
  //  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // 设置球体颜色为红色

  //  // 将球体添加到渲染器
  //  curRenderer->AddActor(actor);
  //  curRenderer->GetRenderWindow()->Render();  // 刷新渲染窗口

  //} else {
  //  std::cout << "No actor picked at the mouse position." << std::endl;
  //}
  vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}

//void PlanAreaInteractorStyle::OnRightButtonDown() {
//  //int* clickPos = this->GetInteractor()->GetEventPosition();
//  //vtkSmartPointer<vtkPointPicker> picker =
//  //    vtkSmartPointer<vtkPointPicker>::New();
//  //picker->Pick(clickPos[0], clickPos[1], 0, this->curRenderer);
//  //double pickedPosition[3];
//  //picker->GetPickPosition(pickedPosition);
//  //this->highlightSquareArea(pickedPosition);
//   vtkInteractorStyleTrackballCamera::OnRightButtonDown();
//}
void PlanAreaInteractorStyle::addSquare( double worldPos[3])
{
	//vtkSmartPointer<vtkPolyData> output = curPolyataWithNor->GetOutput();
	vtkSmartPointer<vtkPointData> pointData = curPolyataWithNor->GetPointData();
	vtkSmartPointer<vtkDataArray> normalArray = pointData->GetNormals();
	vtkIdType pointId = curPolyData->FindPoint(worldPos);
	if (pointId != -1)
	{
		double normal[3];
		normalArray->GetTuple(pointId, normal);

		std::cout << "Normal at point (" << worldPos[0] << ", " << worldPos[1] << ", " << worldPos[2] << "): ("
			<< normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << std::endl;

		double lineLength = 0.02; // 线的长度
		double endPoint[3] = {
            worldPos[0] + normal[0] * lineLength,
            worldPos[1] + normal[1] * lineLength,
            worldPos[2] + normal[2] * lineLength
		};

		// 创建线
		vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
		lineSource->SetPoint1(worldPos);    // 设置起始点
		lineSource->SetPoint2(endPoint); // 设置结束点
		lineSource->Update();
		vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		lineMapper->SetInputConnection(lineSource->GetOutputPort());
		vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
		lineActor->SetMapper(lineMapper);
		lineActor->GetProperty()->SetColor(0.0, 1.0, 0.0); 
        curRenderer->AddActor(lineActor);
        curRenderer->GetRenderWindow()->Render();

		double n[3] = { normal[0], normal[1], normal[2] };
		vtkMath::Normalize(n);
		vtkSmartPointer<vtkTransform>transform = getTransform(n, worldPos);

		vtkNew<vtkBox> box;
		double halfSize = 0.005;
		double bounds[6] = { -halfSize, halfSize, -halfSize, halfSize, -0.001, 0.001 };

        vtkNew<vtkTransform>form;
        form->Translate(worldPos);
        box->SetTransform(form);

		vtkNew<vtkCubeSource> cubeSource;
		cubeSource->SetBounds(-halfSize, halfSize, -halfSize, halfSize, -0.005, 0.005);
		cubeSource->Update();



		vtkNew<vtkSTLReader> stlReader;
		stlReader->SetFileName("C:\\Users\\A015240\\Desktop\\1\\1.stl");
		stlReader->Update();  // 读取 STL 文件



		vtkNew<vtkTransformPolyDataFilter> transformFilter;
		transformFilter->SetTransform(transform);
		transformFilter->SetInputData(stlReader->GetOutput());
        transformFilter->Update();


		//vtkNew<vtkPolyDataNormals> normals;
		//normals->SetInputData(transformFilter->GetOutput());
		//normals->ConsistencyOn();
		//normals->AutoOrientNormalsOn();
		//normals->SplittingOff();  // 防止边缘破裂
		//normals->Update();
		//implicitPolyData->SetInput();

		vtkNew<vtkImplicitPolyDataDistance> implicitPolyData;
		implicitPolyData->SetInput(transformFilter->GetOutput());

		vtkNew<vtkPolyDataMapper> boxMapper;
		boxMapper->SetInputConnection(transformFilter->GetOutputPort());

		vtkNew<vtkActor> boxActor;
		boxActor->SetMapper(boxMapper);


		vtkNew<vtkLoopSubdivisionFilter> subdiv;
		subdiv->SetInputData(curPolyData);
		subdiv->SetNumberOfSubdivisions(1);  // 可尝试 1~3
		subdiv->Update();
  
		vtkNew<vtkClipPolyData> clipper;
		clipper->SetInputData(curPolyData);
		clipper->SetClipFunction(implicitPolyData);
		clipper->InsideOutOn();
		clipper->Update();

 
		//vtkNew<vtkExtractPolyDataGeometry> extractor;
		//extractor->SetInputData(curPolyData);
		//extractor->SetImplicitFunction(implicitPolyData);
		//extractor->ExtractInsideOn();           // 提取在 cube 内的
		//extractor->Update();
		//vtkPolyData* result = extractor->GetOutput();
		//if (!result || result->GetNumberOfCells() == 0) {
		//	std::cout << "result is empty!" << std::endl;
		//}

		boxActor->GetProperty()->SetLineWidth(2.0);      // 线框模式下边框宽度
        boxActor->GetProperty()->SetColor(0, 1, 0);
        boxActor->GetProperty()->SetOpacity(0.3);
		//boxActor->GetProperty()->SetRepresentationToWireframe(); // 线框模式;

		curRenderer->AddActor(boxActor);
		curRenderer->GetRenderWindow()->Render();

		vtkPolyData* clippedData = clipper->GetOutput();

	
		if (!clippedData || clippedData->GetNumberOfCells() == 0) {
			std::cout << "clippedData is empty!" << std::endl;
		}

		// 创建并显示裁剪后的Actor
		vtkNew<vtkPolyDataMapper> clippedMapper;
		clippedMapper->SetInputData(clippedData);
        vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
        vtkPolyDataMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(1.1, 1.5);
		//clippedMapper->SetResolveCoincidentTopologyToPolygonOffset();
		//clippedMapper->SetResolveCoincidentTopologyPolygonOffsetParameters(1.1, 10); // 增大 units
		vtkNew<vtkActor> clippedActor;
		clippedActor->SetMapper(clippedMapper);
		clippedActor->GetProperty()->SetColor(1, 0, 0);
        //clippedActor->GetProperty()->SetRenderOrder(1);
		/*       clippedActor->GetProperty()->EdgeVisibilityOn(); 
			   clippedActor->GetProperty()->SetEdgeColor(0, 0, 0);
			   clippedActor->GetProperty()->SetLineWidth(1.1);*/
		curRenderer->AddActor(clippedActor);
		curRenderer->GetRenderWindow()->Render();

        vtkNew<vtkRenderer>clipperRender;
        vtkNew<vtkRenderWindow>clipperWider;
        vtkNew<vtkRenderWindowInteractor>intor;
        clipperRender->AddActor(clippedActor);
        /*clipperWider->render*/
        clipperWider->AddRenderer(clipperRender);
        clipperWider->SetInteractor(intor);
        intor->Start();

	}
	else
	{
		std::cout << "Point not found in the polydata!" << std::endl;
	}
        
}

vtkSmartPointer<vtkTransform> PlanAreaInteractorStyle::getTransform(double normal[3], double pos[3])
{
    double defNormal[3] = { 0,0,-1 };
    double rotAxis[3];


    vtkMath::Cross(defNormal, normal, rotAxis);

    double costha = vtkMath::Dot(defNormal, normal);
    double tha =  vtkMath::DegreesFromRadians(acos(costha));

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    //transform->RotateZ(60);
    //transform-> ();

    transform->Translate(pos);

	if (vtkMath::Norm(rotAxis) > 1e-6) 
	{
		transform->RotateWXYZ(tha, rotAxis); // 绕 rotAxis 旋转 tha 角度
	}

   

    //transform->RotateWXYZ(60, normal);
    //transform->RotateZ(0);

	return transform;
}


void PlanAreaInteractorStyle::drawSquare(const double worldPos[3]) {
  // 计算综合法线
  double normal[3];
  std::unordered_set<vtkIdType> visitedCells;
  computeAverageNormal(worldPos, 30, normal, visitedCells);
  std::cout << "cell size: " << visitedCells.size() << std::endl;
  double mutableWorldPos[3];
  mutableWorldPos[0] = worldPos[0];
  mutableWorldPos[1] = worldPos[1];
  mutableWorldPos[2] = worldPos[2];
  // 生成正方形
  // vtkSmartPointer<vtkPlaneSource> plane =
  //     vtkSmartPointer<vtkPlaneSource>::New();
  // plane->SetCenter(mutableWorldPos);
  // plane->SetNormal(normal);
  // plane->SetXResolution(1);
  // plane->SetYResolution(1);
  // plane->SetOrigin(worldPos[0] - 0.01, worldPos[1] - 0.01, worldPos[2]);
  // plane->SetPoint1(worldPos[0] + 0.01, worldPos[1] - 0.01, worldPos[2]);
  // plane->SetPoint2(worldPos[0] - 0.01, worldPos[1] + 0.01, worldPos[2]);
  // plane->Update();

  // // 投影正方形
  // vtkSmartPointer<vtkPolyData> projectedPolyData =
  //     vtkSmartPointer<vtkPolyData>::New();
  // projectSquareToSurface(plane->GetOutput(), projectedPolyData);
  vtkSmartPointer<vtkPolyData> highlightPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  highlightPolyData->SetPoints(curPolyData->GetPoints());
  vtkSmartPointer<vtkCellArray> highlightCells =
      vtkSmartPointer<vtkCellArray>::New();
  for (const auto& id : visitedCells) {
    vtkSmartPointer<vtkGenericCell> cell =
        vtkSmartPointer<vtkGenericCell>::New();
    curPolyData->GetCell(id, cell);
    highlightCells->InsertNextCell(cell);
  }
  highlightPolyData->SetPolys(highlightCells);
  vtkSmartPointer<vtkPolyDataMapper> highlightMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  highlightMapper->SetInputData(highlightPolyData);
  vtkSmartPointer<vtkActor> highlightActor = vtkSmartPointer<vtkActor>::New();
  highlightActor->SetMapper(highlightMapper);

  // 渲染投影区域
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  // mapper->SetInputData(plane->GetOutput());
  // 创建箭头
  vtkSmartPointer<vtkArrowSource> arrowSource =
      vtkSmartPointer<vtkArrowSource>::New();
  arrowSource->Update();

  // 创建变换矩阵
  vtkSmartPointer<vtkTransform> transform =
      vtkSmartPointer<vtkTransform>::New();
  transform->Translate(worldPos);

  // 计算旋转轴和角度
  double zAxis[3] = {0.0, 0.0, 1.0};  // Z轴
  double rotationAxis[3];
  vtkMath::Cross(zAxis, normal, rotationAxis);
  double rotationAngle =
      vtkMath::DegreesFromRadians(acos(vtkMath::Dot(zAxis, normal)));

  transform->RotateWXYZ(rotationAngle, rotationAxis);
  transform->Scale(0.0050, 0.0050, 0.0050);  // 可以根据需要调整箭头的缩放比例

  // 应用变换到箭头
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetTransform(transform);
  transformFilter->SetInputConnection(arrowSource->GetOutputPort());
  transformFilter->Update();

  // 创建箭头映射器和演员
  vtkSmartPointer<vtkPolyDataMapper> arrowMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  arrowMapper->SetInputConnection(transformFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> arrowActor = vtkSmartPointer<vtkActor>::New();
  arrowActor->SetMapper(arrowMapper);
  arrowActor->GetProperty()->SetColor(1.0, 0.0, 0.0);  

  // 添加箭头演员到渲染器
  curRenderer->AddActor(arrowActor);

  this->curRenderer->AddActor(highlightActor);
  this->curSquareActor->SetMapper(mapper);
  this->curSquareActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
  this->curRenderer->GetRenderWindow()->Render();
}

void PlanAreaInteractorStyle::highlightSquareArea(const double worldPos[3]) {
  // 计算综合法线
  double normal[3];
  std::unordered_set<vtkIdType> visitedCells;
  computeAverageNormal(worldPos, 15, normal, visitedCells);
  double mutableWorldPos[3];
  mutableWorldPos[0] = worldPos[0];
  mutableWorldPos[1] = worldPos[1];
  mutableWorldPos[2] = worldPos[2];
  // 生成正方形
  vtkSmartPointer<vtkPlaneSource> plane =
      vtkSmartPointer<vtkPlaneSource>::New();
  plane->SetCenter(mutableWorldPos);
  plane->SetNormal(normal);
  plane->SetXResolution(1);
  plane->SetYResolution(1);
  plane->SetOrigin(worldPos[0] - 0.01, worldPos[1] - 0.01, worldPos[2]);
  plane->SetPoint1(worldPos[0] + 0.01, worldPos[1] - 0.01, worldPos[2]);
  plane->SetPoint2(worldPos[0] - 0.01, worldPos[1] + 0.01, worldPos[2]);
  plane->Update();

  // 投影正方形
  vtkSmartPointer<vtkPolyData> projectedPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  projectSquareToSurface(plane->GetOutput(), projectedPolyData);

  // 渲染投影区域
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(plane->GetOutput());

  vtkSmartPointer<vtkActor> newSquareActor = vtkSmartPointer<vtkActor>::New();
  newSquareActor->SetMapper(mapper);
  newSquareActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

  this->highlightedSquares.push_back(newSquareActor);
  this->curRenderer->AddActor(newSquareActor);
  this->curRenderer->GetRenderWindow()->Render();
}
void PlanAreaInteractorStyle::computeAverageNormal(
    const double worldPos[3], int range, double normal[3],
    std::unordered_set<vtkIdType>& visitedCells) {
  // 检查 curPolyData 是否初始化
  if (!curPolyData) {
    std::cerr << "curPolyData is null!" << std::endl;
    return;
  }

  // 使用 vtkPolyDataNormals 计算法线
  vtkSmartPointer<vtkPolyDataNormals> normalsFilter =
      vtkSmartPointer<vtkPolyDataNormals>::New();
  normalsFilter->SetInputData(curPolyData);
  normalsFilter->ComputePointNormalsOn();
  normalsFilter->Update();

  vtkSmartPointer<vtkCellLocator> cellLocator =
      vtkSmartPointer<vtkCellLocator>::New();
  cellLocator->SetDataSet(curPolyData);
  cellLocator->BuildLocator();

  double closestPoint[3];
  double dist2;
  vtkIdType cellId;
  int subId;
  cellLocator->FindClosestPoint(worldPos, closestPoint, cellId, subId, dist2);

  std::queue<vtkIdType> cellsQueue;
  // std::unordered_set<vtkIdType> visitedCells;
  cellsQueue.push(cellId);
  visitedCells.insert(cellId);

  double sumNormal[3] = {0.0, 0.0, 0.0};
  int cellsCount = 0;

  vtkSmartPointer<vtkPolyData> polyDataWithNormals = normalsFilter->GetOutput();

  while (!cellsQueue.empty() && cellsCount < range) {
    vtkIdType currentCellId = cellsQueue.front();
    cellsQueue.pop();

    double currentNormal[3] = {0.0, 0.0, 0.0};

    // 获取当前单元格的顶点
    vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
    curPolyData->GetCellPoints(currentCellId, cellPointIds);

    // 累加当前单元格所有顶点的法线
    for (vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
      vtkIdType pointId = cellPointIds->GetId(i);
      polyDataWithNormals->GetPointData()->GetNormals()->GetTuple(
          pointId, currentNormal);
      vtkMath::Add(sumNormal, currentNormal, sumNormal);
    }
    cellsCount++;

    // 获取相邻单元格
    for (vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
      vtkSmartPointer<vtkIdList> neighboringCells =
          vtkSmartPointer<vtkIdList>::New();
      vtkIdType pointId = cellPointIds->GetId(i);
      curPolyData->GetPointCells(pointId, neighboringCells);

      for (vtkIdType j = 0; j < neighboringCells->GetNumberOfIds(); j++) {
        vtkIdType neighborCellId = neighboringCells->GetId(j);
        if (visitedCells.size() >= range) {
          break;
        }
        if (visitedCells.find(neighborCellId) == visitedCells.end()) {
          cellsQueue.push(neighborCellId);
          visitedCells.insert(neighborCellId);
        }
      }
      if (visitedCells.size() >= range) {
        break;
      }
    }
  }

  vtkMath::Normalize(sumNormal);
  normal[0] = sumNormal[0];
  normal[1] = sumNormal[1];
  normal[2] = sumNormal[2];
}
void PlanAreaInteractorStyle::projectSquareToSurface(
    vtkPolyData* squarePolyData, vtkPolyData* projectedPolyData) {
  vtkSmartPointer<vtkPoints> squarePoints = squarePolyData->GetPoints();
  vtkSmartPointer<vtkPoints> projectedPoints =
      vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkCellLocator> cellLocator =
      vtkSmartPointer<vtkCellLocator>::New();
  cellLocator->SetDataSet(curPolyData);
  cellLocator->BuildLocator();

  for (vtkIdType i = 0; i < squarePoints->GetNumberOfPoints(); ++i) {
    double squarePoint[3];
    squarePoints->GetPoint(i, squarePoint);

    double closestPoint[3];
    double dist2;
    vtkIdType cellId;
    int subId;
    cellLocator->FindClosestPoint(squarePoint, closestPoint, cellId, subId,
                                  dist2);
    projectedPoints->InsertNextPoint(closestPoint);
  }

  projectedPolyData->SetPoints(projectedPoints);
  projectedPolyData->SetPolys(squarePolyData->GetPolys());
}


