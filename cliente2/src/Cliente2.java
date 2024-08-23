import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class Cliente2 {
    private Socket socket;
    private BufferedReader input;
    private PrintWriter output;
    private static int puerto;

    public Cliente2() {
        try {
            System.out.println("Conectando al servidor...");
            socket = new Socket("127.0.0.1", puerto);
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()), true);
            System.out.println("Conectado al Servidor!");
        } catch (Exception e) {
            System.out.println("No se pudo conectar al servidor.");
            e.printStackTrace();
            System.exit(0);
        }
    }

    public void enviar(String mensaje) {
        output.println(mensaje);
    }

    public String recibir() {
        try {
            return input.readLine();
        } catch (Exception e) {
            System.out.println("Error al recibir el mensaje.");
            e.printStackTrace();
            return "";
        }
    }

    public void cerrarSocket() {
        try {
            socket.close();
            System.out.println("Socket cerrado.");
        } catch (Exception e) {
            System.out.println("Error al cerrar el socket.");
            e.printStackTrace();
        }
    }

    public static int verificarUsername(String mensaje) {
        int resultado = 0;
        if (mensaje.equalsIgnoreCase("volver")) {
            resultado = 2;
        } else if (Integer.parseInt(mensaje) > 15 || Integer.parseInt(mensaje) < 5) {
            resultado = 1;
        }
        return resultado;
    }

    public static int verificarPassword(String mensaje) {
        int resultado = 0;
        if (mensaje.equalsIgnoreCase("volver")) {
            resultado = 2;
        } else if (Integer.parseInt(mensaje) >= 50 || Integer.parseInt(mensaje) < 8) {
            resultado = 1;
        }
        return resultado;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("Introducir Puerto de conexion");
        puerto = scanner.nextInt();

        Cliente2 cliente = new Cliente2();

        boolean loop = true;
        int menu = 0;
        String mensaje;
        int verificacion;

        while (loop) {
            switch (menu) {
                case 1:
                    System.out.println("\n******Generador de nombres de usuario******\n\n" +
                            "Escribi \"volver\" para regresar al menu.\nIntroducir longitud del nombre de usuario:");
                    mensaje = scanner.next();

                    verificacion = verificarUsername(mensaje);

                    switch (verificacion) {
                        case 1:
                            System.out.println("ERROR la cantidad de caracteres no esta entre los 5 y 15 permitidos");
                            break;

                        case 2:
                            menu = 0;
                            break;

                        default:
                            cliente.enviar("USERNAME:" + mensaje);
                            mensaje = cliente.recibir();
                            if (mensaje != null && !mensaje.isEmpty()) {
                                System.out.println("Respuesta:\n" + mensaje + "\n");
                            } else {
                                System.out.println("\nEl mensaje no se envio, se corto la conexion por inactividad\n" +
                                        "Se va a cerrar el programa\n\n");
                                cliente.cerrarSocket();
                                System.exit(0);
                            }
                            break;
                    }
                    break;

                case 2:
                    System.out.println("\n******Generador de contraseñas******\n\n" +
                            "Escribi \"volver\" para regresar al menu.\nIntroducir longitud de la contraseña:");
                    mensaje = scanner.next();

                    verificacion = verificarPassword(mensaje);

                    switch (verificacion) {
                        case 1:
                            System.out.println("ERROR la cantidad de caracteres no esta entre los 8 y 50 permitidos");
                            break;

                        case 2:
                            menu = 0;
                            break;

                        default:
                            cliente.enviar("PASSWORD:" + mensaje);
                            mensaje = cliente.recibir();
                            if (mensaje != null && !mensaje.isEmpty()) {
                                System.out.println("Respuesta:\n" + mensaje + "\n");
                            } else {
                                System.out.println("\nEl mensaje no se envio, se corto la conexion por inactividad\n" +
                                        "Se va a cerrar el programa\n\n");
                                cliente.cerrarSocket();
                                System.exit(0);
                            }
                            break;
                    }
                    break;

                case 3:
                    cliente.enviar("Cerrar Sesion");
                    cliente.cerrarSocket();
                    loop = false;
                    break;

                default:
                    System.out.println("**************MENU**************\n\n Elegir funcion\n" +
                            " 1- Generar nombre de usuario\n 2- Generar contraseña\n" +
                            " 3- Cerrar Sesion");
                    menu = scanner.nextInt();
                    break;
            }
        }

        scanner.close();
    }
}
