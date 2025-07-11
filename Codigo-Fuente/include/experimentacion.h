#ifndef EXPERIMENTACION_H
#define EXPERIMENTACION_H

#include <vector>
#include <string>

using namespace std;

// Constantes para la experimentación
const int REP = 100000;
const int DIRECTORIOS_INSERCION = 2000;

// Estructuras para almacenar resultados
struct ResultadoExperimento {
    int tamaño;
    double tiempoCreacion;
    double tiempoPromedioBusqueda;
    double tiempoPromedioEliminacion;
    double tiempoPromedioInsercion;
};

// Clase para manejar los experimentos
class ExperimentacionArbol {
private:
    ArbolSistemaArchivos* arbol;
    vector<string> rutasDisponibles;
    
    // Funciones auxiliares
    vector<string> seleccionarRutasAleatorios(int cantidad);
    vector<string> seleccionarDirectoriosAleatorios(int cantidad);
    void generarDatos(int numDirectorios, int numArchivos, const string& directorio);

public:
    // Constructor
    ExperimentacionArbol();
    
    // Destructor
    ~ExperimentacionArbol();
    
    // Ejecutar experimento completo
    ResultadoExperimento ejecutarExperimento(int numDirectorios, int numArchivos, const string& directorio);
    
    // Experimentos individuales
    double medirTiempoCreacion(const string& directorio);
    double medirTiempoBusqueda(int repeticiones);
    double medirTiempoEliminacion(int repeticiones);
    double medirTiempoInsercion(int repeticiones);
    
    // Generar reportes
    void generarReporte(const vector<ResultadoExperimento>& resultados);
    void ejecutarTodosLosExperimentos();
};

#endif // EXPERIMENTACION_H
