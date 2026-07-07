/**
 * @file     bsp_time.c
 * @brief    定时器驱动实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "bsp_time.h"
#include "tim.h"
#include "bsp_adc.h"
#include "env_parameter.h"
#include "bsp_usart.h"

/*========================= 宏定义 (Macros) ================================*/
#define HEARTBEAT_SEND_INTERVAL 10  // 心跳发送间隔（秒）

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/
static uint32_t heartbeat_send_counter = 0;  // 心跳发送计数器

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
      
      // 主机模式：启动ADC采集
      if (g_env_config.is_master)
      {
        BSP_ADC_Start();
        
        // 每10秒发送一次心跳包
        heartbeat_send_counter++;
        if (heartbeat_send_counter >= HEARTBEAT_SEND_INTERVAL)
        {
          heartbeat_send_counter = 0;
          
          // 构造心跳包
          uint8_t heartbeat_packet[HEARTBEAT_PACKET_LEN] = {
            HEARTBEAT_HEADER,   // 包头
            0, 0,               // 数据个数（小端）
            HEARTBEAT_CMD,      // 指令码
            0,                  // 错误码
            0                   // CRC8（待计算）
          };
          
          // 计算CRC8（前5个字节）
          heartbeat_packet[5] = Calculate_CRC8(heartbeat_packet, 5);
          
          // 通过串口5发送
          BSP_USART_Heart_Send(heartbeat_packet, sizeof(heartbeat_packet));
          printf("send heartbeat packet\r\n");
        }
      }
      // 从机模式：心跳超时检测
      else
      {
        g_heartbeat_timeout++;
        if (g_heartbeat_timeout >= HEARTBEAT_TIMEOUT_SEC)
        {
          // 超时，切换为主机
          EnvParameter_SwitchToMaster();
        }
      }
    }
  }
}
