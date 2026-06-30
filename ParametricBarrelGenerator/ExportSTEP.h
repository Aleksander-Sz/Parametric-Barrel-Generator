#pragma once
#include "BarrelParameters.h"
#include <TopoDS_Shape.hxx>
#include <STEPControl_Writer.hxx>

bool ExportSTEP(std::string filename, TopoDS_Shape barrel);