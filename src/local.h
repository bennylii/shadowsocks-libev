/*
 * local.h - Define the clinet's buffers and callbacks
 *
 * Copyright (C) 2013 - 2015, Max Lv <max.c.lv@gmail.com>
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

#ifndef _LOCAL_H
#define _LOCAL_H

#include <ev.h>
#include <libcork/ds.h>

#include "encrypt.h"
#include "obfs.h"
#include "jconf.h"

#include "common.h"

struct listen_ctx {
    ev_io io;
    char *iface;
    int remote_num;
    int method;
    int timeout;
    int fd;
    struct sockaddr **remote_addr;

    char *protocol_name;
    char *obfs_name;
    char *obfs_param;
    void *protocol_global;
    void *obfs_global;
};

struct server_ctx {
    ev_io io;
    int connected;
    struct server *server;
};

struct server {
    int fd;
    ssize_t buf_len;
    ssize_t buf_idx;
    char *buf; // server send from, remote recv into
    char stage;
    struct enc_ctx *e_ctx;
    struct enc_ctx *d_ctx;
    struct server_ctx *recv_ctx;
    struct server_ctx *send_ctx;
    struct listen_ctx *listener;
    struct remote *remote;

    struct cork_dllist_item entries;

    obfs *protocol;
    obfs *obfs;
    obfs_class *protocol_plugin;
    obfs_class *obfs_plugin;
};

struct remote_ctx {
    ev_io io;
    ev_timer watcher;
    int connected;
    struct remote *remote;
};

struct remote {
    int fd;
    ssize_t buf_len;
    ssize_t buf_idx;
    int direct;
    char *buf; // remote send from, server recv into
    struct remote_ctx *recv_ctx;
    struct remote_ctx *send_ctx;
    struct server *server;
    struct sockaddr_storage addr;
    int addr_len;
    uint32_t counter;
};

#endif // _LOCAL_H
