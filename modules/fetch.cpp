#include <duktape.h>
#include <curl/curl.h>
#include <iostream>
#include <string>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *data) {
    data->append((char *)contents, size * nmemb);
    return size * nmemb;
}

static duk_ret_t get(duk_context *ctx) {
    const char *url = duk_require_string(ctx, 0); // Get the URL from arguments

    // Initialize libcurl
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string response;

    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function to receive the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return 0; // Return 0 to indicate failure
        }

        // Cleanup
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return 0; // Return 0 to indicate failure
    }

    // Push the response body onto the Duktape stack
    duk_push_string(ctx, response.c_str());

    return 1; // Return 1 to indicate success
}

extern "C" duk_ret_t dukopen_fetch(duk_context *ctx) {
    duk_push_object(ctx); // Create a new object
    duk_push_c_function(ctx, get, 1); // Push the fetch function onto the stack
    duk_put_prop_string(ctx, -2,"get"); // Assign it to a global variable
    duk_put_global_string(ctx, "fetch"); // push the fetch api onto the stack
    return 0; // Return 0 to indicate success
}
