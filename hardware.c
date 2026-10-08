#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware.h"
#include "tof.h"
#include "systick.h"

//Variable globales
t_OledParams oled;
static uint slice;  //del PWM
static uint channel; //PWM

void init_config(){
    stdio_init_all();
    init_systick();

    //BOTON
    gpio_init(PULS_PIN);
    gpio_set_dir(PULS_PIN, GPIO_IN);
    gpio_pull_up(PULS_PIN);
    gpio_set_input_hysteresis_enabled(PULS_PIN, true);

    gpio_set_irq_enabled_with_callback(PULS_PIN, GPIO_IRQ_EDGE_FALL, true, puls_callback);
    
    // Config OLED
    oled.i2c = i2c1;
    oled.SDA_PIN = I2C_SDA_PIN;
    oled.SCL_PIN = I2C_SCL_PIN;

    oled.ctlrType = CTRL_SH1106;
    oled.i2c_address = OLED_ADDR;
    oled.height = H_64;
    oled.width = W_132;      

    oledI2cConfig(&oled);

    oledSetTTYMode(&oled, true);
    oledSet_invert(&oled, false);

    // Config VL53L0X

    if (!tofInit(1, VL53L0X_ADDR, 0))
    {
        printf("Error inicializando VL53L0X\n");
    }
    else
    {
        printf("VL53L0X inicializado correctamente\n");
    }
    
    
    //Config PWM
    gpio_set_function(SIG_SERVO_PIN, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(SIG_SERVO_PIN);
    channel = pwm_gpio_to_channel(SIG_SERVO_PIN);
    pwm_set_clkdiv(slice, SERVO_DIV);
    pwm_set_wrap(slice, SERVO_TOP);
    
    pwm_set_enabled(slice, true);
}


//Servo
#define SERVO_MIN_US 600
#define SERVO_MAX_US 2400

void angulo_servo(uint8_t angulo)
{
    uint pulso_us =
        SERVO_MIN_US +
        (angulo * (SERVO_MAX_US - SERVO_MIN_US) / 180);

    uint16_t level =
        (pulso_us * (SERVO_TOP + 1)) / SERVO_PERIODO_US;

    pwm_set_chan_level(slice, channel, level);
}

//VL53L0X
uint32_t distancia_promedio(void){
    uint32_t suma = 0;
    uint32_t validas = 0;

    for (int i = 0; i < N_MEDICIONES; i++)
    {
        uint32_t d = tofReadDistance();

        if (d > 0 && d <= DISTANCIA_MAX_VALIDA)
        {
            suma += d;
            validas++;
        }
    }

    // Si menos de la mitad de las lecturas son válidas, se considera "lejos"
    if (validas * 2 <= N_MEDICIONES)
    {
        return DISTANCIA_LEJOS;
    }

    return suma / validas;
}

//OLED
void mostrar(estado_t estado, uint32_t distancia){
    oledClear(&oled, BLACK);

    if (distancia == DISTANCIA_LEJOS)
        oledPrintfXy(&oled, 0, 0, "Dist: ---");
    else
        oledPrintfXy(&oled, 0, 0, "Dist: %lu mm", (unsigned long)distancia);

    switch (estado)
    {
        case ESTADO_PUERTA_ABIERTA: oledPrintfXy(&oled, 0, 20, "Puerta: ABIERTA"); break;
        case ESTADO_PUERTA_BLOQUEADA: oledPrintfXy(&oled, 0, 20, "Puerta: BLOQUEO"); break;
        default:                    oledPrintfXy(&oled, 0, 20, "Puerta: CERRADA"); break;
    }

    oledDisplay(&oled);
}

//Botón, gracias Claudio
volatile bool boton_pulsado = false;

static int64_t debounce_alarm_cb(alarm_id_t id, void *user_data)
{
    if (!gpio_get(PULS_PIN)) boton_pulsado = true;
    gpio_acknowledge_irq(PULS_PIN, GPIO_IRQ_EDGE_FALL);
    gpio_set_irq_enabled(PULS_PIN, GPIO_IRQ_EDGE_FALL, true);
    return 0;   // no se repite
}

void puls_callback(uint gpio, uint32_t event_mask)
{
    if (gpio != PULS_PIN) return;
    gpio_set_irq_enabled(PULS_PIN, GPIO_IRQ_EDGE_FALL, false);
    add_alarm_in_ms(DELAY_PULS, debounce_alarm_cb, NULL, true);
}