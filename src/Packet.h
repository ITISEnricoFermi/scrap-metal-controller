#include <stdio.h>
#include <Arduino.h>

struct Packet{
    uint8_t type = 0;
    String mac = "ff:ff:ff:ff:ff:ff";
    size_t payload_len = 0;
    String payload = "";
    Packet();
    void print();
};

bool parse(uint8_t* data, size_t len, Packet& packet);