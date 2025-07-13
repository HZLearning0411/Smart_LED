#include "stm32f10x.h"                  // Device header

void IWDG_Init(void)
{
    // 开启LSI时钟
    RCC_LSICmd(ENABLE);
    
    // 等待LSI就绪
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    
    // 配置IWDG预分频器
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_16);  // 40kHz/16 = 2.5kHz
    
    // 设置重载值（约1秒超时）
    // 超时时间 = (重载值+1)/频率 = (2499+1)/2500 = 1秒
    IWDG_SetReload(2499);
    
    // 5. 喂狗并启动看门狗
    IWDG_ReloadCounter();
    IWDG_Enable();
}

