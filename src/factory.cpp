//
// Created by USER on 15.01.2020.
//
#include "factory.hpp"

void Factory::do_deliveries(Time time){
    for(auto& elem: ramp_collection)
        elem.deliver_goods(time);
}

void Factory::do_work(Time time){
    for(auto& elem: worker_collection)
        elem.do_work(time);
}

void Factory::do_package_passing(){
    for(auto& elem: ramp_collection)
        elem.send_package();
    for(auto& elem: worker_collection)
        elem.send_package();
}

/*
    template<class RECEIVER_NODE> void Factory::remove_receiver(NodeColection<RECEIVER_NODE>& collection, ElementID id){
    if(collection.find_by_id(id) == collection.cend())
        return;
    for(auto& elem: ramp_collection){
        elem.receiver_preferences_.remove_receiver(&(*collection.find_by_id(id)));
    }
    for(auto& elem: this->worker_collection){
        elem.receiver_preferences_.remove_receiver(&(*collection.find_by_id(id)));
    }
    collection.remove_by_id(id);
}

 */

bool Factory::is_consistent()  const
{
    try {

        using PfrPair = ReceiverPreferences::preferences_t::value_type; // std::pair<PfrKey, PfrMapped>
        using PfrKey = ReceiverPreferences::preferences_t::key_type; // IPackageReceiver*
        using PfrMap = ReceiverPreferences::preferences_t;

        using Receiver = PfrKey;
        using ReceiverBranch = std::stack<Receiver>;
        using ReceiverTracking = std::pair<Receiver, ReceiverBranch>;

        using NCER = NotConsistentException::ExceptionReason;

        std::stack<ReceiverTracking> to_visit; // lifo
        std::set<Receiver> correct_receivers;
        std::set<Receiver> visited_receivers;
        std::vector<ReceiverTracking> branch_check_receivers;
        // kontener przechowujacy wspolny wezel (cyklow / sciezki i cyklu), do sprawdzenia poprawnosci
        // cykl jest poprawny gdy (wierzcholek wspolny ze sciezka ramp-storage / innym poprawnym cyklem) jest poprawny

        // EKSTRAKCJA RECEIVER'OW Z RAMP  const Ramp &ramp: ramp_collection
        for (auto iterator = ramp_collection.cbegin(); iterator != ramp_collection.cend(); iterator++) {
            std::set<Receiver> visited_receivers_ramp;

            if (iterator->receiver_preferences_.get_preferences().empty())
                throw NotConsistentException(NCER::MissingReceiver, iterator->get_id());
            for (const PfrPair &pair: iterator->receiver_preferences_.get_preferences()) {
                const Receiver cp_receiver = pair.first; // current_processed_receiver
                if (!is_set_containing(visited_receivers_ramp, cp_receiver)) // gdy nie ma wsrod juz wsadzonych
                {
                    ReceiverTracking cp_receiver_t = ReceiverTracking(cp_receiver, ReceiverBranch());
                    to_visit.emplace(cp_receiver_t);

                    visited_receivers_ramp.emplace(cp_receiver_t.first);
                }
            }
        }

        // USTAWIANIE STOREHOUSE'OW JAKO CORRECT const Storehouse &storehouse: storehouse_collection
        for (auto iterator = storehouse_collection.cbegin(); iterator != storehouse_collection.cend(); iterator++) {
            const auto correct_receiver = dynamic_cast<Receiver>(const_cast<Storehouse *>(&(*iterator)));
            visited_receivers.emplace(correct_receiver);
            correct_receivers.emplace(correct_receiver);
        }

        // ROZWIKLYWANIE GRAFU NA BRANCH'E
        while (!to_visit.empty()) {
            ReceiverTracking cp_receiver_t = to_visit.top();
            to_visit.pop();
            Receiver cp_receiver = cp_receiver_t.first;
            ReceiverBranch cp_receiver_b = cp_receiver_t.second;

            if (is_set_containing(visited_receivers, cp_receiver))
                branch_check_receivers.emplace_back(cp_receiver_t); // ten receiver jednoczesnie nalezy do innego branch'a
            else // ekstrakcja receiver'ow danego worker'a
            {
                const PfrMap &cp_receiver_map = dynamic_cast<Worker *>(cp_receiver)->receiver_preferences_.get_preferences();
                if (cp_receiver_map.empty())
                    throw NotConsistentException(NCER::MissingReceiver, cp_receiver->get_id());

                ReceiverBranch next_receiver_b = cp_receiver_b; // WP: kopia? <- Tak, ok
                next_receiver_b.emplace(cp_receiver);

                for (const PfrPair &next_receiver_p: cp_receiver_map) // ekstrakcja nastepnych receiver'ow
                {
                    Receiver next_receiver = next_receiver_p.first;
                    //if(next_receiver->get_receiver_type()!=ReceiverType::WORKER && next_receiver->get_receiver_type()!=ReceiverType::STOREHOUSE) {throw NotConsistentException(NCER::IncorrectReceiver, cp_receiver->get_id());}

                    ReceiverTracking next_receiver_t = ReceiverTracking(next_receiver, next_receiver_b);
                    to_visit.emplace(next_receiver_t);
                }
            }
            visited_receivers.emplace(cp_receiver);

        }

        // SPRAWDZANIE POPRAWNOSCI BRANCH'OW // to/do: upewnic sie ze branch'e powinny byc sprawdzane kolejno <- nieprawda
        while (!branch_check_receivers.empty()) {
            bool is_changed = false;

            for(auto branch_receiver_t = branch_check_receivers.begin(); branch_receiver_t < branch_check_receivers.end(); ++branch_receiver_t)
            {
                Receiver cp_receiver = branch_receiver_t->first;
                ReceiverBranch cp_receiver_b = branch_receiver_t->second;

                if (is_set_containing(correct_receivers, cp_receiver)) {
                    branch_check_receivers.erase(branch_receiver_t);
                    is_changed = true;

                    while (!cp_receiver_b.empty()) // ladujemy cala sciezke do correct_receivers
                    {
                        Receiver correct_receiver = cp_receiver_b.top();
                        cp_receiver_b.pop();

                        correct_receivers.emplace(correct_receiver);
                    }
                }
            }

            if (!is_changed) {
                ElementID some_id = branch_check_receivers.begin()->first->get_id();
                throw NotConsistentException(NCER::MissingReceiver, some_id);
            }
        }


        /*/ SZUKANIE NIEZALEZNYCH WORKER'OW
        for(const Worker& worker: worker_collection)
        {
            const auto receiver = dynamic_cast<Receiver>(const_cast<Worker*>(&worker));
            if(!is_set_containing(correct_receivers, receiver)) throw NotConsistentException(NCER::MissingReceiver, receiver->get_id());
        }
        */

        return true;
    }
    catch(NotConsistentException&)
    {
        return false;
    }

}