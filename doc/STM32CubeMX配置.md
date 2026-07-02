# STM32CubeMX 详细配置指南

本文档说明如何在STM32CubeMX中配置光伏电池采样项目所需的所有外设。

---

## 目录
1. [芯片选择与工程创建](#1-芯片选择与工程创建)
2. [时钟配置](#2-时钟配置)
3. [FreeRTOS配置](#3-freertos配置)
4. [ADC配置](#4-adc配置)
5. [DMA配置](#5-dma配置)
6. [定时器配置](#6-定时器配置)
7. [FDCAN2配置](#7-fdcan2配置)
8. [UART配置](#8-uart配置)
9. [GPIO配置](#9-gpio配置)
10. [NVIC配置](#10-nvic配置)
11. [项目生成](#11-项目生成)

---

## 配置方案说明

本项目采用**方案3：定时采集（精确1秒）**

设计思路：
- 用 TIM2 提供 200Hz 的 ADC 触发（每个通道1秒采200点）
- 用 TIM3 提供 1Hz 的定时信号，用于启动/停止采集周期
- DMA 配置为 Normal 模式，一次采集 200 个样本后产生中断
- 在中断中停止 ADC，设置标志位，等待下一次 TIM3 触发

---

## 1. 芯片选择与工程创建

1. 打开STM32CubeMX
2. 点击 **File** → **New Project**
3. 在 **Part Number** 中搜索 `STM32G474VETx`
4. 选择芯片并点击 **Start Project**
5. 在 **Project Manager** → **Project** 中设置：
   - Project Name: `photovoltaic_cell_sampling`
   - Toolchain/IDE: 根据你的开发环境选择（STM32CubeIDE/KEIL/IAR）
6. 在 **Code Generator** 中勾选：
   - ✅ Generate peripheral initialization as a pair of '.c/.h' files per peripheral

---

## 2. 时钟配置

进入 **Clock Configuration** 标签页：

### 2.1 HSE配置
- HSE Source: **Crystal/Ceramic Resonator** (外部晶振)

### 2.2 系统时钟配置
按照以下路径配置：
```
HSE (假设8MHz) → /M (2) → PLL Source Mux
→ PLL N (85) → PLL P (2) → SYSCLK (170MHz)
```

具体参数：
- PLL Source: **HSE**
- PLLM: **2**
- PLLN: **85**
- PLLP: **2**
- System Clock Mux: **PLLCLK**
- AHB Prescaler: **1** (HCLK = 170MHz)
- APB1 Prescaler: **1** (PCLK1 = 170MHz)
- APB2 Prescaler: **1** (PCLK2 = 170MHz)

### 2.3 外设时钟
- ADC12 Clock Mux: **SYSCLK**
- ADC345 Clock Mux: **SYSCLK**
- FDCAN Clock Mux: **PCLK1**
- UART4/5 Clock Mux: **PCLK1**

---

## 3. FreeRTOS配置

进入 **Middleware and Software Packs** → **FREERTOS**:

### 3.1 FreeRTOS 基础配置

- **Interface**: **CMSIS-RTOS v2**
- **FreeRTOS Version**: 选择最新版本

### 3.2 FreeRTOS 详细参数

在 **Configuration** 标签页下设置：

#### 系统配置 (System)
- **USE_PREEMPTION**: **Enabled** (抢占式调度)
- **TICK_RATE_HZ**: **1000** (1ms系统滴答)
- **MAX_PRIORITIES**: **8**
- **MINIMAL_STACK_SIZE**: **128** (words)
- **MAX_TASK_NAME_LEN**: **16**
- **USE_16_BIT_TICKS**: **Disabled**
- **IDLE_SHOULD_YIELD**: **Enabled**

#### 内存管理 (Memory Management)
- **TOTAL_HEAP_SIZE**: **32768** (32KB，增加了空间)
- **MEMORY_ALLOCATION_SCHEME**: **heap_4** (推荐)

#### 队列与信号量 (Queues and Semaphores)
- **USE_MUTEXES**: **Enabled**
- **USE_RECURSIVE_MUTEXES**: **Enabled**
- **USE_COUNTING_SEMAPHORES**: **Enabled**
- **QUEUE_REGISTRY_SIZE**: **8**

#### 软件定时器 (Software Timers)
- **USE_TIMERS**: **Enabled**
- **TIMER_TASK_PRIORITY**: **2**
- **TIMER_QUEUE_LENGTH**: **5**
- **TIMER_TASK_STACK_DEPTH**: **256**

### 3.3 HAL 延时函数配置

在 **Advanced Settings** 中：
- **vTaskDelay**: **Enabled**

---

## 4. ADC配置

### 4.1 ADC1 配置

进入 **Analog** → **ADC1**:

#### ADC1 参数设置
- Clock Prescaler: **Synchronous clock mode divided by 4**
- Resolution: **12 bits**
- Data Alignment: **Right alignment**
- Scan Conversion Mode: **Enabled**
- Continuous Conversion Mode: **Disabled**
- Discontinuous Conversion Mode: **Disabled**
- Number of Conversions: **7**
- External Trigger Conversion Source: **Timer 2 Trigger Out event** (用于精确触发)
- External Trigger Conversion Edge: **Trigger detection on the rising edge**
- DMA Continuous Requests: **Disabled** (关键：因为用Normal模式)
- End of Conversion Selection: **EOC single conversion**
- Overrun behavior: **Overrun data preserved**
- Sampling Time: **6.5 Cycles** (可根据需要调整)

#### ADC1 通道配置 (Rank顺序)
| Rank | Channel | Pin |
|------|---------|-----|
| Rank 1 | IN1 | PA0 |
| Rank 2 | IN2 | PA1 |
| Rank 3 | IN3 | PA2 |
| Rank 4 | IN4 | PA3 |
| Rank 5 | IN6 | PC0 |
| Rank 6 | IN7 | PC1 |
| Rank 7 | IN8 | PC2 |

---

### 4.2 ADC2 配置

进入 **Analog** → **ADC2**:

#### ADC2 参数设置
与ADC1相同：
- Clock Prescaler: **Synchronous clock mode divided by 4**
- Resolution: **12 bits**
- Data Alignment: **Right alignment**
- Scan Conversion Mode: **Enabled**
- Continuous Conversion Mode: **Disabled**
- Discontinuous Conversion Mode: **Disabled**
- Number of Conversions: **7**
- External Trigger Conversion Source: **Timer 2 Trigger Out event**
- External Trigger Conversion Edge: **Trigger detection on the rising edge**
- DMA Continuous Requests: **Disabled** (关键：因为用Normal模式)
- End of Conversion Selection: **EOC single conversion**
- Overrun behavior: **Overrun data preserved**
- Sampling Time: **6.5 Cycles**

#### ADC2 通道配置
| Rank | Channel | Pin |
|------|---------|-----|
| Rank 1 | IN17 | PA4 |
| Rank 2 | IN13 | PA5 |
| Rank 3 | IN3 | PA6 |
| Rank 4 | IN4 | PA7 |
| Rank 5 | IN9 | PC3 |
| Rank 6 | IN5 | PC4 |
| Rank 7 | IN11 | PC5 |

---

### 4.3 ADC3 配置

进入 **Analog** → **ADC3**:

#### ADC3 参数设置
与ADC1相同：
- Clock Prescaler: **Synchronous clock mode divided by 4**
- Resolution: **12 bits**
- Data Alignment: **Right alignment**
- Scan Conversion Mode: **Enabled**
- Continuous Conversion Mode: **Disabled**
- Discontinuous Conversion Mode: **Disabled**
- Number of Conversions: **5**
- External Trigger Conversion Source: **Timer 2 Trigger Out event**
- External Trigger Conversion Edge: **Trigger detection on the rising edge**
- DMA Continuous Requests: **Disabled** (关键：因为用Normal模式)
- End of Conversion Selection: **EOC single conversion**
- Overrun behavior: **Overrun data preserved**
- Sampling Time: **6.5 Cycles**

#### ADC3 通道配置
| Rank | Channel | Pin |
|------|---------|-----|
| Rank 1 | IN12 | PB0 |
| Rank 2 | IN1 | PB1 |
| Rank 3 | IN4 | PE7 |
| Rank 4 | IN2 | PE9 |
| Rank 5 | IN3 | PE13 |

---

### 4.4 ADC4 配置

进入 **Analog** → **ADC4**:

#### ADC4 参数设置
与ADC1相同：
- Clock Prescaler: **Synchronous clock mode divided by 4**
- Resolution: **12 bits**
- Data Alignment: **Right alignment**
- Scan Conversion Mode: **Enabled**
- Continuous Conversion Mode: **Disabled**
- Discontinuous Conversion Mode: **Disabled**
- Number of Conversions: **7**
- External Trigger Conversion Source: **Timer 2 Trigger Out event**
- External Trigger Conversion Edge: **Trigger detection on the rising edge**
- DMA Continuous Requests: **Disabled** (关键：因为用Normal模式)
- End of Conversion Selection: **EOC single conversion**
- Overrun behavior: **Overrun data preserved**
- Sampling Time: **6.5 Cycles**

#### ADC4 通道配置
| Rank | Channel | Pin |
|------|---------|-----|
| Rank 1 | IN6 | PE8 |
| Rank 2 | IN14 | PE10 |
| Rank 3 | IN15 | PE11 |
| Rank 4 | IN16 | PE12 |
| Rank 5 | IN1 | PE14 |
| Rank 6 | IN4 | PB14 |
| Rank 7 | IN5 | PB15 |

---

## 5. DMA配置

进入 **System Core** → **DMA**:

需要为每个ADC添加DMA通道。

### 5.1 DMA1 配置 (ADC1-2)

点击 **Add** 按钮添加：

| DMA Request | Stream | Direction | Mode | Priority |
|------------|--------|-----------|------|----------|
| ADC1 | DMA1_Channel1 | Peripheral To Memory | Normal | High |
| ADC2 | DMA1_Channel2 | Peripheral To Memory | Normal | High |

配置每个DMA通道：
- Data Width: **Half Word (16-bit)** (Peripheral & Memory)
- Increment Memory: **Enabled**
- Increment Peripheral: **Disabled**

**关键说明**：
- Mode = **Normal**：一次传输完成后自动停止
- 缓冲区大小要在代码中设置为：通道数 × 200 个样本

### 5.2 DMA2 配置 (ADC3-4)

点击 **Add** 按钮添加：

| DMA Request | Stream | Direction | Mode | Priority |
|------------|--------|-----------|------|----------|
| ADC3 | DMA2_Channel1 | Peripheral To Memory | Normal | High |
| ADC4 | DMA2_Channel2 | Peripheral To Memory | Normal | High |

配置每个DMA通道：
- Data Width: **Half Word (16-bit)** (Peripheral & Memory)
- Increment Memory: **Enabled**
- Increment Peripheral: **Disabled**

---

## 6. 定时器配置 (用于触发采样)

### 6.1 TIM2 配置 (ADC采样触发 - 200Hz)

进入 **Timers** → **TIM2**:

#### TIM2 参数设置
- Prescaler (PSC): **0**
- Counter Period (ARR): **849** (170MHz / 850 = 200kHz → 提供200Hz采样率)
- Trigger Event Selection: **Update Event**

#### 定时器输出触发
- Master Mode: **Update Event** (用于触发ADC)

---

### 6.2 TIM3 配置 (1秒周期定时)

进入 **Timers** → **TIM3**:

#### TIM3 参数设置
- Prescaler (PSC): **16999** (170MHz / 17000 = 10kHz)
- Counter Period (ARR): **9999** (10kHz / 10000 = 1Hz)

#### TIM3 中断配置
在 **NVIC Settings** 中启用：
- ✅ TIM3 global interrupt
- Priority: **6** (必须大于FreeRTOS的MAX_SYSCALL_INTERRUPT_PRIORITY)

---

## 7. FDCAN2配置

进入 **Connectivity** → **FDCAN2**:

### 7.1 FDCAN2 基础配置
- Frame Format: **Classic Mode**
- Master Mode: **Normal Mode**
- Prescaler (for Time Quantum): **17**
- Time Quanta:
  - Nominal Time Quanta:
    - Time Segment 1: **16**
    - Time Segment 2: **3**
    - Re-Sync Jump Width: **1**
- FDCAN Mode: **Disabled**
- Auto Retransmission: **Enabled**
- Protocol Exception: **Disabled**

### 7.2 FDCAN2 GPIO配置
- FDCAN2_RX: **PB12**
- FDCAN2_TX: **PB13**

### 7.3 FDCAN2 中断配置
在 **NVIC Settings** 中启用：
- ✅ FDCAN2 interrupt 1
- Priority: 根据需要设置

---

## 8. UART配置

### 8.1 UART4 配置 (调试串口)

进入 **Connectivity** → **UART4**:

#### UART4 参数
- Baud Rate: **115200**
- Word Length: **8 Bits**
- Parity: **None**
- Stop Bits: **1**
- Data Direction: **Receive and Transmit**

#### UART4 GPIO配置
- UART4_TX: **PC10**
- UART4_RX: **PC11**

---

### 8.2 UART5 配置 (心跳串口)

进入 **Connectivity** → **UART5**:

#### UART5 参数
- Baud Rate: **115200**
- Word Length: **8 Bits**
- Parity: **None**
- Stop Bits: **1**
- Data Direction: **Receive and Transmit**

#### UART5 GPIO配置
- UART5_TX: **PC12**
- UART5_RX: **PD2**

---

## 9. GPIO配置

### 9.1 LED指示
进入 **System Core** → **GPIO**:

- PD14:
  - GPIO mode: **Output Push Pull**
  - GPIO Pull-up/Pull-down: **No pull-up and no pull-down**
  - Maximum output speed: **Low**
  - User Label: **LED1**

### 9.2 ADC引脚验证
确认所有ADC输入引脚已自动配置为 **Analog Mode**:
- PA0-7, PC0-5, PB0-1, PB14-15, PE7-14

---

## 10. NVIC配置

进入 **System Core** → **NVIC**:

启用以下中断：
- ✅ FDCAN2 interrupt 1
- ✅ DMA1 channel1 global interrupt
- ✅ DMA1 channel2 global interrupt
- ✅ DMA2 channel1 global interrupt
- ✅ DMA2 channel2 global interrupt
- ✅ TIM3 global interrupt (新增)
- (可选) UART4/5 global interrupt (如需使用中断方式收发)

**中断优先级设置**：
| 中断 | 优先级 | 说明 |
|-----|--------|------|
| DMA1 channel1 | 6 | 必须大于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5 |
| DMA1 channel2 | 6 | 必须大于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5 |
| DMA2 channel1 | 6 | 必须大于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5 |
| DMA2 channel2 | 6 | 必须大于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5 |
| TIM3 global interrupt | 6 | 必须大于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5 |
| FDCAN2 interrupt 1 | 7 | 根据需要调整 |

**重要说明**:
- configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5
- 所有调用FreeRTOS API的中断，优先级**必须** > 5（数值越大，优先级越低）
- 数值6、7都是安全的

---

## 11. 项目生成

1. 保存项目 (Ctrl+S)
2. 点击 **GENERATE CODE** 按钮生成代码
3. 等待代码生成完成

---

## 配置验证检查清单

生成代码后，请检查：

- [ ] FreeRTOS已启用 (CMSIS-RTOS v2)
- [ ] 时钟配置达到170MHz
- [ ] 4个ADC的通道数正确 (7+7+5+7=26)
- [ ] **DMA配置为Normal模式**（关键！不是Circular）
- [ ] **DMA Continuous Requests = Disabled**
- [ ] TIM2配置为200Hz触发 (ARR=849)
- [ ] **TIM3已配置为1Hz定时**
- [ ] TIM3中断已启用
- [ ] FDCAN2、UART4/5参数正确
- [ ] LED引脚PD14已配置
- [ ] FreeRTOS堆大小设置为32KB (heap_4)
- [ ] **所有DMA和TIM3中断优先级 = 6**

---

## 下一步

配置完成后，参考 [项目计划.md](项目计划.md) 开始固件开发！
