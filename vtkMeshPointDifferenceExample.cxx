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
#include <vtkXMLPolyDataReader.h>

int main(int argc, char *argv[])
{
  // Verify arguments
  if(argc < 3)
    {
    std::cerr << "Required arguments: mesh1.vtp mesh2.vtp" << std::endl;
    return EXIT_SUCCESS;
    }

  // Parse arguments
  std::string mesh1FileName = argv[1];
  std::string mesh2FileName = argv[2];
  
  // Output arguments
  std::cout << "Mesh1: " << mesh1FileName << std::endl;
  std::cout << "Mesh2: " << mesh2FileName << std::endl;
  
  // Read the files
  vtkSmartPointer<vtkXMLPolyDataReader> reader1 =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader1->SetFileName(mesh1FileName.c_str());
  reader1->Update();
  
  vtkSmartPointer<vtkXMLPolyDataReader> reader2 =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader2->SetFileName(mesh1FileName.c_str());
  reader2->Update();
  
  // Compare the meshes
  vtkSmartPointer<vtkMeshPointDifference> meshPointDifference =
    vtkSmartPointer<vtkMeshPointDifference>::New();
  meshPointDifference->SetInputConnection(0, reader1->GetOutputPort());
  meshPointDifference->SetInputConnection(1, reader2->GetOutputPort());
  meshPointDifference->Update();

  // Create lookup table
  vtkSmartPointer<vtkLookupTable> lookupTable =
    vtkSmartPointer<vtkLookupTable>::New();
  double range[2];
  vtkDoubleArray::SafeDownCast(meshPointDifference->GetOutput()->GetPointData()->GetArray("Distances"))->GetRange(range);
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
  glyph->SetInput(meshPointDifference->GetOutput());
  glyph->SetVectorModeToUseNormal();
  glyph->OrientOn();

  // Set to mappers
  vtkSmartPointer<vtkPolyDataMapper> mapperA =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperA->SetInput(meshPointDifference->GetOutput());
  mapperA->SetScalarRange(range);
  mapperA->SetLookupTable(lookupTable);

  vtkSmartPointer<vtkPolyDataMapper> mapperB =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperB->SetInputConnection(reader2->GetOutputPort());

  vtkSmartPointer<vtkPolyDataMapper> normalMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  normalMapper->SetInputConnection(glyph->GetOutputPort());

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
