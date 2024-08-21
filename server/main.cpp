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

string generarUsername() {
    return "generoUsername";
}

string generarPassword() {
    return "generoPassword";
}

string ResponderCalculo(string mensaje) {
    string resultado = "";
    if(mensaje.find("USERNAME:") == 0){
        resultado = generarUsername();
    } else if(mensaje.find("PASSWORD:") == 0) {
        resultado = generarPassword();
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

            string respuesta = ResponderCalculo(mensaje);

            cout << ": Respuesta enviada: " << respuesta << endl;

            Servidor->Enviar(respuesta);
        }
        conexion = true;
        Servidor->EsperarCliente();
    }
}

