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
    std::function<int(int)> func = [](int a) {
        std::printf("%d\n", a);
        std::printf("%d has completed an activity!\n", std::this_thread::get_id());
        return a;
    };
    for(int i = 0;i < activities.size();++i)
    {
        auto activity = CoreAsync::ITA_ActivityCreator::create(func,std::move(i));
        activities[i] = activity;
    }
}

void TA_ThreadPoolTest::TearDown()
{
    for(auto &activity : activities)
    {
        if(activity)
        {
            delete activity;
            activity = nullptr;
        }
    }
}

TEST_F(TA_ThreadPoolTest, postActivityTest)
{
    auto ft = m_threadPool.postActivity(activities[0]);
    EXPECT_EQ(0, ft.get().get<int>());
}

TEST_F(TA_ThreadPoolTest, sendActivityTest)
{
    auto ft = m_threadPool.sendActivity(activities[0]);
    EXPECT_EQ(0, ft.get().get<int>());
}

TEST_F(TA_ThreadPoolTest, threadSizeTest)
{
    EXPECT_EQ(std::thread::hardware_concurrency(), m_threadPool.size());
}

TEST_F(TA_ThreadPoolTest, notifyResultTest)
{
    std::vector<std::future<CoreAsync::TA_Variant>> testVec;
    std::vector<int> validVec(1024);
    for(int i = 0;i < activities.size();++i)
    {
        testVec.emplace_back(m_threadPool.postActivity(activities[i]));
        validVec[i] = i;
    }
    EXPECT_EQ(testVec.size(), validVec.size());
    for(int i = 0;i < testVec.size();++i)
    {
        EXPECT_EQ(testVec[i].get().get<int>(), validVec[i]);
    }
}
