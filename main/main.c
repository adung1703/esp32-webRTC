#include <stdio.h>
#include <string.h>
#include "address.h"
#include "esp_log.h"

// ESP32 log tag
static const char *TAG = "ADDRESS_TEST";

void app_main() {
    Address addr1, addr2, addr3;
    char addr_str[ADDRSTRLEN];

    // Test IPv4 address (e.g., 192.168.1.1)
    if (addr_from_string("192.168.1.1", &addr1)) {
        addr_set_port(&addr1, 8080);
        addr_to_string(&addr1, addr_str, sizeof(addr_str));
        ESP_LOGI(TAG, "IPv4 address 1: %s:%d", addr_str, ntohs(addr1.sin.sin_port));
    } else {
        ESP_LOGE(TAG, "Invalid IPv4 address");
    }

    // Test IPv6 address (e.g., 2001:0db8:85a3:0000:0000:8a2e:0370:7334)
    if (addr_from_string("2001:0db8:85a3:0000:0000:8a2e:0370:7334", &addr2)) {
        addr_set_port(&addr2, 9090);
        addr_to_string(&addr2, addr_str, sizeof(addr_str));
        ESP_LOGI(TAG, "IPv6 address 1: %s:%d", addr_str, ntohs(addr2.sin6.sin6_port));
    } else {
        ESP_LOGE(TAG, "Invalid IPv6 address");
    }

    // Test equality of two addresses (IPv4)
    if (addr_equal(&addr1, &addr3)) {
        ESP_LOGI(TAG, "Address 1 and Address 3 are equal");
    } else {
        ESP_LOGI(TAG, "Address 1 and Address 3 are not equal");
    }

    // Test equality of two addresses (IPv6)
    if (addr_equal(&addr2, &addr3)) {
        ESP_LOGI(TAG, "Address 2 and Address 3 are equal");
    } else {
        ESP_LOGI(TAG, "Address 2 and Address 3 are not equal");
    }
}
