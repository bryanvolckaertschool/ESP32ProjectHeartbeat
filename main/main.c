/* Hello World Example

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
#include "afe4404.c"
#include "esp_log.h"

#include "../components/esp-dsp/modules/common/include/esp_dsp.h"

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

static const char *TAG = "main";

void process_and_show(float* data, int length)
{
    dsps_fft2r_fc32(data, length);
    // Bit reverse 
    dsps_bit_rev_fc32(data, length);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(data, length);

    for (int i = 0 ; i < length/2 ; i++) {
        data[i] = 10 * log10f((data[i * 2 + 0] * data[i * 2 + 0] + data[i * 2 + 1] * data[i * 2 + 1])/N);
    }
  
    // Show power spectrum in 64x10 window from -100 to 0 dB from 0..N/4 samples
    dsps_view(data, length/2, 64, 10,  0, 160, '|');

}

uint32_t GetSensorData(){

    return 0;
}

static esp_err_t PerformFFT(){
    esp_err_t ret;
    ESP_LOGI(TAG, "Start Example.");
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret  != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        return 0;
    }

    // Generate hann window
    dsps_wind_hann_f32(wind, N);
    // Generate input signal for x1 A=1 , F=0.1
    float x1[N_SAMPLES]; 
    // = {0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70,-7.86,-6.75,-1.99,3.85,7.61,7.24,2.94,-2.94,-7.24,-7.61,-3.85,1.99,6.75,7.86,4.70,-1.00,-6.16,-7.98,-5.48,0.00,5.48,7.98,6.16,1.00,-4.70};
    for (int i=0 ; i < N ; i++)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        x1[i] = (float) EspSpo2Data(); //casting couch
        // ESP_LOGI(TAG, "$%f;", x1[i]);
    }



    // dsps_tone_gen_f32(x1, N, 1.0, 0.16,  0);


    // Convert two input vectors to one complex vector
    for (int i=0 ; i< N ; i++)
    {
        y_cf[i*2 + 0] = x1[i] * wind[i];
    }
    // FFT
    unsigned int start_b = xthal_get_ccount();
    dsps_fft2r_fc32(y_cf, N);
    unsigned int end_b = xthal_get_ccount();
    // Bit reverse 
    dsps_bit_rev_fc32(y_cf, N);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(y_cf, N);

    for (int i = 0 ; i < N/2 ; i++) {
        y1_cf[i] = 10 * log10f((y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1])/N);
    }
  
    // Show power spectrum in 64x10 window from -100 to 0 dB from 0..N/4 samples
    // ESP_LOGW(TAG, "Signal x1");
    dsps_view(y1_cf, N/2, 64, 10,  -60, 40, '|');

    ESP_LOGI(TAG, "End Example.");
    return ESP_OK;
}


void app_main(void)
{
    ESP_LOGD(TAG, "Begin code app_main");
    ESP_ERROR_CHECK(Afe4404Init());
    

    while (1){
        ESP_ERROR_CHECK(PerformFFT());
       vTaskDelay(100 / portTICK_PERIOD_MS);
       
    //    EspSpo2Data();
    }
}
