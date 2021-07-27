/**
 * this file is part of kawa
 * Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
 *
 * kawa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kawa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kawa.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.3"

// Prefix for basically everything (also config files!)
// Use for distributing binary packages of this program (for example)
// Or just simple debugging
#define INSTALLPREFIX "/home/emily/kawa_test"

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

// the maximum amount of times kawa will retry after a failed package
// download. this is the amount of retries, meaning that if the package
// download fails, kawa will then attempt the download n more times,
// if you care about the exact number.
#define MAXDOWNLOADRETRY 2

#endif // CONFIG_H
