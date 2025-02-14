﻿/*
 *
 * prefix.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */

#ifndef AFX_PREFIX_H__ACA0C7CC_6C96_4143_959E_98894B46E3BA__STREAM_INCLUDED_
#define AFX_PREFIX_H__ACA0C7CC_6C96_4143_959E_98894B46E3BA__STREAM_INCLUDED_

#pragma once

#ifndef WIN32

#ifndef ENABLE_BINRELOC
#define ENABLE_BINRELOC
#endif 

#ifndef BR_PTHREADS
#define  BR_PTHREADS 0 
#endif


#ifdef __cplusplus
extern "C" {
#endif

#undef BR_NAMESPACE
#define BR_NAMESPACE(funcName) funcName


#ifdef ENABLE_BINRELOC

#define br_thread_local_store BR_NAMESPACE(br_thread_local_store)
#define br_locate BR_NAMESPACE(br_locate)
#define br_locate_prefix BR_NAMESPACE(br_locate_prefix)
#define br_prepend_prefix BR_NAMESPACE(br_prepend_prefix)

#ifndef BR_NO_MACROS
	#undef SELFPATH
	#undef PREFIX
	#undef PREFIXDIR
	#undef BINDIR
	#undef SBINDIR
	#undef DATADIR
	#undef LIBDIR
	#undef LIBEXECDIR
	#undef ETCDIR
	#undef SYSCONFDIR
	#undef CONFDIR
	#undef LOCALEDIR

	#define SELFPATH	(br_thread_local_store (br_locate ((void *) "")))
	#define PREFIX		(br_thread_local_store (br_locate_prefix ((void *) "")))
	#define PREFIXDIR	(br_thread_local_store (br_locate_prefix ((void *) "")))
	#define BINDIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/bin")))
	#define SBINDIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/sbin")))
	#define DATADIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/share")))
	#define LIBDIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/lib")))
	#define LIBEXECDIR	(br_thread_local_store (br_prepend_prefix ((void *) "", "/libexec")))
	#define ETCDIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/etc")))
	#define SYSCONFDIR	(br_thread_local_store (br_prepend_prefix ((void *) "", "/etc")))
	#define CONFDIR		(br_thread_local_store (br_prepend_prefix ((void *) "", "/etc")))
	#define LOCALEDIR	(br_thread_local_store (br_prepend_prefix ((void *) "", "/share/locale")))
#endif


/* The following functions are used internally by BinReloc
   and shouldn't be used directly in applications. */

char *br_locate		(void *symbol);
char *br_locate_prefix	(void *symbol);
char *br_prepend_prefix	(void *symbol, char *path);

#endif

const char *br_thread_local_store (char *str);


/* These macros and functions are not guarded by the ENABLE_BINRELOC
 * macro because they are portable. You can use these functions.
 */

#define br_strcat BR_NAMESPACE(br_strcat)
#define br_extract_dir BR_NAMESPACE(br_extract_dir)
#define br_extract_prefix BR_NAMESPACE(br_extract_prefix)
#define br_set_locate_fallback_func BR_NAMESPACE(br_set_locate_fallback_func)

#ifndef BR_NO_MACROS
  #ifndef ENABLE_BINRELOC        
	#define BR_SELFPATH(suffix)	SELFPATH suffix
	#define BR_PREFIX(suffix)	PREFIX suffix
	#define BR_PREFIXDIR(suffix)	BR_PREFIX suffix
	#define BR_BINDIR(suffix)	BINDIR suffix
	#define BR_SBINDIR(suffix)	SBINDIR suffix
	#define BR_DATADIR(suffix)	DATADIR suffix
	#define BR_LIBDIR(suffix)	LIBDIR suffix
	#define BR_LIBEXECDIR(suffix)	LIBEXECDIR suffix
	#define BR_ETCDIR(suffix)	ETCDIR suffix
	#define BR_SYSCONFDIR(suffix)	SYSCONFDIR suffix
	#define BR_CONFDIR(suffix)	CONFDIR suffix
	#define BR_LOCALEDIR(suffix)	LOCALEDIR suffix
  #else
	#define BR_SELFPATH(suffix)	(br_thread_local_store (br_strcat (SELFPATH, suffix)))
	#define BR_PREFIX(suffix)	(br_thread_local_store (br_strcat (PREFIX, suffix)))
	#define BR_PREFIXDIR(suffix)	(br_thread_local_store (br_strcat (BR_PREFIX, suffix)))
	#define BR_BINDIR(suffix)	(br_thread_local_store (br_strcat (BINDIR, suffix)))
	#define BR_SBINDIR(suffix)	(br_thread_local_store (br_strcat (SBINDIR, suffix)))
	#define BR_DATADIR(suffix)	(br_thread_local_store (br_strcat (DATADIR, suffix)))
	#define BR_LIBDIR(suffix)	(br_thread_local_store (br_strcat (LIBDIR, suffix)))
	#define BR_LIBEXECDIR(suffix)	(br_thread_local_store (br_strcat (LIBEXECDIR, suffix)))
	#define BR_ETCDIR(suffix)	(br_thread_local_store (br_strcat (ETCDIR, suffix)))
	#define BR_SYSCONFDIR(suffix)	(br_thread_local_store (br_strcat (SYSCONFDIR, suffix)))
	#define BR_CONFDIR(suffix)	(br_thread_local_store (br_strcat (CONFDIR, suffix)))
	#define BR_LOCALEDIR(suffix)	(br_thread_local_store (br_strcat (LOCALEDIR, suffix)))        
  #endif
#endif

char *br_strcat	(const char *str1, const char *str2);
char *br_extract_dir	(const char *path);
char *br_extract_prefix(const char *path);
typedef char *(*br_locate_fallback_func) (void *symbol, void *data);
void br_set_locate_fallback_func (br_locate_fallback_func func, void *data);


#ifdef __cplusplus
}
#endif

#endif

#endif

