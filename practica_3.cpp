/*
* Autores: Ignacio Millán Aznar, NIP: 841537 y Carlota Moncasi Gosá, NIP: 839841
* PSCD - Práctica 3 (compilar mediante makefile)
*/


#include "./librerias/Semaphore_V4/Semaphore_V4.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <iomanip>
using namespace std;
// ----------------------------------------------------
const int N_EST = 60; //# de estudiantes
const int N_FIL = N_EST /2; //# de filas en la matriz
const int N_COL = 1000; //# de columnas

// ----------------------------------------------------

// Pre : <fila > es un índice de fila de <D>
// Post : devuelve el máximo de la fila <fila>
int maxFila (int D[ N_FIL ][ N_COL ], int fila ) {
    int max = D[fila][0];
	for (int i = 1; i<N_COL; i++){
		if(D[fila][i] > max){
			max = D[fila][i];
		}
	}
    return max ;
}

// ----------------------------------------------------
// Pre : <fila> es un índice de fila de <D>
// Post : devuelve la suma de los elementos de la fila <fila>
int sumaFila ( int D[ N_FIL ][ N_COL ], int fila ) {
    int sum = 0;
	for (int i = 0; i<N_COL; i++){
		sum = sum + D[fila][i];
	}
	return sum ;
}

// ----------------------------------------------------
// Pre : Un proceso ha acabado su ejecución y quiere despertar a otro que sigue bloqueado en wait() y pasarle el testigo(desbloquearlo)
// Post : Si se cumple una de las condiciones, desbloquea el proceso en el semáforo respectivo (signal() de b...) a la condición y decrementa en 1 los permisos del mismo (int d...--)
void pasarTestigo(int pareja[], int filas[], int& sillasOcupadas,  Semaphore* bAsignado[], int dAsignado[], Semaphore* bTerminado[], int dTerminado[], Semaphore& bHayHueco, int& dHayHueco, 
                    Semaphore& bNoHayHueco, int& dNoHayHueco, Semaphore& testigo, bool aviso[], int& contadorTerminados ,Semaphore& bContTerminados, int& parejasTerminadas) {

    int i=0;
    bool pasado= false;
    while(i<N_EST && !pasado)  {
    
        if (dAsignado[i] > 0 && pareja[i] != -1) {  //&& se sabe que filas[nip]!=-1
            dAsignado[i]--;
            bAsignado[i]->signal();
            pasado=true;
        } 
        else if(dTerminado[i] > 0 && aviso[i]) {
            dTerminado[i]--;
            dTerminado[pareja[i]]--;
            bTerminado[i]->signal();
            bTerminado[pareja[i]]->signal();
            pasado=true;
        }
        i++;
    }
    if(!pasado) {
        if (dHayHueco > 0 && sillasOcupadas < 2 ) {
            dHayHueco--;
            bHayHueco.signal();
        }
        else if (dNoHayHueco > 0 && sillasOcupadas == 2) {
            dNoHayHueco--;
            bNoHayHueco.signal();
        }
        else if(contadorTerminados>0 && parejasTerminadas == N_FIL ){ // await parejasTerminadasinadas = N_FIL
            contadorTerminados--;
            bContTerminados.signal();
        }
        else {
            testigo.signal();
        }
    }
    
}

// ----------------------------------------------------
// Pre : 
// Post : El proceso profesor espera a que haya dos estudiantes y se sienten, asigna a cada uno su pareja y fila (y pasa el testigo) y espera a que todos hayan terminado su trabajo para comunicar la
//        finalización del examen (y pasar el testigo)

void Profesor (int pareja[], int nips[], int& sillasOcupadas,  Semaphore* bAsignado[], int dAsignado[], Semaphore* bTerminado[],
        int dTerminado[], Semaphore& bHayHueco, int& dHayHueco, Semaphore& bNoHayHueco, int& dNoHayHueco, Semaphore& testigo, bool aviso[], int filas[], int resultados[],
        int& contadorTerminados, Semaphore& bContTerminados, int& parejasTerminadas) {
    
    for(int i=0; i< N_FIL ; i++) {
        // esperar a que haya dos estudiantes
        testigo.wait();
        if (!(sillasOcupadas == 2)) {
            dNoHayHueco++;
            testigo.signal();
            bNoHayHueco.wait(); //me bloqueo en semáforo
        }
        // comunicar a cada uno su pareja, y la fila que les toca
        pareja[nips[0]]=nips[1];
        pareja[nips[1]]=nips[0];
        filas[nips[0]]=i;
        filas[nips[1]]=i;
        sillasOcupadas=0; //vacía sillas
        pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);

    }
    //esperar a que todas las parejas hayan terminado
    testigo.wait();
    if(parejasTerminadas != N_FIL){
        contadorTerminados++; 
        testigo.signal();
        bContTerminados.wait();
    }
    pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);
}

// ----------------------------------------------------
// Pre : 
// Post : El proceso estudiante llega y espera hasta que haya una silla libre (de las dos), entonces se sienta(y pasa el testigo). Después espera a que el profesor le asigne su pareja y fila correspondientes
//      (y pasa el testigo). El estudiante de NIP menor calcula el máximo de la fila y lo comunica a su pareja(y pasa el testigo), mientras que el de NIP mayor calcula la suma de los elementos de la fila y al terminar, 
//      muestra los resultados suyos (suma) y de su pareja (máximo) por pantalla (y pasa el testigo)

void Estudiante ( int nip, int D[ N_FIL ][ N_COL ], int pareja[], int nips[], int& sillasOcupadas,  Semaphore* bAsignado[], int dAsignado[], Semaphore* bTerminado[], 
        int dTerminado[], Semaphore& bHayHueco, int& dHayHueco, Semaphore& bNoHayHueco, int& dNoHayHueco, Semaphore& testigo, bool aviso[], int filas[], int resultados[],
        int& contadorTerminados, Semaphore& bContTerminados, int& parejasTerminadas) {

    // esperar por una silla libre
    testigo.wait();
    if(sillasOcupadas>=2) {
        dHayHueco++;
        testigo.signal();
        bHayHueco.wait(); //me bloqueo en semáforo
    }
    nips[sillasOcupadas]=nip;
    sillasOcupadas++;
    pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);

    // esperar a que me sea asignada pareja y fila
    testigo.wait();
    if(!(pareja[nip]!=-1)) { // && se sabe que filas[nip]!=-1
        dAsignado[nip]++;
        testigo.signal();
        bAsignado[nip]->wait(); //me bloqueo en semáforo
    }
    pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);
    if (nip < pareja[nip]) {
        // calcular máx de mi fila
        int max = maxFila(D, filas[nip]);
        testigo.wait();
        resultados[nip]=max;
        // hacérselo llegar a mi pareja
        aviso[nip]=true;
        pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);
    } else {
        // calcular la suma de mi fila
        int suma = sumaFila(D, filas[nip]);
        // coger resultado de max (de mi pareja)
        testigo.wait();
        if(!(aviso[pareja[nip]])) {
            dTerminado[pareja[nip]]++;
            dTerminado[nip]++;
            testigo.signal();
            bTerminado[pareja[nip]]->wait(); //me bloqueo en semáforo
            bTerminado[nip]->wait(); //me bloqueo en semáforo
        }

        // mostrar resultados
        cout <<setw(3)<< filas[nip] << "|" <<setw(3)<< pareja[nip] << "-" <<setw(2)<< nip << " |" <<setw(6)<< resultados[pareja[nip]] << " | " << suma << endl; 
        // comunicar finalización
        parejasTerminadas++;
        pasarTestigo(pareja, filas, sillasOcupadas, bAsignado, dAsignado, bTerminado, dTerminado, bHayHueco, dHayHueco, bNoHayHueco, dNoHayHueco, testigo, aviso, contadorTerminados, bContTerminados, parejasTerminadas);
    }
}

// ----------------------------------------------------
// Pre : "Semaphore* bAsignado[N_EST]" y "Semaphore* bTerminado[N_EST]" son vectores de semáforo referenciados por puntero
// Post : elimina el espacio ocupado por los arrays en memoria, después de ser utilizados

void borrarMem(Semaphore* bAsignado[N_EST], Semaphore* bTerminado[N_EST]) {
    delete(*bTerminado);
    delete(*bAsignado);
}

// ----------------------------------------------------
// Pre : el vector nips[2] representa las sillas (máximo 2) que albergan, cada una de ellas, un nip de un estudiante de la pareja. El resto de variables contienen lo que su propio nombre indica.
//       Los semáforos "bHayHueco" y "bNoHayHueco" indican el estado de las sillas con el contador "sillasOcupadas", mientras que "bContTerminados" se refiere a las parejas terminadas(y "parejasTerminadas" cuenta el número de parejas terminadas)
// Post :  Abre el fichero "data_3.txt" y vuelca en la matriz D todos los datos sobre los que van a trabajar los estudiantes. Después llama al proceso Profesor y al proceso Estudiante para que hagan su trabajo y cuando terminan,
//         se da por finalizado el examen.

int main () {
    int D[ N_FIL ][ N_COL ]; // para almacenar los datos
    int fila = 0; // cada pareja cogerá una
    int pareja [ N_EST ]; // pareja [i] será la pareja asignada
    int sillasOcupadas = 0;
    int nips[2];
    int resultados[N_EST];
    int filas[N_EST];
    bool aviso[N_EST];
    int nip = 0;
    int parejasTerminadas=0;

    Semaphore testigo(1);

    Semaphore bHayHueco(0);
    int dHayHueco = 0;
    Semaphore* bAsignado[N_EST];
    int dAsignado[N_EST];
    Semaphore* bTerminado[N_EST];
    int dTerminado[N_EST];
    Semaphore bNoHayHueco(0);
    int dNoHayHueco = 0;
    Semaphore bContTerminados(0);
    int contadorTerminados=0;

    //inicializar vectores
    for(unsigned i=0; i<N_EST; i++){
        resultados[i]=-1;          
        filas[i]=-1;
        aviso[i]=false;
        bAsignado[i] = new Semaphore(0);
        bTerminado[i] = new Semaphore(0);
        pareja[i]=-1;
        dAsignado[i]=0;
        dTerminado[i]=0;
    }
    
    // cargar " data_3.txt " en "D"
    ifstream f;
    f.open("data_3.txt");
    if (f.is_open()) {
        while(!f.eof()) {
            for(int i = 0; i < N_FIL; i++){
                for(int j = 0; j<N_COL; j++){
                    f>>D[i][j];
                }
            }
        }
        f.close();
    }
    else{
        cerr << "NO SE HA PODIDO ABRIR." << endl;
    }

    //threads
    thread P, Q[N_EST];
    P=thread(&Profesor, pareja, nips, ref(sillasOcupadas), bAsignado, dAsignado, bTerminado, 
        dTerminado, ref(bHayHueco), ref(dHayHueco), ref(bNoHayHueco), ref(dNoHayHueco), ref(testigo), aviso, filas, resultados, ref(contadorTerminados), ref(bContTerminados), ref(parejasTerminadas)); 
   
    for(int i=0;i<N_EST;i++) {
        Q[i]=thread(&Estudiante, i, D, pareja, nips, ref(sillasOcupadas), bAsignado,dAsignado, bTerminado, 
        dTerminado, ref(bHayHueco), ref(dHayHueco), ref(bNoHayHueco), ref(dNoHayHueco), ref(testigo), aviso, filas, resultados, ref(contadorTerminados), ref(bContTerminados), ref(parejasTerminadas));
    }
    
    
    P.join();

	for(int j=0; j<N_EST; j++) {
		Q[j].join(); 	
	}
    
    cout << "\n Prueba finalizada \n";
    borrarMem(bTerminado, bAsignado); //ya no se utilizan más

    return 0;
}