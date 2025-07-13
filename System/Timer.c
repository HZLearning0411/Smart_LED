#include "stm32f10x.h"                  // Device header
#include "system.h"
#include "OLED.h"

uint8_t timeout_counter;        //超时计数器，单位为s

extern volatile SystemMode current_mode;  // 声明外部引用

/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM3的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM3为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);					//开启TIM3的更新中断
	
	/*NVIC中断分组*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//选择配置NVIC的TIM3线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM3, DISABLE);			//暂时不开启TIM3
}

/**
  * 函    数：启动/重设超时计数器
  * 参    数：无
  * 返 回 值：无
  */
void Reset_Timeout_Timer(void) {
    current_mode = MANUAL_MODE;
    OLED_ShowString(3, 5, "MA");
    timeout_counter = 5;        //设置为5s超时
    TIM_SetCounter(TIM3, 0);    //从0开始计数，每次调用定时器中断，timeout_counter自减
    TIM_Cmd(TIM3, ENABLE);
}


/*  
 *  函    数：定时器TIM3中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：预期频率 = 72MHz / ((ARR+1) * (PSC+1))
  *         定时器中断周期 = 1 / 预期频率
  *         本中断函数周期为1s
  */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        if(timeout_counter > 0)
        {
            timeout_counter--;              //自减计数
            OLED_ShowNum(4, 6, timeout_counter, 2);
            if(timeout_counter == 0)
            {
                current_mode = AUTO_MODE;   // 更改状态机
                OLED_ShowString(3, 5, "AU");
                TIM_Cmd(TIM3, DISABLE);     // 停止定时器
            }
        }
		
	}
}
