#include "filesystem.h"
#include "reply.h"
#include "file_info.h"
#include "bindings.h"
#include "node_buffer.h"

typedef struct fuse_lowlevel_ops fuops;


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

  #define NFUSE_NEW_REPLY(name, allowd) \
  NFUSE_NEW_WRAP(Reply, name, name->request = req)\
  name->allowed = allowd;

  #define NFUSE_NEW_FILEINFO(name, input) \
  NFUSE_NEW_WRAP(FileInfo, name, name->fi = input)

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


namespace NodeFuse {

    static struct fuse_lowlevel_ops fuse_ops = {};


    static void fill_ops() {
      // c++ does not support designated initializers
        fuse_ops.init        = FileSystem::Init;
        fuse_ops.destroy     = FileSystem::Destroy;
        fuse_ops.lookup      = FileSystem::Lookup;
        fuse_ops.forget      = FileSystem::Forget;
        fuse_ops.getattr     = FileSystem::GetAttr;
        fuse_ops.setattr     = FileSystem::SetAttr;
        fuse_ops.readlink    = FileSystem::ReadLink;
        fuse_ops.mknod       = FileSystem::MkNod;
        fuse_ops.mkdir       = FileSystem::MkDir;
        fuse_ops.unlink      = FileSystem::Unlink;
        fuse_ops.rmdir       = FileSystem::RmDir;
        fuse_ops.symlink     = FileSystem::SymLink;
        fuse_ops.rename      = FileSystem::Rename;
        fuse_ops.link        = FileSystem::Link;
        fuse_ops.open        = FileSystem::Open;
        fuse_ops.read        = FileSystem::Read;
        fuse_ops.read        = FileSystem::Read;
        fuse_ops.write       = FileSystem::Write;
        fuse_ops.flush       = FileSystem::Flush;
        fuse_ops.release     = FileSystem::Release;
        fuse_ops.fsync       = FileSystem::Fsync;
        fuse_ops.opendir     = FileSystem::Opendir;
        fuse_ops.readdir     = FileSystem::Readdir;
        fuse_ops.releasedir  = FileSystem::Releasedir;
        fuse_ops.fsyncdir    = FileSystem::Fsyncdir;
        fuse_ops.statfs      = FileSystem::Statfs;
        fuse_ops.setxattr    = FileSystem::Setxattr;
        fuse_ops.getxattr    = FileSystem::Getxattr;
        fuse_ops.listxattr   = FileSystem::Listxattr;
        fuse_ops.removexattr = FileSystem::Removexattr;
        fuse_ops.access      = FileSystem::Access;
        fuse_ops.create      = FileSystem::Create;
        fuse_ops.getlk       = FileSystem::Getlk;
        fuse_ops.setlk       = FileSystem::Setlk;
        fuse_ops.bmap        = FileSystem::Bmap;
        //fuse_ops.ioctl       = FileSystem::Ioctl;


    };

    //Operations symbols
    FUSE_SYM(init);
    FUSE_SYM(destroy);
    FUSE_SYM(lookup);
    FUSE_SYM(forget);
    FUSE_SYM(getattr);
    FUSE_SYM(setattr);
    FUSE_SYM(readlink);
    FUSE_SYM(mknod);
    FUSE_SYM(mkdir);
    FUSE_SYM(unlink);
    FUSE_SYM(rmdir);
    FUSE_SYM(symlink);
    FUSE_SYM(rename);
    FUSE_SYM(link);
    FUSE_SYM(open);
    FUSE_SYM(read);
    FUSE_SYM(write);
    FUSE_SYM(flush);
    FUSE_SYM(release);
    FUSE_SYM(fsync);
    FUSE_SYM(opendir);
    FUSE_SYM(readdir);
    FUSE_SYM(releasedir);
    FUSE_SYM(fsyncdir);
    FUSE_SYM(statfs);
    FUSE_SYM(setxattr);
    FUSE_SYM(getxattr);
    FUSE_SYM(listxattr);
    FUSE_SYM(removexattr);
    FUSE_SYM(access);
    FUSE_SYM(create);
    FUSE_SYM(getlk);
    FUSE_SYM(setlk);
    FUSE_SYM(bmap);
    FUSE_SYM(ioctl);

    //fuse_conn_info symbols
    FUSE_SYM(conn_info_proto_major);
    FUSE_SYM(conn_info_proto_minor);
    FUSE_SYM(conn_info_async_read);
    FUSE_SYM(conn_info_max_write);
    FUSE_SYM(conn_info_max_readahead);
    FUSE_SYM(conn_info_capable);
    FUSE_SYM(conn_info_want);


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
        #if USE_FUSE_VERSION > 25
        info->Set(conn_info_capable_sym, Integer::New(conn->capable));
        info->Set(conn_info_want_sym, Integer::New(conn->want));
        #endif

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
                            fuse_ino_t fparent,
                            const char* fname) {
        NFUSE_FS_SETUP(lookup);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ENTRY);

        Local<Value> argv[3] = {context, parent, name};

        NFUSE_FS_END(lookup)
    }

    void FileSystem::Forget(fuse_req_t req,
                            fuse_ino_t ino,
                            unsigned long nlookup) {
        NFUSE_FS_SETUP(forget);

        Local<Number> inode = Number::New(ino);
        Local<Number> lookup = Number::New(nlookup);

        Local<Value> argv[3] = {context, inode, lookup};

        NFUSE_FS_END(forget)

        fuse_reply_none(req);
    }

    void FileSystem::GetAttr(fuse_req_t req,
                             fuse_ino_t ino,
                             struct fuse_file_info* fi) {
        NFUSE_FS_SETUP(getattr);

        Local<Number> inode = Number::New(ino);

        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ATTR);

        Local<Value> argv[4] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(getattr)
    }

    void FileSystem::SetAttr(fuse_req_t req,
                             fuse_ino_t ino,
                             struct stat* fattr,
                             int to_set,
                             struct fuse_file_info* fi) {
        NFUSE_FS_SETUP(setattr);

        Local<Number> inode = Number::New(ino);
        Local<Object> attrs = GetAttrsToBeSet(to_set, fattr)->ToObject();
        Local<Integer> set = Integer::New(to_set);

        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ATTR);

        Local<Value> argv[6] = {context, inode, attrs, set, infoObj, replyObj};

        NFUSE_FS_END(setattr)
    }

    void FileSystem::ReadLink(fuse_req_t req, fuse_ino_t ino) {
        NFUSE_FS_SETUP(readlink);

        Local<Number> inode = Number::New(ino);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_READLINK);

        Local<Value> argv[3] = {context, inode, replyObj};

        NFUSE_FS_END(readlink)
    }

    void FileSystem::MkNod(fuse_req_t req,
                           fuse_ino_t fparent,
                           const char* fname,
                           mode_t fmode,
                           dev_t rdev) {
        NFUSE_FS_SETUP(mknod);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);
        Local<Integer> mode = Integer::New(fmode);
        Local<Integer> dev = Integer::New(rdev);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ENTRY);

        Local<Value> argv[6] = {context, parent, name, mode, dev, replyObj};

        NFUSE_FS_END(mknod)
    }

    void FileSystem::MkDir(fuse_req_t req,
                           fuse_ino_t fparent,
                           const char* fname,
                           mode_t fmode) {
        NFUSE_FS_SETUP(mkdir);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);
        Local<Integer> mode = Integer::New(fmode);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ENTRY);

        Local<Value> argv[5] = {context, parent, name, mode, replyObj};

        NFUSE_FS_END(mkdir)
    }

    void FileSystem::Unlink(fuse_req_t req,
                            fuse_ino_t fparent,
                            const char* fname) {
        NFUSE_FS_SETUP(unlink);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, parent, name, replyObj};

        NFUSE_FS_END(unlink);
  }

    void FileSystem::RmDir(fuse_req_t req,
                           fuse_ino_t fparent,
                           const char* fname) {
        NFUSE_FS_SETUP(rmdir);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, parent, name, replyObj};

        NFUSE_FS_END(rmdir)
    }

    void FileSystem::SymLink(fuse_req_t req,
                             const char* flink,
                             fuse_ino_t parent,
                             const char* fname) {
        NFUSE_FS_SETUP(symlink);

        Local<String> link = String::New(flink);
        Local<Number> nparent = Number::New(parent);
        Local<String> name = String::New(fname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ENTRY);

        Local<Value> argv[5] = {context, link, nparent, name, replyObj};

        NFUSE_FS_END(symlink)
    }

    void FileSystem::Rename(fuse_req_t req,
                            fuse_ino_t fparent,
                            const char *fname,
                            fuse_ino_t newparent,
                            const char *newname) {
        NFUSE_FS_SETUP(rename);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);
        Local<Number> nparent = Number::New(newparent);
        Local<String> nname = String::New(newname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[6] = {context, parent, name, nparent, nname, replyObj};

        NFUSE_FS_END(rename)

    }

    void FileSystem::Link(fuse_req_t req,
                             fuse_ino_t ino,
                             fuse_ino_t newparent,
                             const char* newname) {

        NFUSE_FS_SETUP(link);

        Local<Number> inode = Number::New(ino);
        Local<Number> nparent = Number::New(newparent);
        Local<String> name = String::New(newname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_ENTRY);

        Local<Value> argv[6] = {context, inode, nparent, name, replyObj};

        NFUSE_FS_END(link)
    }

    void FileSystem::Open(fuse_req_t req,
                          fuse_ino_t ino,
                          struct fuse_file_info *fi) {

        NFUSE_FS_SETUP(open);

        Local<Number> inode = Number::New(ino);

        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_OPEN);

        Local<Value> argv[4] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(open)
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

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_BUF);

        Local<Value> argv[6] = {context, inode, size, off, infoObj, replyObj};

        NFUSE_FS_END(read)
    }
    void FileSystem::Write(fuse_req_t req,
                      fuse_ino_t ino,
                      const char *buf,
                      size_t fsize,
                      off_t foff,
                      struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(write);

        Local<Number> inode = Number::New(ino);

        // thats safe cast as Buffer make a copy
        Buffer* rbuffer = Buffer::New((char *)buf, fsize);
        Local<Object> buffer = Local<Object>::New(rbuffer->handle_);

        Local<Integer> off = Integer::New(foff);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_WRITE);

        Local<Value> argv[6] = {context, inode, buffer, off, infoObj, replyObj};

        NFUSE_FS_END(write)

    }

    void FileSystem::Flush(fuse_req_t req,
                      fuse_ino_t ino,
                      struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(flush);

        Local<Number> inode = Number::New(ino);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[6] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(flush)

    }

    void FileSystem::Release(fuse_req_t req,
                        fuse_ino_t ino,
                        struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(release);

        Local<Number> inode = Number::New(ino);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(release)
    }

    void FileSystem::Fsync(fuse_req_t req,
                      fuse_ino_t ino,
                      int datasync,
                      struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(fsync);

        Local<Number> inode = Number::New(ino);
        Local<Number> sync = Number::New(datasync);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[5] = {context, inode, sync, infoObj, replyObj};

        NFUSE_FS_END(fsync)
    }

    void FileSystem::Opendir(fuse_req_t req,
                        fuse_ino_t ino,
                        struct fuse_file_info *fi) {

        NFUSE_FS_SETUP(opendir);

        Local<Number> inode = Number::New(ino);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_OPEN);

        Local<Value> argv[5] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(opendir)
    }

    void FileSystem::Readdir(fuse_req_t req,
                        fuse_ino_t ino,
                        size_t fsize,
                        off_t foff,
                        struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(readdir);

        Local<Number> inode = Number::New(ino);
        Local<Number> size = Number::New(fsize);
        Local<Number> off = Number::New(foff);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_BUF);

        Local<Value> argv[6] = {context, inode, size, off, infoObj, replyObj};

        NFUSE_FS_END(readdir)

    }
    void FileSystem::Releasedir(fuse_req_t req,
                            fuse_ino_t ino,
                            struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(readdir);

        Local<Number> inode = Number::New(ino);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, inode, infoObj, replyObj};

        NFUSE_FS_END(readdir)

    }
    void FileSystem::Fsyncdir(fuse_req_t req,
                          fuse_ino_t ino,
                          int datasync,
                          struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(fsyncdir);

        Local<Number> inode = Number::New(ino);
        Local<Number> sync = Number::New(datasync);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[5] = {context, inode, sync, infoObj, replyObj};

        NFUSE_FS_END(fsyncdir)
    }
    void FileSystem::Statfs(fuse_req_t req,
                          fuse_ino_t ino) {

        NFUSE_FS_SETUP(statfs);

        Local<Number> inode = Number::New(ino);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_STATFS);

        Local<Value> argv[3] = {context, inode, replyObj};

        NFUSE_FS_END(statfs)

    }

    void FileSystem::Setxattr(fuse_req_t req,
                          fuse_ino_t ino,
                          const char *fname,
                          const char *fvalue,
                          size_t fsize,
                          int fflags) {
        NFUSE_FS_SETUP(setxattr);

        Local<Number> inode = Number::New(ino);
        Local<String> name = String::New(fname);
        Local<String> value = String::New(fvalue);
        Local<Number> size = Number::New(fsize);
        Local<Integer> flags = Integer::New(fflags);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[7] = {context, inode, name, value, size, flags, replyObj};

        NFUSE_FS_END(setxattr)
    }

    void FileSystem::Getxattr(fuse_req_t req,
                          fuse_ino_t ino,
                          const char *fname,
                          size_t fsize) {
        NFUSE_FS_SETUP(getxattr);

        Local<Number> inode = Number::New(ino);
        Local<String> name = String::New(fname);
        Local<Number> size = Number::New(fsize);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_BUF | REPLY_XATTR);

        Local<Value> argv[5] = {context, inode, name, size, replyObj};

        NFUSE_FS_END(getxattr)
    }
    void FileSystem::Listxattr(fuse_req_t req,
                          fuse_ino_t ino,
                          size_t fsize) {
        NFUSE_FS_SETUP(setxattr);

        Local<Number> inode = Number::New(ino);
        Local<Integer> size = Integer::New(fsize);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_BUF | REPLY_XATTR);

        Local<Value> argv[4] = {context, inode, size, replyObj};

        NFUSE_FS_END(setxattr)

    }
    void FileSystem::Removexattr(fuse_req_t req,
                            fuse_ino_t ino,
                            const char *fname) {
        NFUSE_FS_SETUP(removexattr);

        Local<Number> inode = Number::New(ino);
        Local<String> name = String::New(fname);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, inode, name, replyObj};

        NFUSE_FS_END(removexattr)
    }
    void FileSystem::Access(fuse_req_t req,
                        fuse_ino_t ino,
                        int fmask) {
        NFUSE_FS_SETUP(access);

        Local<Number> inode = Number::New(ino);
        Local<Integer> mask = Integer::New(fmask);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[4] = {context, inode, mask, replyObj};

        NFUSE_FS_END(access)

    }
    void FileSystem::Create(fuse_req_t req,
                        fuse_ino_t fparent,
                        const char *fname,
                        mode_t fmode,
                        struct fuse_file_info *fi) {
        NFUSE_FS_SETUP(create);

        Local<Number> parent = Number::New(fparent);
        Local<String> name = String::New(fname);
        Local<Integer> mode = Integer::New(fmode);
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR | REPLY_CREATE);

        Local<Value> argv[6] = {context, parent, name, mode, infoObj, replyObj};

        NFUSE_FS_END(create)

    }

    void FileSystem::Getlk(fuse_req_t req,
                      fuse_ino_t ino,
                      struct fuse_file_info *fi,
                      struct flock *flock) {
        NFUSE_FS_SETUP(getlk);

        Local<Number> inode = Number::New(ino);
        Local<Object> lock = FlockToObject(flock)->ToObject();
        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_LOCK | REPLY_ERR);

        Local<Value> argv[5] = {context, inode, lock, infoObj, replyObj};

        NFUSE_FS_END(getlk)
    }
    void FileSystem::Setlk(fuse_req_t req,
                      fuse_ino_t ino,
                      struct fuse_file_info *fi,
                      struct flock *flock,
                      int fsleep) {
        NFUSE_FS_SETUP(setlk);

        Local<Number> inode = Number::New(ino);
        Local<Object> lock = FlockToObject(flock)->ToObject();
        Local<Integer> sleep = Integer::New(fsleep);

        NFUSE_NEW_FILEINFO(info, fi);

        NFUSE_NEW_REPLY(reply, REPLY_ERR);

        Local<Value> argv[6] = {context, inode, lock, sleep, infoObj, replyObj};

        NFUSE_FS_END(setlk)

    }

    void FileSystem::Bmap(fuse_req_t req,
                      fuse_ino_t ino,
                      size_t fblocksize,
                      uint64_t fidx) {
        NFUSE_FS_SETUP(bmap);

        Local<Number> inode = Number::New(ino);
        Local<Integer> blocksize = Integer::New(fblocksize);
        Local<Number> idx = Number::New(fidx);

        NFUSE_NEW_REPLY(reply, REPLY_BMAP | REPLY_ERR);

        Local<Value> argv[5] = {context, inode, blocksize, idx, replyObj};

        NFUSE_FS_END(bmap)

    }
/*
    void FileSystem::Ioctl(fuse_req_t req,
                      fuse_ino_t ino,
                      int fcmd,
                      void *arg,
                      struct fuse_file_info *fi,
                      unsigned *flagsp,
                      const void *in_buf,
                      size_t in_bufsz,
                      size_t out_bufszp) {
        NFUSE_FS_SETUP(ioctl);

        Local<Number> inode = Number::New(ino);
        Local<Integer> cmd = Integer::New(fcmd);
        Local<Integer> cmd = Integer::New(fcmd);

        Local<Number> idx = Number::New(fidx);

        NFUSE_NEW_REPLY(reply);

        Local<Value> argv[5] = {context, inode, blocksize, idx, replyObj};

        NFUSE_FS_END(ioctl)

    }
*/

    struct fuse_lowlevel_ops* FileSystem::GetOperations() {
        return &fuse_ops;
    }
}




