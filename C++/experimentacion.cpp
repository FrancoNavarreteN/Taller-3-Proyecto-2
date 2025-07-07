#include "experimentacion.h"
#include <iostream>
#include <chrono>
#include <random>
#include <filesystem>
#include <iomanip>

using namespace std;
using namespace std::chrono;
using namespace std::filesystem;

Experimentacion::Experimentacion() {
    sistema = new SistemaArchivos();
}

Experimentacion::~Experimentacion() {
    delete sistema;
}

vector<string> Experimentacion::generarNombresArchivos(int cantidad) {
    vector<string> nombres = {
        "archivo1.txt", "archivo2.txt", "archivo3.txt", "archivo4.txt",
        "archivo5.txt", "test.dat", "config.ini", "readme.md",
        "documento.pdf", "imagen.jpg", "video.mp4", "audio.wav",
        "programa.exe", "script.sh", "datos.csv", "backup.zip",
        "log.txt", "error.log", "debug.txt", "temp.tmp"
    };
    
    vector<string> resultado;
    random_device rd;
    mt19937 gen(rd());
    
    for (int i = 0; i < cantidad; i++) {
        string nombre = nombres[i % nombres.size()];
        if (i >= nombres.size()) {
            nombre = "archivo_" + to_string(i) + ".txt";
        }
        resultado.push_back(nombre);
    }
    
    return resultado;
}

void Experimentacion::crearDirectoriosSinteticos(const string& rutaBase, int profundidad, int anchura, 
                                                int& contadorDir, int& contadorArch, int maxDir, int maxArch) {
    if (profundidad <= 0 || contadorDir >= maxDir || contadorArch >= maxArch) {
        return;
    }
    
    try {
        create_directories(rutaBase);
        
        // Crear archivos en el directorio actual
        int archivosEnEsteDir = min(anchura, maxArch - contadorArch);
        vector<string> nombresArchivos = generarNombresArchivos(archivosEnEsteDir);
        
        for (int i = 0; i < archivosEnEsteDir && contadorArch < maxArch; i++) {
            string rutaArchivo = rutaBase + "/" + nombresArchivos[i];
            ofstream archivo(rutaArchivo);
            archivo << "Contenido del archivo " << contadorArch << endl;
            archivo.close();
            contadorArch++;
        }
        
        // Crear subdirectorios
        for (int i = 0; i < anchura && contadorDir < maxDir; i++) {
            string nombreSubdir = "directorio_" + to_string(contadorDir);
            string rutaSubdir = rutaBase + "/" + nombreSubdir;
            contadorDir++;
            
            crearDirectoriosSinteticos(rutaSubdir, profundidad - 1, anchura, 
                                     contadorDir, contadorArch, maxDir, maxArch);
        }
    }
    catch (const filesystem_error& e) {
        cout << "Error creando estructura sintética: " << e.what() << endl;
    }
}

void Experimentacion::crearEstructuraSintetica(const string& rutaBase, int numDirectorios, int numArchivos) {
    cout << "Creando estructura sintética..." << endl;
    
    // Limpiar directorio base si existe
    if (exists(rutaBase)) {
        remove_all(rutaBase);
    }
    
    int contadorDir = 0;
    int contadorArch = 0;
    
    // Calcular profundidad y anchura para obtener aproximadamente numDirectorios directorios
    int profundidad = 4; // Profundidad fija
    int anchura = max(2, (int)sqrt(numDirectorios / profundidad));
    
    crearDirectoriosSinteticos(rutaBase, profundidad, anchura, contadorDir, contadorArch, numDirectorios, numArchivos);
    
    cout << "Estructura creada: " << contadorDir << " directorios, " << contadorArch << " archivos" << endl;
}

long long Experimentacion::medirTiempoCreacion(const string& rutaBase) {
    // Reiniciar sistema
    delete sistema;
    sistema = new SistemaArchivos();
    
    auto inicio = high_resolution_clock::now();
    sistema->cargarDesdeDirectorio(rutaBase);
    auto fin = high_resolution_clock::now();
    
    return duration_cast<microseconds>(fin - inicio).count();
}

long long Experimentacion::medirTiempoBusquedas(int repeticiones) {
    vector<string> rutas = sistema->obtenerRutasAleatorias(repeticiones);
    
    if (rutas.empty()) {
        cout << "Advertencia: No hay rutas para buscar" << endl;
        return 0;
    }
    
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < repeticiones; i++) {
        string ruta = rutas[i % rutas.size()];
        sistema->buscar(ruta);
    }
    auto fin = high_resolution_clock::now();
    
    return duration_cast<microseconds>(fin - inicio).count();
}

long long Experimentacion::medirTiempoEliminaciones(int repeticiones) {
    vector<string> rutas = sistema->obtenerRutasAleatorias(repeticiones);
    
    if (rutas.empty()) {
        cout << "Advertencia: No hay rutas para eliminar" << endl;
        return 0;
    }
    
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < repeticiones; i++) {
        string ruta = rutas[i % rutas.size()];
        sistema->eliminar(ruta);
    }
    auto fin = high_resolution_clock::now();
    
    return duration_cast<microseconds>(fin - inicio).count();
}

long long Experimentacion::medirTiempoInserciones(int repeticiones) {
    vector<string> archivosEstaticos = generarNombresArchivos(100);
    vector<string> directorios = sistema->obtenerDirectoriosAleatorios(2000);
    
    if (directorios.empty()) {
        cout << "Advertencia: No hay directorios para insertar archivos" << endl;
        return 0;
    }
    
    auto inicio = high_resolution_clock::now();
    for (int i = 0; i < repeticiones; i++) {
        string directorio = directorios[i % directorios.size()];
        string archivo = archivosEstaticos[i % archivosEstaticos.size()];
        string rutaCompleta = directorio + "/" + archivo + "_" + to_string(i);
        sistema->insertar(rutaCompleta);
    }
    auto fin = high_resolution_clock::now();
    
    return duration_cast<microseconds>(fin - inicio).count();
}

ResultadoExperimento Experimentacion::ejecutarExperimentoCompleto(const string& rutaBase, const string& configuracion) {
    ResultadoExperimento resultado;
    resultado.configuracion = configuracion;
    
    cout << "=== EJECUTANDO EXPERIMENTO: " << configuracion << " ===" << endl;
    
    // Tiempo de creación
    cout << "Midiendo tiempo de creación..." << endl;
    resultado.tiempoCreacion = medirTiempoCreacion(rutaBase);
    
    // Obtener estadísticas
    sistema->obtenerEstadisticas(resultado.totalNodos, resultado.totalArchivos, resultado.alturaMaxima);
    
    const int REP = 10000; // Reducido para pruebas más rápidas
    
    // Búsquedas
    cout << "Midiendo tiempo de búsquedas..." << endl;
    resultado.tiempoBusquedas = medirTiempoBusquedas(REP);
    resultado.tiempoPromedioBusqueda = (double)resultado.tiempoBusquedas / REP;
    
    // Eliminaciones
    cout << "Midiendo tiempo de eliminaciones..." << endl;
    resultado.tiempoEliminaciones = medirTiempoEliminaciones(REP);
    resultado.tiempoPromedioEliminacion = (double)resultado.tiempoEliminaciones / REP;
    
    // Inserciones
    cout << "Midiendo tiempo de inserciones..." << endl;
    resultado.tiempoInserciones = medirTiempoInserciones(REP);
    resultado.tiempoPromedioInsercion = (double)resultado.tiempoInserciones / REP;
    
    return resultado;
}

vector<ResultadoExperimento> Experimentacion::ejecutarTodosLosExperimentos() {
    vector<ResultadoExperimento> resultados;
    
    // Configuración pequeña
    cout << "Preparando configuración pequeña..." << endl;
    crearEstructuraSintetica("./test_pequeno", 100, 1000);  // Reducido para pruebas
    resultados.push_back(ejecutarExperimentoCompleto("./test_pequeno", "Pequeña"));
    
    // Configuración mediana
    cout << "Preparando configuración mediana..." << endl;
    crearEstructuraSintetica("./test_mediano", 500, 5000);  // Reducido para pruebas
    resultados.push_back(ejecutarExperimentoCompleto("./test_mediano", "Mediana"));
    
    // Configuración grande
    cout << "Preparando configuración grande..." << endl;
    crearEstructuraSintetica("./test_grande", 1000, 10000);  // Reducido para pruebas
    resultados.push_back(ejecutarExperimentoCompleto("./test_grande", "Grande"));
    
    return resultados;
}

void Experimentacion::generarCSV(const vector<ResultadoExperimento>& resultados, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    
    // Encabezados
    archivo << "Configuracion,TotalNodos,TotalArchivos,AlturaMaxima,TiempoCreacion,TiempoBusquedas,TiempoEliminaciones,TiempoInserciones,";
    archivo << "TiempoPromedioBusqueda,TiempoPromedioEliminacion,TiempoPromedioInsercion" << endl;
    
    // Datos
    for (const auto& resultado : resultados) {
        archivo << resultado.configuracion << ","
                << resultado.totalNodos << ","
                << resultado.totalArchivos << ","
                << resultado.alturaMaxima << ","
                << resultado.tiempoCreacion << ","
                << resultado.tiempoBusquedas << ","
                << resultado.tiempoEliminaciones << ","
                << resultado.tiempoInserciones << ","
                << fixed << setprecision(3) << resultado.tiempoPromedioBusqueda << ","
                << fixed << setprecision(3) << resultado.tiempoPromedioEliminacion << ","
                << fixed << setprecision(3) << resultado.tiempoPromedioInsercion << endl;
    }
    
    archivo.close();
    cout << "Resultados guardados en: " << nombreArchivo << endl;
}

void Experimentacion::imprimirResultados(const ResultadoExperimento& resultado) {
    cout << "\n=== RESULTADOS " << resultado.configuracion << " ===" << endl;
    cout << "Total nodos: " << resultado.totalNodos << endl;
    cout << "Total archivos: " << resultado.totalArchivos << endl;
    cout << "Altura máxima: " << resultado.alturaMaxima << endl;
    cout << "Tiempo de creación: " << resultado.tiempoCreacion << " microsegundos" << endl;
    cout << "Tiempo de búsquedas: " << resultado.tiempoBusquedas << " microsegundos" << endl;
    cout << "Tiempo de eliminaciones: " << resultado.tiempoEliminaciones << " microsegundos" << endl;
    cout << "Tiempo de inserciones: " << resultado.tiempoInserciones << " microsegundos" << endl;
    cout << "Tiempo promedio búsqueda: " << fixed << setprecision(3) << resultado.tiempoPromedioBusqueda << " microsegundos" << endl;
    cout << "Tiempo promedio eliminación: " << fixed << setprecision(3) << resultado.tiempoPromedioEliminacion << " microsegundos" << endl;
    cout << "Tiempo promedio inserción: " << fixed << setprecision(3) << resultado.tiempoPromedioInsercion << " microsegundos" << endl;
    cout << "=============================" << endl;
    