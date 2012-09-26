{
  "targets": [
    {
      "target_name": "fuse",
      "type": "<(library)",
      "sources": [ 
        "src/bindings.cc",
        "src/file_info.cc",
        "src/filesystem.cc",
        "src/node_fuse.cc",
        "src/reply.cc"
      ],
        'conditions': [
          ['OS=="win"', {
            'cflags': [
              '/WX',
            ],
          }, { # OS != "win"
            'cflags': [
              '<!@(pkg-config --cflags fuse)',
              '-DFUSE_USE_VERSION=26'
            ],
          }],
        ],
      'link_settings': {
        'ldflags': [
          '<!@(pkg-config --libs-only-L --libs-only-other fuse)',
        ],
        'libraries': [
          '<!@(pkg-config --libs-only-l fuse)',
        ],
      },
    },
    
  ]
}
