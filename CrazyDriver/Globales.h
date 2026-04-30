#ifndef GLOBALES_H
#define GLOBALES_H

#include <Arduino.h>

//Pines
#define PIN_JOY_X 34
#define PIN_JOY_Y 36
#define PIN_BTN_SEL 32
#define PIN_BTN_PAUSA 33
#define PIN_BUZZER 13

//Estados del juego
enum Estado {INTRO, MENU, JUGANDO, PAUSA, GAMEOVER};
//Promesas. Cosas que existen en el juego, que le decimos al PC "buscalas"
extern Estado estadoActual;
extern int playerX;
extern int opcionSeleccionada;
//También lista de obstáculos para que el .ino pueda dibujarlos
struct Obstaculo {
  int x;
  int y;
  bool activo;
};
extern Obstaculo listaObstaculos[3];

#endif