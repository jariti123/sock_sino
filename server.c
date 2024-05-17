#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct
{
    char name[50];
    char number[20];
    char date[20];
    char position[50];
    char department[50];
} Employee;

Employee employees[100];

int employee_count = 0;

// 处理命令
void hand_command(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int n;
    while ((n = read(client_socket, buffer, BUFFER_SIZE)) > 0)
    {
        buffer[n] = '\0';
        printf("Received : %s\n", buffer);
        char command[10], name[50], number[20], date[20], position[50], department[50];
        sscanf(buffer, "%s", command);

        if (strcmp(command, "ADD") == 0)
        {
            // 这里要加一个 比较是否有相同的数据 ， 主键是name
            sscanf(buffer, "%*s %s %s %s %s %s", name, number, date, position, department);
            strcpy(employees[employee_count].name, name);
            strcpy(employees[employee_count].number, number);
            strcpy(employees[employee_count].date, date);
            strcpy(employees[employee_count].position, position);
            strcpy(employees[employee_count].department, department);
            employee_count++;
            write(client_socket, "Employee Add \n", 15);
        }
        else if (strcmp(command, "DEL") == 0)
        {
            sscanf(buffer, "%*s %s", name);
            int is_found = 0;
            for (int i = 0; i < employee_count; i++)
            {
                if (strcmp(employees[i].name, name) == 0)
                {
                    for (int j = i; j < employee_count - 1; j++)
                    {
                        employees[j] = employees[j + 1];
                    }
                    employee_count--;
                    is_found == 1;
                    write(client_socket, "Employee Del \n", 15);
                    break;
                }
            }
            if (!is_found)
            {
                write(client_socket, "Employee Not Found \n", 21);
            }
        }
        else if (strcmp(command, "SEARCH") == 0)
        {
            sscanf(buffer, "%*s %s", name);
            int is_found = 0;
            for (int i = 0; i < employee_count; i++)
            {
                if (strcmp(employees[i].name, name) == 0)
                {
                    snprintf(buffer, BUFFER_SIZE, "Name : %s, Number : %s, Date : %s, Position : %s, Department : %s \n",
                             employees[i].name, employees[i].number, employees[i].date, employees[i].position, employees[i].department);
                    write(client_socket, buffer, BUFFER_SIZE);
                    is_found = 1;
                    break;
                }
            }
            if (!is_found)
            {
                write(client_socket, "Employee Not Found\n", 20);
            }
        }
        else if (strcmp(command, "SAVE") == 0)
        {
            FILE *file = fopen("employee.txt", "w");
            for (int i = 0; i < employee_count; i++)
            {
                fprintf(file, "Name : %s, Number : %s, Date : %s, Position : %s, Department : %s \n",
                        employees[i].name, employees[i].number, employees[i].date, employees[i].position, employees[i].department);
            }
            fclose(file);
            write(client_socket, "File Saved\n", 12);
        }
        else
        {
            write(client_socket, "Invalid Command\n", 17);
        }
    }

    close(client_socket);
}

int main()
{
    int server_socket, client_socket;

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    bzero(&server_socket, sizeof(server_socket));
    bzero(&client_socket, sizeof(client_socket));

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
            perror("accpet failed\n");
            continue;
        }
        // 处理 client 信息
        hand_command(client_socket);
    }

    close(server_socket);
    return 0;
}
