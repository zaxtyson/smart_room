#include "manager.h"

RoomManager *manager;

void setup()
{
  Serial.begin(115200);
  manager = new RoomManager();
  manager->CheckServerConnection();
  manager->CheckFingerprintModule();
}

void loop()
{
  manager->OpenDoorWithFingerprint();
  manager->WaitServerInstructions();
}
