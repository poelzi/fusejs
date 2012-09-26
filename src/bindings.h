// Copyright 2012, Camilo Aguilar. Cloudescape, LLC.
#ifndef SRC_BINDINGS_H_
#define SRC_BINDINGS_H_
#include "node_fuse.h"
#include "filesystem.h"
#include <stdlib.h>

namespace NodeFuse {
    class Fuse : public ObjectWrap {
        //friend class FileSystem;
        public:
            static void Initialize(Handle<Object> target);
            Persistent<Object> fsobj;

            Fuse();
            virtual ~Fuse();

        protected:
            static Handle<Value> New(const Arguments& args);
            static Handle<Value> Mount(const Arguments& args);
            static Handle<Value> Unmount(const Arguments& args);

        private:
            int multithreaded;
            int foreground;
            char *mountpoint;
            struct fuse_args *fargs;
            struct fuse_chan *channel;
            struct fuse_session *session;
            static Persistent<FunctionTemplate> constructor_template;
    };
}//namespace NodeFuse

// symbol macro
#define FUSE_SYM(name) \
    static Persistent<String> name ## _sym           = NODE_PSYMBOL( #name );

inline unsigned long ValueToUlong(v8::Handle<Value> value) {
        if(value->IsInt32())
          return (unsigned long)value->Int32Value();
        if(value->IsUint32())
          return (unsigned long)value->Uint32Value();

        String::AsciiValue str(value);
        if(str.length())
            return strtoul(*str, NULL, 10);

        return 0;
    }

    
    
#endif  // SRC_BINDINGS_H
