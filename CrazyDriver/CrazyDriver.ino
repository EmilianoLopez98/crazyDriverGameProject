#include "Globales.h"
#include "Sprites.h"
#include "Pantalla.h"
#include "Logica.h"

//Se le asigna memoria y valor inicial
Estado estadoActual = INTRO;
int playerX = 60;
int opcionSeleccionada = 0;
//Creamos un "pool" de 3 obstáculos máximo en pantalla para que sea fluido
Obstaculo listaObstaculos[3]; //El array real "vive" aquí
int score = 0;
int highScore = 0;

void setup() {
  Serial.begin(115200);

  u8g2.begin();

  pinMode(PIN_BTN_SEL, INPUT_PULLUP);
  pinMode(PIN_BTN_PAUSA, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);

  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Error SD");
  } else {
    cargarScores();
  }
}

void loop() {
  //"Escuchar" sensores y botones
  leerEntradas();

  //Borrar pantalla para nuevo cuadro
  u8g2.clearBuffer();

  //Qué dibujar según el estado
  if (estadoActual == INTRO) {
    dibujarIntro();
  }
  else if (estadoActual == MENU) {
    manejarNavegacionMenu(analogRead(PIN_JOY_Y)); //Eje vertical
    dibujarMenu(opcionSeleccionada);
    musicaMenu();
  }
  else if (estadoActual == MARCADORES) {
    dibujarMarcadores();
    musicaMenu();
  }
  else if (estadoActual == PAUSA) {
    u8g2.drawStr(35, 35, "PAUSA");
    u8g2.drawStr(1, 55, "BOTON ROJO PARA REINICIAR");
  }
  else if (estadoActual == GAMEOVER) {
    u8g2.drawStr(25, 15, "HAZ PERDIDO");
    u8g2.setCursor(40, 30);
    u8g2.print("Puntos: ");
    u8g2.print(score);
    u8g2.drawStr(1, 50, "BOTON ROJO PARA REINICIAR");
  }
  else if (estadoActual == JUGANDO) {
    actualizarPosicionObstaculos(); //Mover obstáculos
    intentarGenerarObstaculo(); //Ver si toca generar/reutilizar uno nuevo
    comprobarColisiones();

    int xJoy = analogRead(PIN_JOY_X); // Eje horizontal
    playerX = procesarJoystick(xJoy);
    
    dibujarJugador(playerX);

    for (int i = 0; i < 3; i++) {
      if (listaObstaculos[i].activo) {
        dibujarObstaculo(listaObstaculos[i].x, listaObstaculos[i].y);
      }
    };

    musicaCrazyDriver();

    dibujarScore(score);
  }

  //Mostrar todo al usuario
  u8g2.sendBuffer();
}