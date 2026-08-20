/*

 * @brief       main
 * @language    C
 * @harfware   Template -32M-Sleep
 * @version     v2.1
 * @date        31-July-2026
 * @author      YunSword (modified)

 * v2.1:
 *  - 主频 5MHz (SYSOSC 32MHz -> SYSPLL 分频), 全内部振荡器, 不依赖外部晶振
 *  - STANDBY1 + TIMG0(LFCLK) 定时唤醒, 每秒醒一次干活, 平时睡觉
 *  - 睡眠档位在 main 里设, 调功耗直接改这一处
 *  - 头部附全睡眠模式教学说明

 * ============================================================================
 *  MSPM0G3507 低功耗模式详解(教学用)
 * ============================================================================
 *  本工程时钟: SYSOSC 32MHz(内部) -> SYSPLL -> MCLK 5MHz; LFOSC(内部) -> LFCLK
 *
 *  睡眠的本质: 按档位逐级停掉 CPU/外设/振荡器的时钟, 靠中断唤醒。
 *  两件事要分清:
 *    - DL_SYSCTL_setPowerPolicyXXX() 只是"选档位"(配置寄存器)
 *    - __WFI() 才是真正"睡下去"的指令
 *  --------------------------------------------------------------------------
 *  一、RUN/SLEEP 档 (最浅, 电流百 uA~mA 级)   入口: setPowerPolicyRUNxSLEEPx()
 *    SLEEP0: 只停 CPU, MCLK 和外设全部照常跑, 任意中断即醒, 唤醒最快(~1-2us)
 *    SLEEP1: CPU 停 + MCLK 切到 LFCLK(32k), SYSOSC 保活给模拟外设(ADC/COMP等)
 *    SLEEP2: 连 SYSOSC 也关, 只剩 LFCLK 域, 唤醒要等时钟重新起来
 *    用途: 任务间隙"打个盹", 比如等串口数据, 醒来接着跑
 *  --------------------------------------------------------------------------
 *  二、STOP 档 (中浅, 电流 ~0.4~1.5uA)       入口: setPowerPolicySTOPx()
 *    CPU/MCLK 全停, PD1 外设掉电(SPI/UART 的寄存器会丢! 唤醒要重新初始化),
 *    PD0 域(GPIO/TIMG0/8/部分UART)保留。RAM 和 GPIO 电平全部保持。
 *    STOP0: SYSOSC 32MHz 保持, ULPCLK 外设(TIMG/ADC/COMP)全速值守, 唤醒~2us
 *    STOP1: SYSOSC 降到 4MHz, 功耗/唤醒速度的折中
 *    STOP2: SYSOSC 关, ULPCLK 改由 LFCLK(32k) 提供, 定时器低速计时
 *    唤醒源: 任意 NVIC 中断(含 GPIO), 醒来从 WFI 下一条指令继续执行
 *    用途: 带外设值守的浅睡(按键/比较器/定时器唤醒)
 *  --------------------------------------------------------------------------
 *  三、STANDBY 档 (深睡, 电流 ~0.5~0.6uA)    入口: setPowerPolicySTANDBYx()
 *    除 PD0 外的域全部掉电, 只留 LFCLK 域给低功耗外设
 *    STANDBY0: TIMG0/1 也停(无时钟), 唤醒源只有 GPIO / RTC / WDT
 *    STANDBY1: TIMG0/1 用 LFCLK(32.768kHz) 继续计数, 可以定时唤醒
 *              ← 本工程就是 STANDBY1 + TIMG0 每 1s 唤醒一次
 *    唤醒源: GPIO / RTC / WDT / TIMG0 / TIMG1, 唤醒~4-6us
 *  --------------------------------------------------------------------------
 *  四、SHUTDOWN (彻底断电, ~20nA)            入口: setPowerPolicySHUTDOWN()
 *    全片断电, RAM 内容丢失, 只能复位/重新上电唤醒, 醒来重走完整启动流程
 *    用途: 纽扣电池撑几年的场景, 本工程用不到
 *  --------------------------------------------------------------------------
 *  各档对比 (电流为 3.3V/25C 典型值, 以数据手册为准):
 *    模式       | CPU | MCLK | 外设         | 唤醒源         | 唤醒时间 | 电流
 *    SLEEP0/1/2 | 停  | 5M/32k/关 | 全活/部分   | 任意中断       | ~1-2us  | 百uA~mA
 *    STOP0/1/2  | 停  | 停    | ULPCLK值守    | 任意中断/GPIO  | 2-5us   | 0.4~1.5uA
 *    STANDBY0   | 停  | 停    | RTC/WDT      | GPIO/RTC/WDT   | 4-6us   | ~0.5uA
 *    STANDBY1   | 停  | 停    | +TIMG0/1     | +TIMG0/1       | 4-6us   | ~0.6uA
 *    SHUTDOWN   | 停  | 停    | 无(RAM丢失)  | 复位/上电       | 数百us  | ~20nA
 *  --------------------------------------------------------------------------
 *  本工程实践(STANDBY1 周期唤醒):
 *    1. TIMG0 时钟选 LFCLK(32.768k), 计数 32768 个 tick = 1s, 零事件中断唤醒
 *    2. 每次入睡前: 关中断 -> setPowerPolicySTANDBY1() -> __WFI()
 *    3. 唤醒后必做: SPI/UART 在掉电域寄存器全丢, 重新初始化再干活
 *    4. 换档位调试: 改 main 循环里 setPowerPolicy 那一行即可
 *  踩过的坑:
 *    - LFOSC 若校准异常, TIMG0 周期会变长(本板实测 4~5s), 可用
 *      DL_TimerG_getTimerCount 前后读数 / 墙钟时间 测实际 LFCLK 频率
 *    - 睡眠期间 UART 收不到蓝牙数据(BUSCLK 停), 要蓝牙唤醒需改 GPIO 唤醒脚
 * ============================================================================
 */

#include "ti_msp_dl_config.h"
#include "oled_spi.h"
#include "stdio.h"
#include "math.h"
#define BUF_SIZE 400 //每个周期采集40点 一共10个周期

void ADC_DMA_Start(void);
double GetRMS(void);// 计算电流有效值，包括标定、滤波等等步骤
void DeepSleep(void);
void CPU_sleep(uint16_t ms_10);
void Peripheral_reset(void);
void OLED_Work(double result);
void OLED_Sleep(void);
void OPA_SetGain(OA_Regs *OPA_INST,double result);

uint16_t buf[BUF_SIZE];//DMA采集缓冲区
static volatile uint8_t DMA_done = 0,//DMA采集完成标志
			gWakeFlag = 0,//深睡标志位
			gCPUSleepFlag = 0;//CPU睡眠标志位
static uint8_t gain = 1;//运放增益


int main(void){

	SYSCFG_DL_init();		
	double result = 0;

	NVIC_EnableIRQ(ADC_Current_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	/*第一次开机，先测一轮，显示一回，以提高启动时间的指标*/
	DL_GPIO_clearPins(LED_PORT,LED_D3_PIN);//第一次开机开灯，指示现在正在工作
	DL_GPIO_setPins(MOS_PORT,MOS_SAMPLE_PIN | MOS_PROTECT_PIN);//打开采样用的两个mos管
	CPU_sleep(60);//前600ms还不太稳定，先等待一会
	ADC_DMA_Start();//采一次样
	while(DMA_done == 0)//等待DMA完成
		;
	DL_GPIO_clearPins(MOS_PORT,MOS_SAMPLE_PIN | MOS_PROTECT_PIN);//采样结束，两个mos管关掉
	DL_ADC12_reset(ADC_Current_INST);//ADC OPA关掉
	DL_OPA_reset(OPA_0_INST);
	
	result = GetRMS();//计算电流有效值
	DMA_done = 0;
	OLED_Work(result);
	OLED_Sleep();
	
    while (1){
		DeepSleep();
		gWakeFlag = 0;

		Peripheral_reset();//外设上电

		DL_GPIO_clearPins(LED_PORT,LED_D3_PIN);//开灯，指示现在正在工作
		DL_GPIO_setPins(MOS_PORT,MOS_SAMPLE_PIN | MOS_PROTECT_PIN);//打开采样用的两个mos管
		OPA_SetGain(OPA_0_INST,result);//OPA增益自动设置
		CPU_sleep(60);//前600ms个周期还不太稳定，先等待一会
		ADC_DMA_Start();//采一次样
		DL_SYSCTL_setPowerPolicyRUN0SLEEP0();
		while(!DMA_done)//等待DMA完成
			__WFE();//等待的过程中，让CPU睡一会
		DL_GPIO_clearPins(MOS_PORT,MOS_SAMPLE_PIN | MOS_PROTECT_PIN);//采样结束，两个mos管关掉
		DL_ADC12_reset(ADC_Current_INST);//ADC OPA关掉
		DL_OPA_reset(OPA_0_INST);
		
		result = GetRMS();//计算电流有效值
		DMA_done = 0;
		OLED_Work(result);
		OLED_Sleep();
	}
}

void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
			if(DL_COMP_getComparatorOutput(COMP_0_INST))//检查电容的电压状态，如果电压大于阈值，继续开机，反之，继续睡觉
            	gWakeFlag = 1;
            break;
        default:
            break;
    }
}

void TIMER_1_INST_IRQHandler(void)
{
	switch (DL_TimerG_getPendingInterrupt(TIMER_1_INST)) {
		case DL_TIMER_IIDX_ZERO:
			gCPUSleepFlag = 1;
			break;
		default:
			break;
	}
}

void ADC_Current_INST_IRQHandler(void){
	switch(DL_ADC12_getPendingInterrupt(ADC_Current_INST)){
		case DL_ADC12_IIDX_DMA_DONE:
			DMA_done = 1;
			DL_GPIO_clearPins(MOS_PORT,MOS_SAMPLE_PIN | MOS_PROTECT_PIN);//采样结束，两个mos管关掉
		break;
		default:
			break;
	}
	return;
}

void ADC_DMA_Start(void){//开始采样
	DL_ADC12_enableDMA(ADC_Current_INST);
	DL_DMA_setSrcAddr(DMA,DMA_CH0_CHAN_ID,DL_ADC12_getMemResultAddress(ADC_Current_INST,DL_ADC12_MEM_IDX_0));
	DL_DMA_setDestAddr(DMA,DMA_CH0_CHAN_ID,(uint32_t)buf);
	DL_DMA_setTransferSize(DMA,DMA_CH0_CHAN_ID,BUF_SIZE);
	DL_DMA_enableChannel(DMA,DMA_CH0_CHAN_ID);
	DL_ADC12_startConversion(ADC_Current_INST);
}

double GetRMS(void){
	long long int rms = 0;
	for(uint16_t i = 0;i<BUF_SIZE;i++)//在等待的过程，计算上一次DMA搬完的数据
		rms += buf[i]*buf[i];
	double rms_f64 = sqrt(rms *1.0 / BUF_SIZE)/4096.0*3.3;
	rms_f64 = (0.91817 * rms_f64 + 0.0034012)/gain;
	switch(gain){
		case 1:
			rms_f64 = 0.990359 * rms_f64 + 0.016488;
			break;
		case 2:
			rms_f64 = 1.022102*rms_f64 - 0.012788;
			break;
		case 4:
			rms_f64 = 0.971941*rms_f64 + 0.013603;
			break;
		case 8:
			rms_f64 = 1.021771*rms_f64- 0.000994;
			break;
		case 16:
			rms_f64 = 0.998849*rms_f64 + 0.002479;
			break;
		default:
			rms_f64 = rms_f64 / 0.1;
			break;
	}
	return rms_f64;
}

void DeepSleep(void){
	DL_SYSCTL_setPowerPolicySTANDBY1();
	DL_GPIO_setPins(LED_PORT,LED_D3_PIN);
	DL_SPI_reset(SPI_0_INST);
	DL_ADC12_reset(ADC_Current_INST);
	DL_OPA_reset(OPA_0_INST);
	DL_TimerG_reset(TIMER_1_INST);
	DL_TimerG_startCounter(TIMER_0_INST);
	while(0 == gWakeFlag)
		__WFE();
	DL_TimerG_stopCounter(TIMER_0_INST);
}

void Peripheral_reset(void){
	DL_SPI_enablePower(SPI_0_INST);
	DL_ADC12_enablePower(ADC_Current_INST);
	DL_OPA_enablePower(OPA_0_INST);
	DL_TimerG_enablePower(TIMER_1_INST);
	SYSCFG_DL_SPI_0_init();
	SYSCFG_DL_ADC_Current_init();
	SYSCFG_DL_OPA_0_init();
	SYSCFG_DL_DMA_init();
	SYSCFG_DL_TIMER_1_init();
	return;
}
void OLED_Work(double result){
	static char OLED_buf[100] = {};
		/*屏幕显示期间，不用的外设通通关掉*/
	DL_GPIO_setPins(LED_PORT,LED_D3_PIN);
		
	OLED_On();
	OLED_RES_Set();
	snprintf(OLED_buf,100,"%.5fA",result);//这个位置实际上要放一个延时，等待OLED开机，不过这个函数的时间可以充当延时
	OLED_Init();
	OLED_ShowString(0,0,OLED_buf);
	OLED_Refresh();
	DL_SPI_reset(SPI_0_INST);//SPI用完就关
	CPU_sleep(50);//显示500ms
}

void OLED_Sleep(void){
	OLED_Off();
	OLED_RES_Clr();
	DL_SPI_reset(SPI_0_INST);
}

void CPU_sleep(uint16_t ms_10){
	DL_SYSCTL_setPowerPolicyRUN0SLEEP0();
	DL_TimerG_setLoadValue(TIMER_1_INST,ms_10);//设置定时器计数值，单位为10ms
	DL_TimerG_startCounter(TIMER_1_INST);
	while(0 == gCPUSleepFlag)
		__WFE();
	gCPUSleepFlag = 0;
}



void OPA_SetGain(OA_Regs *OPA_INST,double result){
	static DL_OPA_Config gOPA_0Config_nselRTOP = {//nsel为RTOP，增益为1
		.pselChannel    = DL_OPA_PSEL_IN0_POS,
		.nselChannel    = DL_OPA_NSEL_RTOP,
		.mselChannel    = DL_OPA_MSEL_GND,
		.gain           = DL_OPA_GAIN_N1_P2,
		.outputPinState = DL_OPA_OUTPUT_PIN_ENABLED,
		.choppingMode   = DL_OPA_CHOPPING_MODE_DISABLE,
	};
	static DL_OPA_Config gOPA_0Config_nselRTAP = {//nsel为RTAP，增益可以配置
		.pselChannel    = DL_OPA_PSEL_IN0_POS,
		.nselChannel    = DL_OPA_NSEL_RTAP,
		.mselChannel    = DL_OPA_MSEL_GND,
		.gain           = DL_OPA_GAIN_N1_P2,
		.outputPinState = DL_OPA_OUTPUT_PIN_ENABLED,
		.choppingMode   = DL_OPA_CHOPPING_MODE_DISABLE,
	};

	if(result > 1){//如果电流大于0.5A，增益改为1
		if(gain != 1)
			gain = 1;
	}
	else if(result >0.5){
		if(gain != 2)
			gain = 2;
	}
	else if(result >0.25){
		if(gain != 4)
			gain = 4;
	}
	else if(result >0.125){
		if(gain != 8)
			gain = 8;
	}
	else{
		if(gain != 16)
			gain = 16;
	}
	if(gain == 1){//进入到这个分支,由于不能直接改nsel，要重新配置，很史 
		DL_OPA_reset(OPA_INST);
		DL_OPA_enablePower(OPA_INST);
		delay_cycles(POWER_STARTUP_DELAY);
		DL_OPA_enableRailToRailInput(OPA_0_INST);
		DL_OPA_init(OPA_INST, &gOPA_0Config_nselRTOP);
		DL_OPA_enable(OPA_0_INST);
	}
	else {
		DL_OPA_reset(OPA_INST);
		DL_OPA_enablePower(OPA_INST);
		delay_cycles(POWER_STARTUP_DELAY);
		DL_OPA_enableRailToRailInput(OPA_0_INST);
		DL_OPA_init(OPA_INST, &gOPA_0Config_nselRTAP);
		DL_OPA_enable(OPA_0_INST);
		switch(gain){
			case 2:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N1_P2);
				break;
			case 4:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N3_P4);
				break;
			case 8:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N7_P8);
				break;
			case 16:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N15_P16);
				break;
			case 32:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N31_P32);
				break;
			default:
				DL_OPA_setGain(OPA_INST,DL_OPA_GAIN_N1_P2);
				break;
		}
	}
	return;
}

