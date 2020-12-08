# gm-lcp
gm-lcp is a collection of GameMonkey Script tools for SpongeBob SquarePants: Lights Camera Pants.

* **compile** compiles a GameMonkey Script source code file into a debug library file, which can be imported into a PAK file.
* **extract** extracts a GameMonkey Script source code file from a debug library file, which is usually exported from a PAK file.

These tools depend on GameMonkey 1.21, which is included and builds as a static library. This library has been modified to support MSVC 2019 and to be able to read and write GameMonkey library files from both the GameCube and PS2 version of Lights Camera Pants. (Xbox is probably supported too, though it has not been tested yet.)

## compile
Usage:
```
compile [-g for gamecube] <input gm source file> <output gm lib file>
```

## extract
Usage:
```
extract <input gm lib file> <output gm source file>
```
