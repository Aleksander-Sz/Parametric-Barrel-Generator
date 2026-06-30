#include "Rifling.h"

TopoDS_Wire HexagonalWire(double riflingOuterRadius)
{
    BRepBuilderAPI_MakePolygon polygon;

    for (size_t i = 0; i < 6; i++)
    {
        polygon.Add(gp_Pnt(cos(M_PI / 3 * i) * riflingOuterRadius, sin(M_PI / 3 * i) * riflingOuterRadius, 0));
    }
    polygon.Close();
    return polygon.Wire();
}
