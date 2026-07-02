#ifndef __DATA_PROCESS_H__
#define __DATA_PROCESS_H__
/**
 * @file     data_process.h
 * @brief    数据处理模块头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"
#include "data_structure.h"
#include "cmsis_os.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
extern ADC_Data_t adc_data;  /* ADC数据结构 */

/*========================= 函数声明 (Function Declarations) ===============*/
/**
 * @brief  数据处理任务入口（原生FreeRTOS）
 */
void app_data_process_task(void *argument);

/**
 * @brief  数据处理模块初始化
 */
void DataProcess_Init(void);

/**
 * @brief  将ADC原始数据映射到对应通道
 * @param  raw_data ADC原始数据指针
 */
void DataProcess_MapRawData(ADC_RawData_t* raw_data);

/**
 * @brief  处理单个通道数据（排序、去极值、求平均）
 * @param  buffer 采样缓冲区指针
 * @return 处理后的平均值
 */
float DataProcess_ProcessChannel(ADC_SampleBuffer_t* buffer);

/**
 * @brief  处理所有通道数据
 */
void DataProcess_ProcessAll(void);

/**
 * @brief  获取ADC数据指针
 * @return ADC数据指针
 */
ADC_Data_t* DataProcess_GetData(void);

/**
 * @brief  数据处理任务主循环
 */
void DataProcess_Task(void);

#ifdef __cplusplus
}
#endif

#endif /* __DATA_PROCESS_H__ */
