/**
 * @file     bsp_adc.c
 * @brief    ADC+DMA驱动实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_adc.h"
#include "adc.h"
#include "tim.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
static ADC_RawData_t adc_raw_data;          /* ADC原始数据缓冲区 */
volatile uint8_t adc_conv_complete = 0;     /* ADC转换完成标志 */

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  ADC初始化
 */
void BSP_ADC_Init(void)
{
    /* ADC和DMA已在MX_ADCx_Init中初始化 */
    adc_conv_complete = 0;
}

/**
 * @brief  启动ADC+DMA采集
 */
void BSP_ADC_Start(void)
{
    /* 启动4个ADC的DMA采集 */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_raw_data.adc1_data, ADC1_CH_COUNT);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_raw_data.adc2_data, ADC2_CH_COUNT);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_raw_data.adc3_data, ADC3_CH_COUNT);
    HAL_ADC_Start_DMA(&hadc4, (uint32_t*)adc_raw_data.adc4_data, ADC4_CH_COUNT);

    /* 启动TIM2触发源 */
    HAL_TIM_Base_Start(&htim2);
}

/**
 * @brief  停止ADC+DMA采集
 */
void BSP_ADC_Stop(void)
{
    /* 停止TIM2 */
    HAL_TIM_Base_Stop(&htim2);

    /* 停止4个ADC */
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);
    HAL_ADC_Stop_DMA(&hadc3);
    HAL_ADC_Stop_DMA(&hadc4);
}

/**
 * @brief  获取ADC原始数据指针
 * @return ADC原始数据指针
 */
ADC_RawData_t* BSP_ADC_GetRawData(void)
{
    return &adc_raw_data;
}

/**
 * @brief  ADC转换完成回调
 */
void BSP_ADC_ConvCpltCallback(void)
{
    adc_conv_complete = 1;
}

/**
 * @brief  ADC DMA转换完成回调
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1 || hadc == &hadc2 || hadc == &hadc3 || hadc == &hadc4)
    {
        BSP_ADC_ConvCpltCallback();
    }
}

/**
 * @brief  ADC DMA半传输完成回调
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1 || hadc == &hadc2 || hadc == &hadc3 || hadc == &hadc4)
    {
        BSP_ADC_ConvCpltCallback();
    }
}

