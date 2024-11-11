# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/thien-gay/esp-idf/esp-idf/components/bootloader/subproject"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/tmp"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/src/bootloader-stamp"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/src"
  "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/thien-gay/esp-idf/esp32-webRTC/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
