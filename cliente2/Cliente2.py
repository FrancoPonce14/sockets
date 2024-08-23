import socket

# Configuración del cliente
puerto = 0
buffer_size = 1024

class Client:
    def __init__(self, ip, puerto):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.connect((ip, puerto))
        print("Conectado al Servidor!")

    def enviar(self, mensaje):
        self.server.sendall(mensaje.encode('utf-8'))

    def recibir(self):
        mensaje = self.server.recv(buffer_size).decode('utf-8')
        return mensaje

    def cerrar_socket(self):
        self.server.close()
        print("Socket cerrado.")

def verificar_username(mensaje):
    if mensaje.lower() == "volver":
        return 2
    try:
        longitud = int(mensaje)
        if longitud > 15 or longitud < 5:
            return 1
    except ValueError:
        pass
    return 0

def verificar_password(mensaje):
    if mensaje.lower() == "volver":
        return 2
    try:
        longitud = int(mensaje)
        if longitud >= 50 or longitud < 8:
            return 1
    except ValueError:
        pass
    return 0

def main():
    global puerto
    puerto = int(input("Introducir Puerto de conexion: "))

    cliente = Client("127.0.0.1", puerto)

    loop = True
    menu = 0

    while loop:
        if menu == 1:
            print("\n******Generador de nombres de usuario******\n")
            print("Escribi \"volver\" para regresar al menu.")
            mensaje = input("Introducir longitud del nombre de usuario:\n")

            verificacion = verificar_username(mensaje)

            if verificacion == 1:
                print("ERROR la cantidad de caracteres no está entre los 5 y 15 permitidos")
            elif verificacion == 2:
                menu = 0
            else:
                cliente.enviar("USERNAME:" + mensaje)
                respuesta = cliente.recibir()
                if respuesta:
                    print("Respuesta:", respuesta)
                else:
                    print("\nEl mensaje no se envió, se cortó la conexión por inactividad.")
                    print("Se va a cerrar el programa.")
                    cliente.cerrar_socket()
                    return
        elif menu == 2:
            print("\n******Generador de contraseñas******\n")
            print("Escribi \"volver\" para regresar al menu.")
            mensaje = input("Introducir longitud de la contraseña:\n")

            verificacion = verificar_password(mensaje)

            if verificacion == 1:
                print("ERROR la cantidad de caracteres no está entre los 8 y 50 permitidos")
            elif verificacion == 2:
                menu = 0
            else:
                cliente.enviar("PASSWORD:" + mensaje)
                respuesta = cliente.recibir()
                if respuesta:
                    print("Respuesta:", respuesta)
                else:
                    print("\nEl mensaje no se envió, se cortó la conexión por inactividad.")
                    print("Se va a cerrar el programa.")
                    cliente.cerrar_socket()
                    return
        elif menu == 3:
            cliente.enviar("Cerrar Sesion")
            cliente.cerrar_socket()
            loop = False
        else:
            print("**************MENU**************")
            print("Elegir función:")
            print("1- Generar nombre de usuario")
            print("2- Generar contraseña")
            print("3- Cerrar Sesión")
            menu = int(input())

if __name__ == "__main__":
    main()
