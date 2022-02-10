# Installing Pony

Prebuilt Pony binaries are available on a number of platforms. They are built using a very generic CPU instruction set and as such, will not provide maximum performance. If you need to get the best performance possible from your Pony program, we strongly recommend [building from source](BUILD.md).

All prebuilt releases are currently AMD64 only. If you want to install on different CPU architecture, you'll need to [build from source](BUILD.md).

Prebuilt Pony installations will use clang as the default C compiler and clang++ as the default C++ compiler. If you prefer to use different compilers, such as gcc and g++, these defaults can be overridden by setting the `$CC` and `$CXX` environment variables to your compiler of choice.

## FreeBSD 13.0

Prebuilt FreeBSD 13.0 packages are available via [ponyup](https://github.com/ponylang/ponyup).

```bash
ponyup default freebsd-13.0
ponyup update ponyc release
```

## Linux

Prebuilt Linux packages are available via [ponyup](https://github.com/ponylang/ponyup) for Glibc and musl libc based Linux distribution. You can install nightly builds as well as official releases using ponyup.

### Select your Linux platform

```bash
ponyup default PLATFORM
```

where `PLATFORM` is from the table below

Distribution | PLATFORM String
--- | ---
Alpine | musl
Linux Mint 19.3 | ubuntu18.04
Rocky 8 | rocky8
Ubuntu 18.04 | ubuntu18.04
Ubuntu 20.04 | ubuntu20.04

N.B. If you platform isn't listed, skip to the next section and ponyup will install, as appropriate a Glibc or musl libc build of ponyc.

### Install the latest release

```bash

ponyup update ponyc release
```

### Additional requirements

All ponyc Linux installations need to have a C compiler such as clang installed. Compilers other than clang might work, but clang is the officially supported C compiler. The following distributions have additional requirements:

Distribution | Requires
--- | ---
Alpine | libexecinfo
CentOS | libatomic
Fedora | libatomic
Rocky | libatomic
Void | libatomic libatomic-devel

### Troubleshooting Glibc compatibility

Most Linux distributions are based on Glibc and all software for them must use the same version of Glibc. You might see an error like the following when trying to use ponyc:

```console
ponyc: /lib/x86_64-linux-gnu/libm.so.6: version `GLIBC_2.29' not found (required by ponyc)
```

If you get that error, it means that the Glibc we compiled ponyc with isn't compatible with your distribution. If your distribution is a long term support release, please [open an issue](https://github.com/ponylang/ponyc/issues) and we'll work towards adding prebuilt images for your distribution. Otherwise, you'll have to [build ponyc from source](BUILD.md).

## macOS

Prebuilt macOS for Intel packages are available via [ponyup](https://github.com/ponylang/ponyup). You can also install nightly builds using ponyup.

To install the most recent ponyc on macOS for Intel:

```bash
ponyup update ponyc release
```

At this time, we don't offer prebuilt Pony binaries for M1 Apple Silicon. You will need to build Pony [from source](BUILD.md).

## Windows

Windows users will need to install:

- Visual Studio 2019 or 2017 (available [here](https://www.visualstudio.com/vs/community/)) or the Visual C++ Build Tools 2019 or 2017 (available [here](https://visualstudio.microsoft.com/visual-cpp-build-tools/)).
  - If using Visual Studio, install the `Desktop Development with C++` workload.
  - If using Visual C++ Build Tools, install the `Visual C++ build tools` workload, and the `C++ core features` individual component.
  - Install the latest `Windows 10 SDK (10.x.x.x) for Desktop` component.

Once you have installed the prerequisites, you can download the latest ponyc release from [Cloudsmith](https://dl.cloudsmith.io/public/ponylang/releases/raw/versions/latest/ponyc-x86-64-pc-windows-msvc.zip).

Unzip the release file in a convenient location, and you will find `ponyc.exe` in the `bin` directory. Following extraction, to make `ponyc.exe` globally available, add it to your `PATH` either by using Advanced System Settings->Environment Variables to extend `PATH` or by using the `setx` command, e.g. `setx PATH "%PATH%;<directory you unzipped to>\bin"`
