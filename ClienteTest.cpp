//*****************************************************************
// File:   Cliente.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2022
// Coms:   Ejemplo de cliente con comunicación síncrona mediante sockets
//         Invocación
//                  ./Cliente
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <sstream>
#include "Socket.hpp"

using namespace std;

vector <string> split (const string& s, char sep) {
    vector <string > elems;
    string el;
    istringstream seqEls(s);
    while (getline(seqEls, el, sep))
        elems.push_back(el);
    return elems;
}

//Pre: <fila> es un índice de fila de <D>
//Post: devuelve el máximo de la fila <fila>
int maxFila(const int D[30][1000], const int fila) { // BIEN

    int max = D[fila][0];

    for (int i = 1; i < 1000; i++) {

        if (D[fila][i] > max)
            max = D[fila][i];

    }

    return max;

}

//Pre: <fila> es un índice de fila de <D>
//Post: devuelve la suma de los els. de la fila <fila>
int sumaFila(const int D[30][1000], const int fila) { // BIEN

    int sum = 0;

    for (int i = 0; i < 1000; i++)
        sum += D[fila][i];

    return sum;

}

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

void Alumno (const int nip, const int D[30][1000], const string ip, const int puerto) {

    // Dirección y número donde escucha el proceso servidor
    string SERVER_ADDRESS = ip; //alternativamente "127.0.0.1"
    int SERVER_PORT = puerto;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket ch(SERVER_ADDRESS, SERVER_PORT);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd;
    do {
        // Conexión con el servidor
        socket_fd = ch.Connect();
        count++;

        // Si error --> esperamos r1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    if(socket_fd == -1) {
        return;
    }

    string mensaje = "sentar," + to_string(nip);
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    cout << nip << " - Voy a mandar: " << mensaje << endl;

    send_bytes = ch.Send(socket_fd, mensaje); // O aquí

    if(send_bytes == -1) {
        cerr << ch.error("Error al enviar nip",errno);
        // Cerramos el socket
        ch.Close();
        exit(1);
    }

    string buffer;

    read_bytes = ch.Receive(socket_fd, buffer, MESSAGE_SIZE);
    if (read_bytes == -1) {
        cerr << ch.error("Error al recibir pareja y fila", errno);
        // Cerramos los sockets
        ch.Close();
    }

    vector<string> trozos = split(buffer, ',');
    int nipPareja = stoi(trozos[0]);
    int fila = stoi(trozos[1]);

    int resultados;

    if (nip < nipPareja) {

        resultados = maxFila(D, fila);

        mensaje = "max," + to_string(resultados);

    } else {

        resultados = sumaFila(D, fila);

        mensaje = "suma," + to_string(resultados);

    }
        
    send_bytes = ch.Send(socket_fd, mensaje);

    if(send_bytes == -1) {
        cerr << ch.error("Error al enviar resultados",errno);
        // Cerramos el socket
        ch.Close();
        exit(1);
    }
    
    // Cerramos el socket
    int error_code = ch.Close();
    if(error_code == -1)
        cerr << ch.error("Error cerrando el socket",errno);

}

int main(int argc,char* argv[]) {
    
    int D[30][1000];

    ifstream f("data_5.txt");
    if (f.is_open()) {
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 1000; j++)
                f >> D[i][j];
        }
        f.close();
    } else
        cout << "No se pudo abrir el archivo" << endl;

    thread Alumnos[60];
    // Lanzamos los hilos
    for (int i = 0; i < 60; i++)
        Alumnos[i] = thread(&Alumno, i, D, argv[1], stoi(argv[2]));
    
    // Join de hilos
    for (int i = 0; i < 60; i++)
        Alumnos[i].join();

    // Despedida
    cout << "Bye bye" << endl;

    return 0;
}
