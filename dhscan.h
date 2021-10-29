#ifndef DHSCAN_H_INCLUDED
#define DHSCAN_H_INCLUDED

/*
 * dhscan.h
 * ========
 * 
 * Header file for Delilah Scanline Renderer (libdhscan).
 * 
 * See README.md for further information.
 */

#include <stddef.h>
#include <stdint.h>

/*
 * The total number of mixing registers the client must provide for the
 * Delilah Scanline Renderer in interpolated shading mode.
 * 
 * @@TODO:
 */
#define DHSCAN_REGCOUNT (8)

/*
 * The different shading modes.
 */
#define DHSCAN_MODE_VERTEX    (1)
#define DHSCAN_MODE_TRIANGLE  (2)

/*
 * Structure prototype for DHSCAN_RENDER.
 * 
 * Definition given in implementation file.
 */
struct DHSCAN_RENDER_TAG;
typedef struct DHSCAN_RENDER_TAG DHSCAN_RENDER;

/*
 * Structure used to hold information about a triangle vertex.
 * 
 * This does NOT need to be the actual data structure the client uses
 * for storing vertices.  This structure is only used within the context
 * of accessor functions for transferring data about a triangle vertex
 * between the client and the Delilah Scanline Renderer.
 */
typedef struct {
  
  /*
   * The projected X coordinate.
   * 
   * This can have any value.  It is in the coordinate space of the
   * output image, where (0, 0) is the upper-left pixel of the image,
   * and (width - 1, height - 1) is the bottom-right pixel of the image.
   * 
   * The X coordinate does NOT need to be within the confines of the
   * image area.  It can also have negative values.
   */
  int32_t x;
  
  /*
   * The projected Y coordinate.
   * 
   * This can have any value.  It is in the coordinate space of the
   * output image, where (0, 0) is the upper-left pixel of the image,
   * and (width - 1, height - 1) is the bottom-right pixel of the image.
   * 
   * The Y coordinate does NOT need to be within the confines of the
   * image area.  It can also have negative values.
   */
  int32_t y;
  
  /*
   * The Z coordinate.
   * 
   * This must be a finite value that is zero or greater.  Lesser Z
   * coordinates mean closer to the viewer, while greater Z coordinates
   * mean further from the viewer.
   */
  float z;
  
} DHSCAN_VERTEX;

/*
 * Function pointer type for vertex accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The int32_t parameter is a triangle index that selects a specific
 * triangle.  It will be at least zero and less than the total number of
 * triangles.
 * 
 * The int parameter is the vertex number within the triangle to query.
 * It will be in range [0, 2].
 * 
 * When this vertex accessor function is called, the client should write
 * the X, Y, and Z coordinates of the requested vertex into the given
 * DHSCAN_VERTEX structure.  See the documentation of that structure for
 * further information.
 * 
 * Triangle vertices should not change during rendering or undefined
 * behavior occurs.
 */
typedef void (*dhscan_fp_vertex)(void *, int32_t, int, DHSCAN_VERTEX *);

/*
 * Function pointer type for shading mode accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The int32_t parameter is a triangle index that selects a specific
 * triangle.  It will be at least zero and less than the total number of
 * triangles.
 * 
 * The return value must be one of the DHSCAN_MODE constants, which
 * determines the kind of shading that is used for this triangle.  If it
 * is DHSCAN_MODE_TRIANGLE, then flat shading is used for the triangle,
 * while DHSCAN_MODE_VERTEX means that interpolated shading is used for
 * the triangle.
 * 
 * This function allows the client to determine which shading mode is
 * used for each specific triangle.
 * 
 * Shading modes for specific triangles should not change during
 * rendering or undefined behavior occurs.
 */
typedef int (*dhscan_fp_mode)(void *, int32_t);

/*
 * Function pointer type for scanline clear accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * When this function is called, the client should clear the scanline
 * buffer to some default value.  This default value will be present in
 * the rendered scanline when no triangle is present at that pixel.
 */
typedef void (*dhscan_fp_clear)(void *);

/*
 * Function pointer type for flat shading accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The first int32_t parameter is the target pixel within the client
 * scanline buffer.  This is always in the range [0, width - 1] where
 * width is the width in pixels of the output image.
 * 
 * The second int32_t parameter is the source triangle index.  It will
 * be at least zero and less than the total number of triangles.
 * 
 * When this function is called, the client should determine the flat
 * shading "color" of the indicated triangle and then copy that "color"
 * to the indicated pixel within the scanline buffer, overwriting any
 * "color" that is currently there.
 * 
 * (The word "color" is in scare quotes because the Delilah Scanline
 * Renderer is not limited to rendering colors, and the client might
 * have some other type of data to render.)
 */
typedef void (*dhscan_fp_flat)(void *, int32_t, int32_t);

/*
 * Function pointer type for vertex shading load accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The first int parameter is the target mixing register.  It is in the
 * range [0, DHSCAN_REGCOUNT - 1].
 * 
 * The int32_t parameter is a triangle index that selects a specific
 * triangle.  It will be at least zero and less than the total number of
 * triangles.
 * 
 * The second int parameter is the vertex number within the triangle to
 * query.  It will be in range [0, 2].
 * 
 * When this function is called, the client should copy the "color" of
 * the indicated triangle vertex, and any "color" associated with the
 * specific triangle, and copy it into the indicated mixing register,
 * overwriting anything currently stored there.
 * 
 * (The word "color" is in scare quotes because the Delilah Scanline
 * Renderer is not limited to rendering colors, and the client might
 * have some other type of data to render.  For example, the vertex
 * "color" might be a normal direction vector and (u,v) texture
 * coordinates, and the triangle "color" might be a material used for
 * rendering.)
 */
typedef void (*dhscan_fp_load)(void *, int, int32_t, int);

/*
 * Function pointer type for vertex shading store accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The int32_t parameter is the target pixel within the client scanline
 * buffer.  This is always in the range [0, width - 1] where width is
 * the width in pixels of the output image.
 * 
 * The int parameter is the source mixing register.  It is in the range
 * [0, DHSCAN_REGCOUNT - 1].
 * 
 * When this function is called, the client should copy the "color"
 * stored in the given mixing register to the indicated pixel in the
 * scanline buffer, overwriting anything currently stored there.
 * 
 * (The word "color" is in scare quotes because the Delilah Scanline
 * Renderer is not limited to rendering colors, and the client might
 * have some other type of data to render.  For example, the "color"
 * stored in the mixing register might be a normal vector, a (u,v)
 * texture coordinate and a rendering material, which is then rendered
 * to an RGB color before being stored in the scanline buffer.)
 */
typedef void (*dhscan_fp_store)(void *, int32_t, int);

/*
 * Function pointer type for the interpolation mixing accessor function.
 * 
 * The (void *) parameter is a custom parameter that is passed through
 * and intended for client data.
 * 
 * The first int parameter is the target mixing register, the second
 * int parameter is the first source mixing register, and the third int
 * parameter is the second source mixing register.  All three int
 * parameters are in range [0, DHSCAN_REGCOUNT - 1].  Furthermore, the
 * three mixing register indices will always be different and never
 * equal each other.
 * 
 * The double parameter must be a finite value in range [0.0, 1.0].
 * 
 * When this function is called, the client should linearly interpolate
 * between the first source mixing register and the second source mixing
 * register according to the double parameter, with 0.0 meaning at the
 * first source mixing register, 1.0 meaning at the second source mixing
 * register, and all values in between meaning somewhere between those
 * two register values.
 * 
 * The source mixing registers should not be modified, but the result
 * should be stored in the target mixing register, whose contents should
 * be overwritten with the mixing result.
 */
typedef void (*dhscan_fp_mix)(void *, int, int, int, double);

#endif
