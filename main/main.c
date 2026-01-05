#include <stdio.h>
#include <stdlib.h> // For rand()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h" // The native logging library
#include "types.h"
#include "esp_spiffs.h"
#include <sys/stat.h>
#include <sys/unistd.h>

//---CONSTANTS---//
#define SENSOR_TASK_STACK_SIZE 4096
#define STORAGE_TASK_STACK_SIZE 4096
#define QUEUE_LENGTH 10

//---LOGGING TAGS---//
// ESP-IDF allows filtering logs by tag. Very professional.
static const char *TAG_SENSOR = "SENSOR";
static const char *TAG_STORAGE = "STORAGE";
static const char *TAG_MAIN = "MAIN";

//---GLOBAL VARIABLES---//
QueueHandle_t data_queue;

//---TASKS---//

void task_sensor(void *pvParameters)
{
    vaccine_data_t packet;
    uint32_t current_simulated_time = 1672531200; // Example start time: Jan 1, 2023

    while(1){
        // 1. Simulate read Data
        packet.timestamp = current_simulated_time++;
        packet.temperature = 2.0 + ((float)(rand()% 100) / 10.0);
        packet.humidity = 50.0 + ((float)(rand() % 20));
        packet.battery_level = 98;
        packet.status_flags = 0;

        // 2. Log Data (Replaces Serial.print)
        // ESP_LOGI(Tag, FormatString, Variables...)
        ESP_LOGI(TAG_SENSOR, "Measured: %.2f C", packet.temperature);

        // 3. Send Data to Queue
        if (xQueueSend(data_queue, &packet, pdMS_TO_TICKS(10) ) != pdPASS) {
            ESP_LOGE(TAG_SENSOR, "Queue Full!"); // LOGE = Error (Red color usually)
        }
        // 4. Wait 1 Second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}      

void task_storage(void *pvParametres)
{
    vaccine_data_t received_packet;
    while (1){
        // Wait forever for data
        if(xQueueReceive(data_queue, &received_packet, portMAX_DELAY) == pdPASS){

            // 2. Open File in APPEND + BINARY mode("ab")
            //  If we used "w", it would delete the file every time.
            FILE *f =fopen("/spiffs/vaccine_data.bin", "ab");
            if (f == NULL){
                ESP_LOGE(TAG_STORAGE, "Failed to open file for writing");
                continue;
            }
            // 3. Write Raw Bytes
            // fwrite(pointer_to_data, size_of_one_element, count, file_handle)
            size_t written = fwrite(&received_packet, sizeof(vaccine_data_t), 1, f);

            // 4. Close immediately to save data
            fclose(f);

            // 5. Verification Log
            if (written == 1){
                ESP_LOGI(TAG_STORAGE, "SAVED to Flash: TS %lu | Temp %.2f",
                         (unsigned long)received_packet.timestamp, received_packet.temperature);
            } else {
                ESP_LOGE(TAG_STORAGE, "Failed to write data to file");
            }
        }
    }
   
}

// Function to mount the file system
void init_spiffs(void)
{
   ESP_LOGI(TAG_MAIN, "Initializing SPIFFS");
   esp_vfs_spiffs_conf_t conf = {
       .base_path = "/spiffs",       // Root directory
       .partition_label = "storage", // MUST match partitions.csv name
       .max_files = 5,
       .format_if_mount_failed = true // Format on first run (Important!)
   };
   // Register the File System
   esp_err_t ret = esp_vfs_spiffs_register(&conf);
   if (ret !=ESP_OK){
    if (ret ==ESP_FAIL){
        ESP_LOGI(TAG_MAIN, "Failed to mount or format filesystem");
    }
    else if (ret == ESP_ERR_NOT_FOUND){
        ESP_LOGI(TAG_MAIN, "Failed to find SPIFFS partition");
    }
    else {
        ESP_LOGI(TAG_MAIN, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return;
   }
    size_t total = 0, used = 0;
    ret= esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK){
        ESP_LOGI(TAG_MAIN,"Failed to get SPIFFS partition information");
    }else{
        ESP_LOGI(TAG_MAIN, "SPIFFS Partition size: total: %d, used: %d", total, used);
    }
}
// --- MAIN ENTRY POINT ---
void app_main(void)
{ 
    // ... Logs init ...
    ESP_LOGI(TAG_MAIN, "System Starting: SCCVM-v1");
    // 1. Create Queue
    // MOUNT FILESYSTEM FIRST
    init_spiffs();
    data_queue = xQueueCreate(QUEUE_LENGTH, sizeof(vaccine_data_t));
    if (data_queue == NULL)
    {
        ESP_LOGE(TAG_MAIN, "Failed to create queue");
        return;
    }
    // 2. Create Tasks
     xTaskCreatePinnedToCore(task_sensor, "Sensor Task", SENSOR_TASK_STACK_SIZE, NULL,5,NULL,1);
     xTaskCreatePinnedToCore(task_storage, "Storage Task", STORAGE_TASK_STACK_SIZE, NULL, 3, NULL, 1);
     ESP_LOGI(TAG_MAIN, "Tasks Created. Main ending (scheduler handles the rest).");
}
