/**
 * @file     env_parameter.c
 * @brief    env参数实现
 */

/*========================= 头文件包含 (Includes) ==========================*/
#include "env_parameter.h"
#include "easyflash.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"

/*========================= 宏定义 (Macros) ================================*/
#define ENV_STR_MAX_LEN     256     /* 字符串最大长度 */

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/
Env_Config_t g_env_config;
uint32_t g_heartbeat_timeout;

/*========================= 静态变量 (Static Variables) ====================*/

/*========================= 静态函数声明 (Static Function Declarations) ====*/

/*========================= 函数实现 (Function Definitions) ================*/
/**
 * @brief 计算CRC8校验码
 */
uint8_t Calculate_CRC8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0x00;
    uint16_t i, j;
    
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief 浮点数组转字符串（逗号分隔）
 */
void FloatArrayToString(const float *array, uint16_t count, char *str, uint16_t str_len)
{
    char temp[32];
    uint16_t i;
    
    str[0] = '\0';
    for (i = 0; i < count; i++) {
        snprintf(temp, sizeof(temp), "%.6f", array[i]);
        strncat(str, temp, str_len - strlen(str) - 1);
        if (i < count - 1) {
            strncat(str, ",", str_len - strlen(str) - 1);
        }
    }
}

/**
 * @brief 字符串转浮点数组（逗号分隔）
 */
void StringToFloatArray(const char *str, float *array, uint16_t count)
{
    char buffer[ENV_STR_MAX_LEN];
    char *token;
    uint16_t i = 0;
    
    strncpy(buffer, str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    token = strtok(buffer, ",");
    while (token != NULL && i < count) {
        array[i] = atof(token);
        i++;
        token = strtok(NULL, ",");
    }
}

/**
 * @brief 加载所有env参数到结构体
 */
void EnvParameter_Load(void)
{
    const char *value;
    uint16_t i;
    uint8_t need_save = 0;
    
    /* ========== 1. 先设置所有默认值 ========== */
    /* 默认主机 */
    g_env_config.is_master = 1;
    
    /* 默认 CAN 节点地址 */
    g_env_config.can_node_addr = 30;
    
    /* 默认不打印ADC数据 */
    g_env_config.print_adc_data = 0;
    
    /* k值默认为1.0，b值默认为0.0 */
    for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        g_env_config.vs_k[i] = 1.0f;
        g_env_config.vs_b[i] = 0.0f;
    }
    for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        g_env_config.cur_k[i] = 1.0f;
        g_env_config.cur_b[i] = 0.0f;
    }
    for (i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        g_env_config.temp_k[i] = 1.0f;
        g_env_config.temp_b[i] = 0.0f;
    }
    
    /* ========== 2. 加载并检查每个 env ========== */
    
    /* 加载 node_role */
    value = ef_get_env("node_role");
    if (value == NULL) {
        /* 未找到，需要创建默认值 */
        ef_set_env("node_role", "master");
        need_save = 1;
    } else if (strcmp(value, "slave") == 0) {
        g_env_config.is_master = 0;
    }
    
    /* 加载 can_node_addr */
    value = ef_get_env("can_node_addr");
    if (value == NULL) {
        /* 未找到，需要创建默认值 */
        ef_set_env("can_node_addr", "30");
        need_save = 1;
    } else {
        g_env_config.can_node_addr = (uint8_t)atoi(value);
    }
    
    /* 加载 print_adc_data */
    value = ef_get_env("print_adc_data");
    if (value == NULL) {
        /* 未找到，需要创建默认值 */
        ef_set_env("print_adc_data", "0");
        need_save = 1;
    } else {
        g_env_config.print_adc_data = (uint8_t)atoi(value);
    }
    
    /* 加载 vs_k */
    value = ef_get_env("vs_k");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.vs_k, ADC_CH_VOLTAGE_COUNT);
    } else {
        need_save = 1;
    }
    
    /* 加载 vs_b */
    value = ef_get_env("vs_b");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.vs_b, ADC_CH_VOLTAGE_COUNT);
    } else {
        need_save = 1;
    }
    
    /* 加载 cur_k */
    value = ef_get_env("cur_k");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.cur_k, ADC_CH_CURRENT_COUNT);
    } else {
        need_save = 1;
    }
    
    /* 加载 cur_b */
    value = ef_get_env("cur_b");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.cur_b, ADC_CH_CURRENT_COUNT);
    } else {
        need_save = 1;
    }
    
    /* 加载 temp_k */
    value = ef_get_env("temp_k");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.temp_k, ADC_CH_TEMP_COUNT);
    } else {
        need_save = 1;
    }
    
    /* 加载 temp_b */
    value = ef_get_env("temp_b");
    if (value != NULL) {
        StringToFloatArray(value, g_env_config.temp_b, ADC_CH_TEMP_COUNT);
    } else {
        need_save = 1;
    }
    
    /* ========== 3. 如果有缺失的 env，保存默认值到 Flash ========== */
    if (need_save) {
        EnvParameter_Save();
    }
}

/**
 * @brief 保存结构体到env
 */
void EnvParameter_Save(void)
{
    char str[ENV_STR_MAX_LEN];
    
    /* 保存 node_role */
    if (g_env_config.is_master) {
        ef_set_env("node_role", "master");
    } else {
        ef_set_env("node_role", "slave");
    }
    
    /* 保存 vs_k */
    FloatArrayToString(g_env_config.vs_k, ADC_CH_VOLTAGE_COUNT, str, sizeof(str));
    ef_set_env("vs_k", str);
    
    /* 保存 vs_b */
    FloatArrayToString(g_env_config.vs_b, ADC_CH_VOLTAGE_COUNT, str, sizeof(str));
    ef_set_env("vs_b", str);
    
    /* 保存 cur_k */
    FloatArrayToString(g_env_config.cur_k, ADC_CH_CURRENT_COUNT, str, sizeof(str));
    ef_set_env("cur_k", str);
    
    /* 保存 cur_b */
    FloatArrayToString(g_env_config.cur_b, ADC_CH_CURRENT_COUNT, str, sizeof(str));
    ef_set_env("cur_b", str);
    
    /* 保存 temp_k */
    FloatArrayToString(g_env_config.temp_k, ADC_CH_TEMP_COUNT, str, sizeof(str));
    ef_set_env("temp_k", str);
    
    /* 保存 temp_b */
    FloatArrayToString(g_env_config.temp_b, ADC_CH_TEMP_COUNT, str, sizeof(str));
    ef_set_env("temp_b", str);
    
    /* 保存 can_node_addr */
    snprintf(str, sizeof(str), "%d", g_env_config.can_node_addr);
    ef_set_env("can_node_addr", str);
    
    /* 保存 print_adc_data */
    snprintf(str, sizeof(str), "%d", g_env_config.print_adc_data);
    ef_set_env("print_adc_data", str);
    
    /* 保存到Flash */
    ef_save_env();
    printf("EnvParameter_Save\n");
}

/**
 * @brief 切换为主机模式并重启
 */
void EnvParameter_SwitchToMaster(void)
{
    g_env_config.is_master = 1;
    EnvParameter_Save();
    
    /* 延时5秒 */
    osDelay(REBOOT_DELAY_MS);
    
    /* 软件复位 */
    NVIC_SystemReset();
}

/**
 * @brief 重置心跳超时计数器
 */
void EnvParameter_ResetHeartbeatTimer(void)
{
    g_heartbeat_timeout = 0;
}

static void EnvParameter_PrintAll(void)
{
    int i;
    printf("-----------------\r\n");
    printf("EnvParameter_Init: node_role = %s\r\n", g_env_config.is_master ? "master" : "slave");
    printf("-----------------\r\n");
    printf("can_node_addr = %d\r\n", g_env_config.can_node_addr);
    printf("-----------------\r\n");
    printf("print_adc_data = %d\r\n", g_env_config.print_adc_data);
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        printf("vs_k[%d] = %f\r\n", i, g_env_config.vs_k[i]);
    }
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_VOLTAGE_COUNT; i++) {
        printf("vs_b[%d] = %f\r\n", i, g_env_config.vs_b[i]);
    }
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        printf("cur_k[%d] = %f\r\n", i, g_env_config.cur_k[i]);
    }
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_CURRENT_COUNT; i++) {
        printf("cur_b[%d] = %f\r\n", i, g_env_config.cur_b[i]);
    }
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        printf("temp_k[%d] = %f\r\n", i, g_env_config.temp_k[i]);
    }
    printf("-----------------\r\n");
    for (i = 0; i < ADC_CH_TEMP_COUNT; i++) {
        printf("temp_b[%d] = %f\r\n", i, g_env_config.temp_b[i]);
    }
    printf("-----------------\r\n");
}

/**
 * @brief Env参数初始化
 */
void EnvParameter_Init(void)
{
    EnvParameter_Load();
    /* 打印所有参数 */
    EnvParameter_PrintAll();
    
    EnvParameter_ResetHeartbeatTimer();
}


