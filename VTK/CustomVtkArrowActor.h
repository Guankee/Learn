#ifndef CUSTOM_VTK_ARROW_H
#define CUSTOM_VTK_ARROW_H

#include <QObject>
#include <vtkPropPicker.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderingCoreModule.h>
#include <vtkProperty.h>
#include <vtkTexture.h>
#include <QtCore/QString>

class CustomVtkArrowActor : public vtkActor
{
	///////////overwrite vtk fuction for render////////////////
public:
	vtkTypeMacro(CustomVtkArrowActor, vtkActor);
	static CustomVtkArrowActor* New();

	virtual void ReleaseGraphicsResources(vtkWindow *window) {
		this->Device->ReleaseGraphicsResources(window);
		this->Superclass::ReleaseGraphicsResources(window);
	}

	virtual int RenderOpaqueGeometry(vtkViewport *viewport) {
		if (!this->Mapper) {
			return 0;
		}
		if (!this->Property) {
			this->GetProperty();
		}
		if (this->GetIsOpaque()) {
			vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
			this->Render(ren);
			return 1;
		}
		return 0;
	}

	virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport) {
		if (!this->Mapper) {
			return 0;
		}
		if (!this->Property) {
			this->GetProperty();
		}
		if (!this->GetIsOpaque()) {
			vtkRenderer *ren = static_cast<vtkRenderer *>(viewport);
			this->Render(ren);
			return 1;
		}
		return 0;
	}

	virtual void Render(vtkRenderer *ren) {
		this->Property->Render(this, ren);
		this->Device->SetProperty(this->Property);
		this->Property->Render(this, ren);
		if (this->BackfaceProperty) {
			this->BackfaceProperty->BackfaceRender(this, ren);
			this->Device->SetBackfaceProperty(this->BackfaceProperty);
		}
		if (this->Texture) {
			this->Texture->Render(ren);
		}
		this->ComputeMatrix();
		this->Device->SetUserMatrix(this->Matrix);
		this->Device->Render(ren, this->Mapper);
	}

	void ShallowCopy(vtkProp *prop) {
		CustomVtkArrowActor *f = CustomVtkArrowActor::SafeDownCast(prop);
		this->vtkActor::ShallowCopy(prop);
	}

protected:
	vtkActor* Device;

	CustomVtkArrowActor() {
		this->Device = vtkActor::New();
	}

	~CustomVtkArrowActor() {
		this->Device->Delete();
	}
	///////////overwrite vtk fuction for render////////////////

	//---------------custom code----------------------------------//
public:

	void createArrow(double* start, double* end, vtkSmartPointer<vtkPolyData> polyData);

	void setArrrowState(bool finish) { isFinished = finish; };
	bool getArrrowState() { return isFinished; };
	double* getArrrowNormal() { return arrowNormal; };

	double* getArrrowStart() { return arrowStart; };
	double* getArrrowEnd() { return arrowEnd; };

	vtkSmartPointer<vtkTransform> getCurrentTransform() { return transformCurrent; };
	vtkSmartPointer<vtkPolyData> getOrgPolyData() { return orgPolyData; };


private:
	vtkSmartPointer<vtkTransform> transformCurrent;
	vtkSmartPointer<vtkPolyData> orgPolyData;

	bool isFinished = false;
	double arrowNormal[3] = {0, 0, 0};

	double arrowStart[3] = { 0, 0, 0 };
	double arrowEnd[3] = { 0, 0, 0 };
	//---------------custom code----------------------------------//
};

#endif		//CUSTOM_VTK_ARROW_H