
#pragma once

#include <PersistantStorage/PersistantStorage.hpp>
#include <stdint.h>

#define MOCK_SIZE (1024)

class PersistantStorageMock : public PersistantStorage
{
    public:
        PersistantStorageMock();
        PersistantStorageMock(uint32_t start_address, uint32_t len);
        virtual ~PersistantStorageMock() {}
        
        virtual void init() override;
        virtual void deinit() override;
        virtual void erase(uint32_t start_address, uint32_t len) override;

        virtual void write8(uint32_t address, uint8_t value) override;
        virtual void write32(uint32_t address, uint32_t value) override;
        virtual void write(uint32_t address, const uint8_t* data, uint32_t len) override;

        virtual void read8(uint32_t address, uint8_t* value) override;
        virtual void read32(uint32_t address, uint32_t* value) override;
        virtual void read(uint32_t address, uint8_t* data, uint32_t len) override;

        void print();

        uint8_t data[MOCK_SIZE];
};
