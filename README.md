# Photovoltaic Cell Sampling

基于 STM32G474VETx + FreeRTOS 的光伏电池采样系统固件。

## 硬件配置

- **MCU**: STM32G474VETx
- **系统时钟**: 170MHz (HSE + PLL)

## 外设配置

### ADC (4个独立ADC，26通道)
- ADC1-4 均配置为 12位分辨率
- **ADC1**: 7通道 (PA0-3, PC0-2) - 电压采样
- **ADC2**: 7通道 (PA4-7, PC3-5) - 电压/电流采样
- **ADC3**: 5通道 (PB0-1, PE7, PE9, PE13) - 电流采样
- **ADC4**: 7通道 (PE8, PE10-12, PE14, PB14-15) - 电流/温度采样
- 总计26个模拟输入通道用于光伏信号采集

### FDCAN2
- CAN 2.0 经典帧格式
- 波特率 500Kbps
- PB12 (RX), PB13 (TX)

### UART
- **UART4**: 115200 8N1, PC10(TX)/PC11(RX) - 调试串口（printf重定向）
- **UART5**: 115200 8N1, PC12(TX)/PD2(RX) - 心跳串口

### GPIO
- LED1: PD14 (高电平亮)

## 软件架构

### FreeRTOS 任务

| 任务名称 | 优先级 | 堆栈大小 | 功能描述 |
|---------|--------|---------|---------|
| defaultTask | Normal | 512 | LED闪烁 |
| dataProcessTask | Normal | 2048 | 数据处理 |

### 项目结构

```
photovoltaic_cell_sampling/
├── Core/
│   ├── Inc/       # 头文件
│   ├── Src/       # 源文件
│   ├── Bsp/       # 板级支持包
│   │   ├── bsp.c/h
│   │   ├── adc/   # ADC驱动
│   │   └── usart/ # 串口驱动
│   └── App/       # 应用层
│       ├── app.c/h
│       └── data_process/ # 数据处理模块
├── Drivers/       # STM32 HAL 驱动
└── doc/           # 文档目录
```

## 功能说明

### 已实现
1. **数据结构定义** - 26通道数据结构，包含12路电压、12路电流、2路温度
2. **ADC+DMA驱动** - 4个ADC通过DMA采集数据
3. **数据处理算法** - 每通道200点采集，去掉10个最大/最小值，计算平均值
4. **printf重定向** - 调试输出到UART4
5. **FreeRTOS多任务** - LED闪烁和数据处理独立运行

### 待实现
1. CAN通信 - 发送26路处理后的数据
2. 心跳包 - UART5定时发送
3. Flash参数存储

## 编译说明

本项目使用 STM32CubeMX 生成，支持以下开发环境：
- STM32CubeIDE
- Keil MDK-ARM
- IAR EWARM
