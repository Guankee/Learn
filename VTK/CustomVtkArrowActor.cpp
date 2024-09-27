#include "CustomVtkArrowActor.h"

#include <QtCore/QDebug>
#include <QRegularExpression>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkMinimalStandardRandomSequence.h>

#include <vtkSmartPointer.h>
#include <vtkArrowSource.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>

#include <vtkMatrix4x4.h>
#include <vtkMatrix3x3.h>
#include <vtkQuaternion.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkInformation.h>

void CustomVtkArrowActor::createArrow(double* start, double* end, vtkSmartPointer<vtkPolyData> polyDatain)
{
	arrowStart[0] = start[0];
	arrowStart[1] = start[1];
	arrowStart[2] = start[2];
	arrowEnd[0] = end[0];
	arrowEnd[1] = end[1];
	arrowEnd[2] = end[2];

	// Compute a basis
	double normalizedX[3];
	double normalizedY[3];
	double normalizedZ[3];

	// The X axis is a vector from start to end
	vtkMath::Subtract(end, start, normalizedX);

	vtkMath::Normalize(normalizedX);
	arrowNormal[0] = normalizedX[0];
	arrowNormal[1] = normalizedX[1];
	arrowNormal[2] = normalizedX[2];

	// Compute a basis
	//double normalizedX[3];
	//double normalizedY[3];
	//double normalizedZ[3];

	// The X axis is a vector from start to end
	vtkMath::Subtract(end, start, normalizedZ);
	double length = vtkMath::Norm(normalizedZ);
	vtkMath::Normalize(normalizedZ);
	// The Z axis is an arbitrary vector cross X
	vtkSmartPointer<vtkMinimalStandardRandomSequence> rng = vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
	rng->SetSeed(8775070);
	double arbitrary[3];
	for (auto i = 0; i < 3; ++i)
	{
		rng->Next();
		arbitrary[i] = rng->GetRangeValue(-10, 10);
	}

	vtkMath::Cross(normalizedZ, arbitrary, normalizedX);
	vtkMath::Normalize(normalizedX);

	// The Y axis is Z cross X
	vtkMath::Cross(normalizedZ, normalizedX, normalizedY);

	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	// Create the direction cosine matrix
	matrix->Identity();
	for (auto i = 0; i < 3; i++)
	{
		matrix->SetElement(i, 0, normalizedX[i]);
		matrix->SetElement(i, 1, normalizedY[i]);
		matrix->SetElement(i, 2, normalizedZ[i]);
	}

	// Apply the transforms
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Translate(start);
	transform->Concatenate(matrix);
	//transform->Scale(length, length, length);

	// Transform the polydata
	vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformPD->SetTransform(transform);
	transformPD->SetInputData(polyDatain);
	transformPD->Update();

	//Finish = CustomVtkArrowActor::createArrowPolyData(start, end, true);
	orgPolyData = transformPD->GetOutput();

	transformCurrent = vtkSmartPointer<vtkTransform>::New();
	transformCurrent->Identity();

	vtkSmartPointer<vtkDataSetMapper> arrowMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	arrowMapper->SetInputData(orgPolyData);

	this->SetMapper(arrowMapper);

	this->SetOrigin(start[0], start[1], start[2]);
}


vtkStandardNewMacro(CustomVtkArrowActor);