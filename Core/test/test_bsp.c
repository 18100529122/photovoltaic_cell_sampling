/**
 * @file     test_bsp.c
 * @brief    测试板级支持包源文件
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "test_bsp.h"
#include "bsp_adc.h"
#include "data_process.h"
#include <stdio.h>
#include <string.h>

/*========================= 宏定义 (Macros) ================================*/

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/

#if (TEST_ENABLE)
/* 测试总开关开启 */
/**
 * @brief  运行所有测试
 */
void Test_Run(void)
{
    printf("\r\n========================================\r\n");
    printf("  BSP Test Suite\r\n");
    printf("========================================\r\n");

#if (TEST_ENABLE_UART)
    Test_UART_Printf();
#endif

#if (TEST_ENABLE_ADC)
    Test_ADC_RawData();
#endif

#if (TEST_ENABLE_DATA_PROCESS)
    Test_DataProcess_Sort();
    Test_DataProcess_Average();
#endif

    printf("\r\n========================================\r\n");
    printf("  All Tests Completed\r\n");
    printf("========================================\r\n");
}
#endif

#if (TEST_ENABLE_UART)
/**
 * @brief  测试串口printf功能
 */
void Test_UART_Printf(void)
{
    printf("\r\n[TEST] UART Printf Test\r\n");
    printf("  Hello, this is a test string!\r\n");
    printf("  Integer test: %d, %ld\r\n", 123, 456789L);
    printf("  Float test: %.2f, %.3f\r\n", 3.14f, 2.718f);
    printf("  [PASS] UART Printf OK\r\n");
}
#endif

#if (TEST_ENABLE_ADC)
/**
 * @brief  测试ADC原始数据
 */
void Test_ADC_RawData(void)
{
    printf("\r\n[TEST] ADC Raw Data Test\r\n");
    
    ADC_RawData_t* raw = BSP_ADC_GetRawData();
    if (raw == NULL) {
        printf("  [FAIL] ADC raw data is NULL\r\n");
        return;
    }
    
    /* 打印ADC1所有通道 (7通道) */
    printf("  ADC1 (7 channels): ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", raw->adc1_data[i]);
    }
    printf("\r\n");
    
    /* 打印ADC2所有通道 (7通道) */
    printf("  ADC2 (7 channels): ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", raw->adc2_data[i]);
    }
    printf("\r\n");
    
    /* 打印ADC3所有通道 (5通道) */
    printf("  ADC3 (5 channels): ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", raw->adc3_data[i]);
    }
    printf("\r\n");
    
    /* 打印ADC4所有通道 (7通道) */
    printf("  ADC4 (7 channels): ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", raw->adc4_data[i]);
    }
    printf("\r\n");
    
    printf("  [PASS] ADC Raw Data OK\r\n");
}
#endif

#if (TEST_ENABLE_DATA_PROCESS)
/**
 * @brief  测试数据处理排序算法
 */
void Test_DataProcess_Sort(void)
{
    printf("\r\n[TEST] Data Process Sort Test\r\n");
    
    uint16_t test_data[10] = {5, 2, 9, 1, 7, 3, 8, 4, 6, 0};
    uint16_t sorted[10];
    memcpy(sorted, test_data, sizeof(test_data));
    
    /* 使用冒泡排序 */
    for (uint16_t i = 0; i < 10 - 1; i++) {
        for (uint16_t j = 0; j < 10 - i - 1; j++) {
            if (sorted[j] < sorted[j + 1]) {
                uint16_t temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    printf("  Original: ");
    for (int i = 0; i < 10; i++) printf("%d ", test_data[i]);
    printf("\r\n  Sorted:   ");
    for (int i = 0; i < 10; i++) printf("%d ", sorted[i]);
    printf("\r\n");
    
    /* 验证是否降序 */
    uint8_t pass = 1;
    for (int i = 0; i < 9; i++) {
        if (sorted[i] < sorted[i + 1]) {
            pass = 0;
            break;
        }
    }
    
    if (pass) {
        printf("  [PASS] Sort Test OK\r\n");
    } else {
        printf("  [FAIL] Sort Test Failed\r\n");
    }
}

/**
 * @brief  测试数据处理平均值计算
 */
void Test_DataProcess_Average(void)
{
    printf("\r\n[TEST] Data Process Average Test\r\n");
    
    ADC_SampleBuffer_t buffer;
    buffer.write_index = 200;
    buffer.buffer_full = 1;
    
    /* 填充测试数据：1-200 */
    for (int i = 0; i < 200; i++) {
        buffer.samples[i] = i + 1;
    }
    
    /* 调用处理函数 */
    float avg = DataProcess_ProcessChannel(&buffer);
    
    /* 预期结果：去掉10个最大(191-200)和10个最小(1-10)，剩下11-190
       总和 = (11+190)*180/2 = 201*90 = 18090
       平均值 = 18090 / 180 = 100.5
    */
    float expected = 100.5f;
    
    printf("  Calculated avg: %.2f\r\n", avg);
    printf("  Expected avg:   %.2f\r\n", expected);
    
    if ((avg > expected - 0.1f) && (avg < expected + 0.1f)) {
        printf("  [PASS] Average Test OK\r\n");
    } else {
        printf("  [FAIL] Average Test Failed\r\n");
    }
}
#endif


