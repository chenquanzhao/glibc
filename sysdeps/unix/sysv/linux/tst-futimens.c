/* Test for futimens
   Copyright (C) 2021 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <support/check.h>
#include <support/xunistd.h>
#include <support/temp_file.h>

#ifndef struct_stat
# define struct_stat struct stat64
#endif

static int temp_fd = -1;

/* struct timespec array with Y2038 threshold minus 2 and 1 seconds.  */
const static struct timespec t1[2] = { { 0x7FFFFFFE, 0 },  { 0x7FFFFFFF, 0 } };

#define PREPARE do_prepare
static void
do_prepare (int argc, char *argv[])
{
  temp_fd = create_temp_file ("futimensat", NULL);
  TEST_VERIFY_EXIT (temp_fd > 0);
}

static int
test_futimens_helper (const struct timespec *ts)
{
  struct_stat st;
  int result;
  time_t t;

  /* Check if we run on port with 32 bit time_t size */
  if (__builtin_add_overflow (ts->tv_sec, 0, &t))
    return 0;

  result = futimens (temp_fd, ts);
  TEST_VERIFY_EXIT (result == 0);

  xfstat (temp_fd, &st);

  /* Check if seconds for atime match */
  TEST_COMPARE (st.st_atime, ts[0].tv_sec);

  /* Check if seconds for mtime match */
  TEST_COMPARE (st.st_mtime, ts[1].tv_sec);

  return 0;
}

static int
do_test (void)
{
  test_futimens_helper (&t1[0]);

  return 0;
}

#include <support/test-driver.c>