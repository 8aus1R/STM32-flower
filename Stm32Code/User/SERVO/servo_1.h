
#include "sys.h"



#define SG90_GPIO_PIN   GPIO_Pin_9
#define SG90_GPIO_PORT  GPIOB
#define SG90_GPIO_CLK  RCC_APB2Periph_GPIOB

#define SG90_GPIO_PIN2   GPIO_Pin_8
#define SG90_GPIO_PORT2  GPIOB
#define SG90_GPIO_CLK2  RCC_APB2Periph_GPIOB

void SG90_Init(void);
void Control_SG90(uint32_t us);
void Control2_SG90(uint32_t us);

