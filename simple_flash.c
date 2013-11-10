#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include "xscsi.h"

char *get_matshita_id (struct sp *sp)
{
   char buf[41];
   int rc;
			
   sp_cdb (sp, "\x12\x00\x00\x00\x24\x00", 6);
   rc = sp_read (sp, buf, sizeof (buf));
   buf[sizeof (buf) - 1] = '\0';
   if (rc < 0) {
     return (NULL);
   }

   if (strncasecmp (&buf[8], "MATSHITA", 8)) {
     return (NULL);
   }

   return (strdup (&buf[8]));
}

int main(int argc, char **argv)
{
  struct sp *sp;
  char sense[USUAL_SENSE], repbuf[1024], *data;
  char *id_buf,write_buffer[32816];
  unsigned char cdb[12]; 
  int ret;
  size_t off,off2,len;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <device index> <file to flash>\n",argv[0]);
    exit(1);
  }

  {
    int fd = open(argv[2],O_RDONLY,0);
    if (fd<0) { fprintf(stderr,"open for read error\n"); exit(1); }
    len = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    if (len<0x120) { fprintf(stderr,"flash data appears to be too short\n"); exit(1); }
    data = (char *)malloc(len);
    if (data==NULL) { fprintf(stderr,"error allocating memory for flash data\n"); exit(1); }
    read(fd,data,len);
    close(fd);
  }

  fprintf(stderr, "compiled at %s %s\n", __DATE__, __TIME__);
  sp = (struct sp*) calloc (1, sizeof_struct_sp ());

  ret = sp_open (sp, argv[1]);
  if (ret < 0) {
    fprintf(stderr, "Could not open device\n");
    exit(1);
  }
  sp_sense (sp, sense, sizeof (sense));
  sp_late (sp, 120, 0);

  id_buf = get_matshita_id (sp);
  if (id_buf == NULL) {
    fprintf(stderr,"Does not appear to be a matshita device\n");
    exit(1);
  }
  printf("Selected device: %s\n",id_buf);
  free(id_buf);

  printf("Continue? ");
  scanf("%s",repbuf);

  if (strcasecmp(repbuf,"y") && strcasecmp(repbuf,"yes")) {
    printf("Not confirmed, exiting\n");
    exit(0);
  }

  memcpy(write_buffer,data,0x30);
  for(off=0x30,off2=0;off<len;off+=0x8000) {
    size_t wlen = (len-off > 0x8000) ? 0x8000 : (len-off);

    wlen+=0x30;

    cdb[0]=0xea;
    cdb[1]=0x00;
    cdb[2]=0x00;
    cdb[3]=off2>>16;
    cdb[4]=off2>>8;
    cdb[5]=off2;
    cdb[6]=wlen>>16;
    cdb[7]=wlen>>8;
    cdb[8]=wlen;
    cdb[9]=0x00;
    cdb[10]=0x00;
    cdb[11]=0x00;

    wlen -= 0x30;

    sp_cdb (sp, (char *)cdb, sizeof (cdb));

    memcpy(&data[0x30],&data[0x30+off2],wlen);
    ret = sp_write (sp, data, wlen+0x30);
    if (ret != 0) {
      fprintf(stderr,"sp_write returned %d\n",ret);
      exit(1);
    }

    off2+=wlen;
  }

  bzero(cdb, sizeof (cdb));
  cdb[0] = 0xf5;

  sp_cdb (sp, (char *)cdb, sizeof (cdb));
  ret = sp_read (sp, repbuf, sizeof (repbuf));

  printf("Finished\n");

  sp_close (sp);
  free (sp);
  return (0);
}
