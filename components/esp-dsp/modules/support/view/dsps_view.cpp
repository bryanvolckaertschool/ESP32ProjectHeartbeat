#include "dsps_view.h"
#include <math.h>
#include "esp_log.h"
#include <limits>

void dsps_view(const float *data, int32_t len, int width, int height, float min, float max, char view_char, uint8_t *heartbeat)
{
    uint8_t *view_data = new uint8_t[width * height];
    float *view_data_min = new float[width];
    float *view_data_max = new float[width];
    //

    for (int y = 0; y < height ; y++) {
        for (int x = 0 ; x < width ; x++) {
            view_data[y * width + x] = ' ';
        }
    }
    for (int i = 0 ; i < width ; i++) {
        view_data_min[i] = max;
        view_data_max[i] = min;
    }
    float x_step = (float)(width) / (float)len;
    float y_step = (float)(height - 1) / (max - min);
    float data_min = std::numeric_limits<float>::max();
    float data_max = std::numeric_limits<float>::min();
    int min_pos = 0;
    int max_pos = 0;

    for (int i = 0 ; i < len ; i++) {
        int x_pos = i * x_step;
        if (data[i] < view_data_min[x_pos]) {
            view_data_min[x_pos] = data[i];
        }
        if (data[i] > view_data_max[x_pos]) {
            view_data_max[x_pos] = data[i];
        }

        if (view_data_min[x_pos] < min) {
            view_data_min[x_pos] = min;
        }
        if (view_data_max[x_pos] > max) {
            view_data_max[x_pos] = max;
        }
        /*
            Calculation for Min and max values: 

            Based on heartrate frequency calculation (F = (i * sF)/N)
            Minimal heartrate = 50 BPM (0.83HZ)
            Maximal heartrate = 200 BPM (3.33HZ)
            In order to get I from formule: i = F*N/sF

        */
        if (data[i] > data_max && i > 21 && i < 85) { // 256 * 50/256 = 21, 256 * 200/256 = 85
            data_max = data[i];
            max_pos = i;
        }
        if (data[i] < data_min) {
            data_min = data[i];
            min_pos = i;
        }
    }
    float heartrate;
    heartrate = max_pos * 10.0;
    heartrate = heartrate/256.0;
    heartrate = heartrate * 60.0;
    *heartbeat = heartrate;
    // ESP_LOGI("Heartbeat", "$%f;", heartrate);
    delete view_data;
    delete view_data_min;
    delete view_data_max;
}

