#include "NodeManagement.h"

#include <esp_now.h>
#include <WiFi.h>
#include <M5Unified.h>
#include <vector>
#include <map>
#include <memory>
class DisplayManager
{
private:
  std::vector<String> display_messages;
  int scroll_position;
  unsigned long last_update;
  PeerManager *peer_manager;

  void drawHeader();
  void drawPeers();
  void drawMessages();

public:
  DisplayManager(PeerManager *pm);

  void initialize();
  void update();
  void addMessage(const String &message);
  void clearMessages();
  void scrollUp();
  void scrollDown();

  bool shouldUpdate() const;
};
