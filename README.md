# libdhscan

The Delilah Scanline Renderer (dhscan) is a simple but flexible scanline rendering library written in C.

## 1. Overview

The whole library is contained within the `dhscan.h` and `dhscan.c` source files.  To use the library, you create a `DHSCAN_RENDER` object.  This object requires the following information:

- Output image dimensions in pixels
- Buffer filled with triangles
- Buffer filled with vertices
- Accessor functions
- Pixel size
- Pixel copy function
- Pixel mixer function

The data in the triangle buffer and the data in the vertex buffer may have any format, providing that each triangle has the same size in bytes and each vertex has the same size in bytes.

The client provides the count of structures and the structure size in bytes for both buffers.  It then provides accessor functions, which are callbacks that are used to read information from structures in the buffer.  This architecture is similar to how the generic `qsort()` and `bsearch()` routines are implemented in the C standard library.

The accessor functions are as follows:

- Read three vertex indices from a triangle
- Read projected X and Y coordinates from a vertex
- Read Z coordinate from a vertex

The vertex indices are element offsets into the vertex buffer.  The projected X and Y coordinates must be integers, though they can be signed and have any value.  The X and Y coordinates must be in the proper coordinate space of the output image.  The Z coordinate is a floating-point value that is used to determine visibility when triangles overlap.  For 2D rendering applications with no significant overlap, the accessor function for the Z coordinate can just return a constant value.

The pixel size is the size in bytes of each pixel structure in the output buffer.  Although it is called a "pixel," this structure does not need to encode a color.  Instead, a pixel copy function and a pixel mixer function are provided by the client.  The pixel copy function takes a buffer vertex structure as input and derives an encoded pixel structure from it.  The pixel mixer function two pixel structures as input along with an interpolation value, and then generates a new pixel structure that is linearly interpolated between the two given input structures.

This open-ended definition of a "pixel" allows the Delilah Scanline Renderer to be used not just for rendering colors, but also for rendering any kind of data that can be linearly interpolated.

Once the `DHSCAN_RENDER` object is set up, it can render each scanline in top-to-bottom order.

For the details of how to do everything described in this overview, see the `dhscan.h` header file.

## 2. Test program

The `dhrender.c` program is provided, which is a test program that can render RGB triangles.  This program is not part of the core `dhscan` library.  It is intended for testing the scanline renderer and demonstrating how to use it in practice.

The `dhrender.c` program has the following dependencies.  __These dependencies are only from the test program, not for the Delilah Scanline Renderer!__

1. [libshastina](http://www.purl.org/canidtech/r/shastina) version 0.9.2 beta or compatible.
2. [libsophistry](http://www.purl.org/canidtech/r/libsophistry) version 0.5.3 beta or compatible.

The test program has the following syntax:

    dhrender [out] [script]

`[out]` is the path to a PNG file to render as output.  It must have a PNG file extension.  It will be overwritten if it already exists.

`[script]` is the path to a Shastina script that defines what will be rendered.  It is a [Shastina](http://www.purl.org/canidtech/r/shastina) dialect with the following format:

    %dhrender;
    %dim 1920 1080;
    # Render a 1920 x 1080 image

    # Define three vertices
    # Parameter 1: X coordinate
    # Parameter 2: Y coordinate
    # Parameter 3: Z coordinate
    # Parameter 4: RGB color as six base-16 digits in {}
    -10 257  5 {6d7800} v
     0   20  9 {ff00ff} v
    37  800 -2 {888877} v

    # Only signed integers and RGB colors in {} are supported!

    # Define a triangle
    # Each integer is vertex index
    # Zero is first vertex
    0 1 2 t

    |;

The script will be interpreted and then the output will be rendered to the PNG file.  RGB color mixing is not particularly accurate since this is only intended to be a testing and demonstration program.

## 3. Compilation

The main `libdhscan` library has no dependencies, except certain platforms may require the math library with a `-lm` switch.  To build a static library using GCC, you can do the following:

    gcc -O2 -c dhscan.c
    ar rcs libdhscan.a dhscan.o

The static library will then be in `libdhscan.a` and the intermediate file `dhscan.o` can be deleted.  You will also need the header file `dhscan.h` both to build the library and to use it from a client program.

To build the test program, suppose that `/dir/include` has the header file for [libshastina](http://www.purl.org/canidtech/r/shastina) and [libsophistry](http://www.purl.org/canidtech/r/libsophistry), and that `/dir/lib` has the library files for libshastina and libsophistry.  Also suppose that the development libraries for `libpng` are installed on the system and registered with `pkg-config`.  Then, you can run the following GCC invocation in the root directory of the `libdhscan` project to build the test program (all on one line):

    gcc -O2 -o dhrender
      -I/dir/include
      -L/dir/lib
      `pkg-config --cflags libpng`
      dhrender.c
      dhscan.c
      -lshastina
      -lsophistry
      -lm
      `pkg-config --libs libpng`
