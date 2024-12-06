#ifndef DISABLE_PEER_SIGNALING
#include <assert.h>
#include <cJSON.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <core_http_client.h>
#include <core_mqtt.h>

#include "base64.h"
#include "config.h"
#include "peer_signaling.h"
#include "ports.h"
#include "ssl_transport.h"
#include "utils.h"
#include  "lws-callbacks.h"

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

enum lws_write_protocol {
	LWS_WRITE_TEXT						= 0,
	LWS_WRITE_BINARY					= 1,
	LWS_WRITE_CONTINUATION					= 2,
	LWS_WRITE_HTTP						= 3,
	LWS_WRITE_PING						= 5,
	LWS_WRITE_PONG						= 6,
	LWS_WRITE_HTTP_FINAL					= 7,
	LWS_WRITE_HTTP_HEADERS					= 8,
	LWS_WRITE_HTTP_HEADERS_CONTINUATION			= 9,
	LWS_WRITE_BUFLIST = 0x20,
	LWS_WRITE_NO_FIN = 0x40,
	LWS_WRITE_H2_STREAM_END = 0x80,
	LWS_WRITE_CLIENT_IGNORE_XOR_MASK = 0x80
};
// Triển khai lại LibWebSockets
#define _LWS_PAD(n) (((n) % _LWS_PAD_SIZE) ? \
		((n) + (_LWS_PAD_SIZE - ((n) % _LWS_PAD_SIZE))) : (n))
#define LWS_PRE _LWS_PAD(4 + 10 + 2)
#define LWS_SEND_BUFFER_PRE_PADDING LWS_PRE
#define LWS_SEND_BUFFER_POST_PADDING 0

#define LWS_WRITE_RAW LWS_WRITE_HTTP


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

// static char * get_string_from_json_object(JsonObject *object) {
//     JsonNode *root;
//     JsonGenerator *generator;
//     gchar *text;

//     root = json_node_init_object(json_node_alloc(), object);
//     generator = json_generator_new();
//     json_generator_set_root(generator, root);
//     text = json_generator_to_data(generator, NULL);

//     g_object_unref(generator);
//     json_node_free(root);

//     return text;
// }

static char* get_string_from_json_object(cJSON* object) {
    char* text;

    // Chuyển đổi object cJSON thành chuỗi JSON
    text = cJSON_Print(object);

    return text;
}


typedef struct PeerSignaling {
    MQTTContext_t mqtt_ctx;
    MQTTFixedBuffer_t mqtt_fixed_buf;

    TransportInterface_t transport;
    NetworkContext_t net_ctx;

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
static struct lws* web_socket = NULL;
static const char* room = "1234";
static const char* token = "";
static const char* receiver_id = "2002";
static const char* peer_id = "1001";
int mainloop = 1;
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

static int lws_websocket_connection_send_text(struct lws* wsi_in, char* str, enum MsgType msgtype) {
    if (str == NULL || wsi_in == NULL)
        return -1;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + JANUS_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char* p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    size_t n;

    switch (msgtype) {
    case JANUS_MSS_ICE_CANDIDATE_GATHERED:
        n = sprintf((char*)p, "{\"janus\":\"trickle\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"candidate\":{\"completed\":true}}", rand_string(transaction, 12), session_id, handle_id/*, str*/);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_ICE_CANDIDATE:
        n = sprintf((char*)p, "{\"janus\":\"trickle\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"candidate\":%s}", rand_string(transaction, 12), session_id, handle_id, str);
        ("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_SDP_OFFER:
//        n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"configure\",\"audio\":true,\"video\":true},\"jsep\":%s}", rand_string(transaction, 12), session_id, handle_id, str);
        n = sprintf((char*)p, "%s", str);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_SDP_ANSWER:
        n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"configure\",\"audio\":true,\"video\":true},\"jsep\":%s}", rand_string(transaction, 12), session_id, handle_id, str);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_REGISTER:
        n = sprintf((char*)p, "{\"janus\":\"attach\",\"transaction\":\"%s\",\"plugin\":\"janus.plugin.videoroom\",\"opaque_id\":\"videoroomtest-wBYXgNGJGa11\",\"session_id\":%lld}", rand_string(transaction, 12), session_id);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_REGISTER_WITH_SERVER:
        n = sprintf((char*)p, "{\"janus\":\"create\",\"transaction\":\"%s\"}", rand_string(transaction, 12));
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_ROOM_REQUEST:
        n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"join\",\"room\":%s,\"ptype\":\"publisher\",\"display\":\"%s\",\"pin\":\"%s\"}}", rand_string(transaction, 12), session_id, handle_id, room, peer_id, token);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
    case JANUS_MSS_KEEP_ALIVE:
        n = sprintf((char*)p, "{\"janus\":\"keepalive\",\"transaction\":\"%s\",\"session_id\":%lld}", rand_string(transaction, 12), session_id);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
        ///////////////////////////////////////////////////////////////////
    case JANUS_MSS_ROOM_PARTICIPANTS:
        n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\" : \"listparticipants\",\"room\":%s}}", rand_string(transaction, 12), session_id, handle_id, room);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        break;
        ///////////////////////////////////////////////////////////////////
    case JANUS_MSS_ROOM_CALL:
        //        if (already == 1) {
        n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%lld,\"handle_id\":%lld,\"body\":{\"request\":\"join\",\"room\":%s,\"ptype\":\"subscriber\",\"feed\":%lld,\"pin\":\"%s\"}}", rand_string(transaction, 12), session_id, handle_id, room, feed_id, token);
        //			n = sprintf((char*)p, "{\"janus\":\"message\",\"transaction\":\"%s\",\"session_id\":%"G_GINT64_FORMAT",\"handle_id\":%"G_GINT64_FORMAT",\"body\":{\"request\":\"join\",\"room\":%s,\"ptype\":\"subscriber\",\"feed\":%"G_GINT64_FORMAT",\"private_id\":%s,\"pin\":\"%s\"}}", rand_string(transaction, 12), session_id, handle_id, room, feed_id, rand_string(transaction, 9), token);
        printf("sent %s\n", (char*)p);
        lws_write(wsi_in, p, n, LWS_WRITE_TEXT);
        //        }
        break;
    default:
        break;
    }
    return n;
}

// static boolean do_register(void)
// {
//     app_state = SERVER_REGISTERING_2;
//     g_print("ATTACH JANUS PLUGIN MSS_REGISTER\n");
//     lws_websocket_connection_send_text(web_socket, (char*)"", JANUS_MSS_REGISTER);

//     return TRUE;
// }


static int websocket_write_back(struct lws* wsi_in, char* str, int str_size_in) {
    if (str == NULL || wsi_in == NULL) {
        return -1;
    }

    int n = 0;
    cJSON *root = NULL;

    // Parse the JSON string using cJSON
    root = cJSON_Parse((const char*)str);
    if (!root) {
        printf("Unknown message format, ignoring\n");
        return 0;
    }

    // Check for required fields ("id" and "type")
    cJSON *id = cJSON_GetObjectItemCaseSensitive(root, "id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(root, "type");
    if (!cJSON_IsString(id) || !cJSON_IsString(type)) {
        g_print("Missing required fields (id or type) in message\n");
        cJSON_Delete(root);  // Free memory
        return 0;
    }

    const char *id_str = id->valuestring;
    const char *type_str = type->valuestring;

    PeerConnectionState state;
    state = peer_connection_get_state(g_ps.pc);

    if (strcmp(type_str, "request") == 0) {
        g_print("Received new request: %s\n", id_str);
        if (state == PEER_CONNECTION_CLOSED ||
            state == PEER_CONNECTION_NEW ||
            state == PEER_CONNECTION_FAILED ||
            state == PEER_CONNECTION_DISCONNECTED) {
            g_ps.id = strdup(id_str);
            peer_connection_create_offer(g_ps.pc);
        }
    } else if (strcmp(type_str, "answer") == 0) {
        cJSON *sdp = cJSON_GetObjectItemCaseSensitive(root, "sdp");
        if (!cJSON_IsString(sdp)) {
            g_print("Missing required field (sdp) in answer message\n");
            cJSON_Delete(root);
            return 0;
        }

        const char *sdp_str = sdp->valuestring;
        g_print("Received answer SDP: %s\n", sdp_str);

        if (state == PEER_CONNECTION_NEW) {
            peer_connection_set_remote_description(g_ps.pc, sdp_str);
        }
    }

    // Free memory allocated by cJSON
    cJSON_Delete(root);

    return n;
}

static int callback_janus(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    printf("\t START: callback_janus %d\n", reason);
    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
    {
        lws_callback_on_writable(wsi);
        printf("Connected\n");
        break;
    }
    case LWS_CALLBACK_CLIENT_RECEIVE:
    {
        // Handle incomming messages here
        printf("got \n");
        unsigned char* buf = (unsigned char*)
                malloc(LWS_SEND_BUFFER_PRE_PADDING + len
                       + LWS_SEND_BUFFER_POST_PADDING);
        unsigned int i;
        for (i = 0; i < len; i++) {
            buf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i] =
                    ((char*)in)[i];
        }
        printf("received data: %s\n", (char*)in);

        websocket_write_back(wsi, (char*)in, -1);

        free(buf);

        break;
    }
    case LWS_CALLBACK_CLIENT_WRITEABLE:
    {
        app_state = SERVER_REGISTERING;
        lws_websocket_connection_send_text(web_socket,(char*)"",JANUS_MSS_REGISTER_WITH_SERVER);
        break;
    }
    case LWS_CALLBACK_CLOSED:
    {
        printf("\n\n \t\t --- CLOSED --- \n\n");
        web_socket = NULL;
        break;
    }
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
    {
        printf("\n\n \t\t --- CLIENT CONNECTION ERROR --- \n\n");
        web_socket = NULL;
        break;
    }
    default:
        break;
    }

    //	printf("\t END: callback_janus %d\n", reason);
    return 0;
}

// static struct lws_protocols protocols[] =
// {
// {
//     "janus-protocol",
//     callback_janus,
//     0,
//     JANUS_RX_BUFFER_BYTES,
// },
// { NULL, NULL, 0, 0 } /* terminator */
// };

static char* peer_signalling_connection_get_text_msg(enum MsgType msgtype) {
    size_t n;
    char p[1000];
    switch (msgtype) {
    case JANUS_MSS_REGISTER_WITH_SERVER :
        n = sprintf(&p, "{\"janus\":\"create\",\"transaction\":\"%s\"}", rand_string(transaction, 12));
        printf("sent %s\n", (char*)p);
    }
    return strdup(p);
}

static void peer_signaling_mqtt_publish(MQTTContext_t* mqtt_ctx, enum MsgType msgtype) {
    MQTTStatus_t status;
    MQTTPublishInfo_t pub_info;
    char *message = peer_signalling_connection_get_text_msg(msgtype);
    memset(&pub_info, 0, sizeof(pub_info));

    pub_info.qos = MQTTQoS0;
    pub_info.retain = false;
    pub_info.pTopicName = g_ps.pubtopic;
    pub_info.topicNameLength = strlen(g_ps.pubtopic);
    pub_info.pPayload = message;
    pub_info.payloadLength = strlen(message);

    status = MQTT_Publish(mqtt_ctx, &pub_info, MQTT_GetPacketId(mqtt_ctx));
    if (status != MQTTSuccess) {
        LOGE("MQTT_Publish failed: Status=%s.", MQTT_Status_strerror(status));
    } else {
        LOGD("MQTT_Publish succeeded.");
    }
}

static void peer_signaling_on_pub_event(const char* msg, size_t size) {
    cJSON *req, *res, *item, *result, *error;
    int id = -1;
    char* payload = NULL;
    PeerConnectionState state;

    req = res = item = result = error = NULL;
    state = peer_connection_get_state(g_ps.pc);
    do {
        req = cJSON_Parse(msg);
        if (!req) {
            error = cJSON_CreateRaw(RPC_ERROR_PARSE_ERROR);
            LOGW("Parse json failed");
            break;
        }

        item = cJSON_GetObjectItem(req, "id");
        if (!item && !cJSON_IsNumber(item)) {
            error = cJSON_CreateRaw(RPC_ERROR_INVALID_REQUEST);
            LOGW("Cannot find id");
            break;
        }

        id = item->valueint;

        item = cJSON_GetObjectItem(req, "method");
        if (!item && cJSON_IsString(item)) {
            error = cJSON_CreateRaw(RPC_ERROR_INVALID_REQUEST);
            LOGW("Cannot find method");
            break;
        }
        if (strcmp(item->valuestring, RPC_METHOD_OFFER) == 0) {
            switch (state) {
            case PEER_CONNECTION_NEW:
            case PEER_CONNECTION_DISCONNECTED:
            case PEER_CONNECTION_FAILED:
            case PEER_CONNECTION_CLOSED: {
                g_ps.id = id;
                peer_connection_create_offer(g_ps.pc);
            } break;
            default: {
                error = cJSON_CreateRaw(RPC_ERROR_INTERNAL_ERROR);
            } break;
            }
        } else if (strcmp(item->valuestring, RPC_METHOD_ANSWER) == 0) {
            item = cJSON_GetObjectItem(req, "params");
            if (!item && !cJSON_IsString(item)) {
                error = cJSON_CreateRaw(RPC_ERROR_INVALID_PARAMS);
                LOGW("Cannot find params");
                break;
            }

            if (state == PEER_CONNECTION_NEW) {
                peer_connection_set_remote_description(g_ps.pc, item->valuestring);
                result = cJSON_CreateString("");
            }

        } else if (strcmp(item->valuestring, RPC_METHOD_STATE) == 0) {
            result = cJSON_CreateString(peer_connection_state_to_string(state));

        } else if (strcmp(item->valuestring, RPC_METHOD_CLOSE) == 0) {
            peer_connection_close(g_ps.pc);
            result = cJSON_CreateString("");

        } else {
            error = cJSON_CreateRaw(RPC_ERROR_METHOD_NOT_FOUND);
            LOGW("Unsupport method");
        }

    } while (0);

    if (result || error) {
        res = cJSON_CreateObject();
        cJSON_AddStringToObject(res, "jsonrpc", RPC_VERSION);
        cJSON_AddNumberToObject(res, "id", id);

        if (result) {
            cJSON_AddItemToObject(res, "result", result);
        } else if (error) {
            cJSON_AddItemToObject(res, "error", error);
        }

        payload = cJSON_PrintUnformatted(res);

        if (payload) {
            //      peer_signaling_mqtt_publish(&g_ps.mqtt_ctx, payload);
            free(payload);
        }
        cJSON_Delete(res);
    }

    if (req) {
        cJSON_Delete(req);
    }
}

HTTPResponse_t peer_signaling_http_request(const TransportInterface_t* transport_interface,
                                           const char* method,
                                           size_t method_len,
                                           const char* host,
                                           size_t host_len,
                                           const char* path,
                                           size_t path_len,
                                           const char* auth,
                                           size_t auth_len,
                                           const char* body,
                                           size_t body_len) {
    HTTPStatus_t status = HTTPSuccess;
    HTTPRequestInfo_t request_info = {0};
    HTTPResponse_t response = {0};
    HTTPRequestHeaders_t request_headers = {0};

    request_info.pMethod = method;
    request_info.methodLen = method_len;
    request_info.pHost = host;
    request_info.hostLen = host_len;
    request_info.pPath = path;
    request_info.pathLen = path_len;
    request_info.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    request_headers.pBuffer = g_ps.http_buf;
    request_headers.bufferLen = sizeof(g_ps.http_buf);

    status = HTTPClient_InitializeRequestHeaders(&request_headers, &request_info);

    if (status == HTTPSuccess) {
        HTTPClient_AddHeader(&request_headers,
                             "Content-Type", strlen("Content-Type"), "application/sdp", strlen("application/sdp"));

        if (auth_len > 0) {
            HTTPClient_AddHeader(&request_headers,
                                 "Authorization", strlen("Authorization"), auth, auth_len);
        }

        response.pBuffer = g_ps.http_buf;
        response.bufferLen = sizeof(g_ps.http_buf);

        status = HTTPClient_Send(transport_interface,
                                 &request_headers, (uint8_t*)body, body ? body_len : 0, &response, 0);

    } else {
        LOGE("Failed to initialize HTTP request headers: Error=%s.", HTTPClient_strerror(status));
    }

    return response;
}

static int peer_signaling_http_post(const char* hostname, const char* path, int port, const char* auth, const char* body) {
    int ret = 0;
    TransportInterface_t trans_if = {0};
    NetworkContext_t net_ctx;
    HTTPResponse_t res;

    trans_if.recv = ssl_transport_recv;
    trans_if.send = ssl_transport_send;
    trans_if.pNetworkContext = &net_ctx;

    if (port <= 0) {
        LOGE("Invalid port number: %d", port);
        return -1;
    }

    ret = ssl_transport_connect(&net_ctx, hostname, port, NULL);

    if (ret < 0) {
        LOGE("Failed to connect to %s:%d", hostname, port);
        return ret;
    }

    res = peer_signaling_http_request(&trans_if, "POST", 4, hostname, strlen(hostname), path,
                                      strlen(path), auth, strlen(auth), body, strlen(body));

    ssl_transport_disconnect(&net_ctx);

    if (res.pHeaders == NULL) {
        LOGE("Response headers are NULL");
        return -1;
    }

    if (res.pBody == NULL) {
        LOGE("Response body is NULL");
        return -1;
    }

    LOGI(
                "Received HTTP response from %s%s\n"
                "Response Headers: %s\nResponse Status: %u\nResponse Body: %s\n",
                hostname, path, res.pHeaders, res.statusCode, res.pBody);

    if (res.statusCode == 201) {
        peer_connection_set_remote_description(g_ps.pc, (const char*)res.pBody);
    }
    return 0;
}


static int peer_signaling_mqtt_subscribe(int subscribed) {
    MQTTStatus_t status = MQTTSuccess;
    MQTTSubscribeInfo_t sub_info;

    uint16_t packet_id = MQTT_GetPacketId(&g_ps.mqtt_ctx);

    memset(&sub_info, 0, sizeof(sub_info));
    sub_info.qos = MQTTQoS0;
    sub_info.pTopicFilter = g_ps.subtopic;
    sub_info.topicFilterLength = strlen(g_ps.subtopic);

    if (subscribed) {
        status = MQTT_Subscribe(&g_ps.mqtt_ctx, &sub_info, 1, packet_id);
    } else {
        status = MQTT_Unsubscribe(&g_ps.mqtt_ctx, &sub_info, 1, packet_id);
    }
    if (status != MQTTSuccess) {
        LOGE("MQTT_Subscribe failed: Status=%s.", MQTT_Status_strerror(status));
        return -1;
    }

    status = MQTT_ProcessLoop(&g_ps.mqtt_ctx);

    if (status != MQTTSuccess) {
        LOGE("MQTT_ProcessLoop failed: Status=%s.", MQTT_Status_strerror(status));
        return -1;
    }

    LOGD("MQTT Subscribe/Unsubscribe succeeded.");
    return 0;
}
#include <cJSON.h>

static void peer_signaling_onicecandidate(char* description, void* userdata) {
    printf("send offer:%s\n", description);
    
    cJSON *jsepOffer = cJSON_CreateObject();
    cJSON_AddStringToObject(jsepOffer, "id", g_ps.id);
    cJSON_AddStringToObject(jsepOffer, "type", "offer");

    // Remove host candidate from SDP
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

    printf("send modified offer:%s\n", description);
    cJSON_AddStringToObject(jsepOffer, "sdp", description);

    char *text = cJSON_Print(jsepOffer);
    lws_websocket_connection_send_text(web_socket, text, JANUS_MSS_SDP_OFFER);

    free(text);
    cJSON_Delete(jsepOffer);
}


int peer_signaling_whip_connect() {
    if (g_ps.pc == NULL) {
        LOGW("PeerConnection is NULL");
        return -1;
    } else if (g_ps.http_port <= 0) {
        LOGW("Invalid HTTP port number: %d", g_ps.http_port);
        return -1;
    }

    peer_connection_create_offer(g_ps.pc);
    return 0;
}

void peer_signaling_whip_disconnect() {
    // TODO: implement
}

int peer_signaling_join_channel() {
    if (g_ps.pc == NULL) {
        LOGW("PeerConnection is NULL");
        return -1;
    } else if (g_ps.mqtt_port <= 0) {
        LOGW("Invalid MQTT port number: %d", g_ps.mqtt_port);
        if (peer_signaling_whip_connect() < 0) {
            LOGW("Tried MQTT and WHIP, connect failed");
            return -1;
        }
        return 0;
    }

    if (peer_signaling_mqtt_connect(g_ps.mqtt_host, g_ps.mqtt_port) < 0) {
        LOGW("Connect MQTT server failed");
        return -1;
    }
    //congnv
    peer_signaling_mqtt_publish(&g_ps.mqtt_ctx,JANUS_MSS_REGISTER);

    peer_signaling_mqtt_subscribe(1);
    return 0;
}

int peer_signaling_loop() {
    //  if (g_ps.mqtt_port > 0) {
    //    MQTT_ProcessLoop(&g_ps.mqtt_ctx);
    //  }
    connect_to_janus_server();
    return 0;
}

void peer_signaling_leave_channel() {
    MQTTStatus_t status = MQTTSuccess;

    if (g_ps.mqtt_port > 0 && peer_signaling_mqtt_subscribe(0) == 0) {
        status = MQTT_Disconnect(&g_ps.mqtt_ctx);
        if (status != MQTTSuccess) {
            LOGE("Failed to disconnect with broker: %s", MQTT_Status_strerror(status));
        }
    }
}

void peer_signaling_set_config(ServiceConfiguration* service_config) {
    char* pos;

    memset(&g_ps, 0, sizeof(g_ps));

    do {
        if (service_config->http_url == NULL || strlen(service_config->http_url) == 0) {
            break;
        }

        if ((pos = strstr(service_config->http_url, "/")) != NULL) {
            strncpy(g_ps.http_host, service_config->http_url, pos - service_config->http_url);
            strncpy(g_ps.http_path, pos, HOST_LEN);
        } else {
            strncpy(g_ps.http_host, service_config->http_url, HOST_LEN);
        }

        g_ps.http_port = service_config->http_port;
        LOGI("HTTP Host: %s, Port: %d, Path: %s", g_ps.http_host, g_ps.http_port, g_ps.http_path);
    } while (0);

    do {
        if (service_config->mqtt_url == NULL || strlen(service_config->mqtt_url) == 0) {
            break;
        }

        strncpy(g_ps.mqtt_host, service_config->mqtt_url, HOST_LEN);
        g_ps.mqtt_port = service_config->mqtt_port;
        LOGD("MQTT Host: %s, Port: %d", g_ps.mqtt_host, g_ps.mqtt_port);
    } while (0);

    //congnv
    do {
        if (service_config->mqtt_url == NULL || strlen(service_config->mqtt_url) == 0) {
            break;
        }

        strncpy(g_ps.ws_host, service_config->ws_url, HOST_LEN);
        g_ps.ws_port = service_config->ws_port;
        LOGD("WS Host: %s, Port: %d", g_ps.mqtt_host, g_ps.mqtt_port);
    } while (0);

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
#endif  // DISABLE_PEER_SIGNALING

void connect_to_janus_server()
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;

    struct lws_context* context_lw = lws_create_context(&info);

    if (!context_lw) {
        lwsl_err("lws init failed!\n");
        return;
    }

    struct lws_client_connect_info ccinfo;
    memset(&ccinfo, 0 , sizeof(ccinfo));
    ccinfo.context = context_lw;
    ccinfo.address = g_ps.ws_host;
    //            ccinfo.port = 8188;
    ccinfo.port = g_ps.ws_port;
    ccinfo.path = "/server";
    ccinfo.host = lws_canonical_hostname(context_lw);
    ccinfo.origin = "origin";
    ccinfo.protocol = protocols[PROTOCOL_JANUS].name;
    web_socket = lws_client_connect_via_info(&ccinfo);

    app_state = SERVER_CONNECTING;

    while (mainloop) {
         lws_service(context_lw, /* timeout_ms = */ 250);
    }

    lws_context_destroy(context_lw);
}
