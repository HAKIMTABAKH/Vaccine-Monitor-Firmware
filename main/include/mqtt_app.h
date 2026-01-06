#ifndef MQTT_APP_H
#define MQTT_APP_H

void mqtt_app_start(void);
void mqtt_send_data(float temperature, uint32_t timestamp);

#endif