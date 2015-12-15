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
const float sunMoveStep = 500.f;

// Step coeff diffus / spéculaire
const float coeffStep = 0.1f;

// Step intensité soleil / global
const float intensityStep = 0.1f;

// Step influence soleil / spéculaire
const int influenceStep = 2;

// Step nombre d'échantillons
const int samplesStep = 1;

// Step delta r
const float stepDeltaR = 5.f;

// Distance post processing effects
const float distMax = 5000.f;

// Facteur d'assombrissement [0,1]
const float shadowFactor = .2342f;

// Facteur de brouillard ]0,1]
const float fogFactor = .314f;
