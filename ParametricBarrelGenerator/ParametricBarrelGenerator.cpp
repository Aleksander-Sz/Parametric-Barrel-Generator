#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <STEPControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

#include <cmath>
#include <Precision.hxx>

#include <iostream>

#define BACK_MOUNT_LENGHT 15.0

int main()
{
    double riflingLenght;
    int caliber;

    std::cout << "Welcome to the parametric barrel generation system.\n";
    std::cout << "The available calibers are:\n1. 5.56 x 45 mm NATO\n2. 7.62 x 51 mm NATO\n3. 12.7 x 99 mm NATO\n";
    std::cout << "Input the desired caliber (1, 2 or 3): ";
    std::cin >> caliber;
    if (caliber < 1 || caliber>3)
    {
        std::cout << "Error, you entered an incorrect value!\n";
        return 0;
    }

    double muzzleBarrelR1 = 0.0, muzzleBarrelR2 = 0.0, muzzleTaperLenght = 0.0;
    double chamberMainConeR1 = 0.0, chamberMainConeR2 = 0.0, chamberMainConeLenght = 0.0, chamberTaperR2 = 0.0, chamberTaperLenght = 0.0, chamberFirstCylinderLenght = 0.0, chamberSecondCylinderR = 0.0, chamberSecondCylinderLenght = 0.0;
    double riflingOuterRadius = 0.0;
    
    double minRiflingLenght = 50.0;

    //temporarily only 5.56:
    switch (caliber)
    {
    case 1: // 5.56
        muzzleBarrelR1 = 12.5;
        muzzleBarrelR2 = 8.5;

        chamberMainConeR1 = 4.825;
        chamberMainConeR2 = 4.5;
        chamberMainConeLenght = 36.5;
        chamberTaperR2 = 3.25;
        chamberTaperLenght = 2.9448;
        chamberFirstCylinderLenght = 44.96 - chamberMainConeLenght - chamberTaperLenght;
        chamberSecondCylinderR = 2.845;
        chamberSecondCylinderLenght = 1.45;

        riflingOuterRadius = 2.845;
        break;
    case 2: // 7.62
        muzzleBarrelR1 = 15.0;
        muzzleBarrelR2 = 10.0;

        chamberMainConeR1 = 6.0;
        chamberMainConeR2 = 5.765;
        chamberMainConeLenght = 43.5;
        chamberTaperR2 = 4.36;
        chamberTaperLenght = 3.86;
        chamberFirstCylinderLenght = 51.18 - chamberMainConeLenght - chamberTaperLenght;
        chamberSecondCylinderR = 3.925;
        chamberSecondCylinderLenght = 3.0;
        
        riflingOuterRadius = 3.96;
        break;
    case 3: // 12.7
        muzzleBarrelR1 = 25.0;
        muzzleBarrelR2 = 15.0;

        chamberMainConeR1 = 10.21;
        chamberMainConeR2 = 9.07;
        chamberMainConeLenght = 76.34;
        chamberTaperR2 = 7.11;
        chamberTaperLenght = 6.96;
        chamberFirstCylinderLenght = 99.31 - chamberMainConeLenght - chamberTaperLenght;
        chamberSecondCylinderR = 6.98;
        chamberSecondCylinderLenght = 4.0;

        riflingOuterRadius = 7.05;
        
        minRiflingLenght = 50.0;
        break;
    }

    std::cout << "Input the desired rifling lenght in milimeters (" << minRiflingLenght << ".0 - 1500.0): ";
    std::cin >> riflingLenght;
    if (riflingLenght < minRiflingLenght || riflingLenght > 1500.0)
    {
        std::cout << "Error, you entered an incorrect value!\n";
        return 0;
    }

    muzzleTaperLenght = riflingLenght - 15.0 + chamberMainConeLenght + chamberTaperLenght + chamberFirstCylinderLenght + chamberSecondCylinderLenght;
    
    std::string filename;
    std::cout << "File name (no extension): ";
    std::cin >> filename;

    TopoDS_Shape muzzle =
        BRepPrimAPI_MakeCone(muzzleBarrelR1, muzzleBarrelR2, muzzleTaperLenght).Shape();

    gp_Trsf move;
    move.SetTranslation(gp_Vec(0, 0, BACK_MOUNT_LENGHT));

    muzzle = BRepBuilderAPI_Transform(muzzle, move).Shape();

    TopoDS_Shape backMount =
        BRepPrimAPI_MakeCylinder(muzzleBarrelR1 - 1.5, BACK_MOUNT_LENGHT).Shape();

    TopoDS_Shape barrel =
        BRepAlgoAPI_Fuse(muzzle, backMount).Shape();

    // Creating the chamber:

    TopoDS_Shape chamberSecondCylinder =
        BRepPrimAPI_MakeCylinder(chamberSecondCylinderR, chamberSecondCylinderLenght);

    move.SetTranslation(gp_Vec(0, 0, chamberFirstCylinderLenght));

    chamberSecondCylinder = BRepBuilderAPI_Transform(chamberSecondCylinder, move).Shape();

    TopoDS_Shape chamberFirstCylinder =
        BRepPrimAPI_MakeCylinder(chamberTaperR2, chamberFirstCylinderLenght).Shape();

    TopoDS_Shape chamber = BRepAlgoAPI_Fuse(chamberFirstCylinder, chamberSecondCylinder);
    move.SetTranslation(gp_Vec(0, 0, chamberTaperLenght));
    chamber = BRepBuilderAPI_Transform(chamber, move).Shape();

    TopoDS_Shape chamberTaper =
        BRepPrimAPI_MakeCone(chamberMainConeR2, chamberTaperR2, chamberTaperLenght).Shape();

    chamber = BRepAlgoAPI_Fuse(chamber, chamberTaper).Shape();
    move.SetTranslation(gp_Vec(0, 0, chamberMainConeLenght));
    chamber = BRepBuilderAPI_Transform(chamber, move).Shape();

    TopoDS_Shape chamberMainCone =
        BRepPrimAPI_MakeCone(chamberMainConeR1, chamberMainConeR2, chamberMainConeLenght).Shape();

    chamber = BRepAlgoAPI_Fuse(chamber, chamberMainCone).Shape();

    // Cutting the chamber shape in the barrel

    barrel = BRepAlgoAPI_Cut(barrel, chamber);

    // Preparing the rifling

    BRepBuilderAPI_MakePolygon polygon;

    for (size_t i = 0; i < 6; i++)
    {
        polygon.Add(gp_Pnt(cos(M_PI / 3 * i) * riflingOuterRadius, sin(M_PI / 3 * i) * riflingOuterRadius, 0));
    }
    polygon.Close();

    TopoDS_Wire hexagonalWire = polygon.Wire();

    Handle(Geom_Plane) plane =
        new Geom_Plane(gp_Pln(gp::XOY()));

    BRepBuilderAPI_MakeFace mkFace(plane, hexagonalWire);
    TopoDS_Face profile = mkFace.Face();

    int sections = 36; // number of intermediate sections
    double totalTurns = riflingLenght / 177.8; // total revolutions over the length
    double totalTwist = totalTurns * 2.0 * M_PI;

    BRepOffsetAPI_ThruSections thru(true, false, Precision::Confusion());

    for (int i = 0; i <= sections; ++i) {
        double t = double(i) / sections;
        double z = t * riflingLenght;
        double angle = t * totalTwist;

        // rotate around Z
        gp_Trsf rot;
        rot.SetRotation(gp::OZ(), angle);
        TopoDS_Shape rotated = BRepBuilderAPI_Transform(hexagonalWire, rot).Shape();

        // translate to the correct Z
        gp_Trsf trans;
        trans.SetTranslation(gp_Vec(0, 0, z));
        TopoDS_Shape sectionShape = BRepBuilderAPI_Transform(rotated, trans).Shape();

        TopoDS_Wire sectionWire = TopoDS::Wire(sectionShape);
        thru.AddWire(sectionWire);
    }

    thru.Build();
    TopoDS_Shape rifling = thru.Shape();

    // Cutting the rifling in the barrel

    move.SetTranslation(gp_Vec(0, 0, muzzleTaperLenght + 15.0 - riflingLenght));
    rifling = BRepBuilderAPI_Transform(rifling, move).Shape();
    barrel = BRepAlgoAPI_Cut(barrel, rifling).Shape();

    STEPControl_Writer writer;

    writer.Transfer(barrel, STEPControl_AsIs);

    filename += ".step";
    const char* filenameBuffer = filename.c_str();
    IFSelect_ReturnStatus status =
        writer.Write(filenameBuffer);

    if (status == IFSelect_RetDone)
    {
        std::cout << "STEP exported.\n";
    }
    else
    {
        std::cout << "\033[31mExport failed, you probably gave an incorrect filename.\033[0m\n";
    }

    return 0;
}