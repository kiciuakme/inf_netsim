//
// Created by USER on 15.12.2019.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

//nodes.hpp – klasy ujęte na diagramie UML w pakiecie Nodes
//typ wyliczeniowy ReceiverType
//klasy IPackageReceiver, Storehouse, ReceiverPreferences, PackageSender, Ramp, Worker
#include "package.hpp"
#include "storage_types.hpp"
#include <memory>
#include <list>
#include <map>
#include "helpers.hpp"
enum class ReceiverType {
    WORKER, STOREHOUSE //,RAMP
};

//class ReceiverPreferences;

class IPackageReceiver : public IPackageIterators {
public:
    virtual void receive_package(Package &&p) = 0;
    virtual ReceiverType get_receiver_type () const =0;
    virtual ElementID get_id() const = 0;
};

class ReceiverPreferences {                                 // Czy konstruktor jest dobry??? Jeszcze jak -ks
public:
    using preferences_t = std::map<IPackageReceiver *, double>;
    using rp_iterator = preferences_t::iterator;
    using rp_const_iterator = preferences_t::const_iterator;

    //ReceiverPreferences(ptr_foo function = [](){return 0.1;}) : list_of_receivers_(),function_(function) {}
    //ReceiverPreferences(ProbabilityGenerator probability_function = probability_generator): list_of_receivers_(),function_(probability_function) {}
    ReceiverPreferences(ProbabilityGenerator probability_function = probability_generator): function_(std::move(probability_function)) {}

    rp_iterator begin() { return list_of_receivers_.begin(); }
    rp_iterator end() { return list_of_receivers_.end(); }
    rp_const_iterator cbegin() const { return list_of_receivers_.cbegin(); }
    rp_const_iterator cend() const { return list_of_receivers_.cend(); }

    void calibrate_probability() {
        if(this->list_of_receivers_.empty())
            return;
        int size = list_of_receivers_.size();
        for (auto &it: list_of_receivers_) {
            it.second = 1.0 / size;
        }
        /*for (rp_const_iterator it = cbegin(); it != cend(); it++)
        {
            it -> second = 1/size;
        }*/
    }

    void add_receiver(IPackageReceiver *new_receiver, double probability) {
        list_of_receivers_[new_receiver] = probability;
        this->calibrate_probability();
    }
    void add_receiver(IPackageReceiver *new_receiver) {
        //list_of_receivers_[new_receiver];
        list_of_receivers_[new_receiver]=1; //-1 dlatego że tura w której zaczyna się przetwarzać też się wlicza do czasu przetwarzania
        this->calibrate_probability();
    }
    void remove_receiver(IPackageReceiver *receiver_to_delete) {
        list_of_receivers_.erase(receiver_to_delete);
        this->calibrate_probability();
    }

    IPackageReceiver *choose_receiver() {
        double chance = function_();
        double sum = 0;
        double previus = 0;
        if (this->list_of_receivers_.size()>0) {
            for (rp_const_iterator it = cbegin(); it != cend(); it++) {
                previus = sum;
                sum += it->second;
                if (previus < chance && chance <= sum) {
                    return it->first;
                }
            }
        }
        return nullptr;
    }

    preferences_t &get_preferences() {
        return list_of_receivers_;
    }

    //const preferences_t& get_preferences() const { return preferences_; } - skopiowane z maila
    const preferences_t& get_preferences() const { return list_of_receivers_; }
protected:
    preferences_t list_of_receivers_;
    ProbabilityGenerator function_;
};

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&&)=default;


    ReceiverPreferences receiver_preferences_;

    std::optional<Package> &get_sending_buffer() {
        return this->sending_buffer;
    };

    void send_package() {
        if (this->sending_buffer) {
            (this->receiver_preferences_.choose_receiver())->receive_package(std::move(this->sending_buffer.value()));
        }
        sending_buffer.reset();
    }

protected:
    void push_package(Package element) {
        sending_buffer.emplace(element);
    }

private:
    std::optional<Package> sending_buffer;
};

/*
class PackageSender
{
private:
    std::optional<Package> sender_buffor;
public: // protected? -kc
    void push_package(Package sb) {sender_buffor = std::move(sb);}
    void send_package()
    {
        // todo -kc
        // jak bedzie receiverpreferences
    }
    std::optional<Package> get_sending_buffer() {return sender_buffor;}
};
 */

class Storehouse : public IPackageReceiver {
public:
    //explicit  Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<IPackageStockpile>(PQFifo())) : id_(id), d_(std::move(d)) {};
    explicit Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueue(PackageQueueType::LIFO))): id_(id),  d_(std::move(d)) {};
    void receive_package(Package &&p) override { d_.get()->push(std::move(p)); }

    IPackageIterators::const_iterator cbegin() const override { return d_.get()->cbegin(); }
    IPackageIterators::const_iterator cend() const override { return d_.get()->cend(); }
    //IPackageIterators::iterator begin() override { return d_.get()->begin(); }
    //IPackageIterators::iterator end() override { return d_.get()->end(); }
    // iteratory są rozwikływane ze względu na przekazaną kolejkę lifo/fifo -kc

    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override {return rt_;}

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
    constexpr static ReceiverType rt_ = ReceiverType::STOREHOUSE;

};

class Worker : public PackageSender, public IPackageReceiver {
private:
    static constexpr ReceiverType rt_ = ReceiverType::WORKER;
    ElementID id_;
    TimeOffset processing_duration;
    std::unique_ptr<IPackageQueue> d_;

    Time processing_start_time = 0;
    std::optional<Package> buffor;
public:
    explicit Worker(const ElementID element_id,
                    const TimeOffset processing_duration_,
                    std::unique_ptr<IPackageQueue> pcontainer)
            : id_(element_id),
              processing_duration(processing_duration_),
              d_(std::move(pcontainer)) {}

    //Worker(Worker&) = default;
    //Worker(const Worker&) const = default;
    //Worker(Worker&&) = default;
    //Worker(const Worker&& const) = default;

    IPackageIterators::const_iterator cbegin() const override { return d_.get()->cbegin(); }
    IPackageIterators::const_iterator cend() const override { return d_.get()->cend(); }
    //IPackageIterators::iterator begin() override { return d_.get()->begin(); }
    //IPackageIterators::iterator end() override { return d_.get()->end(); }

    void receive_package(Package &&p) override { d_->push(std::move(p)); }

    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override {return rt_;}

    TimeOffset get_processing_duration() const { return processing_duration; }
    Time get_package_processing_start_time() const { return processing_start_time; }

    void do_work(Time current_time) {
        if (buffor.has_value() && current_time - processing_start_time == processing_duration) {
            push_package(buffor.value());
            buffor.reset();
        }
        if (!buffor.has_value())
            if (!d_->empty()) {
                buffor = d_->pop();
                //processing_start_time = current_time;
                processing_start_time = current_time - 1;

            }
        //if(buffor.has_value() && current_time - processing_start_time < processing_duration)
    };

};

class Ramp : public PackageSender {
private:
    ElementID id_;
    TimeOffset delivery_interval;
public:
    explicit Ramp(ElementID id, TimeOffset di)
            : id_(id), delivery_interval(di) {}

    void deliver_goods(Time t) {
        if (!((t-1) % delivery_interval)) {
            Package pkg = Package();
            push_package(pkg);
        }
    }

    TimeOffset get_delivery_interval() const { return delivery_interval; }

    ElementID get_id() const { return id_; }
};

#endif //NETSIM_NODES_HPP
