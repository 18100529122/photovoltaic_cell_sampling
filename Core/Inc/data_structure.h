#ifndef __DATA_STRUCTURE_H__
#define __DATA_STRUCTURE_H__
/**
 * @file     data_structure.h
 * @brief    数据结构定义
 */
#ifdef __cplusplus
extern "C" {
#endif

/*========================= 头文件包含 (Includes) ==========================*/
#include <stdint.h>

/*========================= 宏定义 (Macros) ================================*/
/* ADC通道数量定义 */
#define ADC_CH_VOLTAGE_COUNT       12      /* 电压通道数量: VS1~VS12 */
#define ADC_CH_CURRENT_COUNT       12      /* 电流通道数量: CUR1~CUR12 */
#define ADC_CH_TEMP_COUNT          2       /* 温度通道数量: TEMP1~TEMP2 */
#define ADC_CH_TOTAL_COUNT         (ADC_CH_VOLTAGE_COUNT + ADC_CH_CURRENT_COUNT + ADC_CH_TEMP_COUNT)

/* 每个ADC的通道数 */
#define ADC1_CH_COUNT              7
#define ADC2_CH_COUNT              7
#define ADC3_CH_COUNT              5
#define ADC4_CH_COUNT              7

/* 采样参数 */
#define SAMPLES_PER_CHANNEL        200     /* 每个通道采样点数 */
#define REMOVE_MAX_COUNT           10      /* 去掉的最大值个数 */
#define REMOVE_MIN_COUNT           10      /* 去掉的最小值个数 */
#define PROCESS_SAMPLES_COUNT      (SAMPLES_PER_CHANNEL - REMOVE_MAX_COUNT - REMOVE_MIN_COUNT)  /* 实际计算平均值的点数 */

/*========================= 类型定义 (Typedefs) ============================*/
/* ADC原始数据结构体 - DMA缓冲区 */
typedef struct {
    uint16_t adc1_data[ADC1_CH_COUNT];    /* ADC1数据 */
    uint16_t adc2_data[ADC2_CH_COUNT];    /* ADC2数据 */
    uint16_t adc3_data[ADC3_CH_COUNT];    /* ADC3数据 */
    uint16_t adc4_data[ADC4_CH_COUNT];    /* ADC4数据 */
} ADC_RawData_t;

/* 采样点缓冲区结构体 */
typedef struct {
    uint16_t samples[SAMPLES_PER_CHANNEL];  /* 采样点存储 */
    uint16_t write_index;                   /* 写入索引 */
    uint8_t  buffer_full;                   /* 缓冲区是否已满 */
} ADC_SampleBuffer_t;

/* ADC通道数据结构体 */
typedef struct {
    /* 原始采样点缓冲区 */
    ADC_SampleBuffer_t vs[ADC_CH_VOLTAGE_COUNT];      /* VS1~VS12 */
    ADC_SampleBuffer_t cur[ADC_CH_CURRENT_COUNT];    /* CUR1~CUR12 */
    ADC_SampleBuffer_t temp[ADC_CH_TEMP_COUNT];      /* TEMP1~TEMP2 */

    /* 处理后的结果 */
    float vs_result[ADC_CH_VOLTAGE_COUNT];           /* 电压平均值 */
    float cur_result[ADC_CH_CURRENT_COUNT];         /* 电流平均值 */
    float temp_result[ADC_CH_TEMP_COUNT];           /* 温度平均值 */

    /* 状态标志 */
    uint8_t data_ready;                             /* 1s数据准备好标志 */
} ADC_Data_t;

/* CAN发送结果数据结构体 */
typedef struct {
    float voltage[ADC_CH_VOLTAGE_COUNT];            /* 12路电压数据 */
    float current[ADC_CH_CURRENT_COUNT];            /* 12路电流数据 */
    float temperature[ADC_CH_TEMP_COUNT];           /* 2路温度数据 */
} ResultData_t;

/*========================= 全局变量 (Global Variables) ====================*/

/*========================= 函数声明 (Function Declarations) ===============*/

#ifdef __cplusplus
}
#endif
#endif /* __DATA_STRUCTURE_H__ */