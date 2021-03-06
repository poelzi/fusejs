/**
 * Loopback filesystem
 **/
var FileSystem = require('../fuse').FileSystem;
var PosixError = require('../fuse').PosixError;

var util = require('util');

var Loopback = function(options) {
    //this.fuse = fuse;
    this.options = options;

    console.log(options);
    FileSystem.call(this);
};

util.inherits(Loopback, FileSystem);

(function() {
    this.init = function(connInfo) {
        console.log(connInfo);
        console.log('Initializing Loopback filesystem!!');
        console.log(this.options);
    };

    this.destroy = function() {
        console.log('Cleaning up filesystem...');
    };

    this.lookup = function(context, parent, name, reply) {
        console.log('Lookup!');
        console.log(context);
        console.log(name);
        var entry = {
            inode: 1234,
            generation: 2,
            attr: {
                dev: 234881026,
                ino: 13420595,
                mode: 33188,
                nlink: 1,
                uid: 501,
                gid: 20,
                rdev: 0,
                size: 11,
                blksize: 4096,
                blocks: 8,
                atime: 1331780451475, //Date.now();
                mtime: 1331780451475, //Date.now();
                ctime: 1331780451475, //Date.now();
            },
            attr_timeout: 30, //in seconds
            entry_timeout: 60 //in seconds
        };

        reply.entry(entry);
        //reply.err(PosixError.ENOENT);
    };

    this.forget = function(context, inode, nlookup) {
        console.log('Forget was called!!');
    };

    this.getattr = function(context, inode, reply) {
        console.log('Getattr was called!!');
        console.log(context);
        console.log(inode);

        var hello = "Hello World!\n";
        //stat object
        var attrs = {
            //dev: 0,
            ino: 1,
            mode: 16877,
            nlink: 1,
            //uid: 501,
            //gid: 20,
            //rdev: 0,
            size: hello.length,
            //blksize: 4096,
            //blocks: 8,
            //atime: 1331780451475, //Date.now();
            //mtime: 1331780451475, //Date.now();
            //ctime: 1331780451475, //Date.now();
        };
        reply.attr(attrs, 1000);
        //reply.err(PosixError.EIO);
    };

    this.setattr = function(context, inode, attrs, reply) {
        console.log('Setattr was called!!');
        console.log(attrs);

        //reply.attr(attrs, 1000);
        reply.err(PosixError.EIO);
    };

    this.readlink = function(context, inode, reply) {
        console.log('Readlink was called!');
        //reply.readlink('eso');
        reply.err(PosixError.EIO);
    };

    this.mknod = function(context, parent, name, mode, rdev, reply) {
        console.log('Mknod was called!');
        reply.err(PosixError.ENOENT);
        //reply.entry(entry);
    };

    this.mkdir = function(context, parent, name, mode, reply) {
        console.log('Mkdir was called!');
        reply.err(PosixError.EIO);
        //reply.entry(entry);
    };

    this.unlink = function(context, parent, name, reply) {
        console.log('Unlink was called!');
        reply.err(PosixError.EIO);
    };

    this.rmdir = function(context, parent, name, reply) {
        console.log('Rmdir was called!');
        reply.err(0);
    };

    this.symlink = function(context, parent, link, name, reply) {
        console.log('Symlink was called!');
        reply.err(0);
        //reply.entry(entry);
    };

    this.rename = function(context, parent, name, newParent, newName, reply) {
        console.log('Rename was called!');
        reply.err(0);
        //reply.err(PosixError.EIO);
    };

    this.link = function(context, inode, newParent, newName, reply) {
        console.log('Link was called!');
        reply.err(PosixError.EIO);
        //reply.entry(entry);
    };

    this.open = function(context, inode, fileInfo, reply) {
        console.log('Open was called!');
        reply.err(0);
        //reply.open();
    };

    this.read = function() {

    };

    this.write = function() {

    };

    this.flush = function() {

    };

    this.release = function() {

    };

    this.fsync = function() {

    };

    this.opendir = function() {

    };

    this.readdir = function() {

    };

    this.releasedir = function() {

    };

    this.fsyncdir = function() {

    };

    this.statfs = function() {

    };

    this.setxattr = function() {

    };

    this.getxattr = function() {

    };

    this.listxattr = function() {

    };

    this.removexattr = function() {

    };

    this.access = function() {

    };

    this.create = function() {

    };

    this.getlk = function() {

    };

    this.setlk = function() {

    };

    this.bmap = function() {

    };

    this.ioctl = function() {

    };

    this.poll = function() {

    };
}).call(Loopback.prototype);

module.exports = Loopback;
