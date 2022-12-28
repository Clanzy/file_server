# File server

## Description

Socket file server & client. You can upload and download files to and from server, delete them or request a list of files that are currently stored. Server can handle multiple client simultaneously, taking each connection to a separate process via `fork()`.

## Compilation

```console
mkdir build
cd build
cmake ..
```

Requires Linux with C11 and CMake.
