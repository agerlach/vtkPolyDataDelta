#include "vtkMeshPointDifference.h"

#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkKdTreePointLocator.h"
#include "vtkMath.h"
#include "vtkModifiedBSPTree.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(vtkMeshPointDifference);

vtkMeshPointDifference::vtkMeshPointDifference()
{
  this->SetNumberOfInputPorts(2);
}

int vtkMeshPointDifference::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // Get the 2 input meshes
  vtkInformation *inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkPolyData *meshA = vtkPolyData::SafeDownCast(
      inInfo0->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformation *inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkPolyData *meshB = vtkPolyData::SafeDownCast(
      inInfo1->Get(vtkDataObject::DATA_OBJECT()));

  // Get the output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *outputMesh = vtkPolyData::SafeDownCast(
               outInfo->Get(vtkDataObject::DATA_OBJECT()));
  outputMesh->ShallowCopy(meshA);

  // Create a KD-tree of meshA
  vtkSmartPointer<vtkKdTreePointLocator> kDTree = 
    vtkSmartPointer<vtkKdTreePointLocator>::New();
  kDTree->SetDataSet(meshA);
  kDTree->BuildLocator();
   
  // Create array to store distances
  vtkSmartPointer<vtkDoubleArray> distArray =
    vtkSmartPointer<vtkDoubleArray>::New();
  distArray->SetNumberOfComponents(1);
  distArray->SetName("Distances");
  distArray->SetNumberOfValues(meshB->GetNumberOfPoints());
 
  // Loop through each point in meshB
  for(int meshBpointID = 0; meshBpointID < meshB->GetNumberOfPoints(); ++meshBpointID)
    {
    double queryPoint[3];
    meshB->GetPoint(meshBpointID, queryPoint);
  
    // Find the closest point in MeshA to the current (query) point in MeshB
    vtkIdType meshApointID = kDTree->FindClosestPoint(queryPoint);

    //Get the coordinates of the closest point
    double closestPoint[3];
    kDTree->GetDataSet()->GetPoint(meshApointID, closestPoint);
    
    double squaredDistance = vtkMath::Distance2BetweenPoints(queryPoint, closestPoint);
    std::cout << "Distance is: " << squaredDistance << std::endl;
    distArray->SetValue(meshBpointID, squaredDistance);   //set the distance in the array
    }

  outputMesh->GetPointData()->SetScalars(distArray);  //set the active scalars of the polydata to the distance array

  return 1;
}

int vtkMeshPointDifference::FillInputPortInformation( int port, vtkInformation* info )
{
  // This filter has two input ports
  if ( port == 0 )
    {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData" );
    return 1;
    }
  else if(port == 1)
    {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData" );
    return 1;
    }

  return 0;
}
