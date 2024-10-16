/*
* Autores: Ignacio Millán Aznar, NIP: 841537 y Carlota Moncasi Gosá, NIP: 839841
* PSCD - Práctica 4 (compilar mediante makefile)
*/

#include "controlParejas.hpp"
using namespace std; 

/* ----------------------------------------------------
* Pre : 
* Post : En el constructor se inicializan los vectores.
*/
controlParejas::controlParejas(){ //constructor
    
    for(unsigned i=0; i<N_EST; i++) {      
        filas[i]=-1;
        pareja[i]=-1;
        aviso[i]=false;
    }
}

/* ----------------------------------------------------
* Pre : i es el identificador de una fila, por lo tanto su valor estará entre 0 y N_FIL
* Post : En la función asignar se le asignan a cierto estudiante una fila y una pareja,
* el contador de sillas ocupadas se pone a 0 y se da la señal para que pase la siguiente
* pareja y se da también la señal de que la pareja está comunicada entre sí. 
*/

void controlParejas::asignar(int i){
    unique_lock<mutex> lck(mtxMonitor);
    while(sillasOcupadas<2) {
        sillasListas.wait(lck);
    }
    pareja[nips[0]]=nips[1];
    pareja[nips[1]]=nips[0];
    filas[nips[0]]=i;
    filas[nips[1]]=i;
    sillasOcupadas=0;
    haySillas.notify_all();
    filaAsignC.notify_all();
};

/* ----------------------------------------------------
* Pre : 
* Post : En la función contadorTerminadas se espera a que todas las parejas hayan terminado 
* con un bucle en el que se entra si el contador de parejas terminadas es igual al número de 
* filas (que también es el número de parejas totales) y dentro del bucle se espera a la condición 
* parejastermC cuya señal se mandará cuando hayan terminado las parejas. 
*/

void controlParejas::contadorTerminadas(){
    unique_lock<mutex> lck(mtxMonitor);
    while(parejasTerminadas<N_FIL){
        parejasTermC.wait(lck);
    }
};

/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : Esta función se llama desde el estudiante y dentro de ella se espera a que haya 
* sillas libres, si las hay, apunta su nip en el vector de nips (con una componente para cada 
* uno de la pareja) y sumará uno al contador de sillas. Si ese contador llega a dos 
* (número de sillas), mandará una señal que se recibirá en la función asignar para comunicar a la pareja.
*/

void controlParejas::sillaLibre(int nip){
    unique_lock<mutex> lck(mtxMonitor);
    while(sillasOcupadas==2){
        haySillas.wait(lck);
    }
    nips[sillasOcupadas] = nip; //me siento
    sillasOcupadas++;
    if(sillasOcupadas == 2) {
        sillasListas.notify_all();
    } 
   
};

/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : Esta función únicamente espera a que la función asignar comunique a la pareja.
*/

void controlParejas::filaAsignada(int nip){
    unique_lock<mutex> lck(mtxMonitor);
    while(filas[nip]==-1) {
        filaAsignC.wait(lck);
    }
};
/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : función que devuelve el número de fila que se le ha asignado al estudiante.
*/

int controlParejas::dimeFila(int nip) {
    unique_lock<mutex> lck(mtxMonitor);
    return filas[nip];
}
/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : función que devuelve la pareja que se le ha asignado al estudiante. 
*/
int controlParejas::dimePareja(int nip) {
    unique_lock<mutex> lck(mtxMonitor);
    return pareja[nip];
}

/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : Esta función asigna al vector resultados del monitor el resultado 
* que ha obtenido el estudiante de nip menor, después le manda la señal al 
* de nip mayor para que dé su resultado.
*/

void controlParejas::resultadoMenor(int result1, int nip, int resultados[]){
    unique_lock<mutex> lck(mtxMonitor);
    resultados[nip]=result1;
    aviso[nip] = true;
    acabaMiPareja.notify_all();
};

/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : Esta función, que representa al estudiante de nip mayor, espera 
* a que el estudiante de nip menor de la pareja mande la señal de que ha 
* acabado su parte, después asigna al vector resultados del monitor el resultado que ha 
* obtenido del estudiante de nip menor y finalmente, le manda la señal al de nip mayor para
* que dé su resultado.
*/

void controlParejas::resultadoMayor(int result2, int nip, int resultados[]){
    unique_lock<mutex> lck(mtxMonitor);
    while(!(aviso[pareja[nip]])){
        acabaMiPareja.wait(lck);
    }
    resultados[nip]=result2;
    cout <<setw(3)<< filas[nip] << "|" <<setw(3)<< pareja[nip] << "-" <<setw(2)<< nip << " |" <<setw(6)<< resultados[pareja[nip]] << " | " << resultados[nip] << endl; 
    parejasTerminadas++;
};
/* ----------------------------------------------------
* Pre : 0 <= nip <= N_EST
* Post : Esta función asigna comprueba si ya han acabado todas las parejas.
*/

void controlParejas::acabanEstudiantes(int nip, int resultados[]){
    unique_lock<mutex> lck(mtxMonitor);
    
    if (parejasTerminadas == N_FIL) {
        parejasTermC.notify_one();
    }
    
};



