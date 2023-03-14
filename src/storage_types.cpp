//pliki źródłowe "package.cpp" i "storage_types.cpp" zawierające definicje funkcjonalności
// odpowiednich plików nagłówkowych (oraz ewentualne definicje dodatkowych funkcjonalności,
// niezbędnych wyłącznie w ramach danego pliku źródłowego)

#include "storage_types.hpp"



Package PackageQueue::pop() {
    if(this->get_queue_type()==PackageQueueType::LIFO){
        Package result = Package(this->container.back());
        this->container.pop_back();
        return result;
    }
    if(this->get_queue_type()==PackageQueueType::FIFO){
        Package result = Package(this->container.front());
        this->container.pop_front();
        return result;
    }
    return Package();
}
