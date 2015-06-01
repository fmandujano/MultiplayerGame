#include "Jugador.h"


Jugador::Jugador(void)
{
	Jugador("jugador", "-1");
}

Jugador::Jugador(string _nombre, string _id)
{
	w = false;
	a = false;
	s = false;
	d = false;
	disparando = false;
	posicion = new ofVec2f(50,50);
	rapidez = 100; //pixeles sobre segundo
	nombre = _nombre;
	id = _id;
	datosRepl["id"] = id;
}


Jugador::~Jugador(void)
{
}

void Jugador::update()
{
	if(a) posicion->x -= rapidez*ofGetLastFrameTime();
	if(d) posicion->x += rapidez*ofGetLastFrameTime();
	if(w) posicion->y -= rapidez*ofGetLastFrameTime();
	if(s) posicion->y += rapidez*ofGetLastFrameTime();

	//imprime el estado de los inputs
	//if(!esLocal) std::cout << w<<a<<s<<d<< std::endl;

	//replicar el estado del jugador
	if(esLocal)
	{
		std::stringstream *ss = new stringstream();
		*ss <<  w ? "1" : "0";
		*ss << a ? "1" : "0";
		*ss << s ? "1" : "0";
		*ss << d ? "1" : "0";
		*ss << disparando ? "1":"0";
		//std::cout << ss->str() << std::endl;
		datosRepl["input"] =  ss->str();
		//std::cout << datosRepl.getRawString() << std::endl;
	}
}

void Jugador::draw()
{
	int ancho = 30;
	int alto = 34;
	int anchoOruga = 8;
	int largoCanon = alto/2+5;
	ofSetColor(85,107,47);
	//oruga izq
	ofRect(posicion->x-(ancho/2), posicion->y-alto/2, anchoOruga, alto);
	//oruga der
	ofRect(posicion->x+(ancho/2)-anchoOruga, posicion->y-alto/2, anchoOruga, alto);
	//cañon
	ofSetColor(107, 142, 35);
	ofRect(posicion->x-2, posicion->y-largoCanon, 4, largoCanon); 
	//torreta
	ofCircle(posicion->x, posicion->y, ancho/2 - anchoOruga/2);
	//nombre del jugador
	ofSetColor(ofColor::black);
	ofDrawBitmapString(nombre, posicion->x - ancho, posicion->y -alto);
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
		else if(key == ' ') 
		{
			disparando = true;
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
		else if(key == ' ') 
		{
			disparando = false;
		}
	}
}