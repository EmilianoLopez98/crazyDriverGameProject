#ifndef LOGICA_H
#define LOGICA_H

#include "Globales.h"

void leerEntradas() {
  //Lectura del botón de pausa
  if (digitalRead(PIN_BTN_PAUSA) == LOW) {
    delay(200); //Debounce
    if (estadoActual == JUGANDO) {
      estadoActual = PAUSA;
      //Aquí podríamos poner un sonido corto para indicar pausa
    }
    else if (estadoActual == PAUSA) {
      estadoActual = JUGANDO;
    }
  }

  //Lectura botón de selección (para el menú)
  if (digitalRead(PIN_BTN_SEL) == LOW) {
    delay(200); //Debounce
    if (estadoActual == MENU) {
      if (opcionSeleccionada == 0) estadoActual = JUGANDO;
      else if (opcionSeleccionada == 1) {
        //Ir a pantalla de scores, pendiente
      }
    }
  }
}

void sonarBuzzer(int frec) {
  tone(PIN_BUZZER, frec, 100);
}

bool joystickMovido = false; //Bandera para evitar saltos infinitos

void manejarNavegacionMenu(int lecturaY) {
  //Si el joystick se mueve hacia abajo y no estaba marcado como movido 
  if (lecturaY > 3000 && !joystickMovido) {
    opcionSeleccionada = 1;
    joystickMovido = true; //Bloqueamos hasta que regresa al centro el joystick
    sonarBuzzer(500);
  }
  //Si el joystick se mueve hacia arriba
  else if (lecturaY < 1000 && !joystickMovido) {
    opcionSeleccionada = 0;
    joystickMovido = true;
    sonarBuzzer(500);
  }
  //Zona muerta: Si el joystick regresa al centro, liberamos la bandera
  else if (lecturaY > 1500 && lecturaY < 2500) {
    joystickMovido = false;
  }
}

int procesarJoystick(int lectura) {
  static int xInterno = 60;
  //Zona muerta. Solo se mueve si se aleja del centro (2048)
  if (lectura > 2800) xInterno += 3;
  if (lectura < 1200) xInterno -= 3;

  //Limites (evitar que el psj salga de la pantalla)
  if (xInterno < 0) xInterno = 0;
  if (xInterno > 116) xInterno = 116;

  return xInterno;
}

void inicializarObstaculos() {
  for (int i = 0; i < 3; i++) {
    listaObstaculos[i].activo = false;
  }
}

void generarObstaculoAleatorio() {
  for (int i = 0; i < 3; i++) {
    //Buscamos uno que no esté siendo usado
    if (!listaObstaculos[i].activo) {
      listaObstaculos[i].x = random(0, 112); //El límite es 112 por las dimensiones con las que diseñamos el obstaculo
      listaObstaculos[i].y = -16;
      listaObstaculos[i].activo = true;
      break; //Para generar uno a la vez
    }
  }
}

void actualizarPosicionObstaculos() {
  for (int i = 0; i < 3; i++) {
    if (listaObstaculos[i].activo) {
      listaObstaculos[i].y += 2; //Velocidad de caída
    }

    //Si sale por abajo de la pantalla (y > 64)
    if (listaObstaculos[i].y > 64) {
      listaObstaculos[i].activo = false; //Lo desactivamos para reusarlo
    }
  }
}

unsigned long tiempoUltimoObstaculo = 0;
const unsigned long invervaloAparicion = 1500; //1.5 segundos entre aparición de obstáculos

void intentarGenerarObstaculo() {
  unsigned long tiempoActual = millis();

  if(tiempoActual - tiempoUltimoObstaculo >= invervaloAparicion) {
    generarObstaculoAleatorio();
    tiempoUltimoObstaculo = tiempoActual; //Reasignamos el valor para poder repetir el ciclo
  }
}

#endif