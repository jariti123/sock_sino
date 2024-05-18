#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void print_usage()
{
    printf("Usage:\n");
    printf("  client --add --name <name> --number <number> --date <YYYY.MM.DD> --position <position> --department <department>\n");
    printf("  client --del --name <name>\n");
    printf("  client --search --name <name>\n");
    printf("  client --search --name all\n");
    printf("  client --save\n");
    printf("\n");
    printf("Position options:\n");
    printf("  software_engineer\n");
    printf("  hardware_engineer\n");
    printf("  manager\n");
    printf("  director\n");
    printf("  other\n");
    printf("\n");
    printf("Department options:\n");
    printf("  fenliu\n");
    printf("  other\n");
}

void send_command(int socket, const char *command)
{
    char buffer[BUFFER_SIZE];
    write(socket, command, strlen(command));
    int total_read = 0;
    printf("Response : \n");
    while (1)
    {
        int n = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            break;
        }
        buffer[n] = '\0';
        printf("%s\n", buffer);
        if (strstr(buffer, "EOC"))
        {
            break;
        }
    }
}

void receive_response(int socket)
{
    char buffer[BUFFER_SIZE];
    char combined_buffer[2 * BUFFER_SIZE] = {0}; // 组合数据用
    int n;
    int combined_len = 0;
    while ((n = recv(socket, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[n] = '\0';
        if (combined_len + n >= sizeof(combined_buffer))
        {
            // 保留最后一部分，以防止结束标志被截断 EOC 算 4
            int preserve_len = 4;
            combined_buffer[combined_len - preserve_len] = '\0';
            printf("%s", combined_buffer);

            memmove(combined_buffer, combined_buffer + combined_len - preserve_len, preserve_len);
            combined_len = preserve_len;
            combined_buffer[preserve_len] = '\0'; // 确保字符串终止
        }

        strncat(combined_buffer, buffer, n); // 将接收到的数据追加到组合缓冲区
        combined_len += n;
        // 检查是否包含结束标志
        if (strstr(combined_buffer, "EOC") != NULL)
        {
            combined_buffer[combined_len] = '\0';
            printf("%s", combined_buffer);
            break;
        }
    }
    // 打印最后剩余的缓冲区内容
    printf("%s", combined_buffer);
}

int main(int argc, char *argv[])
{
    if (argc < 3 && strcmp(argv[1], "--save") != 0)
    {
        print_usage();
        return -1;
    }
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
    # 查找所有员工信息
    client --search --name
    # 保存所有员工信息到文件
    client --save
    */
    /**
     这里再加点提示
    */
    if (strcmp(argv[1], "--add") == 0 && argc < 12)
    {
        // 添加 的相关提示
        printf("Usage : client --add --name <name> --number <number> --date <YYYY.MM.DD> --position <position> --department <department>\n");
        close(client_socket);
        return -1;
    }

    if ((strcmp(argv[1], "--delete") == 0 || strcmp(argv[1], "--del")) == 0 && argc < 4)
    {
        // 添加 的相关提示
        printf("Usage : client --del --name <name>\n");
        close(client_socket);
        return -1;
    }

    if (strcmp(argv[1], "--search") == 0 && argc < 4)
    {
        // 添加 的相关提示
        printf("Usage : client --search --name <name>\n");
        printf("Usage : client --search --name all\n");
        close(client_socket);
        return -1;
    }

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
    else if (strcmp(argv[1], "--search") == 0 && strcmp(argv[2], "--name") == 0 && strcmp(argv[3], "all") == 0 && argc == 4)
    {
        // 执行查询所有
        snprintf(command, BUFFER_SIZE, "SEARCHALL");
    }
    else if (strcmp(argv[1], "--search") == 0 && strcmp(argv[2], "--name") == 0 && argc == 4)
    {
        // 执行 查询
        snprintf(command, BUFFER_SIZE, "SEARCH %s", argv[3]);
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
    // 封装命令后发送
    send_command(client_socket, command);
    close(client_socket);
    return 0;
}