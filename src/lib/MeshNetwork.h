#ifndef MESH_NETWORK_H
#define MESH_NETWORK_H

#include "Arduino.h"
#include <esp_now.h>
class MeshNetwork
{
    private:
    
        void setupESPNow();
        void setupBroadcast();

    public:
        MeshNetwork();

        void loop();
        void shutdown();

        void onMessageReceived(const uint8_t *mac, const uint8_t* data, int len);
        void onMessageSent(const uint8_t *mac,esp_now_send_status_t STATUS);

        void sendUserMessage(const String &message);
        void handleButtonPree(int button);

        static void onDataReceiveCallback(const uint8_t *mac, const uint8_t *data,int len);
        static void onDataSentCallback(const uint8_t *mac,esp_now_send_status_t STATUS);
};

extern MeshNetwork *g_mesh_network;

#endif