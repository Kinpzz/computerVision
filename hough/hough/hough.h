#pragma once
#include "stdafx.h"

using namespace cimg_library;
using std::vector;
#ifndef HOUGH_H
#define HOUGH_H

struct line {
    double _k, _b;
    line(double k, double b) : _k(k), _b(b) {}
};
struct point {
    int _x, _y;
    int _value;
    point(int x, int y, int v) : _x(x), _y(y), _value(v) {}
};

const double PI = 3.14159265358979323846;

class hough {
public:
    hough(CImg<double> source) : _img(source) , _sigma(3), _hough_threshold(700), _hough_diff(300), _gradient_threshold(50){}
    void set_blur_sigma(double);
    void set_threshold(char, int);
    CImg<double> get_rgb2gray();
    CImg<double> get_gradient();
    CImg<double> get_hough_space();
    vector<point> get_peaks();
    vector<line> get_lines();
    vector<point> get_intersection();
    CImg<double> get_draw_lines();
    double get_distance(int, int);
    bool check_line_valie(int, int);
private:
    CImg<double> _img;
    CImg<double> _result;
    CImg<int> _hough_img;
    vector<point> _peaks;
    vector<line> _lines;
    vector<point> _intersection;

    double _sigma;
    int _hough_threshold;
    int _hough_diff;
    int _gradient_threshold;
};

#endif
