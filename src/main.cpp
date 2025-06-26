#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
// #include <M5Stack.h>
#include <M5Unified.h>

int test = 0; // ตัวแปรสำหรับทดสอบการส่งข้อมูล

typedef struct {
  uint8_t type; // 0: Add peer | 1: Data message
  uint8_t length; // Length of the data
  uint8_t data[100]; // Data payload
  uint8_t mac[6];
} DataMessage;

uint8_t knownPeers[20][6];
int peerCount = 0;

void addPeer(const uint8_t *mac) {
  Serial.print("TEST addPeer: ");
  uint8_t myMac[6];
  WiFi.macAddress(myMac); 
  if (memcmp(mac, myMac, 6) == 0) {
    return;
  }
  for (int i = 0; i < peerCount; i++) {
    if (memcmp(knownPeers[i], mac, 6) == 0) {
      return;
    }
  }

 if(peerCount < 20){
    memcpy(knownPeers[peerCount],mac, 6);
    peerCount++;
    Serial.print("Add peer: ");
    for (int i = 0; i < 6; i++) Serial.printf("%02X:", mac[i]);
    Serial.printf(" peerCount=%d\n", peerCount);
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  }
  else {
    Serial.println("Peer list full, cannot add more peers.");
  }

}
void OnRecvDisplay(DataMessage *message) {
  // แสดงผลที่ตำแหน่งเดียวทุกครั้ง (หรือจะสุ่มตำแหน่งก็ได้)
  int y = 40;

  M5.Lcd.fillRect(0, y, 320, 50, BLACK);
  M5.Lcd.setCursor(10, y);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.print("Mac: ");
  for (int i = 0; i < 6; i++) {
    if (i > 0) M5.Lcd.print(":");
    M5.Lcd.printf("%02X", message->mac[i]);
  }
  y += 30;
  M5.Lcd.setCursor(10, y);
  M5.Lcd.print("Data: ");
  for (int i = 0; i < message->length; i++) {
    M5.Lcd.print((char)message->data[i]);
  }
}
//  void OnRecvDisplay(DataMessage *message) {
//   Serial.print("OnRecvDisplay: message->mac = ");
//     for (int i = 0; i < 6; i++) Serial.printf("%02X:", message->mac[i]);
//     Serial.println();

//     for (int i = 0; i < peerCount; i++) {
//       Serial.print("knownPeers["); Serial.print(i); Serial.print("] = ");
//       for (int j = 0; j < 6; j++) Serial.printf("%02X:", knownPeers[i][j]);
//       Serial.println();
//     }  int index = -1;
  
//   for (int i = 0; i < peerCount; i++) {
//     if (memcmp(knownPeers[i], message->mac, 6) == 0) {
//       index = i;
//       break;
//     }
//   }
//   if (index == -1) return; 
//   Serial.println("TEST OnRecvDisplay After");

//   int y = 10 + index * 50;

//   M5.Lcd.fillRect(0, y, 320, 50, BLACK);

//   M5.Lcd.setCursor(10, y);
//   M5.Lcd.setTextSize(3);
//   M5.Lcd.setTextColor(WHITE, BLACK);
//   M5.Lcd.print("Mac: ");
//   for (int i = 0; i < 6; i++) {
//     if (i > 0) M5.Lcd.print(":");
//     M5.Lcd.printf("%02X", message->mac[i]);
//   }
//   y += 30;
//   M5.Lcd.setCursor(10, y);
//   M5.Lcd.print("Data: ");
//   for (int i = 0; i < message->length; i++) {
//     M5.Lcd.print((char)message->data[i]);
//   }
// }
bool alreadyBroadcastedToThisMAC(const uint8_t *mac) {
  for (int i = 0; i < peerCount; i++) {
    if (memcmp(knownPeers[i], mac, 6) == 0) {
      return true;
    }
  }
  return false;
}
void broadcastMyMAC() {
  Serial.println("TEST broadcastMyMAC");
  DataMessage message;
  message.type = 0; 
  message.length = 6; 
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  memcpy(message.mac, macAddr, 6);

  uint8_t broadcast[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  
  esp_now_send(broadcast, (uint8_t*)&message, sizeof(message));
}
void OnRecv(const uint8_t *mac, const uint8_t *data, int len){
  DataMessage *message = (DataMessage*)data;
  Serial.printf("OnRecv: type=%d, from %02X:%02X:%02X:%02X:%02X:%02X\n",
                message->type, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if(message->type == 0) {
    Serial.println("Received broadcast (type 0)");
    if (!alreadyBroadcastedToThisMAC(mac)) {
      broadcastMyMAC(); 
    }

    
    addPeer(message->mac);
  } else if (message->type == 1 ) { 
    
    OnRecvDisplay(message); 

    Serial.printf("Received data from %02X:%02X:%02X:%02X:%02X:%02X: %.*s\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                  message->length, message->data);
    Serial.println("Data received from peer:");
    for(auto text : message->data) {
      Serial.print((char)text);
    }   
  }
}


// void onSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("Send Status: ");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
// }



void sendData(const char *msg) {
  Serial.println("TEST sendData");
  DataMessage message;
  message.type = 1; 
  message.length = strlen(msg);
  memcpy(message.data, msg, message.length);
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  memcpy(message.mac, macAddr, 6);
  

  for (int i = 0; i < peerCount; i++)
  {
    esp_err_t result = esp_now_send(knownPeers[i], (uint8_t*)&message, sizeof(message));
    if (result == ESP_OK) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }   
  }
}

void setup() {
  
  Serial.begin(115200);
  M5.begin();
  Serial.println("M5Stack ESP-NOW Test");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  randomSeed(millis());
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_peer_info_t peerInfo = {};
  uint8_t broadcast[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  memcpy(peerInfo.peer_addr, broadcast, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  esp_now_register_recv_cb(OnRecv);
  // esp_now_register_send_cb(onSend);
  broadcastMyMAC(); 

}

void loop() {
delay(random(2000, 5000));
    M5.update();
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", test);
    sendData(buf);
    test++;
}