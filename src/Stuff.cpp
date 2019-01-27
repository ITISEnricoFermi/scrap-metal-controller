#include "Packet.h"
#include "Stuff.h"


bool Board::buffCompare(uint8_t* buff, char* str, uint8_t len)
{
    for(int i = 0; i < len; i++)
    {   
        //Serial.println((u_char)buff[i]);
        //Serial.println(str[i]);
         if(buff[i] != str[i]) return false;
    }

    return true;
}

void Board::update(Packet* packet, char* mac)
{
    //Serial.print();
    //if(mac != packet->mac) {Serial.println("wrong mac!"); return;}
    switch (packet->type)
    {
        case 0x31:
            if(buffCompare(packet->payload, "true", packet->payload_len))
            {
                state = LOW;
            }
            if(buffCompare(packet->payload, "false", packet->payload_len))
            {
                state = HIGH;
            }
            break;
    
        case 0x51:
            RState = packet->payload[0];
            GState = packet->payload[1];
            BState = packet->payload[2];

            Serial.print("R => ");
            Serial.println(RState);
            Serial.print("G => ");
            Serial.println(GState);
            Serial.print("B => ");
            Serial.println(BState);

        default:
            break;
    }
}

Board::Board()
{
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);
}

void Board::process()
{
    //digitalWrite(B, state);
    analogWrite(R,RState);
    analogWrite(G,GState);
    analogWrite(B,BState);
}

