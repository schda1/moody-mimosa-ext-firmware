#include <MessageParser.hpp>


#define MSG_INVALID (-1)
#define MSG_NONE (0)

MessageParser::MessageParser(IUart* uart) : uart(uart), head(0), tail(0), count(0), cmd_table(nullptr), cmd_len(0) {}

void MessageParser::init()
{
    uart->init();
    uart->set_read_callback([this](char c) { this->add(c); });
    uart->start_read_async();

    head = 0;
    tail = 0;
    count = 0;
}

void MessageParser::set_command_table(const Command* table, uint8_t len)
{
    cmd_table = table;
    cmd_len = len;
}

void MessageParser::add(uint8_t c)
{
    buf[head] = c;
    head = (head + 1) % MSG_BUFFER_LEN;
    count++;
}

void MessageParser::update(Msg* msg)
{
    find_start();
    check_for_full_message(msg);
}

void MessageParser::find_start()
{
    uint16_t pos = tail;

    if (head == tail) {
        return;
    }

    if (buf[tail] != ':') {

        while (pos != head) {
            pos = (pos + 1) % MSG_BUFFER_LEN;

            if (buf[pos] == ':') {
                break;
            }
        }

        tail = pos;
    }
}

void MessageParser::check_for_full_message(Msg* msg)
{
    uint16_t pos = tail;
    uint16_t start = tail;
    uint16_t end = UINT16_MAX;

    msg->id = MSG_NONE;
    msg->value = 0;

    if (head == tail || buf[tail] != ':') {
        return;
    }

    /* Check if full message contained */
    while (true) {
        pos = (pos + 1) % MSG_BUFFER_LEN;

        if (buf[pos] == '\n') {
            end = (pos + 1) % MSG_BUFFER_LEN;
            break;
        }

        if (pos == head) {
            break;
        }
    }

    /* No end found */
    if (end == UINT16_MAX) {
        return;
    }
    extract_message(msg, start, end);

    /* Update buffer */
    tail = end;
}

void MessageParser::extract_message(Msg* msg, uint16_t start, uint16_t end)
{
    char cbuf[96] = {0};
    char cmd[64];
    uint16_t pos = start;
    uint8_t cmd_pos = 0;

    /* Default */
    msg->id = MSG_INVALID;
    msg->value = 0;
    memset(msg->str, 0, sizeof(msg->str));

    while (pos != end) {
        cbuf[cmd_pos++] = buf[pos];
        pos = (pos + 1) % MSG_BUFFER_LEN;
    }

    uint8_t sep_count = count_number_separators(cbuf, ';');

    if (sep_count == 0) {
        msg->type = MsgData::NONE;
        sscanf(cbuf, ":%s\n", cmd);
    } else if (sep_count == 1) {
        msg->type = MsgData::VALUE;
        sscanf(cbuf, ":%32[^;];%lu\n", cmd, &msg->value);
    } else if (sep_count == 2) {
        msg->type = MsgData::STRING;
        sscanf(cbuf, ":%32[^;];%lu;%32[^\n]\n", cmd, &msg->value, msg->str);
    } else {
        /* Invalid, only 1 or 2 data items are supported */
        return;
    }

    /* Search the command table */
    for (uint8_t i = 0; i < cmd_len; i++) {
        if (cmd_table[i].name == NULL) {
            break;
        }

        if (strcmp(cmd_table[i].name, cmd) == 0) {
            msg->id = cmd_table[i].id;
            break;
        }
    }
}

uint8_t MessageParser::count_number_separators(const char* str, char sep)
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < strlen(str); i++) {
        if (str[i] == sep) {
            count++;
        }
    }

    return count;
}
