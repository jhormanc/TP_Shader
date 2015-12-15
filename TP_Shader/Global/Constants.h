#pragma once

#include "..\Geometry\Vector.h"
#include "..\Geometry\Point.h"
#include "ColorRGB.h"

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
const int nbEchantillon = 1;
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
const int samplesStep = 10;

// Step delta r
const float stepDeltaR = 5.f;

// Distance post processing effects
const float distMax = 6000.f;

// Facteur d'assombrissement [0,1]
const float shadowFactor = .2342f;

// Facteur de brouillard ]0,1]
const float fogFactor = .314f;

const ColorRGB black = ColorRGB{ .0f, .0f, .0f };
const ColorRGB white = ColorRGB{ 0.75f, 0.75f, 0.75f } *255.f;
const ColorRGB blue = ColorRGB{ .25f, .25f, .75f } *255.f;
const ColorRGB red = ColorRGB{ .75f, .25f, .25f } *255.f;
const ColorRGB yellow = ColorRGB{ .9f, .1f, .9f } *255.f;
const ColorRGB purple = ColorRGB{ .9f, .9f, .1f } *255.f;
const ColorRGB grey = ColorRGB{ 105.f, 105.f, 105.f };
const ColorRGB grey_light = ColorRGB{ 190.f, 190.f, 190.f };
const ColorRGB orange = ColorRGB{ 255.f, 140.f, 0.f };

const ColorRGB sky = ColorRGB{ 135.f, 206.f, 235.f };

const ColorRGB roche = { 100.f, 100.f, 100.f };
const ColorRGB roche_claire = { 200.f, 200.f, 200.f };
const ColorRGB terre = { 95.f, 62.f, 5.f };
const ColorRGB terre_claire = { 195.f, 162.f, 105.f };
const ColorRGB roche_terre_claire = { 197.5f, 181.f, 152.5f };
const ColorRGB herbe = { 0.f, 104.f, 10.f };
const ColorRGB neige = { 255.f, 255.f, 255.f };
const ColorRGB bleue = { 0.f, 128.f, 220.f };
