#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <filesystem>

using namespace std;

// Estructura del nodo del árbol k-ario
struct NodoArbol {
    string nombre;
    vector<NodoArbol*> hijos; // Vector siempre ordenado lexicográficamente
    
    // Constructor
    NodoArbol(const string& n) : nombre(n) {}
    
    // Destructor
    ~NodoArbol();
    
    // Verificar si es un archivo (nodo hoja)
    bool esArchivo() const { return hijos.empty(); }
};

// Clase para el árbol del sistema de archivos
class ArbolSistemaArchivos {
private:
    NodoArbol* raiz;
    
    // Funciones auxiliares privadas
    NodoArbol* buscarNodo(const string& ruta);
    vector<string> dividirRuta(const string& ruta);
    int busquedaBinaria(const vector<NodoArbol*>& hijos, const string& nombre);
    void insertarOrdenado(vector<NodoArbol*>& hijos, NodoArbol* nodo);
    void eliminarSubarbol(NodoArbol* nodo);
    void cargarDirectorioRecursivo(const filesystem::path& ruta, NodoArbol* nodo);
    
public:
    // Constructor
    ArbolSistemaArchivos();
    
    // Destructor
    ~ArbolSistemaArchivos();
    
    // Cargar datos desde el sistema de archivos
    void cargarDesdeDirectorio(const string& rutaDirectorio);
    
    // Búsqueda por ruta relativa
    // Retorna: 0 si existe el archivo, 1 si no existe, 2 si es un directorio
    int buscar(const string& ruta);
    
    // Inserción
    // Retorna: 0 en éxito, 1 si el archivo ya existe, 2 si no existe ruta padre
    int insertar(const string& ruta, bool esDirectorio = false);
    
    // Eliminación
    // Retorna: true si tuvo éxito, false si no existe
    bool eliminar(const string& ruta);
    
    // Obtener todas las rutas del árbol (para experimentación)
    vector<string> obtenerTodasLasRutas();
    void obtenerRutasRecursivo(NodoArbol* nodo, const string& rutaActual, vector<string>& rutas);
    
    // Obtener número total de nodos
    int obtenerNumeroNodos();
    int contarNodosRecursivo(NodoArbol* nodo);
};

#endif