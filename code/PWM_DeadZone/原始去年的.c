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


#define PLLCON_SETTING  CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK       50000000

#define CEN 19

int flag=0;
int score=0;

void set_switch(void);
void GPCDEF_IRQHandler(void);
void turn_to_CEN(void);
void motor_second(void);
void motor_first(void);

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
		int i, control, pos;
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
	
		set_switch();
		NVIC_EnableIRQ(GPCDEF_IRQn);
		
		
		while(1){
			printf("Start Game!!\n");
			printf("Input 1 for turn left/3 for turn right\n");
			printf("Input 5 to drop the ball\n");
			
			score=0;
			pos = CEN;
			
			for(i=0; i<5; i++){				
				control = 0;

				while(1){						
					printf("Please input 1 or 3 to control the direction,or input 5 to drop the ball\n");
					scanf("%d", &control);
					
					//control the direction to left
					if(control == 1 && CEN >= 15){				
						pos = pos -2;
						PWM_ConfigOutputChannel(PWMA, PWM_CH2, 100, pos);
		
						// Enable output of all PWMA channels
						PWM_EnableOutput(PWMA, 0xF);

						// Start
						PWM_Start(PWMA, 0xF);							
						CLK_SysTickDelay(100000);						
						PWM_Stop(PWMA, 0xF);
					}
					
					//control the direction to right
					else if(control == 3 && CEN <= 23){			
						pos = pos +2;
						PWM_ConfigOutputChannel(PWMA, PWM_CH2, 100, pos);
		
						// Enable output of all PWMA channels
						PWM_EnableOutput(PWMA, 0xF);

						// Start
						PWM_Start(PWMA, 0xF);							
						CLK_SysTickDelay(100000);						
						PWM_Stop(PWMA, 0xF);
					}
					
					//drop the ball
					else if(control == 5){							
						
						motor_second();
						
						motor_first();
														
						printf("");
						break;
					}
				}
				while(flag == 0){
					printf("");
				}
				flag = 0;
				printf("end of drop\n");
			}
			
			turn_to_CEN();
			printf("End of the game\n");
			printf("Your total score is: %d\n", score);
		}
}

//subfunction

void turn_to_CEN()
{
		PWM_ConfigOutputChannel(PWMA, PWM_CH2, 100, CEN);
		
		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		// Start
		PWM_Start(PWMA, 0xF);							
		CLK_SysTickDelay(10000);						
		PWM_Stop(PWMA, 0xF);
}

void motor_second()
{
		PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, 12);

		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		// Start
		PWM_Start(PWMA, 0xF);								
		CLK_SysTickDelay(1000000);
		PWM_Stop(PWMA, 0xF);
												
	
		PWM_ConfigOutputChannel(PWMA, PWM_CH0, 100, 21);

		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		// Start
		PWM_Start(PWMA, 0xF);							
		CLK_SysTickDelay(100000);
		PWM_Stop(PWMA, 0xF);
}

void motor_first()
{
		//FIRST    
		PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, 14);
														
		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		// Start
		PWM_Start(PWMA, 0xF);				
		CLK_SysTickDelay(1000000);								
		PWM_Stop(PWMA, 0xF);
												
						
		PWM_ConfigOutputChannel(PWMA, PWM_CH1, 100, 21);

		// Enable output of all PWMA channels
		PWM_EnableOutput(PWMA, 0xF);

		// Start
		PWM_Start(PWMA, 0xF);								
		CLK_SysTickDelay(100000);								
		PWM_Stop(PWMA, 0xF);
}

void set_switch()
{
	GPIO_SetMode(PC, BIT1, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 1, GPIO_INT_FALLING);
		
	//PC->PMD = (PC->PMD & (GPIO_PMD_PMD12_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD12_Pos);
	GPIO_SetMode(PC, BIT9, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 9, GPIO_INT_FALLING);
				
	//PC->PMD = (PC->PMD & (GPIO_PMD_PMD12_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD12_Pos);
	GPIO_SetMode(PC, BIT3, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 3, GPIO_INT_FALLING);
			
	//PC->PMD = (PC->PMD & (GPIO_PMD_PMD12_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD12_Pos);
	GPIO_SetMode(PC, BIT4, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 4, GPIO_INT_FALLING);
				
	//PC->PMD = (PC->PMD & (GPIO_PMD_PMD12_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD12_Pos);
	GPIO_SetMode(PC, BIT5, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 5, GPIO_INT_FALLING);
				
	//PC->PMD = (PC->PMD & (GPIO_PMD_PMD12_Msk)) | (GPIO_PMD_OUTPUT << GPIO_PMD_PMD12_Pos);
	GPIO_SetMode(PC, BIT6, GPIO_PMD_INPUT);
	GPIO_EnableInt(PC, 6, GPIO_INT_FALLING);
}

void GPCDEF_IRQHandler()
{
		int test=0;
		
		CLK_SysTickDelay(1000000);	
	
		if(GPIO_GET_INT_FLAG(PC, BIT1)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT1);
			printf("You get 1 point!\n");
			CLK_SysTickDelay(2000000);
			test=1;
			score=score+test;
			
			flag = 1;
		}
		else if(GPIO_GET_INT_FLAG(PC, BIT9)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT9);
			printf("You get 2 point!\n");
			CLK_SysTickDelay(2000000);
			test=2;
			score=score+test;

			flag = 1;
		}
		else if(GPIO_GET_INT_FLAG(PC, BIT3)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT3);
			printf("You get 3 point!\n");
			CLK_SysTickDelay(2000000);
			test=3;
			score=score+test;
			
			flag = 1;
		}
		else if(GPIO_GET_INT_FLAG(PC, BIT4)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT4);
			printf("You get 4 point!\n");
			CLK_SysTickDelay(2000000);
			test=4;
			score=score+test;
			
			flag = 1;
		}
		else if(GPIO_GET_INT_FLAG(PC, BIT5)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT5);
			printf("You get 5 point!\n");
			CLK_SysTickDelay(2000000);
			test=5;
			score=score+test;
			
			flag = 1;
		}
		else if(GPIO_GET_INT_FLAG(PC, BIT6)){
			CLK_SysTickDelay(5000000);
			GPIO_CLR_INT_FLAG(PC, BIT6);
			printf("You get 6 point!\n");
			CLK_SysTickDelay(2000000);
			test=6;
			score=score+test;
			
			flag = 1;
		}
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
