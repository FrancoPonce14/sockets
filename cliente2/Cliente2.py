import socket

buffer = 1024
puerto = 0

class Cliente2:
    def __init__(self, ip, puerto):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.connect((ip, puerto))
        print("Conectado al Servidor!")

    def enviar(self, mensaje):
        self.server.sendall(mensaje.encode('utf-8'))

    def recibir(self):
        mensaje = self.server.recv(buffer).decode('utf-8')
        return mensaje

    def cerrar_socket(self):
        self.server.close()
        print("Socket cerrado.")

def main():
    global puerto
    puerto = int(input("Introducir Puerto de conexion: "))

    cliente = Cliente2("127.0.0.1", puerto)

    while True:
        print("**************MENU**************")
        print("Elegir función:")
        print("1- Generar nombre de usuario")
        print("2- Generar contraseña")
        print("3- Cerrar Sesión")
        
        try:
            opcion = int(input())
        except ValueError:
            print("Opción no válida. Intenta de nuevo.")
            continue

        if opcion == 1:
            print("\n******Generador de nombres de usuario******\n")
            print("Escribe 'volver' para regresar al menú.")
            mensaje = input("Introducir longitud del nombre de usuario:\n")
            cliente.enviar("USERNAME:" + mensaje)

        elif opcion == 2:
            print("\n******Generador de contraseñas******\n")
            print("Escribe 'volver' para regresar al menú.")
            mensaje = input("Introducir longitud de la contraseña:\n")
            cliente.enviar("PASSWORD:" + mensaje)

        elif opcion == 3:
            cliente.enviar("Cerrar Sesion")
            cliente.cerrar_socket()
            break

        else:
            print("Opción no válida. Intenta de nuevo.")
            continue

        respuesta = cliente.recibir()
        if respuesta:
            print("Respuesta:", respuesta)
        else:
            print("\nEl mensaje no se envió, se cortó la conexión por inactividad.")
            print("Se va a cerrar el programa.")
            cliente.cerrar_socket()
            break

if __name__ == "__main__":
    main()
