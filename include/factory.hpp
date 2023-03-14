//
// Created by USER on 15.12.2019.
//

//factory.hpp
//szablon klasy NodeCollection
//klasa Factory
//funkcje do zapisu i odczytu struktury fabryki z pliku

#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include <algorithm>

#include "types.hpp"
#include "nodes.hpp"
#include <set>
#include <stack>
#include "helpers.hpp"
#include <sstream>

template<typename NODE>
class NodeColection
{
private:
    using container_t = typename std::list<NODE>;
    container_t container;

public:
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    iterator begin(){return container.begin();}
    iterator end(){ return container.end();}
    const_iterator cbegin() const {return container.cbegin();}
    const_iterator cend() const {return container.cend();}

    void add(NODE&& node)
    {
        container.push_back(std::move(node));
    }
    void remove_by_id(ElementID id_)
    {
        auto it = std::find_if(begin(),end(),[id_](auto& elem){ return elem.get_id() == id_ ;});
        if (it == end())
        {
            //TO/DO Czy trzeba rzucać wyjątkiem? Sprawdzić w konspekcie <- Kleczke stwierdzil ze nie -ks
        }
        else
        {
            container.erase(it);
        }
    }//std::find_if(this->begin(), this->end(), [&](auto& elem){return elem.get_id() == id;})
    iterator find_by_id(ElementID id_)
    {
        return std::find_if(begin(),end(), [id_](auto& elem){ return elem.get_id() == id_ ;});
    }
    const_iterator find_by_id(ElementID id_) const
    {
        return std::find_if(cbegin(),cend(), [id_](const auto& elem){ return elem.get_id() == id_ ;});
    }
};


//class Factory;
//template<class NODE> NodeColection<NODE>& get_collection(Factory& factory);

class Factory
{
private:
    NodeColection<Ramp> ramp_collection;
    NodeColection<Worker> worker_collection;
    NodeColection<Storehouse> storehouse_collection;

    template <typename NODE>
    void remove_receiver(NodeColection<NODE>& collection_, ElementID id)
    {
        if(collection_.find_by_id(id) == collection_.end()) return;
        for(auto& elem: ramp_collection){
            elem.receiver_preferences_.remove_receiver(&(*collection_.find_by_id(id)));
        }
        for(auto& elem: worker_collection){
            elem.receiver_preferences_.remove_receiver(&(*collection_.find_by_id(id)));
        }
        collection_.remove_by_id(id);
    }

public:
    class NotConsistentException : public std::exception
    {
        /**
            Definicja: Spójność węzła nadawcy

            Dany węzeł nadawcy (rampa/robotnik) jest spójny, gdy:
                *posiada choć jednego zdefiniowanego odbiorcę (w przypadku robotników – nie będącego samym sobą) oraz
                *każdy z jego odbiorców
                    *albo jest magazynem,
                    *albo jest innym spójnym węzłem nadawcy.
         **/
    public:
        enum ExceptionReason
        {
            MissingReceiver,
            IncorrectReceiver,
            //UnknownReceiverType
        };

        ExceptionReason er;
        ElementID id;

        explicit NotConsistentException(ExceptionReason er, ElementID id): er(er), id(id) {}
    };
    bool is_consistent() const;
    void do_deliveries(Time time);
    void do_package_passing();
    void do_work(Time time);

public: // Realizacja pecyfikacji na modłę Kłeczka
    void add_ramp(Ramp&& ramp){ramp_collection.add(std::move(ramp));}
    void remove_ramp(ElementID id) {ramp_collection.remove_by_id(id);}
    NodeColection<Ramp>::iterator find_ramp_by_id(ElementID id) {return ramp_collection.find_by_id(id);}
    NodeColection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {return ramp_collection.find_by_id(id);}
    NodeColection<Ramp>::const_iterator ramp_cbegin() const {return ramp_collection.cbegin();}
    NodeColection<Ramp>::const_iterator ramp_cend() const {return ramp_collection.cend();}


    void add_worker(Worker&& worker) {return worker_collection.add(std::move(worker));}
    void remove_worker(ElementID id) {this->remove_receiver(worker_collection, id);}
    NodeColection<Worker>::iterator find_worker_by_id(ElementID id) {return worker_collection.find_by_id(id);}
    NodeColection<Worker>::const_iterator find_worker_by_id(ElementID id) const {return worker_collection.find_by_id(id);}
    NodeColection<Worker>::const_iterator worker_cbegin() const {return worker_collection.cbegin();}
    NodeColection<Worker>::const_iterator worker_cend() const {return worker_collection.cend();}


    void add_storehouse(Storehouse&& storehouse) {return storehouse_collection.add(std::move(storehouse));}
    void remove_storehouse(ElementID id) {this->remove_receiver(storehouse_collection, id);}
    NodeColection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {return storehouse_collection.find_by_id(id);}
    NodeColection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {return storehouse_collection.find_by_id(id);}
    NodeColection<Storehouse>::const_iterator storehouse_cbegin() const {return storehouse_collection.cbegin();}
    NodeColection<Storehouse>::const_iterator storehouse_cend() const {return storehouse_collection.cend();}


};

Factory load_factory_structure(std::istringstream iss)
{
    Factory fac;
    // "LOADING_RAMP id=1 delivery-interval=3"
    auto r = Ramp(1, 3);
    auto a = iss;
    fac.add_ramp(std::move(r));
    return fac;
}

Factory




#endif //NETSIM_FACTORY_HPP

/*
template<> NodeColection<Ramp>& get_collection(Factory& factory){return factory.ramp_collection;}
template<> NodeColection<Worker>& get_collection(Factory& factory){return factory.worker_collection;}
template<> NodeColection<Storehouse>& get_collection(Factory& factory){return factory.storehouse_collection;}

 * template<class NODE> friend NodeColection<NODE>& get_collection(Factory& factory);
private:
    template<class NODE> void add(NODE&& node){get_collection<NODE>(this).add(node);}
    template<class NODE> void remove(ElementID id){get_collection<NODE>(this).remove_by_id((id));}
        // todo::wymaga specjalizacji pod remove_receiver
    template<class NODE> typename NodeColection<NODE>::iterator find_by_id(ElementID id){return get_collection<NODE>(this).find_by_id(id);}
    template<class NODE> typename NodeColection<NODE>::const_iterator find_by_id(ElementID id) const {return get_collection<NODE>(this).find_by_id(id);}
    template<class NODE> typename NodeColection<NODE>::const_iterator cbegin() const {return get_collection<NODE>(this).cbegin();}
    template<class NODE> typename NodeColection<NODE>::const_iterator cend() const {return get_collection<NODE>(this).cend();}
 *
    //template<class NODE> const NodeColection<NODE>& get_collection(const Factory& factory);
    //template<> const NodeColection<Ramp>& get_collection(const Factory& factory) return factory.ramp_collection;}
    //template<> const NodeColection<Worker>& get_collection(const Factory& factory){return factory.worker_collection;}
    //template<> const NodeColection<Storehouse>& get_collection(const Factory& factory){return factory.storehouse_collection;}

}

    template<class NODE> NodeColection<NODE>& get_collection() {}
    template<> NodeColection<Ramp>& get_collection(){return ramp_collection;}
    template<> NodeColection<Worker>& get_collection(){return worker_collection;}
    template<> NodeColection<Storehouse>& get_collection(){return storehouse_collection;}

    template<class NODE> const NodeColection<NODE>& get_collection() const {}
    template<> const NodeColection<Ramp>& get_collection() const {return ramp_collection;}
    template<> const NodeColection<Worker>& get_collection() const {return worker_collection;}
    template<> const NodeColection<Storehouse>& get_collection() const {return storehouse_collection;}


    template<class RECEIVER_NODE> void remove_receiver(NodeColection<RECEIVER_NODE>& collection, ElementID id);
 */