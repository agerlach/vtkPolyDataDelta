#include "vtkPolyDataDelta.h"

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

  vtkSmartPointer<vtkPolyDataDelta> polyDataDelta =
    vtkSmartPointer<vtkPolyDataDelta>::New();
  polyDataDelta->SetInputConnection(0, meshSourceA->GetOutputPort());
  polyDataDelta->SetInputConnection(1, meshSourceB->GetOutputPort());
  polyDataDelta->Update();

  // Create lookup table
  vtkSmartPointer<vtkLookupTable> lookupTable =
    vtkSmartPointer<vtkLookupTable>::New();
  double range[2];
  vtkDoubleArray::SafeDownCast(polyDataDelta->GetOutput()->GetPointData()->GetArray("Distances"))->GetRange(range);
  std::cout << "Range: " << range[0] << " " << range[1] << std::endl;
  lookupTable->SetRange(range);
  lookupTable->Build();

  // Create scalarbar
  vtkSmartPointer<vtkScalarBarActor> scalarBar =
    vtkSmartPointer<vtkScalarBarActor>::New();
  scalarBar->SetLookupTable(lookupTable);
  scalarBar->SetTitle("Mesh Distance");

  // Create normal glyph
  vtkSmartPointer<vtkGlyph3D> glyph =
    vtkSmartPointer<vtkGlyph3D>::New();
  glyph->SetInputConnection(polyDataDelta->GetOutputPort());
  glyph->SetVectorModeToUseNormal();
  glyph->OrientOn();

  // Set to mappers
  vtkSmartPointer<vtkPolyDataMapper> mapperA =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperA->SetInputConnection(polyDataDelta->GetOutputPort());
  mapperA->SetScalarRange(range);
  mapperA->SetLookupTable(lookupTable);

  vtkSmartPointer<vtkPolyDataMapper> mapperB =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperB->SetInput(meshSourceB->GetOutput());

  vtkSmartPointer<vtkPolyDataMapper> normalMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  normalMapper->SetInput(glyph->GetOutput());

  // Set to Actors
  vtkSmartPointer<vtkActor> actorA =
    vtkSmartPointer<vtkActor>::New();
  actorA->SetMapper(mapperA);

  vtkSmartPointer<vtkActor> actorB =
    vtkSmartPointer<vtkActor>::New();
  actorB->SetMapper(mapperB);
  actorB->GetProperty()->SetOpacity(.3);

  vtkSmartPointer<vtkActor> normalActor =
    vtkSmartPointer<vtkActor>::New();
  normalActor->SetMapper(normalMapper);
  normalActor->GetProperty()->SetOpacity(.8);
  normalActor->GetProperty()->SetColor(0.,.2,0.);

  // Visualize
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actorA);

  bool DISPLAYNORMALS = false;

  if(DISPLAYNORMALS)
    {
    renderer->AddActor(normalActor);
    }
  renderer->AddActor(actorB);
  renderer->AddActor2D(scalarBar);

  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  interactor->SetRenderWindow(renWin);
  interactor->SetInteractorStyle(style);
  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
