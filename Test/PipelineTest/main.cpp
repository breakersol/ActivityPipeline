#include "gtest/gtest.h"
#include "MetaTest.h"
#include "Components/TA_ThreadPool.h"
#include "Cu/TestCuda.h"

#include <algorithm>

int main(int argc, char *argv[])
{
//    ::testing::InitGoogleTest(&argc,argv);

//    return RUN_ALL_TESTS();

    Matrix ma, mb ,mc;

    ma.height = 2;
    ma.width = 2;
    ma.stride = 2;
    ma.elements = new float[ma.stride * ma.height]();

    std::fill(ma.elements, ma.elements + ma.stride * ma.height, 2.0);

    mb.height = 2;
    mb.width = 2;
    mb.stride = 2;
    mb.elements = new float[mb.stride * mb.height]();

    std::fill(mb.elements, mb.elements + mb.stride * mb.height, 3.0);

    mc.height = 2;
    mc.width = 2;
    mc.stride = 2;
    mc.elements = new float[mc.stride * mc.height]();

    std::fill(mc.elements, mc.elements + mc.stride * mc.height, 0.0); 

    test(ma,mb,mc);

    std::cout << *(mc.elements) << std::endl;

    for(int r = 0;r < mc.height;++r)
    {
        for(int c = 0;c < mc.width;++c)
        {
            std::cout << "r/c: " << r << "/" << c << " " << *(mc.elements + r * mc.stride + c) << std::endl;
        }
    }

    return 0;

}
