#ifndef TA_THREADPOOLTEST_H
#define TA_THREADPOOLTEST_H

#include "gtest/gtest.h"
#include "Components/TA_ThreadPool.h"

class TA_ThreadPoolTest : public :: testing :: Test
{
public:
    TA_ThreadPoolTest();
    ~TA_ThreadPoolTest();

    void SetUp() override;
    void TearDown() override;

    CoreAsync::TA_ThreadPool m_threadPool;

};

#endif // TA_THREADPOOLTEST_H
