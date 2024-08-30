#include <winsock2.h>
#include <string>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define PUERTO 5000

std::string vocales = "AEIOUaeiou";
std::string consonantes = "BCDFGHJKLMNPQRSTVWXYZbcdfghjklmnpqrstvwxyz";
std::string numeros = "0123456789";
bool esperando = false;
bool conexion = true;

class Server {
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];

    Server() {
        WSAStartup(MAKEWORD(2, 2), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PUERTO);

        bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        printf("Escuchando para conexiones entrantes en el puerto: %d\n", PUERTO);
    }

    bool AceptarCliente() {
        int clientAddrSize = sizeof(clientAddr);
        client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (client != INVALID_SOCKET) {
            printf("Cliente conectado!\n");
            return true;
        }
        return false;
    }

    void EsperarCliente() {
        printf("Esperando cliente...\n");
        int clientAddrSize = sizeof(clientAddr);
        client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (client != INVALID_SOCKET) {
            printf("Cliente conectado!\n");
        }
    }

    std::string Recibir() {
        recv(client, buffer, sizeof(buffer), 0);
        std::string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
    }

    void Enviar(const std::string& mensaje) {
        strcpy(buffer, mensaje.c_str());
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    void CerrarSocketCliente() {
        closesocket(client);
        printf("Socket cerrado, cliente desconectado.\n");
    }

    void CerrarSocketServidor() {
        closesocket(server);
        WSACleanup();
        printf("Servidor cerrado.\n");
    }
};

void TimeOut(Server* servidor) {
    int tiempo = 0;
    while (esperando) {
        Sleep(1000);
        tiempo++;
        if (tiempo >= 120) {
            printf("\nTimeout, se va a cerrar la conexión con el cliente\n");
            servidor->CerrarSocketCliente();
            conexion = false;
            break;
        }
    }
}

char LetraInicialAleatoria() {
    std::string todo = vocales + consonantes;
    srand(static_cast<unsigned>(time(0)));
    int i = rand() % todo.length();
    return todo[i];
}

std::string GenerarUsername(int longitud) {
    std::string resultado = "";
    char letraInicial = LetraInicialAleatoria();
    resultado = letraInicial;

    if (vocales.find(letraInicial) != std::string::npos) {
        for (int i = 1; i < longitud; i++) {
            if (i % 2 == 1) {
                int consonanteAleatoria = rand() % consonantes.length();
                resultado += consonantes[consonanteAleatoria];
            } else {
                int vocalAleatoria = rand() % vocales.length();
                resultado += vocales[vocalAleatoria];
            }
        }
    } else if (consonantes.find(letraInicial) != std::string::npos) {
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

std::string GenerarPassword(int longitud) {
    std::string alfanumericos = numeros + consonantes + vocales;
    std::string resultado = "";
    for(int i = 0; i < longitud; i++){
        int alfanumericoAleatorio = rand() % alfanumericos.length();
        resultado += alfanumericos[alfanumericoAleatorio];
    }
    return resultado;
}

std::string ResponderCadena(const std::string& mensaje) {
    std::string resultado = "";

    if (mensaje == "volver") {
        resultado = "Volviendo al menú...";
    } else if (mensaje.find("USERNAME:") == 0) {
        int longitud = std::stoi(mensaje.substr(9));
        if (longitud < 5 || longitud > 15) {
            resultado = "ERROR: La longitud del nombre de usuario debe estar entre 5 y 15 caracteres.";
        } else {
            resultado = GenerarUsername(longitud);
        }
    } else if (mensaje.find("PASSWORD:") == 0) {
        int longitud = std::stoi(mensaje.substr(9));
        if (longitud < 8 || longitud > 50) {
            resultado = "ERROR: La longitud de la contraseña debe estar entre 8 y 50 caracteres.";
        } else {
            resultado = GenerarPassword(longitud);
        }
    } else if (mensaje == "Cerrar Sesion") {
        resultado = "Conexion cerrada";
        conexion = false;
    } else {
        resultado = "Comando no reconocido";
    }

    return resultado;
}

int main() {
    Server* Servidor = new Server();

    while (true) {
        Servidor->EsperarCliente();

        while (conexion) {
            printf(": Esperando recibir mensaje\n");

            std::thread timeOut(TimeOut, Servidor);
            esperando = true;
            std::string mensaje = Servidor->Recibir();
            esperando = false;
            timeOut.join();

            if (mensaje.empty()) {
                printf("Error al recibir mensaje, cerrando la conexión.\n");
                conexion = false;
                break;
            }

            printf(": Mensaje recibido: %s\n", mensaje.c_str());

            std::string respuesta = ResponderCadena(mensaje);

            printf(": Respuesta enviada: %s\n", respuesta.c_str());

            Servidor->Enviar(respuesta);

            if (!conexion) {
                Servidor->CerrarSocketCliente();
                break;
            }
        }
    }

    Servidor->CerrarSocketServidor();
    delete Servidor;

    return 0;
}
