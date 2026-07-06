#ifndef __BSP_USART_H__
#define __BSP_USART_H__
/**
 * @file    bsp_usart.h
 * @brief   串口驱动头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"
#include <stdio.h>

/*========================= 宏定义 (Macros) ================================*/
#define USART_DEBUG_RX_BUF_SIZE  128
#define USART_HEART_RX_BUF_SIZE  64

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
extern uint8_t g_uart4_rx_buf[USART_DEBUG_RX_BUF_SIZE];
extern uint16_t g_uart4_rx_len;

extern uint8_t g_uart5_rx_buf[USART_HEART_RX_BUF_SIZE];
extern uint16_t g_uart5_rx_len;

/*========================= 函数声明 (Function Declarations) ===============*/

/**
 * @brief 串口初始化
 */
void BSP_USART_Init(void);

/**
 * @brief 心跳串口发送数据
 * @param data 数据指针
 * @param len 数据长度
 */
void BSP_USART_Heart_Send(uint8_t *data, uint16_t len);

/**
 * @brief 获取 UART4 接收字节
 */
uint8_t BSP_USART_GetUart4RxByte(void);

/**
 * @brief 串口回调函数声明
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
#endif /* __BSP_USART_H__ */
