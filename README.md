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
- 通过 DMA 自动采集，每通道采样 200 点

### FDCAN2
- CAN 2.0 经典帧格式
- 波特率 500Kbps
- PB12 (RX), PB13 (TX)
- 节点地址: 可配置（默认 30），通过 `env_set_can_addr` 命令修改

### UART
- **UART4**: 115200 8N1, PC10(TX)/PC11(RX) - 调试串口（printf重定向 + Letter Shell）
- **UART5**: 115200 8N1, PC12(TX)/PD2(RX) - 心跳串口

### TIM
- **TIM1**: FreeRTOS 系统时钟
- **TIM2**: 基础定时器（预留）
- **TIM3**: 1秒定时器，触发ADC采集

### IWDG（独立看门狗）
- 独立看门狗，时钟源为内部 LSI（32kHz）
- 预分频系数: 256
- 重装载值: 4095
- 超时时间: 约 32.768 秒
- 在 defaultTask 主循环中每 1 秒喂狗一次

### GPIO
- LED1: PD14 (高电平亮)

## 软件架构

### FreeRTOS 任务

| 任务名称 | 优先级 | 堆栈大小 | 功能描述 |
|---------|--------|---------|---------|
| defaultTask | Normal | 512 | LED闪烁、BSP初始化、应用层初始化、主循环 |
| shell_task | tskIDLE_PRIORITY+1 | 1024 | Letter Shell 命令行交互 |
| adc_process | tskIDLE_PRIORITY+2 | 4096 | ADC数据处理、k/b校准、CAN发送 |

### 软件分层架构

```
┌─────────────────────────────────────┐
│   App 层 (应用层)                    │
│   ├── app.c/h                        │
│   └── data_process/ (数据处理模块)   │
├─────────────────────────────────────┤
│   Middleware 层 (中间件层)           │
│   ├── easyflash/ (Flash存储)         │
│   └── letter_shell/ (命令行Shell)   │
├─────────────────────────────────────┤
│   BSP 层 (板级支持包)                │
│   ├── bsp_adc.c/h (ADC驱动)         │
│   ├── bsp_can.c/h (CAN驱动)         │
│   ├── bsp_flash.c/h (Flash驱动)     │
│   ├── bsp_time.c/h (定时器驱动)     │
│   └── bsp_usart.c/h (串口驱动)      │
├─────────────────────────────────────┤
│   STM32 HAL 驱动层                   │
└─────────────────────────────────────┘
```

### 项目结构

```
photovoltaic_cell_sampling/
├── Core/
│   ├── Inc/               # 头文件
│   ├── Src/               # 源文件
│   ├── Bsp/               # 板级支持包
│   │   ├── bsp.c/h
│   │   ├── adc/           # ADC驱动
│   │   ├── can/           # CAN驱动
│   │   ├── flash/         # Flash存储驱动
│   │   ├── time/          # 定时器驱动
│   │   └── usart/         # 串口驱动
│   ├── App/               # 应用层
│   │   ├── app.c/h
│   │   └── data_process/  # 数据处理模块
│   ├── Middleware/        # 中间件
│   │   ├── easyflash/     # EasyFlash 嵌入式 Flash 存储库
│   │   └── letter_shell/  # Letter Shell 命令行组件
│   ├── Startup/           # 启动文件
│   └── test/              # 测试代码
│       ├── test_bsp.c/h
├── Drivers/               # STM32 HAL 驱动
├── Middlewares/           # 中间件（FreeRTOS）
├── doc/                   # 文档目录
│   ├── can协议/           # CAN协议文档
│   └── ai对话/            # AI对话记录
├── .mxproject             # STM32CubeMX 项目文件
├── .ioc                   # STM32CubeMX 配置文件
└── README.md              # 本文档
```

## 功能说明

### 已实现
1. **数据结构定义** - 26通道数据结构，包含12路电压、12路电流、2路温度
2. **ADC+DMA驱动** - 4个ADC通过DMA采集数据，每通道200点采样，TIM3每秒触发一次采集
3. **数据处理算法** - 每通道200点采集，去掉10个最大/最小值，计算剩余180点平均值
4. **printf重定向** - 调试输出到UART4
5. **FreeRTOS多任务** - LED闪烁任务 + Shell命令行任务 + ADC数据处理任务
6. **CAN通信驱动** - 完整的复帧传输协议（首帧+中间帧+末帧），支持106字节数据传输，回环测试验证通过 ✅
7. **Letter Shell 移植** - 命令行 shell 移植完成，支持命令交互 ✅
8. **Flash存储驱动** - 支持16KB数据区（0x0807C000）读写擦除，双Bank正确处理 ✅
9. **EasyFlash 移植** - 轻量级 Flash 环境变量库移植完成，支持 26 通道 kb 值的存储 ✅
10. **BSP测试套件** - ADC测试、数据处理测试、UART测试、CAN回环测试、Flash读写测试、EasyFlash测试 ✅
11. **主从机模式** - 支持主机/从机模式，主机每10秒发送心跳包，从机60秒无心跳自动变主机 ✅
12. **完整 k/b 值管理** - 支持通过 Shell 命令设置任意通道的 k 和 b 值 ✅
13. **k/b 校准应用** - ADC数据处理后自动应用校准公式（真实值 = ADC值 × k + b） ✅
14. **自动 CAN 发送** - 主机模式下，ADC数据处理完成后自动通过 CAN 发送校准后的数据 ✅
15. **ADC 数据打印命令** - 新增 `adc_print` 命令，可打印所有通道校准后的数据 ✅
16. **独立看门狗功能** - 独立看门狗已实现，超时时间约32.768秒，每1秒喂狗一次，系统死机时自动复位 ✅
17. **CAN节点地址可配置** - 支持通过 `env_set_can_addr` 命令修改CAN节点地址，范围0-127 ✅
18. **ADC数据打印开关** - 支持通过 `env_set_print_adc` 控制是否每秒钟打印一次ADC数据 ✅

### 系统启动流程

```
main()
├── HAL_Init()
├── SystemClock_Config()
├── MX_XXX_Init() (外设初始化)
├── osKernelInitialize()
├── MX_FREERTOS_Init()
│   └── 创建 defaultTask
└── osKernelStart()
    └── defaultTask 运行
        ├── BSP_Init()
        ├── Middleware_Init()
        │   ├── LetterShell_Init()
        │   ├── easyflash_init()
        │   └── 创建 shell_task
        ├── App_Init()
        │   ├── BSP_ADC_Init()
        │   ├── DataProcess_Init()
        │   └── 启动 TIM3 定时器
        ├── Test_Run() (如果 TEST_ENABLE=1)
        └── 主循环 (LED闪烁)
```

### CAN通信协议

CAN通信实现了完整的复帧传输协议：

- **帧格式**: CAN 2.0 标准帧（11位ID）
- **节点地址**: 可配置（默认 30），通过 `env_set_can_addr` 命令修改
- **数据长度**: 106字节（设备标识2字节 + 12路电压48字节 + 12路电流48字节 + 2路温度8字节）
- **传输方式**: 复帧（首帧×1 + 中间帧×12 + 末帧×1 = 共14帧）
- **校验方式**: SUM校验（末帧最后1字节）
- **采样周期**: 1秒（TIM3触发）
- **回环测试**: ✅ 通过

详细协议请参考 `doc/can协议/can协议.md`。

### Letter Shell 使用

项目已移植 Letter Shell 3.x 命令行组件：

- **Shell 任务**: 在 `Middleware_Init()` 中创建并启动
- **通信接口**: UART4 (115200 8N1)
- **默认用户**: user (无密码)

#### 可用命令

| 命令 | 功能 |
|------|------|
| `help [cmd]` | 显示所有可用命令或指定命令帮助 |
| `test` | Shell 测试命令 |
| `sysinfo` | 显示系统信息 |
| `cantest` | 测试 CAN 发送功能 |
| `flash_erase <page>|all` | 擦除 Flash 数据页 |
| `flash_write <addr> <data1> <data2> ...` | 向 Flash 写入数据 |
| `flash_read <addr> <len>` | 从 Flash 读取数据 |
| `env_get <key>` | 读取环境变量 |
| `env_set <key> <value>` | 设置环境变量 |
| `env_del <key>` | 删除环境变量 |
| `env_print` | 打印所有环境变量 |
| `env_set_master 1|0` | 设置为主机或从机（设置后需重启） |
| `env_set_vs <0-11> <k> <b>` | 设置指定电压通道的 k 和 b 值 |
| `env_set_cur <0-11> <k> <b>` | 设置指定电流通道的 k 和 b 值 |
| `env_set_temp <0-1> <k> <b>` | 设置指定温度通道的 k 和 b 值 |
| `env_set_can_addr <addr>` | 设置 CAN 节点地址（0-127），设置后需重启 |
| `env_set_print_adc <1|0>` | 设置是否每秒钟打印一次 ADC 数据，修改后立即生效 |
| `adc_print` | 打印所有 ADC 通道校准后的数据 |
| `clear` | 清空控制台 |
| `cmds` / `vars` / `keys` / `users` | 显示各类列表 |

#### 按键功能

- `Tab` - 命令补全
- `↑` / `↓` - 历史命令
- `Backspace` - 删除字符
- `Enter` - 执行命令 (支持 LF 或 CR)

#### 使用示例

连接 UART4 (115200 8N1)，启动后会看到：
```
  _         _   _                  _          _ _
 | |    ___| |_| |_ ___ _ __   ___| |__   ___| | |
 | |   / _ \ __| __/ _ \ '__| / __| '_ \ / _ \ | |
 | |__|  __/ |_| ||  __/ |    \__ \ | | |  __/ | |
 |_____\___|\__|\__\___|_|    |___/_| |_|\___|_|_|

 Build:       Jul  6 2026
 Version:     3.2.4
 Copyright:   (c) 2020 Letter

user:/$
```

输入 `help` 查看所有可用命令：
```
user:/$ help
```

#### EasyFlash 命令使用示例：

```
user:/$ env_print
=== Env List ===
  node_role = master
  can_node_addr = 30
  print_adc_data = 0
  vs_k = 1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000
  vs_b = 0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000
  cur_k = 1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000
  cur_b = 0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000
  temp_k = 1.000000,1.000000
  temp_b = 0.000000,0.000000

mode: normal
```

```
user:/$ env_set_master 0
Set node role to slave and saved! Please reboot to take effect.
```

```
user:/$ env_set_vs 5 1.23 0.05
Set vs[5] k=1.230000, b=0.050000 and saved!
```

```
user:/$ env_set_cur 0 0.87 0.1
Set cur[0] k=0.870000, b=0.100000 and saved!
```

```
user:/$ env_set_temp 1 2.5 0.2
Set temp[1] k=2.500000, b=0.200000 and saved!
```

```
user:/$ env_set_can_addr 31
Set can_node_addr = 31 and saved! (Reboot to take effect)
```

```
user:/$ env_set_print_adc 1
Set print_adc_data = 1 and saved! (Takes effect immediately)
```

#### ADC 数据打印命令使用示例：

```
user:/$ adc_print
=== ADC Data ===
Voltage (VS0-VS11):
  VS0: 12.345678
  VS1: 12.456789
  ...
Current (CUR0-CUR11):
  CUR0: 0.123456
  CUR1: 0.234567
  ...
Temperature (TEMP0-TEMP1):
  TEMP0: 25.123456
  TEMP1: 26.234567
```

### 测试说明

测试代码位于 `Core/test/` 目录下，通过宏定义控制开关：

```c
#define TEST_ENABLE               1    /* 测试总开关 */
#define TEST_ENABLE_ADC           0    /* ADC测试 */
#define TEST_ENABLE_DATA_PROCESS  0    /* 数据处理测试 */
#define TEST_ENABLE_UART          0    /* 串口测试 */
#define TEST_ENABLE_CAN           0    /* CAN测试 */
#define TEST_ENABLE_FLASH         0    /* Flash测试 */
#define TEST_ENABLE_EASYFLASH     0    /* EasyFlash测试 */
```

上电后自动运行所有启用的测试，通过UART4输出测试结果。

**CAN测试输出示例**：
```
[TEST] CAN Loopback Test
  Enabling internal loopback mode...
  Sending & receiving test data (14 frames)...
    [RX] ID=0x1F1, DLC=8, Data=...
    [RX] ID=0x1F2, DLC=8, Data=... (×12)
    [RX] ID=0x1F3, DLC=5, Data=...
  Received 14 frames
  [PASS] CAN Loopback Test OK
  Disabling loopback mode...
```

### 待实现（TODO）

详见 `TODO.md`。

## 编译说明

本项目使用 STM32CubeMX 生成，支持以下开发环境：
- STM32CubeIDE
- Keil MDK-ARM
- IAR EWARM
