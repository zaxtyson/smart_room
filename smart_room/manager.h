#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H
#include "utils.h"
#include "aliyun.h"

class RoomManager
{
public:
    static ESPManager *esp; // static 是为了能在事件回调函数调用
    static Aliyun *client;
    static FingerprintManager *fp;
    static WiFiManager *wifi;

public:
    RoomManager();
    ~RoomManager();
    static void EventHandler(char *topic, byte *payload, unsigned int length); // 事件回调函数
    static void CheckServerConnection();
    static void CheckFingerprintModule();
    static void OpenDoorWithFingerprint();
    static void WaitServerInstructions();
};

#endif
