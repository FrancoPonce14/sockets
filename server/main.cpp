#include <iostream>
#include <winsock2.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <thread>

using namespace std;

#define PUERTO 5000

string vocales = "AEIOU";
string consonantes = "BCDFGHJKLMNPQRSTVWXYZ";
bool esperando = false;
bool conexion = true;

class Server {
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];

    Server() {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PUERTO);

        bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes en el puerto: " << PUERTO << endl;

        int clientAddrSize = sizeof(clientAddr);
        if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
            cout << "Cliente conectado!" << endl;
        }
    }

    void EsperarCliente() {
        bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;

        int clientAddrSize = sizeof(clientAddr);
        if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
            cout << "Cliente conectado!" << endl;
        }
    }

    string Recibir() {
        recv(client, buffer, sizeof(buffer), 0);
        string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }

    void Enviar(string mensaje) {
        strcpy(buffer, mensaje.c_str());
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    void CerrarSocket() {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado.\n" << endl;
    }
};

void TimeOut(Server* servidor) {
    int tiempo = 0;
    while (esperando) {
        Sleep(1000);
        tiempo++;
        if (tiempo >= 120) {
            cout << "\nTimeout, se va a cerrar la conexion con el cliente\n";
            servidor->CerrarSocket();
            conexion = false;
        }
    }
}

char LetraInicialAleatoria() {
    string todo = vocales + consonantes;
    srand(time(0));
    int i = rand() % todo.length();
    return todo[i];
}

string GenerarUsername(int longitud) {
    string resultado = "";
    char letraInicial = LetraInicialAleatoria();
    resultado = letraInicial;

    if (vocales.find(letraInicial) != string::npos) {
        for (int i = 1; i < longitud; i++) {
            if (i % 2 == 1) {
                int consonanteAleatoria = rand() % consonantes.length();
                resultado += consonantes[consonanteAleatoria];
            } else {
                int vocalesAleatoria = rand() % vocales.length();
                resultado += vocales[vocalesAleatoria];
            }
        }
    } else if (consonantes.find(letraInicial) != string::npos) {
        for (int i = 1; i < longitud; i++) {
            if (i % 2 == 1) {
                int vocalesAleatoria = rand() % vocales.length();
                resultado += vocales[vocalesAleatoria];
            } else {
                int consonanteAleatoria = rand() % consonantes.length();
                resultado += consonantes[consonanteAleatoria];
            }
        }
    }

    return resultado;
}

string GenerarPassword(int longitud) {
    return "generoPassword";
}

string ResponderCadena(string mensaje) {
    string resultado = "";
    if(mensaje.find("USERNAME:") == 0){
        int longitud = stoi(mensaje.substr(9));
        resultado = GenerarUsername(longitud);
    } else if(mensaje.find("PASSWORD:") == 0) {
         int longitud = stoi(mensaje.substr(9));
        resultado = GenerarPassword(longitud);
    } else {
        resultado = "Conexion cerrada";
        conexion = false;
    }

    return resultado;
}

int main() {
    Server* Servidor = new Server();
    string mensaje = " ";

    while (true) {
        while (conexion) {

            cout << ": Esperando recibir mensaje" << endl;

            thread timeOut(TimeOut, Servidor);
            esperando = true;
            mensaje = Servidor->Recibir();
            esperando = false;
            timeOut.join();
            cout << ": Mensaje recibido: " << mensaje << endl;

            if (!conexion) {
                break;
            }

            string respuesta = ResponderCadena(mensaje);

            cout << ": Respuesta enviada: " << respuesta << endl;

            Servidor->Enviar(respuesta);
        }
        conexion = true;
        Servidor->EsperarCliente();
    }
}

