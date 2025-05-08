# libnx extras

This repository contains some custom extensions to [libnx Nintendo Switch C homebrew library](https://github.com/switchbrew/libnx) in the form of additional libraries.

## Extras

- [libnx-ext](libnx-ext/): common base libraries for the other extensions

- [libnx-ld](libnx-ld/): support for loading NRO modules as run-time dynamic libraries

- [libnx-cruiser](libnx-cruiser/): support for using the shared WKC (WebKit) NRO libraries from homebrew (used by the browser applets)

- [libnx-ipcext](libnx-ipcext/): IPC for services/features/command not (yet?) covered in base libnx

## Examples

- [cruiser-example](examples/cruiser-example/): example of using WKC from homebrew

Check out each of their READMEs for more details.

## Credits

- [libnx](https://github.com/switchbrew/libnx) for the base of these libraries, and for some used code snippets (like service guard code).
