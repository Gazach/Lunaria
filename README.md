# ☀️ Lunaris

A simple interpreted programming language written in C.

## About

Lunaris (shortened to `Lun` for command-line use) is a hand-crafted interpreted language built from scratch in C17. There is no grand purpose behind it — it exists purely as a learning exercise to explore how programming languages are designed and implemented.

## Motivation

No real-world goal. Just the joy of building something from nothing and learning along the way.

## Inspiration

- [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom
- [Z#](https://youtu.be/JP9n5wHyemU?si=PBMky-VeUWTva64V) by AstroSam

## Reference

- [Lox Language](https://craftinginterpreters.com/the-lox-language.html) — the language described in *Crafting Interpreters*, which served as a structural reference

## Features

[Later]

## Building

Requires [CMake](https://cmake.org/) 3.22+ and a C17-compatible compiler.

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
