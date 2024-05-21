#include "../modular.hpp"
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

duk_func fns[] = {
    {"get",get,"gets the content from a given url"},
    {NULL,NULL,NULL}
};

extern "C" duk_ret_t dukopen_fetch(duk_context *ctx) {
    registerModule(ctx, fns);
    setGlobalModule(ctx, "fetch");
    return 0; // Return 0 to indicate success
}
