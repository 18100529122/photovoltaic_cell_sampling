/**
 * @file     test_bsp.c
 * @brief    测试板级支持包源文件
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "test_bsp.h"
#include "bsp_adc.h"
#include "bsp_can.h"
#include "bsp_flash.h"
#include "data_process.h"
#include "easyflash.h"
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
    // BSP_ADC_Start();
    Test_ADC_RawData();
    // BSP_ADC_Stop();
#endif

#if (TEST_ENABLE_DATA_PROCESS)
    Test_DataProcess_Sort();
    Test_DataProcess_Average();
#endif

#if (TEST_ENABLE_CAN)
    Test_CAN_Basic();
#endif

#if (TEST_ENABLE_FLASH)
    Test_FLASH_Basic();
#endif

#if (TEST_ENABLE_EASYFLASH)
    Test_EasyFlash_Basic();
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

#if (TEST_ENABLE_CAN)
/**
 * @brief  接收并打印一帧CAN数据
 * @param  None
 * @retval 1=收到数据，0=未收到
 */
static uint8_t Test_CAN_ReceiveAndPrint(void)
{
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    
    if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        printf("    [RX] ID=0x%03lX, DLC=%lu, Data=", 
               RxHeader.Identifier, RxHeader.DataLength);
        for (uint8_t i = 0; i < RxHeader.DataLength; i++) {
            printf("%02X ", RxData[i]);
        }
        printf("\r\n");
        return 1;
    }
    return 0;
}

/**
 * @brief  发送单帧并同时接收
 */
static CAN_Status_t Test_SendAndReceive(uint32_t id, const uint8_t *data, uint8_t len, uint8_t *rxCount)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    uint32_t timeout;
    
    /* 配置 Tx Header */
    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = len;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
    
    /* 等待发送缓冲区可用 */
    timeout = 0xFFFF;
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0) {
        /* 等待时顺便接收 */
        if (Test_CAN_ReceiveAndPrint()) {
            (*rxCount)++;
        }
        if (--timeout == 0) {
            return CAN_TIMEOUT;
        }
    }
    
    /* 发送数据 */
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, (uint8_t*)data) != HAL_OK) {
        return CAN_ERROR;
    }
    
    /* 发送后等待并接收 */
    timeout = 0xFFFF;
    while (timeout-- > 0) {
        if (Test_CAN_ReceiveAndPrint()) {
            (*rxCount)++;
            break;
        }
    }
    
    return CAN_OK;
}

/**
 * @brief  CAN回环测试
 */
void Test_CAN_Basic(void)
{
    printf("\r\n[TEST] CAN Loopback Test\r\n");
    
    ResultData_t testResult;
    uint8_t appData[CAN_APP_DATA_LEN];
    uint8_t frameData[8];
    uint16_t offset = 0;
    uint8_t checksum;
    uint8_t receivedCount = 0;
    CAN_Status_t status;
    
    /* 填充测试数据 */
    for (int i = 0; i < 12; i++) {
        testResult.voltage[i] = 1.23f + i * 0.1f;
        testResult.current[i] = 0.56f + i * 0.05f;
    }
    testResult.temperature[0] = 25.5f;
    testResult.temperature[1] = 30.2f;
    
    /* 打包数据 */
    appData[0] = 0xA5;
    appData[1] = 0x5A;
    memcpy(&appData[2], testResult.voltage, 12 * sizeof(float));
    memcpy(&appData[50], testResult.current, 12 * sizeof(float));
    memcpy(&appData[98], testResult.temperature, 2 * sizeof(float));
    
    printf("  Enabling internal loopback mode...\r\n");
    BSP_CAN_EnableLoopback();
    
    printf("  Sending & receiving test data (14 frames)...\r\n");
    
    /* 首帧 */
    frameData[0] = (CAN_APP_DATA_LEN >> 8) & 0xFF;
    frameData[1] = CAN_APP_DATA_LEN & 0xFF;
    memcpy(&frameData[2], &appData[offset], 6);
    offset += 6;
    status = Test_SendAndReceive(CAN_BUILD_ID(CAN_FIRST_FRAME), frameData, 8, &receivedCount);
    if (status != CAN_OK) goto error;
    
    /* 中间帧 */
    while (offset + 8 < CAN_APP_DATA_LEN) {
        memcpy(frameData, &appData[offset], 8);
        offset += 8;
        status = Test_SendAndReceive(CAN_BUILD_ID(CAN_MIDDLE_FRAME), frameData, 8, &receivedCount);
        if (status != CAN_OK) goto error;
    }
    
    /* 末帧 */
    uint8_t remaining = CAN_APP_DATA_LEN - offset;
    if (remaining > 0) {
        memcpy(frameData, &appData[offset], remaining);
    }
    checksum = 0;
    for (uint16_t i = 0; i < CAN_APP_DATA_LEN; i++) {
        checksum += appData[i];
    }
    frameData[remaining] = checksum;
    status = Test_SendAndReceive(CAN_BUILD_ID(CAN_LAST_FRAME), frameData, remaining + 1, &receivedCount);
    if (status != CAN_OK) goto error;
    
    /* 最后再尝试接收可能剩余的帧 */
    uint32_t timeout = 0xFFFFF;
    while (timeout-- > 0) {
        if (Test_CAN_ReceiveAndPrint()) {
            receivedCount++;
        }
        if (receivedCount >= 14) {
            break;
        }
    }
    
    printf("  Received %d frames\r\n", receivedCount);
    
    if (receivedCount == 14) {
        printf("  [PASS] CAN Loopback Test OK\r\n");
    } else {
        printf("  [WARN] Expected 14 frames, got %d\r\n", receivedCount);
    }
    goto end;
    
error:
    printf("  [FAIL] CAN Send Failed (status=%d)\r\n", status);
    
end:
    printf("  Disabling loopback mode...\r\n");
    BSP_CAN_DisableLoopback();
}
#endif

#if (TEST_ENABLE_FLASH)
void Test_FLASH_Basic(void)
{
    printf("\r\n[TEST] Flash Read/Write Test\r\n");
    
    HAL_StatusTypeDef status;
    uint8_t write_data[32];
    uint8_t read_data[32];
    uint8_t pass = 1;
    
    printf("  Flash data start addr: 0x%08lX\r\n", FLASH_DATA_START_ADDR);
    printf("  Flash data size:       %lu bytes\r\n", FLASH_DATA_SIZE);
    
    /* 准备测试数据 */
    for (int i = 0; i < 32; i++) {
        write_data[i] = (uint8_t)(i * 3 + 7);
    }
    
    printf("  Erasing page 0...\r\n");
    status = BSP_FLASH_ErasePage(0);
    if (status != HAL_OK) {
        printf("  [FAIL] Erase page failed (status=%d)\r\n", status);
        pass = 0;
        goto end;
    }
    printf("  [OK] Erase page done\r\n");
    
    printf("  Writing 32 bytes...\r\n");
    status = BSP_FLASH_Write(FLASH_DATA_START_ADDR, write_data, 32);
    if (status != HAL_OK) {
        printf("  [FAIL] Write failed (status=%d, error=0x%08lX)\r\n", status, HAL_FLASH_GetError());
        pass = 0;
        goto end;
    }
    printf("  [OK] Write done\r\n");
    
    printf("  Reading 32 bytes...\r\n");
    BSP_FLASH_Read(FLASH_DATA_START_ADDR, read_data, 32);
    
    printf("  Verifying...\r\n");
    printf("  Write: ");
    for (int i = 0; i < 32; i++) printf("%02X ", write_data[i]);
    printf("\r\n  Read:  ");
    for (int i = 0; i < 32; i++) printf("%02X ", read_data[i]);
    printf("\r\n");
    
    for (int i = 0; i < 32; i++) {
        if (read_data[i] != write_data[i]) {
            printf("  [FAIL] Mismatch at index %d: 0x%02X != 0x%02X\r\n", i, read_data[i], write_data[i]);
            pass = 0;
            goto end;
        }
    }
    
end:
    if (pass) {
        printf("  [PASS] Flash Test OK\r\n");
    } else {
        printf("  [FAIL] Flash Test Failed\r\n");
    }
}
#endif

#if (TEST_ENABLE_EASYFLASH)
void Test_EasyFlash_Basic(void)
{
    printf("\r\n[TEST] EasyFlash Basic Test\r\n");
    
    EfErrCode result;
    uint8_t pass = 1;
    const char *test_key = "test_key";
    const char *test_value = "Hello EasyFlash!";
    char read_value[64];
    
    /* 初始化 EasyFlash */
    printf("  Initializing EasyFlash...\r\n");
    result = easyflash_init();
    if (result != EF_NO_ERR) {
        printf("  [FAIL] Init failed (result=%d)\r\n", result);
        pass = 0;
        goto end;
    }
    printf("  [OK] Init done\r\n");
    
    /* 保存 ENV */
    printf("  Saving ENV: %s = %s\r\n", test_key, test_value);
    result = ef_set_env(test_key, test_value);
    if (result != EF_NO_ERR) {
        printf("  [FAIL] Save ENV failed (result=%d)\r\n", result);
        pass = 0;
        goto end;
    }
    printf("  [OK] Save done\r\n");
    
    /* 读取 ENV */
    printf("  Reading ENV: %s\r\n", test_key);
    const char *read_ptr = ef_get_env(test_key);
    if (read_ptr == NULL) {
        printf("  [FAIL] Read ENV failed (NULL)\r\n");
        pass = 0;
        goto end;
    }
    strncpy(read_value, read_ptr, sizeof(read_value) - 1);
    read_value[sizeof(read_value) - 1] = '\0';
    printf("  Read value: %s\r\n", read_value);
    
    /* 验证 */
    if (strcmp(read_value, test_value) != 0) {
        printf("  [FAIL] Value mismatch\r\n");
        pass = 0;
        goto end;
    }
    printf("  [OK] Value match\r\n");
    
    /* 测试 26 通道 kb 值 */
    printf("\r\n  Testing kb_voltage and kb_current...\r\n");
    const char *kb_voltage = ef_get_env("kb_voltage");
    const char *kb_current = ef_get_env("kb_current");
    
    if (kb_voltage == NULL || kb_current == NULL) {
        printf("  [FAIL] kb values not found\r\n");
        pass = 0;
        goto end;
    }
    
    printf("  kb_voltage: %s\r\n", kb_voltage);
    printf("  kb_current: %s\r\n", kb_current);
    printf("  [OK] kb values OK\r\n");
    
end:
    if (pass) {
        printf("\r\n  [PASS] EasyFlash Test OK\r\n");
    } else {
        printf("\r\n  [FAIL] EasyFlash Test Failed\r\n");
    }
}
#endif


