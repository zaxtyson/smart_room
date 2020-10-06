#ifndef ALIYUN_H
#define ALIYUN_H
#include "utils.h"
#include <PubSubClient.h>

typedef void (*callback)(char *, byte *, unsigned int); // 回调函数指针类型

class Aliyun
{
private:
  String mqtt_client_id;   // 客户端 ID
  String mqtt_username;    // 用户名
  String mqtt_password;    // 连接密码
  String mqtt_broker_addr; // mqtt 服务器地址

  String event_post_topic_prefix; // 用于事件上报的 Topic 前缀(每一个事件有一个topic)
  String property_post_topic;     // 用于属性上报的 Topic
  String service_sub_topic;       // 订阅服务调用的 Topic

  unsigned long last_ms = 0;

  WiFiClient espClient;
  PubSubClient *client;

private:
  void PubMessage(String topic, String msg); // 向指定的 Topic 发布消息(json字符串)
public:
  Aliyun(String product_key, String device_name, String device_secret, String region);
  ~Aliyun();
  void SetEventHandler(callback);                // 设置事件处理函数
  bool ConnectServer();                          // 连接到服务器
  void ReportProperty(String key, String value); // 上报设备属性
  void ReportProperty(String key, int value);
  void ReportEvent(String event, String key, int value); // 事件上报
  void Run();
};
#endif
