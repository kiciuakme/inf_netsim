//
// Created by USER on 11.12.2019.
//

#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

//#include <iostream>
#include <set>
#include "types.hpp"


class Package {
public:
    Package();

    explicit Package(ElementID id);

    Package(const Package&) = default;
    Package(Package&&) noexcept;

    Package& operator=(Package&) = delete;
    Package& operator=(Package&&) noexcept;

    ~Package();

    ElementID get_id() const { return id_; }

    // Pula identyfikatorów aktualnie przyporządkowanych półproduktom.
    inline static std::set<ElementID> assigned_IDs{0};

    // Pula identyfikatorów, które zostały zwolnione w wyniku usuwania półproduktów.
    inline static std::set<ElementID> freed_IDs;

private:
    static const ElementID BLANK_ID = -1;

    /*
     * Począwszy od C++17 pola statyczne mogą być oznaczane jako `inline`,
     * dzięki czemu moża je inicjalizować wewnątrz definicji klasy (a nie
     * w osobnym pliku źródłowym).
     */



    ElementID id_ = BLANK_ID;
};

#endif //NETSIM_PACKAGE_HPP
