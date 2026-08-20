# 无源电流表（Sourceless Ammeter）—— 2026 电赛 B 题 · 测量电路篇

基于 **TI MSPM0G3507** 的 2026 年全国大学生电子设计竞赛 B 题「无源电流表」测量电路（电路 A）固件。

电流互感器（CT）副边同时承担「感应取电」与「电流测量」：深睡时储能电容充电，唤醒后以超低功耗完成电流有效值测量并在 OLED 上显示。实测可在 **0.08A** 原边电流下启动，测量精度 **0.5%** 以内。

配套教程博客：《[2026年电赛B题——无源电流表攻略（测量电路篇）](https://www.cnblogs.com/)》

## 实现内容

- **充电 / 测量切换**
  - 深睡时采样 MOS 全关，储能电容充电；唤醒后打开采样 MOS（`MOS_SAMPLE_PIN | MOS_PROTECT_PIN`）
  - 打开 MOS 后等待约 600ms（`CPU_sleep(60)`）让波形稳定再采样
- **电流有效值测量**
  - ADC12 + DMA 采集 400 点（10 个工频周期 × 每周期 40 点），采样等待期间 CPU 休眠（`RUN0SLEEP0` + `__WFE()`）
  - `GetRMS()`：平方均值开根换算电压 → 按当前增益归一 → 各增益档独立标定系数修正
- **片内运放程控增益**
  - `OPA_SetGain()` 按电流大小自动切换增益 1 / 2 / 4 / 8 / 16
  - 增益 1 配置为 RTOP 跟随器，其余配置为 RTAP 同相放大器（切换需重新初始化 OPA）
- **超低功耗设计**
  - `DeepSleep()`：STANDBY1 深睡 + TIMER_0（LFCLK 32.768kHz）周期唤醒
  - 片内比较器 `COMP_0`（VREF DAC 阈值）检测储能电容电压：电压不足继续睡，足够才开机
  - `CPU_sleep()`：TIMER_1 以 10ms 为粒度计时休眠（`RUN0SLEEP0`）
  - 所有外设（SPI / ADC / OPA / TIMER1）用完即复位断电，唤醒后由 `Peripheral_reset()` 重新上电初始化
- **OLED 显示**
  - SPI 0.96" OLED（`Core/OLED/`），显示电流结果 0.5s 后熄灭并断电

## 工程环境与工具

| 项目 | 说明 |
|---|---|
| 主控芯片 | TI MSPM0G3507（Cortex-M0+，64KB SRAM / 128KB Flash） |
| 开发环境 | Keil MDK uVision，ARM Compiler **V6.21**（ARMCLANG） |
| 外设配置 | TI SysConfig **1.28.0**（`project.syscfg`） |
| 驱动库 | MSPM0 SDK **2.08.00.03**（DriverLib） |
| 时钟方案 | 全内部振荡器：SYSOSC 32MHz → SYSPLL → MCLK **2.25MHz**；LFOSC → LFCLK 32.768kHz |

## 目录结构

```
├── Core/
│   ├── src/          # main.c（主逻辑）、key.c（按键扫描，当前未启用）、ti_msp_dl_config.c
│   ├── inc/          # ti_msp_dl_config.h、key.h
│   └── OLED/         # oled_spi.c/h、oledfont.h（SPI OLED 驱动）
├── Driver/           # TI MSPM0 DriverLib + CMSIS（勿手动修改）
├── keil/             # Keil 工程（project.uvprojx）、启动文件 startup_mspm0g3507_uvision.s
├── toolchains/       # GCC 工具链模板（备用构建方式）
├── project.syscfg    # SysConfig 外设配置（外设名、DMA、中断等）
├── CMakeLists.txt    # CMake 构建模板（当前以 Keil 为准）
├── syscfg.bat        # SysConfig 命令行重新生成外设代码的脚本
├── move_file.bat     # 将生成的外设代码移入 Core/src 与 Core/inc
├── MSPM0G3507_V2.0_引脚资源分配图.png   # MSPM0G3507 引脚资源分配图
├── EVM_MSPM0_V2.0_引脚资源分配图1.png   # EVM 引脚资源分配图
└── MSPM0G3507_V2.0.pdf                  # MSPM0G3507 数据手册
```

## 编译与烧录

1. 安装 Keil MDK（含 ARM Compiler V6.21）、TI MSPM0 SDK 2.08.00.03、TI SysConfig 1.28.0
2. 用 Keil uVision 打开 `keil/project.uvprojx`，直接编译下载即可
3. 若修改了 `project.syscfg`：先按本机路径修改 `syscfg.bat` 中的 SDK / SysConfig 路径，再运行
   `syscfg.bat <工程目录> project.syscfg` 重新生成 `ti_msp_dl_config.c/h`，最后运行 `move_file.bat` 将生成文件移入 `Core/src` 与 `Core/inc`

## 主循环流程

1. 上电后立即测一轮并显示（优化启动时间指标）
2. 进入主循环：
   `DeepSleep()` 深睡 → 比较器判定电容电压足够 → `Peripheral_reset()` 恢复外设 → 打开采样 MOS → `OPA_SetGain()` 自动增益 → 等待 600ms 波形稳定 → ADC+DMA 采样（CPU 休眠）→ `GetRMS()` 计算有效值 → OLED 显示 0.5s → 熄灭 → 回到深睡

## 许可

MIT License，详见 [LICENSE](LICENSE)。
