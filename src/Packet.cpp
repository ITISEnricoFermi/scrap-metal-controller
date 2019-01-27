#include "Packet.h"

Packet::Packet()
{
    payload = (uint8_t*) malloc(sizeof(uint8_t) * 200);
}


void Packet::print()
{
    Serial.println("-----------------");
    Serial.print("type: ");
    Serial.println(type);
    
    Serial.print("mac: ");
    Serial.println(mac);
    
    Serial.print("payload: ");
    Serial.println(payload_len);
    for(size_t i = 0; i < payload_len; i++)
    {
        Serial.print(((char*)payload)[i]);
    }
    Serial.println("");
    Serial.println("-----------------");
}

void Packet::release()
{
    //memset(payload,0,sizeof(uint8_t) * 200);
    //free(payload);
}

bool parse(uint8_t* buffer, size_t len, Packet* packet)
{
    
    enum Steps
    {
        TYPE = 0,
        MAC = 1,
        PAYLOAD_LEN = 2,
        PAYLOAD = 3,
        END = 4
    };
    Steps step = TYPE;
    //read the buffer
    for(size_t i = 0; i < len; i++)
    {
        const uint8_t b = buffer[i];
        switch(step){

            case TYPE:
                packet->type = (uint8_t)b;
                break;

             case MAC:

                uint8_t mac[6];
                for(size_t t = 0; t < 6; t++)
                {
                    const uint8_t mac_byte = buffer[i++];
                    mac[t] = (uint8_t)mac_byte;
                }
                i--;
                    {
                        char macStr[18];
                
                        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        packet->mac = String(macStr);
                    }
                   break;

            case PAYLOAD_LEN:
                packet->payload_len = (uint8_t)b;
                if(packet->payload_len > 200)return false;
                break;

            case PAYLOAD:
                {
                    /*  ~DEPRECATED~
                    uint8_t payload[packet->payload_len] = { 0 };
                    memset(packet->payload, 0, sizeof(uint8_t) * packet->payload_len);
                    */


                    for(size_t j = 0; j < packet->payload_len; j++, i++)
                    {
                        packet->payload[j] = buffer[i];
                    }

                    //packet->payload = payload;
                }
                break;
                return true;

            default:
                return false;
                break;
        }

        //next step
        step = Steps((int)step + 1);

    }

    return false;
}