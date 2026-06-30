#include "Input.h"

bool loadCaliber(int& caliber)
{
    std::cout << "Welcome to the parametric barrel generation system.\n";
    std::cout << "The available calibers are:\n1. 5.56 x 45 mm NATO\n2. 7.62 x 51 mm NATO\n3. 12.7 x 99 mm NATO\n";
    std::cout << "Input the desired caliber (1, 2 or 3): ";
    if (!(std::cin >> caliber))
    {
        std::cerr << "Invalid input.\n";
        return false;
    }
    if (caliber < 1 || caliber>3)
    {
        std::cout << "Error, you entered an incorrect value!\n";
        return false;
    }
    return true;
}

bool loadLenght(double& riflingLenght, double& minRiflingLenght)
{
    std::cout << "Input the desired rifling lenght in milimeters (" << minRiflingLenght << ".0 - 1500.0): ";
    if (!(std::cin >> riflingLenght))
    {
        std::cerr << "Invalid input.\n";
        return false;
    }
    if (riflingLenght < minRiflingLenght || riflingLenght > 1500.0)
    {
        std::cout << "Error, you entered an incorrect value!\n";
        return false;
    }
    return true;
}

bool loadFilename(std::string& filename)
{
    std::cout << "File name (no extension): ";
    if (!(std::cin >> filename))
    {
        std::cerr << "Invalid input.\n";
        return false;
    }
    return true;
}

BarrelParameters getBarrelParameters()
{
    int caliber;

    if (!loadCaliber(caliber))
        throw "Failed to read caliber from console";


    BarrelParameters p;
    switch (caliber)
    {
    case 1: // 5.56
        load556(p);
        break;
    case 2: // 7.62
        load762(p);
        break;
    case 3: // 12.7
        load127(p);
        break;
    }

    if (!loadLenght(p.riflingLenght, p.minRiflingLenght))
        throw "Failed to read rifling lenght from console";

    p.muzzleTaperLenght = p.riflingLenght - 15.0 + p.chamberMainConeLenght + p.chamberTaperLenght + p.chamberFirstCylinderLenght + p.chamberSecondCylinderLenght;

    return p;
}

void load556(BarrelParameters& p)
{
    p.muzzleBarrelR1 = 12.5;
    p.muzzleBarrelR2 = 8.5;

    p.chamberMainConeR1 = 4.825;
    p.chamberMainConeR2 = 4.5;
    p.chamberMainConeLenght = 36.5;
    p.chamberTaperR2 = 3.25;
    p.chamberTaperLenght = 2.9448;
    p.chamberFirstCylinderLenght = 44.96 - p.chamberMainConeLenght - p.chamberTaperLenght;
    p.chamberSecondCylinderR = 2.845;
    p.chamberSecondCylinderLenght = 1.45;

    p.riflingOuterRadius = 2.845;
    p.minRiflingLenght = 254.0;

    p.caliberText = "5.56 mm";
}

void load762(BarrelParameters& p)
{
    p.muzzleBarrelR1 = 15.0;
    p.muzzleBarrelR2 = 10.0;

    p.chamberMainConeR1 = 6.0;
    p.chamberMainConeR2 = 5.765;
    p.chamberMainConeLenght = 43.5;
    p.chamberTaperR2 = 4.36;
    p.chamberTaperLenght = 3.86;
    p.chamberFirstCylinderLenght = 51.18 - p.chamberMainConeLenght - p.chamberTaperLenght;
    p.chamberSecondCylinderR = 3.925;
    p.chamberSecondCylinderLenght = 3.0;

    p.riflingOuterRadius = 3.96;
    p.minRiflingLenght = 406.0;

    p.caliberText = "7.62 mm";
}

void load127(BarrelParameters& p)
{
    p.muzzleBarrelR1 = 25.0;
    p.muzzleBarrelR2 = 15.0;

    p.chamberMainConeR1 = 10.21;
    p.chamberMainConeR2 = 9.07;
    p.chamberMainConeLenght = 76.34;
    p.chamberTaperR2 = 7.11;
    p.chamberTaperLenght = 6.96;
    p.chamberFirstCylinderLenght = 99.31 - p.chamberMainConeLenght - p.chamberTaperLenght;
    p.chamberSecondCylinderR = 6.98;
    p.chamberSecondCylinderLenght = 4.0;

    p.riflingOuterRadius = 7.05;
    p.minRiflingLenght = 559.0;

    p.caliberText = "12.7 mm";
}
