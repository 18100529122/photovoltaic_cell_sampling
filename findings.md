# 光伏电池采样系统 - 发现文档

## 项目结构分析

### 目录结构
```
photovoltaic_cell_sampling/
├── Core/
│   ├── App/              # 应用层
│   │   ├── data_process/ # 数据处理模块
│   │   ├── app.c/h
│   ├── Bsp/              # 板级支持包
│   │   ├── adc/          # ADC驱动
│   │   ├── can/          # CAN驱动
│   │   ├── usart/        # 串口驱动
│   ├── Inc/              # HAL头文件
│   ├── Src/              # HAL源文件
│   ├── Startup/          # 启动文件
│   └── test/             # 测试代码
├── Drivers/              # STM32 HAL驱动
├── Middlewares/          # FreeRTOS中间件
├── doc/                  # 文档
├── README.md
└── photovoltaic_cell_sampling.ioc
```

---

## 硬件配置

### MCU
- **型号**: STM32G474VETx
- **系统时钟**: 170MHz (HSE + PLL)

### ADC配置
- **ADC1**: 7通道 (PA0-3, PC0-2) - 电压/电流采样
- **ADC2**: 7通道 (PA4-7, PC3-5) - 电压/电流采样
- **ADC3**: 5通道 (PB0-1, PE7, PE9, PE13) - 电压采样
- **ADC4**: 7通道 (PE8, PE10-12, PE14, PB14-15) - 电流/温度采样
- **分辨率**: 12位
- **触发源**: TIM2
- **传输方式**: DMA

### 串口配置
- **UART4**: 115200 8N1, PC10(TX)/PC11(RX) - 调试串口（printf重定向）
- **UART5**: 115200 8N1, PC12(TX)/PD2(RX) - 心跳串口

### CAN配置
- **FDCAN2**: CAN 2.0经典帧，500Kbps，PB12(RX)/PB13(TX)
- **波特率参数**:
  - ClockDivider: 1
  - NominalPrescaler: 17
  - NominalSyncJumpWidth: 1
  - NominalTimeSeg1: 16
  - NominalTimeSeg2: 3
- **节点配置**:
  - 节点地址: 30 (0b11110)
  - 帧类型: 01 (广播通信)
  - 总线占用标志: 0 (从节点)
  - A/B总线: 0 (A总线)
- **CAN ID示例**:
  - 首帧: 0x1F1
  - 中间帧: 0x1F2
  - 末帧: 0x1F3

### GPIO
- **LED1**: PD14 - 系统状态指示

---

## 软件架构

### FreeRTOS任务

| 任务名称 | 优先级 | 堆栈 | 功能 | 文件 |
|---------|--------|------|------|------|
| defaultTask | Normal | 128*4 | LED闪烁、BSP初始化、测试运行 | app_freertos.c |
| dataProcessTask | Normal | 512*4 | 数据处理、ADC数据映射 | data_process.c |

### 数据流程

```
模拟信号 → ADC1-4 → DMA → ADC_RawData_t
                          ↓
                   DataProcess_MapRawData()
                          ↓
                   ADC_Data_t (26通道×200点)
                          ↓
                   DataProcess_ProcessAll()
                          ↓
                   排序 → 去极值 → 求平均
                          ↓
                   结果数据 (float类型)
```

### 数据结构

#### ADC_RawData_t
```c
typedef struct {
    uint16_t adc1_data[7];  // ADC1原始数据
    uint16_t adc2_data[7];  // ADC2原始数据
    uint16_t adc3_data[5];  // ADC3原始数据
    uint16_t adc4_data[7];  // ADC4原始数据
} ADC_RawData_t;
```

#### ADC_SampleBuffer_t
```c
typedef struct {
    uint16_t samples[200];  // 200个采样点
    uint16_t write_index;   // 写入索引
    uint8_t  buffer_full;   // 缓冲区满标志
} ADC_SampleBuffer_t;
```

#### ADC_Data_t
```c
typedef struct {
    ADC_SampleBuffer_t vs[12];    // 12路电压
    ADC_SampleBuffer_t cur[12];   // 12路电流
    ADC_SampleBuffer_t temp[2];   // 2路温度
    float vs_result[12];          // 电压平均值
    float cur_result[12];         // 电流平均值
    float temp_result[2];         // 温度平均值
    uint8_t data_ready;           // 数据准备好标志
} ADC_Data_t;
```

---

## CAN通信协议

### 帧格式

#### 首帧 (First Frame)
- **CAN ID**: 0x100 + 节点ID
- **数据**: 
  - Byte 0-1: 总数据长度（大端）
  - Byte 2-7: 前6字节数据

#### 中间帧 (Middle Frame)
- **CAN ID**: 0x200 + 节点ID
- **数据**: 8字节应用数据

#### 末帧 (Last Frame)
- **CAN ID**: 0x300 + 节点ID
- **数据**: 
  - Byte 0-(n-1): 剩余数据
  - Byte n: 校验和

### 应用数据格式
```
Byte 0-1:   设备标识 (0xA5, 0x5A)
Byte 2-49:  12路电压 (float, 小端)
Byte 50-97: 12路电流 (float, 小端)
Byte 98-105:2路温度 (float, 小端)
```

---

## 测试框架

### 测试开关宏
```c
#define TEST_ENABLE              1  // 总开关
#define TEST_ENABLE_UART         1  // UART测试
#define TEST_ENABLE_ADC          1  // ADC测试
#define TEST_ENABLE_DATA_PROCESS 1  // 数据处理测试
#define TEST_ENABLE_CAN          1  // CAN测试
```

### 测试内容
1. **UART测试**: printf功能验证
2. **ADC测试**: 原始数据读取
3. **数据处理测试**: 排序算法、平均值计算
4. **CAN测试**: 回环测试

---

## 已发现的问题/注意事项

1. ✅ **ADC4通道映射问题**: 已修复 - 正确映射为:
   - Rank2-IN14-PE10 -> CUR10
   - Rank3-IN15-PE11 -> VS11
2. **任务数量**: 当前只有2个任务，后续需要添加CAN发送、心跳等任务
3. **UART5未使用**: UART5已配置但未实现心跳功能
4. **Flash存储**: 未实现参数存储功能

---

## CAN配置验证

### ✅ 硬件配置检查结果
| 检查项 | 协议要求 | 实际配置 | 状态 |
|--------|----------|----------|------|
| 波特率 | 500 kbps | 500 kbps | ✅ |
| 时钟分频 | 1 | 1 | ✅ |
| 预分频器 | 17 | 17 | ✅ |
| Sync Jump Width | 1 | 1 | ✅ |
| Time Seg1 | 16 | 16 | ✅ |
| Time Seg2 | 3 | 3 | ✅ |
| 帧格式 | 经典帧 | CLASSIC | ✅ |
| 自动重传 | 使能 | ENABLE | ✅ |

### ✅ 软件实现检查结果
| 检查项 | 协议要求 | 实际实现 | 状态 |
|--------|----------|----------|------|
| CAN ID结构 | 11位标准帧 | 11位标准帧 | ✅ |
| 首帧格式 | 长度(2字节大端)+6字节数据 | 长度(2字节大端)+6字节数据 | ✅ |
| 中间帧格式 | 8字节数据 | 8字节数据 | ✅ |
| 末帧格式 | 剩余数据+SUM校验 | 剩余数据+SUM校验 | ✅ |
| 校验和 | 所有字节累加和 | 所有字节累加和 | ✅ |
| 应用数据格式 | 106字节 | 106字节 | ✅ |

## CAN测试方案

### 测试1: 回环测试 (Loopback Test)
**目的**: 验证CAN硬件和基本驱动功能
**步骤**:
1. 启用内部回环模式
2. 发送测试数据
3. 检查是否能正确接收
4. 验证数据完整性

### 测试2: 单帧发送测试
**目的**: 验证单帧CAN ID和数据格式
**步骤**:
1. 发送首帧 (ID: 0x1F1)
2. 验证ID格式和数据内容
3. 检查数据长度字段 (0x00, 0x6A = 106)

### 测试3: 复帧传输测试
**目的**: 验证完整14帧传输流程
**步骤**:
1. 发送完整106字节数据
2. 验证14帧的顺序
3. 检查末帧的SUM校验

### 测试4: 实际数据传输测试
**目的**: 验证真实采样数据传输
**步骤**:
1. 等待ADC数据处理完成
2. 通过CAN发送真实数据
3. 使用CAN分析仪监控
4. 验证数据格式和内容

## 技术亮点

1. **多ADC协同**: 4个ADC并行采集，提高采样效率
2. **DMA传输**: 减少CPU占用
3. **数据滤波**: 去极值平均算法，提高数据可靠性
4. **模块化设计**: BSP/App分离，便于维护
5. **测试框架**: 宏定义开关，灵活控制测试
6. **CAN协议实现**: 完整符合协议规范的复帧传输
