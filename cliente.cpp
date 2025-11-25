#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#define PORT 8080

using namespace std;

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cerr << "Fallo el socket." << endl;
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        cerr << "Direccion IPv4 invalida o no soportada." << endl;
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cerr << "Fallo en connect." << endl;
        close(sock);
        return 1;
    }

    cout << "Conectado al servidor del parqueadero! \n";

    while (true)
    {
        int spotNum = 0;
        string plate;

        while (true)
        {
            cout << "\nIngrese el numero de plaza (1-40): ";
            if (cin >> spotNum)
            {
                if (spotNum >= 1 && spotNum <= 40)
                {
                    break;
                }
                else
                {
                    cout << "ERROR: Por favor ingrese un numero entre 1 y 40" << endl;
                }
            }
            else
            {
                cout << "ERROR: Entrada no valida. Intente de nuevo. " << endl;
                cin.clear();
                string dummy;
                getline(cin, dummy);
            }
        }

        cout << "Ingrese la placa (ej: ABC123): ";
        cin >> plate;

        string message = to_string(spotNum) + ":" + plate;

        send(sock, message.c_str(), message.length(), 0);
        cout << "-> Enviando: \"" << message << "\"" << endl;

        ssize_t valread = recv(sock, buffer, 1024, 0);
        if (valread > 0)
        {
            if (valread < 1024)
            {
                buffer[valread] = '\0';
            }
            else
            {
                buffer[1023] = '\0';
            }
            cout << "Respuesta del servidor: " << buffer << endl;
        }
        else if (valread == 0)
        {
            cout << "El servidor cerro la conexion." << endl;
            break;
        }
        else
        {
            cerr << "Fallo en recv." << endl;
            break;
        }

        cout << "(Esperando 2 segundos...)" << endl;
        sleep(2);
    }

    close(sock);
    return 0;
}
