#include "filesystem.h"
#include "reply.h"
#include "file_info.h"
#include "bindings.h"

typedef struct fuse_lowlevel_ops fuops;

namespace NodeFuse {

    static struct fuse_lowlevel_ops fuse_ops = {};
    
    
    static void fill_ops() {
      // c++ does not support designated initializers
        fuse_ops.init    = FileSystem::Init;
        fuse_ops.destroy = FileSystem::Destroy;
        fuse_ops.lookup  = FileSystem::Lookup;
        fuse_ops.forget  = FileSystem::Forget;
        fuse_ops.getattr = FileSystem::GetAttr;
        fuse_ops.setattr = FileSystem::SetAttr;
        fuse_ops.readlink= FileSystem::ReadLink;
        fuse_ops.mknod   = FileSystem::MkNod;
        fuse_ops.mkdir   = FileSystem::MkDir;
        fuse_ops.unlink  = FileSystem::Unlink;
        fuse_ops.rmdir   = FileSystem::RmDir;
        fuse_ops.symlink = FileSystem::SymLink;
        fuse_ops.rename  = FileSystem::Rename;
        fuse_ops.link    = FileSystem::Link;
        fuse_ops.open    = FileSystem::Open;
        fuse_ops.read    = FileSystem::Read;
        
        fuse_ops.statfs  = FileSystem::Statfs;

    };

    //Operations symbols
    static Persistent<String> init_sym      = NODE_PSYMBOL("init");
    static Persistent<String> destroy_sym   = NODE_PSYMBOL("destroy");
    static Persistent<String> lookup_sym    = NODE_PSYMBOL("lookup");
    static Persistent<String> forget_sym    = NODE_PSYMBOL("forget");
    static Persistent<String> getattr_sym   = NODE_PSYMBOL("getattr");
    static Persistent<String> setattr_sym   = NODE_PSYMBOL("setattr");
    static Persistent<String> readlink_sym  = NODE_PSYMBOL("readlink");
    static Persistent<String> mknod_sym     = NODE_PSYMBOL("mknod");
    static Persistent<String> mkdir_sym     = NODE_PSYMBOL("mkdir");
    static Persistent<String> unlink_sym    = NODE_PSYMBOL("unlink");
    static Persistent<String> rmdir_sym     = NODE_PSYMBOL("rmdir");
    static Persistent<String> symlink_sym   = NODE_PSYMBOL("symlink");
    static Persistent<String> rename_sym    = NODE_PSYMBOL("rename");
    static Persistent<String> link_sym      = NODE_PSYMBOL("link");
    static Persistent<String> open_sym      = NODE_PSYMBOL("open");
    FUSE_SYM(statfs);
    FUSE_SYM(read);

    //fuse_conn_info symbols
    //Major version of the fuse protocol
    static Persistent<String> conn_info_proto_major_sym     = NODE_PSYMBOL("proto_major");
    //Minor version of the fuse protocol
    static Persistent<String> conn_info_proto_minor_sym     = NODE_PSYMBOL("proto_minor");
    //Is asynchronous read supported
    static Persistent<String> conn_info_async_read_sym      = NODE_PSYMBOL("async_read");
    //Maximum size of the write buffer
    static Persistent<String> conn_info_max_write_sym       = NODE_PSYMBOL("max_write");
    //Maximum readahead
    static Persistent<String> conn_info_max_readahead_sym   = NODE_PSYMBOL("max_readahead");
    //Capability flags, that the kernel supports
    static Persistent<String> conn_info_capable_sym         = NODE_PSYMBOL("capable");
    //Capability flags, that the filesystem wants to enable
    static Persistent<String> conn_info_want_sym            = NODE_PSYMBOL("want");

    
    void FileSystem::Initialize() {
        fill_ops();
    }

    void FileSystem::Init(void* userdata,
                          struct fuse_conn_info* conn) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(userdata);

        Local<Value> vinit = fuse->fsobj->Get(init_sym);
        Local<Function> init = Local<Function>::Cast(vinit);

        //These properties will be read-only for now.
        //TODO set accessors for read/write properties
        Local<Object> info = Object::New();
        info->Set(conn_info_proto_major_sym, Integer::New(conn->proto_major));
        info->Set(conn_info_proto_minor_sym, Integer::New(conn->proto_minor));
        info->Set(conn_info_async_read_sym, Integer::New(conn->async_read));
        info->Set(conn_info_max_write_sym, Number::New(conn->max_write));
        info->Set(conn_info_max_readahead_sym, Number::New(conn->max_readahead));
        //TODO macro to enable certain properties given the fuse version
        //info->Set(conn_info_capable_sym, Integer::New(conn->capable));
        //info->Set(conn_info_want_sym, Integer::New(conn->want));

        Local<Value> argv[1] = {info};

        TryCatch try_catch;

        init->Call(fuse->fsobj, 1, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Destroy(void* userdata) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(userdata);

        Local<Value> vdestroy = fuse->fsobj->Get(destroy_sym);
        Local<Function> destroy = Local<Function>::Cast(vdestroy);

        TryCatch try_catch;

        destroy->Call(fuse->fsobj, 0, NULL);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Lookup(fuse_req_t req,
                            fuse_ino_t parent,
                            const char* name) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vlookup = fuse->fsobj->Get(lookup_sym);
        Local<Function> lookup = Local<Function>::Cast(vlookup);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);
        Local<String> entryName = String::New(name);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[4] = {context, parentInode,
                                entryName, replyObj};
        TryCatch try_catch;

        lookup->Call(fuse->fsobj, 4, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Forget(fuse_req_t req,
                            fuse_ino_t ino,
                            unsigned long nlookup) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vforget = fuse->fsobj->Get(forget_sym);
        Local<Function> forget = Local<Function>::Cast(vforget);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);
        Local<Integer> nlookup_ = Integer::New(nlookup);

        Local<Value> argv[3] = {context, inode, nlookup_};

        TryCatch try_catch;

        forget->Call(fuse->fsobj, 3, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }

        fuse_reply_none(req);
    }

    void FileSystem::GetAttr(fuse_req_t req,
                             fuse_ino_t ino,
                             struct fuse_file_info* fi) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vgetattr = fuse->fsobj->Get(getattr_sym);
        Local<Function> getattr = Local<Function>::Cast(vgetattr);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[3] = {context, inode, replyObj};

        TryCatch try_catch;

        getattr->Call(fuse->fsobj, 3, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::SetAttr(fuse_req_t req,
                             fuse_ino_t ino,
                             struct stat* attr,
                             int to_set,
                             struct fuse_file_info* fi) {
        HandleScope scope;
        Fuse *fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vsetattr = fuse->fsobj->Get(setattr_sym);
        Local<Function> setattr = Local<Function>::Cast(vsetattr);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);

        Local<Object> attrs = GetAttrsToBeSet(to_set, attr)->ToObject();

        Reply *reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[4] = {context, inode, attrs, replyObj};

        TryCatch try_catch;

        setattr->Call(fuse->fsobj, 4, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::ReadLink(fuse_req_t req, fuse_ino_t ino) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vreadlink = fuse->fsobj->Get(readlink_sym);
        Local<Function> readlink = Local<Function>::Cast(vreadlink);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[3] = {context, inode, replyObj};

        TryCatch try_catch;

        readlink->Call(fuse->fsobj, 3, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::MkNod(fuse_req_t req,
                           fuse_ino_t parent,
                           const char* name,
                           mode_t mode,
                           dev_t rdev) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vmknod = fuse->fsobj->Get(mknod_sym);
        Local<Function> mknod = Local<Function>::Cast(vmknod);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);

        Local<String> name_ = String::New(name);
        Local<Integer> mode_ = Integer::New(mode);
        Local<Integer> rdev_ = Integer::New(rdev);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[6] = {context, parentInode,
                                name_, mode_,
                                rdev_, replyObj};

        TryCatch try_catch;

        mknod->Call(fuse->fsobj, 6, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::MkDir(fuse_req_t req,
                           fuse_ino_t parent,
                           const char* name,
                           mode_t mode) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vmkdir = fuse->fsobj->Get(mkdir_sym);
        Local<Function> mkdir = Local<Function>::Cast(vmkdir);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);

        Local<String> name_ = String::New(name);
        Local<Integer> mode_ = Integer::New(mode);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[5] = {context, parentInode,
                                name_, mode_, replyObj};

        TryCatch try_catch;

        mkdir->Call(fuse->fsobj, 5, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Unlink(fuse_req_t req,
                            fuse_ino_t parent,
                            const char* name) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vunlink = fuse->fsobj->Get(unlink_sym);
        Local<Function> unlink = Local<Function>::Cast(vunlink);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);
        Local<String> name_ = String::New(name);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[4] = {context, parentInode, name_, replyObj};

        TryCatch try_catch;

        unlink->Call(fuse->fsobj, 4, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::RmDir(fuse_req_t req,
                           fuse_ino_t parent,
                           const char* name) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vrmdir = fuse->fsobj->Get(rmdir_sym);
        Local<Function> rmdir = Local<Function>::Cast(vrmdir);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);
        Local<String> name_ = String::New(name);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[4] = {context, parentInode, name_, replyObj};

        TryCatch try_catch;

        rmdir->Call(fuse->fsobj, 4, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::SymLink(fuse_req_t req,
                             const char* link,
                             fuse_ino_t parent,
                             const char* name) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vsymlink = fuse->fsobj->Get(symlink_sym);
        Local<Function> symlink = Local<Function>::Cast(vsymlink);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);
        Local<String> name_ = String::New(name);
        Local<String> link_ = String::New(link);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[5] = {context, parentInode,
                                link_, name_, replyObj};

        TryCatch try_catch;

        symlink->Call(fuse->fsobj, 5, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Rename(fuse_req_t req,
                            fuse_ino_t parent,
                            const char *name,
                            fuse_ino_t newparent,
                            const char *newname) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vrename = fuse->fsobj->Get(rename_sym);
        Local<Function> rename = Local<Function>::Cast(vrename);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> parentInode = Number::New(parent);
        Local<String> name_ = String::New(name);
        Local<Number> newParentInode = Number::New(newparent);
        Local<String> newName = String::New(newname);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[6] = {context, parentInode,
                                name_, newParentInode,
                                newName, replyObj};

        TryCatch try_catch;

        rename->Call(fuse->fsobj, 6, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Link(fuse_req_t req,
                             fuse_ino_t ino,
                             fuse_ino_t newparent,
                             const char* newname) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vlink = fuse->fsobj->Get(link_sym);
        Local<Function> link = Local<Function>::Cast(vlink);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);
        Local<Number> newParent = Number::New(newparent);
        Local<String> newName = String::New(newname);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[5] = {context, inode,
                                newParent, newName, replyObj};

        TryCatch try_catch;

        link->Call(fuse->fsobj, 5, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }

    void FileSystem::Open(fuse_req_t req,
                          fuse_ino_t ino,
                          struct fuse_file_info *fi) {
        HandleScope scope;
        Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req));

        Local<Value> vopen = fuse->fsobj->Get(open_sym);
        Local<Function> open = Local<Function>::Cast(vopen);

        Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();
        Local<Number> inode = Number::New(ino);

        FileInfo* info = new FileInfo();
        info->fi = fi;
        Local<Object> infoObj = info->constructor_template->GetFunction()->NewInstance();
        info->Wrap(infoObj);

        Reply* reply = new Reply();
        reply->request = req;
        Local<Object> replyObj = reply->constructor_template->GetFunction()->NewInstance();
        reply->Wrap(replyObj);

        Local<Value> argv[4] = {context, inode, infoObj, replyObj};

        TryCatch try_catch;

        open->Call(fuse->fsobj, 4, argv);

        if (try_catch.HasCaught()) {
            FatalException(try_catch);
        }
    }


#define NFUSE_GET_FS_FUNC(name) \
    Local<Value> v ## name = fuse->fsobj->Get(name ##_sym); \
    if(! v ## name->IsFunction()) { \
      fuse_reply_err(req, ENOSYS); \
      return;    \
    } \
    Local<Function> name = Local<Function>::Cast(v ## name); \
    
#define NFUSE_NEW_WRAP(cls, name, input) \
    cls* name = new cls(); \
    input ;\
    Local<Object> name ## Obj = name->constructor_template->GetFunction()->NewInstance(); \
    name->Wrap(name ## Obj);
    
#define NFUSE_NEW_REPLY(name) \
    NFUSE_NEW_WRAP(Reply, name, name->request = req)
    
#define NFUSE_NEW_FILEINFO(name, input) \
    NFUSE_NEW_WRAP(FileInfo, name, name->fi = input)
    
    
#define NFUSE_FS_HEAD \
    HandleScope scope; \
    Fuse* fuse = static_cast<Fuse *>(fuse_req_userdata(req)); \
    Local<Object> context = RequestContextToObject(fuse_req_ctx(req))->ToObject();

#define NFUSE_FS_SETUP(name)\
    NFUSE_FS_HEAD; \
    NFUSE_GET_FS_FUNC(name)

#define NFUSE_FS_END(name) \
    TryCatch try_catch; \
    name ->Call(fuse->fsobj, sizeof(argv)/sizeof(argv[0]), argv);\
    if (try_catch.HasCaught()) { \
        FatalException(try_catch); \
    }

    void FileSystem::Read(fuse_req_t req,
                          fuse_ino_t ino,
                          size_t fsize,
                          off_t foff,
                          struct fuse_file_info *fi
                         ) {
        
        NFUSE_FS_SETUP(read);
        
        Local<Number> inode = Number::New(ino);
        Local<Integer> size = Integer::New(fsize);
        Local<Integer> off = Integer::New(foff);
        
        NFUSE_NEW_FILEINFO(info, fi);
        
        NFUSE_NEW_REPLY(reply);
        
        Local<Value> argv[6] = {context, inode, size, off, infoObj, replyObj};

        NFUSE_FS_END(read)
        
    }
    
    void FileSystem::Statfs(fuse_req_t req,
                          fuse_ino_t ino) {
        
        NFUSE_FS_SETUP(statfs);
        
        Local<Number> inode = Number::New(ino);

        NFUSE_NEW_REPLY(reply);
        
        Local<Value> argv[3] = {context, inode, replyObj};

        NFUSE_FS_END(statfs)
        
    }


    struct fuse_lowlevel_ops* FileSystem::GetOperations() {
        return &fuse_ops;
    }
}


