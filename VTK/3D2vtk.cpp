#include <open3d/Open3D.h>
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>

#include <iostream>
int main() {
	std::string file_path = "D:\\textures\\texturemeshwithopt.obj";

	std::shared_ptr<open3d::geometry::TriangleMesh> mesh =
		std::make_shared<open3d::geometry::TriangleMesh>();

	open3d::io::ReadTriangleMeshOptions read;
	open3d::io::ReadTriangleMeshFromOBJ(file_path, *mesh, read);
	// open3d::visualization::DrawGeometries({mesh}, "Registration result", 1028,
	// 720,
	//                                       50, 50, false, false, true);
	int num_materials = (int)mesh->textures_.size();
	//  std::cout<<num_materials<<std::endl;
	std::vector<std::vector<Eigen::Vector3d>> points;
	std::vector<std::vector<Eigen::Vector2d>> uvs;
	points.resize(num_materials);
	uvs.resize(num_materials);
	for (size_t i = 0; i < mesh->triangles_.size(); i++) {
		// triangle 里面存的是vertices_的绝对索引啊
		const auto& triangle = mesh->triangles_[i];
		int mi = mesh->triangle_material_ids_[i];

		for (size_t j = 0; j < 3; j++) {
			size_t idx = i * 3 + j;
			size_t vi = triangle(j);
			// points[mi].push_back(mesh->vertices_[vi].cast<float>());
			points[mi].push_back(mesh->vertices_[vi]);
			// triangle_uvs_与vertices_本来就是对应上的
			// 但是这不是有纹理要给分开成成快吗
			// 顶点的位置确定了就可以直接来一个uv的绝对索引
			uvs[mi].push_back(mesh->triangle_uvs_[idx]);
		}
	}

	//std::cout << points.size() << std::endl;
	//std::cout << uvs.size() << std::endl;
	//for (int i = 0; i < num_materials; ++i) {
	//  std::cout << points[i].size() << std::endl;
	//  std::cout << uvs[i].size() << std::endl;
	//}
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	for (int i = 0; i < num_materials; ++i) {
		vtkSmartPointer<vtkPoints> mesh_point = vtkSmartPointer<vtkPoints>::New();
		for (int j = 0; j < points[i].size(); ++j) {
			mesh_point->InsertNextPoint(points[i][j].x(), points[i][j].y(),
				points[i][j].z());
		}
		std::cout << "points  " << points[i].size() << std::endl;
		std::cout << "mesh_point " << mesh_point->GetNumberOfPoints() << std::endl;
		vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
		for (int j = 0; j < points[i].size() / 3; j++) {
			//int num = 0;
			vtkSmartPointer<vtkTriangle> trianle =
				vtkSmartPointer<vtkTriangle>::New();
			trianle->GetPointIds()->SetId(0, 3 * j);
			trianle->GetPointIds()->SetId(1, 3 * j + 1);
			trianle->GetPointIds()->SetId(2, 3 * j + 2);
			cells->InsertNextCell(trianle);
			//num++;
		}
		std::cout << "trianle: " << cells->GetNumberOfCells() << std::endl;
		vtkSmartPointer<vtkPolyData> polygonPolyData =
			vtkSmartPointer<vtkPolyData>::New();
		polygonPolyData->SetPoints(mesh_point);
		polygonPolyData->SetPolys(cells);
		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		// mapper->SetInput(polygonPolyData);
		mapper->SetInputData(polygonPolyData);
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		renderer->AddActor(actor);
	}
	renderer->SetBackground(0.5, 0.5, 0.5);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(640, 480);
	renderWindow->Render();
	renderWindow->SetWindowName("PolyDataNew");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindow->Render();
	renderWindowInteractor->Start();

	return 0;
}