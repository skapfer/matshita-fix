/* xscsi.h
 *
 * Link with one of the local *scsi.c* in order to speak SCSI concisely.
 *
 * FIXME: Add Linux SG_IO pass thru.
 * FIXME: Add Win XP SPTD pass thru.
 * FIXME: Add Win XP SPT pass thru.
 * FIXME: Add Win ME ASPI pass thru.
 * FIXME: Add Dos ASPI pass thru.
 * FIXME: Add Solaris pass thru.
 * FIXME: Add virtual pass thru.
 */

/* Compile once even if included again. */

#ifndef XSCSI_H
#define XSCSI_H 20040625 /* $yyyy$mm$dd version */

/* Link with standard C libraries. */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Link despite C++. */

#ifdef __cplusplus
extern "C" {
#endif

/* Count the elements of an array. */

#define LENGTHOF(A) ((sizeof (A)) / (sizeof (A)[0]))

/* Work with the bytes of integers. */

#define NTH(I, N) (((unsigned char *)&(I))[N])

#if __BYTE_ORDER == __BIG_ENDIAN /* if byte 0 = most significant */
#define BIG(I, N) NTH(I, N)
#define LIL(I, N) BIG(I, sizeof (I) - 1 - (N))
#else /* else if byte 0 = least significant */
#define LIL(I, N) NTH(I, N)
#define BIG(I, N) LIL(I, sizeof (I) - 1 - (N))
#endif

/* Recommend a limit for the length in bytes of the CDB.
 *
 * Consider the limits:
 *
 *	xC	// common max of SCSI over IDE
 *	xC	// common max of SCSI over 1394 i.e. xC = (x20 - x14)
 *	x10	// max SCSI over IDE
 *	x10	// max SCSI over USB
 *	x10	// max declared SCSI thru Dos Aspi or Win32 Ioctl
 *	x10	// max SCSI-3 for standard op xXX other than x7F
 *	xFF	// common max of SCSI pass thru
 *	x104	// max SCSI-3 permits for op x 7F XX XX = 8 + xFC
 *	x107	// max SCSI-3 expresses for op x 7F XX XX = 8 + xFF
 *	x3E8	// max SCSI over 1394 = ((xFF * 4) - x14)
 *
 * Arguably 2001-07 SPC-2 r20: "4.3.3" "The variable length CDB formats" says
 * SK ASC = x 5 24 "ILLEGAL REQUEST" "INVALID FIELD IN CDB" may mean CDB too
 * long to copy out.
 */

#define MAX_CDB 0xFF

/* Recommend a limit for the length in bytes of the sense.
 *
 * Consider the limits:
 *
 *	x8	// just enough to include the "ADDITIONAL LENGTH"
 *	xE	// popular since Win ME/ 98/ 95/ DOS
 *	xE	// just enough to include SK ASC ASCQ
 *	x12	// popular since Win XP/ 2K/ Mac OS X
 *	x12	// just enough to include the "SKSV" "SENSE-KEY SPECIFIC" info
 *	x12	// just enough to include all the fields that SPC-2 r20 names
 *	xFF     // max CDB[4] "ALLOCATION LENGTH" of op x03 "REQUEST SENSE"
 *
 * FIXME: Align USUAL_SENSE length to 4 bytes (32 bits).
 */

#define MAX_SENSE 0xFF
#define USUAL_SENSE 0x12

/* Recommend a limit for time per CDB, independent of data length and opcode.
 *
 * Consider the limits:
 *
 *	15 seconds	// maybe quick enough to reset before you try reboot
 *	30 seconds	// maybe long enough for any disc to spin up
 *	16 hours	// maybe near the max in Win ME/ 98/ 95
 *	24 hours	// when I return tomorrow
 *	28 hours	// the 2002-09 plscsi default
 *	32 hours	// maybe near the max in Win XP/ 2K
 *	24 days		// near x7fffFFFF which might be the max ms of Mac OS X
 */

#define USUAL_S 30
#define USUAL_NS 0

/* Say much in the sign of a 32-bit exit int.
 * Say zero is ok, else positive residue, else negative trouble.
 */

#define SP_THRU		0x80000000 /* negative trouble */

/* Distinguish ways of failing when negative. */

#define SP_DATA_THRU	0x40000000 /* data not counted */
#define SP_SENSE_THRU	0x20000000 /* sense not counted */
//..... SP_LATE_THRU	0x10000000 /* timeout */

#define SP_COMPARE	0x02000000 /* copied wrong data */
#define SP_RESIDUE	0x01000000 /* copied too much or too little data */

#define SP_SENSE	0x00800000 /* credible sense not copied in */
#define SP_DEFERRED	0x00400000 /* success wrongly predicted before now */
#define SP_SK		0x000F0000 /* sk = sense key */
#define SP_ASC		0x0000FF00 /* asc = additional sense code */
#define SP_ASCQ		0x000000FF /* ascq = asc qualifier */

#define SP_SK_ASC	0x000FFF00 /* sk plus asc */ 
#define SP_SK_ASC_ASCQ	0x000FFFFF /* sk plus asc plus ascq */

/* Link with *scsi.c*. */

struct sp;
size_t sizeof_struct_sp(void);

void sp_close(struct sp * sp);
void sp_reconnect(struct sp * sp);
int sp_open(struct sp * sp, char const * name);
int sp_open_ix(struct sp * sp, int name);
void sp_perror(struct sp * sp, char const * st);

char * sp_cdb(struct sp * sp, char * cdb, int min);
char * sp_data(struct sp * sp, char * data, int max);
char * sp_sense(struct sp * sp, char * sense, int max);
int sp_late(struct sp * sp, int s, int ns);

int sp_speak(struct sp * sp);
int sp_read(struct sp * sp, char * to, int max);
int sp_write(struct sp * sp, char const * from, int max);
int sp_data_enough(struct sp * sp);
int sp_sense_enough(struct sp * sp);

/* Yes link despite C++. */

#ifdef __cplusplus
}
#endif

/* Yes compile once even if included again. */

#endif /* XSCSI_H */

/* end of file */
