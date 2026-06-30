#include "Geometry.h"

TopoDS_Shape BuildBarrel(const BarrelParameters& p)
{
    TopoDS_Shape muzzle =
        BRepPrimAPI_MakeCone(p.muzzleBarrelR1, p.muzzleBarrelR2, p.muzzleTaperLenght).Shape();

    gp_Trsf move;
    move.SetTranslation(gp_Vec(0, 0, BACK_MOUNT_LENGHT));

    muzzle = BRepBuilderAPI_Transform(muzzle, move).Shape();

    TopoDS_Shape backMount =
        BRepPrimAPI_MakeCylinder(p.muzzleBarrelR1 - 1.5, BACK_MOUNT_LENGHT).Shape();

    TopoDS_Shape barrel =
        BRepAlgoAPI_Fuse(muzzle, backMount).Shape();
    return barrel;
}

TopoDS_Shape BuildChamber(const BarrelParameters& p)
{
    TopoDS_Shape chamberSecondCylinder =
        BRepPrimAPI_MakeCylinder(p.chamberSecondCylinderR, p.chamberSecondCylinderLenght);

    gp_Trsf move;
    move.SetTranslation(gp_Vec(0, 0, p.chamberFirstCylinderLenght));

    chamberSecondCylinder = BRepBuilderAPI_Transform(chamberSecondCylinder, move).Shape();

    TopoDS_Shape chamberFirstCylinder =
        BRepPrimAPI_MakeCylinder(p.chamberTaperR2, p.chamberFirstCylinderLenght).Shape();

    TopoDS_Shape chamber = BRepAlgoAPI_Fuse(chamberFirstCylinder, chamberSecondCylinder);
    move.SetTranslation(gp_Vec(0, 0, p.chamberTaperLenght));
    chamber = BRepBuilderAPI_Transform(chamber, move).Shape();

    TopoDS_Shape chamberTaper =
        BRepPrimAPI_MakeCone(p.chamberMainConeR2, p.chamberTaperR2, p.chamberTaperLenght).Shape();

    chamber = BRepAlgoAPI_Fuse(chamber, chamberTaper).Shape();
    move.SetTranslation(gp_Vec(0, 0, p.chamberMainConeLenght));
    chamber = BRepBuilderAPI_Transform(chamber, move).Shape();

    TopoDS_Shape chamberMainCone =
        BRepPrimAPI_MakeCone(p.chamberMainConeR1, p.chamberMainConeR2, p.chamberMainConeLenght).Shape();

    chamber = BRepAlgoAPI_Fuse(chamber, chamberMainCone).Shape();
    return chamber;
}

TopoDS_Shape BuildRifling(const BarrelParameters& p)
{
    TopoDS_Wire hexagonalWire = HexagonalWire(p.riflingOuterRadius);

    Handle(Geom_Plane) plane =
        new Geom_Plane(gp_Pln(gp::XOY()));

    BRepBuilderAPI_MakeFace mkFace(plane, hexagonalWire);
    TopoDS_Face profile = mkFace.Face();

    int sections = 36; // number of intermediate sections
    double totalTurns = p.riflingLenght / 177.8; // total revolutions over the length
    double totalTwist = totalTurns * 2.0 * M_PI;

    BRepOffsetAPI_ThruSections thru(true, false, Precision::Confusion());

    for (int i = 0; i <= sections; ++i) {
        double t = double(i) / sections;
        double z = t * p.riflingLenght;
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
    if (!thru.IsDone())
        throw std::runtime_error("Failed to generate rifling.");
    TopoDS_Shape rifling = thru.Shape();
    return rifling;
}
