/* Project Heartbeat OIT Wearable

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "wifi.h" //protocol_examples_common
//Custom components

#include "afe4404.c"

#include "../components/esp-dsp/modules/common/include/esp_dsp.h"
#include "../components/esp_http_client/esp_http_client.c"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
uint8_t heartrate;
bool dataread = false;
bool sending = false;
// static const char *TAG = "main";
////////////////////////////////////////////////////////////////////////////// FFT settings ///////////////////////////////////////////////////////////////////////////////////

    // Define names for the respective settings wich are use in FFT 
    #define N_SAMPLES 256
    int N = N_SAMPLES;
    // Input test array
    // float x1[N_SAMPLES];

    // Window coefficients
    float wind[N_SAMPLES];
    // working complex array
    float y_cf[N_SAMPLES*2];
    // Pointers to result arrays
    float* y1_cf = &y_cf[0];


    // Sum of y1 and y2
    float sum_y[N_SAMPLES/2];

static esp_err_t PerformFFT(){
    esp_err_t ret;
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret  != ESP_OK)
    {
        ESP_LOGE("FFT", "Not possible to initialize FFT. Error = %i", ret);
        return 0;
    }

    // Generate hann window
    dsps_wind_hann_f32(wind, N);
    // Generate input signal for x1 A=1 , F=0.1
    float x1[N_SAMPLES]; 
   
    for (int i=0 ; i < N ; i++)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        x1[i] = (float) EspSpo2Data(); //casting couch
        // ESP_LOGI(TAG, "$%f;", x1[i]);
    }

    // Convert two input vectors to one complex vector
    for (int i=0 ; i< N ; i++)
    {
        y_cf[i*2 + 0] = x1[i] * wind[i];
    }
    // FFT
    // unsigned int start_b = xthal_get_ccount();
    dsps_fft2r_fc32(y_cf, N);
    // unsigned int end_b = xthal_get_ccount();
    // Bit reverse 
    dsps_bit_rev_fc32(y_cf, N);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(y_cf, N);

    for (int i = 0 ; i < N/2 ; i++) {
        y1_cf[i] = 10 * log10f((y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1])/N);
    }
    // Show power spectrum in 64x10 window from -100 to 0 dB from 0..N/4 samples
    dsps_view(y1_cf, N/2, 64, 10,  -60, 40, '|',&heartrate);
    ESP_LOGI("FFT", "$0 %u;", heartrate);
    dataread = true;
    return ESP_OK;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
                output_len = 0;
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                if (output_buffer != NULL) {
                    free(output_buffer);
                    output_buffer = NULL;
                    output_len = 0;
                }
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }
    return ESP_OK;
}

static void PerformPostReq(){
    sending = true;
    static const char *PRQtag = "Post";
        ESP_LOGI(PRQtag, "Connected to AP, begin http");
        char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
        /**
         * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
         * If host and path parameters are not set, query parameter will be ignored. In such cases,
         * query parameter should be specified in URL.
         *
         * If URL as well as host and path parameters are specified, values of host and path will be considered.
         */
            esp_http_client_config_t config = {
            .host = "192.168.0.21",
            .path = "/stress/meting",
            .query = "esp",
            .event_handler = _http_event_handler,
            .user_data = local_response_buffer,        // Pass address of local buffer to get response
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);

        // GET
        esp_err_t err = esp_http_client_perform(client);

        // POST
        // const char *post_data = "{\"Melvis\":\""stienkt e bikke vandaag"\"}";
        char post_data[50];
        sprintf(post_data, "{\"heartrate\":%d,\"id\":%d}",heartrate, CONFIG_Patient_ID);
        esp_http_client_set_url(client, "http://192.168.0.21:3000/stress/meting");
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data));
        err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(PRQtag, "HTTP POST Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
        } else {
            ESP_LOGE(PRQtag, "HTTP POST request failed: %s", esp_err_to_name(err));
        }

        dataread = false;

    ESP_LOGI("main", "Finish http example");
    vTaskDelete(&PerformPostReq);
    sending = false;
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI("app_main", "Begin code app_main");
    ESP_ERROR_CHECK(Afe4404Init());
    ESP_LOGI("app_main", "connecting...");
    esp_err_t isconnected = example_connect();

    while (1){
        ESP_LOGI("app_main", "while 1");
        while(!dataread){
            ESP_LOGI("app_main", "while data niet gelezen");
            ESP_ERROR_CHECK(PerformFFT());
        }
      
        if (isconnected == ESP_OK && !sending){
            xTaskCreate(&PerformPostReq, "PerformPostReq", 8192, NULL, 5, NULL);
        }
        else{ if(!sending){ isconnected = example_connect();}}
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
