/*
 * fontconfig/src/fcmatcher.c
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

#include "fcmatcher.h"

static double
FcCompareNumber (const FcValue *value1, const FcValue *value2, FcValue *bestValue)
{
    double  v1, v2, v;

    switch ((int) value1->type) {
    case FcTypeInteger:
	v1 = (double) value1->u.i;
	break;
    case FcTypeDouble:
	v1 = value1->u.d;
	break;
    default:
	return -1.0;
    }
    switch ((int) value2->type) {
    case FcTypeInteger:
	v2 = (double) value2->u.i;
	break;
    case FcTypeDouble:
	v2 = value2->u.d;
	break;
    default:
	return -1.0;
    }
    v = v2 - v1;
    if (v < 0)
	v = -v;
    *bestValue = FcValueCanonicalize (value2);
    return v;
}

static double
FcCompareString (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    double ret;

    *bestValue = FcValueCanonicalize (v2);
    ret = ((double) FcStrCmpIgnoreCase (FcValueString(v1), FcValueString(v2)) != 0) * 2.0;

    return ret;
}

static inline FcBool fc_iszero(double a) { return fabs (a) <= DBL_EPSILON; }

static double
FcCompareFamily (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    /* rely on the guarantee in FcPatternObjectAddWithBinding that
     * families are always FcTypeString. */
    const FcChar8* v1_string = FcValueString(v1);
    const FcChar8* v2_string = FcValueString(v2);
    int n;
    size_t len1, len2;
    double ret;

    *bestValue = FcValueCanonicalize (v2);

    if (FcToLower(*v1_string) != FcToLower(*v2_string) &&
	*v1_string != ' ' && *v2_string != ' ')
       return 3.0;

    n = FcStrMatchIgnoreCaseAndDelims (v1_string, v2_string, (const FcChar8 *)" ");
    len1 = strlen ((const char *)v1_string);
    len2 = strlen ((const char *)v2_string);

    /* Gives more score on forward matching */
    ret = (double)(len2 - FC_MIN (len2, n)) / (double)len2;
    /* Lower score if less forward matching */
    ret += (!fc_iszero (ret) * 1.0);
    /* Lower score if not exact matching */
    ret += (1.0 * ((double)(len1 - n) / (double)len1) > 0);

    return ret;
}

static double
FcComparePostScript (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    const FcChar8 *v1_string = FcValueString (v1);
    const FcChar8 *v2_string = FcValueString (v2);
    int n;
    size_t len1, len2;
    double ret;

    *bestValue = FcValueCanonicalize (v2);

    if (FcToLower (*v1_string) != FcToLower (*v2_string) &&
	*v1_string != ' ' && *v2_string != ' ')
	return 3.0;

    n = FcStrMatchIgnoreCaseAndDelims (v1_string, v2_string, (const FcChar8 *)" -");
    len1 = strlen ((const char *)v1_string);
    len2 = strlen ((const char *)v2_string);

    /* Gives more score on forward matching */
    ret = (double)(len2 - FC_MIN (len2, n)) / (double)len2;
    /* Lower score if less forward matching */
    ret += (!fc_iszero (ret) * 1.0);
    /* Lower score if not exact matching */
    ret += (1.0 * ((double)(len1 - n) / (double)len1) > 0);

    return ret;
}

double
FcCompareLang (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    FcLangResult    result;
    FcValue value1 = FcValueCanonicalize(v1), value2 = FcValueCanonicalize(v2);

    switch ((int) value1.type) {
    case FcTypeLangSet:
	switch ((int) value2.type) {
	case FcTypeLangSet:
	    result = FcLangSetCompare (value1.u.l, value2.u.l);
	    break;
	case FcTypeString:
	    result = FcLangSetHasLang (value1.u.l,
				       value2.u.s);
	    break;
	default:
	    return -1.0;
	}
	break;
    case FcTypeString:
	switch ((int) value2.type) {
	case FcTypeLangSet:
	    result = FcLangSetHasLang (value2.u.l, value1.u.s);
	    break;
	case FcTypeString:
	    result = FcLangCompare (value1.u.s,
				    value2.u.s);
	    break;
	default:
	    return -1.0;
	}
	break;
    default:
	return -1.0;
    }
    *bestValue = FcValueCanonicalize (v2);
    switch (result) {
    case FcLangEqual:
	return 0;
    case FcLangDifferentCountry:
	return 1;
    case FcLangDifferentLang:
    default:
	return 2;
    }
}

static double
FcCompareBool (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    double ret;

    if (v2->type != FcTypeBool || v1->type != FcTypeBool)
	return -1.0;

    if (v2->u.b != FcDontCare)
	*bestValue = FcValueCanonicalize (v2);
    else
	*bestValue = FcValueCanonicalize (v1);

    ret = (double) ((v2->u.b ^ v1->u.b) == 1) * 2.0;

    return ret;
}

static double
FcCompareCharSet (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    *bestValue = FcValueCanonicalize (v2); /* TODO Improve. */
    return (double) FcCharSetSubtractCount (FcValueCharSet(v1), FcValueCharSet(v2));
}

static double
FcCompareRange (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    FcValue value1 = FcValueCanonicalize (v1);
    FcValue value2 = FcValueCanonicalize (v2);
    double b1, e1, b2, e2, d;

    switch ((int) value1.type) {
    case FcTypeInteger:
        b1 = e1 = value1.u.i;
	break;
    case FcTypeDouble:
        b1 = e1 = value1.u.d;
	break;
    case FcTypeRange:
	b1 = value1.u.r->begin;
	e1 = value1.u.r->end;
	break;
    default:
	return -1;
    }
    switch ((int) value2.type) {
    case FcTypeInteger:
        b2 = e2 = value2.u.i;
	break;
    case FcTypeDouble:
        b2 = e2 = value2.u.d;
	break;
    case FcTypeRange:
	b2 = value2.u.r->begin;
	e2 = value2.u.r->end;
	break;
    default:
	return -1;
    }

    if (e1 < b2)
      d = b2;
    else if (e2 < b1)
      d = e2;
    else
      d = (FC_MAX (b1, b2) + FC_MIN (e1, e2)) * .5;

    bestValue->type = FcTypeDouble;
    bestValue->u.d = d;

    /* If the ranges overlap, it's a match, otherwise return closest distance. */
    if (e1 < b2 || e2 < b1)
	return FC_MIN (fabs (b2 - e1), fabs (b1 - e2));
    else
	return 0.0;
}

static double
FcCompareSize (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    FcValue value1 = FcValueCanonicalize (v1);
    FcValue value2 = FcValueCanonicalize (v2);
    double b1, e1, b2, e2;

    switch ((int) value1.type) {
    case FcTypeInteger:
        b1 = e1 = value1.u.i;
	break;
    case FcTypeDouble:
        b1 = e1 = value1.u.d;
	break;
    case FcTypeRange:
	abort();
	b1 = value1.u.r->begin;
	e1 = value1.u.r->end;
	break;
    default:
	return -1;
    }
    switch ((int) value2.type) {
    case FcTypeInteger:
        b2 = e2 = value2.u.i;
	break;
    case FcTypeDouble:
        b2 = e2 = value2.u.d;
	break;
    case FcTypeRange:
	b2 = value2.u.r->begin;
	e2 = value2.u.r->end;
	break;
    default:
	return -1;
    }

    bestValue->type = FcTypeDouble;
    bestValue->u.d = (b1 + e1) * .5;

    /* If the ranges overlap, it's a match, otherwise return closest distance. */
    if (e1 < b2 || e2 < b1)
	return FC_MIN (fabs (b2 - e1), fabs (b1 - e2));
    if (b2 != e2 && b1 == e2) /* Semi-closed interval. */
        return 1e-15;
    else
	return 0.0;
}

static double
FcCompareFilename (const FcValue *v1, const FcValue *v2, FcValue *bestValue)
{
    const FcChar8 *s1 = FcValueString (v1), *s2 = FcValueString (v2);
    *bestValue = FcValueCanonicalize (v2);
    if (FcStrCmp (s1, s2) == 0)
	return 0.0;
    else if (FcStrCmpIgnoreCase (s1, s2) == 0)
	return 1.0;
    else if (FcStrGlobMatch (s1, s2))
	return 2.0;
    else
	return 3.0;
}

/*
 * Order is significant, it defines the precedence of
 * each value, earlier values are more significant than
 * later values
 */
#define FC_OBJECT(NAME, Type, Cmp)	{ FC_##NAME##_OBJECT,	Cmp,	PRI_##NAME##_STRONG,	PRI_##NAME##_WEAK },
static const FcMatcher _FcMatchers [] = {
    { FC_INVALID_OBJECT, NULL, -1, -1 },
#include "fcobjs.h"
};
#undef FC_OBJECT

const FcMatcher*
FcObjectToMatcher (FcObject object,
		   FcBool   include_lang)
{
    if (include_lang)
    {
	switch (object) {
	case FC_FAMILYLANG_OBJECT:
	case FC_STYLELANG_OBJECT:
	case FC_FULLNAMELANG_OBJECT:
	    object = FC_LANG_OBJECT;
	    break;
	}
    }
    if (object > FC_MAX_BASE_OBJECT ||
	!_FcMatchers[object].compare ||
	_FcMatchers[object].strong == -1 ||
	_FcMatchers[object].weak == -1)
	return NULL;

    return _FcMatchers + object;
}
