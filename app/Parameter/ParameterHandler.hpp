#pragma once

#include <common.hpp>
#include "PersistantStorage/PersistantStorage.hpp"

#define HEADER_SIZE (2)
#define MAX_PARAMETERS (20)
#define MAX_PARAMETER_SIZE (100)

class ParameterHandler
{
public:
    ParameterHandler(PersistantStorage* storage);
    ParameterHandler(const ParameterHandler& other);
    virtual ~ParameterHandler() {}

    template <typename T> T* get_or_create(uint16_t identifier, T default_value);

    template <typename T> T* get(uint16_t identifier);

    void download();
    void upload();
    void print();

    uint16_t get_size();
    uint16_t get_number_of_parameters();
    uint8_t* get_data()
    {
        return data;
    }

private:
    uint8_t* find_parameter(uint16_t identifier, uint8_t target_size);
    uint8_t* add(uint16_t identifier, uint8_t size);
    uint8_t* find_head();

    uint8_t data[MAX_PARAMETER_SIZE];
    uint8_t num_parameters;
    uint8_t* head;

    PersistantStorage* storage;
};

template <typename T> T* ParameterHandler::get_or_create(uint16_t identifier, T default_value)
{
    uint8_t* address = (uint8_t*)get<T>(identifier);

    if (address == nullptr) {
        address = (uint8_t*)add(identifier, sizeof(T));

        if (address == nullptr) {
            return nullptr;
        }

        address += HEADER_SIZE;
        *((T*)address) = default_value;
    }

    return (T*)address;
}

template <typename T> T* ParameterHandler::get(uint16_t identifier)
{
    uint8_t* address = find_parameter(identifier, sizeof(T));

    if (address == nullptr) {
        return nullptr;
    } else {
        return (T*)(address + HEADER_SIZE);
    }
}