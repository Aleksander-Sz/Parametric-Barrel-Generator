#pragma once
#include <iostream>
#include "BarrelParameters.h"

bool loadCaliber(int& caliber);
bool loadLenght(double& riflingLenght, double& minRiflingLenght);
bool loadFilename(std::string& filename);
BarrelParameters getBarrelParameters();
void load556(BarrelParameters& p);
void load762(BarrelParameters& p);
void load127(BarrelParameters& p);