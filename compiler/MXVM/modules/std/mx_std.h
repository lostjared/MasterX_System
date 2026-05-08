/**
 * @file mx_std.h
 * @brief Standard library module C API — function declarations exported to MXVM programs
 * @author Jared Bruni
 */
#ifndef _MX_STD_H__
#define _MX_STD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Store command-line arguments for later access via argc()/argv()
 * @param argc Number of arguments
 * @param argv Argument string array
 */
void set_program_args(int argc, const char **argv);

/** @brief Return the number of stored command-line arguments */
int argc(void);

/** @brief Return the command-line argument at the given index
 * @param idx Zero-based argument index
 * @return Pointer to the argument string
 */
const char *argv(int idx);

/** @brief Free the stored command-line argument memory */
void free_program_args(void);

#ifdef __cplusplus
}
#endif

#endif
