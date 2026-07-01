# Photovoltaic Cell Sampling

基于 STM32G474VETx 的光伏电池采样系统固件。

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
- UART4: 115200 8N1, PC10(TX)/PC11(RX)
- UART5: 115200 8N1, PC12(TX)/PD2(RX)

### GPIO
- LED1: PD14 (高电平亮)

## 项目结构

```
photovoltaic_cell_sampling/
├── Core/
│   ├── Inc/       # 头文件
│   ├── Src/       # 源文件
│   └── Startup/   # 启动文件
├── Drivers/       # STM32 HAL 驱动
└── doc/           # 文档目录
```

## 编译说明

本项目使用 STM32CubeMX 生成，支持以下开发环境：
- STM32CubeIDE
- Keil MDK-ARM
- IAR EWARM

## 注意事项

当前项目为框架代码，业务逻辑待实现。
