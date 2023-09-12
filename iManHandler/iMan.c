#include "iMan.h"

// ALL OF IT IS FROM ChatGPT

#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void fetch_man_page(char * command_name) {

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    struct hostent *server = gethostbyname("man.he.net");
    if (server == NULL) {
        error("Error: Could not resolve hostname");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    // Send an HTTP GET request for the specific URL
    char request[MAX_BUFFER_SIZE];
    snprintf(request, MAX_BUFFER_SIZE, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command_name);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        error("Error sending request");
    }

    // Receive and print the response
    char response[MAX_BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(sockfd, response, MAX_BUFFER_SIZE - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("%s", response);
    }

    if (bytes_received < 0) {
        error("Error receiving response");
    }

    // Close the socket
    close(sockfd);

    // Check if the page does not exist
    if (strstr(response, "404 Not Found") != NULL) {
        fprintf(stderr, "ERROR: No such command\n");
        exit(1);
    }

    return;
}
