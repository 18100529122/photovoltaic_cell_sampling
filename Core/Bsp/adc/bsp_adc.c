/**
 * @file     bsp_adc.c
 * @brief    ADC+DMA驱动实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_adc.h"
#include "adc.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
static ADC_RawData_t adc_raw_data;                /* ADC原始数据缓冲区 */
volatile uint8_t adc1_conv_complete = 0;          /* ADC1转换完成标志 */
volatile uint8_t adc2_conv_complete = 0;          /* ADC2转换完成标志 */
volatile uint8_t adc3_conv_complete = 0;          /* ADC3转换完成标志 */
volatile uint8_t adc4_conv_complete = 0;          /* ADC4转换完成标志 */
volatile uint8_t all_adc_conv_complete = 0;       /* 所有ADC转换完成标志 */

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/
static void CheckAllADCComplete(void);

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  ADC初始化
 */
void BSP_ADC_Init(void)
{
    /* ADC和DMA已在MX_ADCx_Init中初始化 */
    adc1_conv_complete = 0;
    adc2_conv_complete = 0;
    adc3_conv_complete = 0;
    adc4_conv_complete = 0;
    all_adc_conv_complete = 0;
    
    /* 校准ADC1-4 */
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);
}

/**
 * @brief  启动ADC+DMA采集
 */
void BSP_ADC_Start(void)
{
    HAL_StatusTypeDef status;

    /* 确保先停止之前的采集 */
    BSP_ADC_Stop();
    
    /* 清除所有完成标志 */
    adc1_conv_complete = 0;
    adc2_conv_complete = 0;
    adc3_conv_complete = 0;
    adc4_conv_complete = 0;
    all_adc_conv_complete = 0;

    /* 重置TIM2计数器 */
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    /* 启动4个ADC的DMA，缓冲区大小 = 通道数 × 200个采样点 */
    status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_raw_data.adc1_data, ADC1_CH_COUNT * SAMPLES_PER_CHANNEL);
    if (status != HAL_OK) {
        Error_Handler();
    }

    status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_raw_data.adc2_data, ADC2_CH_COUNT * SAMPLES_PER_CHANNEL);
    if (status != HAL_OK) {
        Error_Handler();
    }

    status = HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_raw_data.adc3_data, ADC3_CH_COUNT * SAMPLES_PER_CHANNEL);
    if (status != HAL_OK) {
        Error_Handler();
    }

    status = HAL_ADC_Start_DMA(&hadc4, (uint32_t*)adc_raw_data.adc4_data, ADC4_CH_COUNT * SAMPLES_PER_CHANNEL);
    if (status != HAL_OK) {
        Error_Handler();
    }

    /* 最后启动TIM2触发ADC */
    status = HAL_TIM_Base_Start(&htim2);
    if (status != HAL_OK) {
        Error_Handler();
    }
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
 * @brief  检查所有ADC是否完成
 */
static void CheckAllADCComplete(void)
{
    if (adc1_conv_complete && adc2_conv_complete && adc3_conv_complete && adc4_conv_complete)
    {
        /* 所有ADC都完成了，停止TIM2 */
        HAL_TIM_Base_Stop(&htim2);
        all_adc_conv_complete = 1;
    }
}

/**
 * @brief  ADC DMA转换完成回调
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1)
    {
        adc1_conv_complete = 1;
    }
    else if (hadc == &hadc2)
    {
        adc2_conv_complete = 1;
    }
    else if (hadc == &hadc3)
    {
        adc3_conv_complete = 1;
    }
    else if (hadc == &hadc4)
    {
        adc4_conv_complete = 1;
    }
    
    /* 检查是否所有ADC都完成了 */
    CheckAllADCComplete();
}

/**
 * @brief  ADC DMA半传输完成回调 - 我们不需要这个，留空
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    /* 不处理半传输 */
    (void)hadc;
}

