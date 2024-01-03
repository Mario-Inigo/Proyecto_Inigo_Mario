/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"

#define WAIT_TIME_MS 500 

// Definición de pines y componentes
AnalogIn Galga(A0);  // Pin analógico para la galga
DigitalIn botonTara(D2);   // Botón para tarar
DigitalIn botonReset(D3); // Botón de reset
DigitalOut ledRojo(D4);      // LED rojo
DigitalOut ledVerde(D5);    // LED verde
DigitalOut Alarma(D6);    // Alarma
Grove_LCD_RGB_Backlight Pantalla(PB_9,PB_8); // Pantalla

// Estados de la máquina
enum Estado {
    REPOSO,
    CALIBRANDO,
    MIDIENDO,
    ALARMA
};

// Variables 
float voltajeBase = 0.0;  // Voltaje de base sin peso
float pesoCalibrado = 0.0;  // Peso calibrado
const float limitePeso = 100.0;  // Límite de peso en gramos

// Variables para el temporizador de la alarma
Timer temporizadorAlarma;
const float tiempoLimiteAlarma = 3.0;  // Tiempo límite para desactivar la alarma en segundos

// Función para mostrar mensajes en la pantalla LCD (simulado)
void mostrarMensaje() {
  
  Pantalla.setRGB(0xff, 0xff, 0xff);                

    while (true)
    {
        Pantalla.locate(0,0); // Lo siguiente que se mande al LCD en primera fila, primera columna
        Pantalla.print("El peso es:"); // Escribe un texto fijo
        temp=calculaTemp(ntc.read());   // Convierte el valor del adc en temperatura
        sprintf(cadena,"%.2f ",temp); // Calcula la cadena para visualizar la temperatura en el LCD
        Pantalla.locate(0,1); // Lo siguiente lo escribe en la segunda fila, primera columna
        Pantalla.print(cadena); // Manda el texto formateado al LCD
        thread_sleep_for(WAIT_TIME_MS); // Espera medio segundo.
    }
}

// Función para manejar el estado de parpadeo del LED rojo
void parpadearLED(Estado &estadoActual) {
    while (estadoActual == CALIBRANDO || estadoActual == ALARMA) { 
        ledRojo = !ledRojo;       // Alternar estado del LED
        wait(0.2);        // Esperar 0.2 segundos
    }
}

// Función para manejar el estado de parpadeo de la alarma
void Alarmando(Estado &estadoActual) {
    while (estadoActual == ALARMA) { 
        Alarma = !Alarma;       // Alternar estado del LED
        wait(0.25);        // Esperar 0.2 segundos
    }
}


// Función principal
int main() {
    Estado estadoActual = REPOSO;
    temporizadorAlarma.start();
    
    while (1) {
        switch (estadoActual) {
            case REPOSO:
                // Estado de reposo
                ledRojo = 1;  // Enciende LED rojo
                ledVerde = 0;  // Apaga LED verde
                Alarma = 0;  // Apaga la alarma
                
                if (botonReset) {
                    Pantalla.setRGB(0xff, 0xff, 0xff);  
                    Pantalla.locate(0,0); // Lo siguiente que se mande al LCD en primera fila, primera columna
                    Pantalla.print("Retire cualquier peso y "); // Escribe un texto fijo
                    Pantalla.locate(0,1); // Lo siguiente lo escribe en la segunda fila, primera columna
                    Pantalla.print("presione el botón de tara"); // Escribe un texto fijo
                    thread_sleep_for(WAIT_TIME_MS); // Espera medio segundo

                    Pantalla.print("coloque 100g y presione calibrar"); // Escribe un texto fijo

                    estadoActual = CALIBRANDO;
                }
                wait(1); // Debounce para evitar cambios rápidos de estado
                break;

            case CALIBRANDO:
                // Estado de calibración
                parpadearLED(estadoActual);  // Parpadea el LED rojo
                ledVerde = 0;  // Apaga LED verde
                Alarma = 0;  // Apaga LED de alarma
                
                if (botonReset) {
                    // Si se vuelve a pulsar el boton de calibrar, se reinicia la máquina de estados
                    estadoActual = REPOSO;

                    Pantalla.setRGB(0xff, 0xff, 0xff);  
                    Pantalla.locate(0,0); // Lo siguiente que se mande al LCD en primera fila, primera columna
                    Pantalla.print("Reiniciando..."); // Escribe un texto fijo
                    Pantalla.locate(0,1); // Lo siguiente lo escribe en la segunda fila, primera columna
                    Pantalla.print("Pulsar boton reset"); // Escribe un texto fijo
                    thread_sleep_for(WAIT_TIME_MS); // Espera medio segundo.

                } else if (botonTara) {
                    // Calibrar presionado, realiza la calibración
                    float peso100g = Galga.read();
                    pesoCalibrado = 100.0 / (peso100g - voltajeBase);
                    estadoActual = REPOSO;
                    mostrarMensaje("Calibración exitosa"); 
                }
                wait(1); // Debounce para evitar cambios rápidos de estado
                break;

            case MIDIENDO:
                // Estado de medición
                ledRojo = 0;  // Apaga LED rojo
                ledVerde = 1;  // Enciende LED verde
                Alarma = 0;  // Apaga LED de alarma
                
                // Realiza mediciones y verifica el límite de peso
                float pesoActual = (Galga.read() - voltajeBase) * pesoCalibrado;
                
                if (pesoActual > limitePeso) {
                    estadoActual = ALARMA;
                    temporizadorAlarma.reset();  // Reinicia el temporizador de alarma
                    Pantalla.setRGB(0xff, 0xff, 0xff);  
                    Pantalla.locate(0,0); // Lo siguiente que se mande al LCD en primera fila, primera columna
                    Pantalla.print("Exceso de peso"); // Escribe un texto fijo
                }
                wait(1); // Debounce para evitar cambios rápidos de estado
                break;

            case ALARMA:
                // Estado de alarma
                parpadearLED(estadoActual);  // Parpadea el LED rojo
                ledVerde = 0;  // Apaga LED verde
                
                if (temporizadorAlarma.read() > tiempoLimiteAlarma) {
                    // Desactiva la alarma después de 3 segundos
                    estadoActual = REPOSO;
                    temporizadorAlarma.reset();  // Reinicia el temporizador de alarma
                    mostrarMensaje("Alarma desactivada automáticamente");
                    wait(0.5);
                } else {
                    // Activa y desactiva la alarma cada 0.25 segundos
                    Alarmando(estadoActual);
                }
                wait(1); // Debounce para evitar cambios rápidos de estado
                break;
        }
        
    }
    
}
