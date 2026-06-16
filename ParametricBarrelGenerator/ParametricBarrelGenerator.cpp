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
    std::cout << "Input the desired caliber: ";
    std::cin >> caliber;
    std::cout << "Input the desired rifling lenght: ";
    std::cin >> riflingLenght;
    
    double muzzleBarrelR1, muzzleBarrelR2, muzzleTaperLenght;
    double chamberMainConeR1, chamberMainConeR2, chamberMainConeLenght, chamberTaperR2, chamberTaperLenght, chamberFirstCylinderLenght, chamberSecondCylinderR, chamberSecondCylinderLenght;
    double riflingOuterRadius;

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

        muzzleTaperLenght = riflingLenght + 15.0 - chamberMainConeLenght - chamberTaperLenght - chamberFirstCylinderLenght - chamberSecondCylinderLenght;

        riflingOuterRadius = 2.845;
        break;
    }

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

    IFSelect_ReturnStatus status =
        writer.Write("barrel.step");

    if (status == IFSelect_RetDone)
    {
        std::cout << "STEP exported.\n";
    }
    else
    {
        std::cout << "Export failed.\n";
    }

    return 0;
}