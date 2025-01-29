#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Broadcast address to send to all devices
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Maximum number of peers we can track
#define MAX_PEERS 20
uint8_t peerAddresses[MAX_PEERS][6];
int peerCount = 0;

typedef struct {
    uint8_t messageType;  // 0: discovery, 1: data
    char nodeName[32];    // Identifier for the node
    float temperature;    // Sample sensor data
    bool ledState;
    char message[32];
} MessageData;

MessageData sentData;
const int LED_PIN = 2;

// Generate a random node name
void generateNodeName(char* nodeName) {
    snprintf(nodeName, 32, "Node-%04X", esp_random() & 0xFFFF);
}

bool addPeerIfNew(const uint8_t *mac_addr) {
    // Check if peer already exists
    for(int i = 0; i < peerCount; i++) {
        if(memcmp(peerAddresses[i], mac_addr, 6) == 0) {
            return false;  // Peer already exists
        }
    }
    
    // Add new peer if we have space
    if(peerCount < MAX_PEERS) {
        memcpy(peerAddresses[peerCount], mac_addr, 6);
        
        // Add to ESP-NOW peers
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac_addr, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        
        if (esp_now_add_peer(&peerInfo) == ESP_OK) {
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            Serial.print("Added new peer: ");
            Serial.println(macStr);
            peerCount++;
            return true;
        }
    }
    return false;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        // Remove failed peer
        for(int i = 0; i < peerCount; i++) {
            if(memcmp(peerAddresses[i], mac_addr, 6) == 0) {
                esp_now_del_peer(mac_addr);
                // Shift remaining peers
                for(int j = i; j < peerCount - 1; j++) {
                    memcpy(peerAddresses[j], peerAddresses[j+1], 6);
                }
                peerCount--;
                break;
            }
        }
    }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    MessageData receivedData;
    memcpy(&receivedData, incomingData, sizeof(MessageData));
    
    // Add sender to peers if new
    addPeerIfNew(mac_addr);
    
    if(receivedData.messageType == 0) {
        // Discovery message received
        Serial.print("Discovery from node: ");
        Serial.println(receivedData.nodeName);
    } else {
        // Data message received
        Serial.print("Data from ");
        Serial.print(receivedData.nodeName);
        Serial.print(": ");
        Serial.println(receivedData.message);
        digitalWrite(LED_PIN, receivedData.ledState);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Set WiFi channel
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    
    // Register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // Add broadcast address as peer for discovery
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
    
    // Generate random node name
    generateNodeName(sentData.nodeName);
    Serial.print("My node name: ");
    Serial.println(sentData.nodeName);
}

void loop() {
    static unsigned long lastDiscovery = 0;
    static unsigned long lastData = 0;
    static int counter = 0;
    unsigned long currentTime = millis();
    
    // Send discovery message every 5 seconds
    if(currentTime - lastDiscovery > 5000) {
        sentData.messageType = 0;  // Discovery message
        esp_now_send(broadcastAddress, (uint8_t *)&sentData, sizeof(MessageData));
        lastDiscovery = currentTime;
    }
    
    // Send data message to all known peers every 2 seconds
    if(currentTime - lastData > 2000) {
        sentData.messageType = 1;  // Data message
        sentData.temperature = random(20, 30);
        sentData.ledState = !sentData.ledState;
        snprintf(sentData.message, sizeof(sentData.message), "Hello #%d", counter++);
        
        // Send to all known peers
        for(int i = 0; i < peerCount; i++) {
            esp_now_send(peerAddresses[i], (uint8_t *)&sentData, sizeof(MessageData));
        }
        
        lastData = currentTime;
    }
}