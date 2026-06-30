#pragma once
#include <iostream>

struct BarrelParameters {
    double muzzleBarrelR1;
    double muzzleBarrelR2;

    double chamberMainConeR1;
    double chamberMainConeR2;
    double chamberMainConeLenght;
    double chamberTaperR2;
    double chamberTaperLenght;
    double chamberFirstCylinderLenght;
    double chamberSecondCylinderR;
    double chamberSecondCylinderLenght;

    double riflingOuterRadius;
    double minRiflingLenght;

    double riflingLenght;
    double muzzleTaperLenght;

    std::string caliberText;
};