//
// Created by USER on 15.12.2019.
//

//helpers.hpp – pomocnicze obiekty i funkcje globalne (np. generator liczb losowych)
#ifndef NETSIM_HELPERS_HPP
#define NETSIM_HELPERS_HPP

#include <cstdlib>
#include <random>
#include <ctime>
#include <functional>
#include <random>
#include "types.hpp"
#include <set>

extern std::random_device rd;
extern std::mt19937 rng;

double default_probability_generator();

extern ProbabilityGenerator probability_generator;

template<typename TYPE> bool is_set_containing(const std::set<TYPE>& set, const TYPE& element){return set.find(element) != set.cend();}

#endif //NETSIM_HELPERS_HPP
