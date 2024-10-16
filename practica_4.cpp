
/*
* Autores: Ignacio Millán Aznar, NIP: 841537 y Carlota Moncasi Gosá, NIP: 839841
* PSCD - Práctica 4 (compilar mediante makefile)
*/

#include <fstream>
#include "controlParejas.hpp"


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
// Pre : 
// Post : El proceso profesor espera a que haya dos estudiantes y se sienten, asigna a cada uno su pareja y fila (y pasa el testigo) y espera a que todos hayan terminado su trabajo para comunicar la
//        finalización del examen (y pasar el testigo)

void Profesor (controlParejas& control) {

    for( int i=0; i< N_FIL ; i++) {
        control.asignar(i);
    }
    control.contadorTerminadas();
}

// ----------------------------------------------------
// Pre : 
// Post : El proceso estudiante llega y espera hasta que haya una silla libre (de las dos), entonces se sienta(y pasa el testigo). Después espera a que el profesor le asigne su pareja y fila correspondientes
//      (y pasa el testigo). El estudiante de NIP menor calcula el máximo de la fila y lo comunica a su pareja(y pasa el testigo), mientras que el de NIP mayor calcula la suma de los elementos de la fila y al terminar, 
//      muestra los resultados suyos (suma) y de su pareja (máximo) por pantalla (y pasa el testigo)

void Estudiante ( int nip, int D[ N_FIL ][ N_COL ], int resultados[], controlParejas& control ) {

    int result1 = 0;
    int result2 = 0;
    control.sillaLibre(nip); 
    control.filaAsignada(nip);

    if (nip<control.dimePareja(nip) ){
        result1=maxFila(D, control.dimeFila(nip));
        control.resultadoMenor(result1, nip, resultados);
    }else{
        result2=sumaFila(D, control.dimeFila(nip));
        control.resultadoMayor(result2, nip,  resultados); 
        control.acabanEstudiantes(nip,  resultados);
    }
}



// ----------------------------------------------------
// Pre : el vector nips[2] representa las sillas (máximo 2) que albergan, cada una de ellas, un nip de un estudiante de la pareja. El resto de variables contienen lo que su propio nombre indica.
//       Los semáforos "bHayHueco" y "bNoHayHueco" indican el estado de las sillas con el contador "sillasOcupadas", mientras que "bContTerminados" se refiere a las parejas terminadas(y "parejasTerminadas" cuenta el número de parejas terminadas)
// Post :  Abre el fichero "datos.txt" y vuelca en la matriz D todos los datos sobre los que van a trabajar los estudiantes. Después, llama al proceso Profesor y al proceso Estudiante para que hagan su trabajo y cuando terminan,
//         se da por finalizado el examen.

int main () {

    controlParejas control;

    int D[ N_FIL ][ N_COL ]; // para almacenar los datos
    int nip = 0;
    int resultados[N_EST];
    
    // cargar " datos.txt " en "D"
    ifstream f;
    f.open("datos.txt");
    if (f.is_open()) {

        for(int i = 0; i < N_FIL; i++){
            for(int j = 0; j<N_COL; j++){
                f>>D[i][j];
            }
        }
    
        f.close();
    }
    else{
        cerr << "NO SE HA PODIDO ABRIR." << endl;
    }
    
    //threads
    thread P, Q[N_EST];
    P=thread(&Profesor, ref(control)); 
   
    for(int i=0;i<N_EST;i++) {
        Q[i]=thread(&Estudiante, i, D, resultados, ref(control));
    }
    
    
    P.join();

	for(int j=0; j<N_EST; j++) {
		Q[j].join(); 	
	}
    
    cout << "\n Prueba finalizada \n";

    return 0;
}