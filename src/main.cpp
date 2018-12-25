
#include <WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>

void printWiFiInfo();
void connectToWiFi(const char * ssid, const char * pwd);
void WiFiEvent(WiFiEvent_t event);
IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);

// WiFi network name and password:
const char * networkName = "Sol 2.4";
const char * networkPswd = "U43UU8CRUR";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
IPAddress broadcast;
//const char * udpAddress = "192.168.0.255";
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop(){
  //only send data when connected
  if(connected){
    //Send a packet
    udp.beginPacket(broadcast,udpPort);
    udp.printf("Seconds since boot: %u", millis()/1000);
    udp.endPacket();
  }
  //Print connection info
  printWiFiInfo();

  //Wait for 1 second
  delay(1000);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  WiFi.setHostname("NOME DA CAMBIARE");

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          broadcast = calculateBroadcast(WiFi.localIP(),WiFi.subnetMask());
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}

void printWiFiInfo(){
  if(!connected){
    Serial.println("Not connected to a network!");
    return;
  }
  Serial.println("  ~ WiFi info ~  ");
  //Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Broadcast IP: ");
  Serial.println(broadcast);
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.println("--------------------------");
}

IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet){
  IPAddress broadcast;
  broadcast = ip | ( ~ subnet );
  return broadcast;
}