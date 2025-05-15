/**
 * @file web_server_windows.c
 * @brief Minimal HTTP server framework for Windows using Winsock.
 *
 * This file implements a lightweight web server that handles HTTP requests,
 * routes them to appropriate handlers, and sends back responses.
 *
 * @author Alexander Lapajne
 * @date 2025-05-12
 */


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdbool.h>

#include "http_helpers.h"
#include "web_server.h"

#pragma comment(lib, "Ws2_32.lib")  // Link with Winsock library


RouteHandler find_handler(const char* path, Route* routes) {
    /**
     * @brief Find the appropriate route handler for a given path.
     * 
     * @param path The request path.
     * @param routes The array of defined routes.
     * @return The corresponding route handler function, or NULL if not found.
     */
    for (int i = 0; routes[i].path != NULL; ++i) {
        if (strcmp(path, routes[i].path) == 0) {
            return routes[i].handler;
        }
    }
    return NULL;  // No handler found
}


bool initialize_socket(SOCKET *serverSocket) {
    /**
     * @brief Initialize the Winsock library and create a listening socket.
     * 
     * @param serverSocket Pointer to the socket variable to be initialized.
     * @return true if successful, false otherwise.
     */
    WSADATA wsaData;
    struct sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return false;
    }

    *serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (bind(*serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(*serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(*serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(*serverSocket);
        WSACleanup();
        return false;
    }

    printf("Server is listening on http://localhost:8080\n");
    return true;
}


bool run_http_server(SOCKET serverSocket, Route* routes) {
    /**
     * @brief Main loop to accept and handle incoming HTTP requests.
     * 
     * @param serverSocket The listening socket.
     * @param routes The array of defined routes.
     */

    SOCKET clientSocket;
    char requestBuffer[512];  // Buffer to store the HTTP request
    const char* not_found =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

        // Accept and handle connections
    while (1) {
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed. Error: %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected. Sending response...\n");

        int bytesReceived = recv(clientSocket, requestBuffer, sizeof(requestBuffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Receive failed. Error: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            continue;
        }

        requestBuffer[bytesReceived] = '\0';

        // Parse the request (looking for the first line, e.g., GET /path HTTP/1.1)
        char method[8], path[256], protocol[16];
        int parsed = sscanf(requestBuffer, "%7s %255s %15s", method, path, protocol);

        Param* params = NULL;
        int param_count = 0;

        if (parsed == 3) {

            // Check for illegal path traversal      
            if (strstr(path, "..")) {
                printf("Illegal path traversal attempt: %s\n", path);
                send(clientSocket, not_found, (int)strlen(not_found), 0);
                shutdown(clientSocket, SD_SEND);
                closesocket(clientSocket);
                continue;
            }

            if (strcmp(method, "GET") == 0) {
                char* query_start = strchr(path, '?');
                if (query_start) {
                    *query_start = '\0';
                    query_start++;
                    params = parse_params(query_start, &param_count);
                }
            } else if (strcmp(method, "POST") == 0) {
                char* body = strstr(requestBuffer, "\r\n\r\n");
                if (body) {
                    body += 4;
                    params = parse_params(body, &param_count);
                }
            } else {
                printf("Unsupported method: %s\n", method);
            }
            
            RouteHandler handler = find_handler(path, routes);

            if (handler) {
                char* html = handler(params, param_count);
                char* response = create_response(html);
                free(html);
                if (response) {
                    int bytesSent = send(clientSocket, response, (int)strlen(response), 0);
                    free(response);
                    if (bytesSent == SOCKET_ERROR) {
                        printf("Send failed. Error: %d\n", WSAGetLastError());
                    } else {
                        printf("Sent %d bytes.\n", bytesSent);
                    }
                } else {
                    printf("Failed to create response.\n");
                }
            } else {
                printf("Unknown endpoint: %s\n", path);
                send(clientSocket, not_found, (int)strlen(not_found), 0);
            }
        } else {
            printf("Failed to parse the request.\n");
        }

        if (params) free(params);
        
        shutdown(clientSocket, SD_SEND);  // Proper graceful close
        closesocket(clientSocket);
    }
}


int start_server(Route* routes) {
    /**
     * @brief Start the HTTP server and initialize the socket.
     * 
     * @param routes The array of defined routes.
     * @return 0 on success, 1 on failure.
     */
    SOCKET serverSocket;
    if(initialize_socket(&serverSocket)) {
        run_http_server(serverSocket, routes);
        return 0;
    } else {
        printf("Failed to initialize socket.\n");
        return 1;
    }
}
