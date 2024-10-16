//------------------------------------------------------------------------------
// File:   main_p2_e1.cpp
// Author: PSCD-Unizar
// Date:   septiembre 2020
// Coms:   Parte del práctica 2 de PSCD
//         Compilar mediante
//           g++ main_p2_e1.cpp -o main_p2_e1 -std=c++11 -pthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <atomic>

const int N=512;
const int N_BUSC=8; //# de procesos buscadores
using namespace std;
using Vectint= int[N]; //"VectInt" es un "alias" para vector de int de dim. N
atomic_flag tas=ATOMIC_FLAG_INIT; //false

const int MAX_DATOS = 100;
bool acabado = false;

//-----------------------------------------------------
//Pre: 0 ≤ i ≤ d ≤ N − 1
//Post: result = Num α ∈ [i, d].value = v[α]
//      maxVeces = Max α ∈ [i, d].result = result[α]
//      indMin = i ∈ [0, N-1].maxVeces = v[i, d]
//      indMax = i ∈ [0, N-1].maxVeces = v[i, d]

void search(const int v[],const int i, const int d, const int value, int& result, 
            int& maxVeces, int& indMax, int& indMin, atomic_flag& tas) {

    while (tas.test_and_set()) {
	   this_thread::yield();
	}

    for(int j=i; j<=d; j++) {

       if (v[j]==value){
        result++;
       } 
    }
	if (result>maxVeces) {
		maxVeces=result;
		indMin=i;
		indMax=d;
	}

    tas.clear(); 
}

//-----------------------------------------------------
//Pre: 
//Post: 1. Carga los datos del vector desde el fichero “datos.txt” que se suministra 
//                  con el material adicional de la práctica
//      2. “Avisa” a los procesos buscadores de que los datos están cargados
//      3. Espera a que cada proceso buscador termine su trabajo y le “avise” de ello
//      4. Muestra por la salida estándar el número total de veces que aparece el valor 
//          buscado en el vector

void coordinador(int v[], bool& noCargado, bool acabado[]) {

    
    ifstream f("datos.txt"); 
    if(f.is_open()) {
        int j=0;
        while (!f.eof()) {
            f>>v[j];
            j++;
        }
    }
    noCargado=false;
    bool todoAcabado=false; 
    int m=0; 

    while(!todoAcabado){

        while(acabado[m]&& m<N_BUSC) {
            m++;
        } 
        if(acabado[7]) {
            todoAcabado=true; 
        }
    }
}

//-----------------------------------------------------
//Pre: La variable del tipo std::atomic_flag pasada por referencia permite utilizar test-and-set para asegurar el acceso en exclusión mutua
//Post: 1. Espera a que el proceso coordinador haya cargado los datos
//      2. Lleva a cabo su trabajo, calculando el número de veces que aparece el valor ("value") 
//          y actualiza el valor de las variables "maxVeces", "indMin" e "indMax" del trozo correspondiente
//      3. “Avisa” al coordinador de que ha finalizado su trabajo, y termina

void buscador(const int v[],const int i, const int d, const int value, int& result, bool& noCargado, bool& acabado, 
            int& maxVeces, int& indMin, int& indMax, atomic_flag& tas) {

    while(noCargado){
        this_thread::yield();
    }
    search(v, i, d, value, result, maxVeces, indMin, indMax, tas);
    acabado=true;

}
//-----------------------------------------------------
//Pre: 
//Post: Solicita al usuario que introduzca el entero a buscar en el vector, un número entre 1 y 25, 
//      lanza los 9 procesos y espera a que terminen. Después muestra por pantalla el valor de las variables "maxVeces", "indMin" e "indMax"
//      El ejercicio tiene como objetivo calcular el número de veces que aparece el valor introducido de un número entero en un vector v
//      mediante la ejecución de 8 procesos buscadores y 1 proceso coordinador.

int main(int argc, char* argv[]) {

    int v[N]={}, i, d, value, maxVeces=0, indMax, indMin;
    cout<<"Introduzca el valor que quiere buscar en el vector: \n";
	cin>>value; 
	 
    bool noCargado=true, acabado[N_BUSC];
    int result[N_BUSC]={};

    for(int j=0; j<N_BUSC; j++) {
        acabado[j]=false; 
    }

    thread P, Q[N_BUSC]; 
    P=thread(&coordinador, v, ref(noCargado), acabado);

    for (int j=0; j<N_BUSC; j++) {
        i=j*N/N_BUSC; 
        d=((j+1)*N/N_BUSC)-1;
	    Q[j]=thread(&buscador, v, i, d, value, ref(result[j]), ref(noCargado), ref(acabado[j]), ref(maxVeces), ref(indMin), 
                ref(indMax), ref(tas));
    }

	P.join();

	for(int j=0; j<N_BUSC; j++) {
        Q[j].join(); 
    }

	cout << "maxVeces: "<< maxVeces<< endl;
	cout << "indMin: "<< indMin << endl; 
	cout << "indMax: "<< indMax << endl;

    return 0;
}

