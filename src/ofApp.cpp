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
		//replicar estoado del jugador
		string res = jugadorLocal->datosRepl.getRawString();
		udpManager.Send( res.c_str(), res.size());

		//iterar la lista de jugadores y actualizarlos
		/*for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
		{
			Jugador j = *it;
			j.update();
		}*/
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
	int recb = udpManager.Receive(mensaje, 100);
	//std::cout << recb << std::endl;
	if(recb > 0)
	{
		analizarMensajeUDP(mensaje);
	}

	//iterar la lista de jugadores y actualizarlos
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.update();
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
			Jugador j = *it;
			if(j.id == idstr)
			{
				//aplicar los inputs, W,A,S,D,disparo
				j.w = (input[0] - '0') != 0;
				j.a = (input[1] - '0') != 0;
				j.s = (input[2] - '0') != 0;
				j.d = (input[3] - '0') != 0;
				j.disparando = (input[4] - '0') != 0;
				
				std::cout << "jallado " << j.w<< j.a<< j.s<< j.d<< j.disparando  <<std::endl;
				break;
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
	//iniciarServidor()
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
