#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iomanip>
#include <algorithm>

using namespace std;

const int FILAS = 10;
const int COLUMNAS = 10;
const int MINAS = 15;
const int PUNTOS_POR_ACIERTO = 10;

struct Casilla {
    bool mina;
    bool descubierta;
    int vecinas;
};

struct Puntaje {
    string nombre;
    int puntos;
    int racha;
};

void inicializarTablero(vector<vector<Casilla>> &tablero) {
    for (int i = 0; i < FILAS; ++i) {
        vector<Casilla> fila;
        for (int j = 0; j < COLUMNAS; ++j) {
            Casilla casilla = {false, false, 0};
            fila.push_back(casilla);
        }
        tablero.push_back(fila);
    }
}

void colocarMinas(vector<vector<Casilla>> &tablero) {
    int minasColocadas = 0;
    while (minasColocadas < MINAS) {
        int fila = rand() % FILAS;
        int columna = rand() % COLUMNAS;
        if (!tablero[fila][columna].mina) {
            tablero[fila][columna].mina = true;
            minasColocadas++;
        }
    }
}

void contarMinasVecinas(vector<vector<Casilla>> &tablero, int fila, int columna) {
    static int movimientos[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (int i = 0; i < 8; ++i) {
        int nuevaFila = fila + movimientos[i][0];
        int nuevaColumna = columna + movimientos[i][1];
        if (nuevaFila >= 0 && nuevaFila < FILAS && nuevaColumna >= 0 && nuevaColumna < COLUMNAS && tablero[nuevaFila][nuevaColumna].mina) {
            tablero[fila][columna].vecinas++;
        }
    }
}

void contarMinasVecinasEnTablero(vector<vector<Casilla>> &tablero) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (!tablero[i][j].mina) {
                contarMinasVecinas(tablero, i, j);
            }
        }
    }
}

void mostrarTablero(const vector<vector<Casilla>> &tablero, bool mostrarMinas = false) {
    system("cls");
    cout << "   ";
    for (int j = 0; j < COLUMNAS; ++j) {
        cout << " " << j + 1 << " ";
    }
    cout << endl;

    cout << "  +";
    for (int j = 0; j < COLUMNAS; ++j) {
        cout << "---";
    }
    cout << "+" << endl;

    char letra = 'A';
    for (int i = 0; i < FILAS; ++i) {
        cout << letra++ << " |";
        for (int j = 0; j < COLUMNAS; ++j) {
            if (tablero[i][j].descubierta) {
                if (tablero[i][j].mina) {
                    if (mostrarMinas) {
                        cout << "\033[1;31m" << " * " << "\033[0m";
                    } else {
                        cout << " * ";
                    }
                } else if (tablero[i][j].vecinas > 0) {
                    cout << " " << tablero[i][j].vecinas << " ";
                } else {
                    cout << "   ";
                }
            } else if (mostrarMinas && tablero[i][j].mina) {
                cout << "\033[1;31m" << " * " << "\033[0m";
            } else {
                cout << " # ";
            }
        }
        cout << "|" << endl;
    }

    cout << "  +";
    for (int j = 0; j < COLUMNAS; ++j) {
        cout << "---";
    }
    cout << "+" << endl;
}

bool casillaEstaEnTablero(int fila, int columna) {
    return fila >= 0 && fila < FILAS && columna >= 0 && columna < COLUMNAS;
}

void descubrirCasilla(vector<vector<Casilla>> &tablero, int fila, int columna) {
    if (!casillaEstaEnTablero(fila, columna) || tablero[fila][columna].descubierta) {
        return;
    }
    tablero[fila][columna].descubierta = true;
    if (tablero[fila][columna].vecinas == 0) {
        static int movimientos[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        for (int i = 0; i < 8; ++i) {
            int nuevaFila = fila + movimientos[i][0];
            int nuevaColumna = columna + movimientos[i][1];
            descubrirCasilla(tablero, nuevaFila, nuevaColumna);
        }
    }
}

bool tableroCompletoDescubierto(const vector<vector<Casilla>> &tablero) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (!tablero[i][j].mina && !tablero[i][j].descubierta) {
                return false;
            }
        }
    }
    return true;
}

void mostrarTablaPuntajes(const vector<Puntaje> &puntajes) {
    cout << "\nTabla de puntajes:\n";
    cout << "------------------\n";
    cout << setw(10) << left << "Nombre" << setw(10) << "Puntos" << "Racha\n";
    for (const auto &puntaje : puntajes) {
        cout << setw(10) << left << puntaje.nombre << setw(10) << puntaje.puntos << puntaje.racha << endl;
    }
}

int main() {
    srand(time(0));
    vector<vector<Casilla>> tablero;
    inicializarTablero(tablero);
    colocarMinas(tablero);
    contarMinasVecinasEnTablero(tablero);

    cout << "Bienvenido a Buscaminas\n";
    cout << "Ingrese su nombre: ";
    string nombre;
    cin >> nombre;

    mostrarTablero(tablero);
    this_thread::sleep_for(chrono::seconds(5)); // Esperar 5 segundos

    int puntos = 0;
    int racha = 0;
    while (true) {
        cout << "\nIngrese la fila y columna (ejemplo: A1): ";
        char letra;
        int columna;
        cin >> letra >> columna;
        int fila = letra - 'A';
        columna--;
        if (!casillaEstaEnTablero(fila, columna)) {
            cout << "Posición inválida.\n";
            continue;
        }
        if (tablero[fila][columna].descubierta) {
            cout << "Casilla ya descubierta.\n";
            continue;
        }
        if (tablero[fila][columna].mina) {
            cout << "¡Has perdido!\n";
            mostrarTablero(tablero, true); // Mostrar minas en rojo
            break;
        }
        descubrirCasilla(tablero, fila, columna);
        mostrarTablero(tablero);
        racha++;
        puntos += PUNTOS_POR_ACIERTO * racha;

        if (tableroCompletoDescubierto(tablero)) {
            cout << "¡Has ganado!\n";
            break;
        }
    }

    cout << "Tu puntuación: " << puntos << endl;

    char opcion;
    cout << "¿Deseas ver la tabla de puntajes? (S/N): ";
    cin >> opcion;
    if (opcion == 'S' || opcion == 's') {
        // Aquí deberías tener una estructura de datos que contenga los puntajes y las rachas de cada jugador
        vector<Puntaje> puntajes = {
            {nombre, puntos, racha},
            {"Jugador1", 100, 3}
        };
        sort(puntajes.begin(), puntajes.end(), [](const Puntaje &a, const Puntaje &b) {
            return a.puntos > b.puntos;
        });
        mostrarTablaPuntajes(puntajes);
    }

    cout << "¿Deseas jugar de nuevo? (S/N): ";
    cin >> opcion;
    if (opcion == 'S' || opcion == 's') {
        main();
    }

    return 0;
}
