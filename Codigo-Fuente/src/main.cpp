#include "tree.h"
#include "experimentacion.h"
#include <iostream> 

using namespace std;

int main() {
    cout << "=== SISTEMA DE ARCHIVOS CON ÁRBOL K-ARIO ===" << endl;
    cout << "Iniciando experimentos..." << endl;
    
    ExperimentacionArbol experimento;
    experimento.ejecutarTodosLosExperimentos();
    
    cout << "Experimentos completados." << endl;
    
    return 0;
}
