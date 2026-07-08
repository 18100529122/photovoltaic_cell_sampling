/**
 * @file shell_cmd_list.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell cmd list
 * @version 3.0.0
 * @date 2020-01-17
 * 
 * @copyright (c) 2020 Letter
 * 
 */

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure.h"
#include "data_process.h"
#include "env_parameter.h"
#include "bsp_can.h"
#include "bsp_flash.h"
#include "easyflash.h"

#if SHELL_USING_CMD_EXPORT != 1

extern int shellSetVar(char *name, int value);
extern void shellUp(Shell *shell);
extern void shellDown(Shell *shell);
extern void shellRight(Shell *shell);
extern void shellLeft(Shell *shell);
extern void shellTab(Shell *shell);
extern void shellBackspace(Shell *shell);
extern void shellDelete(Shell *shell);
extern void shellEnter(Shell *shell);
extern void shellHelp(int argc, char *argv[]);
extern void shellUsers(void);
extern void shellCmds(void);
extern void shellVars(void);
extern void shellKeys(void);
extern void shellClear(void);
#if SHELL_EXEC_UNDEF_FUNC == 1
extern int shellExecute(int argc, char *argv[]);
#endif

SHELL_AGENCY_FUNC(shellRun, shellGetCurrent(), (const char *)p1);

/**
 * @brief 测试命令
 */
static int Test_Cmd(int argc, char *argv[])
{
    shellWriteString(shellGetCurrent(), "Hello from letter shell!\r\n");
    return 0;
}

/**
 * @brief 系统信息命令
 */
static int SysInfo_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    shellWriteString(shell, "=== System Info ===\r\n");
    shellWriteString(shell, "MCU: STM32G474VETx\r\n");
    shellWriteString(shell, "Clock: 170MHz\r\n");
    shellWriteString(shell, "CAN Node: 30\r\n");
    return 0;
}

/**
 * @brief CAN 测试命令
 */
static int CANTest_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    ResultData_t testData;
    int i;
    
    shellWriteString(shell, "Starting CAN test...\r\n");
    
    /* 填充测试数据 */
    for (i = 0; i < 12; i++) {
        testData.voltage[i] = 1.23f + i * 0.1f;
        testData.current[i] = 0.56f + i * 0.05f;
    }
    testData.temperature[0] = 25.5f;
    testData.temperature[1] = 30.2f;
    
    /* 发送数据 */
    if (BSP_CAN_SendData(&testData) == CAN_OK) {
        shellWriteString(shell, "CAN data sent successfully!\r\n");
    } else {
        shellWriteString(shell, "CAN send failed!\r\n");
    }
    
    return 0;
}

/**
 * @brief Flash擦除命令
 * Usage: flash_erase <page> 或 flash_erase all
 */
static int FlashErase_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    HAL_StatusTypeDef status;
    char buffer[128];
    uint32_t actual_page;

    if (argc < 2) {
        shellWriteString(shell, "Usage: flash_erase <page>|all\r\n");
        return -1;
    }

    snprintf(buffer, sizeof(buffer), "Flash info: DATA_START=0x%08X, PAGE_SIZE=%u, PAGE_COUNT=%u\r\n", 
             (unsigned int)FLASH_DATA_START_ADDR, (unsigned int)FLASH_PAGE_SIZE, (unsigned int)FLASH_DATA_PAGE_COUNT);
    shellWriteString(shell, buffer);

    if (strcmp(argv[1], "all") == 0) {
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE;
        snprintf(buffer, sizeof(buffer), "Erasing all %u pages starting from page %u...\r\n", 
                 (unsigned int)FLASH_DATA_PAGE_COUNT, (unsigned int)actual_page);
        shellWriteString(shell, buffer);
        status = BSP_FLASH_EraseAllData();
    } else {
        int page = atoi(argv[1]);
        if (page < 0 || page >= (int)FLASH_DATA_PAGE_COUNT) {
            snprintf(buffer, sizeof(buffer), "Invalid page! (0-%u)\r\n", (unsigned int)(FLASH_DATA_PAGE_COUNT - 1));
            shellWriteString(shell, buffer);
            return -1;
        }
        actual_page = (FLASH_DATA_START_ADDR - FLASH_BASE) / FLASH_PAGE_SIZE + page;
        snprintf(buffer, sizeof(buffer), "Erasing logical page %d (physical page %u)...\r\n", page, (unsigned int)actual_page);
        shellWriteString(shell, buffer);
        status = BSP_FLASH_ErasePage(page);
    }

    if (status == HAL_OK) {
        shellWriteString(shell, "Erase done!\r\n");
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Erase failed! (status=%d)\r\n", status);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief Flash写入命令
 * Usage: flash_write <addr> <data1> <data2> ...
 */
static int FlashWrite_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    HAL_StatusTypeDef status;
    uint32_t addr;
    uint8_t data[128];
    int i;
    char buffer[64];
    
    if (argc < 3) {
        shellWriteString(shell, "Usage: flash_write <addr> <data1> <data2> ...\r\n");
        return -1;
    }
    
    addr = strtoul(argv[1], NULL, 0);
    if (addr < FLASH_DATA_START_ADDR || addr >= FLASH_DATA_END_ADDR) {
        snprintf(buffer, sizeof(buffer), "Invalid addr! (0x%08X-0x%08X)\r\n", 
                 (unsigned int)FLASH_DATA_START_ADDR, (unsigned int)(FLASH_DATA_END_ADDR - 1));
        shellWriteString(shell, buffer);
        return -1;
    }
    
    int data_len = argc - 2;
    if (data_len > 128) data_len = 128;
    
    for (i = 0; i < data_len; i++) {
        data[i] = (uint8_t)strtoul(argv[2 + i], NULL, 0);
    }
    
    snprintf(buffer, sizeof(buffer), "Writing %d bytes to 0x%08X...\r\n", data_len, (unsigned int)addr);
    shellWriteString(shell, buffer);
    
    status = BSP_FLASH_Write(addr, data, data_len);
    
    if (status == HAL_OK) {
        shellWriteString(shell, "Write done!\r\n");
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Write failed! (status=%d)\r\n", status);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief Flash读取命令
 * Usage: flash_read <addr> <len>
 */
static int FlashRead_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    uint32_t addr;
    uint32_t len;
    uint8_t data[128];
    int i;
    char buffer[128];
    
    if (argc < 3) {
        shellWriteString(shell, "Usage: flash_read <addr> <len>\r\n");
        return -1;
    }
    
    addr = strtoul(argv[1], NULL, 0);
    len = strtoul(argv[2], NULL, 0);
    
    if (addr < FLASH_DATA_START_ADDR || (addr + len) > FLASH_DATA_END_ADDR) {
        snprintf(buffer, sizeof(buffer), "Invalid addr/len! (0x%08X-0x%08X)\r\n", 
                 (unsigned int)FLASH_DATA_START_ADDR, (unsigned int)(FLASH_DATA_END_ADDR - 1));
        shellWriteString(shell, buffer);
        return -1;
    }
    
    if (len > 128) len = 128;
    
    BSP_FLASH_Read(addr, data, len);
    
    snprintf(buffer, sizeof(buffer), "Read %u bytes from 0x%08X:\r\n", (unsigned int)len, (unsigned int)addr);
    shellWriteString(shell, buffer);
    
    for (i = 0; i < (int)len; i++) {
        if (i % 16 == 0) {
            if (i > 0) shellWriteString(shell, "\r\n");
            snprintf(buffer, sizeof(buffer), "  %04X: ", (unsigned int)(addr + i - FLASH_DATA_START_ADDR));
            shellWriteString(shell, buffer);
        }
        snprintf(buffer, sizeof(buffer), "%02X ", data[i]);
        shellWriteString(shell, buffer);
    }
    shellWriteString(shell, "\r\n");
    
    return 0;
}

/**
 * @brief ENV读取命令
 * Usage: env_get <key>
 */
static int EnvGet_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    
    if (argc < 2) {
        shellWriteString(shell, "Usage: env_get <key>\r\n");
        return -1;
    }
    
    const char *value = ef_get_env(argv[1]);
    if (value == NULL) {
        snprintf(buffer, sizeof(buffer), "Env '%s' not found!\r\n", argv[1]);
        shellWriteString(shell, buffer);
        return -1;
    }
    
    snprintf(buffer, sizeof(buffer), "Env '%s' = '%s'\r\n", argv[1], value);
    shellWriteString(shell, buffer);
    
    return 0;
}

/**
 * @brief ENV设置命令
 * Usage: env_set <key> <value>
 */
static int EnvSet_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    EfErrCode err;
    
    if (argc < 3) {
        shellWriteString(shell, "Usage: env_set <key> <value>\r\n");
        return -1;
    }
    
    err = ef_set_and_save_env(argv[1], argv[2]);
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Env '%s' set to '%s' and saved!\r\n", argv[1], argv[2]);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set env failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief ENV删除命令
 * Usage: env_del <key>
 */
static int EnvDel_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    EfErrCode err;
    
    if (argc < 2) {
        shellWriteString(shell, "Usage: env_del <key>\r\n");
        return -1;
    }
    
    err = ef_del_and_save_env(argv[1]);
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Env '%s' deleted and saved!\r\n", argv[1]);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Delete env failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置主从机命令
 * Usage: env_set_master 1 或 env_set_master 0
 */
static int EnvSetMaster_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    EfErrCode err;
    
    if (argc != 2) {
        shellWriteString(shell, "Usage: env_set_master 1|0\r\n");
        return -1;
    }
    
    int is_master = atoi(argv[1]);
    const char *role_str = is_master ? "master" : "slave";
    
    err = ef_set_and_save_env("node_role", role_str);
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set node role to %s and saved! Please reboot to take effect.\r\n", role_str);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置电压通道k/b值命令
 * Usage: env_set_vs <channel> <k> <b>
 */
static int EnvSetVs_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    float k_array[ADC_CH_VOLTAGE_COUNT];
    float b_array[ADC_CH_VOLTAGE_COUNT];
    int channel;
    float k, b;
    EfErrCode err;
    
    if (argc != 4) {
        shellWriteString(shell, "Usage: env_set_vs <0-11> <k> <b>\r\n");
        return -1;
    }
    
    channel = atoi(argv[1]);
    k = atof(argv[2]);
    b = atof(argv[3]);
    
    if (channel < 0 || channel >= ADC_CH_VOLTAGE_COUNT) {
        shellWriteString(shell, "Invalid channel! (0-11)\r\n");
        return -1;
    }
    
    // 读取现有值或使用默认值
    {
        const char *value = ef_get_env("vs_k");
        if (value) {
            StringToFloatArray(value, k_array, ADC_CH_VOLTAGE_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) k_array[i] = 1.0f;
        }
    }
    {
        const char *value = ef_get_env("vs_b");
        if (value) {
            StringToFloatArray(value, b_array, ADC_CH_VOLTAGE_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) b_array[i] = 0.0f;
        }
    }
    
    // 修改指定通道
    k_array[channel] = k;
    b_array[channel] = b;
    
    // 转换为字符串并保存
    {
        char k_str[256], b_str[256];
        FloatArrayToString(k_array, ADC_CH_VOLTAGE_COUNT, k_str, sizeof(k_str));
        FloatArrayToString(b_array, ADC_CH_VOLTAGE_COUNT, b_str, sizeof(b_str));
        
        err = ef_set_and_save_env("vs_k", k_str);
        if (err == EF_NO_ERR) {
            err = ef_set_and_save_env("vs_b", b_str);
        }
    }
    
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set vs[%d] k=%.6f, b=%.6f and saved!\r\n", channel, k, b);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置电流通道k/b值命令
 * Usage: env_set_cur <channel> <k> <b>
 */
static int EnvSetCur_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    float k_array[ADC_CH_CURRENT_COUNT];
    float b_array[ADC_CH_CURRENT_COUNT];
    int channel;
    float k, b;
    EfErrCode err;
    
    if (argc != 4) {
        shellWriteString(shell, "Usage: env_set_cur <0-11> <k> <b>\r\n");
        return -1;
    }
    
    channel = atoi(argv[1]);
    k = atof(argv[2]);
    b = atof(argv[3]);
    
    if (channel < 0 || channel >= ADC_CH_CURRENT_COUNT) {
        shellWriteString(shell, "Invalid channel! (0-11)\r\n");
        return -1;
    }
    
    // 读取现有值或使用默认值
    {
        const char *value = ef_get_env("cur_k");
        if (value) {
            StringToFloatArray(value, k_array, ADC_CH_CURRENT_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) k_array[i] = 1.0f;
        }
    }
    {
        const char *value = ef_get_env("cur_b");
        if (value) {
            StringToFloatArray(value, b_array, ADC_CH_CURRENT_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) b_array[i] = 0.0f;
        }
    }
    
    // 修改指定通道
    k_array[channel] = k;
    b_array[channel] = b;
    
    // 转换为字符串并保存
    {
        char k_str[256], b_str[256];
        FloatArrayToString(k_array, ADC_CH_CURRENT_COUNT, k_str, sizeof(k_str));
        FloatArrayToString(b_array, ADC_CH_CURRENT_COUNT, b_str, sizeof(b_str));
        
        err = ef_set_and_save_env("cur_k", k_str);
        if (err == EF_NO_ERR) {
            err = ef_set_and_save_env("cur_b", b_str);
        }
    }
    
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set cur[%d] k=%.6f, b=%.6f and saved!\r\n", channel, k, b);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置温度通道k/b值命令
 * Usage: env_set_temp <channel> <k> <b>
 */
static int EnvSetTemp_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    float k_array[ADC_CH_TEMP_COUNT];
    float b_array[ADC_CH_TEMP_COUNT];
    int channel;
    float k, b;
    EfErrCode err;
    
    if (argc != 4) {
        shellWriteString(shell, "Usage: env_set_temp <0-1> <k> <b>\r\n");
        return -1;
    }
    
    channel = atoi(argv[1]);
    k = atof(argv[2]);
    b = atof(argv[3]);
    
    if (channel < 0 || channel >= ADC_CH_TEMP_COUNT) {
        shellWriteString(shell, "Invalid channel! (0-1)\r\n");
        return -1;
    }
    
    // 读取现有值或使用默认值
    {
        const char *value = ef_get_env("temp_k");
        if (value) {
            StringToFloatArray(value, k_array, ADC_CH_TEMP_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_TEMP_COUNT; i++) k_array[i] = 1.0f;
        }
    }
    {
        const char *value = ef_get_env("temp_b");
        if (value) {
            StringToFloatArray(value, b_array, ADC_CH_TEMP_COUNT);
        } else {
            int i;
            for (i = 0; i < ADC_CH_TEMP_COUNT; i++) b_array[i] = 0.0f;
        }
    }
    
    // 修改指定通道
    k_array[channel] = k;
    b_array[channel] = b;
    
    // 转换为字符串并保存
    {
        char k_str[256], b_str[256];
        FloatArrayToString(k_array, ADC_CH_TEMP_COUNT, k_str, sizeof(k_str));
        FloatArrayToString(b_array, ADC_CH_TEMP_COUNT, b_str, sizeof(b_str));
        
        err = ef_set_and_save_env("temp_k", k_str);
        if (err == EF_NO_ERR) {
            err = ef_set_and_save_env("temp_b", b_str);
        }
    }
    
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set temp[%d] k=%.6f, b=%.6f and saved!\r\n", channel, k, b);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置CAN节点地址命令
 * Usage: env_set_can_addr <addr>
 */
static int EnvSetCanAddr_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    int addr;
    EfErrCode err;
    
    if (argc != 2) {
        shellWriteString(shell, "Usage: env_set_can_addr <addr>\r\n");
        return -1;
    }
    
    addr = atoi(argv[1]);
    
    if (addr < 0 || addr > 127) {
        shellWriteString(shell, "Invalid address! (0-127)\r\n");
        return -1;
    }
    
    // 更新内存中的值
    g_env_config.can_node_addr = (uint8_t)addr;
    
    // 保存到Flash
    snprintf(buffer, sizeof(buffer), "%d", addr);
    err = ef_set_and_save_env("can_node_addr", buffer);
    
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set can_node_addr = %d and saved! (Reboot to take effect)\r\n", addr);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief 设置是否打印ADC数据命令
 * Usage: env_set_print_adc <1|0>
 */
static int EnvSetPrintAdc_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    int enable;
    EfErrCode err;
    
    if (argc != 2) {
        shellWriteString(shell, "Usage: env_set_print_adc <1|0>\r\n");
        return -1;
    }
    
    enable = atoi(argv[1]);
    
    if (enable != 0 && enable != 1) {
        shellWriteString(shell, "Invalid value! (0 or 1)\r\n");
        return -1;
    }
    
    // 更新内存中的值
    g_env_config.print_adc_data = (uint8_t)enable;
    
    // 保存到Flash
    snprintf(buffer, sizeof(buffer), "%d", enable);
    err = ef_set_and_save_env("print_adc_data", buffer);
    
    if (err == EF_NO_ERR) {
        snprintf(buffer, sizeof(buffer), "Set print_adc_data = %d and saved! (Takes effect immediately)\r\n", enable);
        shellWriteString(shell, buffer);
        return 0;
    } else {
        snprintf(buffer, sizeof(buffer), "Set failed! Error: %d\r\n", err);
        shellWriteString(shell, buffer);
        return -1;
    }
}

/**
 * @brief ENV打印命令
 * Usage: env_print
 */
static int EnvPrint_Cmd(int argc, char *argv[])
{
    if (argc > 1) {
        shellWriteString(shellGetCurrent(), "Usage: env_print\r\n");
        return -1;
    }
    
    // 使用EasyFlash自带的函数打印所有env
    shellWriteString(shellGetCurrent(), "=== Env List ===\r\n");
    ef_print_env();
    
    return 0;
}

/**
 * @brief ADC数据打印命令
 * Usage: adc_print
 */
static int AdcPrint_Cmd(int argc, char *argv[])
{
    Shell *shell = shellGetCurrent();
    char buffer[128];
    int i;
    
    shellWriteString(shell, "=== ADC Data ===\r\n");
    
    // 获取ADC数据并应用校准
    ADC_Data_t *adc_data_ptr = DataProcess_GetData();
    ResultData_t result_data;
    
    // 应用k/b校准公式: 真实值 = ADC值 * k + b
    for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        result_data.voltage[i] = adc_data_ptr->vs_result[i] * g_env_config.vs_k[i] + g_env_config.vs_b[i];
    }
    for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        result_data.current[i] = adc_data_ptr->cur_result[i] * g_env_config.cur_k[i] + g_env_config.cur_b[i];
    }
    for (i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        result_data.temperature[i] = adc_data_ptr->temp_result[i] * g_env_config.temp_k[i] + g_env_config.temp_b[i];
    }
    
    // 打印电压数据
    shellWriteString(shell, "Voltage (VS0-VS11):\r\n");
    for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        snprintf(buffer, sizeof(buffer), "  VS%d: %.6f\r\n", i, result_data.voltage[i]);
        shellWriteString(shell, buffer);
    }
    
    // 打印电流数据
    shellWriteString(shell, "Current (CUR0-CUR11):\r\n");
    for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        snprintf(buffer, sizeof(buffer), "  CUR%d: %.6f\r\n", i, result_data.current[i]);
        shellWriteString(shell, buffer);
    }
    
    // 打印温度数据
    shellWriteString(shell, "Temperature (TEMP0-TEMP1):\r\n");
    for (i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        snprintf(buffer, sizeof(buffer), "  TEMP%d: %.6f\r\n", i, result_data.temperature[i]);
        shellWriteString(shell, buffer);
    }
    
    return 0;
}

/**
 * @brief shell命令表
 * 
 */
const ShellCommand shellCommandList[] = 
{
    {.attr.value=SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_USER),
    .data.user.name = SHELL_DEFAULT_USER,
    .data.user.password = SHELL_DEFAULT_USER_PASSWORD,
    .data.user.desc = "default user"},
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                   setVar, shellSetVar, set var),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x1B5B4100, shellUp, up),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x1B5B4200, shellDown, down),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B4300, shellRight, right),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B4400, shellLeft, left),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0), 0x09000000, shellTab, tab),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x08000000, shellBackspace, backspace),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x7F000000, shellDelete, delete),
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x1B5B337E, shellDelete, delete),
#if SHELL_ENTER_LF == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0A000000, shellEnter, enter),
#endif
#if SHELL_ENTER_CR == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0D000000, shellEnter, enter),
#endif
#if SHELL_ENTER_CRLF == 1
    SHELL_KEY_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_ENABLE_UNCHECKED,
                   0x0D0A0000, shellEnter, enter),
#endif
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
                   help, shellHelp, show command info\r\nhelp [cmd]),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   users, shellUsers, list all user),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   cmds, shellCmds, list all cmd),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   vars, shellVars, list all var),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   keys, shellKeys, list all key),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   clear, shellClear, clear console),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
                   sh, SHELL_AGENCY_FUNC_NAME(shellRun), run command directly),
#if SHELL_EXEC_UNDEF_FUNC == 1
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
                   exec, shellExecute, execute function undefined),
#endif
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   test, Test_Cmd, test command),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   sysinfo, SysInfo_Cmd, show system info),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   cantest, CANTest_Cmd, test CAN send),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_erase, FlashErase_Cmd, erase flash: flash_erase <page>|all),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_write, FlashWrite_Cmd, write flash: flash_write <addr> <data1> <data2> ...),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   flash_read, FlashRead_Cmd, read flash: flash_read <addr> <len>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_get, EnvGet_Cmd, get env: env_get <key>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set, EnvSet_Cmd, set env: env_set <key> <value>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_del, EnvDel_Cmd, delete env: env_del <key>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_print, EnvPrint_Cmd, print all envs: env_print),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_master, EnvSetMaster_Cmd, set master/slave: env_set_master 1|0),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_vs, EnvSetVs_Cmd, set vs k/b: env_set_vs <0-11> <k> <b>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_cur, EnvSetCur_Cmd, set cur k/b: env_set_cur <0-11> <k> <b>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_temp, EnvSetTemp_Cmd, set temp k/b: env_set_temp <0-1> <k> <b>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_can_addr, EnvSetCanAddr_Cmd, set CAN node address: env_set_can_addr <addr>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   env_set_print_adc, EnvSetPrintAdc_Cmd, set ADC print enable: env_set_print_adc <1|0>),
    SHELL_CMD_ITEM(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                   adc_print, AdcPrint_Cmd, print ADC data: adc_print),
};


/**
 * @brief shell命令表大小
 * 
 */
const unsigned short shellCommandCount 
    = sizeof(shellCommandList) / sizeof(ShellCommand);

#endif
