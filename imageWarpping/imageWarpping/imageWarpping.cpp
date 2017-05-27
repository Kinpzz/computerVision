// imageWrapping.cpp : ��������̨Ӧ�ó��������ڵ㡣
//

#include "stdafx.h"
#include "imageWarpping.h"

using namespace cimg_library;
CImg<double> reverse_wrapping(const CImg<double> source, vector<point> vertex) {
    double x[4], y[4];
for (int i = 0; i < 4; i++) {
    x[i] = vertex.at(i)._x;
    y[i] = vertex.at(i)._y;
}
double dx3 = x[0] - x[1] + x[2] - x[3];
double dy3 = y[0] - y[1] + y[2] - y[3];
double H[3][3];
if (dx3 == 0.0f && dy3 == 0.0f) {
    H[0][0] = x[1] - x[0];
    H[0][1] = y[1] - y[0];
    H[0][2] = 0;
    H[1][0] = x[2] - x[1];
    H[1][1] = y[2] - y[1];
    H[1][2] = 0;
    H[2][0] = x[0];
    H[2][1] = y[0];
    H[2][2] = 1;
} else {
    double dx1 = x[1] - x[2];
    double dx2 = x[3] - x[2];
    double dy1 = y[1] - y[2];
    double dy2 = y[3] - y[2];
    double denominator = dx1 * dy2 - dx2 * dy1;
    double a13 = (dx3 * dy2 - dx2 * dy3) / denominator;
    double a23 = (dx1 * dy3 - dx3 * dy1) / denominator;
    H[0][0] = x[1] - x[0] + a13 * x[1];
    H[0][1] = y[1] - y[0] + a13 * y[1];
    H[0][2] = a13;
    H[1][0] = x[3] - x[0] + a23 * x[3];
    H[1][1] = y[3] - y[0] + a23 * y[3];
    H[1][2] = a23;
    H[2][0] = x[0];
    H[2][1] = y[0];
    H[2][2] = 1;
}
    CImg<double> target(794, 1123, 1, 3);
    cimg_forXY(target, x, y) {
        double _x = (double)x / target._width;
        double _y = (double)y / target._height;
        double denominator = H[0][2] * _x + H[1][2] * _y + H[2][2];
        double tx = (H[0][0] * _x + H[1][0] * _y + H[2][0]) / denominator;
        double ty = (H[0][1] * _x + H[1][1] * _y + H[2][1]) / denominator;
        cimg_forC(target, c) {
            target(x, y, 0, c) = source.linear_atXY(tx, ty, 0, c);
        }
    }
    return target;
}