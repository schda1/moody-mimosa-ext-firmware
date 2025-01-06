#include <PersistantStorage/FramStorage_Spi.hpp>
#include <stdio.h>

// #include "stm32g4xx_hal.h"

#define MB85RS64_LEN (0x10000)
#define SPI_TIMEOUT (100)

enum opcodes
{
    OPCODE_WREN = 0b0110,    /* Write Enable */
    OPCODE_WRDI = 0b0100,    /* Reset Write Enable */
    OPCODE_READ = 0b0011,    /* Read Memory */
    OPCODE_WRITE = 0b0010,   /* Write Memory */
    OPCODE_RDID = 0b10011111 /* Read Device ID */
};

struct fram_devices
{
    uint8_t manufacturer_id;
    uint16_t product_id;
    uint32_t size;
};

const struct fram_devices fram_devices[] = {
    {0x04, 0x0101, 0x0800},  /* MB85RS16    */
    {0x04, 0x0302, 0x2000},  /* MB85RS64V   */
    {0x04, 0x2303, 0x2000},  /* MB85RS64T   */
    {0x04, 0x2503, 0x8000}}; /* MB85RS256TY */

FramStorage_Spi::FramStorage_Spi(ISpi* hspi) : PersistantStorage(0, MB85RS64_LEN), hspi(hspi) {}

FramStorage_Spi::FramStorage_Spi(ISpi* hspi, uint32_t start_address, uint32_t len) : PersistantStorage(start_address, len), hspi(hspi) {}

void FramStorage_Spi::init()
{
    hspi->init();
    printf("spi init \n");
    write_enable(true);
    printf("write enabled \n");
    read_device_id();
    printf("read dev id \n");
}

void FramStorage_Spi::deinit()
{
    hspi->deinit();
}

void FramStorage_Spi::erase()
{
    erase(start_address, len);
}

void FramStorage_Spi::erase(uint32_t start_address, uint32_t len)
{
    uint8_t buffer[32];
    memset(buffer, 0x00, sizeof(buffer));

    for (uint32_t i = 0; i < 8192; i+=64) {
        write(i, buffer, 64);
        HAL_Delay(10);
    }
}

void FramStorage_Spi::write8(uint32_t address, uint8_t value)
{
    write(address, &value, 1);
}

void FramStorage_Spi::write32(uint32_t address, uint32_t value)
{
    write(address, (uint8_t*)&value, 4);
}

void FramStorage_Spi::write(uint32_t address, const uint8_t* data, uint32_t len)
{
    uint8_t buffer[5];
    buffer[0] = OPCODE_WRITE;

    for (uint8_t i = 0; i < address_bytes; i++) {
        buffer[i + 1] = (uint8_t)(address >> (8 * (address_bytes - i - 1)));
    }

    write_enable(true);

    hspi->write_then_write(buffer, 1 + address_bytes, data, len);
}

void FramStorage_Spi::read8(uint32_t address, uint8_t* value)
{
    read(address, value, 1);
}

void FramStorage_Spi::read32(uint32_t address, uint32_t* value)
{
    read(address, (uint8_t*)value, 4);
}

void FramStorage_Spi::read(uint32_t address, uint8_t* data, uint32_t len)
{
    uint8_t buffer[5];
    buffer[0] = OPCODE_READ;

    for (uint8_t i = 0; i < address_bytes; i++) {
        buffer[i + 1] = (uint8_t)(address >> (8 * (address_bytes - i - 1)));
    }

    hspi->write_then_read(buffer, 1 + address_bytes, data, len);

    // set_cs(0);
    // HAL_SPI_Transmit(hspi, buffer, 1 + address_bytes, SPI_TIMEOUT);
    // HAL_SPI_Receive(hspi, data, len, SPI_TIMEOUT);
    // set_cs(1);
}

void FramStorage_Spi::write_enable(bool enable)
{
    uint8_t buffer[1];
    buffer[0] = (enable) ? OPCODE_WREN : OPCODE_WRDI;

    hspi->write(buffer, 1);
}

void FramStorage_Spi::read_device_id()
{
    uint8_t buffer[4];
    buffer[0] = OPCODE_RDID;

    hspi->write_then_read(buffer, 1, buffer, 4);

    if (buffer[1] == 0x7F) {
        manufacturer_id = buffer[0];
        product_id = (((uint16_t)buffer[2]) << 8) | (buffer[3]);
    } else {
        manufacturer_id = buffer[0];
        product_id = (((uint16_t)buffer[1]) << 8) | (buffer[2]);
    }

    printf("Manufacturer ID: 0x%02X\n", manufacturer_id);
    printf("Product ID: 0x%04X\n", product_id);

    if (!valid_device()) {
        manufacturer_id = 0;
        product_id = 0;
        capacity = 0;
    }

    address_bytes = calc_address_bytes();

#ifdef DEBUG_PRINT_FRAM
    printf("Manufacturer ID: 0x%02X\n", manufacturer_id);
    printf("Product ID: 0x%04X\n", product_id);
    printf("Capacity: %ld\n", capacity);
    printf("Address bytes: %d\n", address_bytes);
#endif
}

bool FramStorage_Spi::valid_device()
{
    for (uint8_t i = 0; i < sizeof(fram_devices) / sizeof(fram_devices[0]); i++) {
        if (manufacturer_id == fram_devices[i].manufacturer_id && product_id == fram_devices[i].product_id) {
            capacity = fram_devices[i].size;
            return true;
        }
    }

    return false;
}

uint8_t FramStorage_Spi::calc_address_bytes()
{
    uint8_t bytes = 0;
    uint32_t temp = capacity - 1;

    if (capacity == 0) {
        return 0;
    }

    while (temp > 0) {
        temp >>= 8;
        bytes++;
    }

    return bytes;
}