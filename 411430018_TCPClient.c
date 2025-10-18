#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024 // 緩衝區大小

// 錯誤處理函式
void error_handling(const char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    int read_size;

    // 檢查參數: 程式名稱、Server IP、Server Port
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    // 1. 建立 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error_handling("socket() failed");
    }

    // 2. 準備 server 位址結構
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // 將 IP 字串轉換為二進位格式
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        error_handling("Invalid address / Address not supported");
    }

    // 3. 連線 (Connect)
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error_handling("connect() failed");
    }

    printf("Connected to server %s:%d\n", server_ip, server_port);
    printf("Enter message (Ctrl+D to exit):\n");

    // 4. 讀取使用者輸入 (stdin) 並傳送
    // fgets 會讀取一行，包含換行符 '\n'
    while (fgets(buffer, MAX_BUFFER, stdin) != NULL) {
        
        // 傳送資料給 server
        if (write(sock, buffer, strlen(buffer)) < 0) {
            perror("Client write failed");
            break;
        }

        // 接收 server 的 echo 回應
        read_size = read(sock, buffer, MAX_BUFFER);
        
        if (read_size < 0) {
            perror("Client read failed");
            break;
        } else if (read_size == 0) {
            printf("Server closed the connection.\n");
            break;
        }

        // 印出 server 的回應
        buffer[read_size] = '\0'; // 加上字串結束符
        printf("Server echo: %s", buffer);

        printf("Enter message (Ctrl+D to exit):\n");
    }

    // 5. 關閉 socket
    close(sock);
    printf("Connection closed.\n");
    return 0;
}