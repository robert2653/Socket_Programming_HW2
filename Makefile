# Makefile
# 使用 gcc 或 clang 編譯器 (作業要求) 
CC = gcc
# CC = clang

# 編譯器旗標:
# -Wall: 顯示所有警告
# -g: 產生除錯資訊
CFLAGS = -Wall -g

# 目標檔案名稱
TARGET_SERVER = 411430018_TCPServer
TARGET_CLIENT = 411430018_TCPClient

# 預設目標 (輸入 make 時執行的)
all: $(TARGET_SERVER) $(TARGET_CLIENT)

# 編譯 Server
$(TARGET_SERVER): $(TARGET_SERVER).c
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(TARGET_SERVER).c

# 編譯 Client
$(TARGET_CLIENT): $(TARGET_CLIENT).c
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(TARGET_CLIENT).c

# 清除編譯產生的執行檔
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)