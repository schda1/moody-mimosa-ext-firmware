#pragma once

#include <common.hpp>
#include <HalWrapper/IUart.hpp>

#define MSG_BUFFER_LEN (256)

enum class MsgData
{
    NONE = 0,
    VALUE,
    STRING
};

struct Msg
{
    int8_t id;
    MsgData type;
    uint32_t value;
    char str[64];
};

struct Command
{
    const char* name;
    int8_t id;
    MsgData data;
};

class MessageParser
{

public:
    MessageParser(IUart* uart);

    void init();
    void set_command_table(const Command* cmd_table, uint8_t len);
    void update(Msg* msg);

private:
    void add(uint8_t c);
    void find_start();
    void check_for_full_message(Msg* msg);
    void extract_message(Msg* msg, uint16_t start, uint16_t end);
    uint8_t count_number_separators(const char* str, char sep);

    IUart* uart;

    uint8_t buf[MSG_BUFFER_LEN];
    uint8_t head;
    uint8_t tail;
    uint32_t count;

    const Command* cmd_table;
    uint8_t cmd_len;
};
