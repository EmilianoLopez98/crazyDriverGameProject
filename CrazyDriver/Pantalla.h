#ifndef PANTALLA_H
#define PANTALLA_H

#include <U8g2lib.h>
#include "Globales.h"
#include "Sprites.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void dibujarJugador(int x) {
  //drawXBMP(posX, posY, ancho, alto, nombre_del_sprite)
  //El '54' es la altura (Y) fija donde está el coche en el suelo 
  u8g2.drawXBMP(x, 48, ALTO_SPRITE, ALTO_SPRITE, sprite_coche);
}

void dibujarObstaculo(int x, int y) {
  u8g2.drawXBMP(x, y, ANCHO_SPRITE, ALTO_SPRITE, sprite_cono);
}

void dibujarMenu(int opcionSeleccionada) {
  u8g2.setFont(u8g2_font_haxrcorp4089_tr); // Una fuente tipo pixel art
  u8g2.drawStr(30, 25, "COMENZAR");
  u8g2.drawStr(30, 45, "MARCADORES");

  //Movemos el puntero basados en una variable
  //Si es 0, y=20. Si es 1, y=40
  int punteroY = (opcionSeleccionada == 0) ? 25 : 45;
  u8g2.drawStr(15, punteroY, ">");
}

void dibujarIntro() {
  u8g2.clearBuffer(); // Siempre empezar limpiando
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  
  // Nombres del equipo
  u8g2.drawStr(40, 20, "DED Team");
  u8g2.drawStr(15, 35, "Diego, Emiliano y Dante");
  u8g2.drawStr(35, 55, "Presentan...");
  u8g2.sendBuffer(); // ¡Vital para que se vea!
  
  delay(3000);
  
  u8g2.clearBuffer();
  // Cambiamos a una fuente un poco más grande/negrita para el título
  u8g2.setFont(u8g2_font_ncenB10_tr); 
  u8g2.drawStr(3, 35, "CRAZY DRIVER");
  u8g2.sendBuffer();
  
  delay(2000);
  
  estadoActual = MENU; // Al final del intro, saltamos al menú
}

void dibujarScore(int score) {
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.setCursor(0, 7);
    u8g2.print("Score: ");
    u8g2.print(score);
}

void dibujarMarcadores() {
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(35, 12, "TOP SCORES");

  for (int i = 0; i < 4; i++) {
    u8g2.setCursor(40, 25 + (i * 10));
    u8g2.print(i + 1);
    u8g2.print(". ");
    u8g2.print(listaScores[i]);
  }

  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(1, 62, "BOTON ROJO PARA VOLVER");
}

#endif