#include "TA_ThreadPoolTest.h"
#include "ITA_ActivityCreator.h"

TA_ThreadPoolTest::TA_ThreadPoolTest()
{

}

TA_ThreadPoolTest::~TA_ThreadPoolTest()
{

}

void TA_ThreadPoolTest::SetUp()
{

}

void TA_ThreadPoolTest::TearDown()
{

}

TEST_F(TA_ThreadPoolTest, postActivityTest)
{
    std::function<void(int)> func = [](int a) {std::printf("%d\n", a); std::printf("%d has completed an activity!\n", std::this_thread::get_id());};
    for(int i = 0;i < 10000;++i)
    {
        auto activity = CoreAsync::ITA_ActivityCreator::create(func,std::move(i));
        m_threadPool.postActivity(activity);
    }
}

TEST_F(TA_ThreadPoolTest, sendActivityTest)
{
    std::function<void(int)> func = [](int a) {std::printf("%d\n", a); std::printf("%d has completed an activity!\n", std::this_thread::get_id());};
    for(int i = 0;i < 1024;++i)
    {
        auto activity = CoreAsync::ITA_ActivityCreator::create(func,std::move(i));
        m_threadPool.sendActivity(activity);
    }
}

TEST_F(TA_ThreadPoolTest, threadSizeTest)
{
    EXPECT_EQ(std::thread::hardware_concurrency(), m_threadPool.size());
}
