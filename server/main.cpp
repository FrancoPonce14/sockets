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

string vocales = "AEIOUaeiou";
string consonantes = "BCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz";
string numeros = "0123456789";
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
    }

    bool AceptarCliente() {
        int clientAddrSize = sizeof(clientAddr);
        client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (client != INVALID_SOCKET) {
            cout << "Cliente conectado!" << endl;
            return true;
        }
        return false;
    }

    string Recibir() {
        int bytesRecibidos = recv(client, buffer, sizeof(buffer), 0);
        if (bytesRecibidos > 0) {
            return string(buffer, bytesRecibidos);  // Solo usar la cantidad de bytes recibidos
        }
        return "";
    }

    void Enviar(string mensaje) {
        send(client, mensaje.c_str(), mensaje.length(), 0);
    }

    void CerrarSocketCliente() {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado.\n" << endl;
    }

    void CerrarSocketServidor() {
        closesocket(server);
        WSACleanup();
        cout << "Servidor cerrado.\n" << endl;
    }
};

void TimeOut(Server* servidor) {
    int tiempo = 0;
    while (esperando) {
        Sleep(1000);
        tiempo++;
        if (tiempo >= 120) {
            cout << "\nTimeout, se va a cerrar la conexion con el cliente\n";
            servidor->CerrarSocketCliente();
            conexion = false;
            break;
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
                int vocalAleatoria = rand() % vocales.length();
                resultado += vocales[vocalAleatoria];
            }
        }
    } else if (consonantes.find(letraInicial) != string::npos) {
        for (int i = 1; i < longitud; i++) {
            if (i % 2 == 1) {
                int vocalAleatoria = rand() % vocales.length();
                resultado += vocales[vocalAleatoria];
            } else {
                int consonanteAleatoria = rand() % consonantes.length();
                resultado += consonantes[consonanteAleatoria];
            }
        }
    }

    return resultado;
}

string GenerarPassword(int longitud) {
    string alfanumericos = numeros + consonantes + vocales;
    string resultado = "";
    for(int i = 0; i < longitud; i++){
        int alfanumericoAleatorio = rand() % alfanumericos.length();
        resultado += alfanumericos[alfanumericoAleatorio];
    }
    return resultado;
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

    while (true) {
        if (Servidor->AceptarCliente()) {
            string mensaje = "";

            while (conexion) {
                cout << ": Esperando recibir mensaje" << endl;

                thread timeOut(TimeOut, Servidor);
                esperando = true;
                mensaje = Servidor->Recibir();
                esperando = false;
                timeOut.join();

                if (mensaje.empty()) {
                    cout << "Error al recibir mensaje, cerrando la conexión." << endl;
                    conexion = false;
                    break;
                }

                cout << ": Mensaje recibido: " << mensaje << endl;

                string respuesta = ResponderCadena(mensaje);

                cout << ": Respuesta enviada: " << respuesta << endl;

                Servidor->Enviar(respuesta);

                if (!conexion) {
                    Servidor->CerrarSocketCliente();
                    break;
                }
            }
        }
    }

    Servidor->CerrarSocketServidor();
    delete Servidor;

    return 0;
}
