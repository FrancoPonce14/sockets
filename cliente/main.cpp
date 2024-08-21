#include <iostream>
#include <winsock2.h>
#include <thread>
#include <cstring>
#include <ws2tcpip.h>
#include <stdio.h>

using namespace std;

int puerto = 0;

class Client {
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];

    Client() {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        cout << "Conectando al servidor..." << endl << endl;
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        iResult = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if (iResult == SOCKET_ERROR) {
            wprintf(L"No se pudo conectar, error: %ld\n", WSAGetLastError());
            iResult = closesocket(server);
            if (iResult == SOCKET_ERROR)
                wprintf(L"No se pudo cerrar el socket, error: %ld\n", WSAGetLastError());
            WSACleanup();
            system("pause");
            exit(EXIT_SUCCESS);
        }
        cout << "Conectado al Servidor!" << endl;
    }

    void Enviar(string mensaje) {
        strcpy(buffer, mensaje.c_str());
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    string Recibir() {
        recv(server, buffer, sizeof(buffer), 0);
        string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }

    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado.\n";
    }
};

int verificarUsername(string mensaje) {
    int resultado = 0;
    if (strcasecmp(mensaje.c_str(), "volver") == 0) {
        resultado = 2;
    }else if (std::stoi(mensaje) > 15 || std::stoi(mensaje) < 5) {
        resultado = 1;
    }
    return resultado;
}

int verificarPassword(string mensaje) {
    int resultado = 0;
    if (strcasecmp(mensaje.c_str(), "volver") == 0) {
        resultado = 2;
    } else if (std::stoi(mensaje) >= 50 || std::stoi(mensaje) < 8) {
        resultado = 1;
    }
    return resultado;
}

int main() {
    cout << "Introducir Puerto de conexion\n";
    cin >> puerto;

    Client* Cliente = new Client();

    system("pause");

    boolean loop = true;
    int menu = 0;
    string mensaje = " ";
    int verificacion = 0;
    int eof = 1;

    while (loop) {
        switch (menu) {
            case 1:
                system("cls");
                cout << "\n******Generador de nombres de usuario******\n\n"
                     << "Escribi \"volver\" para regresar al menu."
                     << "\nIntroducir longitud del nombre de usuario:\n";
                cin >> mensaje;

                verificacion = verificarUsername(mensaje);

                switch (verificacion) {
                    case 1:
                        cout << "ERROR la cantidad de caracteres"
                             << "no esta entre los 5 y 15 permitidos\n";
                        system("pause");
                        break;

                    case 2:
                        menu = 0;
                        break;

                    default:
                        Cliente->Enviar("USERNAME:" + mensaje);
                        mensaje = Cliente->Recibir();
                        if (!mensaje.empty()) {
                            cout << "Respuesta:\n" << mensaje << endl << endl;
                            system("pause");
                        } else {
                            cout << "\nEl mensaje no se envio,"
                                 << "se corto la conexion por inactividad\n"
                                 << "Se va a cerrar el programa\n\n";
                            system("pause");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                }
                break;

            case 2:
                system("cls");
                cout << "\n******Generador de contrasenias******\n\n"
                     << "Escribi \"volver\" para regresar al menu."
                     << "\nIntroducir longitud de la contrania:\n";
                cin >> mensaje;

                verificacion = verificarPassword(mensaje);

                switch (verificacion) {
                    case 1:
                        cout << "ERROR la cantidad de caracteres"
                             << " no esta entre los 8 y 50 permitidos\n";
                        system("pause");
                        break;

                    case 2:
                        menu = 0;
                        break;

                    default:
                        Cliente->Enviar("PASSWORD:" + mensaje);
                        mensaje = Cliente->Recibir();
                        if (!mensaje.empty()) {
                            cout << "Respuesta:\n" << mensaje << endl << endl;
                            system("pause");
                        } else {
                            cout << "\nEl mensaje no se envio,"
                                 << "se corto la conexion por inactividad\n"
                                 << "Se va a cerrar el programa\n\n";
                            system("pause");
                            exit(EXIT_SUCCESS);
                        }
                        break;
                }
                break;

            case 3:
                Cliente->Enviar("Cerrar Sesion");
                Cliente->CerrarSocket();
                loop = false;
                break;

            default:
                system("cls");
                cout << "**************MENU**************\n\n Elegir funcion\n"
                     << " 1- Generar nombre de usuario\n 2- Generar contrasenia\n"
                     << " 3- Cerrar Sesion\n";
                cin >> menu;
                break;
        }
    }
}
