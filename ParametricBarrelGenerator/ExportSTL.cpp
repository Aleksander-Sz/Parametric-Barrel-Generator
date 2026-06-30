#include "ExportSTL.h"

bool ExportSTL(const TopoDS_Shape& shape, const std::string& filename)
{
    try
    {
        // Create mesh
        BRepMesh_IncrementalMesh mesh(shape, 0.05);

        if (!mesh.IsDone())
            return false;

        StlAPI_Writer writer;

        std::string name = filename + ".stl";

        writer.Write(shape, name.c_str());
    }
    catch (Standard_Failure const& e)
    {
        std::cerr << e.what();
        return false;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what();
        return false;
    }
    std::cout << "STL exported.\n";
    return true;
}