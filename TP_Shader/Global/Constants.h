#pragma once

#include "..\Geometry\Vector.h"
#include "..\Geometry\Point.h"

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846f
#define INV_PI 0.31830988618379067154f

// INV_TWOPI ...
#ifndef INFINITY
#define INFINITY FLT_MAX
#endif
const int nbRebondMax = 5;
const int nbEchantillon = 15;
const float r_delta = 10.f;
// Distance d'intersection null
const double noIntersect = -10.0f;

// Vecteur d'intersection null
const Vector noIntersectVec (-10.0f);

// Point d'intersection null
const Point noIntersectPoint(-10.0f);

// Pas de déplacement de la caméra
const int moveStep = 10;

// Pas de déplacement du soleil
const float sunMoveStep = 100.f;

// Step coeff diffus / spéculaire
const float coeffStep = 0.05f;

// Step intensité soleil / global
const float intensityStep = 0.05f;

// Step influence soleil / spéculaire
const int influenceStep = 1;

// Step samples
const int samplesStep = 1;
