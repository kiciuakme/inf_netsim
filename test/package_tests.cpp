//
// Created by USER on 16.12.2019.
//
#include "gtest/gtest.h"
#include "package.hpp"

TEST(PackageTests, CreatePackage){
    Package p;
    Package r;
    Package s;
    EXPECT_EQ(p.get_id(), 1);
    EXPECT_EQ(r.get_id(), 2);
    EXPECT_EQ(s.get_id(), 3);
}

TEST(PackageTests, MovePackage){
    Package q;
    auto id_ = q.get_id();
    auto g = Package(std::move(q));
    EXPECT_EQ(g.get_id(), id_);
}

TEST(PackageTests, NextPackages){
    Package::freed_IDs.insert(5);
    Package::freed_IDs.insert(70);
    Package b;
    Package c;
    Package d;
    EXPECT_EQ(b.get_id(), 5);
    EXPECT_EQ(c.get_id(), 70);
    EXPECT_EQ(d.get_id(), 71);
}