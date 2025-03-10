/* uname.c - return system name
 *
 * Copyright 2008 Rob Landley <rob@landley.net>
 *
 * See http://opengroup.org/onlinepubs/9699919799/utilities/uname.html

USE_UNAME(NEWTOY(uname, "oamvrns", TOYFLAG_BIN))
USE_ARCH(NEWTOY(arch, 0, TOYFLAG_USR|TOYFLAG_BIN))

config ARCH 
  bool "arch"
  default y
  help
    usage: arch

    Print machine (hardware) name, same as uname -m.

config UNAME
  bool "uname"
  default y
  help
    usage: uname [-asnrvm]

    Print system information.

    -s	System name
    -n	Network (domain) name
    -r	Kernel Release number
    -v	Kernel Version 
    -m	Machine (hardware) name
    -a	All of the above
*/

#define FOR_uname
#define FORCE_FLAGS
#include "toys.h"

// If a 32 bit x86 build environment working in a chroot under an x86-64
// kernel returns x86_64 for -m it confuses ./configure.  Special case it.

#if defined(__i686__)
#define GROSS "i686"
#elif defined(__i586__)
#define GROSS "i586"
#elif defined(__i486__)
#define GROSS "i486"
#elif defined(__i386__)
#define GROSS "i386"
#endif

void uname_main(void)
{
  int i, flags = toys.optflags, needspace=0;
  struct utsname u;

  uname(&u);
  if (FLAG(o))
#ifdef __ANDROID__
    printf("Android");
#else
    flags |= FLAG_s;
#endif
  if (!flags) flags = FLAG_s;
  for (i=0; i<5; i++) {
    char *c = ((char *) &u)+(sizeof(u.sysname)*i);

    if (flags & ((1<<i)|FLAG_a)) {
      int len = strlen(c);

      // This problem originates in autoconf, so of course the solution
      // is horribly ugly.
#ifdef GROSS
      if (i==4 && !strcmp(c,"x86_64")) {
        printf(GROSS);
        continue;
      }
#endif

      if (needspace++) {
        // We can't decrement on the first entry, because
        // needspace would be 0
        *(--c)=' ';
        len++;
      }
      xwrite(1, c, len);
    }
  }
  putchar('\n');
}

void arch_main(void)
{
  toys.optflags = FLAG_m;
  uname_main();
}
