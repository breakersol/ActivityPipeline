#include "gtest/gtest.h"
#include "MetaTest.h"
#include "Components/TA_ThreadPool.h"

#include "Cu/TestCuda.cu"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc,argv);

    return RUN_ALL_TESTS();
}
