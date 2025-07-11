#include "experimentacion.h"
#include <chrono>    
#include <fstream>   
#include <iostream>  
#include <cstdio>    
#include <random>    
// Constructor
ExperimentacionArbol::ExperimentacionArbol() {
    arbol = new ArbolSistemaArchivos();
}

// Destructor
ExperimentacionArbol::~ExperimentacionArbol() {
    delete arbol;
}

// Seleccionar rutas Aleatorios
vector<string> ExperimentacionArbol::seleccionarRutasAleatorios(int cantidad) {
    vector<string> seleccion;
    if (rutasDisponibles.empty()) return seleccion;
    random_device rd; mt19937 gen(rd());
    uniform_int_distribution<> dis(0, static_cast<int>(rutasDisponibles.size()) - 1);
    for (int i = 0; i < cantidad && i < static_cast<int>(rutasDisponibles.size()); ++i) {
        seleccion.push_back(rutasDisponibles[dis(gen)]);
    }
    return seleccion;
}

// Seleccionar directorios aleatorios para inserción
vector<string> ExperimentacionArbol::seleccionarDirectoriosAleatorios(int cantidad) {
    vector<string> dirs;
    for (const auto &ruta : rutasDisponibles) {
        if (arbol->buscar(ruta) == 2) // directorio
            dirs.push_back(ruta);
    }
    if (dirs.empty()) return {""};
    random_device rd; mt19937 gen(rd());
    uniform_int_distribution<> dis(0, static_cast<int>(dirs.size()) - 1);
    vector<string> seleccion;
    for (int i = 0; i < min(cantidad, static_cast<int>(dirs.size())); ++i) {
        seleccion.push_back(dirs[dis(gen)]);
    }
    return seleccion;
}

// Generar datos con script externo
auto ExperimentacionArbol::generarDatos(int numDirs, int numFiles, const string& dir) -> void {
     string cmd = "bash create_files.bash "
                + to_string(numDirs) + " "
                + to_string(numFiles) + " "
                + dir;
     system(cmd.c_str());
 }
// Medir tiempo de creación (segundos)
auto ExperimentacionArbol::medirTiempoCreacion(const string& dir) -> double {
    printf("Cargando datos desde '%s'...\n", dir.c_str());
    auto start = chrono::high_resolution_clock::now();
    arbol->cargarDesdeDirectorio(dir);
    rutasDisponibles = arbol->obtenerTodasLasRutas();
    auto end = chrono::high_resolution_clock::now();
    auto secs = chrono::duration_cast<chrono::seconds>(end - start);
    return static_cast<double>(secs.count());
}

// Medir tiempo de búsqueda (ns promedio)
auto ExperimentacionArbol::medirTiempoBusqueda(int rep) -> double {
    if (rutasDisponibles.empty()) return 0.0;
    printf("Buscando %d rutas...\n", rep);
    auto pruebas = seleccionarRutasAleatorios(rep);
    auto start = chrono::high_resolution_clock::now();
    for (const auto &r : pruebas) {
        arbol->buscar(r);
    }
    auto end = chrono::high_resolution_clock::now();
    auto ns = chrono::duration_cast<chrono::nanoseconds>(end - start);
    return static_cast<double>(ns.count()) / rep;
}

// Medir tiempo de eliminación (ns promedio)
double ExperimentacionArbol::medirTiempoEliminacion(int rep) {
    if (rutasDisponibles.empty()) return 0.0;
    
    printf("Eliminando %d rutas...\n", rep);
    auto pruebas = seleccionarRutasAleatorios(rep);
    
    // Guardar información de los archivos que vamos a eliminar para recrearlos
    vector<pair<string, bool>> archivosEliminados; // ruta, esDirectorio
    
    for (const auto& r : pruebas) {
        int tipo = arbol->buscar(r);
        if (tipo != 1) { // Si existe (archivo o directorio)
            archivosEliminados.push_back({r, tipo == 2}); // tipo 2 = directorio
        }
    }
    
    // Medir tiempo de eliminación
    auto start = chrono::high_resolution_clock::now();
    for (const auto& r : pruebas) {
        arbol->eliminar(r);
    }
    auto end = chrono::high_resolution_clock::now();
    
    // Recrear los archivos eliminados para mantener el conjunto de datos
    for (const auto& archivo : archivosEliminados) {
        arbol->insertar(archivo.first, archivo.second);
    }
    
    auto ns = chrono::duration_cast<chrono::nanoseconds>(end - start);
    return static_cast<double>(ns.count()) / rep;
}

// Medir tiempo de inserción 
double ExperimentacionArbol::medirTiempoInsercion(int rep) {
    printf("Insertando %d archivos...\n", rep);
    auto dirsIns = seleccionarDirectoriosAleatorios(DIRECTORIOS_INSERCION);
    
    random_device rd; 
    mt19937 gen(rd());
    uniform_int_distribution<> disFile(1, 1000000);
    uniform_int_distribution<> disDir(0, static_cast<int>(dirsIns.size()) - 1);
    
    vector<string> archivosInsertados; // Para limpiar después
    
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < rep; ++i) {
        auto base = dirsIns[disDir(gen)];
        string name = "nuevo_archivo_" + to_string(disFile(gen)) + ".txt";
        string ruta = base.empty() ? name : base + "/" + name;
        
        int resultado = arbol->insertar(ruta, false);
        if (resultado == 0) { // Si se insertó exitosamente
            archivosInsertados.push_back(ruta);
        }
    }
    auto end = chrono::high_resolution_clock::now();
    
    // Limpiar archivos insertados para no afectar otros experimentos
    for (const auto& archivo : archivosInsertados) {
        arbol->eliminar(archivo);
    }
    
    auto ns = chrono::duration_cast<chrono::nanoseconds>(end - start);
    return static_cast<double>(ns.count()) / rep;
}

// Ejecutar experimento completo
auto ExperimentacionArbol::ejecutarExperimento(int numDirs, int numFiles, const string& dir) -> ResultadoExperimento {
    ResultadoExperimento res;
    res.tamaño = numDirs + numFiles;
    printf("=== Generando datos: %d dirs, %d files ===\n", numDirs, numFiles);
    generarDatos(numDirs, numFiles, dir);
    printf("=== Midiendo creacion ===\n");
    res.tiempoCreacion = medirTiempoCreacion(dir);
    printf("  Creacion: %.4f s\n", res.tiempoCreacion);
    printf("=== Midiendo busqueda ===\n");
    res.tiempoPromedioBusqueda = medirTiempoBusqueda(REP);
    printf("  Busqueda: %.4f ns\n", res.tiempoPromedioBusqueda);
    printf("=== Midiendo eliminacion ===\n");
    res.tiempoPromedioEliminacion = medirTiempoEliminacion(REP);
    printf("  Eliminacion: %.4f ns\n", res.tiempoPromedioEliminacion);
    printf("=== Midiendo insercion ===\n");
    res.tiempoPromedioInsercion = medirTiempoInsercion(REP);
    printf("  Insercion: %.4f ns\n", res.tiempoPromedioInsercion);
    return res;
}

// Generar reporte CSV
auto ExperimentacionArbol::generarReporte(const vector<ResultadoExperimento>& resultados) -> void {
    ofstream out("resultados_experimentos.csv");
    out << "Tamaño,TiempoCreacion(s),TiempoBusqueda(ns),TiempoEliminacion(ns),TiempoInsercion(ns)\n";
    for (const auto &r : resultados) {
        out << r.tamaño << ","
            << r.tiempoCreacion << ","
            << r.tiempoPromedioBusqueda << ","
            << r.tiempoPromedioEliminacion << ","
            << r.tiempoPromedioInsercion << "\n";
    }
    out.close();
    printf("Reporte generado: resultados_experimentos.csv\n");
}

// Ejecutar todos los experimentos
auto ExperimentacionArbol::ejecutarTodosLosExperimentos() -> void {
    vector<ResultadoExperimento> resultados;
    printf("=== EXPERIMENTO PEQUENO ===\n");
    resultados.push_back(ejecutarExperimento(20000, 200000, "datos_pequenos"));
    printf("\n=== EXPERIMENTO MEDIANO ===\n");
    resultados.push_back(ejecutarExperimento(100000, 1000000, "datos_medianos"));
    printf("\n=== EXPERIMENTO GRANDE ===\n");
    resultados.push_back(ejecutarExperimento(1000000, 10000000, "datos_grandes"));
    generarReporte(resultados);
}
