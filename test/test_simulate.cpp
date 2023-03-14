#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "nodes.hpp"
#include "simulation.hpp"
#include "helpers.hpp"
#include "reports.hpp"

using ::testing::Return;
using ::testing::_;

//Te dwa testy sprawdzają czy zawartość kolejki jest usuwana poprawnie. Testy przechodzą czyli tak jest
TEST(SimulationTest, first_queue_test) {
    PackageQueue q(PackageQueueType::FIFO);
    q.push(Package());

    EXPECT_EQ(q.cbegin()->get_id(), 1);

}

TEST(SimulationTest, second_queue_test) {
    PackageQueue q(PackageQueueType::FIFO);
    q.push(Package());

    EXPECT_EQ(q.cbegin()->get_id(), 1);

}

//Te dwa testy sprawdzają czy jak kolejka jest w unique_ptr to też jest usuwana poprawnie. Nie jest
TEST(SimulationTest, first_pointer_test) {
    std::unique_ptr<IPackageStockpile> q_ = std::make_unique<PackageQueue>(PackageQueue(PackageQueueType::LIFO));
    q_->push(Package());

    EXPECT_EQ(q_->cbegin()->get_id(), 1);
}

TEST(SimulationTest, second_pointer_test) {
    std::unique_ptr<IPackageStockpile> q_ = std::make_unique<PackageQueue>(PackageQueue(PackageQueueType::LIFO));
    q_->push(Package());

    EXPECT_EQ(q_->cbegin()->get_id(), 1);
}

TEST(SimulationTest, first_node_test) {
    Ramp r_(1,1);
    Worker w_(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO));

    r_.receiver_preferences_.add_receiver(&w_);
    r_.deliver_goods(1);
    r_.send_package();

    EXPECT_EQ(1, w_.get_queue()->cbegin()->get_id());
}

TEST(SimulationTest, second_node_test) {
    Ramp r_(1,1);
    Worker w_(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO));

    r_.receiver_preferences_.add_receiver(&w_);
    r_.deliver_goods(1);
    r_.send_package();

    EXPECT_EQ(1, w_.get_queue()->cbegin()->get_id());
}


TEST(SimulationTest, Simulate) {

    Factory factory;
    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    ReceiverPreferences prefs;

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    simulate(factory, 3, [](Factory&, TimeOffset) {});

    // Robotnik ma pustą kolejkę i bufor.
    EXPECT_EQ(w.cbegin(), w.cend());
    EXPECT_FALSE(w.get_sending_buffer().has_value());

    // Magazyn zawiera półprodukt.
    auto storehouse_it = factory.storehouse_cbegin();

    ASSERT_NE(storehouse_it->cbegin(), storehouse_it->cend());
    EXPECT_EQ(storehouse_it->cbegin()->get_id(), 1);
}
