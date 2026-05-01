#ifndef LOGICA_H
#define LOGICA_H

#include "Globales.h"

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

unsigned long tiempoUltimoObstaculo = 0;
const unsigned long invervaloAparicion = 1500; //1.5 segundos entre aparición de obstáculos

void intentarGenerarObstaculo() {
  unsigned long tiempoActual = millis();

  if(tiempoActual - tiempoUltimoObstaculo >= invervaloAparicion) {
    generarObstaculoAleatorio();
    tiempoUltimoObstaculo = tiempoActual; //Reasignamos el valor para poder repetir el ciclo
  }
}

void reiniciarJuego() {
  score = 0;
  playerX = 60;
  inicializarObstaculos(); //Desactivamos todos los obstáculos
  tiempoUltimoObstaculo = millis();
}

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
    else if (estadoActual == GAMEOVER) {
      reiniciarJuego();
      estadoActual = MENU;
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
  if (xInterno > 112) xInterno = 112;

  return xInterno;
}

void actualizarPosicionObstaculos() {
  for (int i = 0; i < 3; i++) {
    if (listaObstaculos[i].activo) {
      listaObstaculos[i].y += 2; //Velocidad de caída

      //Si sale por abajo de la pantalla (y > 64)
      if (listaObstaculos[i].y > 64) {
        listaObstaculos[i].activo = false; //Lo desactivamos para reusarlo
        score += 10; //¡10 puntos para Griffindor!
        sonarBuzzer(2000); //Pitido agudo y rápido de recompensa
      }
    }
  }
}

void comprobarColisiones() {
  for (int i = 0; i < 3; i++) {
    //Revisamos si el obstáculo está en la pantalla
    if (listaObstaculos[i].activo) {
      //Calculamos la distancia en X (hacemos uso de abs para que no importe si está a la izquierda o derecha)
      int distanciaX = abs(playerX - listaObstaculos[i].x);

      //Definir el umbral (hitbox)
      //Aunque miden 16, usaremos 12 para tener un margen
      bool choqueX = (distanciaX < 12);

      //Revisar el eje Y
      //El coche está en Y=48 y mide 16 de alto (ocupa de 48 a 64)
      //El cono choca si su parte baja entra en ese rango
      bool choqueY = (listaObstaculos[i].y + 12 > 48 && listaObstaculos[i].y < 60);

      if (choqueX && choqueY) {
        //CHOQUE
        sonarBuzzer(150);
        delay(500);
        estadoActual = GAMEOVER;
      };
    }
  }
}

#endif