#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxNetwork.h"
#include "Jugador.h"

#define PUERTO_TCP 12345
#define PUERTO_UDP 12346

//Pantalla de inicio del juego
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	

		enum EstadoApp
		{
			inicio, servidor, cliente
		} estado;

		ofxPanel gui;
		ofxButton botonCrearPartida;
		ofxButton botonConectarPartida;
		//listeners de los botones
		void crearPartida();
		void conectarPartida();

		void updateCliente();
		void updateServidor();
		
		void drawCliente();
		void drawServidor();

		ofxUDPManager udpManager;
		
		Jugador * jugadorLocal;
		//jugadores remotos
		std::vector<Jugador> jugadores;

		/// variables de cliente
		ofxTCPClient TCPClient;
		bool clienteEsperandoID;
		string nombreJugador;
		string idCliente;

		//variables de servidor
		ofxTCPServer TCPServer;
		ofxJSONElement broadcastJSON;

private:
	void analizarMensajeUDP(const char * mensaje);
	void analizarMensajeUDPGlobal(const char * mensaje);
	void dibujarJugadorSimple(string nombre, int x, int y);
};