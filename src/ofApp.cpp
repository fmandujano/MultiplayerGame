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
	jugadorLocal->update();
	if (clienteEsperandoID )//&& TCPClient.isConnected())
	{
		string str = TCPClient.receive();
		idCliente = str;
		ofLog(OF_LOG_NOTICE, "conectado a servidor con id ="+idCliente);
		//TCPClient.send("OK");
		clienteEsperandoID = false;
		jugadorLocal = new Jugador(nombreJugador, idCliente); 
		//conectar por udp 

	}

	//iterar la lista de jugadores y actualizarlos
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.update();
	}
}

void ofApp::updateServidor()
{
	jugadorLocal->update();
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

				//TCPServer.send(i, "cliente" + i);
				//ofLog(OF_LOG_NOTICE, "jugador conectado"+i);
			}
			if(str == "OK")
			{
				ofLog(OF_LOG_NOTICE, "jugador conectado" );
				TCPServer.disconnectClient(i);
			}
		}
	}

	//iterar la lista de jugadores y actualizarlos
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.update();
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
	jugadorLocal->draw();
	//iterar la lista de jugadores remotos y dibujarlos
	for(std::vector<Jugador>::iterator it = jugadores.begin(); it != jugadores.end(); ++it) 
	{
		Jugador j = *it;
		j.draw();
	}
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
	string nombreJugador = ofSystemTextBoxDialog("Tu nombre", "jugador");
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

	//crear un jugador local
	jugadorLocal = new Jugador("SERVER", "-1"); 
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
