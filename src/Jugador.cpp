#include "Jugador.h"


Jugador::Jugador(void)
{
}
Jugador::Jugador(string _nombre, string _id)
{
	
	w = false;
	a = false;
	s = false;
	d = false;
	posicion = new ofVec2f(50,50);
	CambiarNombre(nombre);
	id = _id;
}


Jugador::~Jugador(void)
{
}

void Jugador::update()
{
	if(a) posicion->x -= 3;
	if(d) posicion->x += 3;
	if(w) posicion->y -= 3;
	if(s) posicion->y += 3;

	//replicar el estado del jugador
	//datos["input"] = 


}

void Jugador::draw()
{
	ofSetColor(12,255,0);
	//ofCircle(posicion->x, posicion->y, 10);
	ofRect(posicion->x, posicion->y, 15,15);
	ofSetColor(ofColor::black);
	ofDrawBitmapString(nombre, posicion->x - 17, posicion->y -12);
}

void Jugador::keyPressed(int key)
{
	//solo detecta teclas si es jugador local
	if(esLocal)
	{
		if (key == 'w') 
		{
		w = true;
		}
		else if(key == 's') 
		{
		s =true;
		}
		else if(key == 'a') 
		{
		a =true;
		}
		else if(key == 'd') 
		{
		d =true;
		}
	}
}

void Jugador::keyReleased(int key)
{
	if(esLocal)
	{
		if (key == 'w') 
		{
		w =false;
		}
		else if(key == 's') 
		{
		s =false;
		}
		else if(key == 'a') 
		{
		a =false;
		}
		else if(key == 'd') 
		{
		d =false;
		}
	}
}

void Jugador::CambiarNombre(string n)
{
	nombre = n;
	datosRepl["nombre"] = nombre;
}