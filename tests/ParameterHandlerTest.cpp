#include <common.hpp>
#include <CppUTest/TestHarness.h>

#include <PersistantStorageMock.hpp>
#include <Parameter/ParameterHandler.hpp>


PersistantStorageMock storage;
ParameterHandler* handler;
uint16_t default_id = 0x10;
uint8_t default_value = 0x22;

void create_test_dataset() {
    uint16_t id = 0x10;
    uint8_t default_value = 0x12;

    handler->get_or_create<uint8_t>(id, default_value);
    handler->get_or_create<uint8_t>(id+1, default_value);
    handler->get_or_create<uint8_t>(id+2, default_value);
    handler->get_or_create<uint8_t>(id+3, default_value);
    handler->get_or_create<uint32_t>(id+4, 0x123456);
    handler->get_or_create<uint8_t>(id+5, default_value);
}

TEST_GROUP(ParameterHandler)
{
    void setup() {
        handler = new ParameterHandler(&storage);
        storage.init();
    }

    void teardown() {
        delete handler;
    }
};

TEST(ParameterHandler, test_init)
{   
    LONGS_EQUAL(0, handler->get_number_of_parameters());
    LONGS_EQUAL(0, handler->get_size());
}

TEST(ParameterHandler, test_add_one_uint8)
{   
    uint8_t* param = handler->get_or_create<uint8_t>(default_id, default_value);

    CHECK_TRUE(param != NULL);
    LONGS_EQUAL(default_value, *param);

    LONGS_EQUAL(1, handler->get_number_of_parameters());
    LONGS_EQUAL(2+1, handler->get_size());
}

TEST(ParameterHandler, test_add_one_uint16)
{   
    uint16_t default_value = 0x1234;
    uint16_t* param = handler->get_or_create<uint16_t>(default_id, default_value);

    LONGS_EQUAL(default_value, *param);
    LONGS_EQUAL(1, handler->get_number_of_parameters());
    LONGS_EQUAL(2+2, handler->get_size());
}

TEST(ParameterHandler, test_add_one_uint32)
{   
    uint32_t default_value = 0x12345678;
    uint32_t* param = handler->get_or_create<uint32_t>(default_id, default_value);

    LONGS_EQUAL(default_value, *param);
    LONGS_EQUAL(1, handler->get_number_of_parameters());
    LONGS_EQUAL(2+4, handler->get_size());
}

TEST(ParameterHandler, test_add_one_uint64)
{   
    uint64_t default_value = 0x1234567890ABCDEF;
    uint64_t* param = handler->get_or_create<uint64_t>(default_id, default_value);

    LONGS_EQUAL(default_value, *param);
    LONGS_EQUAL(1, handler->get_number_of_parameters());
    LONGS_EQUAL(2+8, handler->get_size());
}

TEST(ParameterHandler, test_two_different_parameters)
{   
    uint8_t* param1 = handler->get_or_create<uint8_t>(default_id, default_value);
    uint16_t* param2 = handler->get_or_create<uint16_t>(default_id+1, 0x1234);

    LONGS_EQUAL(default_value, *param1);
    LONGS_EQUAL(0x1234, *param2);
    LONGS_EQUAL(2, handler->get_number_of_parameters());
    LONGS_EQUAL(2+1+2+2, handler->get_size());
}

TEST(ParameterHandler, test_same_parameter_twice)
{   
    uint8_t* param1 = handler->get_or_create<uint8_t>(default_id, default_value);
    uint8_t* param2 = handler->get_or_create<uint8_t>(default_id, 0x55);

    LONGS_EQUAL(default_value, *param1);
    LONGS_EQUAL(default_value, *param2);
    LONGS_EQUAL(1, handler->get_number_of_parameters());
    LONGS_EQUAL(2+1, handler->get_size());
}

TEST(ParameterHandler, test_get_parameter)
{   
    uint8_t* param = handler->get_or_create<uint8_t>(default_id, default_value);

    uint8_t* param_ok = handler->get<uint8_t>(default_id);
    uint8_t* param_nok = handler->get<uint8_t>(default_id+1);

    LONGS_EQUAL(*param, *param_ok);
    LONGS_EQUAL(NULL, param_nok);
}

TEST(ParameterHandler, test_get_parameter_wrong_size)
{   
    uint8_t* param = handler->get_or_create<uint8_t>(default_id, default_value);

    uint8_t* param_ok = handler->get<uint8_t>(default_id);
    uint16_t* param_nok = handler->get<uint16_t>(default_id);

    LONGS_EQUAL(*param, *param_ok);
    LONGS_EQUAL(NULL, param_nok);
}

TEST(ParameterHandler, test_too_many_parameters)
{   
    uint8_t* param;

    for (uint8_t i = 0; i < MAX_PARAMETERS; i++) {
        param = handler->get_or_create<uint8_t>(default_id+i, default_value);
        CHECK_TRUE(param != NULL);
    }

    param = handler->get_or_create<uint8_t>(0x200, default_value);
    CHECK_TRUE(param == NULL);
}

TEST(ParameterHandler, test_buffer_out_of_range) 
{
    uint64_t* param;
    uint16_t number_of_possible_uint64 = MAX_PARAMETER_SIZE / (8+2);

    /* There is space for */
    for (uint8_t i = 0; i < number_of_possible_uint64; i++) {
        param = handler->get_or_create<uint64_t>(default_id+i, default_value);
        CHECK_TRUE(param != NULL);
    }

    param = handler->get_or_create<uint64_t>(0x200, default_value);
    CHECK_TRUE(param == NULL);
}

TEST(ParameterHandler, test_upload)
{
    create_test_dataset();
    handler->upload();
    
    MEMCMP_EQUAL(handler->get_data(), storage.data, handler->get_size());
}

TEST(ParameterHandler, test_download)
{
    uint16_t size; 
    uint16_t num_parameters;

    /* Create test dataset and upload */
    create_test_dataset();
    handler->upload();
    num_parameters = handler->get_number_of_parameters();
    size = handler->get_size();

    /* Create new parameter handler */
    delete handler; 
    handler = new ParameterHandler(&storage);

    /* Download content from storage */
    handler->download();
    
    LONGS_EQUAL(num_parameters, handler->get_number_of_parameters());
    LONGS_EQUAL(size, handler->get_size());
    MEMCMP_EQUAL(handler->get_data(), storage.data, handler->get_size());
}