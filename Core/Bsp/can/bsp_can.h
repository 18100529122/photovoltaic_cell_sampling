#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "fdcan.h"
#include "data_structure.h"

/* 节点配置 */
#define CAN_NODE_ADDR           30          /* 节点地址：30 */
#define CAN_BUS_FLAG            0           /* 总线占用标志：0=从节点 */
#define CAN_FRAME_TYPE          1           /* 帧类型：01=广播通信 */
#define CAN_AB_BUS_FLAG         0           /* A/B总线：0=A总线 */

/* ID 构建宏 */
#define CAN_BUILD_ID(single_multi) \
    (((CAN_BUS_FLAG) << 10) | \
     ((CAN_FRAME_TYPE) << 8) | \
     ((CAN_NODE_ADDR) << 3) | \
     ((CAN_AB_BUS_FLAG) << 2) | \
     (single_multi))

/* 单复帧定义 */
#define CAN_SINGLE_FRAME        0x00
#define CAN_FIRST_FRAME         0x01
#define CAN_MIDDLE_FRAME        0x02
#define CAN_LAST_FRAME          0x03

/* 应用数据长度 */
#define CAN_APP_DATA_LEN        106

/* CAN 状态枚举 */
typedef enum {
    CAN_OK = 0,
    CAN_ERROR,
    CAN_BUSY,
    CAN_TIMEOUT
} CAN_Status_t;

/* 函数声明 */
void BSP_CAN_Init(void);
CAN_Status_t BSP_CAN_SendData(const ResultData_t *result);
CAN_Status_t BSP_CAN_SendRawData(const uint8_t *data, uint16_t len);
void BSP_CAN_EnableLoopback(void);
void BSP_CAN_DisableLoopback(void);
void BSP_CAN_ConfigFilter(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAN_H */
