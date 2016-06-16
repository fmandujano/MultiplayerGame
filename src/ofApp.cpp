#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	estado = EstadoApp::inicio;
	gui.setup();
	gui.add(botonCrearPartida.setup("Crear Partida"));
	gui.add(botonConectarPartida.setup("Conectar a servidor"));
	gui.setPosition( (ofGetWidth() / 2)-(gui.getWidth()/2)  , (ofGetHeight() / 2)-(gui.getHeight()/2));
	botonCrearPartida.addListener(this, &ofApp::crearPartida);
	botonConectarPartida.addListener(this, &ofApp::conectarPartida);

	jugadorLocal = NULL;

	//ejemplo de cargar datos con sqlite3
	//Jugador::cargarDatos();
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (estado == EstadoApp::servidor)
	{
		updateServidor();
	}
	else if (estado == EstadoApp::cliente)
	{
		updateCliente();
	}
}

void ofApp::updateCliente()
{
	if (clienteEsperandoID )//&& TCPClient.isConnected())
	{
		string str = TCPClient.receive();
		if(str.size() != 0)
		{
			idCliente = str;
			ofLog(OF_LOG_NOTICE, "conectado a servidor con id: "+idCliente);
			TCPClient.send("OK");
			clienteEsperandoID = false;
			jugadorLocal = new Jugador(nombreJugador, idCliente); 
			jugadorLocal->esLocal = true;
			//conectar por udp 
			udpManager.Create();
			std::cout << "Conectando por UDP a " << TCPClient.getIP().c_str() << std::endl;
			udpManager.Connect( TCPClient.getIP().c_str(), PUERTO_UDP);
			udpManager.SetNonBlocking(true);
		}
		else
		{
			std::cout << "esperando ID"<< std::endl;
		}
	}
	else
	{
		jugadorLocal->update();
		//replicar estado del jugador
		string res = jugadorLocal->datosRepl.getRawString();
		udpManager.Send( res.c_str(), res.size());

		//leer los mensajes entrantes por UDP
		char mensaje[64000];
		string message;  
		string tempMessage;  
		bool getNext = true;  

		while (getNext) 
		{  
			udpManager.Receive(mensaje, 64000);  
			tempMessage = mensaje;  
			if (tempMessage=="") 
			{
				getNext = false;  
			}
			else 
			{
				message = tempMessage;  
			}
		}  
		if(message!="")
		{
			analizarMensajeUDPGlobal(message.c_str());
		}
	}	
}

void ofApp::updateServidor()
{
	jugadorLocal->update();

	//iteramos en todos los clientes conectados por TCP
	for (int i = 0; i < TCPServer.getLastID(); i++) // getLastID is UID of all clients
	{
		if (TCPServer.isClientConnected(i))// && TCPServer.getNumReceivedBytes(i)>0)
		{ 
			string str = TCPServer.receive(i);
			vector<string> parametros = ofSplitString(str, " ");

			//si es un cliente pidiendo un id de jugador, darselo
			if (parametros[0] ==  string("HOLA"))
			{
				std::cout << "jugador " << parametros[1] << " se quiere conectar" << std::endl;
				Jugador * jugadorNuevo = new Jugador(parametros[1], ofToString(i));
				jugadorNuevo->esLocal = false;
				jugadores.push_back(*jugadorNuevo);
				TCPServer.send(i, ofToString(i));
				//ofLog(OF_LOG_NOTICE, "jugador conectado"+i);
			}
			if(str == "OK")
			{
				ofLog(OF_LOG_NOTICE, "jugador conectado" );
				TCPServer.disconnectClient(i);
			}
		}
	}

	//leer los mensajes entrantes por UDP
	char mensaje[100];
	string message;  
    string tempMessage;  
    bool getNext = true;  

    while (getNext) 
	{  
		udpManager.Receive(mensaje, 100);  
        tempMessage = mensaje;  
		if (tempMessage=="") 
		{
			getNext = false;  
		}
		else 
		{
			message = tempMessage;  
        }
	}  
	if(message!="")
	{
		analizarMensajeUDP(message.c_str());
	}

	//iterar la lista de jugadores y actualizarlos
	if(jugadores.size() > 0)
	{
		ofxJSONElement jsonJugador;
		//el primer elemento es un json con datos del jugador local del servidor
		jsonJugador["id"] = jugadorLocal->id;
		jsonJugador["nombre"] = jugadorLocal->nombre;
		jsonJugador["x"] = jugadorLocal->posicion->x;
		jsonJugador["y"] = jugadorLocal->posicion->y;
		broadcastJSON["jugadores"][0] =  jsonJugador;
		int i=1;
		for(std::vector<Jugador>::iterator jug = jugadores.begin(); jug != jugadores.end(); ++jug) 
		{
			jug->update();
			//preparar el mensaje global
			jsonJugador["id"] = jug->id;
			jsonJugador["nombre"] = jug->nombre;
			jsonJugador["x"] = jug->posicion->x;
			jsonJugador["y"] = jug->posicion->y;
			broadcastJSON["jugadores"][i] = jsonJugador;
			i++;
		}

		udpManager.SendAll(broadcastJSON.getRawString().c_str(), broadcastJSON.getRawString().size());
		std::cout << "Enviando " << broadcastJSON.getRawString().size() << "bytes" << std::endl;
		//delete &jsonJugador;
	}
}

void ofApp::analizarMensajeUDP(const char * mensaje)
{
	//std::cout << "UDP: " << mensaje <<std::endl;
	//convertir string a objeto JSON
	ofxJSONElement json;
	if(json.parse(mensaje))
	{
		//obtener el id del jugador
		//std::cout << json["id"].asString() << ":"<<json["input"].asString() <<std::endl;
		//int idJugador = ofToInt(json["id"].asString());
		string idstr = json["id"].asString();
		string input = json["input"].asString();
		//buscarlo en la lista de jugadores remotos
		for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
		{
			if(it->id == idstr)
			{
				//aplicar los inputs, W,A,S,D,disparo
				it->w = (input[0] - '0') != 0;
				it->a = (input[1] - '0') != 0;
				it->s = (input[2] - '0') != 0;
				it->d = (input[3] - '0') != 0;
				it->disparando = (input[4] - '0') != 0;
				break;
			}
		}
	}
}

/* analiza el mensaje enviado de servidor a cliente con los datos de todos los jugadores */
void ofApp::analizarMensajeUDPGlobal(const char * mensaje)
{
	//convertir string a objeto JSON
	ofxJSONElement json;
	if(json.parse(mensaje))
	{
		ofxJSONElement::Value lista = json["jugadores"];
		if(lista.isArray()     )
		{
			for(Json::ValueIterator itr = lista.begin(); itr != lista.end(); itr++)
			{
				//std::cout<<  (*itr)["nombre"].asString();
				dibujarJugadorSimple(  (*itr)["nombre"].asString(),
										ofToInt((*itr)["x"].asString()),
										ofToInt((*itr)["y"].asString()));
			}
		}
	}
}


//--------------------------------------------------------------
void ofApp::draw()
{
	if (estado == EstadoApp::inicio)
	{
		gui.draw();
	}
	else if (estado == EstadoApp::cliente)
	{
		ofDrawBitmapString("CLIENTE",1,10);
		drawCliente();
	}
	else if (estado == EstadoApp::servidor)
	{
		ofDrawBitmapString("SERVIDOR", 1, 10);
		drawServidor();
	}
}

void ofApp::drawCliente()
{
	if(jugadorLocal != NULL)
	{
		jugadorLocal->draw();
	}
		//iterar la lista de jugadores remotos y dibujarlos
	/*
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.draw();
	}*/
}

void ofApp::drawServidor()
{
	jugadorLocal->draw();
	//iterar la lista de jugadores y dibujarlos
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.draw();
	}
}

void ofApp::conectarPartida()
{
	nombreJugador = ofSystemTextBoxDialog("Tu nombre", "jugador");
	string ipString = ofSystemTextBoxDialog("Direccion IP del servidor", "127.0.0.1");
	estado = EstadoApp::cliente;
	bool conectado = TCPClient.setup(ipString, PUERTO_TCP);
	//TCPClient.setMessageDelimiter("\n");
	std::cout << "conectado a servidor" << conectado << std::endl;
	TCPClient.send("HOLA "+nombreJugador );
	clienteEsperandoID = true;
}

void ofApp::crearPartida()
{
	estado = EstadoApp::servidor;
	//crear un servidor TCP para manejar los clientes que se quieren conectar
	TCPServer.setup(PUERTO_TCP);
	//TCPServer.setMessageDelimiter("\n");
	udpManager.Create();
	udpManager.Bind(PUERTO_UDP);
	udpManager.SetNonBlocking(true);
	//crear un jugador local
	nombreJugador = "Servid0r";
	jugadorLocal = new Jugador(nombreJugador, "-1"); 
	jugadorLocal->esLocal = true;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	jugadorLocal->keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	jugadorLocal->keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::dibujarJugadorSimple(string nombre, int x, int y)
{
	std::cout << "dibujando jugador " << nombre << std::endl;
	int ancho = 30;
	int alto = 34;
	int anchoOruga = 8;
	int largoCanon = alto/2+5;
	ofSetColor(85,107,47);
	//oruga izq
	ofRect(x-(ancho/2), y-alto/2, anchoOruga, alto);
	//oruga der
	ofRect(x+(ancho/2)-anchoOruga, y-alto/2, anchoOruga, alto);
	//cañon
	ofSetColor(107, 142, 35);
	ofRect(x-2, y-largoCanon, 4, largoCanon); 
	//torreta
	ofCircle(x, y, ancho/2 - anchoOruga/2);
	//nombre del jugador
	ofSetColor(ofColor::cornflowerBlue);
	ofDrawBitmapString(nombre, x - ancho, y -alto);
}
