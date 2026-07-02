/**
 * @file     data_process.c
 * @brief    数据处理模块实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "data_process.h"
#include "bsp_adc.h"
#include <string.h>
#include <stdio.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
ADC_Data_t adc_data;  /* ADC数据结构 */

/*========================= 静态变量 (Static Variables) ====================*/
static osThreadId_t dataProcessTaskHandle;  /* 数据处理任务句柄 */
const osThreadAttr_t dataProcessTask_attributes = {
  .name = "dataProcessTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

/*========================= 静态函数声明 (Static Function Declarations) ====*/
/**
 * @brief  冒泡排序（降序）
 * @param  data 数据数组
 * @param  len 数据长度
 */
static void sort_data(uint16_t* data, uint16_t len);

/**
 * @brief  数据处理任务
 */
static void DataProcess_TaskEntry(void *argument);

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  数据处理模块初始化
 */
void DataProcess_Init(void)
{
    memset(&adc_data, 0, sizeof(ADC_Data_t));

    /* 创建数据处理任务 */
    dataProcessTaskHandle = osThreadNew(DataProcess_TaskEntry, NULL, &dataProcessTask_attributes);
}

/**
 * @brief  数据处理任务入口
 */
static void DataProcess_TaskEntry(void *argument)
{
    
    /* 启动ADC采集 */
    BSP_ADC_Start();

    for(;;)
    {
        DataProcess_Task();
        osDelay(1);
    }
}

/**
 * @brief  将ADC原始数据映射到对应通道
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
     */
    if (adc_data.vs[2].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[2].samples[adc_data.vs[2].write_index++] = raw_data->adc1_data[0];
    if (adc_data.cur[2].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[2].samples[adc_data.cur[2].write_index++] = raw_data->adc1_data[1];
    if (adc_data.vs[3].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[3].samples[adc_data.vs[3].write_index++] = raw_data->adc1_data[2];
    if (adc_data.cur[3].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[3].samples[adc_data.cur[3].write_index++] = raw_data->adc1_data[3];
    if (adc_data.vs[0].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[0].samples[adc_data.vs[0].write_index++] = raw_data->adc1_data[4];
    if (adc_data.cur[0].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[0].samples[adc_data.cur[0].write_index++] = raw_data->adc1_data[5];
    if (adc_data.vs[1].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[1].samples[adc_data.vs[1].write_index++] = raw_data->adc1_data[6];

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
    if (adc_data.vs[4].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[4].samples[adc_data.vs[4].write_index++] = raw_data->adc2_data[0];
    if (adc_data.cur[4].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[4].samples[adc_data.cur[4].write_index++] = raw_data->adc2_data[1];
    if (adc_data.vs[5].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[5].samples[adc_data.vs[5].write_index++] = raw_data->adc2_data[2];
    if (adc_data.cur[5].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[5].samples[adc_data.cur[5].write_index++] = raw_data->adc2_data[3];
    if (adc_data.cur[1].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[1].samples[adc_data.cur[1].write_index++] = raw_data->adc2_data[4];
    if (adc_data.vs[6].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[6].samples[adc_data.vs[6].write_index++] = raw_data->adc2_data[5];
    if (adc_data.cur[6].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[6].samples[adc_data.cur[6].write_index++] = raw_data->adc2_data[6];

    /*
     * ADC3通道映射 (5通道):
     * Rank1-IN12-PB0  -> VS8
     * Rank2-IN1-PB1   -> CUR8
     * Rank3-IN4-PE7   -> VS9
     * Rank4-IN2-PE9   -> VS10
     * Rank5-IN3-PE13  -> VS12
     */
    if (adc_data.vs[7].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[7].samples[adc_data.vs[7].write_index++] = raw_data->adc3_data[0];
    if (adc_data.cur[7].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[7].samples[adc_data.cur[7].write_index++] = raw_data->adc3_data[1];
    if (adc_data.vs[8].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[8].samples[adc_data.vs[8].write_index++] = raw_data->adc3_data[2];
    if (adc_data.vs[9].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[9].samples[adc_data.vs[9].write_index++] = raw_data->adc3_data[3];
    if (adc_data.vs[11].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[11].samples[adc_data.vs[11].write_index++] = raw_data->adc3_data[4];

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
    if (adc_data.cur[8].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[8].samples[adc_data.cur[8].write_index++] = raw_data->adc4_data[0];
    if (adc_data.cur[9].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[9].samples[adc_data.cur[9].write_index++] = raw_data->adc4_data[1];
    if (adc_data.vs[10].write_index < SAMPLES_PER_CHANNEL)
        adc_data.vs[10].samples[adc_data.vs[10].write_index++] = raw_data->adc4_data[2];
    if (adc_data.cur[10].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[10].samples[adc_data.cur[10].write_index++] = raw_data->adc4_data[3];
    if (adc_data.cur[11].write_index < SAMPLES_PER_CHANNEL)
        adc_data.cur[11].samples[adc_data.cur[11].write_index++] = raw_data->adc4_data[4];
    if (adc_data.temp[1].write_index < SAMPLES_PER_CHANNEL)
        adc_data.temp[1].samples[adc_data.temp[1].write_index++] = raw_data->adc4_data[5];
    if (adc_data.temp[0].write_index < SAMPLES_PER_CHANNEL)
        adc_data.temp[0].samples[adc_data.temp[0].write_index++] = raw_data->adc4_data[6];

    /* 检查是否所有通道都采集满200个点 */
    uint8_t all_full = 1;
    for (int i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        if (adc_data.vs[i].write_index < SAMPLES_PER_CHANNEL) all_full = 0;
        else adc_data.vs[i].buffer_full = 1;
    }
    for (int i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        if (adc_data.cur[i].write_index < SAMPLES_PER_CHANNEL) all_full = 0;
        else adc_data.cur[i].buffer_full = 1;
    }
    for (int i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        if (adc_data.temp[i].write_index < SAMPLES_PER_CHANNEL) all_full = 0;
        else adc_data.temp[i].buffer_full = 1;
    }
    adc_data.data_ready = all_full;
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
    if (adc_conv_complete)
    {
        adc_conv_complete = 0;

        /* 映射ADC原始数据到对应通道 */
        ADC_RawData_t* raw_data = BSP_ADC_GetRawData();
        DataProcess_MapRawData(raw_data);

        /* 检查是否有1秒数据完成 */
        ADC_Data_t* adc_data = DataProcess_GetData();
        if (adc_data->data_ready)
        {
            /* 处理所有通道数据 */
            DataProcess_ProcessAll();

            /* 打印处理后的结果（调试用） */
            printf("=== Data Processed ===\r\n");

            printf("Voltage:\r\n");
            for (int i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
                printf("  VS%d: %.2f\r\n", i + 1, adc_data->vs_result[i]);
            }

            printf("Current:\r\n");
            for (int i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
                printf("  CUR%d: %.2f\r\n", i + 1, adc_data->cur_result[i]);
            }

            printf("Temperature:\r\n");
            for (int i = 0; i < ADC_CH_TEMP_COUNT; i++) {
                printf("  TEMP%d: %.2f\r\n", i + 1, adc_data->temp_result[i]);
            }
        }
    }
}

