/* 
*/
#include "afe4404.h"


static const char *TAG2 = "AFE";

static esp_err_t I2C_WRITE_TO_AFE4404(uint8_t address, uint32_t data, uint8_t length){
    uint8_t configData[3];
    configData[0]=(uint8_t)(data >>16);
    configData[1]=(uint8_t)(((data & 0x00FFFF) >>8));
    configData[2]=(uint8_t)(((data & 0x0000FF)));
    ESP_ERROR_CHECK(i2c_master_write_slave(address, &configData, length));
    ESP_LOGI(TAG2, "Write Addres: %x Value: %d", address, data/*,data2/200,data3/200,data4/200,data5/200*/);
    return ESP_OK;
}

static esp_err_t I2cMasterAfe4404InitializeRegister(){
   //  //basic_settings ======================================================

    ESP_LOGI(TAG2, "first config data aangemaakt ready to write");
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(DIAGNOSIS, 8, 3));
    // ESP_LOGI(TAG2, "Write Addres: %x Value: %d", DIAGNOSIS,0/*,data2/200,data3/200,data4/200,data5/200*/);

    uint32_t reg_val =0;
    reg_val |= (0 << 20);       //  0: Transmitter not pwrd dwn |  1: pwrd dwn in dynamic pwrd-dwn mode 
    reg_val |= (0 << 17);       //  0: LED crrnt range 0-50 |   1: range 0-100
    reg_val |= (1 << 14);       //  0: ADC not pwrd dwn |  1: ADC pwrd dwn in dynamic pwrd-dwn mode
    reg_val |= (1 << 9);        //  0: External Clock (clk pin acts as input in this mode) | 1: Internal Clock (uses 4Mhz internal osc)
    reg_val |= (1 << 4);        //  0: TIA not pwrd dwn |  1: TIA pwrd dwn in dynamic pwrd-dwn mode
    reg_val |= (0 << 3);        //  0: Rest of ADC is not pwrd dwn |  1: Is pwrd dwn in dynamic pwrd-dwn mode
    reg_val |= (0 << 1);        //  0: Normal Mode |  1: RX portion of AFE pwrd dwn  
    reg_val |= (0 << 0);        //  0: Normal Mode |  1: Entire AFE is pwrd dwn   
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SETTINGS, reg_val, 3));
    
    //set_led2_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED2_ST, 0, 3));  
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED2_END, 399, 3));
   
    //set_led2sample_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED2_ST, 80, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED2_END, 399, 3));
  
    //set_adc_reset0_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P0_ST, 401, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P0_END, 407, 3));
 
    //set_led2_converter_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED2_CONV_ST, 408, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED2_CONV_END, 1467, 3));
  
    //set_led3_start_stop ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED3LEDSTC, 400, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED3LEDENDC, 799, 3));

     //set_led3_sample_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED3_ST, 480, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED3_END, 799, 3));

    //set_adc_reset1_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P1_ST, 1469, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P1_END, 1475, 3));
 
    //set_led3_convert_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED3_CONV_ST, 1476, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED3_CONV_END, 2535, 3));
  
    //set_led1_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED1_ST, 800, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED1_END, 1199, 3));

    //set_led1_sample_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED1_ST, 880, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_LED1_END, 1199, 3));
 
    //set_adc_reset2_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P2_ST, 2537, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P2_END, 2543, 3));
 
    //set_led1_convert_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED1_CONV_ST, 2544, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED1_CONV_END, 3603, 3));
 
    //set_amb1_sample_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_AMB1_ST, 1279, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(SMPL_AMB1_END, 1598, 3));
  
    //set_adc_reset3_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P3_ST, 3605, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(ADC_RST_P3_END, 3611, 3));

    //set_amb1_convert_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(AMB1_CONV_ST, 3612, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(AMB1_CONV_END, 4671, 3));
 
    //set_pdn_cycle_start_end ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(PDNCYCLESTC, 5471, 3));
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(PDNCYCLEENDC, 39199, 3));
  
    //set_prpct_count ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(PRPCT, 39999, 3));
 
    //set_timer_and_average_num ======================================================
    uint32_t temp = 0;
    temp |= (1 << 8);  // Timer Enable bit -> to use internal timing engine to do sync for sampling, data conv etc.
    temp |= 3;
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(TIM_NUMAV, temp, 3));
   
    //set_seperate_tia_gain ======================================================
    temp = 0;
    temp |= (1 << 15);   //  Separate TIA gains if this bit = 1; else single gain if = 0;
    temp |= (1 << 2);  //  Control of C2 settings (3 bits -> 0-7)
    temp |= (0 << 0);
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(TIA_GAINS2, temp, 3));
  
   //set_tia_gain ======================================================
    temp = 0;
    temp |= (0 << 8);          //  making 1 will replace ADC_RDY output with signal from timing engine.                        //  controlled by PROG_TG_STC and PROG_TG_ENDC regs.
    temp |= (0 << 3);       //  Control of C1 settings (3 bits -> 0-7)
    temp |= (3 << 0); 
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(TIA_GAINS1, temp, 3));
 
   //set_led_currents ======================================================
    temp = 0;
    temp |= (10 << 0);     // LED 1 addrss space -> 0-5 bits
    temp |= (2 << 6);     // LED 2 addrss space -> 6-11 bits
    temp |= (2 << 12);
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(LED_CONFIG, temp, 3));

   //set_led_currents ======================================================
    temp = 0;
    temp |= (0 << 9);   //  Enable clock output if enable = 1
    temp |= (2 << 1);  
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(CLKOUT, temp, 3));
    //set_prpct_count ======================================================
    ESP_ERROR_CHECK(I2C_WRITE_TO_AFE4404(CLKDIV_PRF, 1, 3));

    return ESP_OK;
}

// /**
//  * @brief code for getting and calculating data from AFE4404 Debugging only
//  */
uint32_t EspSpo2Data(){
    uint8_t data1[3];
    uint8_t length = 3;
    uint32_t retVal = 0;
    uint16_t regAddr = 0x2A;
    uint32_t regVal = 0;
    while(retVal>4194303&retVal<1000){
        i2c_master_read_slave(regAddr, &data1, length);
        retVal = data1[0];
        retVal = (retVal << 8) | data1[1];
        retVal = (retVal << 8) | data1[2]; 
        if (retVal & 0x00200000)  // check if the ADC value is positive or negative
        {
            retVal &= 0x003FFFFF;   // convert it to a 22 bit value
            return (retVal^0xFFC00000);
        }
        ESP_LOGI(TAG2, "$%d 0;", retVal/200);
        if(retVal<4194303){
            regVal = OffsetRaiseBy1(regVal);
            if(retVal>1){
            }else{
                if(regVal =0)regVal = OffsetSetNegative();
                regVal = OffsetRaiseBy1(regVal);
            }
        }
        I2C_WRITE_TO_AFE4404(DAC_SETTING,regVal,3);// is 3 bytes long
    }
        // printf("%d\n",retVal);
    return retVal/200;
}

static uint32_t OffsetSetNegative(){
    //offsetverzetten offset
    uint32_t regVal = 0;
    regVal |= ( 1 << POL_OFFDAC_LED2);
    regVal |= ( 1 << POL_OFFDAC_AMB1);
    regVal |= ( 1 << POL_OFFDAC_LED1);
    regVal |= ( 1 << POL_OFFDAC_LED3);
    return regVal;
}

static uint32_t OffsetRaiseBy1(u_int32_t carryRegsettings){
    //offsert 1 verhogen
    carryRegsettings += ( 1 << I_OFFDAC_LED2);
    carryRegsettings += ( 1 << I_OFFDAC_AMB1);
    carryRegsettings += ( 1 << I_OFFDAC_LED1);
    carryRegsettings += ( 1 << I_OFFDAC_LED3);
    return carryRegsettings;
}


static esp_err_t Afe4404Init(){
	ESP_LOGD(TAG2,"in afe4404 component");
    ESP_ERROR_CHECK(i2c_slave_init());   
	ESP_LOGD(TAG2, "Done slave i2c init!");
	ESP_ERROR_CHECK(i2c_master_init());   
	ESP_LOGD(TAG2, "Done master i2c init!");

    ESP_ERROR_CHECK(I2cMasterAfe4404InitializeRegister());
    ESP_LOGD(TAG2, "Done writing registers!");
	return ESP_OK;
}
