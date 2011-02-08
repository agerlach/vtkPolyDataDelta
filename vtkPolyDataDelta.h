#ifndef __vtkPolyDataDelta_h
#define __vtkPolyDataDelta_h

#include "vtkPolyDataAlgorithm.h"

class vtkPolyDataDelta : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkPolyDataDelta,vtkPolyDataAlgorithm);
  static vtkPolyDataDelta *New();

protected:
  vtkPolyDataDelta();
  ~vtkPolyDataDelta(){}

  int FillInputPortInformation( int port, vtkInformation* info );
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkPolyDataDelta(const vtkPolyDataDelta&);  // Not implemented.
  void operator=(const vtkPolyDataDelta&);  // Not implemented.

};

#endif
