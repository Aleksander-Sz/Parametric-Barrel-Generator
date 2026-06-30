#include "ExportSTEP.h"

bool ExportSTEP(std::string filename, TopoDS_Shape barrel)
{
    try
    {
        STEPControl_Writer writer;

        writer.Transfer(barrel, STEPControl_AsIs);

        std::string filenameA = filename + ".step";
        const char* filenameBuffer = filenameA.c_str();
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

        std::cout << "\033[33mGenerating the .svg files now, be patient...\033[0m\n";
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
    return true;
}
