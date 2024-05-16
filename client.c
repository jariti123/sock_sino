#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_command(int socket, const char *command)
{
    char buffer[BUFFER_SIZE];
    write(socket, command, strlen(command));
    int n = read(socket, buffer, BUFFER_SIZE - 1);
    buffer[n] = '\0';
    printf("Response : %s\n", buffer);
}

int main(int argc, char *argv[])
{
    if (argc < 3 && strcmp(argv[1], "--save") != 0 && strcmp(argv[1], "--searchall") != 0)
    {
        printf("usage:./client --command [options]\n");
        printf("example:./client --search --name test1\n\n");
    }
    // printf("argc %d\n, argv[1] %d, argv[2] %d", argc, strcmp(argv[1], "--add"),strcmp(argv[2], "--name"));
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("create sock faild\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed \n");
        close(client_socket);
        return -1;
    }

    char command[BUFFER_SIZE] = {0};

    /*
    # 添加员工信息命令：
    client --add --name test1 --number 1234 --date 2000.2.2 --position software_engineer --department fenliu
    # 删除员工信息命令：
    client --del --name test1
    # 查找员工信息命令
    client --search --name test1
    # 保存所有员工信息到文件
    client --save
    */

    if (strcmp(argv[1], "--add") == 0 && strcmp(argv[2], "--name") == 0 && argc == 12)
    {
        // 执行 添加
        snprintf(command, BUFFER_SIZE, "ADD %s %s %s %s %s", argv[3], argv[5], argv[7], argv[9], argv[11]);
    }
    else if (strcmp(argv[1], "--del") == 0 && strcmp(argv[2], "--name") == 0 && argc == 4)
    {
        // 执行 删除
        snprintf(command, BUFFER_SIZE, "DEL %s", argv[3]);
    }
    else if (strcmp(argv[1], "--search") == 0 && strcmp(argv[2], "--name") == 0 && argc == 4)
    {
        // 执行 查询
        snprintf(command, BUFFER_SIZE, "SEARCH %s", argv[3]);
    }
    else if (strcmp(argv[1], "--searchall") == 0 && argc == 2)
    {
        // 执行 查询
        snprintf(command, BUFFER_SIZE, "SEARCHALL");
    }
    else if (strcmp(argv[1], "--save") == 0 && argc == 2)
    {
        // 执行存储
        snprintf(command, BUFFER_SIZE, "SAVE");
    }
    else
    {
        printf("Invalid Command \n");
        close(client_socket);
        return -1;
    }
    send_command(client_socket, command);
    close(client_socket);
    return 0;
}