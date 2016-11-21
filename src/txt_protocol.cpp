#include "txt_protocol.h"

#include <algorithm>
#include <string.h>
#include <cctype>

const unsigned MAX_NAME_LEN = 10;
const unsigned MAX_FIELD_LEN = 10;

bool IsDigitString(const char* str) {
    while(*str) {
        if(!std::isdigit(*str))
            return false;
        str++;
    }
    return true;
}

bool CheckTagAndGetFieldData(const char* buff, unsigned buff_size, unsigned& offset, const char* tag, char* data_buff) {
    if (!buff || !tag || !buff_size || offset >= buff_size)
        return false;

    const char* curr_pos = buff + offset;
    unsigned bytes_left = buff_size - offset;

    const char* colon_pos = (const char*)memchr(curr_pos, ':', bytes_left);
    const char* bar_pos = (const char*)memchr(curr_pos, '|', bytes_left);
    if (!colon_pos || !bar_pos || bar_pos < colon_pos)
        return false;

    unsigned tag_len = strlen(tag);
    if (memcmp(curr_pos, tag, tag_len) != 0)
        return false;

    if (bar_pos - 1 - colon_pos >= MAX_FIELD_LEN)
        return false;

    memcpy(data_buff, colon_pos + 1, bar_pos - colon_pos - 1);
    offset += bar_pos - curr_pos + 1;
    return true; 
}

bool GetField(const char* buff, unsigned buff_size, unsigned& offset, const char* tag, std::string& val) {
    char tmp_buff[MAX_FIELD_LEN] = { 0 };
    if(!CheckTagAndGetFieldData(buff, buff_size, offset, tag, tmp_buff))
        return false;

    val = std::string(tmp_buff);
    return val.length() <= MAX_NAME_LEN;
}

bool GetField(const char* buff, unsigned buff_size, unsigned& offset, const char* tag, unsigned& val) {
    char tmp_buff[MAX_FIELD_LEN] = { 0 };
    if(!CheckTagAndGetFieldData(buff, buff_size, offset, tag, tmp_buff))
        return false;

    if(!IsDigitString(tmp_buff))
        return false;

    val = atoi(tmp_buff);
    return true;
}

bool GetField(const char* buff, unsigned buff_size, unsigned& offset, const char* tag, Side& val) {
    unsigned side_val;

    if(!GetField(buff, buff_size, offset, tag, side_val))
        return false;

    if(side_val == BUY || side_val == SELL) {
        val = (Side)side_val;
        return true;
    } else {
        return false;
    }
}

Order* ParseOrder(const std::string& line) {

    // Id, Trader, stock, quantity, Side (Buy or Sell).
    // id:2341|trader:jack|stock:msft|quantity:15|side:0|
    // id:1232|trader:bobo|stock:gogl|quantity:15|side:1|

    unsigned id;
    std::string trader;
    std::string stock;
    unsigned quantity;
    Side side;

    unsigned offset = 0;
    bool ok = GetField(line.c_str(), line.length(), offset, "id", id);
    ok = ok && GetField(line.c_str(), line.length(), offset, "trader", trader);
    ok = ok && GetField(line.c_str(), line.length(), offset, "stock", stock);
    ok = ok && GetField(line.c_str(), line.length(), offset, "quantity", quantity);
    ok = ok && GetField(line.c_str(), line.length(), offset, "side", side);
    return ok 
        ? new Order(id, trader, stock, quantity, side)
        : nullptr;
}

