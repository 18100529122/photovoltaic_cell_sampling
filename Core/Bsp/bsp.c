/**
 * @file     bsp.c
 * @brief    板级支持包源文件
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "can/bsp_can.h"

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/


/**
 * @brief  硬件抽象层初始化 (板级支持包初始化)
 */
void BSP_Init(void)
{
    BSP_USART_Init();
    BSP_ADC_Init();
    BSP_CAN_Init();
}
