#include "vtkMeshPointDifference.h"

#include <vtkActor.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int argc, char *argv[])
{
  // Create source meshes
  int resolution = 50;
  vtkSmartPointer<vtkSphereSource> meshSourceA =
    vtkSmartPointer<vtkSphereSource>::New();
  meshSourceA->SetRadius(10.);
  meshSourceA->SetCenter(0.,0.,0.);
  meshSourceA->SetThetaResolution(resolution);
  meshSourceA->SetPhiResolution(resolution);
  meshSourceA->Update();

  vtkSmartPointer<vtkSphereSource> meshSourceB =
    vtkSmartPointer<vtkSphereSource>::New();
  meshSourceB->SetRadius(11.);
  meshSourceB->SetCenter(1.,0.,0.);
  meshSourceB->SetThetaResolution(resolution);
  meshSourceB->SetPhiResolution(resolution);
  meshSourceB->Update();

  vtkSmartPointer<vtkMeshPointDifference> meshPointDifference =
    vtkSmartPointer<vtkMeshPointDifference>::New();
  meshPointDifference->SetInputConnection(0, meshSourceA->GetOutputPort());
  meshPointDifference->SetInputConnection(1, meshSourceB->GetOutputPort());
  meshPointDifference->Update();

  // Create lookup table
  vtkSmartPointer<vtkLookupTable> lookupTable =
    vtkSmartPointer<vtkLookupTable>::New();
  double range[2];
  vtkDoubleArray::SafeDownCast(meshPointDifference->GetOutput()->GetPointData()->GetArray("Distances"))->GetRange(range);
  std::cout << "Range: " << range[0] << " " << range[1] << std::endl;
  lookupTable->SetRange(range);
  lookupTable->Build();

  // Since we colored mesh B, we should display mesh A and the output of the filter (the colored mesh B)

  // Create MeshA
  vtkSmartPointer<vtkPolyDataMapper> mapperA =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperA->SetInputConnection(meshSourceA->GetOutputPort());

  vtkSmartPointer<vtkActor> actorA =
    vtkSmartPointer<vtkActor>::New();
  actorA->SetMapper(mapperA);
  actorA->GetProperty()->SetOpacity(.3);

  // Create MeshB
  // Create scalarbar
  vtkSmartPointer<vtkScalarBarActor> scalarBar =
    vtkSmartPointer<vtkScalarBarActor>::New();
  scalarBar->SetLookupTable(lookupTable);
  scalarBar->SetTitle("Mesh Distance");
  
  vtkSmartPointer<vtkPolyDataMapper> mapperB =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperB->SetInputConnection(meshPointDifference->GetOutputPort());
  mapperB->SetScalarRange(range);
  mapperB->SetLookupTable(lookupTable);
  
  vtkSmartPointer<vtkActor> actorB =
    vtkSmartPointer<vtkActor>::New();
  actorB->SetMapper(mapperB);

  // Visualize
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actorA);
  renderer->AddActor(actorB);
  renderer->AddActor2D(scalarBar);

  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  interactor->SetRenderWindow(renderWindow);
  interactor->SetInteractorStyle(style);
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
