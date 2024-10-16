//******************************************************************
// File:   Coordinador.cpp
// Author: Ignacio Mill�n Aznar NIP:841537, Carlota Moncasi NIP:839841
// Date:   Noviembre 2022
// Coms:   Servidor multicliente con comunicación síncrona mediante sockets
//         Invocación
//                  ./Coordinador
//*****************************************************************

#include "Socket.hpp"
#include <string.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include "controlParejas.cpp"

using namespace std;

// ----------------------------------------------------
// Pre : 
// Post : Separa cada palabra de un string para guardarse cada dato individualmente

void trocearString(string s, string& n1, int& n2) {
	const char delim[] = ","; //los separadores aqu� son ' ' y ','
	char* token;
	char* copia = strdup(s.c_str()); //trabajaremos sobre una copia

	token = strtok(copia, delim);
        n1 = token;
	token = strtok(nullptr, delim); //hasta el primer ','
	n2 = atoi(token);

	//"strdup" reserva memoria para la copia. Hay que liberarla
	//Como "strdup" viene de C, reserva memoria con "malloc". Mejor usar "free" para
	//liberarla. Si la reserv�ramos mediante "new p" ("p" es una clase), hay que
	//usar "delete".
	free(copia);

}

// ----------------------------------------------------
// Pre : 
// Post : El proceso profesor espera a que haya dos estudiantes y se sienten, asigna a cada uno su pareja y fila (y pasa el testigo) y espera a que todos hayan terminado su trabajo para comunicar la
//        finalización del examen (y pasar el testigo)

void Profesor(controlParejas& control) {

    for( int i=0; i< N_FIL ; i++) {
        control.asignar(i);
    }
    control.contadorTerminadas();
}


// ----------------------------------------------------
// Pre : "controlParejas" es el monitor que sincroniza las funciones
// Post : Lanza un thread Profesor con el comportamiento del profesor, según la práctica
//        anterior que termina cuando todos los estudiantes hayan realizado sus tareas.
//        Espera a que los alumnos se vayan conectando, para poder encargarles una tarea
//        Cada vez que un nuevo alumno se conecta lanza un thread representante para
//        poder atender al alumno, mediante una comunicación de mensajes entre ambos.

void profServ(Socket& chan, int client_fd, controlParejas& control, int resultados[]) {

    // Buffer para recibir el mensaje

    int length = 100;
    string buffer;
    int n2;
    string what;
    int nip, miPareja, miFila;
        // Recibimos el mensaje del cliente "sentar, nip"
        int rcv_bytes = chan.Receive(client_fd,buffer,length);

        if (rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
        }
            
        trocearString(buffer, what, n2);

        //if(what=="sentar"){
	nip=n2;
        control.sillaLibre(nip);
        control.filaAsignada(nip);

        miPareja = control.dimePareja(nip);
        miFila = control.dimeFila(nip);

        // Enviamos la respuesta "nipPareja, fila"
        string s = to_string(miPareja) + "," + to_string(miFila);

        int send_bytes = chan.Send(client_fd, s);
        if(send_bytes == -1) {
            cerr << chan.error("Error al enviar datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
            exit(1);
        }

        rcv_bytes = chan.Receive(client_fd,buffer,length);

        if (rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
        }
        trocearString(buffer, what, n2);

        if(what == "max"){
            control.resultadoMenor(n2, nip, resultados);

        }else {
            control.resultadoMayor(n2, nip, resultados);
            control.acabanEstudiantes(nip, resultados);
        }

}

//-------------------------------------------------------------
// 
// PRE:
// POST: versión distribuida. Por un lado, desarrolla un profesor coordinador y, por otro, un conjunto de
// 60 estudiantes trabajadores, cada uno conectado a su "representante"

int main(int argc, char* argv[]) {
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT;
    if(argv==2){
    
    SERVER_PORT = atoi(argv[1]); //normalmente será un parámetro de invocación. P.e.: argv[1]
    }else{
    	SERVER_PORT=3221;
    	}
    int resultados[N_EST];
    vector<thread> cliente;
    controlParejas control;

    thread P;
    P=thread(&Profesor,ref(control));

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind",errno);
        exit(1);
    }

    //listen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el listen",errno);
        // Cerramos el socket
        chan.Close();
        exit(1);
    }

    for(int i=0; i<N_EST; i++) {
        // Accept
        int new_client_fds = chan.Accept();

        if(new_client_fds == -1) {
            cerr << chan.error("Error en el accept",errno);
            // Cerramos el socket
            chan.Close();
            exit(1);
        } else {
            // arrancar el thread
            cliente.push_back(thread(&profServ, ref(chan), new_client_fds, ref(control), resultados));
            //cout << "Nuevo cliente " + to_string(i) + " aceptado" + "\n";
        }
	    
    }

    P.join();


    //¿Qué pasa si algún thread acaba inesperadamente?
    for (int i=0; i<cliente.size(); i++) {
        cliente[i].join();
    }

    // Cerramos el socket del servidor
    error_code = chan.Close();
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor",errno);
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}
//-------------------------------------------------------------

