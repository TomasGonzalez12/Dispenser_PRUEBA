#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware.h"
#include "picoOled.h"
#include "tof.h"
#include "systick.h"

int main(){
    init_config();

    estado_t estado = ESTADO_PUERTA_CERRADA;
    int distancia = DISTANCIA_LEJOS;
    bool bowl_lleno = false;
    uint32_t t_cierre = 0;          // instante (ms de systick) en que se cerró la puerta

    angulo_servo(PUERTA_CERRADA);
    boton_pulsado = false;
    mostrar(estado, distancia);

    while (true){
        switch (estado)
        {
        case ESTADO_PUERTA_CERRADA:
            //distancia = distancia_promedio();
            distancia = tofReadDistance();

            // SIMULACIÓN: pulsar con la puerta cerrada = el bowl bajó del peso objetivo
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
            if (boton_pulsado)
            {
                boton_pulsado = false;
                angulo_servo(PUERTA_CERRADA);
                bowl_lleno = true;
                t_cierre = get_systick();
                estado = ESTADO_PUERTA_BLOQUEADA;
                mostrar(estado, distancia);
            }
            break;

        case ESTADO_PUERTA_BLOQUEADA:
            if (boton_pulsado)
            {
                boton_pulsado = false;
                bowl_lleno = false;
            }

            // La resta sin signo es correcta aunque el contador de ms desborde
            if ((get_systick() - t_cierre) >= TIEMPO_BLOQUEO_MS)
            {
                estado = ESTADO_PUERTA_CERRADA;
                mostrar(estado, distancia);
            }
            break;
        }

        sleep_ms(20);
    }
}