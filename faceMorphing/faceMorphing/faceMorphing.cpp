/* author : Yan Pengxiang 14331327
 * kinpzzz(at)gmail
 * 2017.4
 */

#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "CImg.h"
using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using namespace cimg_library;

struct point {
    int x;
    int y;
    point(int _x, int _y) : x(_x), y(_y) {}
};

struct triangle {
    point a;
    point b;
    point c;
    int index[3];
    triangle(point _a, point _b, point _c, int a_index, int b_index, int c_index)
        : a(_a), b(_b), c(_c) {
        index[0] = a_index;
        index[1] = b_index;
        index[2] = c_index;
    }
    triangle(point _a, point _b, point _c) : a(_a), b(_b), c(_c) {}

    bool isInTriangle(const point& p);
};

CImg<float> getDetectionPoints(vector<point*>& points, const CImg<float>& src, string filename);
double getDistance(point A, point B);
bool isPointInCircle(point* P, triangle* tri);
void getDelaunayTriangles(vector<triangle*>& triangles, const vector<point*>& points);
void drawTriangles(const vector<triangle*>& triangles, const CImg<float>& src);
triangle* getTransitionTriangle(const triangle* A, const triangle* B, double rate);
void getTransTriangle2Triangle(double(*H)[3], const triangle* src, const triangle* dst);
int cross3(const point &a, const point &b, const point &p);
CImg<float> morphTriangle(double rate, const vector<triangle*>& triangles_A, const vector<triangle*>& triangles_B,
    const CImg<float>& img_A, const CImg<float>& img_B);

int main() {

    // --- step 1 : read img ---

    string filename_A = "img/1.jpg";
    string filename_B = "img/2.jpg";
    CImg<float> img_A(filename_A.c_str());
    CImg<float> img_B(filename_B.c_str());
    
    // --- step 2 : get dection points ---
    vector<point*> points_A, points_B;

    // add four corners and four middle points
    points_A.push_back(new point(0, 0));
    points_A.push_back(new point(0, img_A._height));
    points_A.push_back(new point(img_A._width, 0));
    points_A.push_back(new point(img_A._width, img_A._height));
    points_A.push_back(new point(0, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, 0));
    points_A.push_back(new point(img_A._width, img_A._height/2));
    points_A.push_back(new point(img_A._width/2, img_A._height));

    points_B.push_back(new point(0, 0));
    points_B.push_back(new point(0, img_B._height));
    points_B.push_back(new point(img_B._width, 0));
    points_B.push_back(new point(img_B._width, img_B._height));
    points_B.push_back(new point(0, img_B._height/2));
    points_B.push_back(new point(img_B._width/2, 0));
    points_B.push_back(new point(img_B._width, img_B._height / 2));
    points_B.push_back(new point(img_B._width/2, img_B._height));

    getDetectionPoints(points_A, img_A, filename_A);
    getDetectionPoints(points_B, img_B, filename_B);

    if (points_A.size() != points_B.size()) {
        throw ("the decetion points size are no the same");
    }
    // --- step 3 : get Delaunay triangles --- 
    vector<triangle*> triangles_A, triangles_B;
    getDelaunayTriangles(triangles_A, points_A);

    // get couterpart delaunay triangles of B from A
    for (int i = 0; i < triangles_A.size(); i++) {
        triangles_B.push_back(new triangle(*points_B.at(triangles_A[i]->index[0]),
                                *points_B.at(triangles_A[i]->index[1]),
                                *points_B.at(triangles_A[i]->index[2]),
                                triangles_A[i]->index[0],
                                triangles_A[i]->index[1],
                                triangles_A[i]->index[2]));
    }
    drawTriangles(triangles_A, img_A);
    drawTriangles(triangles_B, img_B);

    // --- step 4: morph ---
    double rate = 0;
    for (int i = 0; i <= 10; i++) {
        CImg<float> frame = morphTriangle(rate, triangles_A, triangles_B, img_A, img_B);
        rate += 0.1;
        string filename = "img/frame/.jpg";
        if (i < 10) {
            filename.insert(10, "0");
            filename.insert(11, std::to_string(i));
        }
        else {
            filename.insert(10, std::to_string(i));
        }

        frame.save(filename.c_str());
        cout << "saving frame " << i << endl;
    }
    return 0;
}


/*
* get dection points to vector<point> manually
* this function support to reuse the records we have record in fliename .txt
* if you want to detect the points by yourself just remove the txt file
* record method from outer loop in inner loop
* in this example 24 points each picture
*/
CImg<float> getDetectionPoints(vector<point*>& points, const CImg<float>& src, const string filename) {
    // 1.jpg to 1.txt which record the points
    string points_file(filename);
    points_file.replace(points_file.end() - 3, points_file.end(), "txt");
    CImg<float> dect_img = src;
    int color[] = { 255, 0, 0 };

    // try to read detection points from file
    ifstream input(points_file);
    ofstream output;
    if (input.fail()) { // if no exist points
        output.open(points_file);
        CImgDisplay disp(dect_img, filename.c_str());

        while (!disp.is_closed()) {
            disp.wait();
            if (disp.button() & 1 && disp.mouse_y() >= 0) {
                point* p = new point(disp.mouse_x(), disp.mouse_y());
                points.push_back(p);
                // draw circle
                dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
                dect_img.display(disp);
                // write file
                output << p->x << "," << p->y << endl;
            }
        }
        output.close();
    }
    else {
        string line;
        while (getline(input, line)) {
            // point format : x,y
            int pos = line.find(',');
            if (pos != string::npos) {
                // skip ","
                string x_str = line.substr(0, pos);
                string y_str = line.substr(pos + 1, string::npos);
                point* p = new point(std::stoi(x_str), std::stoi(y_str));
                points.push_back(p);
                // draw circle
                dect_img.draw_circle(p->x, p->y, dect_img._width / 100, color);
            }
        }
        //dect_img.display();
    }
    input.close();
    return dect_img;
}

double getDistance(point A, point B) {
    double dis = (A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y);
    return sqrt(dis);
}
// to judge is point p in the circumcircle of tri
// formular find in wikipedia Circumscribed circle
bool isPointInCircle(point* P, triangle* tri) {
    point origin(0, 0);
    double a_x = tri->a.x;
    double b_x = tri->b.x;
    double c_x = tri->c.x;
    double a_y = tri->a.y;
    double b_y = tri->b.y;
    double c_y = tri->c.y;
    double D = 2 * (a_x*(b_y-c_y) + b_x*(c_y-a_y) + c_x*(a_y-b_y));
    double x = ((a_x*a_x + a_y*a_y)*(b_y - c_y) + (b_x*b_x + b_y*b_y)*(c_y - a_y) + (c_x*c_x + c_y*c_y)*(a_y - b_y)) / D;
    double y = ((a_x*a_x + a_y*a_y)*(c_x - b_x) + (b_x*b_x + b_y*b_y)*(a_x - c_x) + (c_x*c_x + c_y*c_y)*(b_x - a_x)) / D;
    point center((int)x, (int)y);
    double radius = getDistance(center, tri->a);

    return (getDistance(*P, center) <= radius);
}
void getDelaunayTriangles(vector<triangle*>& triangles, const vector<point*>& points) {
    int size = points.size();
    if (size < 3) return;

    for (int i = 0; i < size - 2; i++) {
        for (int j = i + 1; j < size - 1; j++) {
            for (int k = j + 1; k < size; k++) {
                point* A = points[i];
                point* B = points[j];
                point* C = points[k];
                triangle* tri = new triangle(*A, *B, *C, i, j, k);

                bool no_exist = true;
                for (int m = 0; m < size; m++)
                {
                    point* P = points[m];
                    if (m == i || m == j || m == k) continue;
                    double k1 = (double)(A->y - B->y) / (A->x - B->x);
                    double k2 = (double)(A->y - C->y) / (A->x - C->x);
                    if (isPointInCircle(P, tri) || abs(k1-k2) < 0.1)
                    {
                        no_exist = false;
                        break;
                    }
                }
                if (no_exist)
                {
                    triangles.push_back(tri);
                }
            }
        }
    }

}
// draw triangle lines
void drawTriangles(const vector<triangle*>& triangles, const CImg<float>& src) {
    CImg<float> disp(src);
    int color[] = { 255, 0, 0 };
    for (int i = 0; i < triangles.size(); i++) {
        disp.draw_line(triangles[i]->a.x, triangles[i]->a.y,
            triangles[i]->b.x, triangles[i]->b.y, color);
        disp.draw_line(triangles[i]->a.x, triangles[i]->a.y,
            triangles[i]->c.x, triangles[i]->c.y, color);
        disp.draw_line(triangles[i]->b.x, triangles[i]->b.y,
            triangles[i]->c.x, triangles[i]->c.y, color);
        disp.draw_circle(triangles[i]->a.x, triangles[i]->a.y, disp._width / 100, color);
        disp.draw_circle(triangles[i]->b.x, triangles[i]->b.y, disp._width / 100, color);
        disp.draw_circle(triangles[i]->c.x, triangles[i]->c.y, disp._width / 100, color);
    }
    //disp.display();
}

// transition between source A and B
triangle* getTransitionTriangle(const triangle* A, const triangle* B, double rate) {
    int ax = (int)(rate*(A->a.x) + (1 - rate)*(B->a.x));
    int ay = (int)(rate*(A->a.y) + (1 - rate)*(B->a.y));
    int bx = (int)(rate*(A->b.x) + (1 - rate)*(B->b.x));
    int by = (int)(rate*(A->b.y) + (1 - rate)*(B->b.y));
    int cx = (int)(rate*(A->c.x) + (1 - rate)*(B->c.x));
    int cy = (int)(rate*(A->c.y) + (1 - rate)*(B->c.y));
    return new triangle(point(ax, ay), point(bx, by), point(cx, cy));
}
void getTransTriangle2Triangle(double(*H)[3], const triangle* src, const triangle* dst) {

    // transform src to dst
    int u0 = src->a.x;
    int v0 = src->a.y;
    int u1 = src->b.x;
    int v1 = src->b.y;
    int u2 = src->c.x;
    int v2 = src->c.y;

    int x0 = dst->a.x;
    int y0 = dst->a.y;
    int x1 = dst->b.x;
    int y1 = dst->b.y;
    int x2 = dst->c.x;
    int y2 = dst->c.y;

    // |A|
    int detA;
    detA = u0*v1 + u1*v2 + u2*v0 - u2*v1 - u0*v2 - u1*v0;
    // inv(A) * detA
    int A11, A12, A13, A21, A22, A23, A31, A32, A33;
    A11 = v1 - v2;
    A21 = -(v0 - v2);
    A31 = v0 - v1;
    A12 = -(u1 - u2);
    A22 = u0 - u2;
    A32 = -(u0 - u1);
    A13 = u1*v2 - u2*v1;
    A23 = -(u0*v2 - u2*v0);
    A33 = u0*v1 - u1*v0;

    H[0][0] = (double)(x0*A11 + x1*A21 + x2*A31) / detA;
    H[1][0] = (double)(y0*A11 + y1*A21 + y2*A31) / detA;
    H[2][0] = (double)(A11 + A21 + A31) / detA;
    H[0][1] = (double)(x0*A12 + x1*A22 + x2*A32) / detA;
    H[1][1] = (double)(y0*A12 + y1*A22 + y2*A32) / detA;
    H[2][1] = (double)(A12 + A22 + A32) / detA;
    H[0][2] = (double)(x0*A13 + x1*A23 + x2*A33) / detA;
    H[1][2] = (double)(y0*A13 + y1*A23 + y2*A33) / detA;
    H[2][2] = (double)(A13 + A23 + A33) / detA;
}

// url:http://www.yalewoo.com/in_triangle_test.html
int cross3(const point &a, const point &b, const point &p) {
    return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
}
bool triangle::isInTriangle(const point& p) {

    if (cross3(a, b, p) >= 0 && cross3(b, c, p) >= 0 && cross3(c, a, p) >= 0)
        return true;
    else if (cross3(a, b, p) <= 0 && cross3(b, c, p) <= 0 && cross3(c, a, p) <= 0)
        return true;
    else
        return false;
}
// morph Triangles
CImg<float> morphTriangle(double rate, const vector<triangle*>& triangles_A, const vector<triangle*>& triangles_B,
    const CImg<float>& img_A, const CImg<float>& img_B) {
    int width = (rate * img_A._width + (1 - rate) * img_B._width);
    int height = (rate * img_A._height + (1 - rate) * img_B._height);
    CImg<float> result(width, height, 1, 3);

    for (int i = 0; i < triangles_A.size(); i++) {
        triangle* trans_tri = getTransitionTriangle(triangles_A[i], triangles_B[i], rate);
        double H1[3][3], H2[3][3];
        getTransTriangle2Triangle(H1, trans_tri, triangles_A[i]);
        getTransTriangle2Triangle(H2, trans_tri, triangles_B[i]);
        cimg_forXY(result, x, y) {
            if (trans_tri->isInTriangle(point(x, y))) {

                float tx_a = x*H1[0][0] + y*H1[0][1] + H1[0][2];
                float ty_a = x*H1[1][0] + y*H1[1][1] + H1[1][2];
                float pixel_a[3] = { 0 };
                cimg_forC(img_A, c) {
                    pixel_a[c] = img_A.linear_atXY(tx_a, ty_a, 0, c);
                }

                float tx_b = x*H2[0][0] + y*H2[0][1] + H2[0][2];
                float ty_b = x*H2[1][0] + y*H2[1][1] + H2[1][2];
                float pixel_b[3] = { 0 };
                cimg_forC(img_B, c) {
                    pixel_b[c] = img_B.linear_atXY(tx_b, ty_b, 0, c);
                }

                // morph
                cimg_forC(result, c) {
                    result(x, y, 0, c) = rate*pixel_a[c] + (1 - rate)*pixel_b[c];
                }
            }

        }
    }

    return result;
}
