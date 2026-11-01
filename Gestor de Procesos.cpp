#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cstdlib>

using namespace std;


struct Proceso {
    int id;
    string nombre;
    int prioridad;       
    int memoriaMB;       
    string estado;       
};

struct BloqueMemoria {
    int idBloque;
    int idProceso;
    int tamanoMB;
};

struct NodoProceso {
    Proceso info;
    NodoProceso* siguiente;
    
    NodoProceso(const Proceso& p) : info(p), siguiente(NULL) {}
};

struct NodoMemoria {
    BloqueMemoria info;
    NodoMemoria* siguiente;
    
    NodoMemoria(const BloqueMemoria& b) : info(b), siguiente(NULL) {}
};

class ListaProcesos {
private:
    NodoProceso* cabeza;
    int contador;


public:
    ListaProcesos() : cabeza(NULL), contador(0) {}


    ~ListaProcesos() {
        limpiar();
    }


    void limpiar() {
        NodoProceso* actual = cabeza;
        while (actual != NULL) {
            NodoProceso* aux = actual;
            actual = actual->siguiente;
            delete aux;
        }
        cabeza = NULL;
        contador = 0;
    }


    bool idExiste(int id) const {
        NodoProceso* actual = cabeza;
        while (actual != NULL) {
            if (actual->info.id == id) return true;
            actual = actual->siguiente;
        }
        return false;
    }


    bool insertar(const Proceso& p) {
        if (idExiste(p.id)) {
            cout << " [ERROR] El ID " << p.id << " ya esta registrado.\n";
            return false;
        }


        NodoProceso* nuevo = new NodoProceso(p);
        if (cabeza == NULL) {
            cabeza = nuevo;
        } else {
            NodoProceso* actual = cabeza;
            while (actual->siguiente != NULL) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
        contador++;
        return true;
    }


    bool eliminar(int id) {
        if (cabeza == NULL) return false;


        if (cabeza->info.id == id) {
            NodoProceso* aBorrar = cabeza;
            cabeza = cabeza->siguiente;
            delete aBorrar;
            contador--;
            return true;
        }

        NodoProceso* actual = cabeza;
        while (actual->siguiente != NULL && actual->siguiente->info.id != id) {
            actual = actual->siguiente;
        }


        if (actual->siguiente != NULL) {
            NodoProceso* aBorrar = actual->siguiente;
            actual->siguiente = aBorrar->siguiente;
            delete aBorrar;
            contador--;
            return true;
        }


        return false;
    }


    NodoProceso* buscarPorId(int id) const {
        NodoProceso* actual = cabeza;
        while (actual != NULL) {
            if (actual->info.id == id) return actual;
            actual = actual->siguiente;
        }
        return NULL;
    }


    void buscarPorNombre(const string& nombre) const {
        NodoProceso* actual = cabeza;
        bool encontrado = false;
        cout << "\n --- Resultados de Busqueda para '" << nombre << "' ---\n";
        while (actual != NULL) {
            if (actual->info.nombre.find(nombre) != string::npos) {
                cout << " ID: " << actual->info.id ;
                cout << " Nombre: " << actual->info.nombre ;
                cout << " Pri: " << actual->info.prioridad ;
                cout << " Mem: " << actual->info.memoriaMB << "MB" ;
                cout << " Estado: " << actual->info.estado << "\n";
                encontrado = true;
            }
            actual = actual->siguiente;
        }
        if (!encontrado) cout << " No se encontraron procesos coincidentes.\n";
    }

    bool modificarPrioridad(int id, int nuevaPrioridad) {
        NodoProceso* nodo = buscarPorId(id);
        if (nodo != NULL) {
            nodo->info.prioridad = nuevaPrioridad;
            return true;
        }
        return false;
    }

    bool modificarEstado(int id, const string& nuevoEstado) {
        NodoProceso* nodo = buscarPorId(id);
        if (nodo != NULL) {
            nodo->info.estado = nuevoEstado;
            return true;
        }
        return false;
    }


    void mostrar() const {
        if (cabeza == NULL) {
            cout << "\n [INFO] La lista de procesos esta vacia.\n";
            return;
        }


        cout << "\n =====================================================================\n";
        cout << "                       GESTOR DE PROCESOS (LISTA)                     \n";
        cout << " =====================================================================\n";
        cout << left << setw(8) << "ID"
             << setw(20) << "Nombre"
             << setw(12) << "Prioridad"
             << setw(14) << "Memoria(MB)"
             << setw(15) << "Estado" << "\n";
        cout << " ---------------------------------------------------------------------\n";


        NodoProceso* actual = cabeza;
        while (actual != NULL) {
            cout << left << setw(8) << actual->info.id
                 << setw(20) << actual->info.nombre
                 << setw(12) << actual->info.prioridad
                 << setw(14) << actual->info.memoriaMB
                 << setw(15) << actual->info.estado << "\n";
            actual = actual->siguiente;
        }
        cout << " ---------------------------------------------------------------------\n";
        cout << " Total de procesos registrados: " << contador << "\n";
    }


    NodoProceso* getCabeza() const { return cabeza; }
};




class ColaPrioridadCPU {
private:
    NodoProceso* frente;
    ListaProcesos historialAtendidos; 


public:
    ColaPrioridadCPU() : frente(NULL) {}


    ~ColaPrioridadCPU() {
        limpiar();
    }


    void limpiar() {
        while (frente != NULL) {
            NodoProceso* aux = frente;
            frente = frente->siguiente;
            delete aux;
        }
    }


    void encolar(const Proceso& p) {
        NodoProceso* nuevo = new NodoProceso(p);
        if (frente == NULL || p.prioridad > frente->info.prioridad) {
            nuevo->siguiente = frente;
            frente = nuevo;
        } else {
            NodoProceso* actual = frente;
            while (actual->siguiente != NULL && actual->siguiente->info.prioridad >= p.prioridad) {
                actual = actual->siguiente;
            }
            nuevo->siguiente = actual->siguiente;
            actual->siguiente = nuevo;
        }
    }


    bool desencolarYEjecutar(Proceso& pEjecutado) {
        if (frente == NULL) return false;


        NodoProceso* aux = frente;
        pEjecutado = aux->info;
        pEjecutado.estado = "Terminado";
        
 
        historialAtendidos.insertar(pEjecutado);


        frente = frente->siguiente;
        delete aux;
        return true;
    }


    void mostrarColaPendiente() const {
        if (frente == NULL) {
            cout << "\n [INFO] La cola de ejecucion de CPU esta vacia (Sin pendientes).\n";
            return;
        }


        cout << "\n =====================================================================\n";
        cout << "             COLA PENDIENTE DE ATENCION (PLANIFICADOR CPU)            \n";
        cout << " =====================================================================\n";
        cout << left << setw(8) << "ID"
             << setw(20) << "Nombre"
             << setw(12) << "Prioridad"
             << setw(15) << "Estado" << "\n";
        cout << " ---------------------------------------------------------------------\n";


        NodoProceso* actual = frente;
        while (actual != NULL) {
            cout << left << setw(8) << actual->info.id
                 << setw(20) << actual->info.nombre
                 << setw(12) << actual->info.prioridad
                 << setw(15) << actual->info.estado << "\n";
            actual = actual->siguiente;
        }
        cout << " ---------------------------------------------------------------------\n";
    }


    void mostrarProcesosAtendidos() const {
        cout << "\n --- HISTORIAL DE PROCESOS ATENDIDOS POR LA CPU ---";
        historialAtendidos.mostrar();
    }


    bool estaVacia() const { return frente == NULL; }
};


class PilaMemoria {
private:
    NodoMemoria* tope;
    int capacidadTotalMB;
    int memoriaUsadaMB;
    int contadorBloques;


public:
    PilaMemoria(int capacidadMB = 2048) 
        : tope(NULL), capacidadTotalMB(capacidadMB), memoriaUsadaMB(0), contadorBloques(0) {}


    ~PilaMemoria() {
        limpiar();
    }


    void limpiar() {
        while (tope != NULL) {
            NodoMemoria* aux = tope;
            tope = tope->siguiente;
            delete aux;
        }
        memoriaUsadaMB = 0;
        contadorBloques = 0;
    }


    bool push(int idProceso, int tamanoMB) {
        if (memoriaUsadaMB + tamanoMB > capacidadTotalMB) {
            cout << " [ERROR] Memoria insuficiente. Disponible: " 
                 << (capacidadTotalMB - memoriaUsadaMB) << " MB\n";
            return false;
        }


        contadorBloques++;
        BloqueMemoria b = { contadorBloques, idProceso, tamanoMB };
        NodoMemoria* nuevo = new NodoMemoria(b);
        nuevo->siguiente = tope;
        tope = nuevo;
        memoriaUsadaMB += tamanoMB;
        return true;
    }


    bool pop(BloqueMemoria& bLiberado) {
        if (tope == NULL) return false;


        NodoMemoria* aux = tope;
        bLiberado = aux->info;
        memoriaUsadaMB -= bLiberado.tamanoMB;
        tope = tope->siguiente;
        delete aux;
        return true;
    }


    void mostrarProcesoCima() const {
        if (tope == NULL) {
            cout << "\n [INFO] La pila de memoria esta vacia. No hay bloques en la cima.\n";
            return;
        }
        cout << "\n =====================================================================\n";
        cout << "                   PROCESO EN LA CIMA DE LA PILA (PEEK)               \n";
        cout << " =====================================================================\n";
        cout << " ID Bloque : " << tope->info.idBloque << "\n";
        cout << " ID Proceso: " << tope->info.idProceso << "\n";
        cout << " Tamano MB : " << tope->info.tamanoMB << " MB\n";
        cout << " =====================================================================\n";
    }


    void mostrarEstado() const {
        cout << "\n =====================================================================\n";
        cout << "                      GESTOR DE MEMORIA RAM (PILA)                    \n";
        cout << " =====================================================================\n";
        cout << " Capacidad Total: " << capacidadTotalMB << " MB | Usada: "<< memoriaUsadaMB << " MB | Libre: " << (capacidadTotalMB - memoriaUsadaMB) << " MB\n";
        cout << " ---------------------------------------------------------------------\n";


        if (tope == NULL) {
            cout << " [INFO] No hay bloques de memoria asignados.\n";
            return;
        }


        cout << left << setw(12) << "ID Bloque"
             << setw(14) << "ID Proceso"
             << setw(16) << "Tamano (MB)" << "\n";
        cout << " -----------------------------------------------------------------\n";


        NodoMemoria* actual = tope;
        while (actual != NULL) {
            cout << left << setw(12) << actual->info.idBloque
                 << setw(14) << actual->info.idProceso
                 << setw(16) << actual->info.tamanoMB << "\n";
            actual = actual->siguiente;
        }
        cout << " ---------------------------------------------------------------------\n";
    }


    int getMemoriaUsada() const { return memoriaUsadaMB; }
    int getCapacidad() const { return capacidadTotalMB; }
};

class GestorArchivos
{
	public:
	    static bool guardarProcesos(const ListaProcesos& lista, const string& ruta)
		{
	        ofstream archivo(ruta.c_str());
	        if (!archivo.is_open()) return false;
	
	        NodoProceso* actual = lista.getCabeza();
	        while (actual != NULL)
			{
	            archivo << actual->info.id << ";"
	                    << actual->info.nombre << ";"
	                    << actual->info.prioridad << ";"
	                    << actual->info.memoriaMB << ";"
	                    << actual->info.estado << "\n";
	            actual = actual->siguiente;
	        }
	        archivo.close();
	        return true;
	    }
	
	    static bool cargarProcesos(ListaProcesos& lista, const string& ruta)
		{
	        ifstream archivo(ruta.c_str());
	        if (!archivo.is_open()) return false;
	
	        lista.limpiar();
	        string linea;
	        while (getline(archivo, linea))
			{
	            if (linea.empty()) continue;
	
	            size_t p1 = linea.find(';');
	            size_t p2 = linea.find(';', p1 + 1);
	            size_t p3 = linea.find(';', p2 + 1);
	            size_t p4 = linea.find(';', p3 + 1);
	
	            if (p1 != string::npos && p2 != string::npos && p3 != string::npos && p4 != string::npos)
				{
	                Proceso p;
	                p.id = atoi(linea.substr(0, p1).c_str());
	                p.nombre = linea.substr(p1 + 1, p2 - p1 - 1);
	                p.prioridad = atoi(linea.substr(p2 + 1, p3 - p2 - 1).c_str());
	                p.memoriaMB = atoi(linea.substr(p3 + 1, p4 - p3 - 1).c_str());
	                p.estado = linea.substr(p4 + 1);
	
	                lista.insertar(p);
	            }
	        }
            archivo.close();
	        return true;
	    }
};

int leerEnteroValido(const string& mensaje, int minVal = 0, int maxVal = 999999)
{
    int valor;
    while (true)
	{
        cout << mensaje;
        if (cin >> valor && valor >= minVal && valor <= maxVal)
		{
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
		else
		{
            cout << " [ENTRADA INVALIDA] Ingrese un numero valido entre " << minVal << " y " << maxVal << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string leerTexto(const string& mensaje)
{
    string texto;
    cout << mensaje;
    getline(cin, texto);
    return texto;
}

void mostrarMenuPrincipal()
{
    cout << "\n =====================================================================\n";
    cout << "             SISTEMA DE GESTION DE PROCESOS              \n";
    cout << " =====================================================================\n";
    cout << "  1. Registrar nuevo proceso en Lista\n";
    cout << "  2. Eliminar proceso por ID de Lista\n";
    cout << "  3. Buscar procesos por ID o Nombre\n";
    cout << "  4. Modificar prioridad de proceso\n";
    cout << "  5. Modificar estado de proceso\n";
    cout << "  6. Encolar proceso en CPU (Encolar)\n";
    cout << "  7. Ejecutar proceso en CPU (Desencolar)\n";
    cout << "  8. Asignar memoria a proceso (Pila)\n";
    cout << "  9. Liberar ultimo bloque de memoria (Pila)\n";
    cout << " 10. Mostrar estado general de Lista, CPU y Memoria\n";
    cout << " 11. Guardar estado en archivo\n";
    cout << " 12. Cargar estado desde archivo\n";
    cout << "  0. Salir del programa\n";
    cout << " =====================================================================\n";
}

