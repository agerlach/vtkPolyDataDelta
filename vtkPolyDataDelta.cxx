#include "vtkPolyDataDelta.h"

#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkModifiedBSPTree.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(vtkPolyDataDelta);

vtkPolyDataDelta::vtkPolyDataDelta()
{
  this->SetNumberOfInputPorts(2);
}

int vtkPolyDataDelta::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the 2 input meshes
  vtkInformation *inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkPolyData *meshA = vtkPolyData::SafeDownCast(
      inInfo0->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformation *inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkPolyData *meshB = vtkPolyData::SafeDownCast(
      inInfo1->Get(vtkDataObject::DATA_OBJECT()));

  // get the output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *outputMesh = vtkPolyData::SafeDownCast(
		  outInfo->Get(vtkDataObject::DATA_OBJECT()));
  outputMesh->ShallowCopy(meshA);

  // Calculate BSP tree
  vtkSmartPointer<vtkModifiedBSPTree> bspTree =
    vtkSmartPointer<vtkModifiedBSPTree>::New();
  bspTree->SetDataSet(meshB);
  bspTree->BuildLocator();

  // Calculate meshA normals
  vtkSmartPointer<vtkPolyDataNormals> normals =
    vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInput(meshA);
  normals->ComputeCellNormalsOff();
  normals->ComputeCellNormalsOn();
  normals->SplittingOff();
  normals->Update();
  vtkSmartPointer<vtkDataArray> normalData = normals->GetOutput()->GetPointData()->GetNormals();


  /*! Diff Calculation */
  //------------------------------------------------------------------------------------------------
  double normal[3];       //stores normal extracted for a single point
  double refPt[3];        //stores the current point on meshA
  double vectPtN[3];      //end point of search line in -'ve normal direction
  double vectPtP[3];      //end point of search line in +'ve normal direction
  double x[3];            //intersection point on cell
  double tP;              //length along +'ve search line
  double tN;              //length along -'ve search line
  double pcoords[3];
  int subId;
  int intersectP;         //check if intersection occured
  int intersectN;         //check if intersection occured
  double dist;            //calculated distance for the current point on meshA to meshB
  double max = -1.e22;    //store max distance for scalarbar
  double min = 1.e22;     //store min distance for scalarbar
  double SEARCHLENGTH = 100.;   // half length of search line
  double TOL = 0.001;           // bspTree tolerance

  // Create array to store distances
  vtkSmartPointer<vtkDoubleArray> distArray =
    vtkSmartPointer<vtkDoubleArray>::New();
  distArray->SetNumberOfComponents(1);
  distArray->SetName("Distances");
  distArray->SetNumberOfValues(meshA->GetNumberOfPoints());

  // Loop through each point in meshA
  for(int ii = 0; ii < meshA->GetNumberOfPoints(); ii ++)
    {
    normalData->GetTuple(ii, normal);       //get normal at point
    meshA->GetPoint(ii, refPt);             //get point location

    // Create end points for lines about the +'ve and -'ve direction of the point normal
    vectPtN[0] = refPt[0] - SEARCHLENGTH * normal[0];
    vectPtN[1] = refPt[1] - SEARCHLENGTH * normal[1];
    vectPtN[2] = refPt[2] - SEARCHLENGTH * normal[2];

    vectPtP[0] = refPt[0] + SEARCHLENGTH * normal[0];
    vectPtP[1] = refPt[1] + SEARCHLENGTH * normal[1];
    vectPtP[2] = refPt[2] + SEARCHLENGTH * normal[2];

    // Check the intersection in the +'ve and -'ve directions: these must be seperated since bspTree returns the first intersection when
    // traversing the line segment, which may not be the closest cell. This uses to line segments, both traversed from the point of interest.
    // this then resutls in the closest intersection in both the +'ve and -'ve directions
    intersectN = bspTree->IntersectWithLine(refPt, vectPtN, TOL, tN, x, pcoords, subId);
    intersectP = bspTree->IntersectWithLine(refPt, vectPtP,TOL, tP, x, pcoords, subId);

    // Logic to check for the closest intersection
    if(intersectN == 0 && intersectP == 0)
      {
      dist = 0;                   //case of no intersection, I don't know how to handle this value should really be NaN
      }
    else if(intersectN == 0 && intersectP != 0)
      {
      dist = tP*SEARCHLENGTH;     //case of only an intersection in the +'ve direction
      }
    else if(intersectP == 0 && intersectN !=0 )
      {
      dist = -tN*SEARCHLENGTH;    //case of only an intersection in the -'ve direction
      }
    else                            //case of an intersection in both directions
      {
      if(tN <= tP)                //find the closest interseection
        {
        dist = -tN*SEARCHLENGTH;
        }
      else
        {
        dist = tP*SEARCHLENGTH;
        }
      }

    // store the min and max values
    if(dist > max)
      {
      max = dist;
      }
    else if (dist < min)
      {
      min = dist;
      }

    distArray->SetValue(ii,dist);   //set the distance to the array
    }

  outputMesh->GetPointData()->SetScalars(distArray);  //set the active scalars of the polydata to the distance array

  return 1;
}

int vtkPolyDataDelta::FillInputPortInformation( int port, vtkInformation* info )
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
