/*Cantidad de niveles que tiene el juego antes de que se considere ganada la partida*/
#define CANTIDADNIVELES 15 // OBS: Debe ser multiplo de 3

/*Cantidad de veces que parpadean los LEDs de Error y Pausa para avuisar del siguiente patron*/
#define CANTIDADALERTAS 5

/*Tiempo de parpadeo de las luces de llamada de atencion*/
#define TIEMPOATENCION 200

/*Tiempo de espera entre llamadas a interrupciones*/
#define ESPERA    500

/*Cantidad de LEDs que corresponden a cada dificultad*/
#define LEDSFACIL   3
#define LEDSMEDIO   4
#define LEDSDIFICIL 5 

/*Cantidad de Tiempo que tiene el usuario para responder segun la dificultad*/
#define TIEMPOFACIL     200
#define TIEMPOMEDIO     100
#define TIEMPODIFICIL   50
#define TIEMPOIMPOSIBLE 25

/*Pines para los LEDs*/
const int ledAzul     = PD_0;
const int ledBlanco   = PD_1;
const int ledRojo     = PD_2;
const int ledNaranja  = PD_3;
const int ledAmarillo = PE_1;
const int ledError    = PE_2;
const int ledPausa    = PE_3;

/*Pines para los botones*/
const int botonAzul     = PF_3;
const int botonBlanco   = PB_3;
const int botonRojo     = PC_4;
const int botonNaranja  = PC_5;
const int botonAmarillo = PC_6;
const int botonError    = PC_7;
const int botonPausa    = PD_6;

/*Colores*/
#define AZUL       0
#define BLANCO     1
#define ROJO       2
#define NARANJA    3
#define AMARILLO   4
#define COLORERROR 5
#define COLORPAUSA 6

/*Estados del juego*/          
#define INICIO  0         
#define SIMON   1         
#define MOSTRAR 2
#define JUGADOR 3          
#define PAUSA   4          
#define ERROR   5
#define GANADOR 6

/*Dificultades*/
#define FACIL     0
#define MEDIO     1
#define DIFICIL   2 
#define IMPOSIBLE 3

/*Variables Globales*/
int simon[CANTIDADNIVELES]; // Vector que contiene las ordenes de Simon
int jugador[CANTIDADNIVELES]; // Vector que contiene las respuestas del Jugador
int numeroOrden; // Este numero indica el indice en el que debera insertar la siguiente orden de Simon
int estadoJuego = INICIO; // Esta variable indica como se comporta el juego
int numeroRespuesta; // Este numero indica el indice en el que se debera insertar la siguiente respuesta del Jugador
int dificultadInicial;
int dificultadLeds;
int dificultadTiempo;
int imprimirMenu = 1;
int hayPartidaGuardada = 0;

void setup(){
	Serial.begin(9600);
	randomSeed(analogRead(0));	

	/*Pines Configurados para Salida*/
	pinMode(ledAzul,     OUTPUT);
	pinMode(ledBlanco,   OUTPUT);
	pinMode(ledRojo,     OUTPUT);
	pinMode(ledNaranja,  OUTPUT);
	pinMode(ledAmarillo, OUTPUT);
	pinMode(ledError,    OUTPUT);
	pinMode(ledPausa,    OUTPUT);

	/*Pines Configurados para Entrada*/
	pinMode(botonAzul,     INPUT);
	pinMode(botonBlanco,   INPUT);
	pinMode(botonRojo,     INPUT);
	pinMode(botonNaranja,  INPUT);
	pinMode(botonAmarillo, INPUT);
	pinMode(botonError,    INPUT);
	pinMode(botonPausa,    INPUT);

	/*Anclando interupcciones a los botones*/
	attachInterrupt(botonAzul,     botonAzulInt,     RISING);
	attachInterrupt(botonBlanco,   botonBlancoInt,   RISING);
	attachInterrupt(botonRojo,     botonRojoInt,     RISING);
	attachInterrupt(botonNaranja,  botonNaranjaInt,  RISING);
	attachInterrupt(botonAmarillo, botonAmarilloInt, RISING);
	attachInterrupt(botonPausa,    botonPausaInt,    RISING);
	attachInterrupt(botonError,    botonErrorInt,    RISING);
	/*Cuando inicia el programa, apagar cualquier LED que se haya encendido por algun tipo de ruido*/
	apagarLeds();
}

void loop(){
	switch(estadoJuego){
		case INICIO:
			reinicializarJugador();
			reinicializarSimon();
			numeroOrden = 0;
			numeroRespuesta = 0;
			seleccionarDificultad();
			apagarLeds();
			digitalWrite(ledAzul,HIGH);
			digitalWrite(ledBlanco,HIGH);
			digitalWrite(ledRojo,HIGH);
			digitalWrite(ledNaranja,HIGH);
			delay(TIEMPOATENCION);
			apagarLeds();
			delay(TIEMPOATENCION);
			break;
		case SIMON:
			aumentarDificultad();
			generarOrdenes(dificultadLeds);
			break;
		case MOSTRAR:
			for(int i=0;i<CANTIDADALERTAS;i++){
				llamarAtencion();		
			}
			mostrarOrdenes(dificultadTiempo);
			esperarJugador();
			break;
		case JUGADOR:
			break;
		case PAUSA:
			menuPausa();
			break;	
		case ERROR:
			delay(TIEMPOATENCION);
			digitalWrite(ledError,HIGH);		
			delay(TIEMPOATENCION);
			apagarLeds();		
			break;
	}
}

void aumentarDificultad(){
	switch(dificultadInicial){
		case FACIL:
			if(numeroOrden < CANTIDADNIVELES/3){
				dificultadTiempo = TIEMPOFACIL;	
				dificultadLeds = LEDSFACIL;
			}else if(numeroOrden < 2*CANTIDADNIVELES/3){
				dificultadTiempo = TIEMPOMEDIO;	
				dificultadLeds = LEDSMEDIO;
			}else{
				dificultadTiempo = TIEMPODIFICIL;	
				dificultadLeds = LEDSDIFICIL;
			}
			break;
		case MEDIO: 
			if(numeroOrden < 2*CANTIDADNIVELES/3){
				dificultadTiempo = TIEMPOMEDIO;	
				dificultadLeds = LEDSMEDIO;
			}else{
				dificultadTiempo = TIEMPODIFICIL;	
				dificultadLeds = LEDSDIFICIL;
			}

			break;
		case DIFICIL:
				dificultadTiempo = TIEMPODIFICIL;	
				dificultadLeds = LEDSDIFICIL;
			break;
		case IMPOSIBLE:
			dificultadTiempo = TIEMPOIMPOSIBLE;	
			dificultadLeds = LEDSDIFICIL;
			break;
	}
}

void menuPausa(){
	if (imprimirMenu){
		Serial.print("#############################################################################################\n");
		Serial.print("#Bienvenido al menu de Pausa                                                                #\n");
		Serial.print("#Presiona el boton de Pausa para continuar la partida o selecciona alguna de las opciones:  #\n");
		Serial.print("#- Guardar partida (AZUL)                                                                   #\n");
		if(hayPartidaGuardada){
			Serial.print("#- Cargar partida guardada (BLANCO)                                                         #\n");
		}
		Serial.print("#- Iniciar partida nueva (ROJO)                                                             #\n");
		Serial.print("#############################################################################################\n");
		imprimirMenu = !imprimirMenu;
	}
}

void parpadearLed(int led, int retraso){
	switch(led){
		case AZUL:	
			apagarLeds();
			digitalWrite(ledAzul,HIGH);
			delay(retraso);
			digitalWrite(ledAzul,LOW);
			delay(retraso);
			break;
		case BLANCO:	
			apagarLeds();
			digitalWrite(ledBlanco,HIGH);
			delay(retraso);
			digitalWrite(ledBlanco,LOW);
			delay(retraso);
			break;
		case ROJO:	
			apagarLeds();
			digitalWrite(ledRojo,HIGH);
			delay(retraso);
			digitalWrite(ledRojo,LOW);
			delay(retraso);
			break;
		case NARANJA:	
			apagarLeds();
			digitalWrite(ledNaranja,HIGH);
			delay(retraso);
			digitalWrite(ledNaranja,LOW);
			delay(retraso);
			break;
		case AMARILLO:	
			apagarLeds();
			digitalWrite(ledAmarillo,HIGH);
			delay(retraso);
			digitalWrite(ledAmarillo,LOW);
			delay(retraso);
			break;
		case COLORPAUSA:	
			apagarLeds();
			digitalWrite(ledPausa,HIGH);
			delay(retraso);
			digitalWrite(ledPausa,LOW);
			delay(retraso);
			break;
		case COLORERROR:	
			apagarLeds();
			digitalWrite(ledError,HIGH);
			delay(retraso);
			digitalWrite(ledError,LOW);
			delay(retraso);
			break;
		default:
			apagarLeds();
			encenderLeds();
			delay(retraso);
			apagarLeds();
			delay(retraso);
	}
}

void seleccionarDificultad(){
	if (imprimirMenu){
		Serial.print("#############################################################################################\n");
		Serial.print("#Bienvenido a Simon Dice!!                                                                  #\n");
		Serial.print("#Para empezar a jugar primero debes elegir la dificultad con la que deseas empezar          #\n");
		Serial.print("#Presiona el boton con el color entre parentesis de la dificultad deseada                   #\n");
		Serial.print("#- FACIL (AZUL)                                                                             #\n");
		Serial.print("#- MEDIO (BLANCO)                                                                           #\n");
		Serial.print("#- DIFICIL (ROJO)                                                                           #\n");
		Serial.print("#- IMPOSIBLE (NARANJA)                                                                      #\n");
		Serial.print("#Presta mucha atencion!! Al seleccionar una dificultad los botones de Pausa y de Reiniciar  #\n");
		Serial.print("#empezaran a parpadear, cuando estos terminen, Simon dara su orden y luego solo quedara     #\n");
		Serial.print("#parpadeando el boton de Pausa eso significa que es tu turno de responder a Simon, o Pausar #\n"); 
		Serial.print("#el juego para ver un Menu con diferentes opciones                                          #\n");
		Serial.print("#############################################################################################\n");
		imprimirMenu = !imprimirMenu;
	}
}

/*
* Funcion encenderLeds()
*    Esta funcion enciende
*
*/
void encenderLeds(){
	digitalWrite(ledAzul,HIGH);
	digitalWrite(ledBlanco,HIGH);
	digitalWrite(ledRojo,HIGH);
	digitalWrite(ledNaranja,HIGH);
	digitalWrite(ledAmarillo,HIGH);
	digitalWrite(ledError,HIGH);
	digitalWrite(ledPausa,HIGH);
}


/*
* Funcion apagarLeds()
*    Esta funcion apaga
*
*/
void apagarLeds(){
	digitalWrite(ledAzul,LOW);
	digitalWrite(ledBlanco,LOW);
	digitalWrite(ledRojo,LOW);
	digitalWrite(ledNaranja,LOW);
	digitalWrite(ledAmarillo,LOW);
	digitalWrite(ledError,LOW);
	digitalWrite(ledPausa,LOW);
}

/*
* Funcion generarOrdenes
*    Esta funcion genera las ordenes de Simon en el vector "simon"
*    y actualiza la variable "numeroOrden" con el indice que 
*    corresponde a la siguiente orden de Simon
*/
void generarOrdenes(int dificultad){
	for(int i = 0; i <= numeroOrden; i++ ){
		if(simon[i]== -1){
			simon[i] = random(dificultad);
		}
	}	
	numeroOrden++;
	estadoJuego = MOSTRAR;
}

/*
* Funcion mostrarOrdenes
*    Esta funcion muestra las ordenes en los LEDs correspondientes a
*    cada color.
*/
void mostrarOrdenes(int dificultad){
	for(int i=0;i < numeroOrden;i++){
		switch(simon[i]){
			case AZUL:
				encenderAzul(dificultad);
				break;
			case BLANCO:
				encenderBlanco(dificultad);
				break;
			case ROJO:
				encenderRojo(dificultad);
				break;
			case NARANJA:
				encenderNaranja(dificultad);
				break;
			case AMARILLO:	
				encenderAmarillo(dificultad);
				break;
		}
	}	
	estadoJuego = JUGADOR;
}

/*
* Funcion reinicializarSimon
*	Esta funcion vuelve a colocar "-1" en todos los espacios 
*	del vector "simon"
*/
void reinicializarSimon(){
	for(int i=0;i<CANTIDADNIVELES;i++){
		simon[i]=-1;
	}
}

/*
* Funcion reinicializarJugador
*	Esta funcion vuelve a colocar "-1" en todos los espacios 
*	del vector "jugador"
*/
void reinicializarJugador(){
	for(int i=0;i<CANTIDADNIVELES;i++){
		jugador[i]=-1;
	}
}

/*
* Funcion esperarJugador
*    Esta funcion espera que el jugador termine de dar sus respuestas
*    o que el mismo cometa un error en ellas para pasar a la siguiente
*    orden de Simon, anunciar la partida como perdida en caso de error
*    o anunciar que se gano la partida
*/
void esperarJugador(){
	numeroRespuesta = 0;
	estadoJuego=JUGADOR;
	while(jugador[numeroOrden-1] == -1){
		notificarTurno();
		if(estadoJuego != JUGADOR){
			reinicializarJugador();
			return;	
		}
	}
	reinicializarJugador();
	estadoJuego=SIMON;
}

void verificarPartida(){
	for(int i=0; i<numeroRespuesta;i++){
		if(simon[i] != jugador[i]){
			estadoJuego = ERROR;
			return;
		}
	}
}

void botonAzulInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case INICIO:
				dificultadInicial = FACIL;
				estadoJuego = SIMON;
				break;
			case JUGADOR:
				jugador[numeroRespuesta] = AZUL;
				apagarLeds();
				digitalWrite(ledAzul,HIGH);
				numeroRespuesta++;
				verificarPartida();
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonBlancoInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case INICIO:
				dificultadInicial = MEDIO;
				estadoJuego = SIMON;
				break;
			case JUGADOR:
				jugador[numeroRespuesta] = BLANCO;
				apagarLeds();
				digitalWrite(ledBlanco,HIGH);
				numeroRespuesta++;
				verificarPartida();
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonRojoInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case INICIO:
				dificultadInicial = DIFICIL;
				estadoJuego = SIMON;
				break;
			case JUGADOR:
				jugador[numeroRespuesta] = ROJO;
				apagarLeds();
				digitalWrite(ledRojo,HIGH);
				numeroRespuesta++;
				verificarPartida();
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonNaranjaInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case INICIO:
				dificultadInicial = IMPOSIBLE;
				estadoJuego = SIMON;
				break;
			case JUGADOR:
				jugador[numeroRespuesta] = NARANJA;
				apagarLeds();
				digitalWrite(ledNaranja,HIGH);
				numeroRespuesta++;
				verificarPartida();
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonAmarilloInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case JUGADOR:
				jugador[numeroRespuesta] = AMARILLO;
				apagarLeds();
				digitalWrite(ledAmarillo,HIGH);
				numeroRespuesta++;
				verificarPartida();
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonPausaInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case JUGADOR:
				imprimirMenu = 1;
				estadoJuego = PAUSA;
				break;
			case PAUSA:
				estadoJuego = MOSTRAR;
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void botonErrorInt(){
	static int tiempoAnt    = 0;
	int        tiempoActual = millis(); 
	if ( (tiempoActual-tiempoAnt > ESPERA)){
		switch (estadoJuego){
			case ERROR:
				estadoJuego = INICIO;
				imprimirMenu = 1;
				break;
		}
	}
	tiempoAnt = tiempoActual;
}

void encenderAzul(int dificultad){
	delay(dificultad);
	digitalWrite(ledAzul,HIGH);		
	delay(dificultad);
	digitalWrite(ledAzul,LOW);		
}

void encenderBlanco(int dificultad){
	delay(dificultad);
	digitalWrite(ledBlanco,HIGH);		
	delay(dificultad);
	digitalWrite(ledBlanco,LOW);		
}

void encenderRojo(int dificultad){
	delay(dificultad);
	digitalWrite(ledRojo,HIGH);		
	delay(dificultad);
	digitalWrite(ledRojo,LOW);		
}

void encenderNaranja(int dificultad){
	delay(dificultad);
	digitalWrite(ledNaranja,HIGH);		
	delay(dificultad);
	digitalWrite(ledNaranja,LOW);		
}

void encenderAmarillo(int dificultad){
	delay(dificultad);
	digitalWrite(ledAmarillo,HIGH);		
	delay(dificultad);
	digitalWrite(ledAmarillo,LOW);		
}

void llamarAtencion(){
	delay(TIEMPOATENCION);
	digitalWrite(ledError,HIGH);		
	digitalWrite(ledPausa,HIGH);		
	delay(TIEMPOATENCION);
	apagarLeds();
}

void notificarTurno(){
	delay(TIEMPOATENCION);
	digitalWrite(ledPausa,HIGH);		
	delay(TIEMPOATENCION);
	digitalWrite(ledPausa,LOW);		
}
