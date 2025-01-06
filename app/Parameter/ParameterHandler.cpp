#include <Parameter/ParameterHandler.hpp>


#define MASK_SIZE (0xF000)
#define MASK_ID (0x0FFF)
#define NOT_FOUND (-1)
#define HEADER_SIZE (2)
#define SIZE_OFFSET (12)

ParameterHandler::ParameterHandler(PersistantStorage* pstorage)
{
    num_parameters = 0;
    head = data;
    storage = pstorage;

    memset(data, 0xFF, MAX_PARAMETER_SIZE);
}

ParameterHandler::ParameterHandler(const ParameterHandler& other)
{
    num_parameters = other.num_parameters;
    storage = other.storage;
    head = data;
    memcpy(data, other.data, MAX_PARAMETER_SIZE);
}

void ParameterHandler::download()
{
    uint32_t start_address = storage->get_start_address();
    storage->read(start_address, data, MAX_PARAMETER_SIZE);

    head = find_head();
}

void ParameterHandler::upload()
{
    uint32_t start_address = storage->get_start_address();
    storage->write(start_address, data, MAX_PARAMETER_SIZE);
}

uint16_t ParameterHandler::get_size()
{
    return (size_t)(head - data);
}

uint16_t ParameterHandler::get_number_of_parameters()
{
    return num_parameters;
}

uint8_t* ParameterHandler::find_parameter(uint16_t identifier, uint8_t target_size)
{
    uint8_t* address = data;
    uint16_t size;
    uint16_t id;

    for (uint8_t i = 0; i < num_parameters; i++) {
        id = *((uint16_t*)address) & MASK_ID;
        size = *((uint16_t*)address) & MASK_SIZE;
        size >>= SIZE_OFFSET;
        size++;

        if (id == identifier && size == target_size) {
            return address;
        }

        address += HEADER_SIZE + size;
    }

    return nullptr;
}

uint8_t* ParameterHandler::add(uint16_t identifier, uint8_t size)
{
    if ((size_t)(head + size + HEADER_SIZE - data) > MAX_PARAMETER_SIZE) {
        return nullptr;
    }

    if (num_parameters >= MAX_PARAMETERS) {
        return nullptr;
    }

    uint8_t* new_param = head;

    *((uint16_t*)new_param) = identifier;
    *((uint16_t*)new_param) |= ((size - 1) << SIZE_OFFSET);

    head += HEADER_SIZE + size;
    num_parameters++;

    return new_param;
}

uint8_t* ParameterHandler::find_head()
{
    uint8_t* address = data;
    uint16_t size;
    uint16_t id;

    num_parameters = 0;

    while (true) {
        id = *((uint16_t*)address) & MASK_ID;
        size = *((uint16_t*)address) & MASK_SIZE;
        size >>= SIZE_OFFSET;
        size++;

        if (id == 0xFFF) {
            break;
        }

        num_parameters++;
        address += HEADER_SIZE + size;
    }

    return address;
}

void ParameterHandler::print()
{
    uint8_t* address = data;
    uint32_t value;
    uint16_t size;
    uint16_t id;

    printf("ParameterHandler:\n");

    for (uint8_t i = 0; i < num_parameters; i++) {
        id = *((uint16_t*)address) & MASK_ID;
        size = *((uint16_t*)address) & MASK_SIZE;
        size >>= SIZE_OFFSET;
        size++;

        if (size == 1) {
            value = *((uint8_t*)(address + HEADER_SIZE));
        } else if (size == 2) {
            value = *((uint16_t*)(address + HEADER_SIZE));
        } else if (size == 4) {
            value = *((uint32_t*)(address + HEADER_SIZE));
        } else {
            value = 0;
        }

        printf("[%d] id=%x, size=%d, value=%ld\n", i, id, size, value);

        address += HEADER_SIZE + size;
    }

    printf("Data content: \n");
    for (uint8_t i = 0; i < (size_t)(head - data + 10); i++) {
        printf("0x%02x ", data[i]);

        if (i % 16 == 15) {
            printf("\n");
        }
    }
}