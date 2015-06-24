#pragma once
#include "ofMain.h"
#include "ofxJSONElement.h"
#include "sqlite3.h"

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

	static void cargarDatos();

private:
	static int cargarJugadores(void *NotUsed, int argc, char **argv, char **azColName);

};

/*
estructura de datosRepl
{
	"input":"00000",
	"jugador":"nombre"
}
*/