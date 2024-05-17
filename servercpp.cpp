#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

class Employee
{
public:
    string name;
    string number;
    string date;
    string position;
    string department;

    Employee(const string &name, const string &num, const string &date, const string &position, const string &department)
        : name(name), number(num), date(date), position(position), department(department) {}
};

vector<Employee> employees;

void hand_command(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int n;
    while (n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0) > 0)
    {
        cout << "Received : " << buffer << endl;

        istringstream iss(buffer);
        string command;
        iss >> command;
        if (command == "ADD")
        {
            string name, number, date, position, department;
            iss >> name >> number >> date >> position >> department;
            employees.emplace_back(name, number, date, position, department);
            string response = "Employee Added\n";
        }
        else if (command == "DEL")
        {
            string name;
            iss >> name;
            int is_found = 0;
            for (int i = 0; i < employees.size(); ++i)
            {
                if (employees[i].name == name)
                {
                    employees.erase(employees.begin() + i);
                    --i;
                    is_found = 1;
                    break;
                }
            }
            if (!is_found)
            {
                string response = "Employee Not Found \n";
                send(client_socket, response.c_str(), response.size(), 0);
            }
            else
            {
                string response = "Employee Del \n";
                send(client_socket, response.c_str(), response.size(), 0);
            }
        }
        else if (command == "SEARCH")
        {   
            string name;
            iss >> name;
            int is_found = 0;
            for (auto &emp : employees)
            {
                if (emp.name == name)
                {
                    ostringstream oss;
                    oss << "Name: " << emp.name << ", Number: " << emp.number << ", Date: " << emp.date
                        << ", Position: " << emp.position << ", Department: " << emp.department << "\n";
                    string response = oss.str();
                    write(client_socket, response.c_str(), response.size());
                    is_found = 1;
                }
            }
            if (!is_found)
            {
                string response = "Employee Not Found \n";
                send(client_socket, response.c_str(), response.size(), 0);
            }
        }
        else if (command == "SAVE")
        {
            ofstream ofs("employees.txt");
            for (auto &emp : employees)
            {
                ofs << "Name: " << emp.name << ", Number: " << emp.number << ", Date: " << emp.date
                    << ", Position: " << emp.position << ", Department: " << emp.department << "\n";
            }
            string response = "Employees saved\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
        else
        {
            string response = "Invalid Command\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }

    close(client_socket);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("create socket failed\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed\n");
        close(server_socket);
        return -1;
    }

    printf("listening on 8080...\n");
    listen(server_socket, 128);

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0)
        {
            perror("Accept Failed");
            continue;
        }
        hand_command(client_socket);
    }
    close(client_socket);
    return 0;
}