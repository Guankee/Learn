#include "PlanAreaInteractorStyle.h"

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

#include <numeric>
#include <vector>

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
	}
	else {
		std::cout << "No actor picked at the mouse position." << std::endl;
		std::cout << "Picked position: " << pickedPosition[0] << ", "
			<< pickedPosition[1] << ", " << pickedPosition[2] << std::endl;
	}
	vtkInteractorStyleTrackballCamera::OnMouseMove();
}
void PlanAreaInteractorStyle::OnRightButtonDown() {
	int* clickPos = this->GetInteractor()->GetEventPosition();
	vtkSmartPointer<vtkPointPicker> picker =
		vtkSmartPointer<vtkPointPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0, this->curRenderer);
	double pickedPosition[3];
	picker->GetPickPosition(pickedPosition);
	this->highlightSquareArea(pickedPosition);
	// vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void PlanAreaInteractorStyle::drawSquare(const double worldPos[3]) {
	// 计算综合法线
	double normal[3];
	computeAverageNormal(worldPos, 5, normal);
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

	this->curSquareActor->SetMapper(mapper);
	this->curSquareActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
	this->curRenderer->GetRenderWindow()->Render();
}

void PlanAreaInteractorStyle::highlightSquareArea(const double worldPos[3]) {
	// 计算综合法线
	double normal[3];
	computeAverageNormal(worldPos, 5, normal);
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
void PlanAreaInteractorStyle::computeAverageNormal(const double worldPos[3],
												   int range,
												   double normal[3]) {
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
	std::unordered_set<vtkIdType> visitedCells;
	cellsQueue.push(cellId);
	visitedCells.insert(cellId);

	double sumNormal[3] = { 0.0, 0.0, 0.0 };
	int cellsCount = 0;

	vtkSmartPointer<vtkPolyData> polyDataWithNormals = normalsFilter->GetOutput();

	while (!cellsQueue.empty() && cellsCount < range) {
		vtkIdType currentCellId = cellsQueue.front();
		cellsQueue.pop();

		double currentNormal[3] = { 0.0, 0.0, 0.0 };

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
				if (visitedCells.find(neighborCellId) == visitedCells.end()) {
					cellsQueue.push(neighborCellId);
					visitedCells.insert(neighborCellId);
				}
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