#ifndef __BSP_TIME_H__
#define __BSP_TIME_H__
/**
 * @file     bsp_time.h
 * @brief    定时器驱动头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/
/**
 * @brief  TIM3中断处理函数
 */
void BSP_TIM3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_TIME_H__ */