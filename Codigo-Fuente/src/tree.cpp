#include "tree.h"
#include <filesystem>  
#include <fstream>     
#include <sstream>     
#include <exception> 
#include <algorithm>

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
    int izq = 0, der = static_cast<int>(hijos.size()) - 1;
    
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

// Cargar desde directorio - ahora también guarda el directorio base
void ArbolSistemaArchivos::cargarDesdeDirectorio(const string& rutaDirectorio) {
    filesystem::path ruta(rutaDirectorio);
    if (filesystem::exists(ruta) && filesystem::is_directory(ruta)) {
        directorioBase = filesystem::absolute(ruta).string();
        cargarDirectorioRecursivo(ruta, raiz);
    }
}

// Construir ruta completa del sistema de archivos
string ArbolSistemaArchivos::construirRutaCompleta(const string& rutaRelativa) {
    if (directorioBase.empty()) {
        return rutaRelativa;
    }
    
    if (rutaRelativa.empty() || rutaRelativa == "/") {
        return directorioBase;
    }
    
    return directorioBase + "/" + rutaRelativa;
}

// Crear archivo en el sistema de archivos
bool ArbolSistemaArchivos::crearArchivoSistema(const string& rutaCompleta) {
    try {
        // Crear directorios padre si no existen
        filesystem::path ruta(rutaCompleta);
        filesystem::create_directories(ruta.parent_path());
        
        // Crear archivo vacío
        ofstream archivo(rutaCompleta);
        if (archivo.is_open()) {
            archivo.close();
            return true;
        }
        return false;
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Error al crear archivo: " << e.what() << endl;
        return false;
    }
}

// Crear directorio en el sistema de archivos
bool ArbolSistemaArchivos::crearDirectorioSistema(const string& rutaCompleta) {
    try {
        return filesystem::create_directories(rutaCompleta);
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Error al crear directorio: " << e.what() << endl;
        return false;
    }
}

// Eliminar del sistema de archivos
bool ArbolSistemaArchivos::eliminarDelSistema(const string& rutaCompleta) {
    try {
        if (filesystem::exists(rutaCompleta)) {
            if (filesystem::is_directory(rutaCompleta)) {
                // Eliminar directorio y todo su contenido
                return filesystem::remove_all(rutaCompleta) > 0;
            } else {
                // Eliminar archivo
                return filesystem::remove(rutaCompleta);
            }
        }
        return false;
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Error al eliminar: " << e.what() << endl;
        return false;
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
    
    // Crear en el sistema de archivos primero
    string rutaCompleta = construirRutaCompleta(ruta);
    bool exitoSistema = false;
    
    if (esDirectorio) {
        exitoSistema = crearDirectorioSistema(rutaCompleta);
    } else {
        exitoSistema = crearArchivoSistema(rutaCompleta);
    }
    
    if (!exitoSistema) {
        return 3; // Error del sistema de archivos
    }
    
    // Si el sistema de archivos tuvo éxito, insertar en el árbol
    NodoArbol* nuevoNodo = new NodoArbol(nombreArchivo);
    insertarOrdenado(nodoPadre->hijos, nuevoNodo);
    
    return 0; // Éxito
}

// Eliminación 
int ArbolSistemaArchivos::eliminar(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    if (componentes.empty()) {
        return 1; // Ruta inválida
    }
    
    // Buscar directorio padre
    string rutaPadre = "";
    for (size_t i = 0; i < componentes.size() - 1; i++) {
        if (i > 0) rutaPadre += "/";
        rutaPadre += componentes[i];
    }
    
    NodoArbol* nodoPadre = buscarNodo(rutaPadre);
    if (nodoPadre == nullptr) {
        return 1; // No existe el padre
    }
    
    // Buscar nodo a eliminar
    string nombreArchivo = componentes.back();
    int indice = busquedaBinaria(nodoPadre->hijos, nombreArchivo);
    if (indice == -1) {
        return 1; // No existe el archivo/directorio
    }
    
    // Eliminar del sistema de archivos primero
    string rutaCompleta = construirRutaCompleta(ruta);
    if (!eliminarDelSistema(rutaCompleta)) {
        return 2; // Error del sistema de archivos
    }
    
    // Si el sistema de archivos tuvo éxito, eliminar del árbol
    NodoArbol* nodoAEliminar = nodoPadre->hijos[indice];
    nodoPadre->hijos.erase(nodoPadre->hijos.begin() + indice);
    eliminarSubarbol(nodoAEliminar);
    
    return 0; // Éxito
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

// Busca un nodo por ruta y devuelve:
//   1 si no existe, 0 si es archivo, 2 si es directorio
int ArbolSistemaArchivos::buscar(const string& ruta) {
    NodoArbol* nodo = buscarNodo(ruta);
    if (nodo == nullptr) return 1;        // No existe
    return nodo->esArchivo() ? 0 : 2;     // 0=archivo, 2=directorio
}

// Elimina recursivamente un subárbol liberando memoria
void ArbolSistemaArchivos::eliminarSubarbol(NodoArbol* nodo) {
    if (nodo) delete nodo;
}
