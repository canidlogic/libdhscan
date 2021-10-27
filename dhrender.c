/*
 * dhrender.c
 * ==========
 * 
 * Test program for Delilah Scanline Renderer.
 * 
 * See README.md for further information.
 */

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

/*
 * Shading mode constants.
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
static int first_pass(
    SNSOURCE    * pSrc,
    SCRIPT_INFO * psi,
    int         * perr,
    long        * pline);

static const char *errstr(int code);

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
  
  /* @@TODO: */
  memset(psi, 0, sizeof(SCRIPT_INFO));
  psi->w = 640;
  psi->h = 480;
  psi->shade = SHADE_FLAT;
  psi->vcount = 4096;
  psi->tcount = 1024;
  return 1;
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
      
      default:
        /* Unrecognized error code */
        pResult = "Unknown error";
    }
  }
  
  /* Return the message */
  return pResult;
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
