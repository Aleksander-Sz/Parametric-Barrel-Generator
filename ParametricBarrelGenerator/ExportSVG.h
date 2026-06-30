#pragma once
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopoDS_Shape.hxx>
#include <fstream>
#include "BarrelParameters.h"
#include <TopoDS.hxx>
#include <Geom_Curve.hxx>
#include <BRep_Tool.hxx>
#include <Standard_Handle.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>

bool ExportSVG(std::string filename, TopoDS_Shape barrel, BarrelParameters& p);
void ArrowDefinition(std::ofstream& svgFile);
void CreateAxis(std::ofstream& svgFile, BarrelParameters& p);
void IntersectionPlane(std::ofstream& svgFile, TopoDS_Shape& sectionShape);
void BehindPlane(std::ofstream& svgFile, TopoDS_Shape & barrel);
void PrepareSVGFile(std::ofstream& svgFile, std::string filename, BarrelParameters& p);