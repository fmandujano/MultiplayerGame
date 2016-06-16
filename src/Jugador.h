#pragma once
#include "ofMain.h"
#include "ofxJSONElement.h"
#include "sqlite3.h"

struct Sprite
{
public:
	ofTexture textura;
	int x;
	int y;
	int w;
	int h;
	float px;
	float py;
};


class Jugador
{
public:
	Jugador(void);
	Jugador(string _nombre, string _id);
	~Jugador(void);

	bool esLocal;
	ofVec2f *posicion;
	float rapidez;
	string nombre;
	string id;

	//propiedades visuales del tanque
	int ancho;
	int alto;
	int anchoOruga;
	int largoCanon;


	
	bool w,a,s,d;
	bool disparando;

	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	ofxJSONElement datosRepl;

	void cargarDesdeJSON();
	void cargarSpriteTanque(string nombre);
	static void cargarDatos();

private:
	//sprite del jugador
	Sprite sprite;
	//nombre del sprite de tanque
	string nombreTanque;

	/*
	Strictly speaking, the Callback() function should be an extern "C" free function, not a static member.
	http://stackoverflow.com/questions/12201240/c-how-to-access-member-variables-through-a-callback-method
	*/
	static int cargarJugadores(void *NotUsed, int argc, char **argv, char **azColName);
	static int procesarRegistroSprite(void *NotUsed, int argc, char **argv, char **azColName);
};

/*
estructura de datosRepl
{
	"input":"00000",
	"jugador":"nombre"
}
*/