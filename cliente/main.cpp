#include <cstdio>
#include <winsock2.h>
#include <cstring>
#include <cstdlib>
#include <string>

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
        printf("Conectando al servidor...\n\n");
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
        printf("Conectado al Servidor!\n");
    }

    void Enviar(const std::string& mensaje) {
        strcpy(buffer, mensaje.c_str());
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    std::string Recibir() {
        recv(server, buffer, sizeof(buffer), 0);
        std::string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }

    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        printf("Socket cerrado.\n");
    }
};

int main() {
    printf("Introducir Puerto de conexion\n");
    scanf("%d", &puerto);

    Client* Cliente = new Client();

    system("pause");

    std::string mensaje = "";

    while (true) {
        system("cls");
        printf("**************MENU**************\n\n"
               "1- Generar nombre de usuario\n2- Generar contraseña\n"
               "3- Cerrar Sesión\n");
        int opcion;
        scanf("%d", &opcion);

        if (opcion == 1) {
            printf("\nIntroducir longitud del nombre de usuario:\n");
            char buffer[100];
            scanf("%s", buffer);
            mensaje = buffer;
            Cliente->Enviar("USERNAME:" + mensaje);
        } else if (opcion == 2) {
            printf("\nIntroducir longitud de la contraseña:\n");
            char buffer[100];
            scanf("%s", buffer);
            mensaje = buffer;
            Cliente->Enviar("PASSWORD:" + mensaje);
        } else if (opcion == 3) {
            Cliente->Enviar("Cerrar Sesion");
            Cliente->CerrarSocket();
            break;
        } else {
            printf("Opción no válida. Intenta de nuevo.\n");
            system("pause");
            continue;
        }

        std::string respuesta = Cliente->Recibir();
        if (!respuesta.empty()) {
            printf("Respuesta:\n%s\n\n", respuesta.c_str());
            system("pause");
        } else {
            printf("El mensaje no se envió, se cortó la conexión por inactividad.\n");
            Cliente->CerrarSocket();
            break;
        }
    }

    delete Cliente;
    return 0;
}
