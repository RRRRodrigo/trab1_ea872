#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

#include "oo_model.hpp"

#include <ncurses.h>
#include <cstdlib>

#define MAX_Y 20
#define MAX_X 2000

using namespace std::chrono;

Corpo::Corpo(float massa, float velocidade, float posicao) {
  this->massa = massa;
  this->velocidade = velocidade;
  this->posicao = posicao;
}

void Corpo::update(float nova_velocidade, float nova_posicao) {
  this->velocidade = nova_velocidade;
  this->posicao = nova_posicao;
}

float Corpo::get_massa() {
  return this->massa;
}

float Corpo::get_velocidade() {
  return this->velocidade;
}

float Corpo::get_posicao() {
  return this->posicao;
}

ListaDeCorpos::ListaDeCorpos() {
  this->corpos = new std::vector<Corpo *>(0);
}

void ListaDeCorpos::hard_copy(ListaDeCorpos *ldc) {
  std::vector<Corpo *> *corpos = ldc->get_corpos();
  for (int k=0; k<corpos->size(); k++) {
    Corpo *c = new Corpo( (*corpos)[k]->get_massa(),\
                          (*corpos)[k]->get_posicao(),\
                          (*corpos)[k]->get_velocidade()
                        );
    this->add_corpo(c);
  }
}

void ListaDeCorpos::add_corpo(Corpo *c) {
  (this->corpos)->push_back(c);
}

std::vector<Corpo*> *ListaDeCorpos::get_corpos() {
  return (this->corpos);
}

Projetil::Projetil(){ //Cria o mapa de projeteis
  this->mapa = (char**)malloc(MAX_Y * sizeof(char*));
  for(int i=0;i<MAX_Y;i++){
    this->mapa[i] = (char*)malloc(MAX_X * sizeof(char));
  }
}

void Projetil::gerar(float chance){ //O mapa e gerado colocando obstaculos aleatorios nele. Um numero aleatorio e adquirido para cada posicao do mapa, com uma chance de 1% de colocar um obstaculo naquela posicao do mapa.
    // RAND_MAX
    int i = 0, j = 0;
    int alea;

    for(i=0;i<MAX_Y;i++){
        for(j=0;j<MAX_X;j++){
            alea = rand();
            if(alea >= (RAND_MAX * chance))
                this->mapa[i][j] = '#';
            else
                this->mapa[i][j] = ' ';
        }
    }
}

bool Projetil::update(Corpo* corpo){ //A funcao de update atualiza as posicoes do mapa e checa se o jogador cometeu uma colisao ou nao. Caso o jogador tenha atingido um obstaculo, a funcao retorna 1 e o jogo e terminado na main.
  //mover tudo pra esquerda
  for(int i=0;i<MAX_Y;i++){
    if(mapa[i][0] == '#');
	  mapa[i][0] = ' ';
  }
  for(int i=0;i<MAX_Y;i++){
     for(int j=0;j<MAX_X-1;j++){
        if(mapa[i][j+1] == '#')
          this->mapa[i][j] = '#';
          this->mapa[i][j+1] = 'X';
        if(mapa[i][j] == 'X')
          mapa[i][j] = ' ';
      }
    }
  int pos_player = corpo->get_posicao();
  if(mapa[pos_player][0] == '#')
    return 1;
  return 0;
}

Fisica::Fisica(ListaDeCorpos *ldc) {
  this->lista = ldc;
}

void Fisica::update(float deltaT) {
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();
  
  for (int i = 0; i < (*c).size(); i++) {
    float new_vel = (*c)[i]->get_velocidade() - ((*c)[i]->get_velocidade()/3);
    float new_pos = (*c)[i]->get_posicao() + (float)deltaT * new_vel/1000;
    (*c)[i]->update(new_vel, new_pos);
  }
}

void Fisica::salto(float direcao) { //funcao nao implementada no projeto final
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();

  for (int i = 0; i < (*c).size(); i++) {
    float new_vel = (*c)[i]->get_velocidade() + (160 * direcao);
    float new_pos = (*c)[i]->get_posicao();
      if(new_pos > 19){
        new_pos = 19;
        new_vel = 0;
      }
      if(new_pos < 0){
        new_pos = 0;
        new_vel = 0;
      }
    (*c)[i]->update(new_vel, new_pos);
  }
}

void Fisica::step(float direcao) { //Move o jogador, a direcao negativa move o jogador para baixo e vice-versa
  std::vector<Corpo *> *c = this->lista->get_corpos();

  float new_pos = (*c)[0]->get_posicao() - 1*(direcao);
  if(new_pos > 19){
      new_pos = 19;
    }
  if(new_pos < 0)
    new_pos = 0;
  (*c)[0]->update(0, new_pos);
}

Tela::Tela(ListaDeCorpos *ldc, int maxI, int maxJ, float maxX, float maxY) {
  this->lista = ldc;
  this->lista_anterior = new ListaDeCorpos();
  this->lista_anterior->hard_copy(this->lista);
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
}

void Tela::update(char** mapa){ 
  int i, k;
  std::vector<Corpo *> *corpos_old = this->lista_anterior->get_corpos();
  // Apaga corpos na tela
  
  for (k=0; k<corpos_old->size(); k++)
  {
    i = (int) ((*corpos_old)[k]->get_posicao());
    move(i+20, k);   /* Move cursor to position */
  	echochar(' ');  /* Prints character, advances a position */
    move(0, 0);
  }

  // Desenha corpos na tela
  std::vector<Corpo *> *corpos = this->lista->get_corpos();

  for (k=0; k<corpos->size(); k++)
  {
    i = (int) ((*corpos)[k]->get_posicao()) * \ 
        (this->maxI / this->maxX);
//posicao atual do jogador
    if(i+20 < this->maxY){
    	move(i+20, k);   /* Move cursor to position */
    	echochar('*');  /* Prints character, advances a position */
    }

    // Atualiza corpos antigos
    (*corpos_old)[k]->update(  (*corpos)[k]->get_velocidade(),\
                               (*corpos)[k]->get_posicao());
  }

  //Escreve o mapa no terminal do ncurses
  for (int j=0; j<20; j++) { 
    for (int k=0;k<20;k++){
       move(j+20, k);
       echochar(mapa[j][k]);
    }
  }
  move(i+20, 0); //reescreve o ponto no terminal do ncurses
  echochar('*');
  return;
  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}

void threadfun (char *keybuffer, int *control)
{
  char c;
  while ((*control) == 1) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
    else (*keybuffer) = 0;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses

  raw();				         /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
	noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */

  this->rodando = 1;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}

