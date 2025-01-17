
#include "http_simple.h"

typedef struct http_simple_local_data {
    int has_sent_header;
    int has_recv_header;
    char *ret_buffer;
    char *encode_buffer;
}http_simple_local_data;

void http_simple_local_data_init(http_simple_local_data* data) {
    data->has_sent_header = 0;
    data->has_recv_header = 0;
    data->ret_buffer = NULL;
    data->encode_buffer = NULL;
}

obfs * http_simple_new_obfs() {
    obfs * self = new_obfs();
    self->l_data = malloc(sizeof(http_simple_local_data));
    http_simple_local_data_init((http_simple_local_data*)self->l_data);
    return self;
}

void http_simple_dispose(obfs *self) {
    http_simple_local_data *local = (http_simple_local_data*)self->l_data;
    if (local->ret_buffer != NULL) {
        free(local->ret_buffer);
    }
    if (local->encode_buffer != NULL) {
        free(local->encode_buffer);
    }
    free(local);
    dispose_obfs(self);
}

char http_simple_hex(char c) {
    if (c < 10) return c + '0';
    return c - 10 + 'a';
}

void http_simple_encode_head(obfs *self, char *data, int datalength) {
    http_simple_local_data *local = (http_simple_local_data*)self->l_data;
    if (local->encode_buffer == NULL) {
        local->encode_buffer = (char*)malloc(datalength * 3 + 1);
    }
    int pos = 0;
    for (; pos < datalength; ++pos) {
        local->encode_buffer[pos * 3] = '%';
        local->encode_buffer[pos * 3 + 1] = http_simple_hex(((unsigned char)data[pos] >> 4));
        local->encode_buffer[pos * 3 + 2] = http_simple_hex(data[pos] & 0xF);
    }
    local->encode_buffer[pos * 3] = 0;
}

int http_simple_client_encode(obfs *self, char *encryptdata, int datalength) {
    http_simple_local_data *local = (http_simple_local_data*)self->l_data;
    if (local->has_sent_header) {
        return datalength;
    }
    char hostport[128];
    int head_size = 64;
    int outlength;
    if (local->ret_buffer == NULL) {
        local->ret_buffer = (char*)malloc(datalength + 1024);
    }
    if (head_size > datalength)
        head_size = datalength;
    http_simple_encode_head(self, encryptdata, head_size);
    if (self->server.param && strlen(self->server.param) == 0)
        self->server.param = NULL;
    if (self->server.port == 80)
        sprintf(hostport, "%s", self->server.param ? self->server.param : self->server.host);
    else
        sprintf(hostport, "%s:%d", self->server.param ? self->server.param : self->server.host, self->server.port);
    sprintf(local->ret_buffer,
            "GET /%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: %s\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.8\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "DNT: 1\r\n"
            "Connection: keep-alive\r\n"
            "\r\n",
            local->encode_buffer,
            hostport,
            "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:40.0) Gecko/20100101 Firefox/40.0"
            );
    outlength = strlen(local->ret_buffer);
    memcpy(local->ret_buffer + outlength, encryptdata + head_size, datalength - head_size);
    outlength += datalength - head_size;
    local->has_sent_header = 1;
    memcpy(encryptdata, local->ret_buffer, outlength);
    return outlength;
}

int http_simple_client_decode(obfs *self, char *encryptdata, int datalength, int *needsendback) {
    http_simple_local_data *local = (http_simple_local_data*)self->l_data;
    *needsendback = 0;
    if (local->has_recv_header) {
        return datalength;
    }
    char* data_begin = strstr(encryptdata, "\r\n\r\n");
    if (data_begin) {
        int outlength;
        data_begin += 4;
        local->has_recv_header = 1;
        outlength = datalength - (data_begin - encryptdata);
        memmove(encryptdata, data_begin, outlength);
        return outlength;
    } else {
        return 0;
    }
}

