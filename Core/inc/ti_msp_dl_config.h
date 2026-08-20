/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                      4500000



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                            (23U)
#define TIMER_0_INST_PUB_0_CH                                                (1)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMG6)
#define TIMER_1_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                             (0U)
#define TIMER_1_INST_PUB_0_CH                                                (2)



/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI1
#define SPI_0_INST_IRQHandler                                   SPI1_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI1_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                       DL_GPIO_PIN_8
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM25)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM25_PF_SPI1_PICO
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM26)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM26_PF_SPI1_SCLK



/* Defines for ADC_Current */
#define ADC_Current_INST                                                    ADC0
#define ADC_Current_INST_IRQHandler                              ADC0_IRQHandler
#define ADC_Current_INST_INT_IRQN                                (ADC0_INT_IRQn)
#define ADC_Current_ADCMEM_0                                  DL_ADC12_MEM_IDX_0
#define ADC_Current_ADCMEM_0_REF               DL_ADC12_REFERENCE_VOLTAGE_EXTREF
#define ADC_Current_ADCMEM_0_REF_VOLTAGE_V                                    3.30



/* Defines for COMP_0 */
#define COMP_0_INST                                                        COMP0
#define COMP_0_INST_INT_IRQN                                      COMP0_INT_IRQn

/* Defines for COMP_0 DACCODE0 */
#define COMP_0_DACCODE0                                                    (152)


/* GPIO configuration for COMP_0 */
#define GPIO_COMP_0_IN2P_PORT                                            (GPIOA)
#define GPIO_COMP_0_IN2P_PIN                                    (DL_GPIO_PIN_14)
#define GPIO_COMP_0_IOMUX_IN2P                                   (IOMUX_PINCM36)
#define GPIO_COMP_0_IOMUX_IN2P_FUNC               (IOMUX_PINCM36_PF_UNCONNECTED)




/* Defines for VREF */
#define VREF_VOLTAGE_MV                                                     3300
#define GPIO_VREF_VREFPOS_PORT                                             GPIOA
#define GPIO_VREF_VREFPOS_PIN                                     DL_GPIO_PIN_23
#define GPIO_VREF_IOMUX_VREFPOS                                  (IOMUX_PINCM53)
#define GPIO_VREF_IOMUX_VREFPOS_FUNC                IOMUX_PINCM53_PF_UNCONNECTED
#define GPIO_VREF_VREFNEG_PORT                                             GPIOA
#define GPIO_VREF_VREFNEG_PIN                                     DL_GPIO_PIN_21
#define GPIO_VREF_IOMUX_VREFNEG                                  (IOMUX_PINCM46)
#define GPIO_VREF_IOMUX_VREFNEG_FUNC                IOMUX_PINCM46_PF_UNCONNECTED
#define VREF_READY_DELAY                                                    (45)



/* Defines for OPA_0 */
#define OPA_0_INST                                                          OPA0
#define GPIO_OPA_0_IN0POS_PORT                                             GPIOA
#define GPIO_OPA_0_IN0POS_PIN                                     DL_GPIO_PIN_26
#define GPIO_OPA_0_IOMUX_IN0POS                                  (IOMUX_PINCM59)
#define GPIO_OPA_0_IOMUX_IN0POS_FUNC                IOMUX_PINCM59_PF_UNCONNECTED
#define GPIO_OPA_0_OUT_PORT                                                GPIOA
#define GPIO_OPA_0_OUT_PIN                                        DL_GPIO_PIN_22
#define GPIO_OPA_0_IOMUX_OUT                                     (IOMUX_PINCM47)
#define GPIO_OPA_0_IOMUX_OUT_FUNC                   IOMUX_PINCM47_PF_UNCONNECTED



/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define ADC_Current_INST_DMA_TRIGGER                  (DMA_ADC0_EVT_GEN_BD_TRIG)


/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for D3: GPIOA.3 with pinCMx 8 on package pin 9 */
#define LED_D3_PIN                                               (DL_GPIO_PIN_3)
#define LED_D3_IOMUX                                              (IOMUX_PINCM8)
/* Defines for DC: GPIOB.7 with pinCMx 24 on package pin 21 */
#define OLED_DC_PORT                                                     (GPIOB)
#define OLED_DC_PIN                                              (DL_GPIO_PIN_7)
#define OLED_DC_IOMUX                                            (IOMUX_PINCM24)
/* Defines for RES: GPIOA.27 with pinCMx 60 on package pin 47 */
#define OLED_RES_PORT                                                    (GPIOA)
#define OLED_RES_PIN                                            (DL_GPIO_PIN_27)
#define OLED_RES_IOMUX                                           (IOMUX_PINCM60)
/* Defines for CS: GPIOB.6 with pinCMx 23 on package pin 20 */
#define OLED_CS_PORT                                                     (GPIOB)
#define OLED_CS_PIN                                              (DL_GPIO_PIN_6)
#define OLED_CS_IOMUX                                            (IOMUX_PINCM23)
/* Defines for VCC: GPIOA.28 with pinCMx 3 on package pin 3 */
#define OLED_VCC_PORT                                                    (GPIOA)
#define OLED_VCC_PIN                                            (DL_GPIO_PIN_28)
#define OLED_VCC_IOMUX                                            (IOMUX_PINCM3)
/* Port definition for Pin Group MOS */
#define MOS_PORT                                                         (GPIOA)

/* Defines for SAMPLE: GPIOA.1 with pinCMx 2 on package pin 2 */
#define MOS_SAMPLE_PIN                                           (DL_GPIO_PIN_1)
#define MOS_SAMPLE_IOMUX                                          (IOMUX_PINCM2)
/* Defines for PROTECT: GPIOA.0 with pinCMx 1 on package pin 1 */
#define MOS_PROTECT_PIN                                          (DL_GPIO_PIN_0)
#define MOS_PROTECT_IOMUX                                         (IOMUX_PINCM1)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_ADC_Current_init(void);
void SYSCFG_DL_COMP_0_init(void);
void SYSCFG_DL_VREF_init(void);
void SYSCFG_DL_OPA_0_init(void);
void SYSCFG_DL_DMA_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
