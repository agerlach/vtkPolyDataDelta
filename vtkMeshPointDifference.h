#ifndef __vtkMeshPointDifference_h
#define __vtkMeshPointDifference_h

#include "vtkPolyDataAlgorithm.h"

/*
 * This class colors every point of MeshB based on its distance to the closest point
 * in MeshA.
 */
class vtkMeshPointDifference : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkMeshPointDifference,vtkPolyDataAlgorithm);
  static vtkMeshPointDifference *New();

protected:
  vtkMeshPointDifference();
  ~vtkMeshPointDifference(){}

  int FillInputPortInformation( int port, vtkInformation* info );
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkMeshPointDifference(const vtkMeshPointDifference&);  // Not implemented.
  void operator=(const vtkMeshPointDifference&);  // Not implemented.

};

#endif
