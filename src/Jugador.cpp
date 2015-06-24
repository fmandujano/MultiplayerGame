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


	//leer las propiedades visuales desde un archivo
	string rutaArchivo = "tanque.json";
	ofxJSONElement json;
	if (json.open(rutaArchivo))
	{
		ancho = json["ancho"].asInt();
		alto = json["alto"].asInt();
		anchoOruga = json["woruga"].asInt();
		largoCanon = json["lcannon"].asInt();
	}
	else
	{
		throw EXCEPTION_READ_FAULT;
	}
}


Jugador::~Jugador(void)
{
}

/*
	Carga los datos desde una base de datos implementada con sqlite3
*/
void Jugador::cargarDatos()
{
	cout<<"Cargando datos desde sqlite" << endl;
	//referencia al archivo de la base de datos
	sqlite3 *db;
	//contador de registro, utilizado apra almacenar codigos de retorno
	int rc;
	//buffer para los mensajes de error
	char *zErrMsg = 0;
	//abrir el archivo y cargar la BD
	rc = sqlite3_open("data/data.sqlite", &db);
	//si rc es distinto de 0, hubo un error
	if(rc)
	{
		printf("no se puede acceder a la base de datos");
		sqlite3_close(db);
	}

	//seleccionar todas las columnas del registro cuyo nombre es chuchito
	string query = "SELECT * FROM jugador WHERE nombre='chuchito'";
	rc = sqlite3_exec(db, query.c_str(), cargarJugadores, 0, &zErrMsg); //cargarjugadores se llamara por cada registro que se encuentre
}

/*
	callback llamado para cada registro encontrado en la consulta de jugadores
*/
int Jugador::cargarJugadores(void *NotUsed, int argc, char **argv, char **azColName)
{
	//simplemente imprimira los datos en la consola
	std::cout << "Registro:\n";
	for(int i=0; i<argc; i++)
	{
		std::cout << azColName[i] << " : " << argv[i] << std::endl;
	}
	//retornar 0, codigo de que no hubo error
	return 0;
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
	std::stringstream *ss = new stringstream();
	*ss <<  w ? "1" : "0";
	*ss << a ? "1" : "0";
	*ss << s ? "1" : "0";
	*ss << d ? "1" : "0";
	*ss << disparando ? "1":"0";
	//std::cout << ss->str() << std::endl;
	datosRepl["input"] =  ss->str();
	delete ss;
}

void Jugador::draw()
{
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