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

const int N=512;
const int N_BUSC=8; //# de procesos buscadores
using namespace std;
using Vectint= int[N]; //"VectInt" es un "alias" para vector de int de dim. N

const int MAX_DATOS = 100;
bool acabado = false;



//-----------------------------------------------------
//Pre: 0 ≤ i ≤ d ≤ N − 1
//Post: result = Num α ∈ [i, d].value = v[α]

void search(const int v[],const int i, const int d, const int value, int& result) { 
    
    for(int j=i; j<=d; j++) {
       if (v[j]==value) {
            result++;
       } 
    }
    
}


//-----------------------------------------------------
//Pre: 
//Post: 1. Carga los datos del vector desde el fichero “data_2.txt” que se suministra 
//                  con el material adicional de la práctica
//      2. “Avisa” a los procesos buscadores de que los datos están cargados
//      3. Espera a que cada proceso buscador termine su trabajo y le “avise” de ello
//      4. Muestra por la salida estándar el número total de veces que aparece el valor 
//          buscado en el vector

void coordinador(int v[], bool& noCargado, bool acabado[]) {

    ifstream f("data_2.txt"); 
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
    while(!todoAcabado) {
        while(acabado[m]&& m<N_BUSC) {
            m++;
        } 
        if(acabado[7]) {
            todoAcabado=true; 
        }
    } 
}

//-----------------------------------------------------
//Pre: 
//Post: 1. Espera a que el proceso coordinador haya cargado los datos
//      2. Lleva a cabo su trabajo, calculando el número de veces que aparece el valor ("value") del trozo correspondiente
//      3. “Avisa” al coordinador de que ha finalizado su trabajo, y termina

void buscador(const int v[],const int i, const int d, const int value, int& result, bool& noCargado, bool& acabado) {
    while(noCargado) {
        this_thread::yield();
    }

    search(v,i,d,value,result);
    acabado=true;

}

//-----------------------------------------------------
//Pre: 
//Post: Solicita al usuario que introduzca el entero a buscar en el vector, un número entre 1 y 25, 
//      lanza los 9 procesos y espera a que terminen.
//		El ejercicio tiene como objetivo calcular el número de veces que aparece el valor introducido de un número entero en un vector v
//		mediante la ejecución de 8 procesos buscadores y 1 proceso coordinador.

int main(int argc, char* argv[]) {

    int v[N]={}, i, d, value;
	bool noCargado=true, acabado[N_BUSC];
    int result[N_BUSC]={};

	cout<<"introduzca el valor que quiere buscar en el vector: ";
	cin>>value; 

	for(int j=0; j<N_BUSC; j++){
		acabado[j]=false; 
	}

	thread P, Q[N_BUSC]; 
	P=thread(&coordinador,v, ref(noCargado),acabado); 

	for (int j=0; j<N_BUSC; j++){
		i=j*N/N_BUSC; 
		d=((j+1)*N/N_BUSC)-1;
		Q[j]=thread(&buscador, v, i, d,value, ref(result[j]), ref(noCargado), ref(acabado[j]));
	}

	P.join();

	for(int j=0; j<N_BUSC; j++) {
		Q[j].join(); 	
	}

	int total=0; 
	for(int j=0;j<N_BUSC; j++){
		total+=result[j];
	}

	cout<<"el valor total de veces que aparece el valor "<< value <<" en el vector es: "<< total <<endl;
	return 0;
	}

