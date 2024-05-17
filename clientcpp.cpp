#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

void send_command(int socket, const string &command)
{
    char buffer[BUFFER_SIZE] = {0};
    send(socket, command.c_str(), command.size(), 0);
    int n = recv(socket, buffer, BUFFER_SIZE, 0);
    buffer[n] = '\0';
    cout << "Response : " << buffer << endl;
}

int main(int argc, char const *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("create socket failed\n");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Connection failed" << endl;
        return -1;
    }

    if (argc < 3 && string(argv[1]) != "--save")
    {
        cerr << "usage:./clientcpp --command [options]\n"
             << endl;
        cerr << "example:./clientcpp --search --name test1\n\n"
             << endl;
    }

    string command;
    if (string(argv[1]) == "--add" && string(argv[2]) == "--name" && argc == 12)
    {
        command = "ADD " + string(argv[3]) + " " + string(argv[5]) + " " + string(argv[7]) + " " + string(argv[9]) + " " + string(argv[11]);
    }
    else if (string(argv[1]) == "--del" && string(argv[2]) == "--name" && argc == 4)
    {
        command = "DEL " + string(argv[3]);
    }
    else if (string(argv[1]) == "--search" && string(argv[2]) == "--name" && argc == 4)
    {
        command = "SEARCH " + string(argv[3]);
    }
    else if (string(argv[1]) == "--searchall" && argc == 2)
    {
        command = "SEARCHALL";
    }
    else if (string(argv[1]) == "--save" && argc == 2)
    {
        command = "SAVE";
    }
    else
    {
        cerr << "Invalid Command" << endl;
        return 1;
    }
    send_command(client_socket, command);
    close(client_socket);
    return 0;
}