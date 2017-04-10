#include "stdafx.h"
#include "hough.h"
#include <string>
using std::string;
int main() {
    vector<string> filename = { "Dataset/test1.jpg","Dataset/test2.jpg" };
    int test_para[2][4] = { 3, 900, 500, 70,
                        2, 650, 250, 45 };
    for (int i = 0; i < filename.size(); i++) {
        CImg<double> file(filename[i].c_str());
        hough hough_runner(file);
        hough_runner.set_blur_sigma(test_para[i][0]);
        hough_runner.set_threshold('h', test_para[i][1]);
        hough_runner.set_threshold('d', test_para[i][2]);
        hough_runner.set_threshold('g', test_para[i][3]);
        hough_runner.get_rgb2gray().display();
        hough_runner.get_gradient().display();
        hough_runner.get_hough_space().display();
        hough_runner.get_draw_lines().display();
    }
    return 0;
}
