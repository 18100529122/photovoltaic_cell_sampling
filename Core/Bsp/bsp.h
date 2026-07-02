#ifndef __BSP_H__
#define __BSP_H__
/**
 * @file     bsp.h
 * @brief    板级支持包头文件
 */
#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "can/bsp_can.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/

/**
 * @brief 硬件抽象层初始化 (板级支持包初始化)
 */
void BSP_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* __BSP_H__ */
