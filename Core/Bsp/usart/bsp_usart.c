/**
 * @file    bsp_usart.c
 * @brief   串口驱动实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_usart.h"
#include "usart.h"
#include "letter_shell_port.h"
#include <string.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
uint8_t g_uart4_rx_buf[USART_DEBUG_RX_BUF_SIZE];
uint16_t g_uart4_rx_len = 0;

uint8_t g_uart5_rx_buf[USART_HEART_RX_BUF_SIZE];
uint16_t g_uart5_rx_len = 0;

/*========================= 静态变量 (Static Variables) ====================*/
static uint8_t s_uart4_rx_byte;
static uint8_t s_uart5_rx_byte;

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/

/**
 * @brief printf重定向
 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

/**
 * @brief 串口初始化
 */
void BSP_USART_Init(void)
{
    /* 启动串口接收中断 */
    HAL_UART_Receive_IT(&huart4, &s_uart4_rx_byte, 1);
    HAL_UART_Receive_IT(&huart5, &s_uart5_rx_byte, 1);
}

/**
 * @brief 心跳串口发送数据
 * @param data 数据指针
 * @param len 数据长度
 */
void BSP_USART_Heart_Send(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return;
    }
    HAL_UART_Transmit(&huart5, data, len, 1000);
}

/**
 * @brief 获取 UART4 接收字节
 */
uint8_t BSP_USART_GetUart4RxByte(void)
{
    return s_uart4_rx_byte;
}

/**
 * @brief 串口接收完成回调函数
 * @param huart 串口句柄
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4) {
        /* UART4接收回调 - 交给 shell 处理 */
        /* 将接收到的字符交给 shell 处理，shell 自己会处理回显 */
        LetterShell_UART_RxCallback(s_uart4_rx_byte);
        /* 继续下一次接收 */
        HAL_UART_Receive_IT(&huart4, &s_uart4_rx_byte, 1);
    }
    else if (huart->Instance == UART5) {
        /* UART5接收回调 */
        if (g_uart5_rx_len < USART_HEART_RX_BUF_SIZE - 1) {
            g_uart5_rx_buf[g_uart5_rx_len++] = s_uart5_rx_byte;
        }
        /* 继续下一次接收 */
        HAL_UART_Receive_IT(&huart5, &s_uart5_rx_byte, 1);
    }
}

