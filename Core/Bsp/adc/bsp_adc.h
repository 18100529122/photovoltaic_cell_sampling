#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__
/**
 * @file     bsp_adc.h
 * @brief    ADC+DMA驱动头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"
#include "data_structure.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
extern volatile uint8_t adc1_conv_complete;  /* ADC1转换完成标志 */
extern volatile uint8_t adc2_conv_complete;  /* ADC2转换完成标志 */
extern volatile uint8_t adc3_conv_complete;  /* ADC3转换完成标志 */
extern volatile uint8_t adc4_conv_complete;  /* ADC4转换完成标志 */
extern volatile uint8_t all_adc_conv_complete;  /* 所有ADC转换完成标志 */

/*========================= 函数声明 (Function Declarations) ===============*/
/**
 * @brief  ADC初始化
 */
void BSP_ADC_Init(void);

/**
 * @brief  启动ADC+DMA采集
 */
void BSP_ADC_Start(void);

/**
 * @brief  停止ADC+DMA采集
 */
void BSP_ADC_Stop(void);

/**
 * @brief  获取ADC原始数据指针
 * @return ADC原始数据指针
 */
ADC_RawData_t* BSP_ADC_GetRawData(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H__ */
