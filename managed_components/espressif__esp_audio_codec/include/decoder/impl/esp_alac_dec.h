/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2024 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#include "esp_audio_dec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  ALAC decoder configuration (required)
 */
typedef struct {
    uint8_t *codec_spec_info; /*!< Codec specified information (Magic Cookie) */
    uint32_t spec_info_len;   /*!< Codec specified information length */
} esp_alac_dec_cfg_t;

/**
 * @brief  Default decoder operations for ALAC
 */
#define ESP_ALAC_DEC_DEFAULT_OPS() { \
    .open = esp_alac_dec_open,       \
    .decode = esp_alac_dec_decode,   \
    .close = esp_alac_dec_close,     \
}

/**
 * @brief  Register ALAC decoder
 *
 * @note  If user want to use decoder through decoder common API, need register it firstly.
 *        Register can use either of following methods:
 *          1: Manually call `esp_alac_dec_register`.
 *          2: Call `esp_audio_dec_register_default` and use menuconfig to enable it.
 *        When user want to use ALAC decoder only and not manage it by common part, no need to call this API,
 *        And call `esp_alac_dec_open`, `esp_alac_dec_decode`, `esp_alac_dec_close` instead.
 *
 * @return
 *       - ESP_AUDIO_ERR_OK        On success
 *       - ESP_AUDIO_ERR_MEM_LACK  Fail to allocate memory
 */
esp_audio_err_t esp_alac_dec_register(void);

/**
 * @brief  Open ALAC decoder
 *
 * @param[in]   cfg         Pointer to `esp_alac_dec_cfg_t`
 * @param[in]   cfg_sz      Set to sizeof(esp_alac_dec_cfg_t)
 * @param[out]  dec_handle  The ALAC decoder handle
 *
 * @return
 *       - ESP_AUDIO_ERR_OK                 On success
 *       - ESP_AUDIO_ERR_MEM_LACK           Fail to allocate memory
 *       - ESP_AUDIO_ERR_INVALID_PARAMETER  Invalid parameter
 *       - ESP_AUDIO_ERR_FAIL               Fail to initial decoder
 */
esp_audio_err_t esp_alac_dec_open(void *cfg, uint32_t cfg_sz, void **dec_handle);

/**
 * @brief  Decode ALAC encoded data
 *
 * @param[in]      dec_handle  Decoder handle
 * @param[in,out]  raw         Raw data to be decoded
 * @param[in,out]  frame       Decoded PCM frame data
 * @param[out]     dec_info    Information of decoder
 *
 * @return
 *       - ESP_AUDIO_ERR_OK                 On success
 *       - ESP_AUDIO_ERR_MEM_LACK           Fail to allocate memory
 *       - ESP_AUDIO_ERR_INVALID_PARAMETER  Invalid parameter
 *       - ESP_AUDIO_ERR_BUFF_NOT_ENOUGH    No enough frame buffer to hold output PCM frame data
 *       - ESP_AUDIO_ERR_FAIL               Fail to decode data
 */
esp_audio_err_t esp_alac_dec_decode(void *dec_handle, esp_audio_dec_in_raw_t *raw, esp_audio_dec_out_frame_t *frame,
                                      esp_audio_dec_info_t *dec_info);

/**
 * @brief  Close ALAC decoder
 *
 * @param[in]  dec_handle  Decoder handle
 *
 * @return
 *       - ESP_AUDIO_ERR_OK                 On success
 *       - ESP_AUDIO_ERR_INVALID_PARAMETER  Invalid parameter
 */
esp_audio_err_t esp_alac_dec_close(void *dec_handle);

#ifdef __cplusplus
}
#endif