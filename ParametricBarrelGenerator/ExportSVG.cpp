#include "ExportSVG.h"

bool ExportSVG(std::string filename, TopoDS_Shape barrel, BarrelParameters& p)
{
    try
    {
        gp_Pln sectionPlane(
            gp_Pnt(0, 0, 0),
            gp_Dir(0, 1, 0)); // XZ plane

        BRepAlgoAPI_Section section(barrel, sectionPlane);
        section.Build();

        TopoDS_Shape sectionShape = section.Shape();

        std::ofstream svgFile;
        PrepareSVGFile(svgFile, filename, p);

        BehindPlane(svgFile, barrel);

        IntersectionPlane(svgFile, sectionShape);

        // Now behind the cutting plane

        BehindPlane(svgFile, barrel);

        // The axis
        CreateAxis(svgFile, p);

        svgFile << "</svg>";
        svgFile.close();

        std::cout << "SVG exported.\n";
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

void ArrowDefinition(std::ofstream& svgFile)
{
    svgFile << "<defs>\n";

    svgFile << "<marker id=\"arrow\" markerWidth=\"6\" markerHeight=\"6\" "
        << "refX=\"6\" refY=\"3\" orient=\"auto-start-reverse\" "
        << "markerUnits=\"strokeWidth\">\n";

    svgFile << "  <path d=\"M0,0 L6,3 L0,6 z\" fill=\"black\" />\n";
    svgFile << "</marker>\n";

    svgFile << "</defs>\n";

    svgFile << "<style>\n";
    svgFile << "text { font-family: monospace; }\n";
    svgFile << "line { shape-rendering: crispEdges; }\n";
    svgFile << "</style>\n";
}

void CreateAxis(std::ofstream& svgFile, BarrelParameters& p)
{
    svgFile << "<!-- AXIS X -->\n";
    svgFile << "<line x1=\"-5\" y1=\"0\" x2=\"" << (int)p.muzzleTaperLenght << "\" y2=\"0\" stroke=\"black\" stroke-width=\"0.3\" marker-end=\"url(#arrow)\"/>\n";
    svgFile << "<text x=\"" << (int)p.muzzleTaperLenght << "\" y=\"5\" font-size=\"5\">X</text>\n";

    svgFile << "<!-- AXIS Y -->\n";
    svgFile << "<line x1=\"-5\" y1=\"0\" x2=\"-5\" y2=\"" << -(int)p.muzzleBarrelR1 - 5 << "\" stroke=\"black\" stroke-width=\"0.3\" marker-end=\"url(#arrow)\"/>\n";
    svgFile << "<text x=\"3\" y=\"" << -(int)p.muzzleBarrelR1 - 5 << "\" font-size=\"5\">Y</text>\n";

    double chamberTotalLenght = p.chamberMainConeLenght + p.chamberTaperLenght + p.chamberFirstCylinderLenght + p.chamberSecondCylinderLenght;
    svgFile << "<!-- RIFLING LENGTH -->\n";
    svgFile << "<line x1=\"" << (int)chamberTotalLenght << "\" y1=\"" << (int)p.muzzleBarrelR1 + 3 << "\" x2=\"" << (int)(chamberTotalLenght + p.riflingLenght) << "\" y2=\"" << (int)p.muzzleBarrelR1 + 3 << "\" "
        << "stroke=\"black\" stroke-width=\"0.3\" marker-start=\"url(#arrow)\" marker-end=\"url(#arrow)\"/>\n";

    svgFile << "<line x1=\"" << (int)chamberTotalLenght << "\" y1=\"" << (int)(p.riflingOuterRadius / 2.0) << "\" x2=\"" << (int)chamberTotalLenght << "\" y2=\"" << (int)p.muzzleBarrelR1 + 3 << "\" stroke=\"black\" stroke-width=\"0.2\"/>\n";
    svgFile << "<line x1=\"" << (int)(chamberTotalLenght + p.riflingLenght) << "\" y1=\"" << (int)(p.riflingOuterRadius) << "\" x2=\"" << (int)(chamberTotalLenght + p.riflingLenght) << "\" y2=\"" << (int)p.muzzleBarrelR1 + 3 << "\" stroke=\"black\" stroke-width=\"0.2\"/>\n";

    svgFile << "<text x=\"" << (int)(chamberTotalLenght + p.riflingLenght / 2.0) << "\" y=\"" << (int)p.muzzleBarrelR1 + 1 << "\" font-size=\"5\">" << (float)p.riflingLenght << " mm</text>\n";

    svgFile << "<!-- CALIBER -->\n";
    svgFile << "<line x1=\"" << (int)(chamberTotalLenght + p.riflingLenght + 5) << "\" y1=\"" << (int)(p.riflingOuterRadius) << "\" x2=\"" << (int)(chamberTotalLenght + p.riflingLenght + 5) << "\" y2=\"" << -(int)(p.riflingOuterRadius) << "\" "
        << "stroke=\"black\" stroke-width=\"0.3\" marker-start=\"url(#arrow)\" marker-end=\"url(#arrow)\"/>\n";

    svgFile << "<line x1=\"" << (int)(chamberTotalLenght + p.riflingLenght) << "\" y1=\"" << (int)(p.riflingOuterRadius) << "\" x2=\"" << (int)(chamberTotalLenght + p.riflingLenght + 5) << "\" y2=\"" << (int)(p.riflingOuterRadius) << "\" stroke=\"black\" stroke-width=\"0.2\" stroke-dasharray=\"2,2\"/>\n";
    svgFile << "<line x1=\"" << (int)(chamberTotalLenght + p.riflingLenght) << "\" y1=\"" << -(int)(p.riflingOuterRadius) << "\" x2=\"" << (int)(chamberTotalLenght + p.riflingLenght + 5) << "\" y2=\"" << -(int)(p.riflingOuterRadius) << "\" stroke=\"black\" stroke-width=\"0.2\" stroke-dasharray=\"2,2\"/>\n";

    svgFile << "<text x=\"" << (int)(chamberTotalLenght + p.riflingLenght + 6) << "\" y=\"0\" font-size=\"5\">" << p.caliberText << "</text>\n";
}

void IntersectionPlane(std::ofstream& svgFile, TopoDS_Shape& sectionShape)
{
    for (TopExp_Explorer exp(sectionShape, TopAbs_EDGE);
        exp.More();
        exp.Next())
    {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());

        double first, last;

        Handle(Geom_Curve) curve =
            BRep_Tool::Curve(edge, first, last);

        svgFile << "<path d=\"";

        bool firstPoint = true;

        for (int i = 0; i <= 250; ++i)
        {
            double t =
                first + (last - first) * i / 250.0;

            gp_Pnt p;
            curve->D0(t, p);

            double x = p.Z();   // barrel length
            double y = -p.X();  // invert Y for SVG

            if (firstPoint)
            {
                svgFile << "M " << x << " " << y;
                firstPoint = false;
            }
            else
            {
                svgFile << " L " << x << " " << y;
            }
        }
        svgFile << "\" "
            << "fill=\"none\" "
            << "stroke=\"black\" "
            << "stroke-width=\"0.8\"/>\n";
    }
}

void BehindPlane(std::ofstream& svgFile, TopoDS_Shape& barrel)
{
    // cut the barrel in half

    TopoDS_Shape box =
        BRepPrimAPI_MakeBox(gp_Pnt(100, -100, 0), gp_Pnt(-100, 0, 2000));
    barrel = BRepAlgoAPI_Cut(barrel, box);

    Handle(HLRBRep_Algo) hlr = new HLRBRep_Algo();

    gp_Dir viewDir(0, 1, 0);
    HLRAlgo_Projector projector(
        gp_Ax2(gp_Pnt(0, 0, 0), viewDir));

    hlr->Projector(projector);

    hlr->Add(barrel);

    hlr->Update();
    hlr->Hide();

    HLRBRep_HLRToShape hlrShape(hlr);

    TopoDS_Shape visibleEdges = hlrShape.VCompound();
    TopoDS_Shape hiddenEdges = hlrShape.HCompound();

    for (TopExp_Explorer exp(hiddenEdges, TopAbs_EDGE);
        exp.More();
        exp.Next())
    {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());

        BRepAdaptor_Curve curve(edge);

        double first = curve.FirstParameter();
        double last = curve.LastParameter();

        svgFile << "<path d=\"";

        bool firstPoint = true;

        for (int i = 0; i <= 200; ++i)
        {
            double t =
                first + (last - first) * i / 200.0;

            gp_Pnt p;
            curve.D0(t, p);

            double x = p.X();
            double y = p.Y();

            if (firstPoint)
            {
                svgFile << "M " << x << " " << y;
                firstPoint = false;
            }
            else
            {
                svgFile << " L " << x << " " << y;
            }
        }

        svgFile << "\" fill=\"none\" stroke=\"black\" stroke-width=\"0.3\"/>\n";
    }
}

void PrepareSVGFile(std::ofstream& svgFile, std::string filename, BarrelParameters& p)
{
    filename += ".svg";
    const char* filenameBuffer = filename.c_str();
    svgFile = std::ofstream(filenameBuffer);
    if (!svgFile.is_open())
    {
        std::cout << "\033[31mError, failed to create the .svg file!\033[0m\n";
        throw "Error, failed to create the .svg file";
    }
    svgFile
        << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        << "width=\"" << (int)p.muzzleTaperLenght + 70 << "\" height=\"" << (int)p.muzzleBarrelR1 * 2 + 20 << "\" "
        << "viewBox=\"-10 " << -(int)(p.muzzleBarrelR1 + 10) << " " << (int)p.muzzleTaperLenght + 70 << " " << (int)p.muzzleBarrelR1 * 2 + 20 << "\">\n";

    // Arrow definitions
    ArrowDefinition(svgFile);
}
