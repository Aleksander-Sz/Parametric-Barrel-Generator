#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS_Shape.hxx>
#include <STEPControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>

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

    //temporarily only 5.56:
    switch (caliber)
    {
    case 1: // 5.56
        muzzleBarrelR1 = 12.5;
        muzzleBarrelR2 = 8.5;
        muzzleTaperLenght = 400.0f;

        chamberMainConeR1 = 4.825;
        chamberMainConeR2 = 4.5;
        chamberMainConeLenght = 36.5;
        chamberTaperR2 = 3.25;
        chamberTaperLenght = 2.9448;
        chamberFirstCylinderLenght = 44.96 - chamberMainConeLenght - chamberTaperLenght;
        chamberSecondCylinderR = 2.845;
        chamberSecondCylinderLenght = 1.45;
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

    //TopoDS_Shape chamberMainCone = 
    //    BRepPrimAPI_MakeCone

    TopoDS_Shape chamberTaper =
        BRepPrimAPI_MakeCone(chamberMainConeR2, chamberTaperR2, chamberTaperLenght);

    TopoDS_Shape chamberMainCone =
        BRepPrimAPI_MakeCone(chamberMainConeR1, chamberMainConeR2, chamberMainConeLenght);


    //move.SetTranslation = 

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