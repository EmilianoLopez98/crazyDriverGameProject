#ifndef LOGICA_H
#define LOGICA_H

#include "Globales.h"

//Melodía al perder
int melodiaMuerte[] = {NOTE_C5, NOTE_G4, NOTE_E4, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3};
int duracionNotas[] = {150, 150, 150, 150, 150, 150, 300};
unsigned long tiempoUltimaNota = 0;
int notaActual = 0;

void reproducirMusicaMuerte() {
  for (int i = 0; i < 7; i++) {
    tone(PIN_BUZZER, melodiaMuerte[i], duracionNotas[i]);
    delay(duracionNotas[i] * 1.3); //Usamos delay porque el juego ya terminó
  };
  noTone(PIN_BUZZER);
}

void musicaMenu() {
  if (estadoActual != MENU) return; //Solo suena el menu

  unsigned long tiempoActual = millis();
  int melodia[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};

  if (tiempoActual - tiempoUltimaNota > 250) { //Velocidad de la nota
    tone(PIN_BUZZER, melodia[notaActual], 100);
    notaActual = (notaActual + 1) % 4; //Ciclo de 4 notas
    tiempoUltimaNota = tiempoActual;
  };
}

void musicaCrazyDriver() {
  if (estadoActual != JUGANDO) return;

  unsigned long tiempoActual = millis();
  int melodia[] = {NOTE_C4, NOTE_E4};

  if (tiempoActual - tiempoUltimaNota > 250) { //Velocidad de la nota
    tone(PIN_BUZZER, melodia[notaActual], 100);
    notaActual = (notaActual + 1) % 4; //Ciclo de 4 notas
    tiempoUltimaNota = tiempoActual;
  };
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

unsigned long tiempoUltimoObstaculo = 0;
const unsigned long invervaloAparicion = 800; //1.5 segundos entre aparición de obstáculos

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
      else if (opcionSeleccionada == 1) estadoActual = MARCADORES;
    }
    else if (estadoActual == PAUSA) {
      reiniciarJuego();
      estadoActual = MENU;
    }
    else if (estadoActual == GAMEOVER || estadoActual == MARCADORES) {
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
        sonarBuzzer(2000);
      }
    }
  }
}

int listaScores[4] = {0, 0, 0, 0};

void cargarScores() {
  File file = SD.open("/records.json");
  if (!file) {
    Serial.println("No hay archivo JSON, usando ceros.");
    return;
  }

  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, file);

  if (!error) {
    JsonArray arr = doc["scores"];
    for (int i = 0; i < 4; i++) {
      listaScores[i] = arr[i] | 0;
    }
    highScore = listaScores[0];
  }
  file.close();
}

void guardarPuntaje(int nuevoPuntaje) {
  // 1. Lógica de inserción y ordenamiento
  if (nuevoPuntaje > listaScores[3]) {
    listaScores[3] = nuevoPuntaje;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3 - i; j++) {
        if (listaScores[j] < listaScores[j + 1]) {
          int temp = listaScores[j];
          listaScores[j] = listaScores[j + 1];
          listaScores[j + 1] = temp;
        }
      }
    }
  }

  // 2. Crear el JSON (Corregido para ArduinoJson V7)
  JsonDocument doc;
  JsonArray scoresNode = doc["scores"].to<JsonArray>(); 
  
  for (int i = 0; i < 4; i++) {
    scoresNode.add(listaScores[i]);
  }

  // 3. Escribir en SD
  File file = SD.open("/records.json", FILE_WRITE);
  if (file) {
    serializeJson(doc, file);
    file.close();
    highScore = listaScores[0];
    Serial.println("JSON guardado correctamente.");
  } else {
    Serial.println("Error al abrir archivo para escribir.");
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
        guardarPuntaje(score);
        reproducirMusicaMuerte();

        estadoActual = GAMEOVER;

        if(score > highScore) {
          highScore = score;
        }
        delay(500);
      };
    }
  }
}

#endif