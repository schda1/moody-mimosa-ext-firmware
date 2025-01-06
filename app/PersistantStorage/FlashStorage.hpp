#pragma once

#include <common.hpp>
#include <PersistantStorage/PersistantStorage.hpp>


class FlashStorage : public PersistantStorage
{

public:
    FlashStorage();
    FlashStorage(uint32_t start_address, uint32_t len);
    virtual ~FlashStorage() {}

    virtual void init() override;
    virtual void deinit() override;


    virtual void erase(uint32_t start_address, uint32_t len) override;
    virtual void erase() override;

    virtual void write8(uint32_t address, uint8_t value) override;
    virtual void write32(uint32_t address, uint32_t value) override;
    virtual void write(uint32_t address, const uint8_t* data, uint32_t size) override;

    virtual void read8(uint32_t address, uint8_t* value) override;
    virtual void read32(uint32_t address, uint32_t* value) override;
    virtual void read(uint32_t address, uint8_t* data, uint32_t size) override;

private:
    bool is_start_of_page(uint32_t address);
    bool is_within_range(uint32_t address, uint32_t len);
    bool is_double_word_aligned(uint32_t address);
    bool is_range_erased(uint32_t address, uint32_t len);

    uint32_t get_page(uint32_t address);
    uint32_t get_bank(uint32_t address);
};
