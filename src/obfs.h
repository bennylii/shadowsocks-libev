/*
 * obfs.h - Define shadowsocks server's buffers and callbacks
 *
 * Copyright (C) 2015 - 2015, Break Wa11 <mmgac001@gmail.com>
 *
 * This file is part of the shadowsocks-libev.
 *
 * shadowsocks-libev is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-libev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with shadowsocks-libev; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _OBFS_H
#define _OBFS_H

#include <stdint.h>
#include <unistd.h>

typedef struct server_info {
    char host[64];
    uint16_t port;
    char *param;
    void *g_data;
    uint8_t *iv;
    size_t iv_len;
    uint8_t *recv_iv;
    size_t recv_iv_len;
    uint8_t *key;
    size_t key_len;
    size_t tcp_mss;
}server_info;

typedef struct obfs {
    server_info server;
    void *l_data;
}obfs;

typedef struct obfs_class {
    void * (*init_data)();
    obfs * (*new_obfs)();
    void (*set_server_info)(obfs *self, server_info *server);
    void (*dispose)(obfs *self);

    int (*client_pre_encrypt)(obfs *self,
            char *plaindata,
            int datalength);
    int (*client_encode)(obfs *self,
            char *encryptdata,
            int datalength);
    int (*client_decode)(obfs *self,
            char *encryptdata,
            int datalength,
            int *needsendback);
    int (*client_post_decrypt)(obfs *self,
            char *plaindata,
            int datalength);
}obfs_class;

obfs_class * new_obfs_class(char *plugin_name);
void free_obfs_class(obfs_class *plugin);

void set_server_info(obfs *self, server_info *server);
obfs * new_obfs();
void dispose_obfs(obfs *self);

#endif // _OBFS_H
