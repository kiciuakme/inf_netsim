//
// Created by USER on 11.12.2019.
//

#ifndef NETSIM_STORAGE_TYPES_HPP
#define NETSIM_STORAGE_TYPES_HPP

//plik nagłówkowy "storage_types.hpp"
// zawierający definicje klas IPackageStockpile,
// IPackageQueue i PackageQueue oraz typu wyliczeniowego PackageQueueType

//#include <deque>
#include "package.hpp"
#include "types.hpp"
#include <list>

/*enum PackageQueueType
{
    lifo,
    fifo
}; // todo: ok, bez znacznikow?
 */

enum class PackageQueueType
{
    LIFO,
    FIFO
};

class IPackageIterators
{
public:
    using iterator = std::list<Package>::iterator;
    using const_iterator = std::list<Package>::const_iterator;
    //virtual iterator begin() = 0;
    //virtual iterator end() = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
};

class IPackageStockpile : public IPackageIterators
{
public:
    virtual void push(Package&& pkg) = 0;
    virtual bool empty() const = 0; // rnm: is_empty()
    virtual container_size_type size() const = 0;
};

class IPackageQueue: public IPackageStockpile
{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue: public IPackageQueue

{
    using iterator = std::list<Package>::iterator;
protected:
    std::list<Package> container;
    PackageQueueType queue_type;

public:
    explicit PackageQueue(PackageQueueType type) : queue_type(type){};

    //iterator begin() override {return container.begin();}
    //iterator end() override {return container.end();}
    const_iterator cbegin() const override {return container.cbegin();}
    const_iterator cend() const override {return container.cend();}

    Package pop() override;

    container_size_type size() const override {return container.size();}
    bool empty() const override {return !(container.size());}
    void push(Package&& pkg) override {container.emplace_back(std::move(pkg)); return void();}
    PackageQueueType get_queue_type() const override {return this->queue_type;}

};

/*
class PQLifo: public PackageQueue // stack
{
private:
    const static PackageQueueType pq_type = PackageQueueType::lifo; // niepotrzebne?
public:
    Package pop() override {Package op = std::move(container.back()); container.pop_back(); return op;}
    PackageQueueType get_queue_type() const override {return pq_type;}
};

class PQFifo: public PackageQueue // queue
{
private:
    const static PackageQueueType pq_type = PackageQueueType::fifo;
public:
    Package pop() override {Package op = std::move(container.front()); container.pop_front(); return op;}
    PackageQueueType get_queue_type() const override {return pq_type;}
};
*/

#endif //NETSIM_STORAGE_TYPES_HPP
