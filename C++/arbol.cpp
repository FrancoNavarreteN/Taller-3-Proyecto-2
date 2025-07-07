#include "arbol.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <random>

using namespace std;
using namespace std::filesystem;

// Implementación de NodoArbol
NodoArbol::NodoArbol(const string& n) : nombre(n) {}

NodoArbol::~NodoArbol() {
    for (auto hijo : hijos) {
        delete hijo;
    }
}

bool NodoArbol::esArchivo() const {
    return hijos.empty();
}

// Implementación de SistemaArchivos
SistemaArchivos::SistemaArchivos() {
    raiz = new NodoArbol("raiz");
}

SistemaArchivos::~SistemaArchivos() {
    delete raiz;
}

int SistemaArchivos::buscarBinario(const vector<NodoArbol*>& hijos, const string& nombre) {
    int izq = 0, der = hijos.size() - 1;
    
    while (izq <= der) {
        int medio = izq + (der - izq) / 2;
        
        if (hijos[medio]->nombre == nombre) {
            return medio;
        }
        else if (hijos[medio]->nombre < nombre) {
            izq = medio + 1;
        }
        else {
            der = medio - 1;
        }
    }
    return -1; // No encontrado
}

void SistemaArchivos::insertarOrdenado(vector<NodoArbol*>& hijos, NodoArbol* nuevo) {
    auto pos = lower_bound(hijos.begin(), hijos.end(), nuevo,
        [](const NodoArbol* a, const NodoArbol* b) {
            return a->nombre < b->nombre;
        });
    hijos.insert(pos, nuevo);
}

vector<string> SistemaArchivos::dividirRuta(const string& ruta) {
    vector<string> componentes;
    string componente = "";
    
    for (char c : ruta) {
        if (c == '/') {
            if (!componente.empty()) {
                componentes.push_back(componente);
                componente = "";
            }
        } else {
            componente += c;
        }
    }
    
    if (!componente.empty()) {
        componentes.push_back(componente);
    }
    
    return componentes;
}

void SistemaArchivos::cargarDesdeDirectorio(const string& rutaBase) {
    try {
        for (const auto& entrada : recursive_directory_iterator(rutaBase)) {
            if (entrada.is_regular_file() || entrada.is_directory()) {
                string rutaRelativa = relative(entrada.path(), rutaBase).string();
                // Reemplazar backslashes con forward slashes en Windows
                replace(rutaRelativa.begin(), rutaRelativa.end(), '\\', '/');
                insertarRuta(rutaRelativa);
            }
        }
    }
    catch (const filesystem_error& ex) {
        cout << "Error accediendo al directorio: " << ex.what() << endl;
    }
}

void SistemaArchivos::insertarRuta(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    NodoArbol* actual = raiz;
    
    for (const string& componente : componentes) {
        int indice = buscarBinario(actual->hijos, componente);
        
        if (indice == -1) {
            // No existe, crear nuevo nodo
            NodoArbol* nuevo = new NodoArbol(componente);
            insertarOrdenado(actual->hijos, nuevo);
            actual = nuevo;
        } else {
            // Ya existe, moverse al nodo existente
            actual = actual->hijos[indice];
        }
    }
}

int SistemaArchivos::buscar(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    NodoArbol* actual = raiz;
    
    for (const string& componente : componentes) {
        int indice = buscarBinario(actual->hijos, componente);
        
        if (indice == -1) {
            return 1; // No existe
        }
        
        actual = actual->hijos[indice];
    }
    
    // Si llegamos aquí, existe
    return actual->esArchivo() ? 0 : 2; // 0 = archivo, 2 = directorio
}

int SistemaArchivos::insertar(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    
    if (componentes.empty()) return 2;
    
    // Buscar directorio padre
    NodoArbol* actual = raiz;
    for (int i = 0; i < componentes.size() - 1; i++) {
        int indice = buscarBinario(actual->hijos, componentes[i]);
        
        if (indice == -1) {
            return 2; // No existe ruta padre
        }
        
        actual = actual->hijos[indice];
    }
    
    // Verificar si ya existe el archivo/directorio
    string nombreNuevo = componentes.back();
    int indice = buscarBinario(actual->hijos, nombreNuevo);
    
    if (indice != -1) {
        return 1; // Ya existe
    }
    
    // Insertar nuevo nodo
    NodoArbol* nuevo = new NodoArbol(nombreNuevo);
    insertarOrdenado(actual->hijos, nuevo);
    
    return 0; // Éxito
}

bool SistemaArchivos::eliminar(const string& ruta) {
    vector<string> componentes = dividirRuta(ruta);
    
    if (componentes.empty()) return false;
    
    // Buscar directorio padre
    NodoArbol* padre = raiz;
    for (int i = 0; i < componentes.size() - 1; i++) {
        int indice = buscarBinario(padre->hijos, componentes[i]);
        
        if (indice == -1) {
            return false; // No existe ruta
        }
        
        padre = padre->hijos[indice];
    }
    
    // Buscar el nodo a eliminar
    string nombreEliminar = componentes.back();
    int indice = buscarBinario(padre->hijos, nombreEliminar);
    
    if (indice == -1) {
        return false; // No existe
    }
    
    // Eliminar nodo y subárbol
    delete padre->hijos[indice];
    padre->hijos.erase(padre->hijos.begin() + indice);
    
    return true;
}

void SistemaArchivos::obtenerTodasLasRutas(NodoArbol* nodo, string rutaActual, vector<string>& rutas) {
    if (nodo == raiz) {
        rutaActual = "";
    } else {
        if (!rutaActual.empty()) rutaActual += "/";
        rutaActual += nodo->nombre;
        
        // Agregar tanto archivos como directorios
        rutas.push_back(rutaActual);
    }
    
    for (auto hijo : nodo->hijos) {
        obtenerTodasLasRutas(hijo, rutaActual, rutas);
    }
}

void SistemaArchivos::obtenerDirectorios(NodoArbol* nodo, string rutaActual, vector<string>& directorios) {
    if (nodo == raiz) {
        rutaActual = "";
    } else {
        if (!rutaActual.empty()) rutaActual += "/";
        rutaActual += nodo->nombre;
        
        // Solo agregar directorios (nodos que no son hojas)
        if (!nodo->esArchivo()) {
            directorios.push_back(rutaActual);
        }
    }
    
    for (auto hijo : nodo->hijos) {
        obtenerDirectorios(hijo, rutaActual, directorios);
    }
}

vector<string> SistemaArchivos::obtenerRutasAleatorias(int cantidad) {
    vector<string> rutas;
    obtenerTodasLasRutas(raiz, "", rutas);
    
    if (rutas.size() <= cantidad) {
        return rutas;
    }
    
    // Seleccionar aleatoriamente
    random_device rd;
    mt19937 gen(rd());
    shuffle(rutas.begin(), rutas.end(), gen);
    
    return vector<string>(rutas.begin(), rutas.begin() + cantidad);
}

vector<string> SistemaArchivos::obtenerDirectoriosAleatorios(int cantidad) {
    vector<string> directorios;
    obtenerDirectorios(raiz, "", directorios);
    
    if (directorios.size() <= cantidad) {
        return directorios;
    }
    
    // Seleccionar aleatoriamente
    random_device rd;
    mt19937 gen(rd());
    shuffle(directorios.begin(), directorios.end(), gen);
    
    return vector<string>(directorios.begin(), directorios.begin() + cantidad);
}

int SistemaArchivos::calcularAltura(NodoArbol* nodo) {
    if (nodo->hijos.empty()) {
        return 0;
    }
    
    int alturaMaxima = 0;
    for (auto hijo : nodo->hijos) {
        alturaMaxima = max(alturaMaxima, calcularAltura(hijo));
    }
    
    return alturaMaxima + 1;
}

void SistemaArchivos::obtenerEstadisticas(int& totalNodos, int& totalArchivos, int& alturaMaxima) {
    totalNodos = 0;
    totalArchivos = 0;
    alturaMaxima = calcularAltura(raiz);
    
    vector<string> rutas;
    obtenerTodasLasRutas(raiz, "", rutas);
    
    totalNodos = rutas.size();
    
    // Contar archivos
    for (const string& ruta : rutas) {
        if (buscar(ruta) == 0) { // Es archivo
            totalArchivos++;
        }
    }
}

void SistemaArchivos::imprimirEstructura(int maxNiveles) {
    cout << "=== ESTRUCTURA DEL ÁRBOL ===" << endl;
    imprimirNodo(raiz, 0, maxNiveles);
    cout << "=============================" << endl;
}

void SistemaArchivos::imprimirNodo(NodoArbol* nodo, int nivel, int maxNiveles, const string& prefijo) {
    if (nivel > maxNiveles) return;
    
    cout << prefijo << nodo->nombre;
    if (nodo->esArchivo()) {
        cout << " (archivo)";
    } else {
        cout << " (directorio, " << nodo->hijos.size() << " hijos)";
    }
    cout << endl;
    
    if (nivel < maxNiveles) {
        for (int i = 0; i < nodo->hijos.size(); i++) {
            string nuevoPrefijo = prefijo + "  ";
            if (i == nodo->hijos.size() - 1) {
                nuevoPrefijo = prefijo + "  ";
            }
            imprimirNodo(nodo->hijos[i], nivel + 1, maxNiveles, nuevoPrefijo);
        }
    }
}