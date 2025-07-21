#include <lib/DisplayMenagement.h>
#include <lib/NodeManagement.h>

DisplayManager::DisplayManager(PeerManager *pm)
    : peer_manager(pm), scroll_position(0), last_update(0) {}

void DisplayManager::initialize() {
    M5.begin();
    display_messages.clear();
    scroll_position = 0;
    last_update = millis();
}

void DisplayManager::update() {
    if (!shouldUpdate()) return;
    M5.Display.clear();
    drawHeader();
    drawPeers();
    drawMessages();
    last_update = millis();
}

void DisplayManager::addMessage(const String &message) {
    display_messages.push_back(message);
    if (display_messages.size() > 100) {
        display_messages.erase(display_messages.begin());
    }
}

void DisplayManager::clearMessages() {
    display_messages.clear();
    scroll_position = 0;
}

void DisplayManager::scrollUp() {
    if (scroll_position > 0) scroll_position--;
}

void DisplayManager::scrollDown() {
    if (scroll_position < (int)display_messages.size() - 1) scroll_position++;
}

bool DisplayManager::shouldUpdate() const {
    return (millis() - last_update) > 100;
}

void DisplayManager::drawHeader() {
    M5.Display.setCursor(0, 0);
    M5.Display.setTextSize(2);
    M5.Display.print("Mesh Network");
}

void DisplayManager::drawPeers() {
    if (!peer_manager) return;
    M5.Display.setCursor(0, 20);
    M5.Display.setTextSize(1);
    M5.Display.print("Peers: ");
    M5.Display.print(peer_manager->toString());
}

void DisplayManager::drawMessages() {
    int y = 40;
    M5.Display.setTextSize(1);
    for (size_t i = scroll_position; i < display_messages.size() && y < M5.Display.height(); ++i, y += 12) {
        M5.Display.setCursor(0, y);
        M5.Display.print(display_messages[i]);
    }
}