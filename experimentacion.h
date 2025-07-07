#ifndef EXPERIMENTACION_H
#define EXPERIMENTACION_H

#include "arbol.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct ResultadoExperimento {
    long long tiempoCreacion;
    long long tiempoBusquedas;
    long long tiempoEliminaciones;
    long long tiempoInserciones;
    double tiempoPromedioBusqueda;
    double tiempoPromedioEliminacion;
    double tiempoPromedioInsercion;
    int totalNodos;
    int totalArchivos;
    int alturaMaxima;
    string configuracion;
};

class Experimentacion {
private:
    SistemaArchivos* sistema;
    
public:
    Experimentacion();
    ~Experimentacion();
    
    // Crear estructura de archivos sintética para pruebas
    void crearEstructuraSintetica(const string& rutaBase, int numDirectorios, int numArchivos);
    
    // Experimento de tiempo de creación
    long long medirTiempoCreacion(const string& rutaBase);
    
    // Experimento de búsquedas
    long long medirTiempoBusquedas(int repeticiones);
    
    // Experimento de eliminaciones
    long long medirTiempoEliminaciones(int repeticiones);
    
    // Experimento de inserciones
    long long medirTiempoInserciones(int repeticiones);
    
    // Ejecutar experimento completo
    ResultadoExperimento ejecutarExperimentoCompleto(const string& rutaBase, const string& configuracion);
    
    // Ejecutar todos los experimentos con diferentes configuraciones
    vector<ResultadoExperimento> ejecutarTodosLosExperimentos();
    
    // Generar archivo CSV con resultados
    void generarCSV(const vector<ResultadoExperimento>& resultados, const string& nombreArchivo);
    
    // Imprimir resultados en consola
    void imprimirResultados(const ResultadoExperimento& resultado);
    
    // Validar implementación con casos de prueba
    bool validarImplementacion();
    
    // Obtener referencia al sistema (para pruebas)
    SistemaArchivos* obtenerSistema() { return sistema; }
    
private:
    // Generar nombres de archivos aleatorios
    vector<string> generarNombresArchivos(int cantidad);
    
    // Crear directorio y archivos sintéticos
    void crearDirectoriosSinteticos(const string& rutaBase, int profundidad, int anchura, int& contadorDir, int& contadorArch, int maxDir, int maxArch);
};

#endif // EXPERIMENTACION_H