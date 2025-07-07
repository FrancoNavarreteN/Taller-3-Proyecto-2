#include "tree.h"
#include <algorithm>
#include <sstream>
#include <iostream>

// Destructor del nodo
NodoArbol::~NodoArbol() {
    for (NodoArbol* hijo : hijos) {
        delete hijo;
    }
}

// Constructor de la clase ArbolSistemaArchivos
ArbolSistemaArchivos::ArbolSistemaArchivos() {
    raiz = new NodoArbol("raiz");
}

// Destructor
ArbolSistemaArchivos::~ArbolSistemaArchivos() {
    delete raiz;
}

// Dividir ruta en componentes
vector<string> ArbolSistemaArchivos::dividirRuta(const string& ruta) {
    vector<string> componentes;
    stringstream ss(ruta);
    string componente;
    
    while (getline(ss, componente, '/')) {
        if (!componente.empty()) {
            componentes.push_back(componente);
        }
    }
    
    return componentes;
}

// Búsqueda binaria en el vector de hijos
int ArbolSistemaArchivos::busquedaBinaria(const vector<NodoArbol*>& hijos, const string& nombre) {
    int izq = 0, der = hijos.size() - 1;
    
    while (izq <= der) {
        int medio = izq + (der - izq) / 2;
        
        if (hijos[medio]->nombre == nombre) {
            return medio;
        } else if (hijos[medio]->nombre < nombre) {
            izq = medio + 1;
        } else {
            der = medio - 1;
        }
    }
    
    return -1; // No encontrado
}

// Insertar nodo manteniendo orden lexicográfico
void ArbolSistemaArchivos::insertarOrdenado(vector<NodoArbol*>& hijos, NodoArbol* nodo) {
    auto it = lower_bound(hijos.begin(), hijos.end(), nodo,
        [](const NodoArbol* a, const NodoArbol* b) {
            return a->nombre < b->nombre;
        });
    hijos.insert(it, nodo);
}

// Buscar nodo por ruta
NodoArbol* ArbolSistemaArchivos::buscarNodo(const string& ruta) {
    if (ruta.empty() || ruta == "/") {
        return raiz;
    }
    
    vector<string> componentes = dividirRuta(ruta);
    NodoArbol* nodoActual = raiz;
    
    for (const string& componente : componentes) {
        int indice = busquedaBinaria(nodoActual->hijos, componente);
        if (indice == -1) {
            return nullptr;
        }
        nodoActual = nodoActual->hijos[indice];
    }
    
    return nodoActual;
}

// Cargar directorio recursivamente
void ArbolSistemaArchivos::cargarDirectorioRecursivo(const filesystem::path& ruta, NodoArbol* nodo) {
    try {
        for (const auto& entrada : filesystem::directory_iterator(ruta)) {
            string nombre = entrada.path().filename().string();
            NodoArbol* nuevoNodo = new NodoArbol(nombre);
            
            if (entrada.is_directory()) {
                cargarDirectorioRecursivo(entrada.path(), nuevoNodo);
            }
            
            insertarOrdenado(nodo->hijos, nuevoNodo);
        }
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Error al acceder al directorio: " << e.what() << endl;
    }
}

// Cargar desde directorio
void ArbolSistemaArchivos::cargarDesdeDirectorio(const string& rutaDirectorio) {
    filesystem::path ruta(rutaDirectorio);
    if (filesystem::exists(ruta) && filesystem::is_directory(ruta)) {
        cargarDirectorioRecursivo(ruta, raiz);
    }
}

// Búsqueda
int ArbolSistemaArchivos::buscar(const string& ruta) {
    NodoArbol* nodo = buscarNodo(ruta);
    
    if (nodo == nullptr) {
        return 1; // No existe
    }
    
    if (nodo->esArchivo()) {
        return 0; // Es archivo
    } else {
        return 2; // Es directorio
    }
}

// Inserción
int ArbolSistemaArchivos::insertar(const string& ruta, bool esDirectorio) {
    vector<string> componentes = dividirRuta(ruta);
    if (componentes.empty()) {
        return 2; // Ruta inválida
    }
    
    // Buscar directorio padre
    string rutaPadre = "";
    for (size_t i = 0; i < componentes.size() - 1; i++) {
        if (i > 0) rutaPadre += "/";
        rutaPadre += componentes[i];
    }
    
    NodoArbol* nodoPadre = buscarNodo(rutaPadre);
    if (nodoPadre == nullptr || nodoPadre->esArchivo()) {
        return 2; // No existe ruta padre o el padre es un archivo
    }
    
    // Verificar si ya existe
    string nombreArchivo = componentes.back();
    int indice = busquedaBinaria(nodoPadre->hijos, nombreArchivo);
    if (indice != -1) {
        return 1; // El archivo ya existe
    }
    
    // Insertar nuevo nodo
    NodoArbol* nuevoNodo = new NodoArbol(nombreArchivo);
    insertarOrdenado(nodoPadre->hijos, nuevoNodo);
    
    return 0; // Éxito
}

// Eliminar subárbol
void ArbolSistemaArchivos::eliminarSubarbol(NodoArbol* nodo) {
    if (nodo != nullptr) {
        delete nodo;
    }
}

// Eliminación
bool ArbolSistemaArchivos::eliminar(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    if (componentes.empty()) {
        return false;
    }
    
    // Buscar directorio padre
    string rutaPadre = "";
    for (size_t i = 0; i < componentes.size() - 1; i++) {
        if (i > 0) rutaPadre += "/";
        rutaPadre += componentes[i];
    }
    
    NodoArbol* nodoPadre = buscarNodo(rutaPadre);
    if (nodoPadre == nullptr) {
        return false;
    }
    
    // Buscar nodo a eliminar
    string nombreArchivo = componentes.back();
    int indice = busquedaBinaria(nodoPadre->hijos, nombreArchivo);
    if (indice == -1) {
        return false;
    }
    
    // Eliminar nodo
    NodoArbol* nodoAEliminar = nodoPadre->hijos[indice];
    nodoPadre->hijos.erase(nodoPadre->hijos.begin() + indice);
    eliminarSubarbol(nodoAEliminar);
    
    return true;
}

// Obtener todas las rutas
vector<string> ArbolSistemaArchivos::obtenerTodasLasRutas() {
    vector<string> rutas;
    obtenerRutasRecursivo(raiz, "", rutas);
    return rutas;
}

void ArbolSistemaArchivos::obtenerRutasRecursivo(NodoArbol* nodo, const string& rutaActual, vector<string>& rutas) {
    if (nodo == nullptr) return;
    
    string nuevaRuta = rutaActual;
    if (nodo != raiz) {
        if (!nuevaRuta.empty()) nuevaRuta += "/";
        nuevaRuta += nodo->nombre;
        rutas.push_back(nuevaRuta);
    }
    
    for (NodoArbol* hijo : nodo->hijos) {
        obtenerRutasRecursivo(hijo, nuevaRuta, rutas);
    }
}

// Contar nodos
int ArbolSistemaArchivos::obtenerNumeroNodos() {
    return contarNodosRecursivo(raiz) - 1; // -1 para no contar la raiz
}

int ArbolSistemaArchivos::contarNodosRecursivo(NodoArbol* nodo) {
    if (nodo == nullptr) return 0;
    
    int contador = 1;
    for (NodoArbol* hijo : nodo->hijos) {
        contador += contarNodosRecursivo(hijo);
    }
    
    return contador;
}