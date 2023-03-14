//
// Created by USER on 16.01.2020.
//

#include "simulation.hpp"


bool IntervalReportNotifier::should_generate_report(Time t) const {
    return t%dt_==0;
}

bool SpecificTurnsReportNotifier::should_generate_report(Time t) const {
    auto it = turns_.find(t);
    return (it != turns_.end());
}

void simulate(Factory& factory, TimeOffset rounds, std::function<void (Factory&, Time)> fun){
    for(TimeOffset time=1; time<=rounds; time++){
        factory.do_deliveries(time);    //dostawa
        factory.do_package_passing();   //przekazanie
        factory.do_work(time);          //przetworzenie
        fun(factory, time);             //raportowanie
    }
}