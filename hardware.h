#ifndef HARDWARE_H
#define HARDWARE_H

#include "picoOled.h"

//PINES
#define I2C_SDA_PIN          (14)
#define I2C_SCL_PIN          (15)
#define SIG_SERVO_PIN        (16)
#define PULS_PIN             (18)


//PUERTA
typedef enum {
    ESTADO_PUERTA_CERRADA,
    ESTADO_PUERTA_ABIERTA,
    ESTADO_PUERTA_BLOQUEADA
} estado_t;

#define PUERTA_CERRADA       (20)       //grados de angulos
#define PUERTA_ABIERTA       (160)
#define TIEMPO_BLOQUEO_MS    (5UL * 60UL * 1000UL)   // 5 min; para probar, 10000 (10 s)


//Mediciones
#define N_MEDICIONES         (10)     
#define DISTANCIA_OBJETIVO   (60)       //cm
#define DISTANCIA_MAX_VALIDA (200)  
#define DISTANCIA_LEJOS      (0xFFFF)


//OLED
#define OLED_ADDR            (0x3C)
extern t_OledParams oled;


//VL53L0X
#define VL53L0X_ADDR         (0x29)
extern uint32_t promedio_VL53L0X;


//PWM
#define SERVO_FREQ           (50)
#define SERVO_PERIODO_US     (1000000 / SERVO_FREQ)
#define SERVO_TOP            (59999)
#define SERVO_DIV            (50.0f)


//PULSADOR
#define DELAY_PULS           (30)
extern volatile bool boton_pulsado;


//Funciones
void init_config();
void angulo_servo(uint8_t);
uint32_t distancia_promedio();
void mostrar(estado_t, uint32_t);
void puls_callback(uint, uint32_t);
static int64_t debounce_alarm_cb(alarm_id_t id, void *user_data);

#endif 
