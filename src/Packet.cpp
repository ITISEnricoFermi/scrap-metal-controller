#include "Packet.h"

Packet::Packet(){
    // size_t offset = 0;

    // //1 byte type
    // //6 byte mac address
    // //1 byte payload length
    // //0 ~ 128 byte payload

    // m_type = packet [offset++];

    // uint8_t mac[6];

    // for(int i = 0; i < 6; i++, offset++){
    //     mac[i] = packet[offset];
    // }
    
    // char macStr[18] = { 0 };
    // sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    // m_mac = String(macStr);
    // length = (int)packet[offset++];
    // char payload[length];
    // for(int i = 0 ; i < length; i++){
    //     payload[i] = packet[offset + i];
    // }    
    // m_payload = String(payload);

}


void Packet::print(){
    Serial.println("-----------------");
    Serial.print("type: ");
    Serial.println(type);
    
    Serial.print("mac: ");
    Serial.println(mac);
    
    Serial.print("payload: ");
    Serial.println(payload_len);
    Serial.println(payload);
    Serial.println("-----------------");
}


bool parse(uint8_t* data, size_t len, Packet& packet){
    
    enum Steps{
        TYPE = 0,
        MAC = 1,
        PAYLOAD_LEN = 2,
        PAYLOAD = 3,
        END = 4
    };
    Steps step = TYPE;
    //read the buffer
    for(size_t i = 0; i < len; i++){
        const auto b = data[i];
        switch(step){

            case TYPE:
                packet.type = (uint8_t)b;
                break;

             case MAC:

                uint8_t mac[6];
                for(size_t t = 0; t < 6; t++, i++){
                    const auto mac_byte = data[i];
                    mac[t] = (uint8_t)mac_byte;
                }
                i--;
                    {
                        char macStr[18] = { 0 };
                
                        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        packet.mac = String(macStr);
                    }
                   break;

            case PAYLOAD_LEN:
                packet.payload_len = (size_t)b;
                break;

            case PAYLOAD:
                {
                        uint8_t payload[packet.payload_len]= { 0 };
                    for(size_t j = 0; j < packet.payload_len; j++, i++){
                        const auto payload_byte = data[i];
                        payload[j] = (uint8_t)payload_byte;
                    }
                    packet.payload = String((char*)payload);
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