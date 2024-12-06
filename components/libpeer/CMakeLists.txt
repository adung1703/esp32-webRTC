cmake_minimum_required(VERSION 3.1)

include(${CMAKE_CURRENT_LIST_DIR}/third_party/coreHTTP/httpFilePaths.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party/coreMQTT/mqttFilePaths.cmake)

if (DEFINED ENV{IDF_PATH})
  file(GLOB ESP32_CODES "./src/*.c")
  idf_component_register(
   SRCS ${ESP32_CODES} ${HTTP_SOURCES} ${MQTT_SOURCES} ${MQTT_SERIALIZER_SOURCES}
   INCLUDE_DIRS "./src" ${HTTP_INCLUDE_PUBLIC_DIRS} ${MQTT_INCLUDE_PUBLIC_DIRS}
   REQUIRES mbedtls srtp json esp_netif
  )
  add_definitions("-DCONFIG_USE_LWIP=1" "-DCONFIG_USE_USRSCTP=0" "-DCONFIG_AUDIO_BUFFER_SIZE=8096" "-DCONFIG_DATA_BUFFER_SIZE=102400" "-DHTTP_DO_NOT_USE_CUSTOM_CONFIG" "-DMQTT_DO_NOT_USE_CUSTOM_CONFIG")
  return()
endif()

