#pragma once
#include <iostream>
#include "BarrelParameters.h"
#include <TopoDS_Shape.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

bool ExportSTL(const TopoDS_Shape& shape, const std::string& filename);