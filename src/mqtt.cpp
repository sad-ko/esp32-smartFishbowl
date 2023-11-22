#include <Arduino.h>
#include <ArduinoJson.h>
#include <UbidotsEsp32Mqtt.h>
#include <defines.h>
#include <mqtt.h>
#include <state_machine.h>

/* -- Credenciales -- */
const char *UBIDOTS_TOKEN = "BBFF-VfVWtbeDj6PUpfZmWTcyqCfiqUOTRj";
const char *WIFI_SSID = "Wokwi-GUEST";
const char *WIFI_PASS = "";
const char *SUBS_TOPIC = "/v2.0/devices/android-smart-fishbowl/send-event";
const char *PUB_DEV = "esp32-smart-fishbowl";
const char *PUB_VAR = "send-state";

QueueHandle_t queue_to_esp;
QueueHandle_t queue_to_mqtt;
TaskHandle_t xMqttHandle;

enum events tmp_event = EV_CAPTURED;
enum states tmp_state = ST_IDLE;
Ubidots ubidots(UBIDOTS_TOKEN);
BaseType_t xReturned;
bool old_msg = true;

void callback(char *topic, byte *payload, unsigned int length)
{
  // Pequeño parche para ignorar el mensaje viejo que llega siempre al subscribirse.
  if (old_msg == true)
  {
    old_msg = false;
    return;
  }

  DebugPrint("Message arrived [");
  DebugPrint(topic);
  DebugPrint("] ");
  for (int i = 0; i < length; i++)
  {
    DebugPrint((char)payload[i]);
  }
  DebugPrintln();

  StaticJsonDocument<64> json;
  DeserializationError json_rc = deserializeJson(json, payload);
  if (json_rc != DeserializationError::Ok)
  {
    DebugPrint("[Error] - ");
    DebugPrint(F("deserializeJson() failed: "));
    DebugPrintln(json_rc.f_str());
    return;
  }

  // Recibimos un mensaje desde mqtt...
  // Notificamos al core del esp-32 sobre este evento.
  tmp_event = (enum events)json["value"];
  xReturned = xQueueSend(queue_to_esp, (void *)&tmp_event, (TickType_t)QUEUE_NO_WAIT);
  if (xReturned != pdPASS)
  {
    DebugPrintln("[Error] - No se pudo enviar el mensaje al nucleo principal.");
  }
}

bool mqtt_connect()
{
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setup();
  ubidots.connect();
  ubidots.setCallback(callback);
  return ubidots.subscribe(SUBS_TOPIC);
}

void mqtt_task(void *pvParameters)
{
  if (mqtt_connect() == false)
  {
    DebugPrintln("[Error] - No se pudo conectar con ubidots.");
    // TODO: Error handling
  }

  while (true)
  {
    ubidots.loop();
    if (!ubidots.connected())
    {
      ubidots.reconnect();
      ubidots.subscribe(SUBS_TOPIC);
    }

    xReturned = xQueueReceive(queue_to_mqtt, &(tmp_state), (TickType_t)QUEUE_NO_WAIT);
    if (xReturned == pdPASS)
    {
      // Enviamos el nuevo estado al mqtt...
      DebugPrint("Publicando mensaje: ");
      DebugPrintln(g_states_str[tmp_state]);
      ubidots.add(PUB_VAR, (float)tmp_state);
      ubidots.publish(PUB_DEV);
    }
  }
}

void init_mqtt_communication()
{
  byte idle_core = (xPortGetCoreID() + 1) % 2;

  // Esta queue le envia los eventos a la esp-32
  queue_to_esp = xQueueCreate(QUEUE_MAX, sizeof(enum events));
  // Esta queue envia el nuevo estado de la esp-32
  queue_to_mqtt = xQueueCreate(QUEUE_MAX, sizeof(enum states));

  if (queue_to_esp == NULL || queue_to_mqtt == NULL)
  {
    DebugPrintln("[Error] - No se pudieron crear las colas de mensajes.");
  }

  BaseType_t xReturned =
      xTaskCreatePinnedToCore(mqtt_task, "mqtt task", QUEUE_STACK_SIZE, NULL,
                              tskIDLE_PRIORITY, &xMqttHandle, idle_core);

  if (xReturned != pdPASS)
  {
    DebugPrintln("[Error] - No se pudo crear tarea en el otro nucleo.");
  }
}