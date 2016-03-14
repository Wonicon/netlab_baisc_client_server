/**
 * @file     config.c
 * @author   whz
 * @brief    这个文件存放客户端使用到的相关的常量
 */

/**
 * @brief 服务器的 IP
 *
 * 这个 IP 地址可以使用通过抓包或者 strace 命令观察到
 */
const char *SERVER_IP = "114.212.191.33";

/**
 * @brief 服务器的端口号
 *
 * 这个端口号同 #SERVER_IP 一样，可以通过抓包或者 strace 命令观察到
 */
const unsigned short SERVER_PORT = 4321;

/**
 * @brief socket() 调用失败时配合 perror() 使用
 */
const char *MSG_SOCKET_FAILURE = "Failed to create socket for client";

/**
 * @brief connect() 调用失败时配合 perror() 使用
 */
const char *MSG_CONNECT_FAILURE = "Failed to connect to server";