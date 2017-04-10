// colorTransfer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CImg.h"
#include <string>
#include <cmath>
#include <iostream>
#include <direct.h>
using namespace cimg_library;

void colorTransfer(const CImg<double>&, CImg<double>&);

int main() {
    // format testi_src.jpg
    std::string src_filename;
    std::string dst_filename;
    std::string result_filename;
    if (_access("/img/result",0) == -1) mkdir("img/result");
    std::cout << "running..." << std::endl;
    for (int i = 1; i <= 10; i++) {
        src_filename = "img/test_src.png";
        dst_filename = "img/test_dst.png";
        result_filename = "img/result/test_result.bmp";
        src_filename.insert(8, std::to_string(i));
        dst_filename.insert(8, std::to_string(i));
        result_filename.insert(15, std::to_string(i));
        CImg<double> src(src_filename.c_str());
        CImg<double> dst(dst_filename.c_str());
        colorTransfer(src, dst);
        dst.save(result_filename.c_str());
    }
    std::cout << "save result successfully..." << std::endl;
    return 0;
}

void colorTransfer(const CImg<double>& src, CImg<double>& dst) {
    // rgb to lab
    CImg<double> labsrc = src.get_RGBtoLab();
    CImg<double> labdst = dst.get_RGBtoLab();

    // compute mean and standar variance of each channel
    double meansrc[3], stdsrc[3], meandst[3], stddst[3];
    for (int i = 0; i < 3; i++) {
        stdsrc[i] = sqrt(labsrc.get_channel(i).variance_mean(1, meansrc[i]));
        stddst[i] = sqrt(labdst.get_channel(i).variance_mean(1, meandst[i]));
    }
    // convert dst's mean and std to src's
    cimg_forXYC(labdst, x, y, c) {
        labdst(x, y, c) -= meandst[c];                           // dst substracts mean of each its each channel 
        labdst(x, y, c) = stdsrc[c] / stddst[c] * labdst(x,y,c); // dst convert its std to src's std
        labdst(x, y, c) += meansrc[c];                           // dst adds mean of src's each channel
    }
    // lab to rgb
    dst = labdst.get_LabtoRGB();
}