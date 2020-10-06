#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// 阿里云认证三元组
const String PRODUCT_KEY = "";
const String DEVICE_NAME = "";
const String DEVICE_SECRET = "";
const String SERVER_REGION = "cn-shanghai";

// MQTT 配置
const unsigned long CHECK_INTERVAL = 20000; // 心跳包间隔(ms)
const int MQTT_PORT = 1883;                 // MQTT 端口

// 硬件对应的针脚
const uint8_t PIN_LED = 2;        // D4
const uint8_t PIN_DOOR = 0;       // D3
const uint8_t PIN_LIGHT_ONE = 13; // D7
const uint8_t PIN_LIGHT_TWO = 12; // D6
const uint8_t PIN_WATER = 14;     // S3 饮水机继电器
const uint8_t PIN_FP_TX = 4;      // D2 指纹模块 TX 线接口
const uint8_t PIN_FP_RX = 5;      // D1 指纹模块 RX 线接口

// 其它配置
const int MAX_CONNECT_TIMES = 3;         // wifi 连接重连次数
const int SMART_CONFIG_TIMEOUT = 30;     // 智能配网超时(s)
const uint32_t FINGERPRINT_PASSWORD = 0; // 指纹模块连接密码

#endif