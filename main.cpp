#include <iostream>
#include <string>
#include <vector>
#include "arbol.h"
#include "experimentacion.h"

using namespace std;

void mostrarMenu() {
    cout << "\n=== SIMULADOR DE SISTEMA DE ARCHIVOS ===" << endl;
    cout << "1. Cargar estructura desde directorio" << endl;
    cout << "2. Buscar archivo/directorio" << endl;
    cout << "3. Insertar archivo/directorio" << endl;
    cout << "4. Eliminar archivo/directorio" << endl;
    cout << "5. Mostrar estructura del árbol" << endl;
    cout << "6. Mostrar estadísticas" << endl;
    cout << "7. Ejecutar experimentos completos" << endl;
    cout << "8. Validar implementación" << endl;
    cout << "9. Salir" << endl;
    cout << "Seleccione una opción: ";
}

void manejarCarga(SistemaArchivos& sistema) {
    string ruta;
    cout << "Ingrese la ruta del directorio a cargar: ";
    cin.ignore();
    getline(cin, ruta);
    
    cout << "Cargando estructura desde: " << ruta << endl;
    sistema.cargarDesdeDirectorio(ruta);
    
    int totalNodos, totalArchivos, altura;
    sistema.obtenerEstadisticas(totalNodos, totalArchivos, altura);
    
    cout << "Carga completada:" << endl;
    cout << "- Total de nodos: " << totalNodos << endl;
    cout << "- Total de archivos: " << totalArchivos << endl;
    cout << "- Altura máxima: " << altura << endl;
}

void manejarBusqueda(SistemaArchivos& sistema) {
    string ruta;
    cout << "Ingrese la ruta a buscar (ej: directorio/archivo.txt): ";
    cin.ignore();
    getline(cin, ruta);
    
    int resultado = sistema.buscar(ruta);
    
    switch (resultado) {
        case 0:
            cout << "✓ Encontrado: '" << ruta << "' es un archivo" << endl;
            break;
        case 1:
            cout << "✗ No encontrado: '" << ruta << "' no existe" << endl;
            break;
        case 2:
            cout << "✓ Encontrado: '" << ruta << "' es un directorio" << endl;
            break;
    }
}

void manejarInsercion(SistemaArchivos& sistema) {
    string ruta;
    cout << "Ingrese la ruta del nuevo archivo/directorio: ";
    cin.ignore();
    getline(cin, ruta);
    
    int resultado = sistema.insertar(ruta);
    
    switch (resultado) {
        case 0:
            cout << "✓ Éxito: '" << ruta << "' insertado correctamente" << endl;
            break;
        case 1:
            cout << "✗ Error: '" << ruta << "' ya existe" << endl;
            break;
        case 2:
            cout << "✗ Error: La ruta padre no existe" << endl;
            break;
    }
}

void manejarEliminacion(SistemaArchivos& sistema) {
    string ruta;
    cout << "Ingrese la ruta a eliminar: ";
    cin.ignore();
    getline(cin, ruta);
    
    bool resultado = sistema.eliminar(ruta);
    
    if (resultado) {
        cout << "✓ Éxito: '" << ruta << "' eliminado correctamente" << endl;
    } else {
        cout << "✗ Error: '" << ruta << "' no existe" << endl;
    }
}

void mostrarEstadisticas(SistemaArchivos& sistema) {
    int totalNodos, totalArchivos, altura;
    sistema.obtenerEstadisticas(totalNodos, totalArchivos, altura);
    
    cout << "\n=== ESTADÍSTICAS DEL SISTEMA ===" << endl;
    cout << "Total de nodos: " << totalNodos << endl;
    cout << "Total de archivos: " << totalArchivos << endl;
    cout << "Total de directorios: " << (totalNodos - totalArchivos) << endl;
    cout << "Altura máxima del árbol: " << altura << endl;
    cout << "===============================" << endl;
}

void ejecutarExperimentos() {
    cout << "\n=== INICIANDO EXPERIMENTOS ===" << endl;
    cout << "Esto puede tomar varios minutos..." << endl;
    
    Experimentacion exp;
    vector<ResultadoExperimento> resultados = exp.ejecutarTodosLosExperimentos();
    
    cout << "\n=== RESUMEN DE RESULTADOS ===" << endl;
    for (const auto& resultado : resultados) {
        exp.imprimirResultados(resultado);
    }
    
    // Generar archivo CSV con resultados
    exp.generarCSV(resultados, "resultados_experimentos.csv");
    
    cout << "\nExperimentos completados. Revise el archivo 'resultados_experimentos.csv'" << endl;
}

void validarImplementacion() {
    cout << "\n=== VALIDANDO IMPLEMENTACIÓN ===" << endl;
    
    SistemaArchivos sistema;
    
    // Prueba 1: Inserción básica
    cout << "Prueba 1: Inserción básica..." << endl;
    sistema.insertar("directorio1/archivo1.txt");
    sistema.insertar("directorio1/archivo2.txt");
    sistema.insertar("directorio2/subdirectorio/archivo3.txt");
    
    // Prueba 2: Búsqueda
    cout << "Prueba 2: Búsqueda..." << endl;
    if (sistema.buscar("directorio1/archivo1.txt") == 0) {
        cout << "✓ Búsqueda de archivo correcta" << endl;
    }
    if (sistema.buscar("directorio1") == 2) {
        cout << "✓ Búsqueda de directorio correcta" << endl;
    }
    if (sistema.buscar("inexistente") == 1) {
        cout << "✓ Búsqueda de elemento inexistente correcta" << endl;
    }
    
    // Prueba 3: Eliminación
    cout << "Prueba 3: Eliminación..." << endl;
    if (sistema.eliminar("directorio1/archivo1.txt")) {
        cout << "✓ Eliminación de archivo correcta" << endl;
    }
    if (sistema.buscar("directorio1/archivo1.txt") == 1) {
        cout << "✓ Verificación de eliminación correcta" << endl;
    }
    
    // Prueba 4: Inserción con errores
    cout << "Prueba 4: Manejo de errores..." << endl;
    if (sistema.insertar("directorio1/archivo2.txt") == 1) {
        cout << "✓ Detección de duplicados correcta" << endl;
    }
    if (sistema.insertar("inexistente/archivo.txt") == 2) {
        cout << "✓ Detección de ruta padre inexistente correcta" << endl;
    }
    
    cout << "Validación completada." << endl;
}

int main() {
    cout << "=== TAREA 2 - INFO088 ===" << endl;
    cout << "Simulador de Sistema de Archivos con Árbol K-ario" << endl;
    cout << "Universidad Austral de Chile" << endl;
    
    SistemaArchivos sistema;
    int opcion;
    
    do {
        mostrarMenu();
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                manejarCarga(sistema);
                break;
            case 2:
                manejarBusqueda(sistema);
                break;
            case 3:
                manejarInsercion(sistema);
                break;
            case 4:
                manejarEliminacion(sistema);
                break;
            case 5:
                cout << "Ingrese el número máximo de niveles a mostrar: ";
                int niveles;
                cin >> niveles;
                sistema.imprimirEstructura(niveles);
                break;
            case 6:
                mostrarEstadisticas(sistema);
                break;
            case 7:
                ejecutarExperimentos();
                break;
            case 8:
                validarImplementacion();
                break;
            case 9:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opción inválida. Intente nuevamente." << endl;
        }
        
    } while (opcion != 9);
    
    return 0;
}