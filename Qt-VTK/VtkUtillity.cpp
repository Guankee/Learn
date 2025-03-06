#include"VtkUtillity.h"

void VtkUtillity::Test(){
  std::cout<<"hello test"<<std::endl;
}

void VtkUtillity::fixPolyData(vtkSmartPointer<vtkPolyData>polyData)
{
	if (!polyData) {
		return;
	}
	std::cout << "Number of points: " << polyData->GetNumberOfPoints() << std::endl;
	std::cout << "Number of cells: " << polyData->GetNumberOfCells() << std::endl;
	vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
	cleaner->SetInputData(polyData);
	cleaner->SetTolerance(1e-6);
	cleaner->Update();

	vtkSmartPointer<vtkPolyData> cleanedPolyData = cleaner->GetOutput();
	cleanedPolyData->BuildLinks();
	polyData->DeepCopy(cleanedPolyData);
	std::cout << "PolyData cleaned and fixed!" << std::endl;
	std::cout << "Number of points: " << polyData->GetNumberOfPoints() << std::endl;
	std::cout << "Number of cells: " << polyData->GetNumberOfCells() << std::endl;
}

void VtkUtillity::booleanTest()
{
	// ???????????????
	vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource->SetXLength(5);
	cubeSource->SetYLength(5);
	cubeSource->SetZLength(5);
	cubeSource->Update();

	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(3);
	sphereSource->SetCenter(2, 2, 2);
	sphereSource->Update();
	// ??§Ó?????????????????????

	vtkNew<vtkTriangleFilter> filter1;
	filter1->SetInputData(cubeSource->GetOutput());
	filter1->Update();
	vtkNew<vtkTriangleFilter> filter2;
	filter2->SetInputData(sphereSource->GetOutput());
	filter2->Update();

	vtkSmartPointer<vtkBooleanOperationPolyDataFilter> booleanFilter = vtkSmartPointer<vtkBooleanOperationPolyDataFilter>::New();
	booleanFilter->SetInputData(0, filter1->GetOutput());
	booleanFilter->SetInputData(1, filter2->GetOutput());
	booleanFilter->SetOperationToIntersection();
	booleanFilter->Update();

	// ??????
	vtkSmartPointer<vtkPolyData> resultPolyData = booleanFilter->GetOutput();

	// ??????????????
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(resultPolyData);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // ???¨²??

	// ??????
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkNew<vtkPolyDataMapper> cubmapper;
	cubmapper->SetInputData(filter1->GetOutput());
	vtkNew<vtkActor>cubActor;
	cubActor->SetMapper(cubmapper);
	cubActor->GetProperty()->SetColor(1.0, 1.0, 0);

	vtkNew<vtkPolyDataMapper> sheremapper;
	sheremapper->SetInputData(filter2->GetOutput());
	vtkNew<vtkActor>spActor;
	spActor->SetMapper(sheremapper);
	spActor->GetProperty()->SetColor(0.0, 1.0, 0);


	renderer->AddActor(actor);
	//renderer->AddActor(cubActor);
	//renderer->AddActor(spActor);
	renderer->SetBackground(0.1, 0.1, 0.1); // ????????
	renderWindow->Render();
	renderWindowInteractor->Start();
}



