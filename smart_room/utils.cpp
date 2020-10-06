#include "utils.h"

void LedBlink(int blink_times, int last_ms, int interval_ms)
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  for (size_t i = 0; i < blink_times; i++)
  {
    digitalWrite(PIN_LED, LOW);
    delay(last_ms);
    digitalWrite(PIN_LED, HIGH);
    delay(interval_ms);
  }
}

String Hmac256(String str, String ds)
{
  byte hash_code[32];
  SHA256 sha256;

  const char *key = ds.c_str();
  size_t key_len = ds.length();

  sha256.resetHMAC(key, key_len);
  sha256.update((const byte *)str.c_str(), str.length());
  sha256.finalizeHMAC(key, key_len, hash_code, sizeof(hash_code));

  String sign = "";
  for (byte i = 0; i < 32; ++i)
  {
    sign += "0123456789ABCDEF"[hash_code[i] >> 4];
    sign += "0123456789ABCDEF"[hash_code[i] & 0xf];
  }

  return sign;
}

bool WiFiManager::AutoConfig()
{
  Serial.print("\nAutoConfig Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  for (int i = 0; i < 10; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nAutoConfig Success!");
      Serial.printf("SSID:%s\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\n", WiFi.psk().c_str());
      return true;
    }

    Serial.print(".");
    LedBlink(1, 100);
    delay(900);
  }
  Serial.println("\nAutoConfig Faild!");
  return false;
}

bool WiFiManager::SmartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\nWaiting for Smartconfig");
  WiFi.beginSmartConfig();
  for (int i = 0; i <= SMART_CONFIG_TIMEOUT * 2; i++)
  {
    Serial.print(".");
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true); // 设置自动连接
      return true;
    }
    LedBlink(1, 100);
    delay(400);
  }
  return false;
}

bool WiFiManager::ConnectWifi()
{
  if (WiFi.status() == WL_CONNECTED)
    return true; // 已经连接

  if (AutoConfig()) // 尝试连接上次保存的 Wifi
    return true;

  if (SmartConfig()) // 使用手机智能配网
    return true;

  return false; // 还是连不上
}

//--------------------- End of WiFi Manager -------------------------------

FingerprintManager::FingerprintManager()
{
  fp_serial = new SoftwareSerial(PIN_FP_TX, PIN_FP_RX);
  finger = new Adafruit_Fingerprint(fp_serial, FINGERPRINT_PASSWORD);
  finger->begin(9600); // 高于 9600 会导致无法连接指纹模块
}

FingerprintManager::~FingerprintManager()
{
  delete fp_serial;
  delete finger;
}

bool FingerprintManager::ConnectFingerprintModule()
{
  if (finger->verifyPassword())
  {
    Serial.println("Found fingerprint sensor successfully!");
    return true;
  }
  Serial.println("Did not find fingerprint sensor!!!");
  return false;
}

int FingerprintManager::GetFingerprintNum()
{
  finger->getTemplateCount();
  Serial.print("Fingerprint num:");
  int num = finger->templateCount;
  Serial.println(num);
  return num;
}

int FingerprintManager::CheckFingerprint()
{
  int p = finger->getImage();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger->image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger->fingerFastSearch();
  if (p != FINGERPRINT_OK)
    return -1;

  return finger->fingerID;
}

void FingerprintManager::CleanAllFingerprint()
{
  Serial.println("Clean all fingerprint...");
  finger->emptyDatabase();
}

void FingerprintManager::EnrollFingerprint()
{
  int id = GetFingerprintNum() + 1;
  int stat = -1;
  Serial.println("Please put your finger to the module.");
  Serial.print("Your fingerprint will be stored as ID:");
  Serial.println(id);
  LedBlink(2, 150, 150);
  do
  {
    stat = finger->getImage();
  } while (stat != FINGERPRINT_OK);
  Serial.println("Please remove your finger->");
  finger->image2Tz(1);
  LedBlink(1, 1000);
  delay(1000);

  LedBlink(2, 150, 150);
  Serial.println("Please put your finger again :)");
  do
  {
    stat = finger->getImage();
  } while (stat != FINGERPRINT_NOFINGER);
  do
  {
    stat = finger->getImage();
  } while (stat != FINGERPRINT_OK);
  finger->image2Tz(2);
  LedBlink(1, 1000);
  delay(500);

  Serial.println("Creating model for your fingerprint now...");
  stat = finger->createModel();
  if (stat == FINGERPRINT_OK)
  {
    Serial.println("Two fingerprints matches!");
  }
  else
  {
    LedBlink(6, 100, 100);
    Serial.println("Something wrong,try again.");
    return;
  }

  stat = finger->storeModel(id);
  if (stat == FINGERPRINT_OK)
  {
    LedBlink(1, 1300);
    Serial.println("Fingerprint stored!");
  }
  else
  {
    LedBlink(6, 100, 100);
    Serial.println("Store fingprint error!");
    return;
  }
}

void FingerprintManager::DeleteFingerprint(unsigned int id)
{
  if (finger->deleteModel(id) == FINGERPRINT_OK)
  {
    Serial.print("Fingerprint Deleted: ID#");
    Serial.println(id);
  }
  else
  {
    Serial.println("Fingerprint Delete error");
  }
}

// ---------- End of FingerprintManager ---------------

void ESPManager::Reboot()
{
  Serial.println("Reboot ESP8266...");
  ESP.restart();
}

void ESPManager::OpenDoor()
{
  Serial.println("FBI,Open the door!");
  door_servo.attach(PIN_DOOR);
  door_servo.write(50);
  delay(2000);
  door_servo.write(0);
  delay(300);
  door_servo.detach();
}

void ESPManager::OpenLightOne()
{
  Serial.println("Light One: ON");
  light_one_servo.attach(PIN_LIGHT_ONE);
  light_one_servo.write(10);
  delay(200);
  light_one_servo.detach();
}

void ESPManager::CloseLightOne()
{
  Serial.println("Light One: OFF");
  light_one_servo.attach(PIN_LIGHT_ONE);
  light_one_servo.write(40);
  delay(200);
  light_one_servo.detach();
}

void ESPManager::OpenLightTwo()
{
  Serial.println("Light Two: ON");
  light_two_servo.attach(PIN_LIGHT_TWO);
  light_two_servo.write(120);
  delay(200);
  light_two_servo.detach();
}

void ESPManager::CloseLightTwo()
{
  Serial.println("Light Two: OFF");
  light_two_servo.attach(PIN_LIGHT_TWO);
  light_two_servo.write(75);
  delay(200);
  light_two_servo.detach();
}

void ESPManager::OpenHotWater()
{
  Serial.println("Hot Water: ON");
  pinMode(PIN_WATER, OUTPUT);
  digitalWrite(PIN_WATER, HIGH);
}

void ESPManager::CloseHotWater()
{
  Serial.println("Hot Water: OFF");
  pinMode(PIN_WATER, OUTPUT);
  digitalWrite(PIN_WATER, LOW);
}
