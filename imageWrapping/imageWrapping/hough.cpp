// hough.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "hough.h"

void hough::set_blur_sigma(double sigma) {
    _sigma = sigma;
}

// 'h' for hough peaks threshold
// 'g' for gradient threshold 
// 'd' for hough diff
void hough::set_threshold(char type, int threshold) {
    switch (type) {
    case 'h':
        _hough_threshold = threshold;
        break;
    case 'g':
        _gradient_threshold = threshold;
        break;
    case 'd':
        _hough_diff = threshold;
        break;
    default:
        break;
    }
}

double hough::get_distance(int x, int y) {
    return sqrt(x * x + y * y);
}

double hough::get_distance(point a, point b) {
    return get_distance(a._x - b._x, a._y - b._y);
}

// rgb to gray :0.2989 * R + 0.5870 * G + 0.1140 * B
CImg<double> hough::get_rgb2gray() {
    CImg<double> _gray_img(_img._width, _img._height);
    double r, g, b;
    cimg_forXY(_gray_img, x, y) {
        r = _img(x, y, 0);
        g = _img(x, y, 1);
        b = _img(x, y, 2);
        _gray_img(x, y) = 0.2989 * r + 0.5870 * g + 0.1140 * b;
    }
    _result = _gray_img;
    return _result;
}

CImg<double> hough::get_gradient() {
    get_rgb2gray();
    CImg<double> _blur_img = _result;
    // blur
    _blur_img.blur(_sigma);
    CImg<double> _gradient(_blur_img._width, _blur_img._height);
    // define a 3x3 neighbour matrix
    CImg_3x3(I, double);
    double gx, gy, grad;
    cimg_for3x3(_blur_img, x, y, 0, 0, I, double) {
        // sobel operator
        gx = -Ipp - 2 * Ipc - Ipn + Inp + 2 * Inc + Inn;
        gy = -Ipp - 2 * Icp - Inp + Ipn + 2 * Icn + Inn;
        grad = sqrt(gx * gx + gy * gy);
        if (grad > _gradient_threshold) {
            _gradient(x,y) = 1;
        }
        else {
            _gradient(x, y) = 0;
        }
    }
    _result = _gradient;
    return _result;
}

CImg<double> hough::get_hough_space() {
    get_gradient();
    // hough space in polar coordinate system
    // (angle, r) r is distance between original point and line in rectangular coordinate system
    // point in rectangular to line in hough in polar
    // line  in rectangular to point in hough in polar
    _hough_img = CImg<int>(360, round(get_distance(_result._width, _result._height)));

    // reset the pixel to zero 
    _hough_img.fill(0);

    double theta;
    int r;
    cimg_forXY(_result, x, y) {
        if (_result(x, y) == 1) {
            cimg_forX(_hough_img, angle) {
                theta = angle * PI / 180.0;
                r = round((x*cos(theta) + y*sin(theta)));
                if (r >= 0 && r <= _hough_img._height) {
                    _hough_img(angle, r) += 1;
                }
            }
        }
    }
    return _hough_img;
}

vector<point> hough::get_peaks() {
    if (_peaks.size() == 0) {
        if (_hough_img.data() == NULL) get_hough_space();
        bool is_neighbour_peaks;
        vector<point> peaks;
        cimg_forXY(_hough_img, angle, r) {
            if (_hough_img(angle, r) > _hough_threshold) {
                is_neighbour_peaks = false;
                for (vector<point>::iterator iter = peaks.begin(); iter != peaks.end();) {
                    if ((int)get_distance((*iter)._x - angle, (*iter)._y - r) < _hough_diff) {
                        is_neighbour_peaks = true;
                        if ((*iter)._value < _hough_img(angle, r)) {
                            iter = peaks.erase(iter);
                        }
                        else {
                            iter++;
                        }
                    }
                    else {
                        iter++;
                    }
                }
                if (is_neighbour_peaks == false) {
                    peaks.push_back(point(angle, r, _hough_img(angle, r)));
                }
            }
        }
        std::sort(peaks.begin(), peaks.end(), [](point a, point b) { return a._value > b._value; });
        if (peaks.size() >= 4) {
            for (int i = 0; i < 4; i++) {
                _peaks.push_back(peaks.at(i));
            }
        }
        else {
            for (int i = 0; i < peaks.size(); i++) {
                _peaks.push_back(peaks.at(i));
            }
        }
    }
    return _peaks;
}

bool hough::check_line_valie(int angle, int r) {
    return true;
}

vector<line> hough::get_lines() {
    if (_lines.size() == 0) {
        if (_peaks.size() == 0) get_peaks();
        _lines.clear();
        double angle, k, b;
        for (int i = 0; i < _peaks.size(); i++) {
            angle = _peaks[i]._x*PI / 180.0;
            k = -cos(angle) / sin(angle);
            b = _peaks[i]._y / sin(angle);
            _lines.push_back(line(k, b));
            std::cout << "y = " << k << " * x + " << b << std::endl;
        }
    }
    return _lines;
}

vector<point> hough::get_intersection() {
    if (_intersection.size() == 0) {
        if (_lines.size() == 0) get_lines();
        _intersection.clear();
        int x0, y0;
        double k0, k1, b0, b1;
        for (int i = 0; i < _lines.size(); i++) {
            for (int j = i + 1; j < _lines.size(); j++) {
                k0 = _lines[i]._k;
                k1 = _lines[j]._k;
                b0 = _lines[i]._b;
                b1 = _lines[j]._b;
                x0 = round((b1 - b0) / (k0 - k1));
                y0 = round((k0*b1 - k1*b0) / (k0 - k1));
                if (x0 >= 0 && x0 < _img._width &&  y0 >= 0 && y0 < _img._height) {
                    _intersection.push_back(point(x0, y0, 0));
                }
            }
        }
        std::sort(_intersection.begin(), _intersection.end(), [](point a, point b) { return (a._x * a._y) < (b._x * b._y); });
        point origin = _intersection.at(0);
        std::sort(_intersection.begin(), _intersection.end(), [&origin](point a, point b) {
            return ((a._x - origin._x) * (a._x - origin._x) + (a._y - origin._y) * (a._y - origin._y))
                < ((b._x - origin._x) * (b._x - origin._x) + (b._y - origin._y) * (b._y - origin._y));});
        double width, height;
        vector<point> temp;
        if (_intersection.at(1)._y < _intersection.at(2)._y) {
            temp.push_back(_intersection.at(0));
            temp.push_back(_intersection.at(1));
            temp.push_back(_intersection.at(3));
            temp.push_back(_intersection.at(2));
        }
        else {
            temp.push_back(_intersection.at(2));
            temp.push_back(_intersection.at(3));
            temp.push_back(_intersection.at(1));
            temp.push_back(_intersection.at(0));
        }
        _intersection = temp;
        for (int i = 0; i < _intersection.size(); i++) {
            std::cout << "(" << _intersection.at(i)._x << ", " << _intersection.at(i)._y << ")" << std::endl;
        }
    }
    return _intersection;
}

CImg<double> hough::get_draw_lines() {
    if (_intersection.size() == 0) get_intersection();
    int x0, y0, x1, y1;
    const double color[] = { 255,255,0 };
    // draw lines
    for (int i = 0; i < _lines.size(); i++) {
        if (_lines[i]._k > 1) {
            y0 = 0;
            y1 = _img._height;
            x0 = round((double)(y0 - _lines[i]._b) / _lines[i]._k);
            x1 = round((double)(y1 - _lines[i]._b) / _lines[i]._k);
        }
        else {
            x0 = 0;
            x1 = _img._width;
            y0 = round(_lines[i]._k * x0 + _lines[i]._b);
            y1 = round(_lines[i]._k * x1 + _lines[i]._b);
        }
        _img.draw_line(x0, y0, x1, y1, color);
    }
    // draw intersection
    for (int i = 0; i < _intersection.size(); i++) {
        _img.draw_circle(_intersection[i]._x, _intersection[i]._y, 10, color);
    }
    return _img;
}