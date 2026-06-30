#pragma once
#include "BarrelParameters.h"
#include "Rifling.h"
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <STEPControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRep_Tool.hxx>

#define BACK_MOUNT_LENGHT 15.0

TopoDS_Shape BuildBarrel(const BarrelParameters& p);

TopoDS_Shape BuildChamber(const BarrelParameters& p);

TopoDS_Shape BuildRifling(const BarrelParameters& p);