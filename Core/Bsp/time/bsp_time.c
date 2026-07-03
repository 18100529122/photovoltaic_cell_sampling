/**
 * @file     bsp_time.c
 * @brief    定时器驱动实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_time.h"
#include "tim.h"
#include "bsp_adc.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief  TIM3中断处理函数
 */
void BSP_TIM3_IRQHandler(void)
{
  // 检查更新中断标志
  if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) != RESET)
  {
    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET)
    {
      // 清除中断标志
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
      
      // 每秒启动一次ADC采集
      BSP_ADC_Start();
    }
  }
}
