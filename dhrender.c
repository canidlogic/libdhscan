/*
 * dhrender.c
 * ==========
 * 
 * Test program for Delilah Scanline Renderer.
 * 
 * See README.md for further information.
 */

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shastina.h"
#include "sophistry.h"

/*
 * Constants
 * =========
 */

/*
 * Error codes.
 * 
 * Negative Shastina error codes are also supported.  You can use the
 * errstr() function to convert to an error message.
 * 
 * Remember to update errstr() if updating these codes!
 */
#define ERR_OK    (0)   /* No error */
#define ERR_NOSIG (1)   /* Failed to read script signature */
#define ERR_HEADC (2)   /* Invalid header metacommand */
#define ERR_HEADR (3)   /* Repetition of header metacommand */
#define ERR_HEADS (4)   /* Header metacommand syntax error */
#define ERR_DIM   (5)   /* Dimension out of range */
#define ERR_SHADE (6)   /* Unrecognized shading mode */
#define ERR_NODIM (7)   /* Dimensions not declared */
#define ERR_NOSHA (8)   /* Shading mode not declared */
#define ERR_STRAY (9)   /* Stray metacommand after header */
#define ERR_MANYV (10)  /* Too many vertices */
#define ERR_MANYT (11)  /* Too many triangles */

/*
 * Shading mode constants.
 * 
 * These must be greater than zero.
 */
#define SHADE_FLAT  (1)   /* Flat shading */
#define SHADE_INTER (2)   /* Interpolated shading */

/*
 * Maximum number of vertices and triangles supported.
 */
#define MAX_VERTEX (16384)
#define MAX_TRIS   (16384)

/*
 * Type declarations
 * =================
 */

/*
 * Used to gather information about the script from the first pass.
 */
typedef struct {
  
  /*
   * The width in pixels of the output image.
   * 
   * Will always be in range [1, SPH_IMAGE_MAXDIM] from the Sophistry
   * header.
   */
  int32_t w;
  
  /*
   * The height in pixels of the output image.
   * 
   * Will always be in range [1, SPH_IMAGE_MAXDIM] from the Sophistry
   * header.
   */
  int32_t h;
  
  /*
   * The shading mode.
   * 
   * This is one of the SHADE_ constants.
   */
  int shade;
  
  /*
   * The total number of vertices declared in the script.
   * 
   * In range [0, MAX_VERTEX].
   */
  int32_t vcount;
  
  /*
   * The total number of triangles declared in the script.
   * 
   * In range [0, MAX_TRIS].
   */
  int32_t tcount;
  
} SCRIPT_INFO;

/*
 * Local data
 * ==========
 */

/*
 * The name of the executable module, for log messages.
 * 
 * This will be set at the start of the program entrypoint.
 */
static const char *pModule = NULL;

/*
 * Local functions
 * ===============
 */

/* Prototypes */
static void begin_data(int32_t vcount, int32_t tcount);
static int declare_vert(int32_t x, int32_t y, int32_t z, uint32_t c);
static int declare_tri(int32_t i, int32_t j, int32_t k, uint32_t c);
static int check_decl(void);

static int first_pass(
    SNSOURCE    * pSrc,
    SCRIPT_INFO * psi,
    int         * perr,
    long        * pline);

static const char *errstr(int code);
static int parseInt(const char *pstr, int32_t *pv);

/*
 * Initialize internal data structures so that they are ready to accept
 * vertex and triangle declarations.
 * 
 * You must determine the exact number of vertices and triangles before
 * calling this function.  Use first_pass() to get this information.
 * 
 * This function can only be called once.  A fault occurs if it is
 * called more than once.
 * 
 * The vertex count must be in range [0, MAX_VERTEX] and the triangle
 * count must be in range [0, MAX_TRIS].
 * 
 * Parameters:
 * 
 *   vcount - the number of vertices that will be declared
 * 
 *   tcount - the number of triangles that will be declared
 */
static void begin_data(int32_t vcount, int32_t tcount) {
  /* @@TODO: */
  fprintf(stderr, "begin_data %ld %ld\n", (long) vcount, (long) tcount);
}

/*
 * Declare a vertex.
 * 
 * You must call begin_data() before this function.  This function must
 * then be called exactly as many times as the vcount value given to
 * begin_data(), to declare each vertex.  Declaration order is
 * significant, as it determines the vertex index.
 * 
 * The X and Y coordinates can have any value.  The Z coordinate must be
 * zero or greater.  The color c must be in range [0, 0xffffff].
 * 
 * If flat shading mode is being used, the vertex color does not matter
 * and can be set to zero.
 * 
 * Parameters:
 * 
 *   x - the X coordinate of the vertex
 * 
 *   y - the Y coordinate of the vertex
 * 
 *   z - the Z coordinate of the vertex
 * 
 *   c - the packed RGB color of the vertex
 * 
 * Return:
 * 
 *   non-zero if successful, zero if too many vertices are being
 *   declared
 */
static int declare_vert(int32_t x, int32_t y, int32_t z, uint32_t c) {
  /* @@TODO: */
  fprintf(stderr, "declare_vert %ld %ld %ld %l06x\n",
            (long) x, (long) y, (long) z, (long) c);
  return 1;
}

/*
 * Declare a triangle.
 * 
 * You must call begin_data() before this function.  This function must
 * then be called exactly as many times as the tcount value given to
 * begin_data(), to declare each triangle.  Declaration order is
 * significant, as it determines the triangle index.
 * 
 * The i, j, and k values are indices into the vertex array that 
 * determine the vertices of the triangle.  The must all have values
 * that are zero or greater, and less than the vcount value given to
 * begin_data().  The referenced vertices do NOT need to be declared
 * before using them in a triangle.
 * 
 * The color c must be in range [0, 0xffffff].  If interpolated shading
 * mode is being used, the triangle color does not matter and can be set
 * to zero.
 * 
 * Parameters:
 * 
 *   i - the index of the first vertex
 * 
 *   j - the index of the second vertex
 * 
 *   k - the index of the third vertex
 * 
 *   c - the packed RGB color of the triangle
 * 
 * Return:
 * 
 *   non-zero if successful, zero if too many triangles are being
 *   declared
 */
static int declare_tri(int32_t i, int32_t j, int32_t k, uint32_t c) {
  /* @@TODO: */
  fprintf(stderr, "declare_tri %ld %ld %ld %l06x\n",
            (long) i, (long) j, (long) k, (long) c);
  return 1;
}

/*
 * Check whether all necessary vertex and triangle declarations have
 * been made.
 * 
 * This returns non-zero only if:
 * 
 *   1) begin_data(vcount, tcount) has been called
 *   2) declare_vert() has been called vcount times
 *   3) declare_tri() has been called tcount times
 * 
 * In all other cases, the function returns zero.
 * 
 * Return:
 * 
 *   non-zero if all triangles and vertices have been declared, zero if
 *   not
 */
static int check_decl(void) {
  /* @@TODO: */
  return 1;
}

/*
 * Run the first pass on the Shastina script.
 * 
 * The first pass is responsible for handling all metacommands, parsing
 * the metacommand header, counting the total number of vertex and
 * triangle operations that are present in the script, and making sure
 * there is nothing after the EOF marker |; at the end of the script.
 * 
 * If successful, information about the script gathered from the first
 * pass will be written into the passed SCRIPT_INFO structure, *perr
 * will be set to ERR_OK (zero), *pline will be set to zero, and a
 * non-zero value will be returned.
 * 
 * If there is an error, the SCRIPT_INFO structure will be in an
 * undefined state, *perr will have an error code that can be converted
 * into a message with errstr(), *pline will be greater than zero if the
 * error occurred on a specific line within the script, else zero if
 * there is no specific error number.
 * 
 * The source will be rewound at the start of the function.  At the end
 * of the function, the source will be positioned at the end of the
 * stream if successful, or at an undefined location if there was an
 * error.
 * 
 * The given source must support multipass operation.
 * 
 * Parameters:
 * 
 *   pSrc - the input source to read the script from
 * 
 *   psi - pointer to structure to receive information about script
 * 
 *   perr - pointer to variable to receive error code
 * 
 *   pline - pointer to variable to receive line number
 * 
 * Return:
 * 
 *   non-zero if successful, zero if error
 */
static int first_pass(
    SNSOURCE    * pSrc,
    SCRIPT_INFO * psi,
    int         * perr,
    long        * pline) {
  
  int status = 1;
  int i = 0;
  int32_t iv = 0;
  SNPARSER *pr = NULL;
  SNENTITY ent;
  
  /* Clear structures */
  memset(&ent, 0, sizeof(SNENTITY));
  
  /* Check parameters */
  if ((pSrc == NULL) || (psi == NULL) ||
      (perr == NULL) || (pline == NULL)) {
    abort();
  }
  if (!snsource_ismulti(pSrc)) {
    abort();
  }
  
  /* Reset return state */
  memset(psi, 0, sizeof(SCRIPT_INFO));
  *perr = ERR_OK;
  *pline = 0;
  
  /* Rewind the source */
  if (!snsource_rewind(pSrc)) {
    status = 0;
    *perr = SNERR_IOERR;
  }
  
  /* Get a Shastina parser */
  if (status) {
    pr = snparser_alloc();
  }
  
  /* Read the file signature */
  if (status) {
    snparser_read(pr, &ent, pSrc);
    if (ent.status != SNENTITY_BEGIN_META) {
      status = 0;
      *perr = ERR_NOSIG;
    }
  }
  if (status) {
    snparser_read(pr, &ent, pSrc);
    if (ent.status != SNENTITY_META_TOKEN) {
      status = 0;
      *perr = ERR_NOSIG;
    }
    if (status && (strcmp(ent.pKey, "dhrender") != 0)) {
      status = 0;
      *perr = ERR_NOSIG;
    }
  }
  if (status) {
    snparser_read(pr, &ent, pSrc);
    if (ent.status != SNENTITY_END_META) {
      status = 0;
      *perr = ERR_NOSIG;
    }
  }
  
  /* Clear the metacommand header fields in the info structure */
  if (status) {
    psi->w = 0;
    psi->h = 0;
    psi->shade = 0;
  }
  
  /* Read rest of metacommand header */
  if (status) {
    for(snparser_read(pr, &ent, pSrc);
        ent.status == SNENTITY_BEGIN_META;
        snparser_read(pr, &ent, pSrc)) {
      
      /* Metacommand starting, so read the token that determines which
       * command */
      snparser_read(pr, &ent, pSrc);
      if (ent.status < 0) {
        status = 0;
        *perr = ent.status;
        *pline = snparser_count(pr);
        
      } else if (ent.status != SNENTITY_META_TOKEN) {
        status = 0;
        *perr = ERR_HEADC;
        *pline = snparser_count(pr);
      }
      
      /* Get parameters appropriate to metacommand and process */
      if (status && (strcmp(ent.pKey, "dim") == 0)) {
        /* Dimension metacommand; make sure not already used */
        if ((psi->w > 0) || (psi->h > 0)) {
          status = 0;
          *perr = ERR_HEADR;
          *pline = snparser_count(pr);
        }
        
        /* Read two integer tokens for width and height */
        if (status) {
          for(i = 0; i < 2; i++) {
            
            /* Read a metacommand token */
            snparser_read(pr, &ent, pSrc);
            if (ent.status < 0) {
              status = 0;
              *perr = ent.status;
              *pline = snparser_count(pr);
            } else if (ent.status != SNENTITY_META_TOKEN) {
              status = 0;
              *perr = ERR_HEADS;
              *pline = snparser_count(pr);
            }
            
            /* Parse metacommand token as integer */
            if (status) {
              if (!parseInt(ent.pKey, &iv)) {
                status = 0;
                *perr = ERR_HEADS;
                *pline = snparser_count(pr);
              }
            }
            
            /* Check integer range */
            if (status) {
              if ((iv < 1) || (iv > SPH_IMAGE_MAXDIM)) {
                status = 0;
                *perr = ERR_DIM;
                *pline = snparser_count(pr);
              }
            }
            
            /* Store integer value into structure */
            if (status && (i == 0)) {
              psi->w = iv;
            
            } else if (status && (i == 1)) {
              psi->h = iv;
              
            } else if (status) {
              abort();  /* shouldn't happen */
            }
            
            /* Leave loop if error */
            if (!status) {
              break;
            }
          }
        }
        
      } else if (status && (strcmp(ent.pKey, "shade") == 0)) {
        /* Shading mode metacommand; make sure not already used */
        if (psi->shade != 0) {
          status = 0;
          *perr = ERR_HEADR;
          *pline = snparser_count(pr);
        }
        
        /* Read the shading type metatoken */
        if (status) {
          snparser_read(pr, &ent, pSrc);
          if (ent.status < 0) {
            status = 0;
            *perr = ent.status;
            *pline = snparser_count(pr);
          
          } else if (ent.status != SNENTITY_META_TOKEN) {
            status = 0;
            *perr = ERR_HEADS;
            *pline = snparser_count(pr);
          }
        }
        
        /* Set the appropriate value */
        if (status && (strcmp(ent.pKey, "vertex") == 0)) {
          psi->shade = SHADE_INTER;
        } else if (status && (strcmp(ent.pKey, "triangle") == 0)) {
          psi->shade = SHADE_FLAT;
        } else if (status) {
          /* Unrecognized shading mode */
          status = 0;
          *perr = ERR_SHADE;
          *pline = snparser_count(pr);
        }
        
      } else if (status) {
        /* Unrecognized metacommand */
        status = 0;
        *perr = ERR_HEADC;
        *pline = snparser_count(pr);
      }
      
      /* Metacommand should now end */
      if (status) {
        snparser_read(pr, &ent, pSrc);
        if (ent.status < 0) {
          status = 0;
          *perr = ent.status;
          *pline = snparser_count(pr);
        
        } else if (ent.status != SNENTITY_END_META) {
          status = 0;
          *perr = ERR_HEADC;
          *pline = snparser_count(pr);
        }
      }
      
      /* Leave loop if error */
      if (!status) {
        break;
      }
    }
    if (status && (ent.status < 0)) {
      status = 0;
      *perr = ent.status;
      *pline = snparser_count(pr);
    }
  }
  
  /* We should have gotten the output dimensions and shading mode from
   * the header metacommands */
  if (status && ((psi->w < 1) || (psi->h < 1))) {
    status = 0;
    *perr = ERR_NODIM;
  
  } else if (status && (psi->shade == 0)) {
    status = 0;
    *perr = ERR_NOSHA;
  }
  
  /* Clear the counter fields in the info structure */
  if (status) {
    psi->tcount = 0;
    psi->vcount = 0;
  }
  
  /* If we are successful, we should have just read the first token
   * after the metacommand header; process this token and all following
   * tokens until the EOF token, checking that there are no further
   * metacommand tokens, and counting the number of triangle and vertex
   * operations */
  if (status) {
    for( ; ent.status > 0; snparser_read(pr, &ent, pSrc)) {
      
      /* Check that this entity is not any metacommand type token */
      if ((ent.status == SNENTITY_BEGIN_META) ||
          (ent.status == SNENTITY_END_META) ||
          (ent.status == SNENTITY_META_TOKEN) ||
          (ent.status == SNENTITY_META_STRING)) {
        status = 0;
        *perr = ERR_STRAY;
        *pline = snparser_count(pr);
      }
      
      /* If this is an operation token, update counts */
      if (status && (ent.status == SNENTITY_OPERATION)) {
        if (strcmp(ent.pKey, "v") == 0) {
          /* Found a vertex command, so update count */
          if (psi->vcount < MAX_VERTEX) {
            (psi->vcount)++;
          } else {
            status = 0;
            *perr = ERR_MANYV;
            *pline = snparser_count(pr);
          }
          
        } else if (strcmp(ent.pKey, "t") == 0) {
          /* Found a triangle command, so update count */
          if (psi->tcount < MAX_TRIS) {
            (psi->tcount)++;
          } else {
            status = 0;
            *perr = ERR_MANYT;
            *pline = snparser_count(pr);
          }
        }
      }
      
      /* Leave loop if error */
      if (!status) {
        break;
      }
    }
    if (status && (ent.status < 0)) {
      status = 0;
      *perr = ent.status;
      *pline = snparser_count(pr);
    }
  }
  
  /* If we got here successfully, we just read the EOF token, so make
   * sure nothing further in input file */
  if (status) {
    status = snsource_consume(pSrc);
    if (status < 0) {
      *perr = status;
      *pline = snparser_count(pr);
      status = 0;
    } else if (status == 0) {
      abort();  /* shouldn't happen */
    }
  }
  
  /* Adjust line number if necessary */
  if ((*pline < 0) || (*pline >= LONG_MAX)) {
    *pline = 0;
  }
  
  /* Release parser if allocated */
  snparser_free(pr);
  pr = NULL;
  
  /* Clear info structure if failure */
  if (!status) {
    memset(psi, 0, sizeof(SCRIPT_INFO));
  }
  
  /* Return status */
  return status;
}

/*
 * Get an error message from an error code.
 * 
 * The error message begins with an uppercase letter but does not have
 * punctuation or a line break at the end.
 * 
 * If code is negative, this is passed through to snerror_str().  If
 * code is zero, "No error" is returned.  If code is unrecognized,
 * "Unknown error" is returned.
 * 
 * Parameters:
 * 
 *   code - the error code
 * 
 * Return:
 * 
 *   an error message
 */
static const char *errstr(int code) {
  
  const char *pResult = NULL;
  
  /* Check code type */
  if (code < 0) {
    /* Negative errors are Shastina errors */
    pResult = snerror_str(code);
  
  } else {
    /* Handle non-Shastina errors */
    switch (code) {
      
      case ERR_OK:
        pResult = "No error";
        break;
      
      case ERR_NOSIG:
        pResult = "Failed to read script signature";
        break;
      
      case ERR_HEADC:
        pResult = "Invalid header metacommand";
        break;
      
      case ERR_HEADR:
        pResult = "Repetition of header metacommand";
        break;
      
      case ERR_HEADS:
        pResult = "Header metacommand syntax error";
        break;
      
      case ERR_DIM:
        pResult = "Image output dimension out of range";
        break;
      
      case ERR_SHADE:
        pResult = "Unrecognized shading mode";
        break;
      
      case ERR_NODIM:
        pResult = "You must declare output dimensions in header";
        break;
      
      case ERR_NOSHA:
        pResult = "You must declare shading mode in header";
        break;
      
      case ERR_STRAY:
        pResult = "Stray metacommand after metacommand header";
        break;
      
      case ERR_MANYV:
        pResult = "Too many declared vertices";
        break;
      
      case ERR_MANYT:
        pResult = "Too many declared triangles";
        break;
      
      default:
        /* Unrecognized error code */
        pResult = "Unknown error";
    }
  }
  
  /* Return the message */
  return pResult;
}

/*
 * Parse the given string as a signed integer.
 * 
 * pstr is the string to parse.
 * 
 * pv points to the integer value to use to return the parsed numeric
 * value if the function is successful.
 * 
 * In two's complement, this function will not successfully parse the
 * least negative value.
 * 
 * Parameters:
 * 
 *   pstr - the string to parse
 * 
 *   pv - pointer to the return numeric value
 * 
 * Return:
 * 
 *   non-zero if successful, zero if failure
 */
static int parseInt(const char *pstr, int32_t *pv) {
  
  int negflag = 0;
  int32_t result = 0;
  int status = 1;
  int32_t d = 0;
  
  /* Check parameters */
  if ((pstr == NULL) || (pv == NULL)) {
    abort();
  }
  
  /* If first character is a sign character, set negflag appropriately
   * and skip it */
  if (*pstr == '+') {
    negflag = 0;
    pstr++;
  } else if (*pstr == '-') {
    negflag = 1;
    pstr++;
  } else {
    negflag = 0;
  }
  
  /* Make sure we have at least one digit */
  if (*pstr == 0) {
    status = 0;
  }
  
  /* Parse all digits */
  if (status) {
    for( ; *pstr != 0; pstr++) {
    
      /* Make sure in range of digits */
      if ((*pstr < '0') || (*pstr > '9')) {
        status = 0;
      }
    
      /* Get numeric value of digit */
      if (status) {
        d = (int32_t) (*pstr - '0');
      }
      
      /* Multiply result by 10, watching for overflow */
      if (status) {
        if (result <= INT32_MAX / 10) {
          result = result * 10;
        } else {
          status = 0; /* overflow */
        }
      }
      
      /* Add in digit value, watching for overflow */
      if (status) {
        if (result <= INT32_MAX - d) {
          result = result + d;
        } else {
          status = 0; /* overflow */
        }
      }
    
      /* Leave loop if error */
      if (!status) {
        break;
      }
    }
  }
  
  /* Invert result if negative mode */
  if (status && negflag) {
    result = -(result);
  }
  
  /* Write result if successful */
  if (status) {
    *pv = result;
  }
  
  /* Return status */
  return status;
}

/*
 * Program entrypoint
 * ==================
 */

int main(int argc, char *argv[]) {
  
  int status = 1;
  int i = 0;
  int ecode = 0;
  long lnum = 0;
  
  const char *arg_pOutPath = NULL;
  const char *arg_pScriptPath = NULL;
  
  SNSOURCE *pScriptSrc = NULL;
  FILE *fhScript = NULL;
  
  SCRIPT_INFO si;
  
  /* Initialize structures */
  memset(&si, 0, sizeof(SCRIPT_INFO));
  
  /* Get module name */
  pModule = NULL;
  if ((argc > 0) && (argv != NULL)) {
    pModule = argv[0];
  }
  if (pModule == NULL) {
    pModule = "dhrender";
  }
  
  /* Check that parameters are present */
  if (argc > 0) {
    if (argv == NULL) {
      abort();
    }
    for(i = 0; i < argc; i++) {
      if (argv[i] == NULL) {
        abort();
      }
    }
  }
  
  /* We need exactly two parameters beyond module name */
  if (argc != 3) {
    fprintf(stderr, "%s: Wrong number of parameters!\n", pModule);
    status = 0;
  }
  
  /* Get the parameters */
  if (status) {
    arg_pOutPath = argv[1];
    arg_pScriptPath = argv[2];
  }
  
  /* Open the script file for reading */
  if (status) {
    fhScript = fopen(arg_pScriptPath, "rb");
    if (fhScript == NULL) {
      fprintf(stderr, "%s: Failed to open script file!\n", pModule);
      status = 0;
    }
  }
  
  /* Wrap the script file handle in a Shastina source and transfer
   * ownership of the file handle to the source object */
  if (status) {
    pScriptSrc = snsource_stream(
                  fhScript, SNSTREAM_OWNER | SNSTREAM_RANDOM);
    fhScript = NULL;
  }
  
  /* Run the first pass */
  if (status) {
    if (!first_pass(pScriptSrc, &si, &ecode, &lnum)) {
      /* First pass failed */
      if (lnum > 0) {
        fprintf(stderr, "%s: [Line %ld] %s!\n",
                  pModule, lnum, errstr(ecode));
      } else {
        fprintf(stderr, "%s: %s!\n",
                  pModule, errstr(ecode));
      }
      status = 0;
    }
  }
  
  /* @@TODO: */
  if (status) {
    printf("Width:  %ld\n", (long) si.w);
    printf("Height: %ld\n", (long) si.h);
    printf("Shade:  %d\n", si.shade);
    printf("Tcount: %ld\n", (long) si.tcount);
    printf("Vcount: %ld\n", (long) si.vcount);
  }
  
  /* Close script file handle if open */
  if (fhScript != NULL) {
    fclose(fhScript);
    fhScript = NULL;
  }
  
  /* Close script source if open */
  snsource_free(pScriptSrc);
  pScriptSrc = NULL;
  
  /* Invert status and return */
  if (status) {
    status = 0;
  } else {
    status = 1;
  }
  return status;
}
