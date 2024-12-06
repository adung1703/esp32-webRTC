#ifndef DISABLE_PEER_SIGNALING
#include <assert.h>
#include <cJSON.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

// #include <core_http_client.h>
// #include <core_mqtt.h>

#include "base64.h"
#include "config.h"
#include "peer_signaling.h"
#include "ports.h"
#include "ssl_transport.h"
#include "utils.h"
// #include <libwebsockets.h>
// #include <json-glib/json-glib.h>
#include "esp_websocket_client.h"
#include "esp_log.h"

#define KEEP_ALIVE_TIMEOUT_SECONDS 60
#define CONNACK_RECV_TIMEOUT_MS 1000

#define TOPIC_SIZE 128
#define HOST_LEN 64
#define CRED_LEN 128
#define PEER_ID_SIZE 100

#define RPC_VERSION "2.0"

#define RPC_METHOD_STATE "state"
#define RPC_METHOD_OFFER "offer"
#define RPC_METHOD_ANSWER "answer"
#define RPC_METHOD_CLOSE "close"

#define RPC_ERROR_PARSE_ERROR "{\"code\":-32700,\"message\":\"Parse error\"}"
#define RPC_ERROR_INVALID_REQUEST "{\"code\":-32600,\"message\":\"Invalid Request\"}"
#define RPC_ERROR_METHOD_NOT_FOUND "{\"code\":-32601,\"message\":\"Method not found\"}"
#define RPC_ERROR_INVALID_PARAMS "{\"code\":-32602,\"message\":\"Invalid params\"}"
#define RPC_ERROR_INTERNAL_ERROR "{\"code\":-32603,\"message\":\"Internal error\"}"

#define WEBSOCKET_MESSAGE_BUFFER_SIZE 2000

static char message_buffer[WEBSOCKET_MESSAGE_BUFFER_SIZE];
static int message_length = 0;

static char* rand_string(char* str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

typedef struct PeerSignaling {
    // MQTTContext_t mqtt_ctx;
    // MQTTFixedBuffer_t mqtt_fixed_buf;

    // TransportInterface_t transport;
    // NetworkContext_t net_ctx;

    uint8_t mqtt_buf[CONFIG_MQTT_BUFFER_SIZE];
    uint8_t http_buf[CONFIG_HTTP_BUFFER_SIZE];

    char subtopic[TOPIC_SIZE];
    char pubtopic[TOPIC_SIZE];

    uint16_t packet_id;
    char *id;

    int mqtt_port;
    int http_port;
    char mqtt_host[HOST_LEN];
    char http_host[HOST_LEN];
    char http_path[HOST_LEN];
    char ws_host[HOST_LEN];
    int  ws_port;
    char username[CRED_LEN];
    char password[CRED_LEN];
    char client_id[CRED_LEN];
    PeerConnection* pc;

} PeerSignaling;

enum MsgType {
    JANUS_MSS_UNKNOWN = 0,
    JANUS_MSS_ICE_CANDIDATE = 1,
    JANUS_MSS_ICE_CANDIDATE_GATHERED = 2,
    JANUS_MSS_SDP_OFFER = 3,
    JANUS_MSS_SDP_ANSWER = 4,
    JANUS_MSS_REGISTER = 5,
    JANUS_MSS_REGISTER_WITH_SERVER = 6,
    JANUS_MSS_ROOM_REQUEST = 7,
    JANUS_MSS_KEEP_ALIVE = 8,
    JANUS_MSS_ROOM_CALL = 9,
    ///////////////////////////////////////////////////////////
    JANUS_MSS_ROOM_PARTICIPANTS = 10,
    //////////////////////////////////////////////////////////
};

static PeerSignaling g_ps;

//Janus information
long long feed_id = 0;
long long session_id = 0;
long long handle_id = 0;
char transaction[12];
// static struct lws* web_socket = NULL;
esp_websocket_client_handle_t web_socket = NULL; // Global handle for the WebSocket client
static const char* room = "1234";
static const char* token = "";
static const char* receiver_id = "2002";
static const char* peer_id = "1001";

static const char *TAG = "WEBSOCKET";

static int mainloop = 1;
enum AppState {
    APP_STATE_UNKNOWN = 0,
    APP_STATE_ERROR = 1, /* generic error */
    SERVER_CONNECTING = 1000,
    SERVER_CONNECTION_ERROR,
    SERVER_CONNECTED, /* Ready to register */
    SERVER_REGISTERING = 2000,
    SERVER_REGISTRATION_ERROR,
    SERVER_REGISTERED, /* Ready to call a peer */
    SERVER_REGISTERING_2 = 2500,
    SERVER_REGISTRATION_ERROR_2,
    SERVER_REGISTERED_2, /* Ready to call a peer */
    SERVER_REGISTERING_3 = 2700,
    SERVER_REGISTRATION_ERROR_3,
    SERVER_REGISTERED_3, /* Ready to call a peer */
    SERVER_CLOSED, /* server connection closed by us or the server */
    PEER_CONNECTING = 3000,
    PEER_CONNECTION_ERROR,
    PEER_CONNECTED,
    PEER_CALL_NEGOTIATING = 3500,
    PEER_CALL_NEGOTIATED = 4000,
    PEER_CALL_STARTED,
    PEER_CALL_STOPPING,
    PEER_CALL_STOPPED,
    PEER_CALL_ERROR,
};
static enum AppState app_state = APP_STATE_UNKNOWN;
#define JANUS_RX_BUFFER_BYTES (1024*100)
enum protocols
{
    PROTOCOL_JANUS = 0,
    PROTOCOL_COUNT
};

int esp_websocket_send_text(esp_websocket_client_handle_t web_socket, char *str, enum MsgType msgtype) {
    if (str == NULL || web_socket == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return -1;
    }

    char buf[1200];  // Adjust size based on your message requirements
    int n = 0;

    switch (msgtype) {
        case JANUS_MSS_ICE_CANDIDATE_GATHERED:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"trickle\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"candidate\":{\"completed\":true}}",
                         rand_string(transaction, 12), session_id, handle_id);
            ESP_LOGI(TAG, "Sending: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_ICE_CANDIDATE:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"trickle\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"candidate\":%s}",
                         rand_string(transaction, 12), session_id, handle_id, str);
            ESP_LOGI(TAG, "Sending: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_SDP_OFFER:
            n = snprintf(buf, sizeof(buf), "%s", str);
            ESP_LOGI(TAG, "Sending SDP Offer: %s", buf);
            // esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            int bytes_sent = esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            if (bytes_sent < 0) {
                ESP_LOGE(TAG, "Failed to send WebSocket message: %d", bytes_sent);
            } else {
                ESP_LOGI(TAG, "WebSocket message sent: %s (%d bytes)", buf, bytes_sent);
            }
            break;

        case JANUS_MSS_SDP_ANSWER:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"configure\",\"audio\":true,\"video\":true},\"jsep\":%s}",
                         rand_string(transaction, 12), session_id, handle_id, str);
            ESP_LOGI(TAG, "Sending SDP Answer: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_REGISTER:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"attach\",\"transaction\":\"%s\",\"plugin\":\"janus.plugin.videoroom\",\"opaque_id\":\"videoroomtest-wBYXgNGJGa11\",\"session_id\":%lld}",
                         rand_string(transaction, 12), session_id);
            ESP_LOGI(TAG, "Sending Register: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_REGISTER_WITH_SERVER:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"create\",\"transaction\":\"%s\"}",
                         rand_string(transaction, 12));
            ESP_LOGI(TAG, "Sending Register with Server: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_ROOM_REQUEST:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"join\",\"room\":%s,\"ptype\":\"publisher\",\"display\":\"%s\",\"pin\":\"%s\"}}",
                         rand_string(transaction, 12), session_id, handle_id, room, peer_id, token);
            ESP_LOGI(TAG, "Sending Room Request: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_KEEP_ALIVE:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"keepalive\",\"transaction\":\"%s\",\"session_id\":%lld}",
                         rand_string(transaction, 12), session_id);
            ESP_LOGI(TAG, "Sending Keep Alive: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_ROOM_PARTICIPANTS:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\" : \"listparticipants\",\"room\":%s}}",
                         rand_string(transaction, 12), session_id, handle_id, room);
            ESP_LOGI(TAG, "Sending Room Participants Request: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        case JANUS_MSS_ROOM_CALL:
            n = snprintf(buf, sizeof(buf),
                         "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"join\",\"room\":%s,\"ptype\":\"subscriber\",\"feed\":%lld,\"pin\":\"%s\"}}",
                         rand_string(transaction, 12), session_id, handle_id, room, feed_id, token);
            ESP_LOGI(TAG, "Sending Room Call: %s", buf);
            esp_websocket_client_send_text(web_socket, buf, n, portMAX_DELAY);
            break;

        default:
            ESP_LOGE(TAG, "Unsupported message type");
            return -1;
    }

    return n;  // Returns the number of bytes sent
}

static int websocket_handle_message(char *str) {
    if (str == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return -1;
    }

    // Parse the JSON string
    cJSON *root = cJSON_Parse(str);
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON: %s", str);
        return -1;
    }

    // Check required fields
    cJSON *type_item = cJSON_GetObjectItem(root, "type");
    cJSON *id_item = cJSON_GetObjectItem(root, "id");
    if (!cJSON_IsString(type_item) || !cJSON_IsString(id_item)) {
        ESP_LOGE(TAG, "Invalid JSON: Missing 'type' or 'id' fields");
        cJSON_Delete(root);
        return -1;
    }

    const char *type = type_item->valuestring;
    const char *id = id_item->valuestring;
    ESP_LOGI(TAG, "Message received: type=%s, id=%s", type, id);

    PeerConnectionState state = peer_connection_get_state(g_ps.pc);

    if (strcmp(type, "request") == 0) {
        ESP_LOGI(TAG, "Received a new request: %s", id);
        if (state == PEER_CONNECTION_CLOSED ||
            state == PEER_CONNECTION_NEW ||
            state == PEER_CONNECTION_FAILED ||
            state == PEER_CONNECTION_DISCONNECTED) {
            g_ps.id = strdup(id); // Save the request ID
            peer_connection_create_offer(g_ps.pc);
        }
    } else if (strcmp(type, "answer") == 0) {
        cJSON *sdp_item = cJSON_GetObjectItem(root, "sdp");
        if (cJSON_IsString(sdp_item)) {
            const char *sdp = sdp_item->valuestring;
            ESP_LOGI(TAG, "Received an answer SDP: %s", sdp);
            if (state == PEER_CONNECTION_NEW) {
                peer_connection_set_remote_description(g_ps.pc, sdp);
            }
        } else {
            ESP_LOGE(TAG, "Missing or invalid 'sdp' in answer");
        }
    } else {
        ESP_LOGW(TAG, "Unknown message type: %s", type);
    }

    cJSON_Delete(root); // Free the JSON object
    return 0;
}

static void peer_signaling_onicecandidate(char *description, void *userdata) {
    printf("send offer here:\n%s\n", description);

    // Create JSON object for the offer
    cJSON *jsepOffer = cJSON_CreateObject();
    cJSON_AddStringToObject(jsepOffer, "id", g_ps.id); // Use the global peer state ID
    cJSON_AddStringToObject(jsepOffer, "type", "offer");

    // Remove host candidates from SDP
    char sdp_without_host_candidate[5000];
    memset(sdp_without_host_candidate, 0, sizeof(sdp_without_host_candidate));

    char *line = strtok(description, "\n");
    while (line != NULL) {
        if (strstr(line, "typ host") == NULL) {
            strcat(sdp_without_host_candidate, line);
            strcat(sdp_without_host_candidate, "\n");
        }
        line = strtok(NULL, "\n");
    }
    strcpy(description, sdp_without_host_candidate);

    printf("send modified offer:\n%s\n", description);

    // Add the modified SDP to the JSON object
    cJSON_AddStringToObject(jsepOffer, "sdp", description);

    // Convert the JSON object to a string
    char *text = cJSON_PrintUnformatted(jsepOffer);
    if (text == NULL) {
        printf("Error creating JSON string\n");
        cJSON_Delete(jsepOffer);
        return;
    }

    // Send the offer over WebSocket
    esp_websocket_send_text(web_socket, text, JANUS_MSS_SDP_OFFER);

    // Free allocated resources
    cJSON_Delete(jsepOffer);
    free(text);
}

void peer_signaling_set_config(ServiceConfiguration* service_config) {
    char* pos;

    memset(&g_ps, 0, sizeof(g_ps));
    //congnv
    // do {
    //     if (service_config->ws_url == NULL || strlen(service_config->ws_url) == 0) {
    //         break;
    //     }

    //     strncpy(g_ps.ws_host, service_config->ws_url, HOST_LEN);
    //     g_ps.ws_port = service_config->ws_port;
    //     LOGI("WS Host: %s, Port: %d", g_ps.ws_host, g_ps.ws_port);
    // } while (0);

    if (service_config->client_id != NULL && strlen(service_config->client_id) > 0) {
        strncpy(g_ps.client_id, service_config->client_id, CRED_LEN);
        snprintf(g_ps.subtopic, sizeof(g_ps.subtopic), "webrtc/%s/jsonrpc", service_config->client_id);
        snprintf(g_ps.pubtopic, sizeof(g_ps.pubtopic), "webrtc/%s/jsonrpc-reply", service_config->client_id);
    }

    if (service_config->username != NULL && strlen(service_config->username) > 0) {
        strncpy(g_ps.username, service_config->username, CRED_LEN);
    }

    if (service_config->password != NULL && strlen(service_config->password) > 0) {
        strncpy(g_ps.password, service_config->password, CRED_LEN);
    }

    g_ps.pc = service_config->pc;
    peer_connection_onicecandidate(g_ps.pc, peer_signaling_onicecandidate);
}

// WebSocket event handler
static void websocket_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WebSocket connected");
        app_state = SERVER_REGISTERING;
        // Send registration message
        esp_websocket_send_text(web_socket, "", JANUS_MSS_REGISTER_WITH_SERVER);
        break;

    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WebSocket received data: %.*s", data->data_len, (char *)data->data_ptr);
        ESP_LOGI(TAG, "WebSocket data info: fin=%d, opcode=%d, len=%d",
            data->fin, data->op_code, data->data_len);        
             
        // Check if the current fragment fits into the buffer
        if (message_length + data->data_len < WEBSOCKET_MESSAGE_BUFFER_SIZE) {
            // Append the fragment to the buffer
            memcpy(message_buffer + message_length, data->data_ptr, data->data_len);
            message_length += data->data_len;

            // If this is the final fragment, process the complete message
            if (data->data_len < 1024) {
                message_buffer[message_length] = '\0'; // Null-terminate the string
                ESP_LOGI(TAG, "Complete WebSocket message received: %s", message_buffer);

                // Handle the complete message
                websocket_handle_message(message_buffer);

                // Reset the buffer
                message_length = 0;
            }
        } else {
            ESP_LOGE(TAG, "WebSocket message too large, discarding!");
            message_length = 0; // Reset the buffer
        }
        break;

    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WebSocket disconnected");
        web_socket = NULL; // Reset WebSocket handle
        break;

    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGE(TAG, "WebSocket connection error");
        web_socket = NULL; // Reset WebSocket handle
        break;
        
    // case WEBSOCKET_EVENT_ERROR_RECEIVE:
    //     ESP_LOGE(TAG, "Error receiving WebSocket data");
    //     break;

    default:
        ESP_LOGW(TAG, "Unhandled WebSocket event: %ld", event_id);
        break;
    }
}

void connect_to_server()
{
    // Configuration for the WebSocket 
    esp_websocket_client_config_t websocket_cfg = {
        .host = "192.168.66.217",                // WebSocket server host
        .port = 8000,                           // WebSocket server port
        .path = "/server",                      // WebSocket server path
        // .origin = "origin",                     // Optional: Adjust as per your server's requirements
        // .protocol = "janus-protocol",           // Optional: Specify your WebSocket subprotocol
    };

    // The global WebSocket 
    web_socket = esp_websocket_client_init(&websocket_cfg);
    
    if (!web_socket) {
        ESP_LOGE(TAG, "Failed to initialize WebSocket client");
        return;
    }
    // Register a single event handler for all WebSocket events
    esp_websocket_register_events(web_socket, ESP_EVENT_ANY_ID, websocket_event_handler, NULL);

    // Start the WebSocket connection
    if (esp_websocket_client_start(web_socket) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WebSocket connection");
        return;
    }   

    ESP_LOGI(TAG, "WebSocket client started, connecting to %s:%d%s", 
             websocket_cfg.host, websocket_cfg.port, websocket_cfg.path);
    // while(1);
    // Stay here

}

// Cleanup
void websocket_stop() {
    if (web_socket != NULL) {
        esp_websocket_client_stop(web_socket);
        esp_websocket_client_destroy(web_socket);
        web_socket = NULL;
        ESP_LOGI(TAG, "WebSocket stopped");
    }
}

int peer_signaling_loop() {
    connect_to_server();
    return 0;
}

void peer_signaling_leave_channel() {
    websocket_stop();
}

#endif  // DISABLE_PEER_SIGNALING