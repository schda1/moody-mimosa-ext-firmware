
#include <PersistantStorage/PersistantStorage.hpp>

PersistantStorage::PersistantStorage() : PersistantStorage(0, 0) {}

PersistantStorage::PersistantStorage(uint32_t start_address, uint32_t len) : start_address(start_address), len(len) {}

void PersistantStorage::set_region(uint32_t start_address, uint32_t len)
{
    this->start_address = start_address;
    this->len = len;
}

void PersistantStorage::erase()
{
    erase(start_address, len);
}