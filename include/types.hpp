//
// Created by USER on 11.12.2019.
//

#ifndef NETSIM_TYPES_HPP
#define NETSIM_TYPES_HPP

#include <functional>

using ElementID = std::size_t;
using container_size_type = std::size_t;

using Time = int;
using TimeOffset = int;

using ProbabilityGenerator = std::function<double()>;
//plik nagłówkowy "types.hpp" zawierający definicję aliasy ElementID

#endif //NETSIM_TYPES_HPP
