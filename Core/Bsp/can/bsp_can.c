#include "bsp_can.h"
#include <string.h>

/* 本地函数声明 */
static CAN_Status_t SendFrame(uint32_t id, const uint8_t *data, uint8_t len);
static uint8_t CalculateChecksum(const uint8_t *data, uint16_t len);
static void PackResultData(const ResultData_t *result, uint8_t *buffer);

/**
 * @brief  CAN 初始化
 * @param  None
 * @retval None
 */
void BSP_CAN_Init(void)
{
    /* 配置过滤器 */
    BSP_CAN_ConfigFilter();
    
    /* 启动 FDCAN */
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  配置 CAN 过滤器
 * @param  None
 * @retval None
 */
void BSP_CAN_ConfigFilter(void)
{
    FDCAN_FilterTypeDef sFilterConfig;
    
    /* 配置标准帧过滤器，接收所有标准帧 */
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0x000;
    sFilterConfig.FilterID2 = 0x7FF;
    
    if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK) {
        Error_Handler();
    }
    
    /* 配置全局过滤器 */
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, 
                                     FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  启用内部回环模式
 * @param  None
 * @retval None
 */
void BSP_CAN_EnableLoopback(void)
{
    /* 停止 FDCAN */
    if (HAL_FDCAN_Stop(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
    
    /* 重新初始化 FDCAN 为回环模式 */
    hfdcan2.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
    if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
    
    /* 重新配置过滤器 */
    BSP_CAN_ConfigFilter();
    
    /* 重新启动 FDCAN */
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  禁用内部回环模式，恢复正常模式
 * @param  None
 * @retval None
 */
void BSP_CAN_DisableLoopback(void)
{
    /* 停止 FDCAN */
    if (HAL_FDCAN_Stop(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
    
    /* 重新初始化 FDCAN 为正常模式 */
    hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
    if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
    
    /* 重新配置过滤器 */
    BSP_CAN_ConfigFilter();
    
    /* 重新启动 FDCAN */
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  发送结果数据
 * @param  result: 结果数据指针
 * @retval CAN_Status_t
 */
CAN_Status_t BSP_CAN_SendData(const ResultData_t *result)
{
    uint8_t appData[CAN_APP_DATA_LEN];
    
    /* 打包数据 */
    PackResultData(result, appData);
    
    /* 发送原始数据 */
    return BSP_CAN_SendRawData(appData, CAN_APP_DATA_LEN);
}

/**
 * @brief  发送原始数据（复帧）
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval CAN_Status_t
 */
CAN_Status_t BSP_CAN_SendRawData(const uint8_t *data, uint16_t len)
{
    uint8_t frameData[8];
    uint16_t offset = 0;
    uint8_t checksum;
    CAN_Status_t status;
    
    /* 首帧 */
    frameData[0] = (len >> 8) & 0xFF;  /* 长度高字节 */
    frameData[1] = len & 0xFF;         /* 长度低字节 */
    memcpy(&frameData[2], &data[offset], 6);
    offset += 6;
    
    status = SendFrame(CAN_BUILD_ID(CAN_FIRST_FRAME), frameData, 8);
    if (status != CAN_OK) return status;
    
    /* 中间帧（每帧8字节） */
    while (offset + 8 < len) {
        memcpy(frameData, &data[offset], 8);
        offset += 8;
        
        status = SendFrame(CAN_BUILD_ID(CAN_MIDDLE_FRAME), frameData, 8);
        if (status != CAN_OK) return status;
    }
    
    /* 末帧 */
    uint8_t remaining = len - offset;
    if (remaining > 0) {
        memcpy(frameData, &data[offset], remaining);
    }
    
    /* 计算校验和 */
    checksum = CalculateChecksum(data, len);
    frameData[remaining] = checksum;
    
    status = SendFrame(CAN_BUILD_ID(CAN_LAST_FRAME), frameData, remaining + 1);
    return status;
}

/**
 * @brief  发送单帧
 * @param  id: CAN ID
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval CAN_Status_t
 */
static CAN_Status_t SendFrame(uint32_t id, const uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    
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
    uint32_t timeout = 0xFFFF;
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0) {
        if (--timeout == 0) {
            return CAN_TIMEOUT;
        }
    }
    
    /* 发送数据 */
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, (uint8_t*)data) != HAL_OK) {
        return CAN_ERROR;
    }
    
    return CAN_OK;
}

/**
 * @brief  计算校验和
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval uint8_t: 校验和
 */
static uint8_t CalculateChecksum(const uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

/**
 * @brief  打包结果数据
 * @param  result: 结果数据指针
 * @param  buffer: 输出缓冲区
 * @retval None
 */
static void PackResultData(const ResultData_t *result, uint8_t *buffer)
{
    /* 设备标识 */
    buffer[0] = 0xA5;
    buffer[1] = 0x5A;
    
    /* 电压数据（12路float，小端） */
    memcpy(&buffer[2], result->voltage, 12 * sizeof(float));
    
    /* 电流数据（12路float，小端） */
    memcpy(&buffer[50], result->current, 12 * sizeof(float));
    
    /* 温度数据（2路float，小端） */
    memcpy(&buffer[98], result->temperature, 2 * sizeof(float));
}
