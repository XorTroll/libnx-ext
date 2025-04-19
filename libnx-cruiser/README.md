# libnx-cruiser

**libnx-cruiser** is a Nintendo Switch homebrew C (libnx-based) library that allows homebrew to use the [shared WKC (WebKit) libraries used by the browser-applets](https://switchbrew.org/wiki/Internet_Browser#OSS).

## Why? How?

Because it's fun, and because it's possible (as the existence of this repo already should suggest).

Loading NROs as actual dynamic libraries (as opposed to homebrew NROs) is more than possible (check [libnx-ld](../libnx-ld/) for that) so the actual "fun part" in this project is to provide all symbols that these libraries rely on: around half of them are standard C/C++ symbols (more or less straightforward to forward or to mimic) while the rest are Nintendo SDK functions: these are not necessarily straightforward to implement due to the different types used in libnx (for threads, mutexes and so on) but its duable regardless.

## Modules

The following modules are provided in the shared data:

- `oss_wkc`

- `peer_wkc`

- `webkit_wkc`

- `libfont`

- `cairo_wkc`

## Functionality

These libraries abstract/perform the following steps to load the shared WKC modules:

- [BrowserDll shared data](http://switchbrew.org/wiki/Title_list#System_Data_Archives) is mounted as RomFS

- The modules are loaded: this implies decompressing the LZ4-compressed NROs and then relying on [libnx-ld](../libnx-ld/)

- Nontrivial-to-implement symbols (that WKC modules rely on, mostly wrappers between libnx and Nintendo SDK functions) are implemented

- Relevant WKC symbols are looked up for you to access them easily (while still exposing lookup code for other functions you may wish to use)

## Building

This project relies on [devKitPro](https://devkitpro.org/) to be compiled, as well as `switch-lz4` libraries (used for LZ4 NRO decompressing).

TODO: (...)

## TODO

- Find a better way to force-export all stdc/stdcpp symbols when compiling

- Test this for older versions than `19.0.1`: BrowserDll NRO locations are different, and the Nintendo SDK/stdc/stdcpp symbols they rely on will be different as well

- Test more of the functionality provided by these modules (so far basic JavaScriptCore has been tested)

- Come up with some cool project to use this library: perhaps a proof-of-concept simple web browser using WKC with homebrew graphics code?

## Credits

- [libnx](https://github.com/switchbrew/libnx) as the core homebrew library

- [ninupdates reports](https://yls8.mtheall.com/ninupdates/reports.php) for providing explicit diffs/listings of file differences in BrowserDll data for each firmware version

- [switch-oss](https://github.com/reswitched/switch-oss) for collecting the open-sourced part of the WKC modules, which were really helpful for finding function and type definitions
