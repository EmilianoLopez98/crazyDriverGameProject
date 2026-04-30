#include "Globales.h"
#include "Sprites.h"
#include "Pantalla.h"
#include "Logica.h"

//Se le asigna memoria y valor inicial
Estado estadoActual = MENU;
int playerX = 60;
int opcionSeleccionada = 0;
//Creamos un "pool" de 3 obstáculos máximo en pantalla para que sea fluido
Obstaculo listaObstaculos[3]; //El array real "vive" aquí

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  pinMode(PIN_BTN_SEL, INPUT_PULLUP);
  pinMode(PIN_BTN_PAUSA, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
}

void loop() {
  //"Escuchar" sensores y botones
  leerEntradas();

  //Borrar pantalla para nuevo cuadro
  u8g2.clearBuffer();

  //Qué dibujar según el estado
  if (estadoActual == MENU) {
    manejarNavegacionMenu(analogRead(PIN_JOY_Y)); //Eje vertical
    dibujarMenu(opcionSeleccionada);
  }
  else if (estadoActual == JUGANDO) {
    actualizarPosicionObstaculos(); //Mover obstáculos
    intentarGenerarObstaculo(); //Ver si toca generar/reutilizar uno nuevo

    int xJoy = analogRead(PIN_JOY_X); // Eje horizontal
    playerX = procesarJoystick(xJoy);
    
    dibujarJugador(playerX);

    for (int i =0; i < 3; i++) {
      if (listaObstaculos[i].activo) {
        dibujarObstaculo(listaObstaculos[i].x, listaObstaculos[i].y);
      }
    }
  }
  else if (estadoActual == PAUSA) {
    u8g2.drawStr(40, 32, "PAUSA");
  }

  //Mostrar todo al usuario
  u8g2.sendBuffer();

  Serial.println("");
  Serial.println("");
  Serial.print("Joystick X: ");
  Serial.println(analogRead(PIN_JOY_X));
  Serial.print("Joystick Y: ");
  Serial.println(analogRead(PIN_JOY_Y));
}