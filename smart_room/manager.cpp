#include "manager.h"

WiFiManager *RoomManager::wifi = new WiFiManager;
ESPManager *RoomManager::esp = new ESPManager;
Aliyun *RoomManager::client = new Aliyun(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, SERVER_REGION);
FingerprintManager *RoomManager::fp = new FingerprintManager;

RoomManager::RoomManager()
{
    client->SetEventHandler(EventHandler);
}

RoomManager::~RoomManager()
{
    delete fp;
    delete client;
    delete wifi;
    delete esp;
}

void RoomManager::CheckWifiConnection()
{
    for (int i = 0; i < MAX_CONNECT_TIMES; i++)
    {
        if (wifi->ConnectWifi())
            return;
    }
}

void RoomManager::CheckFingerprintModule()
{
    for (int i = 0; i < MAX_CONNECT_TIMES; i++)
    {
        if (fp->ConnectFingerprintModule())
            return;
        client->ReportEvent("fingerprint_status", "status", 1); // 指纹模块异常, 上报云端
        LedBlink(3, 1000, 500);                                 // 闪灯 3 次警告
    }
}

void RoomManager::OpenDoorWithFingerprint()
{
    int finger_id = fp->CheckFingerprint();
    if (finger_id != -1)
    {
        // 上报开门事件, 钉钉推送提醒给舍友, 防止舵机机械传动部分异常无法开门
        // 上报指纹 ID 用于识别用户
        client->ReportEvent("door_opened", "fid", finger_id);
        esp->OpenDoor();
        LedBlink(1, 500);
    }
}

void RoomManager::WaitServerInstructions()
{
    client->Run(); // 等待服务器信息并响应
}

void RoomManager::EventHandler(char *topic, byte *payload, unsigned int length) // 事件处理回调函数
{
    LedBlink(1, 100); // 收到消息闪一下
    String topic_str = String(topic);
    Serial.print("Receive from topic: ");
    Serial.println(topic_str.c_str());

    // 录入指纹
    if (topic_str.endsWith("enroll_fingerprint"))
    {
        fp->EnrollFingerprint();
        client->ReportProperty("fingerprint_num", fp->GetFingerprintNum());
    }

    // 清空指纹库
    else if (topic_str.endsWith("clean_fingerprint"))
    {
        fp->CleanAllFingerprint();
        client->ReportProperty("fingerprint_num", 0);
    }

    // 开门
    else if (topic_str.endsWith("open_door"))
    {
        // 上报开门事件(fid=-1表示使用远程解锁), 钉钉推送提醒
        client->ReportEvent("door_opened", "fid", -1);
        esp->OpenDoor();
    }

    // 开启灯1
    else if (topic_str.endsWith("light_one_on"))
    {
        esp->OpenLightOne();
        client->ReportProperty("light_one_status", 1);
    }

    // 关闭灯1
    else if (topic_str.endsWith("light_one_off"))
    {
        esp->CloseLightOne();
        client->ReportProperty("light_one_status", 0);
    }

    // 开启灯2
    else if (topic_str.endsWith("light_two_on"))
    {
        esp->OpenLightTwo();
        client->ReportProperty("light_two_status", 1);
    }

    // 关闭灯2
    else if (topic_str.endsWith("light_two_off"))
    {
        esp->CloseLightTwo();
        client->ReportProperty("light_two_status", 0);
        return;
    }

    // 开启饮水机
    else if (topic_str.endsWith("hot_water_on"))
    {
        esp->OpenHotWater();
        client->ReportProperty("hot_water_status", 1);
    }

    // 关闭饮水机
    else if (topic_str.endsWith("hot_water_off"))
    {
        esp->CloseHotWater();
        client->ReportProperty("hot_water_status", 0);
    }
}
