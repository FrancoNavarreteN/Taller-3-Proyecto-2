#include "experimentacion.h"
#include "tree.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <cstdlib>

// Constructor
ExperimentacionArbol::ExperimentacionArbol() {
    arbol = new ArbolSistemaArchivos();
}

// Destructor
ExperimentacionArbol::~ExperimentacionArbol() {
    delete arbol;
}

// Medir tiempo entre dos puntos
double ExperimentacionArbol::medirTiempo(chrono::high_resolution_clock::time_point inicio, 
                                       chrono::high_resolution_clock::time_point fin) {
    auto duracion = chrono::duration_cast<chrono::nanoseconds>(fin - inicio);
    return static_cast<double>(duracion.count());
}

// Seleccionar rutas aleatorias
vector<string> ExperimentacionArbol::seleccionarRutasAleatorias(int cantidad) {
    vector<string> seleccionadas;
    if (rutasDisponibles.empty()) return seleccionadas;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, rutasDisponibles.size() - 1);
    
    for (int i = 0; i < cantidad && i < static_cast<int>(rutasDisponibles.size()); i++) {
        seleccionadas.push_back(rutasDisponibles[dis(gen)]);
    }
    
    return seleccionadas;
}

// Generar datos usando el script bash
void ExperimentacionArbol::generarDatos(int numDirectorios, int numArchivos, const string& directorio) {
    string comando = "./create_files.bash " + to_string(numDirectorios) + " " + 
                     to_string(numArchivos) + " " + directorio;
    system(comando.c_str());
}

// Medir tiempo de creación
double ExperimentacionArbol::medirTiempoCreacion(const string& directorio) {
    auto inicio = chrono::high_resolution_clock::now();
    
    arbol->cargarDesdeDirectorio(directorio);
    rutasDisponibles = arbol->obtenerTodasLasRutas();
    
    auto fin = chrono::high_resolution_clock::now();
    
    return medirTiempo(inicio, fin);
}

// Medir tiempo de búsqueda
double ExperimentacionArbol::medirTiempoBusqueda(int repeticiones) {
    if (rutasDisponibles.empty()) return 0.0;
    
    vector<string> rutasPrueba = seleccionarRutasAleatorias(repeticiones);
    
    auto inicio = chrono::high_resolution_clock::now();
    
    for (const string& ruta : rutasPrueba) {
        arbol->buscar(ruta);
    }
    
    auto fin = chrono::high_resolution_clock::now();
    
    return medirTiempo(inicio, fin) / repeticiones;
}

// Medir tiempo de eliminación
double ExperimentacionArbol::medirTiempoEliminacion(int repeticiones) {
    if (rutasDisponibles.empty()) return 0.0;
    
    vector<string> rutasPrueba = seleccionarRutasAleatorias(repeticiones);
    
    auto inicio = chrono::high_resolution_clock::now();
    
    for (const string& ruta : rutasPrueba) {
        arbol->eliminar(ruta);
    }
    
    auto fin = chrono::high_resolution_clock::now();
    
    return medirTiempo(inicio, fin) / repeticiones;
}

// Medir tiempo de inserción
double ExperimentacionArbol::medirTiempoInsercion(int repeticiones) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);
    
    auto inicio = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < repeticiones; i++) {
        string rutaNueva = "nuevo_archivo_" + to_string(dis(gen)) + ".txt";
        arbol->insertar(rutaNueva, false);
    }
    
    auto fin = chrono::high_resolution_clock::now();
    
    return medirTiempo(inicio, fin) / repeticiones;
}

// Ejecutar experimento completo
ResultadoExperimento ExperimentacionArbol::ejecutarExperimento(int numDirectorios, int numArchivos, const string& directorio) {
    ResultadoExperimento resultado;
    resultado.tamaño = numDirectorios + numArchivos;
    
    cout << "Generando datos: " << numDirectorios << " directorios, " << numArchivos << " archivos..." << endl;
    generarDatos(numDirectorios, numArchivos, directorio);
    
    cout << "Midiendo tiempo de creación..." << endl;
    resultado.tiempoCreacion = medirTiempoCreacion(directorio);
    
    cout << "Midiendo tiempo de búsqueda..." << endl;
    resultado.tiempoPromedioBusqueda = medirTiempoBusqueda(REP);
    
    cout << "Midiendo tiempo de eliminación..." << endl;
    resultado.tiempoPromedioEliminacion = medirTiempoEliminacion(REP);
    
    cout << "Midiendo tiempo de inserción..." << endl;
    resultado.tiempoPromedioInsercion = medirTiempoInsercion(REP);
    
    return resultado;
}

// Generar reporte
void ExperimentacionArbol::generarReporte(const vector<ResultadoExperimento>& resultados) {
    ofstream archivo("resultados_experimentos.csv");
    
    archivo << "Tamaño,TiempoCreacion(ns),TiempoBusqueda(ns),TiempoEliminacion(ns),TiempoInsercion(ns)" << endl;
    
    for (const auto& resultado : resultados) {
        archivo << resultado.tamaño << ","
                << resultado.tiempoCreacion << ","
                << resultado.tiempoPromedioBusqueda << ","
                << resultado.tiempoPromedioEliminacion << ","
                << resultado.tiempoPromedioInsercion << endl;
    }
    
    archivo.close();
    cout << "Reporte generado en resultados_experimentos.csv" << endl;
}

// Ejecutar todos los experimentos
void ExperimentacionArbol::ejecutarTodosLosExperimentos() {
    vector<ResultadoExperimento> resultados;
    
    // Configuración pequeña
    cout << "=== EXPERIMENTO PEQUEÑO ===" << endl;
    resultados.push_back(ejecutarExperimento(20000, 200000, "datos_pequeños"));
    
    // Configuración mediana
    cout << "=== EXPERIMENTO MEDIANO ===" << endl;
    resultados.push_back(ejecutarExperimento(100000, 1000000, "datos_medianos"));
    
    // Configuración grande
    cout << "=== EXPERIMENTO GRANDE ===" << endl;
    resultados.push_back(ejecutarExperimento(1000000, 10000000, "datos_grandes"));
    
    generarReporte(resultados);
}