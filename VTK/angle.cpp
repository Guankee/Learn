#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
// vtkTransformPolyDataFilter
#include <vtkAppendPolyData.h>
#include <vtkAxesActor.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#include <Eigen/Dense>
#include <cmath>
#include <iostream>

#include "CustomVtkArrowActor.h"

#define M_PI 3.14159265358979323846  // pi

void rotateNormalTowardsDirection(const Eigen::Vector3d& p1,
								  const Eigen::Vector3d& p2,
								  const Eigen::Vector3d& p3, double angle,
								  Eigen::Vector3d& newNormal) {
	Eigen::Vector3d v = p2 - p1;                                // 目标方向
	Eigen::Vector3d n = (p2 - p1).cross(p3 - p1).normalized();  // 原法线
	Eigen::Vector3d k = v.cross(n).normalized();                // 旋转轴

   // Eigen::Vector3d k = n.cross(v).normalized();  // 旋转轴

	double radians = angle * M_PI / 180.0;  // 转换为弧度
	newNormal = n * cos(radians) + (k.cross(n)) * sin(radians) +
		k * (k.dot(n)) * (1 - cos(radians));
}
void rotateNormal(const Eigen::Vector3d& input, const Eigen::Vector3d& axis,
				  double angle, Eigen::Vector3d& output) {
	double radians = angle * M_PI / 180.0;  // 转换为弧度

	output = input * cos(radians) + (axis.cross(input)) * sin(radians) +
		axis * (axis.dot(input)) * (1 - cos(radians));

}
void rotateNormal(const Eigen::Vector3d& input, const Eigen::Vector3d& p1,
				  const Eigen::Vector3d& p2,
				  double angle, Eigen::Vector3d& output) {
	Eigen::Vector3d v = p2 - p1;
	Eigen::Vector3d axis = v.cross(input).normalized();
	double radians = angle * M_PI / 180.0;  // 转换为弧度
	output = input * cos(radians) + (axis.cross(input)) * sin(radians) +
		axis * (axis.dot(input)) * (1 - cos(radians));

}
Eigen::Vector3d averageNormals(const Eigen::Vector3d& n1, const Eigen::Vector3d& n2) {
	return (n1 + n2).normalized();
}

vtkPolyData* CreateSingleVectorArrowData(double* origin, double* target,
										 double length) {
	vtkMath* math;
	math->Subtract(target, origin, target);  // 计算向量的朝向
											 // target=target-origin

	vtkPoints* points = vtkPoints::New();  // 记录起点坐标
	points->InsertNextPoint(origin);

	vtkVertex* vertex =
		vtkVertex::New();  // 建立起点的拓扑(不建立拓扑的话是不行的)
	vertex->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());
	for (int i = 0; i < points->GetNumberOfPoints(); i++) {
		vertex->GetPointIds()->SetId(i, i);  // setId(拓扑的id, 顶点的id)
	}

	vtkDoubleArray* normals =
		vtkDoubleArray::New();  // 创建法向量属性，存入向量的朝向target
	normals->SetNumberOfComponents(3);
	normals->InsertNextTuple(target);

	vtkDoubleArray* scalars =
		vtkDoubleArray::New();  // 创建标量属性，存入向量的长度length
	scalars->SetNumberOfComponents(1);
	scalars->SetName("scalars");
	scalars->InsertNextTuple1(length);

	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->InsertNextCell(
		vertex);  // 将建立的拓扑用vtkCellArray封装，用于赋予vtkPolyData

	vtkPolyData* polydata = vtkPolyData::New();     // 创建几何实体
	polydata->SetPoints(points);                    // 赋予起点
	polydata->SetVerts(vertices);                   // 赋予拓扑
	polydata->GetPointData()->SetNormals(normals);  // 赋予向量朝向
	polydata->GetPointData()->SetScalars(scalars);  // 赋予向量长度
	return polydata;
}

vtkSmartPointer<vtkPolyData> CreateArrow(double* startPoint, double* direction,
										 double length) {
	vtkSmartPointer<vtkArrowSource> arrowSource =
		vtkSmartPointer<vtkArrowSource>::New();

	double endPoint[3] = { startPoint[0] + direction[0] * length,
						  startPoint[1] + direction[1] * length,
						  startPoint[2] + direction[2] * length };

	double arrowDirection[3] = { endPoint[0] - startPoint[0],
								endPoint[1] - startPoint[1],
								endPoint[2] - startPoint[2] };
	double arrowLength = vtkMath::Norm(arrowDirection);
	if (arrowLength == 0.0) return nullptr;  // 如果起点和终点重合则返回空

	// 归一化方向向量
	vtkMath::Normalize(arrowDirection);
	double angle =
		atan2(arrowDirection[1], arrowDirection[0]) * 180.0 / vtkMath::Pi();

	// 创建变换
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Translate(startPoint[0], startPoint[1],
						 startPoint[2]);  // 设置箭头的起点
	transform->RotateZ(angle);            // 根据方向旋转箭头
	transform->Scale(arrowLength * 0.1, arrowLength * 0.1,
					 arrowLength * 0.1);  // 根据箭头长度缩放

	// 应用变换
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputConnection(arrowSource->GetOutputPort());
	transformFilter->SetTransform(transform);
	transformFilter->Update();

	// 返回箭头的 PolyData
	return transformFilter->GetOutput();
}
vtkSmartPointer<vtkActor> creatLine(Eigen::Vector3d p1, Eigen::Vector3d p2) {
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(p1(0), p1(1), p1(2));  // 第一个点
	points->InsertNextPoint(p2(0), p2(1), p2(2));  // 第二个点

	// 创建线段
	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 0);  // 线的第一个点
	line->GetPointIds()->SetId(1, 1);  // 线的第二个点

	// 创建 vtkPolyData 对象
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);

	// 创建一个单元数组来存储线段
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(line);  // 添加线段到单元数组
	polyData->SetLines(lines);    // 将单元数组设置到 PolyData

	// 创建映射器和演员
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);  // 设置输入数据

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // 设置颜色为红色

	return actor;
}

vtkSmartPointer<vtkPolyData> createDefaultArrowGeometry() {
	vtkSmartPointer<vtkArrowSource> arrowSource =
		vtkSmartPointer<vtkArrowSource>::New();
	arrowSource->Update();  // 生成箭头几何体
	return arrowSource->GetOutput();
}
// void createTextActor(vtkSmartPointer<vtkRenderer> renderer, double pos[3],
//                      const std::string& text,
//                      vtkSmartPointer<vtkTextActor>& textActor) {
//   textActor = vtkSmartPointer<vtkTextActor>::New();
//   textActor->SetText(text.c_str());
//   textActor->SetTexture
//   // 将三维坐标转换为屏幕坐标
//   double screenPos[3];
//   renderer->GetActiveCamera()->GetPosition();  // 获取相机位置
//   renderer->SetDisplayPoint(pos[0], pos[1], pos[2]);
//   renderer->DisplayToNormalizedDisplay(renderer->GetDisplayPoint(screenPos));

//   textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
//   textActor->SetPosition(screenPos[0],
//                          screenPos[1]);  // 使用转换后的屏幕坐标设置位置

//   textActor->GetTextProperty()->SetFontSize(24);
//   textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);  // 设置文本颜色为白色
// }
vtkSmartPointer<CustomVtkArrowActor> createArrowActor(double* start,
													  double* end,
													  vtkStdString colorStr) {
	const double arrowFaceScale = 0.08;  // 0.08 is arrow for the face scale
	const double needleLength = 0.03;

	vtkSmartPointer<vtkPolyData> needlePolyData = createDefaultArrowGeometry();

	vtkSmartPointer<CustomVtkArrowActor> arrowActor =
		vtkSmartPointer<CustomVtkArrowActor>::New();
	arrowActor->createArrow(start, end, needlePolyData);

	vtkNew<vtkNamedColors> colors;
	arrowActor->GetProperty()->SetColor(colors->GetColor3d(colorStr).GetData());
	arrowActor->GetProperty()->SetDiffuse(1.0);
	arrowActor->GetProperty()->SetSpecular(0.0);

	return arrowActor;
}
// void fitPlaneNormal(const Eigen::MatrixXd& points, Eigen::Vector4d& normal) {
// 	// 计算均值
// 	Eigen::Vector3d centroid = points.colwise().mean();
// 	Eigen::MatrixXd centered = points.rowwise() - centroid.transpose();

// 	// 打印中心化后的矩阵以调试
// 	std::cout << "Centered Matrix:\n" << centered << std::endl;

// 	// 计算SVD
// 	Eigen::JacobiSVD<Eigen::MatrixXd> svd(centered.transpose(), Eigen::ComputeThinU | Eigen::ComputeThinV);
// 	// 检查svd的结果
// 	std::cout << "U Matrix:\n" << svd.matrixU() << std::endl;
// 	std::cout << "S Matrix:\n" << svd.singularValues() << std::endl;
// 	std::cout << "V Matrix:\n" << svd.matrixV() << std::endl;
// 	normal = svd.matrixV().col(2);
// 	normal.normalize();

// 	//Eigen::Vector4d n = svd.matrixV().col(2); // 最后一个特征向量
// 	//Eigen::Vector3d temp{ normal(0) / normal(3),normal(1) / normal(3),normal(2) / normal(3) };
// 	//normal = temp.normalized();
// }


int main() {



	// Eigen::MatrixXd points2(4, 3);
	// points2 << 0, 0, 0,
	// 	1, 0, 0,
	// 	0, 1, 0,
	// 	0, -1, 0;

	// std::cout << "Points Matrix:\n" << points2 << std::endl;

	// double angle = 60.0;  // 旋转角度
	// Eigen::Vector4d newNormal;

	// fitPlaneNormal(points2, newNormal);

	// std::cout << "Plane Normal: " << newNormal.transpose() << std::endl;

	// return 0;


	Eigen::Vector3d p1(0, 0, 0);
	Eigen::Vector3d p2(1, 0, 0);
	Eigen::Vector3d p3(0, 1, 0);
	Eigen::Vector3d p4(0, -1, 0);
 // 计算向量 p1p2 和 p1p3
    Eigen::Vector3d v1 = p2 - p1;
    Eigen::Vector3d v2 = p3 - p1;

    // 计算法向量（叉积）
    Eigen::Vector3d normal = v1.cross(v2).normalized();

    // 平面方程 Ax + By + Cz + D = 0 的 D
    double D = -normal.dot(p1);

    // 计算 p4 到平面的距离公式：|Ax + By + Cz + D| / sqrt(A^2 + B^2 + C^2)
    double distance = std::abs(normal(0) * p4(0) + normal(1) * p4(1) + normal(2) * p4(2) + D) / normal.norm();
	// rotateNormalTowardsDirection(p1, p2, p3, angle, newNormal);

	// 创建 VTK 点
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(p1(0), p1(1), p1(2));
	points->InsertNextPoint(p2(0), p2(1), p2(2));
	points->InsertNextPoint(p3(0), p3(1), p3(2));
	points->InsertNextPoint(p4(0), p4(1), p4(2));
	// 圆锥的点
	vtkSmartPointer<vtkPoints> conePoints = vtkSmartPointer<vtkPoints>::New();
	// 创建一个平面
	vtkSmartPointer<vtkPlaneSource> planeSource =
		vtkSmartPointer<vtkPlaneSource>::New();
	planeSource->SetOrigin(0, 0, 0);
	planeSource->SetPoint1(1, 0, 0);
	planeSource->SetPoint2(0, -1, 0);
	planeSource->SetResolution(10, 10);

	// 创建平面的映射和演员
	vtkSmartPointer<vtkPolyDataMapper> planeMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	planeMapper->SetInputConnection(planeSource->GetOutputPort());
	vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor>::New();
	planeActor->SetMapper(planeMapper);
	planeActor->GetProperty()->SetOpacity(0.5);  // 设置透明度

	// const double arrowFaceScale = 0.08;//0.08 is arrow for the face scale
	const double needleLength = 0.01;

	// Eigen::Vector3d p1(0, 0, 0);  // 起始点
	// Eigen::Vector3d newNormal(1, 1, 0).normalized();  // 单位方向向量

	// 计算新点
	Eigen::Vector3d n = (p3 - p1).cross(p2 - p1).normalized();  // 原法线



	Eigen::Vector3d oldPoint = p2 + 1 * n;
	double startPoint[3] = { p2(0), p2(1), p2(2) };  // 箭头根部
	double end[3] = { oldPoint(0), oldPoint(1), oldPoint(2) };
	// points->InsertNextPoint(newPoint(0), newPoint(1), newPoint(2));
	conePoints->InsertNextPoint(oldPoint(0), oldPoint(1), oldPoint(2));
	// double end[3] = { p1(0) + needleLength * newNormal(0),
	//	p1(1) + needleLength * newNormal(1),
	//	p1(2) + needleLength * newNormal(2) };
	//  double startPoint[3] = { 0, 0, 0 }; // 箭头根部
	//  double end[3] = { 0, 0, 1 };
	//  vtkSmartPointer<CustomVtkArrowActor> arrowActor =
	//  createArrowActor(startPoint, end, "blue");
	//  arrowActor->GetProperty()->SetOpacity(0.3);

	vtkSmartPointer<vtkActor> lineactor = creatLine(p2, oldPoint);
	Eigen::Vector3d vec;
	rotateNormal(n, p1, p2, 30, vec);
	Eigen::Vector3d newPoint = p2 + 1 * vec;

	// points->InsertNextPoint(newPoint(0), newPoint(1), newPoint(2));
	conePoints->InsertNextPoint(newPoint(0), newPoint(1), newPoint(2));
	// double end[3] = { p1(0) + needleLength * newNormal(0),
	//	p1(1) + needleLength * newNormal(1),
	//	p1(2) + needleLength * newNormal(2) };
	//  double startPoint[3] = { 0, 0, 0 }; // 箭头根部
	//  double end[3] = { 0, 0, 1 };
	//  vtkSmartPointer<CustomVtkArrowActor> arrowActor =
	//  createArrowActor(startPoint, end, "blue");
	//  arrowActor->GetProperty()->SetOpacity(0.3);

	vtkSmartPointer<vtkActor> newlineactor = creatLine(p2, newPoint);

	// Eigen::Vector3d n = (p2 - p1).cross(p3 - p1).normalized();  // 原法线

	double old_startPoint[3] = { p1(0), p1(1), p1(2) };  // 箭头根部
	double old_end[3] = { p1(0) + needleLength * n(0), p1(1) + needleLength * n(1),
						 p1(2) + needleLength * n(2) };
	vtkSmartPointer<CustomVtkArrowActor> old_arrowActor =
		createArrowActor(old_startPoint, old_end, "red");
	old_arrowActor->GetProperty()->SetOpacity(0.3);
	// 创建球体表示点
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(0.05);  // 球体半径

	vtkSmartPointer<vtkPolyData> polyPointData =
		vtkSmartPointer<vtkPolyData>::New();
	polyPointData->SetPoints(points);

	vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetSourceConnection(sphereSource->GetOutputPort());
	glyph->SetInputData(polyPointData);
	glyph->SetScaleFactor(1.0);

	// 创建映射和演员
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(glyph->GetOutputPort());

	vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();
	sphereActor->SetMapper(mapper);
	sphereActor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // 设置颜色为红色

	vtkSmartPointer<vtkConeSource> coneSource =
		vtkSmartPointer<vtkConeSource>::New();
	coneSource->SetHeight(0.3);     // 设置圆锥高度
	coneSource->SetRadius(0.1);     // 设置底部半径
	coneSource->SetResolution(10);  // 设置圆锥底部的分辨率

	vtkSmartPointer<vtkPolyData> conePloyData =
		vtkSmartPointer<vtkPolyData>::New();
	conePloyData->SetPoints(conePoints);

	vtkSmartPointer<vtkGlyph3D> coneGlyp = vtkSmartPointer<vtkGlyph3D>::New();
	coneGlyp->SetSourceConnection(coneSource->GetOutputPort());
	coneGlyp->SetInputData(conePloyData);
	coneGlyp->SetScaleFactor(0.6);

	vtkSmartPointer<vtkPolyDataMapper> coneMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	coneMapper->SetInputConnection(coneGlyp->GetOutputPort());

	vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
	coneActor->SetMapper(coneMapper);
	coneActor->GetProperty()->SetColor(0.0, 1.0, 0.0);  // 设置颜色为红色
	// 设置渲染
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	axes->SetTotalLength(1.0, 1.0, 1.0);  // 设置坐标轴的长度





	// 添加演员到渲染器
	renderer->AddActor(axes);
	renderer->AddActor(planeActor);
	// renderer->AddActor(arrowActor);
	renderer->AddActor(sphereActor);

	renderer->AddActor(lineactor);
	renderer->AddActor(newlineactor);

	// renderer->AddActor(coneActor);
	// renderer->AddActor(old_arrowActor);

	// sphereActor
	// renderer->AddActor(rotatedActor_n);
	// renderer->AddActor(rotatedActor);
	renderer->SetBackground(0, 0, 0);  // 设置背景色为黑色

	renderWindow->Render();
	renderWindowInteractor->Start();

	return 0;
}
