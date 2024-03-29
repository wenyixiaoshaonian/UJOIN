#include <stm32f10x.h>   
#include "BspTime.h"
#include "common.h"

/*************外部变量****************/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
/***********************************/
void GPIO_Configuration(void)
{
	//IO口配置成功指示灯
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 					 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC , GPIO_Pin_7);
//  GPIO_SetBits(GPIOC , GPIO_Pin_7);

   //串口配置
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
		
//	//IO口配置成功指示灯
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 					 
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB , GPIO_Pin_12);
////	//GPIO_SetBits(GPIOB , GPIO_Pin_15);

//   //串口配置
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
//    // 配置 USART1 Tx (PA.09) 作为功能引脚并上拉输出模式
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

//    //配置 USART1 Tx (PA.10) 作为功能引脚并是浮空输入模式
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void UART_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* USART1 配置 ------------------------------------------------------------
         USART1 配置如下:
          - 波特率      = 115200 baud
          - 字长        = 8 Bits
          - 一个停止位
          - 无校验
          - 无硬件流控制
          - 接受和发送使能
    --------------------------------------------------------------------------*/
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    GPIO_Configuration();
    USART_Init(UART5, &USART_InitStructure);
    // 使能 USART1
    USART_Cmd(UART5, ENABLE);
}

int main(void) 	
{   
	int i;
	unsigned char *addr = NULL;
	SystemInit(); 
	GPIO_Configuration();		  
	//GPIO_SetBits(GPIOB , GPIO_Pin_13);
	//GPIO_SetBits(GPIOB , GPIO_Pin_15);
	FLASH_Unlock();
	UART_Init();
	BspTim2Init();
	
  SerialPutString("go into the bootloader\r\n");
  Main_Menu ();
	
	 
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
            {
                SerialPutString("Execute user Program\r\n");
                BspTim2Close();
							TIM_DeInit(TIM3);
              TIM_Cmd(TIM3, DISABLE);
							USART_DeInit(UART5);
              USART_Cmd(UART5,DISABLE);
							USART_DeInit(USART1);
              USART_Cmd(USART1,DISABLE);
              RCC_RTCCLKCmd(DISABLE);
                //跳转至用户代码
							addr = (unsigned char *)ApplicationAddress;
                JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
                Jump_To_Application = (pFunction) JumpAddress;
//							  for(i = 0;i<16;i++)
//									{
////										 SerialPutChar(*(volatile unsigned int*)(ApplicationAddress+4*i));
//										SerialPutChar(addr[i]);
//										SerialPutString("  ");
//										 
//									}
//									SerialPutString("\n");
                //初始化用户程序的堆栈指针
                __set_MSP(*(__IO uint32_t*) ApplicationAddress);
                Jump_To_Application();
            }
            else
            {
                SerialPutString("no user Program\r\n\n");
            }
	
  while(1); 
}