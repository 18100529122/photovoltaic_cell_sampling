#ifndef __BSP_TEST_H__
#define __BSP_TEST_H__
/**
 * @file     test_bsp.h
 * @brief    测试板级支持包头文件
 */
#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"

/*========================= 宏定义 (Macros) ================================*/
/* 测试功能开关 - 1=开启，0=关闭 */
#define TEST_ENABLE               0    /* 测试总开关 */
#if (TEST_ENABLE)
#define TEST_ENABLE_ADC           0    /* ADC测试 */
#define TEST_ENABLE_DATA_PROCESS  0    /* 数据处理测试 */
#define TEST_ENABLE_UART          0    /* 串口测试 */
#define TEST_ENABLE_CAN           0    /* CAN测试 */
#define TEST_ENABLE_FLASH         0    /* Flash测试 */
#else
/* 测试总开关关闭 */
#define TEST_ENABLE_ADC           0    /* ADC测试 */
#define TEST_ENABLE_DATA_PROCESS  0    /* 数据处理测试 */
#define TEST_ENABLE_UART          0    /* 串口测试 */
#define TEST_ENABLE_CAN           0    /* CAN测试 */
#define TEST_ENABLE_FLASH         0    /* Flash测试 */
#endif

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/
#if (TEST_ENABLE)
void Test_Run(void);
#endif

#if (TEST_ENABLE_ADC)
void Test_ADC_RawData(void);
#endif

#if (TEST_ENABLE_DATA_PROCESS)
void Test_DataProcess_Sort(void);
void Test_DataProcess_Average(void);
#endif

#if (TEST_ENABLE_UART)
void Test_UART_Printf(void);
#endif

#if (TEST_ENABLE_CAN)
void Test_CAN_Basic(void);
#endif

#if (TEST_ENABLE_FLASH)
void Test_FLASH_Basic(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* __BSP_TEST_H__ */
