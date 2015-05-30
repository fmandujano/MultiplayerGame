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
	string nombre;
	string id;

	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);

	void CambiarNombre(string n);

	
	ofxJSONElement datosRepl;

private:
	bool w,a,s,d;
	bool disparando;
};

