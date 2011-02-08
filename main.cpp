//#include <QtCore/QCoreApplication>
#include <QtDebug>
//#include <QVector>

#include <vtkSphereSource.h>
#include <vtkModifiedBSPTree.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkDataArray.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPLYReader.h>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkPointData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkGlyph3D.h>
#include <vtkProperty.h>
#include <vtkCellLocator.h>




int main(int argc, char *argv[])
{
    //Calc dist of B relative to A
    //QCoreApplication a(argc, argv);

    //settings
    double SEARCHLENGTH = 15;   // half length of search line
    double TOL = 0.001;           // bspTree tolerance
    char *name = "dist";
    bool DISPLAYNORMALS = false;
    bool USESIMULATEDMESHES = false;
    bool USECLOSESTPOINT = true;
    char *scan1name = "banana4.ply";
    char *scan2name = "banana3.ply";

    // Create source meshes
    vtkSmartPointer<vtkPolyData> polyA =
            vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> polyB =
            vtkSmartPointer<vtkPolyData>::New();
    if(USESIMULATEDMESHES)
    {
        double res = 100.;
        vtkSmartPointer<vtkSphereSource> meshA =
                vtkSmartPointer<vtkSphereSource>::New();
        meshA->SetRadius(10.);
        meshA->SetCenter(0.,0.,0.);
        meshA->SetThetaResolution(res);
        meshA->SetPhiResolution(res);
        meshA->Update();
        polyA = meshA->GetOutput();

        vtkSmartPointer<vtkSphereSource> meshB =
                vtkSmartPointer<vtkSphereSource>::New();
        meshB->SetRadius(11.);
        meshB->SetCenter(1.,0.,0.);
        meshB->SetThetaResolution(res);
        meshB->SetPhiResolution(res);
        meshB->Update();
        polyB = meshB->GetOutput();
    }
    else
    {
        vtkSmartPointer<vtkPLYReader> reader =
                vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName(scan1name);
        reader->Update();

        polyA->DeepCopy(reader->GetOutput());
        polyA->Update();

        reader->SetFileName(scan2name);
        reader->Update();

        polyB->DeepCopy(reader->GetOutput());
        polyB->Update();

    }


    // Calculate BSP tree
    vtkSmartPointer<vtkModifiedBSPTree> bspTree =
            vtkSmartPointer<vtkModifiedBSPTree>::New();
    bspTree->SetDataSet(polyB);
    bspTree->BuildLocator();


    vtkSmartPointer<vtkCellLocator> cellLoc =
            vtkSmartPointer<vtkCellLocator>::New();
    cellLoc->SetDataSet(polyB);
    cellLoc->BuildLocator();


    // Calculate meshA normals
    vtkSmartPointer<vtkPolyDataNormals> normals =
            vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInput(polyA);
    normals->ComputeCellNormalsOff();
    normals->ComputeCellNormalsOn();
    //normals->SplittingOff();
    normals->Update();
    vtkSmartPointer<vtkDataArray> normalData = normals->GetOutput()->GetPointData()->GetNormals();


    /*! Diff Calculation */
    //------------------------------------------------------------------------------------------------
    double normal[3];       //stores normal extracted for a single point
    double normalLength;    //length of the normal, used to ensure that it is normalized
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

    //QVector<double> *distVect = new QVector<double>();
    vtkIdType a;

    // create array to store distances
    vtkSmartPointer<vtkDoubleArray> distArray =
            vtkSmartPointer<vtkDoubleArray>::New();
    distArray->SetNumberOfComponents(1);
    distArray->SetName(name);
    distArray->SetNumberOfValues(polyA->GetNumberOfPoints());


    // loop throug each point in meshA
    for(int ii = 0; ii < polyA->GetNumberOfPoints(); ii ++)
    {

        if(int(float(ii)/float(polyA->GetNumberOfPoints())*100.) % 10 == 0)
            qDebug() << ii;
        normalData->GetTuple(ii, normal);       //get normal at point
        polyA->GetPoint(ii, refPt);             //get point location


        if(!USECLOSESTPOINT)
        {
            // Create end points for lines about the +'ve and -'ve direction of the point normal
            normalLength = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
            vectPtN[0] = refPt[0] - SEARCHLENGTH * normal[0]/normalLength;
            vectPtN[1] = refPt[1] - SEARCHLENGTH * normal[1]/normalLength;
            vectPtN[2] = refPt[2] - SEARCHLENGTH * normal[2]/normalLength;

            vectPtP[0] = refPt[0] + SEARCHLENGTH * normal[0]/normalLength;
            vectPtP[1] = refPt[1] + SEARCHLENGTH * normal[1]/normalLength;
            vectPtP[2] = refPt[2] + SEARCHLENGTH * normal[2]/normalLength;

            // Check the intersection in the +'ve and -'ve directions: these must be seperated since bspTree returns the first intersection when
            // traversing the line segment, which may not be the closest cell. This uses to line segments, both traversed from the point of interest.
            // this then resutls in the closest intersection in both the +'ve and -'ve directions
            intersectN = bspTree->IntersectWithLine(refPt, vectPtN, TOL, tN, x, pcoords, subId);
            intersectP = bspTree->IntersectWithLine(refPt, vectPtP,TOL, tP, x, pcoords, subId);

            // Logic to check for the closest intersection
            if(intersectN == 0 && intersectP == 0)
                dist = 0;                   //case of no intersection, I don't know how to handle this value should really be NaN
            else if(intersectN == 0 && intersectP != 0)
                dist = tP*SEARCHLENGTH;     //case of only an intersection in the +'ve direction
            else if(intersectP == 0 && intersectN !=0 )
                dist = -tN*SEARCHLENGTH;    //case of only an intersection in the -'ve direction
            else                            //case of an intersection in both directions
            {
                if(tN <= tP)                //find the closest interseection
                    dist = -tN*SEARCHLENGTH;
                else
                    dist = tP*SEARCHLENGTH;
            }


        }
        else
        {
            vtkMath::Normalize(normal);
            vectPtP[0] = refPt[0] + normal[0]/normalLength;
            vectPtP[1] = refPt[1] + normal[1]/normalLength;
            vectPtP[2] = refPt[2] + normal[2]/normalLength;
            cellLoc->FindClosestPoint(refPt,x,a,subId,dist);
            if(vtkMath::Dot(vectPtP,x) >= 0)
                dist = sqrt(dist);
            else
                dist = -sqrt(dist);
        }

        // store the min and max values
        if(dist > max)
            max = dist;
        else if (dist < min)
            min = dist;

        distArray->SetValue(ii,dist);   //set the distance to the array
        //distVect->append(dist);


    }


//    qDebug() << *distVect;
//    qDebug() << "(min, max) = " << min << max;
//    qDebug() << "T's = " << tN << tP;
//    qDebug() << "Inter = " << intersectN << intersectP;

    polyA->GetPointData()->AddArray(distArray);     //add the array to the polydata
    polyA->GetPointData()->SetActiveScalars(name);  //set the active scalars of the polydata to the distance array
    polyA->Update();


    // create lookup table
    vtkSmartPointer<vtkLookupTable> lookupTable =
            vtkSmartPointer<vtkLookupTable>::New();
    lookupTable->SetRange(min,max);
    lookupTable->SetNumberOfColors(1000);
    lookupTable->Build();

    // create scalarbar
    vtkSmartPointer<vtkScalarBarActor> scalarBar =
            vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(lookupTable);
    scalarBar->SetMaximumNumberOfColors(1000);
    scalarBar->SetTitle("Mesh Distance");



    // Create normal glyph
    vtkSmartPointer<vtkGlyph3D> glyph =
            vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInput(normals->GetOutput());
    glyph->SetVectorModeToUseNormal();
    glyph->OrientOn();


    // Set to mappers
    vtkSmartPointer<vtkPolyDataMapper> mapperA =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperA->SetInput(polyA);
    mapperA->SetScalarRange(min,max);
    mapperA->SetLookupTable(lookupTable);

    vtkSmartPointer<vtkPolyDataMapper> mapperB =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperB->SetInput(polyB);

    vtkSmartPointer<vtkPolyDataMapper> normalMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    normalMapper->SetInput(glyph->GetOutput());

    // Set to Actors
    vtkSmartPointer<vtkActor> actorA =
            vtkSmartPointer<vtkActor>::New();
    actorA->SetMapper(mapperA);
    //actorA->GetProperty()->LightingOff();;

    vtkSmartPointer<vtkActor> actorB =
            vtkSmartPointer<vtkActor>::New();
    actorB->SetMapper(mapperB);
    actorB->GetProperty()->SetOpacity(.3);

    vtkSmartPointer<vtkActor> normalActor =
            vtkSmartPointer<vtkActor>::New();
    normalActor->SetMapper(normalMapper);
    normalActor->GetProperty()->SetOpacity(.8);
    normalActor->GetProperty()->SetColor(0.,.2,0.);

    // Set renderer
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actorA);
    if(DISPLAYNORMALS)
        renderer->AddActor(normalActor);
    //renderer->AddActor(actorB);
    renderer->AddActor2D(scalarBar);


    // Set render window
    vtkSmartPointer<vtkRenderWindow> renWin =
            vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);

    // set render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    interactor->SetRenderWindow(renWin);
    interactor->SetInteractorStyle(style);
    interactor->Initialize();
    interactor->Start();


















   // return a.exec();
}
