#include "stm32f10x.h"                  // Device header

void IWDG_Init(void)
{
    // ����LSIʱ��
    RCC_LSICmd(ENABLE);
    
    // �ȴ�LSI����
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    
    // ����IWDGԤ��Ƶ��
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_16);  // 40kHz/16 = 2.5kHz
    
    // ��������ֵ��Լ1�볬ʱ��
    // ��ʱʱ�� = (����ֵ+1)/Ƶ�� = (2499+1)/2500 = 1��
    IWDG_SetReload(2499);
    
    // 5. ι�����������Ź�
    IWDG_ReloadCounter();
    IWDG_Enable();
}

