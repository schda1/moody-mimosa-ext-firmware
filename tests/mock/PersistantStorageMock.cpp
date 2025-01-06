
#include <PersistantStorageMock.hpp>

PersistantStorageMock::PersistantStorageMock() : PersistantStorage(0, MOCK_SIZE) {}

PersistantStorageMock::PersistantStorageMock(uint32_t start_address, uint32_t len) : PersistantStorage(start_address, len) {}

void PersistantStorageMock::init()
{
    for (uint32_t i = 0; i < len; i++) {
        data[i] = 0;
    }
}

void PersistantStorageMock::deinit()
{
    for (uint32_t i = 0; i < len; i++) {
        data[i] = 0;
    }
}

void PersistantStorageMock::erase(uint32_t start_address, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        data[i] = 0;
    }
}

void PersistantStorageMock::write8(uint32_t address, uint8_t value)
{
    data[address] = value;
}

void PersistantStorageMock::write32(uint32_t address, uint32_t value)
{
    *((uint32_t*)(data + address)) = value;
}

void PersistantStorageMock::write(uint32_t address, const uint8_t* data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        this->data[address + i] = data[i];
    }
}

void PersistantStorageMock::read8(uint32_t address, uint8_t* value)
{
    *value = data[address];
}

void PersistantStorageMock::read32(uint32_t address, uint32_t* value)
{
    *value = *((uint32_t*)(data + address));
}

void PersistantStorageMock::read(uint32_t address, uint8_t* data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        data[i] = this->data[address + i];
    }
}

void PersistantStorageMock::print()
{
    for (uint32_t i = 0; i < len; i++) {
        printf("0x%02x ", data[i]);

        if (i % 16 == 15) {
            printf("\n");
        }
    }
}