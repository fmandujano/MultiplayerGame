#include "ofMain.h"
#include "ofApp.h"



//========================================================================
int main(int argc, char** argv )
{
	int sizeX = 500;
	int sizeY = 500;
	bool fullscreen = false;
	
	//cargar datos desde archivo INI
	ifstream configFile;
	configFile.open("data/config.ini");

	if(configFile.is_open())
	{
		char * linea = new char[100]; //malloc
		char * llave,* valor;
		while( !configFile.eof()  )
		{
			configFile.getline(linea, 100);
			//ignorar lineas que comiencen con estos caracteres

			if( linea[0] != '/' &&
				linea[0] != '[' &&
				linea[0] != '\r' &&
				linea[0] != '\n' && 
				linea[0] != '\0')
			{
				//supongamos que se lee sizex=135
				llave = strtok(linea, "=");
				valor = strtok(NULL, "=");
				if(llave != NULL && valor != NULL)
				{
					printf("llave: %s, valor=%s \n", llave, valor);
					//comparar la llave con cada una de las variables a configurar
					if( !strcmp(llave, "sizex"))	sizeX = atoi(valor);
					if( !strcmp(llave, "sizey"))	sizeY = atoi(valor);
					if( !strcmp(llave, "fullscreen"))	fullscreen = atoi(valor)==0 ? false:true;
				}
			}
		}


		ofSetupOpenGL(sizeX, sizeY, fullscreen ? OF_FULLSCREEN : OF_WINDOW);			// <-------- setup the GL context
		// this kicks off the running of my app
		// can be OF_WINDOW or OF_FULLSCREEN
		// pass in width and height too:
		ofRunApp(new ofApp());
	}
	else return ERROR_BAD_CONFIGURATION;
}
