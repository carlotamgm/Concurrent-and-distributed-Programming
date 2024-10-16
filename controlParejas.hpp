/*
* Autores: Ignacio Millán Aznar, NIP: 841537 y Carlota Moncasi Gosá, NIP: 839841
* PSCD - Práctica 4 (compilar mediante makefile)
*/

#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "./librerias/Logger_V3/Logger.hpp"

const int N_EST = 60; //# de estudiantes
const int N_FIL = N_EST /2; //# de filas en la matriz
const int N_COL = 1000; //# de columnas

class controlParejas {
    public:
        /* ----------------------------------------------------
        * Pre : 
        * Post : En el constructor se inicializan los vectores
        */
        controlParejas();

        /* ----------------------------------------------------
        * Pre : i es el identificador de una fila, por lo tanto su valor estará entre 0 y N_FIL
        * Post : En la función asignar se le asignan a cierto estudiante una fila y una pareja,
        * el contador de sillas ocupadas se pone a 0 y se da la señal para que pase la siguiente
        * pareja y se da también la señal de que la pareja esta comunicada entre sí. 
        */
        void asignar(int i );

        /* ----------------------------------------------------
        * Pre : 
        * Post : En la función contadorTerminadas se espera a que todas las parejas hayan terminado 
        * con un bucle en el que se entra si el contador de parejas terminadas es igual al número de 
        * filas (que también es el número de parejas totales) y dentro del bucle se espera a la condición 
        * parejastermC cuya señal se mandará cuando hayan terminado las parejas. 
        */

        void contadorTerminadas();

        /* ----------------------------------------------------
        * Pre : 
        * Post : Esta función se llama desde el estudiante y dentro de ella se espera a que haya 
        * sillas libres, si las hay apunta su nip en el vector de nips (con una componente para cada 
        * uno de la pareja) y sumará uno al contador de sillas, si ese contador llega a dos 
        * (número de sillas), mandará una señal que se recibirá en la función asignar para comunicar a la pareja.
        */

        void sillaLibre(int nip);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : Esta función únicamente espera a que la función asignar comunique a la pareja.
        */

        void filaAsignada(int nip);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : función que devuelve el número de fila que se le ha asignado al estudiante. 
        */
        int dimeFila(int nip);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : función que devuelve la pareja que se le ha asignado al estudiante. 
        */
        int dimePareja(int nip);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : Esta función asigna al vector resultados del monitor el resultado 
        * que ha obtenido el estudiante de nip menor, después le manda la señal al 
        * de nip mayor para que de su resultado.
        */

        void resultadoMenor(int result, int nip, int resultados[]);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : Esta función, que representa al estudiante de nip mayor, espera 
        * a que el estudiante de nip menor de la pareja mande la señal de que ha 
        * acabado su parte, después asigna al vector resultados del monitor el resultado que ha 
        * obtenido del estudiante de nip menor y finalmente, le manda la señal al de nip mayor para
        * que dé su resultado.
        */
        void resultadoMayor(int result, int nip, int resultados[]);

        /* ----------------------------------------------------
        * Pre : 0 <= nip <= N_EST
        * Post : Esta función asigna comprueba si ya han acabado todas las parejas.
        */
        void acabanEstudiantes(int nip, int resultados[]);
       
       
    
    private:
        int pareja [ N_EST ]; // pareja [i] será la pareja asignada
        int sillasOcupadas = 0;
        int nips[2];
        bool aviso[N_EST];
        int filas[N_EST];
        int parejasTerminadas=0;
        mutex mtxMonitor; //el testigo
        condition_variable parejasTermC; //condición para saber cuando acaban todas las parejas
        condition_variable sillasListas; //condición para cuando dos estudiantes hayan ocupado las sillas 
        condition_variable filaAsignC; //condición para cuando se hayan asignado las filas y las parejas
        condition_variable haySillas; //condición para saber si hay sillas disponibles
        condition_variable acabaMiPareja; //condición para saber si ha acabado el estudiante de la pareja con nip menor
        
};