#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.3"

// Prefix for basically everything (also config files!)
// Use for distributing binary packages of this program (for example)
// Or just simple debugging
#define INSTALLPREFIX "/home/jonas/kawa_test"

// This is the color in which important stuff (like the occurence in kawa search)
// is highlighted. Some examples: [0;32m Green; [0;31m Red; [0;33m Yellow; [0;34m Blue
// Replace the first 0 with a 1 for bold text.
#define HIGHLIGHTCOLOR "[0;32m"

// Maximum dependency resolving depth.
#define MAXDEPTH 512

// you can use any number here, but for the fastest install times just use the number of threads you CPU has
#define THREADNUM "$(nproc)"
// ex.: #define THREADNUM "8" // for 8 jobs

// dependency types that will be included with every installation
// (by default of course, they can still be switched on/off)
// some examples: [req]uired [rec]ommended [opt]ional [doc]umentation
#define DEPTYPES "req rec"

#endif // CONFIG_H
