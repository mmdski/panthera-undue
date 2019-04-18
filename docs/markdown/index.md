---
short-description: panthera docs
...

# panthera docs
Open-channel hydraulics library
```
/*
 *
 *               _   _                             _
 *   ___ ___ ___| |_| |_ ___ ___ ___ ___ _ _ ___ _| |_ _ ___
 *  | . | .'|   |  _|   | -_|  _| .'|___| | |   | . | | | -_|
 *  |  _|__,|_|_|_| |_|_|___|_| |__,|   |___|_|_|___|___|___|
 *  |_|
 *                                              Version 0.0.X
 *
 *                         Marian Domanski <mmdski@gmail.com>
 *
 */
```

[panthera GitHub project](https://github.com/mmdski/panthera-undue)

## Status
It's working!
![It's working!](images/normalresults.png)

## Features

### Completed features
* Cross section computations
  * Area
  * Top width
  * Wetted perimeter
  * Hydraulic depth
  * Hydraulic radius
  * Conveyance
  * Velocity coefficient (energy flux correction coefficient)
  * Critical flow
  * Results caching
* Cross section table
* Steady flow
  * Gradually varied
      * Standard step

### Planned features
* Steady flow
  * Normal
  * Gradually varied
    * Single step
    * Simultaneous solution
* Gradually varied unsteady flow
  * Method of characteritics
* Python extensions to major features
* HDF5 storage

## Building and testing (linux)
* Build requirements
  * GCC
  * Meson
  * Ninja (required by Meson)
* Test requirements
  * GLib
  * Valgrind

```
$ git clone https://github.com/mmdski/panthera-undue.git
$ cd panthera-undue
$ meson build
$ ninja -C build test
```

## Implementation notes
A goal of this project is for the code to also be natively compilable on
Windows. As a result, I'm using portable solutions. The only exception so far
is GLib for testing.

### Cross section
The cross section interface caches computation results. This means once a
set of values is calculated for a given depth, the results are saved for the
next time they're needed.

### CII
panthera uses several interfaces and implementations from David Hanson's book
*[C Interfaces and Implementations: Techniques for Creating Reusable Software](
http://www.cs.princeton.edu/software/cii/)*. The CII code in panthera is only
slightly modified, if at all.

### Red-black BST
The cross section table implementation in panthera uses the red-black binary
search tree described in *[Algorithms](https://algs4.cs.princeton.edu/home/)*
by Robert Sedgewick and Kevin Wayne. The code was ported to C from their
[Java implementation](https://algs4.cs.princeton.edu/33balanced/RedBlackBST.java.html).

