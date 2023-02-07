/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/10 2:05p $
 * @brief    Demonstrate how to use PWM Dead Zone function.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"
#include "Scankey.h"


#define PLLCON_SETTING  CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK       50000000


#define cen1 20
#define cen2 18
int control, pos1, pos2,i;
int i;


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/**
 * @brief       PWMA IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle PWMA interrupt event
 */
void GPCDEF_IRQHandler(void)
{
    /* To check if PE.5 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PC, BIT9))
    {
			if(pos2 >= 11){
					pos2 = pos2 - 1;
					printf("current pos2 = %d\n",pos2);
					PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, pos2);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}
				CLK_SysTickDelay(1000000);
        GPIO_CLR_INT_FLAG(PC, BIT9);
        printf("PC9 = 1\n");
    }
		if(GPIO_GET_INT_FLAG(PC, BIT11))
    {
			if(pos2 <= 25){
					pos2 = pos2 + 1;
					printf("current pos2 = %d\n",pos2);
					PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, pos2);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}
				CLK_SysTickDelay(10000000);
        GPIO_CLR_INT_FLAG(PC, BIT11);
        printf("PC11 = 1\n");
    }
		if(GPIO_GET_INT_FLAG(PC, BIT13))
    {
					if(pos1 <= 23){
					pos1 = pos1 + 1;
					printf("current pos1 = %d\n",pos1);
					PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, pos1);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
					}
				CLK_SysTickDelay(5000000);
        GPIO_CLR_INT_FLAG(PC, BIT13);
        printf("PC13 = 1\n");
    }
		if(GPIO_GET_INT_FLAG(PC, BIT15))
    {
					if(pos1 >= 15){
					pos1 = pos1 - 1;
					printf("current pos1 = %d\n",pos1);
					PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, pos1);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
					}
				CLK_SysTickDelay(5000000);
        GPIO_CLR_INT_FLAG(PC, BIT15);
        printf("PC15 = 1\n");
    }
		if(GPIO_GET_INT_FLAG(PC, BIT7))
    {
				printf("PC7 = 1\n");
				for(i = 0 ;i<4 ;i++){
				PWM_ConfigOutputChannel(PWMA, PWM_CH2, 100, 17);
					
				// Enable output of all PWMA channels
				PWM_EnableOutput(PWMA, 0xF);

				// Start
				PWM_Start(PWMA, 0xF);							
				CLK_SysTickDelay(10000000);						
				PWM_Stop(PWMA, 0xF);
				}
				GPIO_CLR_INT_FLAG(PC, BIT7);
				CLK_SysTickDelay(10000000);						
				return;
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC, PD, PE and PF interrupts */
        PC->ISRC = PC->ISRC;
        PD->ISRC = PD->ISRC;
        PE->ISRC = PE->ISRC;
        PF->ISRC = PF->ISRC;
        //printf("Un-expected interrupts.\n");
    }
		return;
}
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for IRC22M clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external 12MHz XTAL, internal 22.1184MHz */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC22M_EN_Msk);

    /* Enable PLL and Set PLL frequency */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_CLKDIV_HCLK(2));

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable PWM module clock */
    CLK_EnableModuleClock(PWM01_MODULE);
    CLK_EnableModuleClock(PWM23_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select PWM module clock source */
    CLK_SetModuleClock(PWM01_MODULE, CLK_CLKSEL1_PWM01_S_HXT, 0);
    CLK_SetModuleClock(PWM23_MODULE, CLK_CLKSEL1_PWM23_S_HXT, 0);
	
	

    /* Reset PWMA channel0~channel3 */
    SYS_ResetModule(PWM03_RST);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = PLL_CLOCK;            // PLL
    SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);
    /* Set GPA multi-function pins for PWMA Channel0~3 */
    SYS->GPA_MFP = SYS_GPA_MFP_PA12_PWM0 | SYS_GPA_MFP_PA13_PWM1 | SYS_GPA_MFP_PA14_PWM2 | SYS_GPA_MFP_PA15_PWM3;
		
		GPIO_SetMode(PC, BIT9, GPIO_PMD_INPUT);
		GPIO_EnableInt(PC, 9, GPIO_INT_FALLING);
		GPIO_SetMode(PC, BIT11, GPIO_PMD_INPUT);
		GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
		GPIO_SetMode(PC, BIT13, GPIO_PMD_INPUT);
		GPIO_EnableInt(PC, 13, GPIO_INT_FALLING);
		GPIO_SetMode(PC, BIT15, GPIO_PMD_INPUT);
		GPIO_EnableInt(PC, 15, GPIO_INT_FALLING);
		GPIO_SetMode(PC, BIT14, GPIO_PMD_INPUT);
		GPIO_EnableInt(PC, 7, GPIO_INT_FALLING);
		
		
		//NVIC_EnableIRQ(GPCDEF_IRQn);
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
		
		
		/**------------Turn to center-----------*/
		PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, cen1);
			
		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		printf("test : rotate more than 360 degree\n");
		for(i=0; i<3; i++){
				PWM_ConfigOutputChannel(PWMA, PWM_CH2, 100, 17);
					
				// Enable output of all PWMA channels
				PWM_EnableOutput(PWMA, 0xF);

				// Start
				PWM_Start(PWMA, 0xF);							
				CLK_SysTickDelay(10000000);						
				PWM_Stop(PWMA, 0xF);
		}
		NVIC_EnableIRQ(GPCDEF_IRQn);
		printf("Start Game!!\n");
		printf("Input 1 for turn left/3 for turn right\n");
		printf("Input 2 to turn up/5 to turn down\n");
		
		pos1 = cen1;
		pos2 = cen2;
		pos2 = 26;
		/**-------End----------**/
	
		
		while(1){
			control = 0;		
			OpenKeyPad();
			control = ScanKey();
			CLK_SysTickDelay(70000);
			if(control !=0){
				printf("scankey = %d\n",control);
			if(control == 3 && pos2 >= 11){
					pos2 = pos2 - 1;
					printf("current pos2 = %d\n",pos2);
					PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, pos2);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}	
			if(control == 1 && pos2 <= 25){
					pos2 = pos2 + 1;
					printf("current pos2 = %d\n",pos2);
					PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, pos2);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}
			
			if(control == 2 && pos1 <= 23){
					pos1 = pos1 + 1;
					printf("current pos1 = %d\n",pos1);
					PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, pos1);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}	
			if(control == 4 && pos1 >= 15){
					pos1 = pos1 - 1;
					printf("current pos1 = %d\n",pos1);
					PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, pos1);
			
					// Enable output of all PWMA channels
					PWM_EnableOutput(PWMA, 0xF);

					// Start
					PWM_Start(PWMA, 0xF);							
					CLK_SysTickDelay(50000);						
					PWM_Stop(PWMA, 0xF);
			}
		}
		}
}

//subfunction




/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
