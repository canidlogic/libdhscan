# libdhscan

The Delilah Scanline Renderer (dhscan) is a simple but flexible scanline rendering library written in C.

## 1. Overview

The whole library is contained within the `dhscan.h` and `dhscan.c` source files.  To use the library, you create a `DHSCAN_RENDER` object.  This object requires the following information:

- Output image dimensions in pixels
- Total number of triangles
- Accessor functions

The _accessor functions_ are callbacks provided by the client that allow the Delilah Scanline Renderer to access input and output information.  The following accessor functions are required:

- Read X, Y, Z coordinates from a triangle vertex
- Determine shading mode of a specific triangle
- Clear scanline buffer

Triangles are accessed as an integer index that is greater than or equal to zero and less than the total number of triangles that was passed as a parameter to the Delilah Scanline Renderer.  The order of the triangles does not matter.  Triangle vertices are accessed as an index in range [0, 2] for a specific triangle.  The order of triangle vertices does not matter.  It is left up to the client to manage the actual data structures, which gives the client flexibility to decide how the data is structured.

The X and Y coordinates must be integers, though they can be signed and have any value.  The X and Y coordinates must be in the proper coordinate space of the output image.  This means that Delilah Scanline Renderer is actually a 2D renderer, although it can also be used for 3D rendering if the client handles projecting vertex coordinates into 2D space and clipping.

The Z coordinate is a floating-point value that is used to determine visibility when triangles overlap.  For 2D rendering applications with no significant overlap, the accessor function can just return a constant value for the Z coordinate.  The Delilah Scanline Renderer maintains a Z buffer for each scanline, which the client can read after rendering if the client desires to capture Z buffer information.  Z coordinate values must always be zero or greater, with smaller Z coordinates being closer to the viewer.

The shading mode accessor function determines for each triangle whether the triangle shading is _flat_ (triangle shading) or _interpolated_ (vertex shading).  Flat shading means that each triangle has data associated with it that is merely copied to each pixel that it occupies.  Interpolated shading means that each triangle vertex has data associated with it which is interpolated across the triangle surface.

Both shading modes require additional accessor functions specific to the shading mode.  See the
following subsections for further information.

The client is responsible for maintaining a scanline buffer.  The scanline buffer stores the rendered pixels for a single scanline of the output image.  The Delilah Scanline Renderer renders the output image scanline by scanline.  In all shading modes, the client must provide an accessor function that clears the scanline buffer to a "default" pixel value, which might be either a background color or a fully transparent pixel value, for example.  Specific shading modes have special accessors for rendering into the scanline buffer, as described in the subsections below.

### 1.1 Flat shading

In flat shading mode, the following accessor function is also required:

- Copy triangle data to scanline pixel index

When rendering a scanline, this function will be invoked to copy the data from a specific triangle to a specific pixel within the scanline.  The Delilah Scanline Renderer knows nothing about what this actual data is; it might be a color or it might be something else.

### 1.2 Interpolated shading

In interpolated shading mode, the client must maintain an array of _mixing registers_ for the Delilah Scanline Renderer.  The total number of mixing registers required in this array is determined by the constant `DHSCAN_REGCOUNT`.  The following accessor functions are then required in interpolated shading mode:

- Load triangle vertex data into a mixing register
- Store mixing register to a scanline pixel
- Mixing function

Interpolated shading supports both data specific to vertices, and data that applies to the triangle as a whole.  (If no vertex-specific data is required, however, flat shading is more efficient and easier to use.)  The load function takes a reference both to a specific triangle and to a specific vertex within that triangle, so that both vertex and triangle data can be copied into the mixing register.

The store function copies the value in a mixing register into a scanline pixel.  Sophisticated clients might perform pixel rendering here, with information such as UV coordinates, surface normals, and material information stored in the mixing register, which is then rendered to an RGB color in the scanline buffer.

The mixing function takes a destination register index, two source register indices, and a floating-point value in range [0.0, 1.0].  The data in the two source registers should be linearly interpolated according to the floating-point value, and the result should then be written to the destination register.

### 1.3 Summary

The flexible accessor callback function architecture allows the Delilah Scanline Renderer to be independent from the specific definition of triangles and colors used by the clients.  It even allows the Delilah Scanline Renderer to be used in cases where color is not what is being rendered, but something else entirely is being used.

For the details of how to do everything described in this overview, see the `dhscan.h` header file.

## 2. Test program

The `dhrender.c` program is provided, which is a test program that can render RGB triangles.  This program is not part of the core `dhscan` library.  It is intended for testing the scanline renderer and demonstrating how to use it in practice.

The `dhrender.c` program has the following dependencies.  __These dependencies are only from the test program, not for the Delilah Scanline Renderer!__

1. [libshastina](http://www.purl.org/canidtech/r/shastina) version 0.9.3 beta or compatible.
2. [libsophistry](http://www.purl.org/canidtech/r/libsophistry) version 0.5.3 beta or compatible.

The test program has the following syntax:

    dhrender [out] [script]

`[out]` is the path to a PNG file to render as output.  It must have a PNG file extension.  It will be overwritten if it already exists.

`[script]` is the path to a Shastina script that defines what will be rendered.  It is a [Shastina](http://www.purl.org/canidtech/r/shastina) dialect with the following format:

    %dhrender;

    %dim 1920 1080;
    # Render a 1920 x 1080 image

    %shade vertex;
    # Interpolated mode where vertices have RGB color
    # Use %shade triangle; for flat shading
    # In flat shading, triangles will have RGB color

    # Define three vertices
    # Parameter 1: X coordinate
    # Parameter 2: Y coordinate
    # Parameter 3: Z coordinate, must be zero or greater
    # Parameter 4: RGB color as six base-16 digits in {}
    -10 257 5 {6d7800} v
     0   20 9 {ff00ff} v
    37  800 2 {888877} v

    # Parameter 4 only present if %shade vertex;
    # Only signed integers and RGB colors in {} are supported!

    # Define a triangle
    # Each integer is vertex index
    # Zero is first vertex
    0 1 2 t
    # If %shade triangle; then also RGB color fourth parameter

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
