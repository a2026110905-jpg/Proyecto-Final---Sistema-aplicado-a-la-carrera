#include <iostream>
#include <string>
using namespace std;

const int MAX = 64;      // maximo de subredes que se pueden listar
const int MAX_IP = 100;  // maximo de IP que se pueden ocupar

// ---------------- DECLARACION DE FUNCIONES ----------------
int leerEntero(string mensaje, int minimo, int maximo);
long long tamanoBloque(int prefijo);
bool esDireccionDeRed(long long red, int prefijo);
void registrarDatosRequeridos(int &o1, int &o2, int &o3, int &o4, int &prefijo, int &subredes, int &hosts);
long long juntarOctetos(int o1, int o2, int o3, int o4);
string aTexto(long long numero);
bool calcularPrefijo(int prefijoBase, int cantidad, int hosts, int &nuevoPrefijo);
void generarSubredes(long long red, int nuevoPrefijo, int cantidad,
                     string subredes[], string primeros[],
                     string ultimos[], string broadcasts[]);
void mostrarTabla(int cantidad, int nuevoPrefijo, string subredes[],
                  string primeros[], string ultimos[], string broadcasts[]);
long long leerIP();
void registrarIP(string ipsOcupadas[], int &totalOcupadas,
                 long long red, int nuevoPrefijo, int cantidad);
void liberarIP(string ipsOcupadas[], int &totalOcupadas);
void mostrarOcupadas(string ipsOcupadas[], int totalOcupadas);
int mostrarMenu();

// ---------------- PROGRAMA PRINCIPAL ----------------
int main() {
    int o1, o2, o3, o4, prefijo, cantidad, hosts, nuevoPrefijo;
    long long red = 0;
    string subredes[MAX], primeros[MAX], ultimos[MAX], broadcasts[MAX];
    string ipsOcupadas[MAX_IP];
    int totalOcupadas = 0;
    bool calculado = false;
    int opcion;

    do {
        opcion = mostrarMenu();
        switch (opcion) {
            case 1:
                registrarDatosRequeridos(o1, o2, o3, o4, prefijo, cantidad, hosts);
                if (calcularPrefijo(prefijo, cantidad, hosts, nuevoPrefijo)) {
                    red = juntarOctetos(o1, o2, o3, o4);
                    generarSubredes(red, nuevoPrefijo, cantidad, subredes, primeros, ultimos, broadcasts);
                    calculado = true;
                    totalOcupadas = 0;  // nueva red: se limpia la lista de IP ocupadas
                    mostrarTabla(cantidad, nuevoPrefijo, subredes, primeros, ultimos, broadcasts);
                } else {
                    calculado = false;
                    cout << "\nNo es posible con esa red: no alcanzan los bits o los hosts." << endl;
                }
                break;
            case 2:
                if (calculado) {
                    mostrarTabla(cantidad, nuevoPrefijo, subredes, primeros, ultimos, broadcasts);
                } else {
                    cout << "\nPrimero calcule las subredes (opcion 1)." << endl;
                }
                break;
            case 3:
                if (calculado) {
                    registrarIP(ipsOcupadas, totalOcupadas, red, nuevoPrefijo, cantidad);
                } else {
                    cout << "\nPrimero calcule las subredes (opcion 1)." << endl;
                }
                break;
            case 4:
                liberarIP(ipsOcupadas, totalOcupadas);
                break;
            case 5:
                mostrarOcupadas(ipsOcupadas, totalOcupadas);
                break;
            case 6:
                cout << "\nSaliendo del programa..." << endl;
                break;
        }
    } while (opcion != 6);

    return 0;
}

// ---------------- DESARROLLO DE FUNCIONES ----------------

// VALIDACION: pide un entero y repite hasta que sea un numero
// dentro del rango [minimo, maximo]. Se usa en TODAS las entradas numericas.
int leerEntero(string mensaje, int minimo, int maximo) {
    int valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            // Se escribio algo que no es un numero (letras, simbolos, etc.)
            cin.clear();              // quita el estado de error de cin
            cin.ignore(1000, '\n');   // descarta lo que se escribio
            cout << "Entrada no valida: escriba un numero entero." << endl;
        } else {
            cin.ignore(1000, '\n');   // descarta lo que sobre en la linea
            if (valor < minimo || valor > maximo) {
                cout << "Valor fuera de rango: debe estar entre "
                     << minimo << " y " << maximo << "." << endl;
            } else {
                valido = true;
            }
        }
    } while (!valido);
    return valor;
}

// Cantidad total de direcciones de un bloque con ese prefijo (2^(32 - prefijo))
long long tamanoBloque(int prefijo) {
    long long tamano = 1;
    for (int i = 0; i < 32 - prefijo; i++) {
        tamano = tamano * 2;
    }
    return tamano;
}

// VALIDACION: una direccion de red debe ser multiplo del tamano del bloque
// (por ejemplo 192.168.10.5/24 NO es una direccion de red; 192.168.10.0/24 si)
bool esDireccionDeRed(long long red, int prefijo) {
    return red % tamanoBloque(prefijo) == 0;
}

// Pide todos los datos validados. Paso por referencia (&): los valores llegan a main.
void registrarDatosRequeridos(int &o1, int &o2, int &o3, int &o4, int &prefijo, int &subredes, int &hosts) {
    cout << "\nREGISTRO DE DIRECCION DE RED:\n";
    bool redValida = false;
    do {
        cout << endl;
        o1 = leerEntero("Ingrese el primer octeto (0-255): ", 0, 255);
        o2 = leerEntero("Ingrese el segundo octeto (0-255): ", 0, 255);
        o3 = leerEntero("Ingrese el tercer octeto (0-255): ", 0, 255);
        o4 = leerEntero("Ingrese el cuarto octeto (0-255): ", 0, 255);
        prefijo = leerEntero("\nIngrese el prefijo de la red (1-30): ", 1, 30);
        if (esDireccionDeRed(juntarOctetos(o1, o2, o3, o4), prefijo)) {
            redValida = true;
        } else {
            cout << "\n" << aTexto(juntarOctetos(o1, o2, o3, o4)) << "/" << prefijo
                 << " no es una direccion de red valida (tiene bits de host en 1)."
                 << "\nEjemplo: 192.168.10.0 con prefijo 24. Intente de nuevo." << endl;
        }
    } while (!redValida);
    subredes = leerEntero("\nCantidad de subredes requeridas (1-64): ", 1, MAX);
    hosts = leerEntero("Cantidad de hosts requeridos por subred (minimo 1): ", 1, 1000000000);
}

// Junta los 4 octetos en un solo numero (base 256)
long long juntarOctetos(int o1, int o2, int o3, int o4) {
    return (long long)o1 * 16777216 + o2 * 65536 + o3 * 256 + o4;
}

// Hace lo inverso: separa el numero en 4 octetos y arma el texto
string aTexto(long long numero) {
    int cuarto = numero % 256;
    numero = numero / 256;
    int tercero = numero % 256;
    numero = numero / 256;
    int segundo = numero % 256;
    numero = numero / 256;
    int primero = numero;
    return to_string(primero) + "." + to_string(segundo) + "." +
           to_string(tercero) + "." + to_string(cuarto);
}

// Calcula el nuevo prefijo. Devuelve true si se puede cumplir con
// las subredes y los hosts pedidos. nuevoPrefijo sale por referencia.
bool calcularPrefijo(int prefijoBase, int cantidad, int hosts, int &nuevoPrefijo) {
    int bitsPrestados = 0;
    int potencia = 1;
    while (potencia < cantidad) {
        potencia = potencia * 2;
        bitsPrestados++;
    }
    nuevoPrefijo = prefijoBase + bitsPrestados;
    if (nuevoPrefijo > 30) {
        return false;
    }
    return (tamanoBloque(nuevoPrefijo) - 2) >= hosts;
}

// Llena los 4 arreglos. La posicion i de cada arreglo describe la misma subred.
void generarSubredes(long long red, int nuevoPrefijo, int cantidad,
                     string subredes[], string primeros[],
                     string ultimos[], string broadcasts[]) {
    long long salto = tamanoBloque(nuevoPrefijo);
    for (int i = 0; i < cantidad; i++) {
        long long inicio = red + i * salto;
        long long fin = inicio + salto - 1;
        subredes[i] = aTexto(inicio);
        primeros[i] = aTexto(inicio + 1);
        ultimos[i] = aTexto(fin - 1);
        broadcasts[i] = aTexto(fin);
    }
}

void mostrarTabla(int cantidad, int nuevoPrefijo, string subredes[],
                  string primeros[], string ultimos[], string broadcasts[]) {
    cout << "\nNuevo prefijo: /" << nuevoPrefijo << endl;
    for (int i = 0; i < cantidad; i++) {
        cout << "\nSubred " << i + 1 << ": " << subredes[i] << "/" << nuevoPrefijo << endl;
        cout << "  Hosts utilizables: " << primeros[i] << " - " << ultimos[i] << endl;
        cout << "  Broadcast: " << broadcasts[i] << endl;
    }
}

// Pide una IP octeto por octeto (validados) y la devuelve como numero
long long leerIP() {
    int o1 = leerEntero("Primer octeto (0-255): ", 0, 255);
    int o2 = leerEntero("Segundo octeto (0-255): ", 0, 255);
    int o3 = leerEntero("Tercer octeto (0-255): ", 0, 255);
    int o4 = leerEntero("Cuarto octeto (0-255): ", 0, 255);
    return juntarOctetos(o1, o2, o3, o4);
}

void registrarIP(string ipsOcupadas[], int &totalOcupadas,
                 long long red, int nuevoPrefijo, int cantidad) {
    cout << "\nREGISTRAR (OCUPAR) UNA IP:\n";
    if (totalOcupadas >= MAX_IP) {
        cout << "\nLa lista de IP ocupadas esta llena." << endl;
        return;
    }
    long long ip = leerIP();
    string texto = aTexto(ip);
    long long salto = tamanoBloque(nuevoPrefijo);

    // VALIDACION: la IP debe estar dentro de alguna de las subredes calculadas
    if (ip < red || ip >= red + cantidad * salto) {
        cout << "\n" << texto << " no pertenece a ninguna de las subredes calculadas." << endl;
        return;
    }
    int numeroSubred = (int)((ip - red) / salto) + 1;
    long long posicion = (ip - red) % salto;  // 0 = direccion de subred

    // VALIDACION: no se puede ocupar la direccion de subred ni el broadcast
    if (posicion == 0) {
        cout << "\n" << texto << " es la direccion de la subred " << numeroSubred
             << ", no es utilizable." << endl;
        return;
    }
    if (posicion == salto - 1) {
        cout << "\n" << texto << " es el broadcast de la subred " << numeroSubred
             << ", no es utilizable." << endl;
        return;
    }
    // VALIDACION: no se puede ocupar dos veces la misma IP
    for (int i = 0; i < totalOcupadas; i++) {
        if (ipsOcupadas[i] == texto) {
            cout << "\n" << texto << " ya esta ocupada." << endl;
            return;
        }
    }
    ipsOcupadas[totalOcupadas] = texto;
    totalOcupadas++;
    cout << "\n" << texto << " registrada en la subred " << numeroSubred << "." << endl;
}

void liberarIP(string ipsOcupadas[], int &totalOcupadas) {
    cout << "\nLIBERAR UNA IP:\n";
    if (totalOcupadas == 0) {
        cout << "\nNo hay IP ocupadas." << endl;
        return;
    }
    string texto = aTexto(leerIP());

    int posicion = -1;
    for (int i = 0; i < totalOcupadas; i++) {
        if (ipsOcupadas[i] == texto) {
            posicion = i;
        }
    }
    if (posicion == -1) {
        cout << "\n" << texto << " no esta registrada como ocupada." << endl;
    } else {
        // Se corren una posicion hacia atras los elementos siguientes
        for (int i = posicion; i < totalOcupadas - 1; i++) {
            ipsOcupadas[i] = ipsOcupadas[i + 1];
        }
        totalOcupadas--;
        cout << "\n" << texto << " fue liberada." << endl;
    }
}

void mostrarOcupadas(string ipsOcupadas[], int totalOcupadas) {
    if (totalOcupadas == 0) {
        cout << "\nNo hay IP ocupadas." << endl;
    } else {
        cout << "\nIP OCUPADAS (" << totalOcupadas << "):" << endl;
        for (int i = 0; i < totalOcupadas; i++) {
            cout << i + 1 << ". " << ipsOcupadas[i] << endl;
        }
    }
}

int mostrarMenu() {
    cout << "\n===== AYUDANTE DE SUBREDES IPv4 =====" << endl;
    cout << "1. Ingresar red y calcular subredes" << endl;
    cout << "2. Ver tabla de subredes" << endl;
    cout << "3. Registrar (ocupar) una IP" << endl;
    cout << "4. Liberar una IP" << endl;
    cout << "5. Ver IP ocupadas" << endl;
    cout << "6. Salir" << endl;
    return leerEntero("Opcion: ", 1, 6);
}
