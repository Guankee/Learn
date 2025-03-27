#include"CutActorBoxWidget.h"

vtkStandardNewMacro(CutActorBoxWidgetCommand);

//void CutActorBoxWidgetComnand::SetActorProperty(vtkSmartPointer<vtkProperty> frontProperty, vtkSmartPointer<vtkProperty> backProperty){
//	actorFrontProperty = frontProperty;
//	actorBackProperty = backProperty;
//}

void CutActorBoxWidgetCommand::Execute(vtkObject* caller, unsigned long eventId, void* calldata)
{
	if (!curActor||!curRenderer){
		return;
	}

	vtkBoxWidgetNT* widget = vtkBoxWidgetNT::SafeDownCast(caller);
	widget->GetPlanes(planes);
	vtkPolyData* data = vtkPolyData::SafeDownCast(curActor->GetMapper()->GetInput());
	clipper->SetInputData(data);
	clipper->SetImplicitFunction(planes);
	clipper->Update();

	if (clipper->GetOutput()->GetNumberOfCells()!=0){
		curRenderer->RemoveActor(curActor);
	}
	curRenderer->GetVTKWindow()->Render();

	selectPolyData = clipper->GetOutput();
	selectMapper->SetInputData(clipper->GetOutput());
	selectActor->SetMapper(selectMapper);

	if (curActor->GetTexture()){
		selectActor->SetTexture(curActor->GetTexture());
		selectActor->SetProperty(curActor->GetProperty());
		selectActor->SetBackfaceProperty(curActor->GetBackfaceProperty());
	}

	curRenderer->ResetCameraClippingRange();
	curRenderer->AddActor(selectActor);
	curRenderer->GetRenderWindow()->Render();
}

void CutActorBoxWidgetCommand::cleanSelectData()
{
	if (selectActor && selectActor->GetMapper()) {
		vtkPolyData* polyData = vtkPolyData::SafeDownCast(selectActor->GetMapper()->GetInput());
		if (polyData) {
			polyData->Initialize();  
			//selectActor->GetMapper()->SetInputData(nullptr);
		}
	}

	if (selectPolyData){
		selectPolyData->Initialize();
	}	
}

vtkStandardNewMacro(CutActorBoxWidget);

void CutActorBoxWidget::sizeHandles()
{
	Q_ASSERT(boxWidget);
	boxWidget->SizeHandles();
}

CutActorBoxWidget::CutActorBoxWidget()
{
	if (!boxWidget){
		boxWidget = vtkSmartPointer<vtkBoxWidgetNT>::New();
	}

	if (!command) {
		command = vtkSmartPointer<CutActorBoxWidgetCommand>::New();
	}

	boxWidget->AddObserver(vtkCommand::InteractionEvent, command);
	boxWidget->SetScalingRangeEnabled(false);

	vtkProperty* pr = boxWidget->GetOutlineProperty();
	vtkProperty* spr = boxWidget->GetSelectedOutlineProperty();
	pr->SetColor(0, 0, 1.0);
	spr->SetColor(1.0, 0, 0);
	pr = boxWidget->GetHandleProperty();
	spr = boxWidget->GetSelectedHandleProperty();
	pr->SetColor(0.3, 1.0, 0);
	pr->SetOpacity(0.6);
	spr->SetColor(1.0, 0, 0);
	spr->SetOpacity(0.6);
}
CutActorBoxWidget::~CutActorBoxWidget() 
{
}
bool CutActorBoxWidget::isHasPolyData(vtkSmartPointer<vtkActor> actor)
{
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
void CutActorBoxWidget::setIneractor(vtkSmartPointer<QVTKInteractor>interactor)
{
	if (!interactor){
		return;
	}

	curInteractor = interactor;
	boxWidget->SetInteractor(curInteractor);
}

void CutActorBoxWidget::setRenderer(vtkSmartPointer<vtkRenderer> render)
{
	command->setRenderer(render);
}

void CutActorBoxWidget::setActor(vtkSmartPointer<vtkActor> actor)
{
	curActor = actor;
	//bool ret = (curActor == command->getCurActor() ? true : false);
	if (curActor!= command->getCurActor()){
		boxWidget->SetProp3D(actor);
		boxWidget->SetPlaceFactor(1.0);
		boxWidget->PlaceWidget();
		command->setActor(actor);
		command->cleanSelectData();
	}
	else{
		if (isHasPolyData(command->getSelectActor()))
		{
			boxWidget->SetProp3D(command->getSelectActor());
			boxWidget->SetPlaceFactor(1.0);
			boxWidget->PlaceWidget();
		}
	}	
}

std::vector<double> CutActorBoxWidget::getCenterPoint()
{
	if (!isHasPolyData(command->getSelectActor())) {
		return {};
	}

	vtkSmartPointer<vtkPolyData>poly = vtkSmartPointer<vtkPolyData>::New();
	boxWidget->GetPolyData(poly);
	vtkPoints* points = poly->GetPoints();
	/*
	获取定义框小部件的polydata（包括点数据）
	该polydata由6个四边形面和15个点组成：
	前8个点定义了八个角顶点
	接下来的6个点依次定义-x、+x、-y、+y、-z、+z六个面的中心点
	最后一个点（第 15 个点） 表示六面体的中心点
	*/
	if (points->GetNumberOfPoints()<15){
		return {};
	}

	double center[3];
	points->GetPoint(14, center);
	return { center[0], center[1], center[2] };
}

void CutActorBoxWidget::on()
{
	Q_ASSERT(boxWidget);
	boxWidget->On();
	sizeHandles();
}

void CutActorBoxWidget::off()
{
	Q_ASSERT(boxWidget);
	boxWidget->Off();
}



