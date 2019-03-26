//
// Created by ericjin on 3/26/19.
//

#include <stdio.h>
#define MAX_LEN = 20480;

int main() {
    printf("fdf000");
    return 0;
}

void startConnet() {
    int server_socket_fp;   // 服务端socket句柄
    struct sockaddr_in server_addr;   // 地址结构体
    char buff[MAX_LEN];

    /**
     * 创建socket
     * AF_INET: IPV4网络操作
     * SOCK_STREAM: 使用TCP协议传输数据
     * 0: 自动选择协议
     */
    if ((server_socket_fp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket failed");
        exit(EXIT_FAILURE);
    }

    /**
     * 将socket与地址绑定
     * bind参数：
     *     socket描述符
     *     地址结构体，需要强转成struct sockaddr类型
     *     结构体长度，传入的是指针，无法再函数内部获取长度
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_ANY表示任何地址访问都可以
    server_addr.sin_port = htons(PORT);   // 端口号
    if (bind(server_socket_fp, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("bind socket with addr struct failed!");
        exit(EXIT_FAILURE);
    }

    /**
     * 监听链接
     * 参数：
     *     socket描述符
     *     待连接队列的最大个数，一般为5
     */
    listen(server_socket_fp, MAX_QUEUE);

    /**
     * 与客户端建立连接
     * 参数：
     *     socket文件描述符
     *     客户端地址信息的结构体，不关心可以传NULL
     *     客户端地址长度，不关心可以传NULL
     * 返回值：
     *     socket文件描述符，在与客户端建立连接后，accpet还是会生成一个专门用于和当前客户端通信的socket，
     *     而原来那个socket照常负责和其他等待建立连接的客户端建立通信
     */
    int client_socket_fp = accept(server_socket_fp, NULL, NULL);

    /**
     * 读取浏览器的内容
     * 参数：
     *     文件描述符，从哪个文件读，这里填写accept返回的文件句柄
     *     读的内容存到buff中
     *     共读多少个字节
     */
    read(client_socket_fp, buff, MAX_LEN);
    printf("%s", buff);

    /**
    * 向浏览器写入内容
    * 参数：
    *     文件描述符，往哪个文件写，这里填写accept返回的文件句柄
    *     内容的首地址
    *     共读多少个字节
    */
    char status[] = "HTTP/1.0 200 OK\r\n";
    char header[] = "Server: DWBServer\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
    char body[] = "<html><head><title>C语言构建小型Web服务器</title></head><body><h2>欢迎</h2><p>Hello，World</p></body></html>";

    write(client_socket_fp, status, sizeof(status));
    write(client_socket_fp, header, sizeof(header));
    write(client_socket_fp, body, sizeof(body));

    close(client_socket_fp);
    close(server_socket_fp);
}




