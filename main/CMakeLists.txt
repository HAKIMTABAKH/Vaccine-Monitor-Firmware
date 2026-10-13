idf_component_register(SRCS "main.c" "wifi_connect.c" "mqtt_app.c" 
                       INCLUDE_DIRS "include"
                       REQUIRES nvs_flash esp_wifi esp_event lwip spiffs mqtt)