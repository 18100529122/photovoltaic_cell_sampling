/**
 * @file     data_process.c
 * @brief    数据处理模块实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "data_process.h"
#include "bsp_adc.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
ADC_Data_t adc_data;  /* ADC数据结构 */

/*========================= 静态变量 (Static Variables) ====================*/
TaskHandle_t dataProcessTaskHandle = NULL;  /* 数据处理任务句柄（原生FreeRTOS） */

/*========================= 静态函数声明 (Static Function Declarations) ====*/
/**
 * @brief  冒泡排序（降序）
 * @param  data 数据数组
 * @param  len 数据长度
 */
static void sort_data(uint16_t* data, uint16_t len);

/**
 * @brief  数据处理任务（原生FreeRTOS）
 */
void app_data_process_task(void *argument);

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  数据处理模块初始化
 */
void DataProcess_Init(void)
{
    memset(&adc_data, 0, sizeof(ADC_Data_t));
    printf("DataProcess_Init - mem clear complete\r\n");
    /* 创建数据处理任务（使用FreeRTOS原生优先级） */
    BaseType_t ret = xTaskCreate(app_data_process_task, 
                                 "adc_process", 
                                 4096, 
                                 NULL, 
                                 tskIDLE_PRIORITY + 2, 
                                 NULL);
    if (ret != pdPASS) {
        printf("Failed to create adc_process task! Error: %ld\r\n", ret);
    } else {
        printf("adc_process task created successfully!\r\n");
    }
}

/**
 * @brief  数据处理任务入口（原生FreeRTOS）
 */
void app_data_process_task(void *argument)
{
    printf("app_data_process_task starting...\r\n");

    while (1)
    {
        DataProcess_Task();
        osDelay(10);
    }
}

/**
 * @brief  将ADC原始数据映射到对应通道（一次性处理200个采样点
 * @param  raw_data ADC原始数据指针
 */
void DataProcess_MapRawData(ADC_RawData_t* raw_data)
{
    /*
     * ADC1通道映射 (7通道):
     * Rank1-IN1-PA0  -> VS3
     * Rank2-IN2-PA1  -> CUR3
     * Rank3-IN3-PA2  -> VS4
     * Rank4-IN4-PA3  -> CUR4
     * Rank5-IN6-PC0  -> VS1
     * Rank6-IN7-PC1  -> CUR1
     * Rank7-IN8-PC2  -> VS2
     * DMA数据布局: [Ch0_Sample0, Ch1_Sample0, ..., Ch6_Sample0, Ch0_Sample1, Ch1_Sample1, ...]
     */
    for (uint16_t sample = 0; sample < SAMPLES_PER_CHANNEL; sample++) {
        uint16_t base_idx = sample * ADC1_CH_COUNT;
        adc_data.vs[2].samples[sample] = raw_data->adc1_data[base_idx + 0];
        adc_data.cur[2].samples[sample] = raw_data->adc1_data[base_idx + 1];
        adc_data.vs[3].samples[sample] = raw_data->adc1_data[base_idx + 2];
        adc_data.cur[3].samples[sample] = raw_data->adc1_data[base_idx + 3];
        adc_data.vs[0].samples[sample] = raw_data->adc1_data[base_idx + 4];
        adc_data.cur[0].samples[sample] = raw_data->adc1_data[base_idx + 5];
        adc_data.vs[1].samples[sample] = raw_data->adc1_data[base_idx + 6];
    }
    adc_data.vs[2].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[2].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[3].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[3].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[0].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[0].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[1].write_index = SAMPLES_PER_CHANNEL;

    /*
     * ADC2通道映射 (7通道):
     * Rank1-IN17-PA4  -> VS5
     * Rank2-IN13-PA5  -> CUR5
     * Rank3-IN3-PA6   -> VS6
     * Rank4-IN4-PA7   -> CUR6
     * Rank5-IN9-PC3   -> CUR2
     * Rank6-IN5-PC4   -> VS7
     * Rank7-IN11-PC5  -> CUR7
     */
    for (uint16_t sample = 0; sample < SAMPLES_PER_CHANNEL; sample++) {
        uint16_t base_idx = sample * ADC2_CH_COUNT;
        adc_data.vs[4].samples[sample] = raw_data->adc2_data[base_idx + 0];
        adc_data.cur[4].samples[sample] = raw_data->adc2_data[base_idx + 1];
        adc_data.vs[5].samples[sample] = raw_data->adc2_data[base_idx + 2];
        adc_data.cur[5].samples[sample] = raw_data->adc2_data[base_idx + 3];
        adc_data.cur[1].samples[sample] = raw_data->adc2_data[base_idx + 4];
        adc_data.vs[6].samples[sample] = raw_data->adc2_data[base_idx + 5];
        adc_data.cur[6].samples[sample] = raw_data->adc2_data[base_idx + 6];
    }
    adc_data.vs[4].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[4].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[5].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[5].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[1].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[6].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[6].write_index = SAMPLES_PER_CHANNEL;

    /*
     * ADC3通道映射 (5通道):
     * Rank1-IN12-PB0  -> VS8
     * Rank2-IN1-PB1   -> CUR8
     * Rank3-IN4-PE7   -> VS9
     * Rank4-IN2-PE9   -> VS10
     * Rank5-IN3-PE13  -> VS12
     */
    for (uint16_t sample = 0; sample < SAMPLES_PER_CHANNEL; sample++) {
        uint16_t base_idx = sample * ADC3_CH_COUNT;
        adc_data.vs[7].samples[sample] = raw_data->adc3_data[base_idx + 0];
        adc_data.cur[7].samples[sample] = raw_data->adc3_data[base_idx + 1];
        adc_data.vs[8].samples[sample] = raw_data->adc3_data[base_idx + 2];
        adc_data.vs[9].samples[sample] = raw_data->adc3_data[base_idx + 3];
        adc_data.vs[11].samples[sample] = raw_data->adc3_data[base_idx + 4];
    }
    adc_data.vs[7].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[7].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[8].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[9].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[11].write_index = SAMPLES_PER_CHANNEL;

    /*
     * ADC4通道映射 (7通道):
     * Rank1-IN6-PE8   -> CUR9
     * Rank2-IN14-PE10 -> CUR10
     * Rank3-IN15-PE11 -> VS11
     * Rank4-IN16-PE12 -> CUR11
     * Rank5-IN1-PE14  -> CUR12
     * Rank6-IN4-PB14  -> TEMP2
     * Rank7-IN5-PB15  -> TEMP1
     */
    for (uint16_t sample = 0; sample < SAMPLES_PER_CHANNEL; sample++) {
        uint16_t base_idx = sample * ADC4_CH_COUNT;
        adc_data.cur[8].samples[sample] = raw_data->adc4_data[base_idx + 0];
        adc_data.cur[9].samples[sample] = raw_data->adc4_data[base_idx + 1];
        adc_data.vs[10].samples[sample] = raw_data->adc4_data[base_idx + 2];
        adc_data.cur[10].samples[sample] = raw_data->adc4_data[base_idx + 3];
        adc_data.cur[11].samples[sample] = raw_data->adc4_data[base_idx + 4];
        adc_data.temp[1].samples[sample] = raw_data->adc4_data[base_idx + 5];
        adc_data.temp[0].samples[sample] = raw_data->adc4_data[base_idx + 6];
    }
    adc_data.cur[8].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[9].write_index = SAMPLES_PER_CHANNEL;
    adc_data.vs[10].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[10].write_index = SAMPLES_PER_CHANNEL;
    adc_data.cur[11].write_index = SAMPLES_PER_CHANNEL;
    adc_data.temp[1].write_index = SAMPLES_PER_CHANNEL;
    adc_data.temp[0].write_index = SAMPLES_PER_CHANNEL;

    /* 所有通道都已采集满200个点 */
    for (int i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        adc_data.vs[i].buffer_full = 1;
    }
    for (int i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        adc_data.cur[i].buffer_full = 1;
    }
    for (int i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        adc_data.temp[i].buffer_full = 1;
    }
    adc_data.data_ready = 1;
}

/**
 * @brief  冒泡排序（降序）
 * @param  data 数据数组
 * @param  len 数据长度
 */
static void sort_data(uint16_t* data, uint16_t len)
{
    for (uint16_t i = 0; i < len - 1; i++) {
        for (uint16_t j = 0; j < len - i - 1; j++) {
            if (data[j] < data[j + 1]) {
                uint16_t temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

/**
 * @brief  处理单个通道数据（排序、去极值、求平均）
 * @param  buffer 采样缓冲区指针
 * @return 处理后的平均值
 */
float DataProcess_ProcessChannel(ADC_SampleBuffer_t* buffer)
{
    if (!buffer->buffer_full) return 0.0f;

    /* 复制数据用于排序 */
    uint16_t temp[SAMPLES_PER_CHANNEL];
    memcpy(temp, buffer->samples, sizeof(temp));

    /* 冒泡排序（降序） */
    sort_data(temp, SAMPLES_PER_CHANNEL);

    /* 去掉10个最大值和10个最小值，求剩余180个数据的平均值 */
    uint32_t sum = 0;
    for (uint16_t i = REMOVE_MAX_COUNT; i < SAMPLES_PER_CHANNEL - REMOVE_MIN_COUNT; i++) {
        sum += temp[i];
    }

    return (float)sum / (float)PROCESS_SAMPLES_COUNT;
}

/**
 * @brief  处理所有通道数据
 */
void DataProcess_ProcessAll(void)
{
    if (!adc_data.data_ready) return;

    /* 处理电压通道 */
    for (int i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        adc_data.vs_result[i] = DataProcess_ProcessChannel(&adc_data.vs[i]);
        /* 重置缓冲区 */
        adc_data.vs[i].write_index = 0;
        adc_data.vs[i].buffer_full = 0;
    }

    /* 处理电流通道 */
    for (int i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        adc_data.cur_result[i] = DataProcess_ProcessChannel(&adc_data.cur[i]);
        /* 重置缓冲区 */
        adc_data.cur[i].write_index = 0;
        adc_data.cur[i].buffer_full = 0;
    }

    /* 处理温度通道 */
    for (int i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        adc_data.temp_result[i] = DataProcess_ProcessChannel(&adc_data.temp[i]);
        /* 重置缓冲区 */
        adc_data.temp[i].write_index = 0;
        adc_data.temp[i].buffer_full = 0;
    }

    adc_data.data_ready = 0;
}

/**
 * @brief  获取ADC数据指针
 * @return ADC数据指针
 */
ADC_Data_t* DataProcess_GetData(void)
{
    return &adc_data;
}

/**
 * @brief  数据处理任务主循环
 */
void DataProcess_Task(void)
{
    if (all_adc_conv_complete)
    {
        all_adc_conv_complete = 0;

        /* 映射ADC原始数据到对应通道 */
        ADC_RawData_t* raw_data = BSP_ADC_GetRawData();
        DataProcess_MapRawData(raw_data);

        /* 处理所有通道数据 */
        DataProcess_ProcessAll();

        /* 打印测试 打印所有处理后的数据  */
        // printf("-----------------\n");
        // printf("adc_data.vs_result: ");
        // for (int i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        //     printf("%.2f ", adc_data.vs_result[i]);
        // }
        // printf("\n");
        // printf("adc_data.cur_result: ");
        // for (int i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        //     printf("%.2f ", adc_data.cur_result[i]);
        // }
        // printf("\n");
        // printf("adc_data.temp_result: ");
        // for (int i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        //     printf("%.2f ", adc_data.temp_result[i]);
        // }
        // printf("\n");
    }
}

