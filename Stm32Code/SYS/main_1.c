#include "sys.h"
#include "usart3.h"

SENSOR SensorData;//????????
THRESHOLD Threshold;//???????
SYSTEM System;//??????????

void mqttPublic(void);
void Threshold_Init(THRESHOLD *Threshold);
static u8 Read_Soil_Moisture_Stable(void);
static u8 Network_Ready(void);
static void Network_Task(void);

// ??? ADC ????????
u8 get_soil_moisture(u16 adc_val)
{
    // ???:?????????ADC?4000,?????1200 (?????????????)
    int val = (4000 - adc_val) * 100 / (4000 - 1200);
    if(val > 100) val = 100;
    if(val < 0) val = 0;
    return (u8)val;
}

// Soil moisture display/control stability:
// 1. Smooth the ADC input to suppress probe noise.
// 2. Apply a small deadband so OLED does not flicker on +/-1% jitter.
static u8 Read_Soil_Moisture_Stable(void)
{
    static u16 filtered_adc = 0;
    static u8 stable_percent = 0;
    static u8 initialized = 0;
    u16 adc_now;
    u8 percent_now;

    adc_now = Get_Adc_Average(1, 8);

    if (!initialized) {
        filtered_adc = adc_now;
        stable_percent = get_soil_moisture(adc_now);
        initialized = 1;
        return stable_percent;
    }

    filtered_adc = (filtered_adc * 3 + adc_now) / 4;
    percent_now = get_soil_moisture(filtered_adc);

    if (percent_now > stable_percent) {
        if ((percent_now - stable_percent) >= 2) {
            stable_percent = percent_now;
        }
    } else if (stable_percent > percent_now) {
        if ((stable_percent - percent_now) >= 2) {
            stable_percent = percent_now;
        }
    }

    return stable_percent;
}

int main(void)
{
    delay_init();//???????
    TIM4_Init(4000, 7200 - 1); //???4???
    TIM2_Init(499, 7199); //???2??? ?????? ???????????
    NVIC_Config(); //???????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    My_USART1();//??1??? (?????) ???:9600
    My_USART2(); //??2??? (?ESP8266???)???:115200
    RELAY_GPIO_Config();//??????
    Adc_Init();//adc???,??????
    oled_Init();//oled???
   
    wifi_GPIO_Config();//ESP8266???????
    Ali_MsessageInit();//?????????????
    Beep_Init();//??????
    LED_GPIO_Config();//LED????
    DHT11_Init();//?????????
    KEY_Init();//?????

    // ???????????????????????
    Threshold_Init(&Threshold);//?????????????????

    oled_Clear();
    isKey1=isKey2=isKey3=isKey4=0;

    /*******************************************/
    while (1) {
        KeyScan(); // ????????????

        DHT11_Read_TempAndHumidity(&dht11Data); //?????;

        // ??????????????
        SensorData.SoliVal = Read_Soil_Moisture_Stable();
        // ??????????
        SensorData.LightVal = Get_Adc_Average(5, 5) / 41;

        Mode_Decide();//???? (?????????)

        // Keep local buttons/display responsive. Network work runs after local UI
        // and reconnects with a delay when WiFi/MQTT is unavailable.
        Network_Task();

        // Publish data only after MQTT connect and subscribe packets finish.
        if (System.mqttflag && Network_Ready()) {
            mqttPublic();//?MQTT???????
            System.mqttflag = 0;
        }

        delay_ms(50); // ??????????????,??????????
    }
}

static u8 Network_Ready(void)
{
    return (Connect_flag && ConnectPack_flag && SubscribePack_flag);
}

static void Network_Task(void)
{
    static u16 retry_delay = 200;

    if (Connect_flag) {
        mqtt_Content();
        retry_delay = 200;
        return;
    }

    if (retry_delay > 0) {
        retry_delay--;
        return;
    }

    mqtt_Content();

    if (!Connect_flag) {
        retry_delay = 600;
    }
}

//?MQTT???????
void mqttPublic(void)
{
    char Data[255];
    /*****???????JSON???????*****/
    // ????:?? SensorData ????????,???????? 100- ??
    sprintf(Data,
            "{\"sensor1\":%d,\"sensor2\":%d,\"sensor3\":%d,\"sensor4\":%d,\"sensor5\":%d,\"sensor6\":%d,\"mode\":%d,\"pump\":%d,\"fan\":%d,\"light\":%d}",
            dht11Data.temp_int, SensorData.SoliVal, SensorData.LightVal
            , Threshold.TempMax,  Threshold.SoliMin, Threshold.LightMin
            , OperateMode, System.Switch2, System.Switch1, System.Switch3
           );
    mqtt_PublishQs0(P_TOPIC_NAME, Data, strlen(Data)); //????,???:Topic,JSON????,????
}

//?????????????????
void Threshold_Init(THRESHOLD *Threshold)
{
    Threshold->TempMax = 34;
    Threshold->HumiMin = 25;
    Threshold->SoliMin = 40;
    Threshold->LightMin = 50;
  
    System.Switch1 = 0;
    System.Switch2 = 0;
    System.Switch3 = 0;
    System.Switch4 = 0;
    System.Switch5 = 0;
    System.Switch6 = 0;
    System.mqttflag = 0;
    System.ClearFlag = 0;
}
