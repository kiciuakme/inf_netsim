#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
//#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"

using ::std::cout;
using ::std::endl;

TEST(PackageQueueTest, IsFifoCorrect) {
    PackageQueue q(PackageQueueType::FIFO);
    q.push(Package());
    q.push(Package());

    Package p(std::move(q.pop()));
    EXPECT_EQ(p.get_id(), 1);

    p = q.pop();
    EXPECT_EQ(p.get_id(), 2);
}

TEST(PackageQueueTest, IsLifoCorrect) {
    PackageQueue q(PackageQueueType::LIFO);
    q.push(Package());
    q.push(Package());

    Package p(std::move(q.pop()));
    EXPECT_EQ(p.get_id(), 2);

    p = q.pop();
    EXPECT_EQ(p.get_id(), 1);
}
