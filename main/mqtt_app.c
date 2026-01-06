#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h" // IMPORTANT: Uses built-in certificates

static const char *TAG = "MQTT";
esp_mqtt_client_handle_t client = NULL;

// Callback: Handles events (Connected, Disconnected, etc.)
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT Connected!");
        // We can subscribe here if we wanted to receive commands
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT Disconnected");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT Error");
        break;
    default:
        break;
    }
}

void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "Starting MQTT Client...");

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://broker.emqx.io:8883",            // MQTTS = SSL
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach, // Auto-Verify Certs
    };

    client = esp_mqtt_client_init(&mqtt_cfg);

    // Register the callback
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);

    // Start the client
    esp_mqtt_client_start(client);
}

// Function to publish data
void mqtt_send_data(float temp, float humidity, uint32_t timestamp, uint8_t battery_level, uint8_t status_flags)
{
    if (client == NULL)
        return;

    char payload[100];
    // Create a JSON string: {"ts": 12345, "temp": 5.5, "humidity": 50.00, "battery_level": 98, "status_flags": 0}
    snprintf(payload, sizeof(payload), "{\"ts\":%lu,\"temp\":%.2f,\"humidity\":%.2f,\"battery_level\":%u,\"status_flags\":%u}", (unsigned long)timestamp, temp, humidity, battery_level, status_flags);

    // Topic: sccvm/device_001/data
    // msg_id = esp_mqtt_client_publish(client, topic, data, len, qos, retain)
    int msg_id = esp_mqtt_client_publish(client, "sccvm/device_001/data", payload, 0, 1, 0);

    if (msg_id != -1)
    {
        ESP_LOGI(TAG, "Sent: %s", payload);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send MQTT message");
    }
}