/*
 * fontconfig/src/fcmatcher.h
 *
 * Copyright © 2000 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the author(s) not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR(S) DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include "fcint.h"

#ifndef _FCMATCHER_H_
#define _FCMATCHER_H_

/* Define priorities to -1 for objects that don't have a compare function. */

#define PRI_NULL(n)				\
    PRI_ ## n ## _STRONG = -1,			\
    PRI_ ## n ## _WEAK = -1,
#define PRI1(n)
#define PRI_FcCompareFamily(n)		PRI1(n)
#define PRI_FcCompareString(n)		PRI1(n)
#define PRI_FcCompareNumber(n)		PRI1(n)
#define PRI_FcCompareBool(n)		PRI1(n)
#define PRI_FcCompareFilename(n)	PRI1(n)
#define PRI_FcCompareCharSet(n)		PRI1(n)
#define PRI_FcCompareLang(n)		PRI1(n)
#define PRI_FcComparePostScript(n)	PRI1(n)
#define PRI_FcCompareRange(n)		PRI1(n)
#define PRI_FcCompareSize(n)		PRI1(n)

#define FC_OBJECT(NAME, Type, Cmp)	PRI_##Cmp(NAME)

typedef enum _FcMatcherPriorityDummy {
#include "fcobjs.h"
} FcMatcherPriorityDummy;

#undef FC_OBJECT


/* Canonical match priority order. */

#undef PRI1
#define PRI1(n)					\
    PRI_ ## n,					\
    PRI_ ## n ## _STRONG = PRI_ ## n,		\
    PRI_ ## n ## _WEAK = PRI_ ## n

typedef enum _FcMatcherPriority {
    PRI1(FILE),
    PRI1(FONTFORMAT),
    PRI1(VARIABLE),
    PRI1(SCALABLE),
    PRI1(COLOR),
    PRI1(FOUNDRY),
    PRI1(CHARSET),
    PRI_FAMILY_STRONG,
    PRI_POSTSCRIPT_NAME_STRONG,
    PRI1(LANG),
    PRI_FAMILY_WEAK,
    PRI_POSTSCRIPT_NAME_WEAK,
    PRI1(SYMBOL),
    PRI1(SPACING),
    PRI1(SIZE),
    PRI1(PIXEL_SIZE),
    PRI1(STYLE),
    PRI1(SLANT),
    PRI1(WEIGHT),
    PRI1(WIDTH),
    PRI1(FONT_HAS_HINT),
    PRI1(DECORATIVE),
    PRI1(ANTIALIAS),
    PRI1(RASTERIZER),
    PRI1(OUTLINE),
    PRI1(FONTVERSION),
    PRI_END
} FcMatcherPriority;

#undef PRI1

#endif /* _FCMATCHER_H_ */
