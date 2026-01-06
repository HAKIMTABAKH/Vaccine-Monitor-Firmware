#ifndef MQTT_APP_H
#define MQTT_APP_H

void mqtt_app_start(void);
void mqtt_send_data(float temp, float humidity, uint32_t timestamp, uint8_t battery_level, uint8_t status_flags);

#endif