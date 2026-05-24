# ☀️ Lunaris

A simple interpreted programming language written in C.

## About

Lunaris (shortened to `Lun` for command-line use) is a hand-crafted interpreted language built from scratch in C17. There is no grand purpose behind it — it exists purely as a learning exercise to explore how programming languages are designed and implemented.

## Motivation

No real-world goal. Just the joy of building something from nothing and learning along the way.

## Inspiration

- [Lox Language](https://craftinginterpreters.com/the-lox-language.html) — the language described in *Crafting Interpreters*, which served as a structural reference
- [Z#](https://youtu.be/JP9n5wHyemU?si=PBMky-VeUWTva64V) by AstroSam

## Reference

- [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom

## Features

[Later]

## Building

Requires :

- [CMake](https://cmake.org/) 3.22+ and a C17-compatible compiler.
- [ninja](https://ninja-build.org/) 1.12.1+
- [Clang](https://clang.llvm.org/) version 21.1.0+ (recommended)

```bat
build.bat release
```

The compiled binary (`Lun.exe`) will be placed in the `build/release/` directory.

## Usage

**Run a file:**
```
Lun script.lun
```

**Start the REPL:**
```
Lun
```

## Target Platform

x64 Windows

## License

This project is for personal learning. No license applied. Feel free to use it :D
