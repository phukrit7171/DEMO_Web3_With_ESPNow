

#include <lib/NodeManagement.h>


PeerManager *g_peer_menage = nullptr;
Peer *g_peer = nullptr;

// ---- Peer Implementation ----
Peer::Peer(const uint8_t *mac, int rssi) {
    memcpy(mac_address, mac, 6);
    rssi_value = rssi;
}

const uint8_t *Peer::getMac() const {
    return mac_address;
}

int Peer::getRssi() const {
    return rssi_value;
}

String Peer::toString() const {
    char buf[18];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac_address[0], mac_address[1], mac_address[2],
            mac_address[3], mac_address[4], mac_address[5]);
    return String(buf);
}

bool Peer::equals(const uint8_t *other_mac) const {
    return memcmp(mac_address, other_mac, 6) == 0;
}

// ---- PeerManager Implementation ----
// PeerManager::PeerManager() {
//     memset(parent_node, 0, 6);
//     memset(child_node, 0, 12);
// }

void PeerManager::addEspNowPeer(const uint8_t *mac) {
    // Implementation here
}

String PeerManager::toString() const {
    // Implementation here
    return String();
}

String PeerManager::macToString(const uint8_t *mac) const {
    char buf[18];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}

void PeerManager::addPeer(const uint8_t *mac, int rssi) {
    // Implementation here
}

bool PeerManager::findChild(const uint8_t *mac) const {
    // Implementation here
    return false;
}

void PeerManager::sendToParent(const uint8_t *toMacAddress, const uint8_t *fromMacAddress, const uint8_t *DestinationMacAddr, const uint8_t data[100]) const {
    // Implementation here
}

void PeerManager::sendToChild(const uint8_t *toChildMacAddress, const uint8_t data[100], const uint8_t length) const {
    // Implementation here
}

void PeerManager::broadcastToAllPeers(const Message &msg) {
    // Implementation here
}

// ---- Message Struct Constructors ----
DiscorveryMessage::DiscorveryMessage() {
    type = DISCORVERY;
    memset(mac, 0, 6);
    length = 0;
}

ReplyDiscorvery::ReplyDiscorvery() {
    type = DISCORVERY;
    memset(mac, 0, 6);
    rssi = 0;
    length = 0;
}

Message::Message() {
    type = DATA;
    memset(to, 0, 6);
    memset(from, 0, 6);
    memset(data, 0, 100);
    length = 0;
}
