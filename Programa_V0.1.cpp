#include <iostream>
#include <string>
using namespace std;
const int MAX = 64;
long long aNumero(int o1, int o2, int o3, int o4);
string aTexto(long long numero);
bool calcularPrefijo(int prefijoBase, int subredes, int hosts, int &nuevoPrefijo);
void generarSubredes(long long red, int nuevoPrefijo, int cantidad,
                     string subredes[], string primeros[],
                     string ultimos[], string broadcasts[]);
void mostrarTabla(int cantidad, int nuevoPrefijo, string subredes[],
                  string primeros[], string ultimos[], string broadcasts[]);

int main() {
    int o1, o2, o3, o4, prefijo, cantidad, hosts, nuevoPrefijo;
    string subredes[MAX], primeros[MAX], ultimos[MAX], broadcasts[MAX];

    cout << "Direccion de red (4 octetos separados por espacio): ";
    cin >> o1 >> o2 >> o3 >> o4;
    cout << "Prefijo actual (ej. 24): ";
    cin >> prefijo;
    cout << "Cantidad de subredes: ";
    cin >> cantidad;
    cout << "Hosts minimos por subred: ";
    cin >> hosts;

    if (calcularPrefijo(prefijo, cantidad, hosts, nuevoPrefijo)) {
        long long red = aNumero(o1, o2, o3, o4);
        generarSubredes(red, nuevoPrefijo, cantidad, subredes, primeros, ultimos, broadcasts);
        mostrarTabla(cantidad, nuevoPrefijo, subredes, primeros, ultimos, broadcasts);
    } else {
        cout << "No es posible con esa red." << endl;
    }
    return 0;
}
long long aNumero(int o1, int o2, int o3, int o4) {
    return (long long)o1 * 16777216 + o2 * 65536 + o3 * 256 + o4;
}

string aTexto(long long n) {
    int o1 = n / 16777216;
    int o2 = (n / 65536) % 256;
    int o3 = (n / 256) % 256;
    int o4 = n % 256;
    return to_string(o1) + "." + to_string(o2) + "." + to_string(o3) + "." + to_string(o4);
}
bool calcularPrefijo(int prefijoBase, int subredes, int hosts, int &nuevoPrefijo) {
    int bitsPrestados = 0;
    int potencia = 1;
    while (potencia < subredes) {
        potencia = potencia * 2;
        bitsPrestados++;
    }
    nuevoPrefijo = prefijoBase + bitsPrestados;
    if (nuevoPrefijo > 30) {
        return false;
    }
    long long tamano = 1;
    for (int i = 0; i < 32 - nuevoPrefijo; i++) {
        tamano = tamano * 2;
    }
    return (tamano - 2) >= hosts;
}

void generarSubredes(long long red, int nuevoPrefijo, int cantidad,
                     string subredes[], string primeros[],
                     string ultimos[], string broadcasts[]) {
    long long salto = 1;
    for (int i = 0; i < 32 - nuevoPrefijo; i++) {
        salto = salto * 2;
    }
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
        cout << "Subred " << i + 1 << ": " << subredes[i]
             << " | Hosts: " << primeros[i] << " - " << ultimos[i]
             << " | Broadcast: " << broadcasts[i] << endl;
    }
}