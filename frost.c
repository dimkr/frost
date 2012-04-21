/*
 * frost.c - a simple suspend tool for GNU/Linux.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h> /* for EXIT_SUCCESS and EXIT_FAILURE */
#include <string.h> /* for strnlen() */
#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for O_WRONLY */
#include <unistd.h> /* for write() and getopt() */

/****************
 * command-line *
 ****************/

/* the command-line options */
enum {
	OPTION_HELP = 'h',
	OPTION_SUSPEND_MEM = 'm',
	OPTION_SUSPEND_DISK = 'd',
	OPTION_INVALID = '?'
} options;

/* the getopt() options description string */
#define GETOPT_STRING "hmd"

/* the usage message */
#define USAGE_MESSAGE "Usage: %s [OPTION]...\nSuspend the system, either to " \
                      "RAM or a swap partition.\n\n" \
                      "Options:\n" \
                      "\t-h\tShow this message and quit\n" \
                      "\t-m\tSuspend to RAM\n" \
                      "\t-d\tSuspend to disk\n" \
                      "\n"

/*******************
 * operation modes *
 *******************/

/* the power state file under the sysfs file system */
#define POWER_STATE_FILE "/sys/power/state"

/* the strings written to the power state file */
#define SUSPEND_STRING_MEM "mem"
#define SUSPEND_STRING_DISK "disk"

/* the maximum length of a suspend mode string */
#define MAX_MODE_LENGTH (sizeof(SUSPEND_STRING_DISK) - 1)

/*******************
 * misc. constants *
 *******************/

/* an invalid file descriptor */
#define INVALID_FD (-1)

/* a boolean type */
typedef enum {
	FALSE,
	TRUE
} bool_t;

/******************
 * implementation *
 ******************/

bool_t suspend(char *mode) {
	/* the return value */
	bool_t ret = FALSE;

	/* the file descriptor */
	int fd = 0;

	/* open the power state file */
	if (-1 == (fd = open(POWER_STATE_FILE, O_WRONLY))) {
		#if (-1) != INVALID_FD
		fd = INVALID_FD;
		#endif
		goto end;
	}

	/* write the string to the file */
	if (-1 == write(fd, mode, strnlen(mode, MAX_MODE_LENGTH)))
		goto end;

	/* report success */
	ret = TRUE;

end:
	/* close the file */
	if (INVALID_FD != fd)
		(void) close(fd);

	return ret;
}

int main(int argc, char *argv[]) {
	/* the exit code */
	int ret = EXIT_FAILURE;

	/* the current command-line option */
	char option = '\0';

	/* the suspend mode */
	char *mode = NULL;

	/* a boolean which indicates whether the suspend mode was specified */
	bool_t is_mode_specified = FALSE;

	/* a boolean which indicates whether the command-line is valid */
	bool_t is_valid = TRUE;

	/* if no options were passed, the command-line is invalid */
	if (1 == argc)
		is_valid = FALSE;
	else {
		/* parse the command-line */
		while (-1 != (option = getopt(argc, argv, GETOPT_STRING))) {
			switch (option) {
				/* help */
				case OPTION_HELP:
					is_valid = FALSE;
					/* report success, since the user asked for help nicely */
					ret = EXIT_SUCCESS;
					break;

				/* suspend to RAM */
				case OPTION_SUSPEND_MEM:
					/* chwck the mode was already specified */
					if (TRUE == is_mode_specified) {
						is_valid = FALSE;
						break;
					}
					mode = SUSPEND_STRING_MEM;
					is_mode_specified = TRUE;
					break;

				/* suspend to disk */
				case OPTION_SUSPEND_DISK:
					/* chwck the mode was already specified */
					if (TRUE == is_mode_specified) {
						is_valid = FALSE;
						break;
					}
					mode = SUSPEND_STRING_DISK;
					is_mode_specified = TRUE;
					break;

				/* any invalid option */
				case OPTION_INVALID:
					is_valid = FALSE;
					break;
			}
		}
	}

	/* if there are any non-option arguments (e.g getopt() parsed only some
	 * arguments, an invalid command-line (which contains non-option arguments)
	 * was used */
	if (argc > optind)
		is_valid = FALSE;

	/* if the command-line is invalid, show the help message and exit */
	if (FALSE == is_valid) {
		(void) printf(USAGE_MESSAGE, argv[0]);
		goto end;
	}

	/* suspend the system */
	if (FALSE == suspend(mode))
		goto end;

	/* report success */
	ret = EXIT_SUCCESS;

end:
	return ret;
}