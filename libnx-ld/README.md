# libnx-ld

**libnx-ld** is a Nintendo Switch homebrew C (libnx-based) library that allows homebrew to dynamically load NRO libraries (as they are actually used by official Nintendo SDK).

## Why? How?

The idea started with [libtransistor]()'s simple support for this feature, and the goal of adapting it to work with libnx.

Is it useful? It depends. For this case, these libraries were made as a necessity for [libnx-hwkc](../libnx-hwkc/) to be a thing, but they are standalone here so that other devs can use them for other purposes.

Regular homebrew probably won't benefit a lot from this, since statically-linked homebrew NROs (not to be confused with NROs compiled as shared libraries) are already well-established and work as expected. However, this is here for any special need someone might have ;)

## Functionality

The NROs are loaded using [RO services](https://switchbrew.org/wiki/RO_services): this implies generating [fake NRRs](https://switchbrew.org/wiki/NRR) for each of them on-the-fly loading them via `ldr:ro`, and handling all required relocations and symbol-resolving

NROs could alternatively be loaded manually using SVCs directly (as [nx-hbloader](https://github.com/switchbrew/nx-hbloader) does with homebrew NROs, and as libtransistor also supported) but RO works just fine and it's less complicated to implement here (it basically abstracts a lot of the manual handling).

## Building

This project relies on [devKitPro](https://devkitpro.org/) to be compiled.

TODO: (...)

## Credits

- [libnx](https://github.com/switchbrew/libnx) as the core homebrew library, and specially for its RELR relocating code

- [switchbrew](https://switchbrew.org/wiki/Main_Page) for detailed docs about services and NRO/NRR formats

- [oss-rtld](https://github.com/marysaka/oss-rtld) as the basis for most of the relocating and symbol-resolving code
