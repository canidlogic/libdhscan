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
 * Program entrypoint
 * ==================
 */

int main(int argc, char *argv[]) {
  
  int status = 1;
  int i = 0;
  
  const char *arg_pOutPath = NULL;
  const char *arg_pScriptPath = NULL;
  
  SNSOURCE *pScriptSrc = NULL;
  FILE *fhScript = NULL;
  
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
  
  /* @@TODO: */
  
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
