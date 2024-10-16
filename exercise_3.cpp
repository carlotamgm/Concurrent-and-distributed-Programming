//------------------------------------------------------------------------------
// File:   ejercicio_3.cpp
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
#include <cmath>

using namespace std;


const int MAX_DATOS = 100;
bool acabado = false;

void calcMedia(double datos[MAX_DATOS], double& media, int retardo, bool& acabado) {

    media=0.0;
    for(int i=0; i<MAX_DATOS; i++) {
        media+=datos[i];
    }
    media=media/MAX_DATOS;
    acabado=true;
    //el thread que me ejecuta se bloquea hasta se hayan cargado los datos
    this_thread::sleep_for(chrono::milliseconds(retardo));
}

void maxmin(double datos[MAX_DATOS], double& max, double& min, int retardo) {
    min=datos[0];
    for(int i=1; i<MAX_DATOS; i++) {
        if(datos[i] < min) {
            min = datos[i];
        } if(datos[i] > max) {
            max = datos[i];
        }
    }

    //el thread que me ejecuta se bloquea hasta se hayan cargado los datos
    this_thread::sleep_for(chrono::milliseconds(retardo));
}

void sigma(double datos[MAX_DATOS], double& desviacion, double media, int retardo) {
    double suma=0.0;
    for(int i=0; i<MAX_DATOS; i++) {
        suma += pow((datos[i]-media), 2);
    }
    desviacion = sqrt(suma/MAX_DATOS);

    //el thread que me ejecuta se bloquea hasta que se calcule la media
    this_thread::sleep_for(chrono::milliseconds(retardo));
}

int main(int argc, char* argv[]) {

    srand(time(NULL)); //semilla de aleatorios

    double media, desviacion, max, min;
    double datos[MAX_DATOS];
    const int MAX = 10000;

    for(int i=0; i<MAX_DATOS; i++) { //declaro vector de reales aleatorios
        datos[i] = (double)rand()/((double)(RAND_MAX/(MAX-1)));
    }

    thread th_1(&calcMedia, datos, ref(media), 100, ref(acabado));   //th_1 se pone en marcha
    th_1.join();
    if(acabado) {
        thread th_3(&sigma, datos, ref(desviacion), media, 150); //th_3 se pone en marcha
        th_3.join(); 
    }
    thread th_2(&maxmin, datos,ref(max),ref(min), 150); //th_2 se pone en marcha
    th_2.join();

    cout << "# datos: " + to_string(MAX_DATOS) + "\n";
    cout << "media: " + to_string(media) + "\n";
    cout << "máx: " + to_string(max) +"\n";
    cout << "min: " + to_string(min) + "\n";
    cout << "sigma: " + to_string(desviacion) + "\n";    
    return 0;
}

