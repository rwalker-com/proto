#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/inotify.h>

static const char* argv0;

#define ERRBAIL(fmt,...) do { fprintf(stderr, "%s: error: "fmt"\n", argv0, __VA_ARGS__); exit(1); } while (0)

#include <string.h>
#define EVT_MASK_TO_STR_SIZE \
sizeof(\
"|IN_ACCESS"\
"|IN_MODIFY"\
"|IN_ATTRIB"\
"|IN_CLOSE_WRITE"\
"|IN_CLOSE_NOWRITE"\
"|IN_OPEN"\
"|IN_MOVED_FROM"\
"|IN_MOVED_TO"\
"|IN_DELETE"\
"|IN_DELETE_SELF"\
"|IN_ISDIR"\
"|IN_ONESHOT"\
"|0x00000000")

static const char* evt_mask_to_string(uint32_t mask, char* out)
{

#define IFSETCAT(x) if (mask&x) { strcat(out,"|"#x); }; mask &= ~x

   if (!mask) {
      strcpy(out, "0x00000000");
   } else {
      *out = '\0';

      IFSETCAT(IN_ACCESS);
      IFSETCAT(IN_MODIFY);
      IFSETCAT(IN_ATTRIB);
      IFSETCAT(IN_CLOSE_WRITE);
      IFSETCAT(IN_CLOSE_NOWRITE);
      IFSETCAT(IN_OPEN);
      IFSETCAT(IN_MOVED_FROM);
      IFSETCAT(IN_MOVED_TO);
      IFSETCAT(IN_DELETE);
      IFSETCAT(IN_DELETE_SELF);
      IFSETCAT(IN_ISDIR);
      IFSETCAT(IN_ONESHOT);

      if (mask) {
         char unkbits[sizeof("|0x00000000")];
         sprintf(unkbits, "|0x%08x", mask);
         strcat(out, unkbits);
      }

      memmove(out, out+1, strlen(out));
   }

   return out;
}

int main(int argc, const char **argv)
{
   argv0 = argv[0];

   if (argc < 2) {
      ERRBAIL("no paths supplied\n"
              "Usage: %s <PATH>...\n"
              "   %s sets up inotifiers for each PATH specified, information \n"
              "   about changes is printed to stdout\n",
              argv0, argv0);
   }
   argv++;
   argc--;

   {
      int fd = inotify_init();

      if (fd < 0) {
         ERRBAIL("could not initialize inotify, errno %d", errno);
      }

      int *wds = (int*)malloc(sizeof(int)*(argc-1));

      {
         int i;
         for (i = 0; i < argc; i++) {
            wds[i] = inotify_add_watch(fd, argv[i], IN_ALL_EVENTS);
            if (wds[i] < 0) {
               perror("could not add watch");
               ERRBAIL("could not add watch for %s, errno %d",
                       argv[i], errno);
            }
         }
      }

      {
         int   buflen = 1024;
         char* buf    = (char*)malloc(sizeof(char)*buflen);

         for (;;) {
            int n;

            while ((n = read(fd, buf, buflen)) < 0 && (EINTR == errno));

            if (0 == n || (n < 0 && EINVAL == errno)) {
               buflen *= 2;
               buf     = (char*)realloc(buf, sizeof(char)*buflen);
               fprintf(stderr, "growing buf to %d\n", buflen);
            } else if (n < 0) {
               ERRBAIL("read event failed %d", errno);
            } else {
               char* p = buf;
               while (n >= sizeof(struct inotify_event)) {
                  struct inotify_event* evt = (struct inotify_event*)p;
                  char combo[EVT_MASK_TO_STR_SIZE];
                  fprintf(stdout,
                          "-----------------------------\n"
                          "wd:      0x%08x\n"
                          "mask:    %s\n"
                          "cookie:  0x%08x\n"
                          "len:     %d\n"
                          "name:    %s\n",
                          evt->wd,
                          evt_mask_to_string(evt->mask, combo),
                          evt->cookie,
                          evt->len,
                          evt->len ? evt->name : "");
                  {
                     int eaten = evt->len + sizeof(struct inotify_event);
                     n -= eaten;
                     p += eaten;
                  }
               }
               fprintf(stdout,
                       "=============================\n");
            }
         }
      }

      free(wds);
   }
}
