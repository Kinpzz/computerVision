#pragma once
using std::vector;
using namespace cimg_library;
#ifndef HISTEQN
#define HISTEQN
// singleton using inside static
class histEqn {
public:
    static histEqn& getInstance() {
        static histEqn instance;
        return instance;
    }
    vector<int> getImageHist(const CImg<int> &);
    void histEqualization(CImg<int>&);
    void histEqualizationRGB(CImg<int>&);
    void histEqualizationGray(CImg<int>&);
private:
    histEqn() {}
};
#endif