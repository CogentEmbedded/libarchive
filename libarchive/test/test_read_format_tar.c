/*-
 * Copyright (c) 2003-2007 Tim Kientzle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "test.h"

/*
 * Each of these archives is a short archive with a single entry.  The
 * corresponding verify function verifies the entry structure returned
 * from libarchive is what it should be.  The support functions pad with
 * lots of zeros, so we can trim trailing zero bytes from each hardcoded
 * archive to save space.
 *
 * The naming here follows the tar file type flags.  E.g. '1' is a hardlink,
 * '2' is a symlink, '5' is a dir, etc.
 */

/* Empty archive. */
static unsigned char archiveEmpty[] = {
	/* 512 zero bytes */
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};

static void verifyEmpty(void)
{
	struct archive_entry *ae;
	struct archive *a;

	assert((a = archive_read_new()) != NULL);
	assertA(0 == archive_read_support_filter_all(a));
	assertA(0 == archive_read_support_format_all(a));
	assertA(0 == archive_read_open_memory(a, archiveEmpty, 512));
	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
	assertEqualInt(archive_filter_code(a, 0), ARCHIVE_FILTER_NONE);
	assertEqualString(archive_filter_name(a, 0), "none");
	failure("512 zero bytes should be recognized as a tar archive.");
	assertEqualInt(archive_format(a), ARCHIVE_FORMAT_TAR);
	assertEqualInt(archive_entry_is_encrypted(ae), 0);
	assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

/* Single entry with a hardlink. */
static unsigned char archive1[] = {
'h','a','r','d','l','i','n','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0',
'0','6','4','4',' ',0,'0','0','1','7','5','0',' ',0,'0','0','1','7','5','0',
' ',0,'0','0','0','0','0','0','0','0','0','0','0',' ','1','0','6','4','6',
'0','5','2','6','6','2',' ','0','1','3','0','5','7',0,' ','1','f','i','l',
'e',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,'0',
'0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
't','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0',
'0','0','0','0','0',' ',0,'0','0','0','0','0','0',' '};

static void verify1(struct archive_entry *ae)
{
	/* A hardlink is not a symlink. */
	assert(archive_entry_filetype(ae) != AE_IFLNK);
	/* Nor is it a directory. */
	assert(archive_entry_filetype(ae) != AE_IFDIR);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0644);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "hardlink");
	assertEqualString(archive_entry_hardlink(ae), "file");
	assert(archive_entry_symlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184388530);
}

/* Verify that symlinks are read correctly. */
static unsigned char archive2[] = {
's','y','m','l','i','n','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0',
'0','0','7','5','5',' ','0','0','0','1','7','5','0',' ','0','0','0','1','7',
'5','0',' ','0','0','0','0','0','0','0','0','0','0','0',' ','1','0','6','4',
'6','0','5','4','1','0','1',' ','0','0','1','3','3','2','3',' ','2','f','i',
'l','e',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,
'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'0','0','0','0','0','0','0',' ','0','0','0','0','0','0','0',' '};

static void verify2(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "symlink");
	assertEqualString(archive_entry_symlink(ae), "file");
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184389185);
}

/* Character device node. */
static unsigned char archive3[] = {
'd','e','v','c','h','a','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0',
'0','0','7','5','5',' ','0','0','0','1','7','5','0',' ','0','0','0','1','7',
'5','0',' ','0','0','0','0','0','0','0','0','0','0','0',' ','1','0','6','4',
'6','0','5','4','1','0','1',' ','0','0','1','2','4','1','2',' ','3',0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,
'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'0','0','0','0','0','0','0',' ','0','0','0','0','0','0','0',' '};

static void verify3(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFCHR);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "devchar");
	assert(archive_entry_symlink(ae) == NULL);
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184389185);
}

/* Block device node. */
static unsigned char archive4[] = {
'd','e','v','b','l','o','c','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0',
'0','0','7','5','5',' ','0','0','0','1','7','5','0',' ','0','0','0','1','7',
'5','0',' ','0','0','0','0','0','0','0','0','0','0','0',' ','1','0','6','4',
'6','0','5','4','1','0','1',' ','0','0','1','2','5','7','0',' ','4',0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,
'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'0','0','0','0','0','0','0',' ','0','0','0','0','0','0','0',' '};

static void verify4(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFBLK);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "devblock");
	assert(archive_entry_symlink(ae) == NULL);
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184389185);
}

/* Directory. */
static unsigned char archive5[] = {
'.',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0','0',
'7','5','5',' ',0,'0','0','1','7','5','0',' ',0,'0','0','1','7','5','0',
' ',0,'0','0','0','0','0','0','0','0','0','0','0',' ','1','0','3','3',
'4','0','4','1','7','3','6',' ','0','1','0','5','6','1',0,' ','5',0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,
'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'0','0','0','0','0','0',' ',0,'0','0','0','0','0','0',' '};

static void verify5(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFDIR);
	assertEqualInt(archive_entry_mtime(ae), 1131430878);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
}

/* fifo */
static unsigned char archive6[] = {
'f','i','f','o',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0',
'0','0','7','5','5',' ','0','0','0','1','7','5','0',' ','0','0','0','1','7',
'5','0',' ','0','0','0','0','0','0','0','0','0','0','0',' ','1','0','6','4',
'6','0','5','4','1','0','1',' ','0','0','1','1','7','2','4',' ','6',0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',0,
'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'0','0','0','0','0','0','0',' ','0','0','0','0','0','0','0',' '};

static void verify6(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFIFO);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "fifo");
	assert(archive_entry_symlink(ae) == NULL);
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184389185);
}

/* GNU long link name */
static unsigned char archiveK[] = {
'.','/','.','/','@','L','o','n','g','L','i','n','k',0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'0','0','0','0','0','0','0',0,'0','0','0','0','0','0','0',0,'0','0','0',
'0','0','0','0',0,'0','0','0','0','0','0','0','0','6','6','6',0,'0','0','0',
'0','0','0','0','0','0','0','0',0,'0','1','1','7','1','5',0,' ','K',0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u','s','t','a','r',' ',' ',
0,'r','o','o','t',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'w','h','e','e','l',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'t',
'h','i','s','_','i','s','_','a','_','v','e','r','y','_','l','o','n','g','_',
's','y','m','l','i','n','k','_','b','o','d','y','_','a','b','c','d','e','f',
'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y',
'z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q',
'r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i',
'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a',
'b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t',
'u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l',
'm','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d',
'e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w',
'x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g',
'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
'_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r',
's','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j',
'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b',
'c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u',
'v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m',
'n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e',
'f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x',
'y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p',
'q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g','h',
'i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
's','y','m','l','i','n','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','1',
'2','0','7','5','5',0,'0','0','0','1','7','5','0',0,'0','0','0','1','7','5',
'0',0,'0','0','0','0','0','0','0','0','0','0','0',0,'1','0','6','4','6','0',
'5','6','7','7','0',0,'0','3','5','4','4','7',0,' ','2','t','h','i','s','_',
'i','s','_','a','_','v','e','r','y','_','l','o','n','g','_','s','y','m','l',
'i','n','k','_','b','o','d','y','_','a','b','c','d','e','f','g','h','i','j',
'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b',
'c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u',
'v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l',0,
'u','s','t','a','r',' ',' ',0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,'t','i','m'};

static void verifyK(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "symlink");
	assertEqualString(archive_entry_symlink(ae),
	    "this_is_a_very_long_symlink_body_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz");
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184390648);
}

/* TODO: GNU long name */

/* TODO: Solaris ACL */

/* Pax extended long link name */
static unsigned char archivexL[] = {
'.','/','P','a','x','H','e','a','d','e','r','s','.','8','6','9','7','5','/',
's','y','m','l','i','n','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0','0','0','6','4','4',0,'0','0','0','1',
'7','5','0',0,'0','0','0','1','7','5','0',0,'0','0','0','0','0','0','0','0',
'7','5','3',0,'1','0','6','4','6','0','5','7','6','1','1',0,'0','1','3','7',
'1','4',0,' ','x',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'u',
's','t','a','r',0,'0','0',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,'0','0','0','0','0','0','0',0,'0','0','0','0','0','0','0',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'4','5','1',' ','l','i','n','k','p','a','t',
'h','=','t','h','i','s','_','i','s','_','a','_','v','e','r','y','_','l','o',
'n','g','_','s','y','m','l','i','n','k','_','b','o','d','y','_','a','b','c',
'd','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
'w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n',
'o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f',
'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y',
'z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q',
'r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i',
'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a',
'b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t',
'u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l',
'm','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d',
'e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w',
'x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g',
'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
'_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r',
's','t','u','v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j',
'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b',
'c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u',
'v','w','x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m',
'n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d','e',
'f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x',
'y','z',10,'2','0',' ','a','t','i','m','e','=','1','1','8','4','3','9','1',
'0','2','5',10,'2','0',' ','c','t','i','m','e','=','1','1','8','4','3','9',
'0','6','4','8',10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'s','y','m',
'l','i','n','k',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0','0','0','0','7',
'5','5',0,'0','0','0','1','7','5','0',0,'0','0','0','1','7','5','0',0,'0',
'0','0','0','0','0','0','0','0','0','0',0,'1','0','6','4','6','0','5','6',
'7','7','0',0,'0','3','7','1','2','1',0,' ','2','t','h','i','s','_','i','s',
'_','a','_','v','e','r','y','_','l','o','n','g','_','s','y','m','l','i','n',
'k','_','b','o','d','y','_','a','b','c','d','e','f','g','h','i','j','k','l',
'm','n','o','p','q','r','s','t','u','v','w','x','y','z','_','a','b','c','d',
'e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w',
'x','y','z','_','a','b','c','d','e','f','g','h','i','j','k','l','m','u','s',
't','a','r',0,'0','0','t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,'t','i','m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,'0','0','0','0','0','0','0',0,'0','0','0','0','0','0','0'};

static void verifyxL(struct archive_entry *ae)
{
	assertEqualInt(archive_entry_filetype(ae), AE_IFLNK);
	assertEqualInt(archive_entry_mode(ae) & 0777, 0755);
	assertEqualInt(archive_entry_uid(ae), 1000);
	assertEqualInt(archive_entry_gid(ae), 1000);
	assertEqualString(archive_entry_uname(ae), "tim");
	assertEqualString(archive_entry_gname(ae), "tim");
	assertEqualString(archive_entry_pathname(ae), "symlink");
	assertEqualString(archive_entry_symlink(ae),
	    "this_is_a_very_long_symlink_body_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
	    "abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz");
	assert(archive_entry_hardlink(ae) == NULL);
	assertEqualInt(archive_entry_mtime(ae), 1184390648);
}


/* TODO: Any other types of headers? */

static void verify(unsigned char *d, size_t s,
    void (*f)(struct archive_entry *),
    int compression, int format)
{
	struct archive_entry *ae;
	struct archive *a;
	unsigned char *buff = malloc(100000);

	memcpy(buff, d, s);
	memset(buff + s, 0, 2048);

	assert((a = archive_read_new()) != NULL);
	assertA(0 == archive_read_support_filter_all(a));
	assertA(0 == archive_read_support_format_all(a));
	assertA(0 == archive_read_open_memory(a, buff, s + 1024));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualInt(archive_filter_code(a, 0), compression);
	assertEqualInt(archive_format(a), format);
	assertEqualInt(archive_entry_is_encrypted(ae), 0);
	assertEqualIntA(a, archive_read_has_encrypted_entries(a), ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED);

	/* Verify the only entry. */
	f(ae);

	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
	free(buff);
}

#define SMALL_ADVANCE 67
static void
check_tar_seek(struct archive *a, const unsigned char *payload,
    size_t payload_size, int64_t offset)
{
	unsigned char buff[SMALL_ADVANCE];
	size_t expected;
	ssize_t bytes_read;

	expected = payload_size - (size_t)offset;
	if (expected > sizeof(buff))
		expected = sizeof(buff);

	bytes_read = archive_read_data(a, buff, sizeof(buff));
	if (expected == 0) {
		assertEqualInt(0, bytes_read);
		return;
	}

	assertEqualInt((int)expected, bytes_read);
	assertEqualMem(buff, payload + offset, expected);
}

static void
read_whole_tar_entry(
    struct archive *a, unsigned char *payload, size_t payload_size)
{
	unsigned char dummy;
	size_t total;
	ssize_t bytes_read;

	total = 0;
	while (total < payload_size) {
		bytes_read =
		    archive_read_data(a, payload + total, payload_size - total);
		assert(bytes_read >= 0);
		total += bytes_read;
	}
	assertEqualInt((int)payload_size, (int)total);
	assertEqualInt(0, archive_read_data(a, &dummy, 1));
}

static void
check_tar_sequential(
    struct archive *a, const unsigned char *payload, size_t payload_size)
{
	unsigned char buff[4096];
	size_t total;
	ssize_t bytes_read;
	size_t remaining;

	total = 0;
	while (1) {
		bytes_read = archive_read_data(a, buff, sizeof(buff));
		assert(bytes_read >= 0);
		if (bytes_read == 0)
			break;
		assert(total <= payload_size);
		remaining = payload_size - total;
		assert((size_t)bytes_read <= remaining);
		assertEqualMem(buff, payload + total, (size_t)bytes_read);
		total += (size_t)bytes_read;
	}
	assertEqualInt((int)payload_size, (int)total);
}

struct synthetic_tar_entry {
	const char *pathname;
	const char *uname;
	const char *gname;
	int64_t size;
	mode_t perm;
	int64_t mtime;
	long mtime_nsec;
};

struct test_rng {
	uint32_t values[256];
	size_t next;
};

static void
test_rng_init(struct test_rng *rng)
{
	rng->next = sizeof(rng->values) / sizeof(rng->values[0]);
}

static uint32_t
test_rng_next(struct test_rng *rng)
{
	if (rng->next >= sizeof(rng->values) / sizeof(rng->values[0])) {
		fill_with_pseudorandom_data(rng->values, sizeof(rng->values));
		rng->next = 0;
	}
	return (rng->values[rng->next++]);
}

static void
run_tar_seek_checks(struct archive *a, const unsigned char *payload,
    size_t payload_size, struct test_rng *rng)
{
	int i;

	assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
	check_tar_sequential(a, payload, payload_size);
	assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
	check_tar_sequential(a, payload, payload_size);

	assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
	check_tar_seek(a, payload, payload_size, 0);
	assertEqualInt((int)payload_size, archive_seek_data(a, 0, SEEK_END));
	check_tar_seek(a, payload, payload_size, payload_size);

	for (i = 0; i < 64; i++) {
		int64_t current;
		int64_t target;
		int64_t delta;

		target = test_rng_next(rng) % (payload_size + 1);
		assertEqualInt(target, archive_seek_data(a, target, SEEK_SET));
		check_tar_seek(a, payload, payload_size, target);

		current = target;
		if (current < (int64_t)payload_size) {
			size_t advanced = payload_size - (size_t)current;
			if (advanced > SMALL_ADVANCE)
				advanced = SMALL_ADVANCE;
			current += advanced;
		}

		delta = (test_rng_next(rng) % (payload_size + 1)) - current;
		target = current + delta;
		assertEqualInt(target, archive_seek_data(a, delta, SEEK_CUR));
		check_tar_seek(a, payload, payload_size, target);

		current = target;
		if (current < (int64_t)payload_size) {
			size_t advanced = payload_size - (size_t)current;
			if (advanced > SMALL_ADVANCE)
				advanced = SMALL_ADVANCE;
			current += advanced;
		}

		target = test_rng_next(rng) % (payload_size + 1);
		assertEqualInt(
		    target, archive_seek_data(
		                a, target - (int64_t)payload_size, SEEK_END));
		check_tar_seek(a, payload, payload_size, target);
	}
}
#undef SMALL_ADVANCE

DEFINE_TEST(test_read_format_tar_seek_data)
{
	static const struct synthetic_tar_entry entries[] = {
		{ "seek/empty.bin", "reader", "archive", 0, 0644, 1712836001, 0 },
		{ "seek/small.bin", "reader", "archive", 38, 0600, 1712836002, 111111111 },
		{ "seek/block-minus-one.bin", "reader", "archive", 511, 0640, 1712836003, 222222222 },
		{ "seek/block.bin", "reader-with-a-deliberately-long-name", "group", 512, 0644, 1712836004, 333333333 },
		{ "seek/block-plus-one.bin", "user", "group-with-a-deliberately-long-name", 513, 0664, 1712836005, 444444444 },
		{ "seek/abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_tail.bin", "reader", "archive", 4269, 0644, 1712836006, 555555555 }
	};
	struct archive *a;
	struct archive *aw;
	struct archive_entry *ae;
	unsigned char *archive_buff;
	unsigned char **dumped;
	size_t archive_size, used;
	struct test_rng rng;
	size_t i;

	archive_size = 128 * 1024;
	archive_buff = malloc(archive_size);
	dumped = calloc(sizeof(entries) / sizeof(entries[0]), sizeof(*dumped));
	assert(archive_buff != NULL);
	assert(dumped != NULL);

	assert((aw = archive_write_new()) != NULL);
	assertEqualIntA(aw, ARCHIVE_OK, archive_write_add_filter_none(aw));
	assertEqualIntA(aw, ARCHIVE_OK, archive_write_set_format_pax(aw));
	assertEqualIntA(aw, ARCHIVE_OK,
	    archive_write_open_memory(aw, archive_buff, archive_size, &used));

	for (i = 0; i < sizeof(entries) / sizeof(entries[0]); i++) {
		assert((ae = archive_entry_new()) != NULL);
		archive_entry_set_pathname(ae, entries[i].pathname);
		archive_entry_set_filetype(ae, AE_IFREG);
		archive_entry_set_perm(ae, entries[i].perm);
		archive_entry_set_size(ae, entries[i].size);
		archive_entry_set_uid(ae, 1000 + (int)i);
		archive_entry_set_gid(ae, 2000 + (int)i);
		archive_entry_set_uname(ae, entries[i].uname);
		archive_entry_set_gname(ae, entries[i].gname);
		archive_entry_set_mtime(
		    ae, entries[i].mtime, entries[i].mtime_nsec);
		if (entries[i].size > 0) {
			dumped[i] = malloc((size_t)entries[i].size);
			assert(dumped[i] != NULL);
			fill_with_pseudorandom_data(
			    dumped[i], (size_t)entries[i].size);
		}
		assertEqualIntA(aw, ARCHIVE_OK, archive_write_header(aw, ae));
		if (entries[i].size > 0) {
			assertEqualIntA(aw, (int)entries[i].size,
			    archive_write_data(
			        aw, dumped[i], (size_t)entries[i].size));
		}
		archive_entry_free(ae);
	}
	assertEqualIntA(aw, ARCHIVE_OK, archive_write_close(aw));
	assertEqualInt(ARCHIVE_OK, archive_write_free(aw));

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
	assertEqualIntA(
	    a, ARCHIVE_OK, read_open_memory_seek(a, archive_buff, used, 113));

	test_rng_init(&rng);
	for (i = 0; i < sizeof(entries) / sizeof(entries[0]); i++) {
		static const unsigned char empty_payload[1];
		const unsigned char *payload = dumped[i];

		assertEqualIntA(
		    a, ARCHIVE_OK, archive_read_next_header(a, &ae));
		assertEqualString(
		    entries[i].pathname, archive_entry_pathname(ae));
		assertEqualInt((int)entries[i].size, archive_entry_size(ae));

		if (payload == NULL)
			payload = empty_payload;
		read_whole_tar_entry(a, dumped[i], (size_t)entries[i].size);
		run_tar_seek_checks(a, payload, (size_t)entries[i].size, &rng);
		assertEqualIntA(a, ARCHIVE_OK, archive_read_data_skip(a));
	}
	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
	for (i = 0; i < sizeof(entries) / sizeof(entries[0]); i++)
		free(dumped[i]);
	free(dumped);
	free(archive_buff);
}

DEFINE_TEST(test_read_format_tar_seek_data_sparse)
{
	const char *refname = "test_read_format_gtar_sparse_1_17_posix10.tar";
	struct archive *a;
	struct archive_entry *ae;
	int sparse_count;

	extract_reference_file(refname);

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_tar(a));
	assertEqualIntA(
	    a, ARCHIVE_OK, archive_read_open_filename(a, refname, 3));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("sparse", archive_entry_pathname(ae));
	sparse_count = archive_entry_sparse_reset(ae);
	assert(sparse_count > 1);

	assertEqualInt(ARCHIVE_FAILED, archive_seek_data(a, 0, SEEK_SET));
	assertEqualString(
	    "Seeking of non-contiguous tar entries is unsupported",
	    archive_error_string(a));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

static void
dump_tar_entry(struct archive *a, int fd, int64_t entry_size)
{
	char buff[65536];
	int64_t remaining;
	ssize_t bytes_read;
	ssize_t bytes_written;
	size_t to_read;

	remaining = entry_size;
	while (remaining > 0) {
		to_read = (size_t)remaining;
		if (to_read > sizeof(buff))
			to_read = sizeof(buff);
		bytes_read = archive_read_data(a, buff, to_read);
		assert(bytes_read >= 0);
		assert(bytes_read > 0);
		bytes_written = write(fd, buff, bytes_read);
		assertEqualInt((int)bytes_read, bytes_written);
		remaining -= bytes_read;
	}
	assertEqualInt(0, archive_read_data(a, buff, 1));
}

#if defined(_WIN32)
static ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset)
{
	const off_t r = lseek(fildes, offset, SEEK_SET);
	if (r == -1)
	{
		return -1;
	}
	return read(fildes, buf, nbyte);
}
#endif

static void
check_tar_seek_dump(
    struct archive *a, int fd, int64_t entry_size, int64_t offset)
{
	char actual[4096];
	char expected[4096];
	size_t to_read;
	ssize_t actual_read, expected_read;

	to_read = (size_t)(entry_size - offset);
	if (to_read > sizeof(actual))
		to_read = sizeof(actual);

	actual_read = archive_read_data(a, actual, to_read);
	if (to_read == 0) {
		assertEqualInt(0, actual_read);
		return;
	}
	assertEqualInt((int)to_read, actual_read);

	expected_read = pread(fd, expected, to_read, offset);
	assertEqualInt((int)to_read, expected_read);
	assertEqualMem(actual, expected, to_read);
}

static void
check_tar_sequential_matches_dump(struct archive *a, int fd, int64_t entry_size)
{
	char actual[4096];
	char expected[4096];
	int64_t total;
	ssize_t actual_read, expected_read;
	size_t to_read;

	total = 0;
	while (1) {
		actual_read = archive_read_data(a, actual, sizeof(actual));
		assert(actual_read >= 0);
		if (actual_read == 0)
			break;
		to_read = (size_t)actual_read;
		expected_read = pread(fd, expected, to_read, total);
		assertEqualInt((int)to_read, expected_read);
		assertEqualMem(actual, expected, to_read);
		total += actual_read;
	}
	assertEqualInt(entry_size, total);
}

DEFINE_TEST(test_read_format_tar_seek_data_h264_archive)
{
	const char *pathname;
	struct archive *a;
	struct archive_entry *ae;
	int dumpfd;
	int files_tested;
	struct test_rng rng;

	pathname = getenv("TAR_SEEK_ARCHIVE_PATH");
	if (pathname == NULL || pathname[0] == '\0') {
		skipping("no env variable defined, skipping the test");
		return;
	}

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
	assertEqualIntA(
	    a, ARCHIVE_OK, archive_read_open_filename(a, pathname, 10240));

	files_tested = 0;
	test_rng_init(&rng);
	while (archive_read_next_header(a, &ae) == ARCHIVE_OK) {
		char dumpname[64];
		int64_t entry_size;
		int i;

		if (archive_entry_filetype(ae) != AE_IFREG) {
			assertEqualIntA(
			    a, ARCHIVE_OK, archive_read_data_skip(a));
			continue;
		}

		entry_size = archive_entry_size(ae);
		files_tested++;
		snprintf(dumpname, sizeof(dumpname), "tar-seek-dump-%d.bin",
		    files_tested);
		dumpfd =
		    open(dumpname, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0644);
		assert(dumpfd >= 0);

		dump_tar_entry(a, dumpfd, entry_size);

		assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
		check_tar_sequential_matches_dump(a, dumpfd, entry_size);
		assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
		check_tar_sequential_matches_dump(a, dumpfd, entry_size);

		assertEqualInt(0, archive_seek_data(a, 0, SEEK_SET));
		check_tar_seek_dump(a, dumpfd, entry_size, 0);

		assertEqualInt(entry_size, archive_seek_data(a, 0, SEEK_END));
		check_tar_seek_dump(a, dumpfd, entry_size, entry_size);

		for (i = 0; i < 8; i++) {
			int64_t target;
			int64_t current;
			int64_t delta;

			target = test_rng_next(&rng) % (entry_size + 1);
			assertEqualInt(
			    target, archive_seek_data(a, target, SEEK_SET));
			check_tar_seek_dump(a, dumpfd, entry_size, target);

			current = target;
			if (current < entry_size) {
				size_t advanced =
				    (size_t)(entry_size - current);
				if (advanced > 4096)
					advanced = 4096;
				current += advanced;
			}

			delta =
			    (test_rng_next(&rng) % (entry_size + 1)) - current;
			target = current + delta;
			assertEqualInt(
			    target, archive_seek_data(a, delta, SEEK_CUR));
			check_tar_seek_dump(a, dumpfd, entry_size, target);

			target = test_rng_next(&rng) % (entry_size + 1);
			assertEqualInt(
			    target, archive_seek_data(
			                a, target - entry_size, SEEK_END));
			check_tar_seek_dump(a, dumpfd, entry_size, target);
		}

		assertEqualInt(0, close(dumpfd));
		assertEqualInt(0, unlink(dumpname));
		assertEqualIntA(a, ARCHIVE_OK, archive_read_data_skip(a));
	}

	assert(files_tested > 0);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_read_format_tar)
{
	verifyEmpty();
	verify(archive1, sizeof(archive1), verify1,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archive2, sizeof(archive2), verify2,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archive3, sizeof(archive3), verify3,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archive4, sizeof(archive4), verify4,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archive5, sizeof(archive5), verify5,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archive6, sizeof(archive6), verify6,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_USTAR);
	verify(archiveK, sizeof(archiveK), verifyK,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_GNUTAR);
	verify(archivexL, sizeof(archivexL), verifyxL,
	    ARCHIVE_FILTER_NONE, ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE);
}
