#ifndef MQTT_H
#define MQTT_H

#include <freertos/queue.h>

extern QueueHandle_t queue_to_esp;
extern QueueHandle_t queue_to_mqtt;
extern TaskHandle_t xMqttHandle;

void init_mqtt_communication();

#endif  // MQTT_H