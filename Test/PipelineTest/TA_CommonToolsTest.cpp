#include "TA_CommonToolsTest.h"
#include "Components/TA_CommonTools.h"

TA_CommonToolsTest::TA_CommonToolsTest()
{

}

TA_CommonToolsTest::~TA_CommonToolsTest()
{

}

void TA_CommonToolsTest::SetUp()
{

}

void TA_CommonToolsTest::TearDown()
{

}

TEST_F(TA_CommonToolsTest, decToBinTest)
{
    std::string res = CoreAsync::TA_CommonTools::decimalToBinary(1991817);
    EXPECT_EQ(res, "111100110010010001001");
}
