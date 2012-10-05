// Copyright 2012, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_NODE_FUSE_H_
#define SRC_NODE_FUSE_H_

#include <node.h>
#include <fuse_lowlevel.h>
#include <errno.h>
#include <string.h>

#define NODE_FUSE_VERSION "0.0.1"

using namespace v8;
using namespace node;

#define THROW_IF_MISSING_PROPERTY(obj, symbol, name)                                \
    if (!obj->Has(symbol)) {                                                        \
        return v8::ThrowException(v8::Exception::TypeError(                         \
        v8::String::New("You must have set the property " #name " in the object")));\
    }                                                                               \

#define FUSEJS_THROW_EXCEPTION(err, fuse_err)                                       \
        v8::Local<v8::Value> exception = v8::Exception::Error(                      \
        v8::String::Concat(v8::String::New(err), v8::String::New(fuse_err)));        \
        ThrowException(exception);

namespace NodeFuse {
    enum {
      REPLY_ERR      = 1<<0,
      REPLY_NONE     = 1<<1,
      REPLY_ENTRY    = 1<<2,
      REPLY_CREATE   = 1<<3,
      REPLY_ATTR     = 1<<4,
      REPLY_READLINK = 1<<5,
      REPLY_OPEN     = 1<<6,
      REPLY_WRITE    = 1<<7,
      REPLY_BUF      = 1<<8,
      REPLY_IOV      = 1<<9,
      REPLY_STATFS   = 1<<10,
      REPLY_XATTR    = 1<<11,
      REPLY_LOCK     = 1<<12,
      REPLY_BMAP     = 1<<13,
    };


    int ObjectToFuseEntryParam(Handle<Value> value, struct fuse_entry_param* entry);
    int ObjectToStat(Handle<Value> value, struct stat* stat);
    int ObjectToStatVfs(Handle<Value> value, struct statvfs* statbuf);
    int ObjectToFlock(Handle<Value> value, struct flock* lock);
    Handle<Value> FuseEntryParamToObject(const struct fuse_entry_param* entry);
    Handle<Value> RequestContextToObject(const struct fuse_ctx* ctx);
    Handle<Value> GetAttrsToBeSet(int to_set, struct stat* stat);
    Handle<Value> FlockToObject(struct flock* lock);
}

#endif  // SRC_NODE_FUSE_H_

