#include "stdafx.h"
#include "histEqn.h"

int main() {
    std::vector<char*> list = { "img/RGB_test1.jpg",
                                "img/RGB_test2.jpg",
                                "img/RGB_test3.jpg",
                                "img/RGB_test4.jpg",
                                "img/RGB_test5.jpg",
                                "img/gray_test1.tif",
                                "img/gray_test2.tif",
                                "img/gray_test3.tif",
                                "img/gray_test4.tif",
                                "img/gray_test5.tif"};
    histEqn histEqnC = histEqn::getInstance();
    for (auto i : list) {
        char* filename = i;
        CImg<int> test(filename);
        test.display();
        try {
            histEqnC.histEqualization(test);
        }
        catch (const char* &e) {
            std::cout << e << std::endl;
        }
        test.display();
    }
    return 0;
}
