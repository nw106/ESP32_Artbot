#include <esp_log.h>
#include <esp_http_server.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <stdio.h>
#include <string.h>

static const char *REST_TAG = "http_post";
static const char *NVS_NAMESPACE = "storage";  // NVS storage namespace
static const char *NVS_KEY = "file_number";    // Key for storing file number

// Function to read the last stored file number from NVS
static int get_next_file_number() {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int file_number = 1;  // Default starting value

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
    
    // Get the next available file number
    int file_number = get_next_file_number();
    if (file_number > 9999) {
        ESP_LOGE(REST_TAG, "File number limit exceeded (9999)");
        httpd_resp_send(req, "File numbering error!", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    // Generate new filename (e.g., "/sdcard/0001.jpg")
    char file_path[32];
    snprintf(file_path, sizeof(file_path), "/sdcard/%04d.jpg", file_number);

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

    char response_msg[64];
    snprintf(response_msg, sizeof(response_msg), "Upload Successful: %s", file_path);
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
