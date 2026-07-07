/**
 * @file     app.c
 * @brief    应用层整合实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "app.h"
#include "bsp_adc.h"
#include "data_process.h"
#include "env_parameter.h"
#include "tim.h"
#include <stdio.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  应用层初始化
 */
void App_Init(void)
{
    /* 先初始化Env参数 */
    EnvParameter_Init();
    
    BSP_ADC_Init();
    DataProcess_Init();

    printf("App Init Complete\r\n");

    /* 启动TIM3，每秒触发一次ADC采集 */
    HAL_TIM_Base_Start_IT(&htim3);
}

