#include "stdafx.h"
#include "hough.h"
#include "imageWrapping.h"

#include <string>
using std::string;
int main() {
    vector<string> filename = { "Dataset/test1.jpg","Dataset/test2.jpg" };
    int test_para[][4] = { 2, 500, 200, 30,
                        3, 500, 250, 40 };
    for (int i = 0; i < filename.size(); i++) {
        CImg<double> file(filename[i].c_str());
        hough hough_runner(file);
        
        hough_runner.set_blur_sigma(test_para[i][0]);
        hough_runner.set_threshold('h', test_para[i][1]);
        hough_runner.set_threshold('d', test_para[i][2]);
        hough_runner.set_threshold('g', test_para[i][3]);
        
        /*
        hough_runner.get_rgb2gray().display();
        hough_runner.get_gradient().display();
        hough_runner.get_hough_space().display();
        */
        CImg<double> img = hough_runner.get_draw_lines();
        img.display();
        reverse_wrapping(img, hough_runner.get_intersection()).display();

    }
    return 0;
}
