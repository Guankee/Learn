#ifndef CUT_ACTOR_BOX_WIDGET_H
#define CUT_ACTOR_BOX_WIDGET_H

#include "vtkBoxWidgetNT.h"

#include<vtkActor.h>
#include<vtkRenderer.h>
#include<vtkSmartPointer.h>
#include<QVTKInteractor.h>
#include<vtkDataSetMapper.h>
#include<vtkProperty.h>
#include<vtkPolyData.h>
#include<vtkCommand.h>
#include<vtkPlanes.h>
#include<vtkExtractPolyDataGeometry.h>
#include<vtkRenderWindow.h>

class CutActorBoxWidgetCommand :public vtkCommand
{
public:
	static CutActorBoxWidgetCommand* New();
	vtkTypeMacro(CutActorBoxWidgetCommand, vtkCommand);

	CutActorBoxWidgetCommand() = default;
	~CutActorBoxWidgetCommand() = default;
	void Execute(vtkObject* caller, unsigned long eventId, void* calldata) override;
	//void SetActorProperty(vtkSmartPointer<vtkProperty>actorFrontProperty, 
	//	                  vtkSmartPointer<vtkProperty>actorBackProperty);
	void setRenderer(vtkSmartPointer<vtkRenderer>renderer) {
		curRenderer = renderer;
	}
	void setActor(vtkSmartPointer<vtkActor>actor) {
		curActor = actor;
	}
	void cleanSelectData();
	vtkSmartPointer<vtkActor>getSelectActor() { return selectActor; }
	vtkSmartPointer<vtkPolyData> getSelectPolyData() { return selectPolyData; }
	vtkSmartPointer<vtkActor>getCurActor() { return curActor; }
private:
	vtkSmartPointer<vtkActor>curActor = nullptr;
	vtkSmartPointer<vtkRenderer>curRenderer = nullptr;
	vtkSmartPointer<vtkPolyData>selectPolyData = nullptr;
	vtkSmartPointer<vtkActor>selectActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkDataSetMapper>selectMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	vtkSmartPointer<vtkPlanes>planes = vtkSmartPointer<vtkPlanes>::New();
	vtkSmartPointer<vtkExtractPolyDataGeometry>clipper = vtkSmartPointer<vtkExtractPolyDataGeometry>::New();
	//vtkSmartPointer<vtkProperty>actorFrontProperty = nullptr;
	//vtkSmartPointer<vtkProperty>actorBackProperty = nullptr;
};

class CutActorBoxWidget : public vtkObject
{
public:
	static CutActorBoxWidget* New();
	vtkTypeMacro(CutActorBoxWidget, vtkObject);
	void setIneractor(vtkSmartPointer<QVTKInteractor>interactor);
	void setRenderer(vtkSmartPointer<vtkRenderer>render);
	void setActor(vtkSmartPointer<vtkActor>actor);
	/**
	 * 获取当前包围盒子的中心位置.
	 * @return vector<double>{x,y,x}
	 */
	std::vector<double> getCenterPoint();
	/**
	 * 开启包围盒子切割功能.
	 */
	void on();
	/**
	 * 关闭包围盒子切割功能.
	 */
	void off();
	//根据场景调整句柄大小
	void sizeHandles();
	vtkSmartPointer<CutActorBoxWidgetCommand> getCutCommand() { return command; };

	/**
	 * 获取当前裁剪数据.
	 * @return 当前裁剪数据.
	 */
	vtkSmartPointer<vtkPolyData> getSelectPolyData() {
		return command->getSelectPolyData();
	}
	CutActorBoxWidget();
	~CutActorBoxWidget();
protected:
	bool isHasPolyData(vtkSmartPointer<vtkActor> actor);
private: 

	vtkSmartPointer<vtkActor>curActor = nullptr;
	vtkSmartPointer<QVTKInteractor>curInteractor = nullptr;
	vtkSmartPointer<vtkProperty>actorFrontProperty = nullptr;
	vtkSmartPointer<vtkProperty>actorBackProperty = nullptr;
	vtkSmartPointer<vtkBoxWidgetNT>boxWidget = nullptr;
	vtkSmartPointer<CutActorBoxWidgetCommand>command = nullptr;

};
#endif //CUT_ACTOR_BOX_WIDGET_H