#ifndef HARDWARE_H
#define HARDWARE_H

#include "picoOled.h"

//Variables globales
extern t_OledParams oled;
extern uint32_t promedio_VL53L0X;
extern volatile bool boton_pulsado;

typedef enum {
    ESTADO_PUERTA_CERRADA,
    ESTADO_PUERTA_ABIERTA
} estado_t;

//VARIOS
#define PULS_PIN             (18)
#define DELAY_PULS           (30)

#define PUERTA_CERRADA       (20)       //grados de angulos
#define PUERTA_ABIERTA       (160)

//Mediciones
#define N_MEDICIONES         (10)     
#define DISTANCIA_OBJETIVO   (60)       //cm
#define DISTANCIA_MAX_VALIDA (200)  
#define DISTANCIA_LEJOS      (0xFFFF)

//I2C
#define I2C_SDA_PIN          (14)
#define I2C_SCL_PIN          (15)
#define OLED_ADDR            (0x3C)
#define VL53L0X_ADDR         (0x29)

//PWM
#define SIG_SERVO_PIN        (16)
#define SERVO_FREQ           (50)
#define SERVO_PERIODO_US     (1000000 / SERVO_FREQ)
#define SERVO_TOP            (59999)
#define SERVO_DIV            (50.0f)

//Funciones
void init_config();
void angulo_servo(uint8_t);
uint32_t distancia_promedio();
void mostrar(estado_t, uint32_t);
void puls_callback(uint, uint32_t);
static int64_t debounce_alarm_cb(alarm_id_t id, void *user_data);

#endif 
