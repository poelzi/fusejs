#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_
#include "node_fuse.h"

namespace NodeFuse {
    class FileSystem {
        public:
            FileSystem();
            virtual ~FileSystem();

            static void Initialize();

            static struct fuse_lowlevel_ops* GetOperations();
            static void Init(void* userdata,
                             struct fuse_conn_info* conn);
            static void Destroy(void* userdata);
            static void Lookup(fuse_req_t req,
                               fuse_ino_t parent,
                               const char* name);
            static void Forget(fuse_req_t req,
                                fuse_ino_t ino,
                                unsigned long nlookup);
            static void GetAttr(fuse_req_t req,
                                fuse_ino_t ino,
                                struct fuse_file_info* fi);
            static void SetAttr(fuse_req_t req,
                                fuse_ino_t ino,
                                struct stat* attr,
                                int to_set,
                                struct fuse_file_info* fi);
            static void ReadLink(fuse_req_t req, fuse_ino_t ino);
            static void MkNod(fuse_req_t req,
                              fuse_ino_t parent,
                              const char* name,
                              mode_t mode,
                              dev_t rdev);
            static void MkDir(fuse_req_t req,
                              fuse_ino_t parent,
                              const char *name,
                              mode_t mode);
            static void Unlink(fuse_req_t req,
                               fuse_ino_t parent,
                               const char* name);
            static void RmDir(fuse_req_t req,
                              fuse_ino_t parent,
                              const char* name);
            static void SymLink(fuse_req_t req,
                                const char* link,
                                fuse_ino_t parent,
                                const char* name);
            static void Rename(fuse_req_t req,
                               fuse_ino_t parent,
                               const char* name,
                               fuse_ino_t newparent,
                               const char* newname);
            static void Link(fuse_req_t req,
                             fuse_ino_t ino,
                             fuse_ino_t newparent,
                             const char* newname);
            static void Open(fuse_req_t req,
                             fuse_ino_t ino,
                             struct fuse_file_info* fi);
            static void Read(fuse_req_t req,
                             fuse_ino_t ino,
                             size_t size,
                             off_t off,
                             struct fuse_file_info *fi);
            static void Write(fuse_req_t req,
                              fuse_ino_t ino,
                              const char *buf,
                              size_t size,
                              off_t off,
                              struct fuse_file_info *fi);
            static void Flush(fuse_req_t req,
                              fuse_ino_t ino,
                              struct fuse_file_info *fi);
            static void Release(fuse_req_t req,
                                fuse_ino_t ino,
                                struct fuse_file_info *fi);
            static void Fsync(fuse_req_t req,
                              fuse_ino_t ino,
                              int datasync,
                              struct fuse_file_info *fi);
            static void Opendir(fuse_req_t req,
                                fuse_ino_t ino,
                                struct fuse_file_info *fi);
            static void Readdir(fuse_req_t req,
                                fuse_ino_t ino,
                                size_t size,
                                off_t off,
                                struct fuse_file_info *fi);
            static void Releasedir(fuse_req_t req,
                                   fuse_ino_t ino,
                                   struct fuse_file_info *fi);
            static void Fsyncdir(fuse_req_t req,
                                 fuse_ino_t ino,
                                 int datasync,
                                 struct fuse_file_info *fi);
            static void Statfs(fuse_req_t req,
                               fuse_ino_t ino);
            static void Setxattr(fuse_req_t req,
                                 fuse_ino_t ino,
                                 const char *name,
                                 const char *value,
                                 size_t size,
                                 int flags);
            static void Getxattr(fuse_req_t req,
                                 fuse_ino_t ino,
                                 const char *name,
                                 size_t size);
            static void Listxattr(fuse_req_t req,
                                  fuse_ino_t ino,
                                  size_t size);
            static void Removexattr(fuse_req_t req,
                                    fuse_ino_t ino,
                                    const char *name);
            static void Access(fuse_req_t req,
                               fuse_ino_t ino,
                               int mask);
            static void Create(fuse_req_t req,
                               fuse_ino_t parent,
                               const char *name,
                               mode_t mode,
                               struct fuse_file_info *fi);
            static void Getlk(fuse_req_t req,
                              fuse_ino_t ino,
                              struct fuse_file_info *fi,
                              struct flock *lock);
            static void Setlk(fuse_req_t req,
                              fuse_ino_t ino,
                              struct fuse_file_info *fi,
                              struct flock *lock,
                              int sleep);
            static void Bmap(fuse_req_t req,
                             fuse_ino_t ino,
                             size_t blocksize,
                             uint64_t idx);
            static void Ioctl(fuse_req_t req,
                              fuse_ino_t ino,
                              int cmd,
                              void *arg,
                              struct fuse_file_info *fi,
                              unsigned *flagsp,
                              const void *in_buf,
                              size_t in_bufsz,
                              size_t out_bufszp);
            //static void(*  poll )(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph)

    };
}//namespace NodeFuse

#endif  // SRC_FILESYSTEM_H
