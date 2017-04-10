#include "stdafx.h"
#include "histEqn.h"

// get the histogram of the pixel values in a pthotos
// pixel values range from 0 ~ 255
vector<int> histEqn::getImageHist(const CImg<int>& img) {
    vector<int> hist(256, 0);
    cimg_forXY(img, x, y) {
        hist.at(img(x, y))++;
    }
    return hist;
}

// perform histogram equalization
// allow gray image and RGB image
// RGB image will operate on y channel
void histEqn::histEqualization(CImg<int>& img) {
    if (img._spectrum == 1) {
        try {
            histEqualizationGray(img);
        }
        catch (const char* &e) {
            std::cout << e << std::endl;
            throw(e);
        }
    }
    else if (img._spectrum == 3) {
        try {
            histEqualizationRGB(img);
        }
        catch (const char* &e) {
            std::cout << e << std::endl;
            throw(e);
        }
    }
    else {
        throw("Error channel number");
    }
}

// operate RGB image
void histEqn::histEqualizationRGB(CImg<int>& img) {
    if (img._spectrum != 3) {
        throw("RGB Error channel number");
    }

    // rgb to yuv
    CImg<int> img_yuv = img;
    int r, g, b;
    cimg_forXY(img_yuv,x,y){
        r = img(x, y, 0);
        g = img(x, y, 1);
        b = img(x, y, 2);
        img_yuv(x, y, 0) = 0.299*r + 0.587*g + 0.114*b;
        img_yuv(x, y, 1) = -0.169*r - 0.331*g + 0.5*b + 128;
        img_yuv(x, y, 2) = 0.5*r - 0.419*g - 0.081*b + 128;
    }
    // y is luminance
    CImg<int> y_channel = img_yuv.get_channel(0);
    try {
        histEqualizationGray(y_channel);
    }
    catch (const char* &e) {
        std::cout << e << std::endl;
        throw(e);
    }

    // yuv to rgb   
    int y, u, v;
    cimg_forXY(img_yuv, i, j) {
        y = y_channel(i, j);
        u = img_yuv(i, j, 1);
        v = img_yuv(i, j, 2);
        img(i, j, 0) = y + 1.13983 * (v - 128);
        img(i, j, 1) = y - 0.39465 * (u - 128) - 0.58060 * (v - 128);
        img(i, j, 2) = y + 2.03211 * (u - 128);
    }
}
// operate gray image
void histEqn::histEqualizationGray(CImg<int>& img) {

    if (img._spectrum != 1) {
        throw("Gray Error channel number");
    }

    vector<int> hist, mapping;
    hist = getImageHist(img);
    int pixel_num = img._height * img._width;
    double cummulate_sum = 0;
    int value_mapped;
    // cdf
    for (auto i = hist.begin() ; i != hist.end(); i++) {
        cummulate_sum += *i;
        value_mapped = round(255*cummulate_sum/pixel_num);
        mapping.push_back(value_mapped);
    }
    // mapping
    cimg_forXY(img, x, y) {
        img(x,y) = mapping.at(img(x,y));
    }
}
