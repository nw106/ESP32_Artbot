//original http_post
/*
#include <esp_log.h>
#include <esp_http_server.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static const char *REST_TAG = "http_post";
static const char *NVS_NAMESPACE = "storage";  
static const char *NVS_KEY = "file_number";    
#define BUFFER_SIZE 1024  // Buffer size for file reading

// Function to extract file extension from filename
static void get_file_extension(const char *filename, char *ext, size_t ext_size) {
    const char *dot = strrchr(filename, '.'); 
    if (dot && dot != filename) {
        strncpy(ext, dot, ext_size - 1);
        ext[ext_size - 1] = '\0';
    } else {
        strcpy(ext, ".bmp");  // Default extension if none found
    }
}

// Function to read the last stored file number from NVS
static int get_next_file_number() {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int32_t file_number = 1;  // Default starting value

    // Open NVS storage
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(REST_TAG, "Failed to open NVS storage!");
        return file_number;
    }

    // Read the stored file number
    err = nvs_get_i32(nvs_handle, NVS_KEY, &file_number);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(REST_TAG, "No file number found, starting at 1");
    } else if (err != ESP_OK) {
        ESP_LOGE(REST_TAG, "Error reading file number from NVS!");
        nvs_close(nvs_handle);
        return file_number;
    }

    // Increment the file number for the next file
    file_number++;

    // Store the updated file number in NVS
    err = nvs_set_i32(nvs_handle, NVS_KEY, file_number);
    if (err != ESP_OK) {
        ESP_LOGE(REST_TAG, "Failed to update file number in NVS!");
    }

    // Commit the changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(REST_TAG, "Failed to commit changes to NVS!");
    }

    // Close NVS
    nvs_close(nvs_handle);

    return file_number;
}

// File Upload Handler
static esp_err_t upload_handler(httpd_req_t *req)
{
    char buf[1024];
    int received;
    
    char original_filename[64] = {0};
    char file_extension[16] = {0};

    // Extract filename from request headers
    if (httpd_req_get_hdr_value_str(req, "Content-Disposition", original_filename, sizeof(original_filename)) == ESP_OK) {
        char *filename_start = strstr(original_filename, "filename=");
        if (filename_start) {
            filename_start += 9; // Skip "filename="
            char *quote_start = strchr(filename_start, '"');
            if (quote_start) {
                filename_start = quote_start + 1;
                char *quote_end = strchr(filename_start, '"');
                if (quote_end) {
                    *quote_end = '\0';
                }
            }
        }
    }

    // Extract file extension
    get_file_extension(original_filename, file_extension, sizeof(file_extension));
    
    // Get the next available file number
    int file_number = get_next_file_number();
    if (file_number > 9999) {
        ESP_LOGE(REST_TAG, "File number limit exceeded (9999)");
        httpd_resp_send(req, "File numbering error!", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    // Construct new filename preserving the extension
    char file_path[64];
    snprintf(file_path, sizeof(file_path), "/sdcard/%04d%s", file_number, file_extension);

    FILE *file = fopen(file_path, "wb");
    if (!file) {
        ESP_LOGE(REST_TAG, "Failed to open file for writing: %s", file_path);
        return ESP_FAIL;
    }

    while ((received = httpd_req_recv(req, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, received, file);
    }

    fclose(file);
    ESP_LOGI(REST_TAG, "File upload complete: %s", file_path);

    char response_msg[128];
    snprintf(response_msg, sizeof(response_msg), "Upload Successful: %.100s", file_path);
    httpd_resp_send(req, response_msg, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

// URI Registration
static const httpd_uri_t upload_uri = {
    .uri       = "/upload",
    .method    = HTTP_POST,
    .handler   = upload_handler,
    .user_ctx  = NULL
};

// Register HTTP POST Endpoint
void register_http_post_handler(httpd_handle_t server)
{
    httpd_register_uri_handler(server, &upload_uri);
}


*/