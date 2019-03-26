//
// Created by ericjin on 3/26/19.
//

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <zconf.h>
#include "../utils/map.h"

#define PORT 8080        // 服务器监听端口
#define MAX_QUEUE 5     // 待连接的队列最大数
#define MAX_LEN 1024    // 接收浏览器数据

p_map static analysisRequest(char *, p_map);

p_map static addNode(p_map, char *);

/**
 * 开始启动socket
 */
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
    // 分析请求信息
    p_map request = (p_map) malloc(sizeof(map));
    request = analysisRequest(buff, request);
    while (request != NULL) {
        printf("%s: %s\n", request->key, request->value);
        request = request->next;
    }

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

/**
 * 获取返回的参数，包括请求方式以及请求的路径等等
 * @param str
 * @param request
 * @return
 */
p_map static analysisRequest(char *str, p_map request) {
    char *p;
    // 获取每一行的数据
    p = strtok(str, "\r\n"); // 获取第一行的数据，提取出请求方式跟请求路径
    // 对第一行的数据进行分割
    char *item = strtok(p, " ");   // 按照空格进行分隔
    request = addItem(request, "method", item);  // 将请求方式保存
    item = strtok(NULL, " ");    // 再次分隔，分割出路径
    request = addItem(request, "path", item);
    // 对post和get方式进行分别解析
    p_map dict = NULL;
    p_map method = findItem(request, "method");
    char *r;
    // get方式
    if (strcmp("GET", method->value) == 0) {
        // 解析url
        char *buff = findItem(request, "path")->value;
        r = strsep(&buff, "?");
        findItem(request, "path")->value = r;
        char *arg = strsep(&buff, "?"); // 获取到url后面的参数

        char *argItem = strsep(&arg, "&");  // 拿到url后面的参数
        while (argItem != NULL) {
            // 对name=eric解析
            char *k = strsep(&argItem, "=");
            char *v = strsep(&argItem, "=");
            request = addItem(request, k, v);
            argItem = strsep(&arg, "&");  // r = 'name=eric'
        }
    }

    return request;
}




