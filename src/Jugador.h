#pragma once
#include "ofMain.h"
#include "ofxJSONElement.h"


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
	
	bool w,a,s,d;
	bool disparando;

	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	ofxJSONElement datosRepl;

private:
};

/*
estructura de datosRepl
{
	"input":"00000",
	"jugador":"nombre"
}
*/