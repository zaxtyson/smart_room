#ifndef UTILS_H
#define UTILS_H
#include "config.h"
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <SHA256.h>
#include <Adafruit_Fingerprint.h>

void LedBlink(int blink_times, int last_ms, int interval_ms = 0); // LED 闪烁

String Hmac256(String sign, String ds); // sha256 加密算法

class WiFiManager
{
private:
    bool AutoConfig();  // 尝试自动连接 wifi
    bool SmartConfig(); // 手机智能配网
public:
    bool ConnectWifi(); // 尝试连接wifi
};

class FingerprintManager
{
private:
    SoftwareSerial *fp_serial;
    Adafruit_Fingerprint *finger;

public:
    FingerprintManager();
    ~FingerprintManager();
    bool ConnectFingerprintModule();         // 检查是否连接指纹模块
    int GetFingerprintNum();                 // 获取保存指纹总数
    int CheckFingerprint();                  // 尝试匹配指纹, 返回指纹 ID
    void CleanAllFingerprint();              // 清空指纹库
    void EnrollFingerprint();                // 录入指纹
    void DeleteFingerprint(unsigned int id); // 删除指纹
};

class ESPManager
{
private:
    Servo door_servo;
    Servo light_one_servo;
    Servo light_two_servo;

public:
    void Reboot();        //重启单片机
    void OpenDoor();      // 开门
    void OpenLightOne();  // 开启灯1
    void CloseLightOne(); // 关闭灯1
    void OpenLightTwo();  // 开启灯2
    void CloseLightTwo(); // 关闭灯2
    void OpenHotWater();  // 开启饮水机
    void CloseHotWater(); // 关闭饮水机
};

#endif
