#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// Credenciales Wi-Fi
const char* WIFI_SSID = "C21 URBAN";
const char* WIFI_PASS = "AAV29JPI05";

// MQTT AWS IoT
const char* MQTT_BROKER = "a2dbaocrje4klj-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32";

// Tópicos MQTT
const char* UPDATE_TOPIC = "$aws/things/my_thing/shadow/update";
const char* UPDATE_DELTA_TOPIC = "$aws/things/my_thing/shadow/update/delta";
const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIURZMbK5DswxnFTGJFhcx23n8sqWswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDUxMTA1MTcz
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALgWKtRJG2eaRpTsBap3
sOy7dr1xKTmYWKMipWWIfsEOwPLXoydPhYZX68b1ur5ZZAs+8UzsWLQxTpH8gMT8
KkalflYrmKbbdl3i3WmtaDdunJofJfSeszDNLvXX03kJWI9mrv678Xe/pHmFYjlW
VXe0j3OpYGGUGBbP5sf1WtBgqlDmBHcJiUFmGwLuNCWHmi9AeEcFrWQUNJMCxjvU
3TjVdgOLKL49kCkWjRF8w4eA7ZcOVOJIFrM09i6mDkRC6lUPEcgr9r1dHOcvNVYV
JExUaU9dwD8G3LBwe/XgDX+gq+W+c62Ued40Fdf/TEDs5rOx9uIyWiBdgdKvOQqz
bOECAwEAAaNgMF4wHwYDVR0jBBgwFoAUll7npSJBsmQ1Fe8mnIx3dRMXivcwHQYD
VR0OBBYEFGVuz6ZJuYeWZkj92nsetdpFD4WdMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCmbB4V0AWc2G4uqmJYDHsgKrPk
i8sf+BdLsU0LTfcjQj+tJECTCdJZ/WJkxztztkNZjeXb3C4KSKHeqG2aMds0zPOA
sozy3f2q8CPiGXPw/5aJKDgJ4nu561wswxNMfkK2liCamh8KQqBjSXhPJcttG1bm
ju+z1pWVOKyRmanufBl6xnmjJYRDbI3TdYn2ZGr83tzRT4g0EAko3phZ2kNMf11g
NoAQAOENOSn7UMQSZab+YkJX7mmqPZJqx3SD9KLTf8gxUlPZVV0f8gEp3cM3xy5m
OVvsHcm+RYeYSKS70Q5disdr6fXvIxyCwAkD3N/zt8YBK48g5h0JZ7pJKX1V
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAuBYq1EkbZ5pGlOwFqnew7Lt2vXEpOZhYoyKlZYh+wQ7A8tej
J0+FhlfrxvW6vllkCz7xTOxYtDFOkfyAxPwqRqV+ViuYptt2XeLdaa1oN26cmh8l
9J6zMM0u9dfTeQlYj2au/rvxd7+keYViOVZVd7SPc6lgYZQYFs/mx/Va0GCqUOYE
dwmJQWYbAu40JYeaL0B4RwWtZBQ0kwLGO9TdONV2A4sovj2QKRaNEXzDh4Dtlw5U
4kgWszT2LqYORELqVQ8RyCv2vV0c5y81VhUkTFRpT13APwbcsHB79eANf6Cr5b5z
rZR53jQV1/9MQOzms7H24jJaIF2B0q85CrNs4QIDAQABAoIBAE+LjKBEounmuT8a
qewaYkI5UzrTDixMD2kWzH+ONq93j5qf4WlYLGt7zG8hubsM53UQkrZZno7mi3v7
ryzvhdkXI/taRVE8ne493M0QWGszicnzUbRdazLYRsWGqGOxzSnSBiYe0l2bWxgD
sgjyrGXYiPBCrVoc4zfgSGn5NG+7yd1d5N1U0EZ11+4jeQjPB0+GcaoKuXcr1htc
ew0oXuoKTCIuQ/xfb/rg7Gj4V15gNfqg+4qPNA7k4aM1Su/JeclrSkwbhj6ZMSi5
xVDnlgRB5DmHK9XUywUiY4syNP9wFrKLqlKUe17ZCusznctbWVsDc6FWK7UYrkaP
e8FQptECgYEA23rwGUYpqvqnVadbpV9DID4WMNcJZMN7q9tF1TmgwVLpL2pwLRCq
tXDp9WhMnq/DPvzfrobcOqLz7cJaGBQVrjVHj+ruq+/mkApgRkohD/3MviThq6l/
hu58EtZGyp9mvgwBaxBPYxHyMajUgq5SUHj7gMQsv6g1HfqSWiQVEj0CgYEA1reW
vrHUwwcYK/OJvgX4ilXaeTBAO1b0UDUt0YtKEPAnd53wKOYRqZW7sN/BRFJ2AHEP
amG5c8dZGexkbBguYLNM21+WPcZodZOap+WW0NNYMieNXIFYkrbQXNCcTlCEy8qo
GTfmCQlF02dvezvBrYWVKWgo3hXM7ik4fBHe43UCgYAdqIc5YxnOWQCzumHP/wd7
G+izwNssGp5josXVAmyOsz1/gsqVzGLFEfULjISODQI9SL6RPvgTlGGSB2lVR6GG
YPzCneSSp2Bopn5MAUhmeM1qBGHaaJUrDp8FjJ8UUmX32Hd4mEOiB5LK0x2kaxoE
ddTGmbvSSlCjNHECbU3LoQKBgQDGAGiXk3/DFUC6Dwv/Wpw2XTNpvBAPaIFFkbSd
3le/xOZwTQnRfU/eVcDOO5iKu4wrnhb8IAAXf0ZwPGjisW3bKJ0AU2ZFszhS7Hma
yIJKMgrLvWODYZffO4xresgZA/FhyJE5KC4OOSnV6DWKq7FPpbbGh1HgcrQXD5YR
+5zRnQKBgDWwDz9rZX5cE8tFzQl94w1QLS8psk9l6oeGdLhOPBKU1kSB3d1HZpHD
FmkOsjAF3CAU/ny0wO1CXXT8LZH0u1oQr80iF+tDuSxIje01NuPTWewi/k+YqeJr
gfCC0hNCegKom6exUOqenyb/Eh7Z2uwXg5Q3ZTaE+xb5pI2OGEF3
-----END RSA PRIVATE KEY-----
)KEY";

// Servo
Servo myServo;
int servoPin = 26;       // Pin de señal del servo (por ejemplo, GPIO 26)
int servoState = 0;      // Estado: 0 = cerrado, 1 = abierto

// MQTT y JSON
WiFiClientSecure wiFiClient;
PubSubClient client(wiFiClient);
StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
char outputBuffer[128];

// Función que mueve el servo y reporta el estado
void setServoPosition() {
  if (servoState == 1) {
    myServo.write(170);  // abrir hasta 170°, no 180
  } else {
    myServo.write(0);    // cerrar
  }

  delay(300);  // permitir que el servo complete el movimiento

  // Reportar al Device Shadow
  outputDoc["state"]["reported"]["motoropen"] = servoState;
  serializeJson(outputDoc, outputBuffer);
  client.publish(UPDATE_TOPIC, outputBuffer);
}

// Manejar mensajes entrantes desde AWS
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += (char) payload[i];
  Serial.println("Mensaje de " + String(topic) + ": " + message);

  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err && String(topic) == UPDATE_DELTA_TOPIC) {
    // Recibe estado desde atributo led_builtin (puedes cambiar a "servo" si es posible)
    servoState = inputDoc["state"]["motoropen"].as<int>();
    setServoPosition();
  }
}

// Configurar conexión Wi-Fi
void setupWiFi() {
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. IP: ");
  Serial.println(WiFi.localIP());
}

// Reconexión a MQTT si se pierde
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect(CLIENT_ID)) {
      Serial.println("Conectado");
      client.subscribe(UPDATE_DELTA_TOPIC);
      Serial.println("Suscrito a: " + String(UPDATE_DELTA_TOPIC));
      delay(100);
      setServoPosition(); // Reportar estado inicial
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" - Reintentando en 5s");
      delay(5000);
    }
  }
}

// Configuración inicial
void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  myServo.write(0);  // posición segura al inicio
  delay(500);

  setupWiFi();

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}

// Bucle principal
void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}