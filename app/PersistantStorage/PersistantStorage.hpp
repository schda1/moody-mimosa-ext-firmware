#pragma once

#include <common.hpp>

class PersistantStorage
{
public:
    PersistantStorage();
    PersistantStorage(uint32_t start_address, uint32_t len);

    virtual ~PersistantStorage() {}

    virtual void set_region(uint32_t start_address, uint32_t len);

    virtual void init() = 0;
    virtual void deinit() = 0;
    
    virtual void erase();
    virtual void erase(uint32_t start_address, uint32_t len) = 0;

    virtual void write8(uint32_t address, uint8_t value) = 0;
    virtual void write32(uint32_t address, uint32_t value) = 0;
    virtual void write(uint32_t address, const uint8_t* data, uint32_t len) = 0;

    virtual void read8(uint32_t address, uint8_t* value) = 0;
    virtual void read32(uint32_t address, uint32_t* value) = 0;
    virtual void read(uint32_t address, uint8_t* data, uint32_t len) = 0;

    uint32_t get_start_address() { return start_address; }

protected:
    uint32_t start_address;
    uint32_t len;
};
