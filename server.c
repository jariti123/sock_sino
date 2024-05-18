#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EMPLOYEES 100

// position enum 类型
typedef enum
{
    SOFTWARE_ENGINEER,
    HARDWARE_ENGINEER,
    MANAGER,
    DIRECTOR,
    OTHER
} Position;

// department enum 类型
typedef enum
{
    FENLIU,
    OTHER_PART
} Department;

typedef struct
{
    char name[50];
    int number;
    struct tm date;
    Position position;
    Department department;
} Employee;

Employee employees[MAX_EMPLOYEES];

int employee_count = 0;

// enum 处理
Position string_to_positon(const char *pos_str)
{
    if (strcmp(pos_str, "software_engineer") == 0)
        return SOFTWARE_ENGINEER;
    if (strcmp(pos_str, "hardware_engineer") == 0)
        return HARDWARE_ENGINEER;
    if (strcmp(pos_str, "manager") == 0)
        return MANAGER;
    if (strcmp(pos_str, "director") == 0)
        return DIRECTOR;
    return OTHER;
}

const char *position_to_string(Position pos)
{
    switch (pos)
    {
    case SOFTWARE_ENGINEER:
        return "software_engineer";
    case HARDWARE_ENGINEER:
        return "hardware_engineer";
    case MANAGER:
        return "manager";
    case DIRECTOR:
        return "director";
    default:
        return "other";
    }
}

Department string_to_department(const char *depart_str)
{
    if (strcmp(depart_str, "fenliu") == 0)
        return FENLIU;
    return OTHER_PART;
}

const char *department_to_string(Department depart)
{
    switch (depart)
    {
    case FENLIU:
        return "fenliu";
    default:
        return "other";
    }
}

// 处理日期
struct tm string_to_date(const char *date_str)
{
    struct tm date = {0};
    sscanf(date_str, "%d.%d.%d", &date.tm_year, &date.tm_mon, &date.tm_mday);
    return date;
}
void date_to_string(const struct tm *date, char *buffer, size_t size)
{
    snprintf(buffer, size, "%04d.%02d.%02d", date->tm_year, date->tm_mon, date->tm_mday);
}

// 处理命令
void hand_command(int client_socket)
{
    char buffer[BUFFER_SIZE];
    const char *end_response = "EOC";
    int n;
    while (n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0) > 0)
    {
        printf("Received : %s\n", buffer);
        char command[10];
        sscanf(buffer, "%s", command);
        if (strcmp(command, "ADD") == 0)
        {
            char name_str[50], number_str[10], date_str[11], position_str[20], department_str[20];
            sscanf(buffer, "%*s %s %s %s %s %s", name_str, number_str, date_str, position_str, department_str);
            int is_found = 0;
            for (int i = 0; i < employee_count; i++)
            {
                if (strcmp(employees[i].name, name_str) == 0)
                {
                    employees[i].number = atoi(number_str);
                    employees[i].date = string_to_date(date_str);
                    employees[i].position = string_to_positon(position_str);
                    employees[i].department = string_to_department(department_str);
                    is_found = 1;
                    const char *response = "Employee Modified\n";
                    send(client_socket, response, strlen(response), 0);
                    break;
                }
            }
            if (employee_count < MAX_EMPLOYEES && !is_found)
            {
                Employee emp;
                strcpy(emp.name, name_str);
                emp.number = atoi(number_str);
                emp.date = string_to_date(date_str);
                emp.position = string_to_positon(position_str);
                emp.department = string_to_department(department_str);
                employees[employee_count++] = emp;
                const char *response = "Employee Added\n";
                send(client_socket, response, strlen(response), 0);
            }
            else if(employee_count >= MAX_EMPLOYEES)
            {
                const char *response = "Employee list is full\n";
                send(client_socket, response, strlen(response), 0);
            }
            send(client_socket, end_response, strlen(end_response), 0);
        }
        else if (strcmp(command, "DEL") == 0)
        {
            char name[100];
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
                    is_found = 1;
                    break;
                }
            }
            const char *response = is_found ? "Employee Del \n" : "Employee Not Found \n";
            send(client_socket, response, strlen(response), 0);
            send(client_socket, end_response, strlen(end_response), 0);
        }
        else if (strcmp(command, "SEARCH") == 0)
        {
            char name[100];
            char date_str[11];
            sscanf(buffer, "%*s %s", name);
            int is_found = 0;
            for (int i = 0; i < employee_count; i++)
            {
                if (strcmp(employees[i].name, name) == 0)
                {
                    is_found = 1;
                    char response[BUFFER_SIZE];
                    date_to_string(&employees[i].date, date_str, sizeof(date_str));
                    snprintf(response, sizeof(response), "Name : %s, Number : %d, Date : %s, Position : %s, Department : %s \n",
                             employees[i].name, employees[i].number, date_str, position_to_string(employees[i].position), department_to_string(employees[i].department));
                    send(client_socket, response, strlen(response), 0);
                    //send(client_socket, end_response, strlen(end_response), 0);
                    break;
                }
            }
            if (!is_found)
            {
                const char *response = "Employee Not Found\n";
                send(client_socket, response, strlen(response), 0);
            }
            send(client_socket, end_response, strlen(end_response), 0);
        }
        else if (strcmp(command, "SEARCHALL") == 0)
        {
            char date_str[11];
            char response[BUFFER_SIZE] = "";
            for (int i = 0; i < employee_count; i++)
            {
                date_to_string(&employees[i].date, date_str, sizeof(date_str));
                char emp_str[BUFFER_SIZE];
                snprintf(emp_str, sizeof(emp_str), "Name : %s, Number : %d, Date : %s, Position : %s, Department : %s \n",
                         employees[i].name, employees[i].number, date_str, position_to_string(employees[i].position), department_to_string(employees[i].department));
                if (strlen(response) + strlen(emp_str) < BUFFER_SIZE)
                {
                    strcat(response, emp_str);
                }
                else
                {
                    send(client_socket, response, strlen(response), 0);
                    printf("Response : %s\n", response);
                    bzero(&response, sizeof(response));
                    strcat(response, emp_str);
                }
            }

            printf("%s\n", response);
            send(client_socket, response, strlen(response), 0);
            send(client_socket, end_response, strlen(end_response), 0);
        }
        else if (strcmp(command, "SAVE") == 0)
        {
            FILE *file = fopen("employee.txt", "w");
            char date_str[11];
            for (int i = 0; i < employee_count; i++)
            {
                date_to_string(&employees[i].date, date_str, sizeof(date_str));
                fprintf(file, "Name : %s, Number : %d, Date : %s, Position : %s, Department : %s \n",
                        employees[i].name, employees[i].number, date_str, position_to_string(employees[i].position), position_to_string(employees[i].department));
            }
            fclose(file);
            const char *response = "File Saved\n";
            send(client_socket, response, strlen(response), 0);
            send(client_socket, end_response, strlen(end_response), 0);
        }
        else
        {
            const char *response = "Invalid command\n";
            send(client_socket, response, strlen(response), 0);
            send(client_socket, end_response, strlen(end_response), 0);
        }
        bzero(&buffer, sizeof(buffer));
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
