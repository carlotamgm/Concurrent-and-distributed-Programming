//------------------------------------------------------------------------------
// File:   ejercicio_2.cpp
// Author: Ignacio Millán Aznar y Carlota Moncasi Gosá
// Date:   septiembre 2022
// Coms:   Parte del práctica 1 de PSCD
//         Compilar mediante
//           g++ practica_1_V1.cpp -o practica_1_V1 -std=c++11 -pthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>

using namespace std;

void saludo(int id, int retardo, int veces) {

    for(int i=1; i<=veces; i++) {
        // cout << "Soy " << id << endl;
        cout << "Soy " + to_string(id) + "\n";
        //el thread que me ejecuta se bloquea durante "retardo" milisegundos
        this_thread::sleep_for(chrono::milliseconds(retardo));
    }
}

int main(int argc, char* argv[]) {

    srand(time(NULL));
    int retardo = 100 + rand()%(301-100);
    int veces = 5 + rand()%(16-5);

    const int N=9;
    thread P[N];
    for(int i=0; i<N; i++) {
        P[i]=thread(&saludo, i, retardo, veces);
    }
    for(int i=0; i<9; i++) {
        P[i].join();
    }
}
