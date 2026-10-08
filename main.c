#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware.h"
#include "picoOled.h"
#include "tof.h"


//Variables globales
uint32_t promedio_VL53L0X = 0;

int main(){
    //Inicializaciones
    init_config();

    estado_t estado = ESTADO_PUERTA_CERRADA;
    uint32_t distancia = DISTANCIA_LEJOS;
    bool bowl_lleno = false;

    angulo_servo(PUERTA_CERRADA);
    boton_pulsado = false;     
    mostrar(estado, distancia);
    
    //Maquina de estados
    while (true){
        switch (estado)
        {
        case ESTADO_PUERTA_CERRADA:
            distancia = distancia_promedio();

            // SIMULACIÓN: pulsar con la puerta cerrada = la mascota comió
            // y el bowl bajó del peso objetivo.
            if (boton_pulsado)
            {
                boton_pulsado = false;
                bowl_lleno = false;
            }

            if (distancia < DISTANCIA_OBJETIVO && !bowl_lleno)
            {
                boton_pulsado = false;
                angulo_servo(PUERTA_ABIERTA);
                estado = ESTADO_PUERTA_ABIERTA;
            }

            mostrar(estado, distancia);
            break;

        case ESTADO_PUERTA_ABIERTA:
            // Cierra al recibir el OK de la balanza (pulsador),
            // sin importar la distancia actual.
            if (boton_pulsado)
            {
                boton_pulsado = false;
                angulo_servo(PUERTA_CERRADA);
                bowl_lleno = true;
                estado = ESTADO_PUERTA_CERRADA;
                mostrar(estado, distancia);
            }
            break;
        }

        sleep_ms(20);
    }
}