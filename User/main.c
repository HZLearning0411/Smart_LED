#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "AD.h"
#include "Encoder.h"
#include "system.h"
#include "Timer.h"

volatile SystemMode current_mode;

uint8_t AD_Value;   //光敏电阻AD采集值
static uint16_t current_brightness = 0; //当前LED亮度

int main(void)
{
	/*模块初始化*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //配置NVIC为分组2
	OLED_Init();		
	PWM_Init();			
	AD_Init();
    Encoder_Init();
    Timer_Init();
   
    OLED_ShowString(1, 1, "LED:000%");			//1行1列显示字符串LED亮度
    OLED_ShowString(2, 1, "Light:");            //外部光线强度参考值
    OLED_ShowString(3, 1, "MODE:AU");			    //3行1列显示字符串ExLight:（外部光线亮度）
    OLED_ShowString(4, 1, "Time:");             //模式切换倒计时

    
	while (1)
	{
        AD_Value = AD_GetValue();
        uint16_t target_brightness = ((AD_GetValue()-1200) * 100) / 4095;      
        OLED_ShowNum(2, 7, 4095 - AD_GetValue(), 4);    //2行7列显示光敏电阻参考值        
        OLED_ShowNum(1, 5, current_brightness , 3);		//1行5列显示当前LED亮度        
        
        if(current_mode == AUTO_MODE)
        {
            int16_t diff_ls = target_brightness - current_brightness;  //获取差值，实现亮度平滑过度
            if(diff_ls > 5) current_brightness += 5;
            else if(diff_ls < -5) current_brightness -= 5;
            else current_brightness = target_brightness;           
        }
        else
        {
            current_brightness = Encoder_Get(current_brightness);
        }
        PWM_SetCompare3(current_brightness);
	}
}


