#include <fstream>

#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRAlgo_Projector.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <cmath>
#include <Precision.hxx>

#include <iostream>

#include "Input.h"
#include "Geometry.h"
#include "ExportSVG.h"
#include "ExportSTEP.h"
#include "ExportSTL.h"

int main()
{
    try
    {
        BarrelParameters p = getBarrelParameters();

        std::string filename;
        if (!loadFilename(filename))
            return 0;

        TopoDS_Shape barrel = BuildBarrel(p);

        // Creating the chamber:

        TopoDS_Shape chamber = BuildChamber(p);

        // Cutting the chamber shape in the barrel

        barrel = BRepAlgoAPI_Cut(barrel, chamber);

        // Preparing the rifling

        TopoDS_Shape rifling = BuildRifling(p);

        // Cutting the rifling in the barrel

        gp_Trsf move;
        move.SetTranslation(gp_Vec(0, 0, p.muzzleTaperLenght + 15.0 - p.riflingLenght));
        rifling = BRepBuilderAPI_Transform(rifling, move).Shape();
        barrel = BRepAlgoAPI_Cut(barrel, rifling).Shape();

        if (!ExportSTEP(filename, barrel))
            return 0;
        // Now the .svg file

        if (!ExportSVG(filename, barrel, p))
            return 0;

        if (!ExportSTL(barrel, filename))
            return 0;
    }
    catch (Standard_Failure const& e)
    {
        std::cerr << e.what();
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what();
    }
    return 0;
}