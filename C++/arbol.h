#ifndef ARBOL_H
#define ARBOL_H

#include <vector>
#include <string>

using namespace std;

struct NodoArbol {
    string nombre;
    vector<NodoArbol*> hijos; // Vector siempre ordenado lexicográficamente
    
    NodoArbol(const string& n);
    ~NodoArbol();
    
    // Verificar si es archivo (nodo hoja)
    bool esArchivo() const;
};

class SistemaArchivos {
private:
    NodoArbol* raiz;
    
    // Función auxiliar para buscar binariamente en el vector de hijos
    int buscarBinario(const vector<NodoArbol*>& hijos, const string& nombre);
    
    // Función auxiliar para insertar manteniendo orden lexicográfico
    void insertarOrdenado(vector<NodoArbol*>& hijos, NodoArbol* nuevo);
    
    // Dividir ruta en componentes
    vector<string> dividirRuta(const string& ruta);
    
    // Función auxiliar para obtener todas las rutas
    void obtenerTodasLasRutas(NodoArbol* nodo, string rutaActual, vector<string>& rutas);
    
    // Función auxiliar para obtener directorios
    void obtenerDirectorios(NodoArbol* nodo, string rutaActual, vector<string>& directorios);

public:
    SistemaArchivos();
    ~SistemaArchivos();
    
    // Tarea 1: Carga de datos desde sistema de archivos
    void cargarDesdeDirectorio(const string& rutaBase);
    
    // Tarea 2: Inserción de rutas en el árbol
    void insertarRuta(const string& ruta);
    
    // Tarea 3: Búsqueda por ruta relativa
    // Retorna: 0 si existe archivo, 1 si no existe, 2 si es directorio
    int buscar(const string& ruta);
    
    // Tarea 4: Inserción de nuevo archivo/directorio
    // Retorna: 0 éxito, 1 ya existe, 2 no existe ruta padre
    int insertar(const string& ruta);
    
    // Tarea 4: Eliminación
    // Retorna: true si se eliminó, false si no existe
    bool eliminar(const string& ruta);
    
    // Función para obtener rutas aleatorias (para experimentación)
    vector<string> obtenerRutasAleatorias(int cantidad);
    
    // Función para obtener directorios aleatorios
    vector<string> obtenerDirectoriosAleatorios(int cantidad);
    
    // Función para obtener estadísticas del árbol
    void obtenerEstadisticas(int& totalNodos, int& totalArchivos, int& alturaMaxima);
    
    // Función auxiliar para calcular altura
    int calcularAltura(NodoArbol* nodo);
    
    // Función para imprimir estructura (útil para debugging)
    void imprimirEstructura(int maxNiveles = 3);
    
private:
    void imprimirNodo(NodoArbol* nodo, int nivel, int maxNiveles, const string& prefijo = "");
};

#endif // ARBOL_H