#ifndef __ENV_PARAMETER_H__
#define __ENV_PARAMETER_H__
/**
 * @file     env_parameter.h
 * @brief    env参数整合头文件
 */

#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include "main.h"
#include "data_structure.h"

/*========================= 宏定义 (Macros) ================================*/
#define HEARTBEAT_TIMEOUT_SEC     60      /* 心跳超时时间（秒） */
#define REBOOT_DELAY_MS           5000    /* 重启前延时（毫秒） */

/*========================= 类型定义 (Typedefs) ============================*/

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/
void EnvParameter_Init(void);
uint8_t Calculate_CRC8(const uint8_t *data, uint16_t len);
void EnvParameter_Load(void);
void EnvParameter_Save(void);
void EnvParameter_SwitchToMaster(void);
void EnvParameter_SwitchToSlave(void);
void EnvParameter_ResetHeartbeatTimer(void);
void FloatArrayToString(const float *array, uint16_t count, char *str, uint16_t str_len);
void StringToFloatArray(const char *str, float *array, uint16_t count);


#ifdef __cplusplus
}
#endif

#endif /* __ENV_PARAMETER_H__ */
