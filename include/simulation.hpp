//
// Created by USER on 15.12.2019.
//

#ifndef NETSIM_SIMULATION_HPP
#define NETSIM_SIMULATION_HPP

#include "factory.hpp"

void simulate(Factory& factory, TimeOffset rounds, std::function<void (Factory&, Time)> fun);

class IntervalReportNotifier{
private:
    TimeOffset dt_;
public:
    explicit IntervalReportNotifier(TimeOffset to): dt_(to) {};
    bool should_generate_report(Time t) const;
};

class SpecificTurnsReportNotifier{
private:
    std::set<Time> turns_;
public:
    explicit SpecificTurnsReportNotifier(std::set<Time> turns): turns_(std::move(turns)) {};
    bool should_generate_report(Time t) const;
};

#endif //NETSIM_SIMULATION_HPP
