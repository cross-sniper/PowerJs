#include <duk_config.h>
#include <duktape.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
#include <unordered_map>

using namespace std;

static char *_read_file(const char *f);

unordered_map<string, string> url_to_file_;
unordered_map<string, function<void(const char *, size_t, int)>> url_to_post_handler_;

static int handle_client_request(int client_socket);
static char *_read_file(const char *f) {
    FILE *file = fopen(f, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        char *contents = (char *)malloc(size + 1);
        if (contents) {
            fread(contents, 1, size, file);
            contents[size] = '\0';
            fclose(file);
            return contents;
        }
        fclose(file);
    }
    return nullptr;  // File not found or error occurred
}

static int cwrite(duk_context *ctx) {
    const char *response = duk_get_string(ctx, 0);
    int client_socket = duk_get_number(ctx, 1);

    write(client_socket, response, strlen(response));

    return 0;
}

static duk_ret_t bind(duk_context *ctx) {
    const char *url = duk_get_string(ctx, 0);
    const char *file_to_show = duk_get_string(ctx, 1);
    url_to_file_[url] = file_to_show;
    return 0;
}

static duk_ret_t run(duk_context *ctx) {
    int port = duk_get_number(ctx, 0);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        perror("Error setting socket option");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        handle_client_request(client_socket);
        close(client_socket);
    }

    return 0;
}

static int handle_client_request(int client_socket) {
    const char *response_template = "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s";
    const char *not_found_response = "HTTP/1.1 404 Not Found\r\n\r\nFile not found.";

    char buffer[1024];
    ssize_t bytesRead = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';


        const char *url_start = strstr(buffer, "GET ");
        if (url_start) {
            const char *url_end = strchr(url_start + 4, ' ');
            if (url_end) {
                string requested_url(url_start + 4, url_end);
                auto it = url_to_file_.find(requested_url);

                if (it != url_to_file_.end()) {
                    char *file_contents = _read_file(it->second.c_str());
                    if (file_contents) {
                        char response[1024];
                        snprintf(response, sizeof(response), response_template, strlen(file_contents), file_contents);
                        write(client_socket, response, strlen(response));
                        free(file_contents);
                    } else {
                        write(client_socket, not_found_response, strlen(not_found_response));
                    }
                } else {
                    write(client_socket, not_found_response, strlen(not_found_response));
                }
            }
        } else {

            const char *url_start = strstr(buffer, "POST ");
            if (url_start) {
                const char *url_end = strchr(url_start + 5, ' ');
                if (url_end) {
                    string requested_url(url_start + 5, url_end);
                    auto it = url_to_post_handler_.find(requested_url);
                    // Replace the problematic section in handle_client_request function
                    if (it != url_to_post_handler_.end()) {
                        // Extract POST data from the request
                        const char *data_start = strstr(buffer, "\r\n\r\n");
                        if (data_start) {
                            data_start += 4;  // Move past "\r\n\r\n"
                            size_t data_size = bytesRead - (data_start - buffer);

                            // Call the registered POST handler
                            it->second(data_start, data_size, client_socket);
                        }
                    } else {
                        // Handle case where no POST handler is registered for the URL
                        const char *not_found_response = "HTTP/1.1 404 Not Found\r\n\r\nNo POST handler registered.";
                        write(client_socket, not_found_response, strlen(not_found_response));
                    }

                }
            }
        }
        close(client_socket);
    }

    return 0;
}

static duk_ret_t cbind(duk_context *ctx) {
	/*
    if (!luna_istable(L, 1)) {
        lunaL_error(L, "Argument must be a table");
        return 0;
    }

    luna_pushnil(L);

    while (luna_next(L, 1) != 0) {
        const char *url = lunaL_checkstring(L, -2);
        const char *file = lunaL_checkstring(L, -1);

        url_to_file_[url] = file;

        luna_pop(L, 1);
    }
	*/
    if(!duk_is_object(ctx, 0)){
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "Argument must be a table\ntype: %s",duk_get_type(ctx, 0));
        return 0;
    }
    duk_push_null(ctx);
    while(duk_next(ctx, 1, 0) != 0){
        const char *url = duk_get_string(ctx, -2);
        const char *file = duk_get_string(ctx, -1);
        url_to_file_[url] = file;
        duk_pop(ctx);
    }
    return 0;
}

static duk_ret_t crun(duk_context *L) {
    int port = duk_get_number(L, 0);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        perror("Error setting socket option");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        if (handle_client_request(client_socket) != 0) {
            close(server_socket);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        close(client_socket);
    }

    return 0;
}

static duk_ret_t frun(duk_context *ctx) {
    // Get the JavaScript function from the stack
    duk_require_function(ctx, 0);
    
    // Get the port number from the stack
    int port = duk_require_int(ctx, 1);

    // Create a server socket and set it up
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        perror("Error setting socket option");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }
        
        // Read HTTP request to get the path
        char buffer[1024];
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0) {
            perror("Error reading from client");
            close(client_socket);
            continue;
        }
        
        // Extract the path from the HTTP request
        char *path_start = strstr(buffer, "GET ");
        if (path_start == nullptr) {
            perror("Error parsing HTTP request");
            close(client_socket);
            continue;
        }
        path_start += 4; // Move past "GET "
        char *path_end = strchr(path_start, ' ');
        if (path_end == nullptr) {
            perror("Error parsing HTTP request");
            close(client_socket);
            continue;
        }
        *path_end = '\0'; // Null-terminate the path
        const char *path = path_start;

        // Call the user-defined JavaScript function
        duk_dup(ctx, 0); // Duplicate the function
        duk_push_int(ctx, client_socket); // Push the client socket to the stack
        duk_push_string(ctx, path); // Push the path to the stack
        duk_call(ctx, 2); // Call the function with 2 arguments (client_socket and path)
        duk_pop(ctx); // Pop the function result

        close(client_socket);
    }

    return 0;
}

extern "C" void dukopen_server(duk_context *L) {
    duk_push_object(L);

    duk_push_c_function(L, bind,2);
    duk_put_prop_string(L, -2, "bind");

    duk_push_c_function(L, cbind,0);
    duk_put_prop_string(L, -2, "cbind");

    duk_push_c_function(L, frun,2);
    duk_put_prop_string(L, -2, "frun");

    duk_push_c_function(L, run,1);
    duk_put_prop_string(L, -2, "run");

    duk_push_c_function(L, crun,1);
    duk_put_prop_string(L, -2, "crun");

    // Add the cwrite function
    duk_push_c_function(L, cwrite,2);
    duk_put_prop_string(L, -2, "cwrite");
    duk_put_global_string(L, "server");

}
