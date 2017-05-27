#pragma once
#include "stdafx.h"
#include "hough.h"
using std::vector;
using namespace cimg_library;
#ifndef IMAGEWRAPPING_H
#define IMAGEWRAPPING_H
CImg<double> reverse_wrapping(const CImg<double>, vector<point>);
#endif