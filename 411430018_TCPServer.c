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
    perror(message); // 顯示系統錯誤訊息
    exit(1);
}

// 處理 client 連線的函式
void handle_client(int client_sock) {
    char buffer[MAX_BUFFER];
    int read_size;

    // 持續讀取 client 傳來的資料
    // read() 會回傳讀取到的位元組數
    // 0: 表示 client 關閉連線 (EOF)
    // <0: 表示發生錯誤
    while ((read_size = read(client_sock, buffer, MAX_BUFFER)) > 0) {
        
        // 伺服器印出收到的訊息 (方便除錯)
        buffer[read_size] = '\0'; // 加上字串結束符
        printf("Server received: %s", buffer);

        // Echo: 將收到的資料原封不動寫回 client
        if (write(client_sock, buffer, read_size) < 0) {
            perror("Server write failed");
            break; // 寫入失敗則跳出迴圈
        }
    }

    if (read_size == 0) {
        printf("Client disconnected.\n");
    } else if (read_size < 0) {
        perror("Server read failed");
    }

    close(client_sock); // 處理完畢，關閉這個 client 的 socket
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock; // Server 和 Client 的 socket file descriptor
    struct sockaddr_in server_addr, client_addr; // 位址結構
    socklen_t client_addr_len;

    // 檢查是否有提供 port 參數
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]); // 將參數轉為整數

    // 1. 建立 socket
    // AF_INET: IPv4
    // SOCK_STREAM: TCP
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        error_handling("socket() failed");
    }

    // (可選) 設置 SO_REUSEADDR 避免 "Address already in use" 錯誤
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. 準備 server 位址結構
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // 監聽所有網路介面
    server_addr.sin_port = htons(port); // Port (轉換為 network byte order)

    // 3. 綁定 (Bind) socket 到指定 port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error_handling("bind() failed");
    }

    // 4. 監聽 (Listen)
    // SOMAXCONN 是系統預設的最大等待隊列長度
    if (listen(server_sock, SOMAXCONN) < 0) {
        error_handling("listen() failed");
    }

    printf("Server listening on port %d...\n", port);

    // 5. 接受 (Accept)
    // 伺服器會在此 block 住，直到有 client 連線
    while (1) {
        client_addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if (client_sock < 0) {
            perror("accept() failed"); // accept 失敗，但伺服器繼續執行
            continue;
        }

        // 取得 client 的 IP 位址並印出
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // 處理這個 client 的請求
        handle_client(client_sock);
    }

    close(server_sock); // (理論上不會執行到，除非 server 停止)
    return 0;
}