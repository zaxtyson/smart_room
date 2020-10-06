#include "aliyun.h"

Aliyun::Aliyun(String product_key, String device_name, String device_secret, String region)
{
    client = new PubSubClient(espClient);
    // 通信 Topic
    event_post_topic_prefix = "/sys/" + product_key + "/" + device_name + "/thing/event/"; // ${event_name}/post
    property_post_topic = "/sys/" + product_key + "/" + device_name + "/thing/event/property/post";
    service_sub_topic = "/sys/" + product_key + "/" + device_name + "/thing/service/+";
    // MQTT 信息
    mqtt_username = device_name + "&" + product_key;
    mqtt_broker_addr = product_key + ".iot-as-mqtt." + region + ".aliyuncs.com";
    mqtt_client_id = device_name + "|securemode=3,signmethod=hmacsha256|";
    String sign = "clientId" + device_name + "deviceName" + device_name + "productKey" + product_key;
    mqtt_password = Hmac256(sign, device_secret);

    client->setServer(mqtt_broker_addr.c_str(), MQTT_PORT);
}

Aliyun::~Aliyun() { delete client; }

void Aliyun::SetEventHandler(callback handler)
{
    client->setCallback(handler);
}

bool Aliyun::ConnectServer()
{
    Serial.println("Check connection to Aliyun IoT Server...");
    if (client->connected())
    {
        Serial.println("Connect server success!");
        return true;
    }

    client->disconnect();
    if (client->connect(mqtt_client_id.c_str(), mqtt_username.c_str(), mqtt_password.c_str()))
    {
        Serial.println("Connect server success!");
        return true;
    }

    Serial.print("Connect error, reason:");
    Serial.println(client->state());
    return false;
}

void Aliyun::PubMessage(String topic, String msg)
{
    String payload = "{\"params\":" + msg + "}";
    client->publish(topic.c_str(), payload.c_str());
    Serial.println("Report to Topic: " + topic);
    Serial.println("Payload: " + payload);
}

void Aliyun::ReportProperty(String key, String value)
{
    String msg = "{\"" + key + "\": \"" + value + "\"}";
    PubMessage(property_post_topic, msg);
}

void Aliyun::ReportProperty(String key, int value)
{
    String msg = "{\"" + key + "\":" + value + "}";
    PubMessage(property_post_topic, msg);
}

void Aliyun::ReportEvent(String event, String key, int value)
{
    String topic = event_post_topic_prefix + event + "/post";
    String msg = "{\"" + key + "\":" + value + "}";
    PubMessage(topic, msg);
}

void Aliyun::Run()
{
    client->loop();
    if (millis() - last_ms >= CHECK_INTERVAL)
    {
        last_ms = millis();
        ConnectServer();
    }
}
