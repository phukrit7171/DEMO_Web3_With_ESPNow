#ifndef NODE_MENAGE_H
#define NODE_MENAGE_H
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>

class PeerManager;
// class MessageHandler;
class Peer;

enum MessageType : uint8_t
{
    DISCORVERY = 0,
    DATA = 1,
    ROUNTING = 2
};


struct DiscorveryMessage
{
    /* data */
    MessageType type;
    uint8_t mac[6];
    uint8_t length;

    DiscorveryMessage();
};

struct ReplyDiscorvery
{
    /* data */
    MessageType type;
    uint8_t mac[6];
    uint8_t rssi;
    uint8_t length;
    ReplyDiscorvery();
};

struct Message
{
    MessageType type;
    uint8_t to[6];
    uint8_t from[6];
    uint8_t data[100];
    uint8_t length;
    Message();
};
class Peer{
    private:
        uint8_t mac_address[6];
        int rssi_value;
    public:
        Peer(const uint8_t *mac, int rssi);

        /// @brief get mac_addr from peer
        /// @return mac_addr
        const uint8_t *getMac() const;

        /// @brief get rssi from peer
        /// @return RSSI 
        int getRssi() const;

        /// @brief get mac_addr to String
        /// @return (String) MAC_Address
        String toString() const;

        /// @brief Check if the provided MAC address matches this peer's MAC address.
        /// @param other_mac Pointer to the MAC address to compare.
        /// @return true if the MAC addresses are equal, false otherwise.
        bool equals(const uint8_t *other_mac) const;
};

class PeerManager
{
    private:
        uint8_t parent_node[6];
        uint8_t child_node[2][6];
        void addEspNowPeer(const uint8_t *mac);
        String macToString(const uint8_t *mac) const;

    public:
        // PeerManager();
        String toString() const;

        void addPeer(const uint8_t *mac, int rssi);
        bool findChild(const uint8_t *mac)const;
        void sendToParent(const uint8_t *toMacAddress, const uint8_t *fromMacAddress, const uint8_t*DestinationMacAddr, const uint8_t data[100])const;
        void sendToChild(const uint8_t *toChildMacAddress, const uint8_t data[100], const uint8_t lenght)const;
        void broadcastToAllPeers(const Message &msg);
};
extern PeerManager *g_peer_menage;
extern Peer *g_peer;

#endif