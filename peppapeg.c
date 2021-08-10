/*
 * Peppa PEG -  Ultra lightweight PEG Parser in ANSI C.
 *
 * MIT License
 *
 * Copyright (c) 2021 Ju
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

/** Start of khash.h */
/* The MIT License
   Copyright (c) 2008, 2009, 2011 by Attractive Chaos <attractor@live.co.uk>
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#ifndef __AC_KHASH_H
#define __AC_KHASH_H
#define AC_VERSION_KHASH_H "0.2.8"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#if UINT_MAX == 0xffffffffu
typedef unsigned int khint32_t;
#elif ULONG_MAX == 0xffffffffu
typedef unsigned long khint32_t;
#endif

#if ULONG_MAX == ULLONG_MAX
typedef unsigned long khint64_t;
#else
typedef unsigned long long khint64_t;
#endif

#ifndef kh_inline
#ifdef _MSC_VER
#define kh_inline __inline
#else
/* #define kh_inline inline */ /* ANSI has no inline. */
#define kh_inline
#endif
#endif /* kh_inline */

#ifndef klib_unused
#if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
#define klib_unused __attribute__ ((__unused__))
#else
#define klib_unused
#endif
#endif /* klib_unused */

typedef khint32_t khint_t;
typedef khint_t khiter_t;

#define __ac_isempty(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&2)
#define __ac_isdel(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&1)
#define __ac_iseither(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&3)
#define __ac_set_isdel_false(flag, i) (flag[i>>4]&=~(1ul<<((i&0xfU)<<1)))
#define __ac_set_isempty_false(flag, i) (flag[i>>4]&=~(2ul<<((i&0xfU)<<1)))
#define __ac_set_isboth_false(flag, i) (flag[i>>4]&=~(3ul<<((i&0xfU)<<1)))
#define __ac_set_isdel_true(flag, i) (flag[i>>4]|=1ul<<((i&0xfU)<<1))

#define __ac_fsize(m) ((m) < 16? 1 : (m)>>4)

#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef kcalloc
#define kcalloc(N,Z) calloc(N,Z)
#endif
#ifndef kmalloc
#define kmalloc(Z) malloc(Z)
#endif
#ifndef krealloc
#define krealloc(P,Z) realloc(P,Z)
#endif
#ifndef kfree
#define kfree(P) free(P)
#endif

static const double __ac_HASH_UPPER = 0.77;

#define __KHASH_TYPE(name, khkey_t, khval_t) \
	typedef struct kh_##name##_s { \
		khint_t n_buckets, size, n_occupied, upper_bound; \
		khint32_t *flags; \
		khkey_t *keys; \
		khval_t *vals; \
	} kh_##name##_t;

#define __KHASH_PROTOTYPES(name, khkey_t, khval_t)	 					\
	extern kh_##name##_t *kh_init_##name(void);							\
	extern void kh_destroy_##name(kh_##name##_t *h);					\
	extern void kh_clear_##name(kh_##name##_t *h);						\
	extern khint_t kh_get_##name(const kh_##name##_t *h, khkey_t key); 	\
	extern int kh_resize_##name(kh_##name##_t *h, khint_t new_n_buckets); \
	extern khint_t kh_put_##name(kh_##name##_t *h, khkey_t key, int *ret); \
	extern void kh_del_##name(kh_##name##_t *h, khint_t x);

#define __KHASH_IMPL(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	SCOPE kh_##name##_t *kh_init_##name(void) {							\
		return (kh_##name##_t*)kcalloc(1, sizeof(kh_##name##_t));		\
	}																	\
	SCOPE void kh_destroy_##name(kh_##name##_t *h)						\
	{																	\
		if (h) {														\
			kfree((void *)h->keys); kfree(h->flags);					\
			kfree((void *)h->vals);										\
			kfree(h);													\
		}																\
	}																	\
	SCOPE void kh_clear_##name(kh_##name##_t *h)						\
	{																	\
		if (h && h->flags) {											\
			memset(h->flags, 0xaa, __ac_fsize(h->n_buckets) * sizeof(khint32_t)); \
			h->size = h->n_occupied = 0;								\
		}																\
	}																	\
	SCOPE khint_t kh_get_##name(const kh_##name##_t *h, khkey_t key) 	\
	{																	\
		if (h->n_buckets) {												\
			khint_t k, i, last, mask, step = 0; \
			mask = h->n_buckets - 1;									\
			k = __hash_func(key); i = k & mask;							\
			last = i; \
			while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
				i = (i + (++step)) & mask; \
				if (i == last) return h->n_buckets;						\
			}															\
			return __ac_iseither(h->flags, i)? h->n_buckets : i;		\
		} else return 0;												\
	}																	\
	SCOPE int kh_resize_##name(kh_##name##_t *h, khint_t new_n_buckets) \
	{ /* This function uses 0.25*n_buckets bytes of working space instead of [sizeof(key_t+val_t)+.25]*n_buckets. */ \
		khint32_t *new_flags = 0;										\
		khint_t j = 1;													\
		{																\
			kroundup32(new_n_buckets); 									\
			if (new_n_buckets < 4) new_n_buckets = 4;					\
			if (h->size >= (khint_t)(new_n_buckets * __ac_HASH_UPPER + 0.5)) j = 0;	/* requested size is too small */ \
			else { /* hash table size to be changed (shrink or expand); rehash */ \
				new_flags = (khint32_t*)kmalloc(__ac_fsize(new_n_buckets) * sizeof(khint32_t));	\
				if (!new_flags) return -1;								\
				memset(new_flags, 0xaa, __ac_fsize(new_n_buckets) * sizeof(khint32_t)); \
				if (h->n_buckets < new_n_buckets) {	/* expand */		\
					khkey_t *new_keys = (khkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(khkey_t)); \
					if (!new_keys) { kfree(new_flags); return -1; }		\
					h->keys = new_keys;									\
					if (kh_is_map) {									\
						khval_t *new_vals = (khval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(khval_t)); \
						if (!new_vals) { kfree(new_flags); return -1; }	\
						h->vals = new_vals;								\
					}													\
				} /* otherwise shrink */								\
			}															\
		}																\
		if (j) { /* rehashing is needed */								\
			for (j = 0; j != h->n_buckets; ++j) {						\
				if (__ac_iseither(h->flags, j) == 0) {					\
					khkey_t key = h->keys[j];							\
					khval_t val;										\
					khint_t new_mask;									\
					new_mask = new_n_buckets - 1; 						\
					if (kh_is_map) val = h->vals[j];					\
					__ac_set_isdel_true(h->flags, j);					\
					while (1) { /* kick-out process; sort of like in Cuckoo hashing */ \
						khint_t k, i, step = 0; \
						k = __hash_func(key);							\
						i = k & new_mask;								\
						while (!__ac_isempty(new_flags, i)) i = (i + (++step)) & new_mask; \
						__ac_set_isempty_false(new_flags, i);			\
						if (i < h->n_buckets && __ac_iseither(h->flags, i) == 0) { /* kick out the existing element */ \
							{ khkey_t tmp = h->keys[i]; h->keys[i] = key; key = tmp; } \
							if (kh_is_map) { khval_t tmp = h->vals[i]; h->vals[i] = val; val = tmp; } \
							__ac_set_isdel_true(h->flags, i); /* mark it as deleted in the old hash table */ \
						} else { /* write the element and jump out of the loop */ \
							h->keys[i] = key;							\
							if (kh_is_map) h->vals[i] = val;			\
							break;										\
						}												\
					}													\
				}														\
			}															\
			if (h->n_buckets > new_n_buckets) { /* shrink the hash table */ \
				h->keys = (khkey_t*)krealloc((void *)h->keys, new_n_buckets * sizeof(khkey_t)); \
				if (kh_is_map) h->vals = (khval_t*)krealloc((void *)h->vals, new_n_buckets * sizeof(khval_t)); \
			}															\
			kfree(h->flags); /* free the working space */				\
			h->flags = new_flags;										\
			h->n_buckets = new_n_buckets;								\
			h->n_occupied = h->size;									\
			h->upper_bound = (khint_t)(h->n_buckets * __ac_HASH_UPPER + 0.5); \
		}																\
		return 0;														\
	}																	\
	SCOPE khint_t kh_put_##name(kh_##name##_t *h, khkey_t key, int *ret) \
	{																	\
		khint_t x;														\
		if (h->n_occupied >= h->upper_bound) { /* update the hash table */ \
			if (h->n_buckets > (h->size<<1)) {							\
				if (kh_resize_##name(h, h->n_buckets - 1) < 0) { /* clear "deleted" elements */ \
					*ret = -1; return h->n_buckets;						\
				}														\
			} else if (kh_resize_##name(h, h->n_buckets + 1) < 0) { /* expand the hash table */ \
				*ret = -1; return h->n_buckets;							\
			}															\
		} /* TODO: to implement automatically shrinking; resize() already support shrinking */ \
		{																\
			khint_t k, i, site, last, mask = h->n_buckets - 1, step = 0; \
			x = site = h->n_buckets; k = __hash_func(key); i = k & mask; \
			if (__ac_isempty(h->flags, i)) x = i; /* for speed up */	\
			else {														\
				last = i; \
				while (!__ac_isempty(h->flags, i) && (__ac_isdel(h->flags, i) || !__hash_equal(h->keys[i], key))) { \
					if (__ac_isdel(h->flags, i)) site = i;				\
					i = (i + (++step)) & mask; \
					if (i == last) { x = site; break; }					\
				}														\
				if (x == h->n_buckets) {								\
					if (__ac_isempty(h->flags, i) && site != h->n_buckets) x = site; \
					else x = i;											\
				}														\
			}															\
		}																\
		if (__ac_isempty(h->flags, x)) { /* not present at all */		\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size; ++h->n_occupied;									\
			*ret = 1;													\
		} else if (__ac_isdel(h->flags, x)) { /* deleted */				\
			h->keys[x] = key;											\
			__ac_set_isboth_false(h->flags, x);							\
			++h->size;													\
			*ret = 2;													\
		} else *ret = 0; /* Don't touch h->keys[x] if present and not deleted */ \
		return x;														\
	}																	\
	SCOPE void kh_del_##name(kh_##name##_t *h, khint_t x)				\
	{																	\
		if (x != h->n_buckets && !__ac_iseither(h->flags, x)) {			\
			__ac_set_isdel_true(h->flags, x);							\
			--h->size;													\
		}																\
	}

#define KHASH_DECLARE(name, khkey_t, khval_t)		 					\
	__KHASH_TYPE(name, khkey_t, khval_t) 								\
	__KHASH_PROTOTYPES(name, khkey_t, khval_t)

#define KHASH_INIT2(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	__KHASH_TYPE(name, khkey_t, khval_t) 								\
	__KHASH_IMPL(name, SCOPE, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal)

#define KHASH_INIT(name, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal) \
	KHASH_INIT2(name, static kh_inline klib_unused, khkey_t, khval_t, kh_is_map, __hash_func, __hash_equal)

#define kh_int_hash_func(key) (khint32_t)(key)
#define kh_int_hash_equal(a, b) ((a) == (b))
#define kh_int64_hash_func(key) (khint32_t)((key)>>33^(key)^(key)<<11)
#define kh_int64_hash_equal(a, b) ((a) == (b))
static kh_inline khint_t __ac_X31_hash_string(const char *s)
{
	khint_t h = (khint_t)*s;
	if (h) for (++s ; *s; ++s) h = (h << 5) - h + (khint_t)*s;
	return h;
}
#define kh_str_hash_func(key) __ac_X31_hash_string(key)
#define kh_str_hash_equal(a, b) (strcmp(a, b) == 0)
#define khash_t(name) kh_##name##_t
#define kh_init(name) kh_init_##name()
#define kh_destroy(name, h) kh_destroy_##name(h)
#define kh_clear(name, h) kh_clear_##name(h)
#define kh_resize(name, h, s) kh_resize_##name(h, s)
#define kh_put(name, h, k, r) kh_put_##name(h, k, r)
#define kh_get(name, h, k) kh_get_##name(h, k)
#define kh_del(name, h, k) kh_del_##name(h, k)
#define kh_exist(h, x) (!__ac_iseither((h)->flags, (x)))
#define kh_key(h, x) ((h)->keys[x])
#define kh_val(h, x) ((h)->vals[x])
#define kh_value(h, x) ((h)->vals[x])
#define kh_begin(h) (khint_t)(0)
#define kh_end(h) ((h)->n_buckets)
#define kh_size(h) ((h)->size)
#define kh_n_buckets(h) ((h)->n_buckets)

#define kh_foreach(h, kvar, vvar, code) { khint_t __i;		\
	for (__i = kh_begin(h); __i != kh_end(h); ++__i) {		\
		if (!kh_exist(h,__i)) continue;						\
		(kvar) = kh_key(h,__i);								\
		(vvar) = kh_val(h,__i);								\
		code;												\
	} }
#define kh_foreach_value(h, vvar, code) { khint_t __i;		\
	for (__i = kh_begin(h); __i != kh_end(h); ++__i) {		\
		if (!kh_exist(h,__i)) continue;						\
		(vvar) = kh_val(h,__i);								\
		code;												\
	} }
#define KHASH_SET_INIT_INT(name)										\
	KHASH_INIT(name, khint32_t, char, 0, kh_int_hash_func, kh_int_hash_equal)
#define KHASH_MAP_INIT_INT(name, khval_t)								\
	KHASH_INIT(name, khint32_t, khval_t, 1, kh_int_hash_func, kh_int_hash_equal)
#define KHASH_SET_INIT_INT64(name)										\
	KHASH_INIT(name, khint64_t, char, 0, kh_int64_hash_func, kh_int64_hash_equal)
#define KHASH_MAP_INIT_INT64(name, khval_t)								\
	KHASH_INIT(name, khint64_t, khval_t, 1, kh_int64_hash_func, kh_int64_hash_equal)
typedef const char *kh_cstr_t;
#define KHASH_SET_INIT_STR(name)										\
	KHASH_INIT(name, kh_cstr_t, char, 0, kh_str_hash_func, kh_str_hash_equal)
#define KHASH_MAP_INIT_STR(name, khval_t)								\
	KHASH_INIT(name, kh_cstr_t, khval_t, 1, kh_str_hash_func, kh_str_hash_equal)
#endif /* __AC_KHASH_H */

/** End of khash.h */

#include "peppapeg.h"

KHASH_MAP_INIT_STR(rules, P4_Expression*)

struct P4_Grammar {
    /** A map associating names with expressions. Type: Map<str, P4_Expression*>. */
    khash_t(rules)*         rules;
    /** The total number of spaced rules. */
    size_t                  spaced_count;
    /** The repetition rule for spaced rules. */
    P4_Expression*          spaced_rules;
    /** The recursion limit, or maximum allowed nested rules. */
    size_t                  depth;
    /** The callback after a match for an expression is successful. */
    P4_MatchCallback        on_match;
    /** The callback after a match for an expression is failed. */
    P4_ErrorCallback        on_error;
    /** The callback to free user data. */
    P4_UserDataFreeFunc     free_func;
};

struct P4_Expression {
    /* The name of expression. */
    P4_String               name;
    /** The kind of expression. */
    P4_ExpressionKind       kind;
    /** The flag of expression. */
    P4_ExpressionFlag       flag;

    union {
        /** Used by P4_Numeric. */
        size_t                      num;

        /** Used by P4_Literal and P4_BackReference. */
        struct {
            P4_String               literal;
            bool                    sensitive;
            size_t                  backref_index;
        };

        /** Used by P4_Reference..P4_Negative. */
        struct {
            P4_String               reference;
            P4_Expression*          ref_expr;
        };

        /** Used by P4_Range. */
        struct {
            size_t                  ranges_count;
            struct P4_RuneRange*    ranges;
        };

        /** Used by P4_Sequence..P4_Choice. */
        struct {
            P4_Expression**  members;
            size_t                  count;
        };

        /** Used by P4_ZeroOrOnce..P4_RepeatExact.
         * repeat the expr for n times, n >= min and n <= max. */
        struct {
            P4_Expression*   repeat_expr; /* maybe we can merge it with ref_expr? */
            size_t                  repeat_min;
            size_t                  repeat_max;
        };
    };
};

struct P4_Frame {
    /** The current matching expression for the frame. */
    P4_Expression*  expr;
    /** The current rule for the frame. */
    P4_Expression*  rule;
    /** Whether spacing is applicable to frame & frame dependents. */
    bool            space;
    /** Whether silencing is applicable to frame & frame dependents. */
    bool            silent;
    /** Whether cut is enabled to frame. */
    bool            cut;
    /** The next frame in the stack. */
    P4_Frame*       next;
};

struct P4_Source {
    /** The grammar used to parse the source. */
    P4_Grammar*     grammar;
    /** The entry rule name in the grammar used to parse the source. */
    P4_String       entry_name;

    /** The content of the source. */
    P4_String       content;
    /** The length of the source. */
    P4_Slice        slice;

    /**
     * The position of the consumed input. Min: 0, Max: strlen(content).
     *
     * It's possible the pos is less then length of content when the Source
     * is successfully parsed. It's called a partial parse.
     *
     * To avoid that, the rule will need to be wrapped with an EOI and SOI.
     * An SOI is Positive(Range(1, 0x10ffff))
     * and An EOI is Negative(Range(1, 0x10ffff)). When the rule is wrapped,
     * the input is guaranteed to be parsed until all bits are consumed.
     * */
    size_t          pos;
    /**
     * The line number of the unconsumed input. Min: 1, Max: countlines(content).
     */
    size_t          lineno;
    /**
     * The bytes offset of the line in the unconsumed input.
     */
    size_t          offset;

    /** The error code of the parse. */
    P4_Error        err;
    /** The error message of the parse. */
    char            errmsg[120];

    /** The root of abstract syntax tree. */
    P4_Node*        root;

    /** Reserved: whether to enable DEBUG logs. */
    bool            verbose;

    /** The flag for checking if the parse is matching SPACED rules.
     *
     * Since we're wrapping SPACED rules into a repetition rule internally,
     * it's important to prevent matching SPACED rules in P4_MatchRepeat.
     *
     * XXX: Maybe there are some better ways to prevent that?
     */
    bool            whitespacing;

    /** The top frame in the stack. */
    P4_Frame*       frame_stack;
    /** The size of frame stack. */
    size_t          frame_stack_size;
};

/** It indicates the function or type is for public use. */
# define P4_PUBLIC

/** It indicates the function or type is not for public use. */
# define P4_PRIVATE(type) static type

# define unwrap_grammar(r) ((r)->errmsg[0] == 0 ? (r)->grammar : NULL)
# define unwrap_expr(r) ((r)->errmsg[0] == 0 ? (r)->expr : NULL)

static void panic(const char * str)     __attribute__((noreturn));
static void panic(const char * str)     { fputs(str, stderr); exit(1); }
static void panicf(const char * fmt, ...) __attribute__((noreturn));
static void panicf(const char * fmt, ...) { va_list args; va_start(args, fmt); vfprintf(stderr, fmt, args); exit(1); }

# if defined(DEBUG)
#define UNREACHABLE() panicf("[%s:%d] This code should not be reached in %s()\n", __FILE__, __LINE__, __func__);
# elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
# elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define UNREACHABLE() __builtin_unreachable()
# else
#define UNREACHABLE()
# endif

#define assert(c,m) \
    if(!(c)) \
        panicf("[%s:%d] Assert failed in %s(): %s\n", __FILE__, __LINE__, __func__, (m));

# define catch_oom(s) do { \
    if ((s) == NULL) { \
        panic("out of memory."); \
    } \
} while (0);

# define catch_err(s, ...) do { \
    if ((err = (s)) != P4_Ok) { \
        __VA_ARGS__; \
        goto finalize; \
    } \
} while (0);

# define is_end(s) ((s)->pos >= (s)->slice.stop.pos)
# define is_tight(e) (((e)->flag & P4_FLAG_TIGHT) != 0)
# define is_scoped(e) (((e)->flag & P4_FLAG_SCOPED) != 0)
# define is_spaced(e) (((e)->flag & P4_FLAG_SPACED) != 0)
# define is_squashed(e) (((e)->flag & P4_FLAG_SQUASHED) != 0)
# define is_lifted(e) (((e)->flag & P4_FLAG_LIFTED) != 0)
# define is_non_terminal(e) (((e)->flag & P4_FLAG_NON_TERMINAL) != 0)
# define is_rule(e) ((e)->name != NULL)
# define need_silent(s) ((s)->frame_stack ? (s)->frame_stack->silent : false)
# define need_whitespace(s) (!(s)->whitespacing && ((s)->frame_stack ? (s)->frame_stack->space : false))

# define no_error(s) ((s)->err == P4_Ok)
# define no_match(s) ((s)->err == P4_MatchError)

# define set_position(s, d) \
    do { \
        (s)->pos = (d)->pos; \
        (s)->lineno = (d)->lineno; \
        (s)->offset = (d)->offset; \
    } while (0);
# define mark_position(s, p) \
    P4_Position* p = & (P4_Position) { \
        .pos = (s)->pos, \
        .lineno = (s)->lineno, \
        .offset = (s)->offset \
    };

# define get_slice_size(s) ((s)->stop.pos - (s)->start.pos)
# define set_slice(s, a, b) \
    do { \
        set_position(&((s)->start), (a)); \
        set_position(&((s)->stop), (b)); \
    } while (0)

#define foreach_child(node, child, code) \
    for ((child) = (node)->head; \
            (child) != NULL; \
            (child) = (child)->next) { \
        code; \
    }

#define strdup(x) panic("Do not use strdup(). Use STRDUP() instead.")
static char *STRDUP(const char *src) { /* STRDUP is not ANSI. Copy source here. */
    char *dst = P4_MALLOC(strlen (src) + 1);
    if (dst == NULL) return NULL;
    strcpy(dst, src);
    return dst;
}

# define autofree __attribute__ ((cleanup (cleanup_freep)))
static void cleanup_freep (void *p) { /* clean a malloc-ed variable. */
  void **pp = (void **) p;
  if (*pp) P4_FREE(*pp);
}

P4_PRIVATE(size_t)       P4_GetRuneSize(P4_Rune ch);
P4_PRIVATE(P4_Rune)      P4_GetRuneLower(P4_Rune ch);
P4_PRIVATE(P4_Rune)      P4_GetRuneUpper(P4_Rune ch);
P4_PRIVATE(int)          P4_CaseCmpInsensitive(const void*, const void*, size_t n);

P4_PRIVATE(size_t)       P4_GetPosition(P4_Source*);
P4_PRIVATE(void)         P4_DiffPosition(P4_String str, P4_Position* start, size_t offset, P4_Position* stop);

# define P4_AdoptNode(head, tail, list) \
    do { \
        if ((list) != NULL) {\
            if ((head) == NULL) (head) = (list); \
            if ((tail) == NULL) (tail) = (list); \
            else (tail)->next = (list); \
            if ((tail) != NULL) {\
                while ((tail)->next != NULL) \
                    (tail) = (tail)->next; \
            } \
        } \
    } while (0)

# define P4_AddSomeGrammarRule(g, name, rule) \
    do { \
        P4_Error err = P4_Ok;       \
        P4_Expression* expr = NULL; \
                                    \
        if (grammar == NULL || name == NULL) { \
            err = P4_NullError;     \
            goto end;               \
        }                           \
        if ((expr = (rule)) == NULL) \
            panic("failed to create expression: out of memory"); \
                                    \
        if ((err=P4_AddGrammarRule(grammar, name, expr))!=P4_Ok) {\
            goto end;               \
        }                           \
                                    \
        break;                      \
        end:                        \
        if (expr != NULL) {         \
            P4_DeleteExpression(expr); \
        }                           \
        return err;                 \
    } while (0)

P4_PRIVATE(P4_String)           P4_RemainingText(P4_Source*);

P4_PRIVATE(bool)                P4_NeedLift(P4_Source*, P4_Expression*);

# define P4_MatchRaisef(s,e,m,...) \
    do { \
        (s)->err = (e); \
        memset((s)->errmsg, 0, sizeof((s)->errmsg)); \
        sprintf((s)->errmsg, "line %zu:%zu, " m, \
                (s)->lineno, (s)->offset, __VA_ARGS__); \
    } while (0);

# define P4_EvalRaisef(r,m,...) \
    do { \
        memset((r)->errmsg, 0, sizeof((r)->errmsg)); \
        sprintf((r)->errmsg, (m), __VA_ARGS__); \
    } while (0);

P4_PRIVATE(void)                P4_RescueError(P4_Source*);

P4_PRIVATE(P4_Error)            P4_PushFrame(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Error)            P4_PopFrame(P4_Source*, P4_Frame*);
# define P4_PeekFrame(s) ((s)->frame_stack)

P4_PRIVATE(P4_Expression*)      P4_GetReference(P4_Source*, P4_Expression*);

P4_PRIVATE(P4_String)           P4_CopySliceString(P4_String, P4_Slice*);

P4_PRIVATE(P4_Error)            P4_SetWhitespaces(P4_Grammar*);
P4_PRIVATE(P4_Expression*)      P4_GetWhitespaces(P4_Grammar*);

P4_PRIVATE(P4_Error)            P4_RefreshReference(P4_Expression*, P4_String);

P4_PRIVATE(P4_Node*)           P4_Match(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchLiteral(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchRange(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchReference(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchPositive(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchNegative(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchCut(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchSequence(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchChoice(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchRepeat(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchSpacedExpressions(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Node*)           P4_MatchBackReference(P4_Source*, P4_Expression*, P4_Slice*, P4_Expression*);

P4_PRIVATE(P4_Error)            P4_PegEvalFlag(P4_Node* node, P4_ExpressionFlag *flag);
P4_PRIVATE(P4_Error)            P4_PegEvalRuleFlags(P4_Node* node, P4_ExpressionFlag* flag);
P4_PRIVATE(P4_Error)            P4_PegEvalNumber(P4_Node* node, size_t* num);
P4_PRIVATE(P4_Error)            P4_PegEvalChar(P4_Node* node, P4_Rune* rune);
P4_PRIVATE(P4_Error)            P4_PegEvalLiteral(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalInsensitiveLiteral(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalRange(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalMembers(P4_Node* node, P4_Expression* expr, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalSequence(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalChoice(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalPositive(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalNegative(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalRepeat(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalDot(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalReference(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalBackReference(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalExpression(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalGrammarRule(P4_Node* node, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalRuleName(P4_Node* node, P4_String* result);
P4_PRIVATE(P4_Error)            P4_PegEvalGrammarReferences(P4_Grammar* grammar, P4_Expression* expr, P4_Result* result);
P4_PRIVATE(P4_Error)            P4_PegEvalGrammar(P4_Node* node, P4_Result* result);

static P4_RuneRange _C[] = {
    {0x0000, 0x001f, 1}, {0x007f, 0x009f, 1}, {0x00ad, 0x0600, 1363}, {0x0601, 0x0605, 1},
    {0x061c, 0x06dd, 193}, {0x070f, 0x08e2, 467}, {0x180e, 0x200b, 2045}, {0x200c, 0x200f, 1},
    {0x202a, 0x202e, 1}, {0x2060, 0x2064, 1}, {0x2066, 0x206f, 1}, {0xd800, 0xf8ff, 1},
    {0xfeff, 0xfff9, 250}, {0xfffa, 0xfffb, 1},
    {0x110bd, 0x110cd, 16}, {0x13430, 0x13438, 1}, {0x1bca0, 0x1bca3, 1}, {0x1d173, 0x1d17a, 1},
    {0xe0001, 0xe0020, 31}, {0xe0021, 0xe007f, 1}, {0xf0000, 0xffffd, 1}, {0x100000, 0x10fffd, 1},
};
static P4_RuneRange _Cc[] = {
    {0x0000, 0x001f, 1}, {0x007f, 0x009f, 1},
};
static P4_RuneRange _Cf[] = {
    {0x00ad, 0x0600, 1363}, {0x0601, 0x0605, 1}, {0x061c, 0x06dd, 193}, {0x070f, 0x08e2, 467},
    {0x180e, 0x200b, 2045}, {0x200c, 0x200f, 1}, {0x202a, 0x202e, 1}, {0x2060, 0x2064, 1},
    {0x2066, 0x206f, 1}, {0xfeff, 0xfff9, 250}, {0xfffa, 0xfffb, 1},
    {0x110bd, 0x110cd, 16}, {0x13430, 0x13438, 1}, {0x1bca0, 0x1bca3, 1}, {0x1d173, 0x1d17a, 1},
    {0xe0001, 0xe0020, 31}, {0xe0021, 0xe007f, 1},
};
static P4_RuneRange _Co[] = {
    {0xe000, 0xf8ff, 1},
    {0xf0000, 0xffffd, 1}, {0x100000, 0x10fffd, 1},
};
static P4_RuneRange _Cs[] = {
    {0xd800, 0xdfff, 1},
};
static P4_RuneRange _L[] = {
    {0x0041, 0x005a, 1}, {0x0061, 0x007a, 1}, {0x00aa, 0x00b5, 11}, {0x00ba, 0x00c0, 6},
    {0x00c1, 0x00d6, 1}, {0x00d8, 0x00f6, 1}, {0x00f8, 0x02c1, 1}, {0x02c6, 0x02d1, 1},
    {0x02e0, 0x02e4, 1}, {0x02ec, 0x02ee, 2}, {0x0370, 0x0374, 1}, {0x0376, 0x0377, 1},
    {0x037a, 0x037d, 1}, {0x037f, 0x0386, 7}, {0x0388, 0x038a, 1}, {0x038c, 0x038e, 2},
    {0x038f, 0x03a1, 1}, {0x03a3, 0x03f5, 1}, {0x03f7, 0x0481, 1}, {0x048a, 0x052f, 1},
    {0x0531, 0x0556, 1}, {0x0559, 0x0560, 7}, {0x0561, 0x0588, 1}, {0x05d0, 0x05ea, 1},
    {0x05ef, 0x05f2, 1}, {0x0620, 0x064a, 1}, {0x066e, 0x066f, 1}, {0x0671, 0x06d3, 1},
    {0x06d5, 0x06e5, 16}, {0x06e6, 0x06ee, 8}, {0x06ef, 0x06fa, 11}, {0x06fb, 0x06fc, 1},
    {0x06ff, 0x0710, 17}, {0x0712, 0x072f, 1}, {0x074d, 0x07a5, 1}, {0x07b1, 0x07ca, 25},
    {0x07cb, 0x07ea, 1}, {0x07f4, 0x07f5, 1}, {0x07fa, 0x0800, 6}, {0x0801, 0x0815, 1},
    {0x081a, 0x0824, 10}, {0x0828, 0x0840, 24}, {0x0841, 0x0858, 1}, {0x0860, 0x086a, 1},
    {0x08a0, 0x08b4, 1}, {0x08b6, 0x08c7, 1}, {0x0904, 0x0939, 1}, {0x093d, 0x0950, 19},
    {0x0958, 0x0961, 1}, {0x0971, 0x0980, 1}, {0x0985, 0x098c, 1}, {0x098f, 0x0990, 1},
    {0x0993, 0x09a8, 1}, {0x09aa, 0x09b0, 1}, {0x09b2, 0x09b6, 4}, {0x09b7, 0x09b9, 1},
    {0x09bd, 0x09ce, 17}, {0x09dc, 0x09dd, 1}, {0x09df, 0x09e1, 1}, {0x09f0, 0x09f1, 1},
    {0x09fc, 0x0a05, 9}, {0x0a06, 0x0a0a, 1}, {0x0a0f, 0x0a10, 1}, {0x0a13, 0x0a28, 1},
    {0x0a2a, 0x0a30, 1}, {0x0a32, 0x0a33, 1}, {0x0a35, 0x0a36, 1}, {0x0a38, 0x0a39, 1},
    {0x0a59, 0x0a5c, 1}, {0x0a5e, 0x0a72, 20}, {0x0a73, 0x0a74, 1}, {0x0a85, 0x0a8d, 1},
    {0x0a8f, 0x0a91, 1}, {0x0a93, 0x0aa8, 1}, {0x0aaa, 0x0ab0, 1}, {0x0ab2, 0x0ab3, 1},
    {0x0ab5, 0x0ab9, 1}, {0x0abd, 0x0ad0, 19}, {0x0ae0, 0x0ae1, 1}, {0x0af9, 0x0b05, 12},
    {0x0b06, 0x0b0c, 1}, {0x0b0f, 0x0b10, 1}, {0x0b13, 0x0b28, 1}, {0x0b2a, 0x0b30, 1},
    {0x0b32, 0x0b33, 1}, {0x0b35, 0x0b39, 1}, {0x0b3d, 0x0b5c, 31}, {0x0b5d, 0x0b5f, 2},
    {0x0b60, 0x0b61, 1}, {0x0b71, 0x0b83, 18}, {0x0b85, 0x0b8a, 1}, {0x0b8e, 0x0b90, 1},
    {0x0b92, 0x0b95, 1}, {0x0b99, 0x0b9a, 1}, {0x0b9c, 0x0b9e, 2}, {0x0b9f, 0x0ba3, 4},
    {0x0ba4, 0x0ba8, 4}, {0x0ba9, 0x0baa, 1}, {0x0bae, 0x0bb9, 1}, {0x0bd0, 0x0c05, 53},
    {0x0c06, 0x0c0c, 1}, {0x0c0e, 0x0c10, 1}, {0x0c12, 0x0c28, 1}, {0x0c2a, 0x0c39, 1},
    {0x0c3d, 0x0c58, 27}, {0x0c59, 0x0c5a, 1}, {0x0c60, 0x0c61, 1}, {0x0c80, 0x0c85, 5},
    {0x0c86, 0x0c8c, 1}, {0x0c8e, 0x0c90, 1}, {0x0c92, 0x0ca8, 1}, {0x0caa, 0x0cb3, 1},
    {0x0cb5, 0x0cb9, 1}, {0x0cbd, 0x0cde, 33}, {0x0ce0, 0x0ce1, 1}, {0x0cf1, 0x0cf2, 1},
    {0x0d04, 0x0d0c, 1}, {0x0d0e, 0x0d10, 1}, {0x0d12, 0x0d3a, 1}, {0x0d3d, 0x0d4e, 17},
    {0x0d54, 0x0d56, 1}, {0x0d5f, 0x0d61, 1}, {0x0d7a, 0x0d7f, 1}, {0x0d85, 0x0d96, 1},
    {0x0d9a, 0x0db1, 1}, {0x0db3, 0x0dbb, 1}, {0x0dbd, 0x0dc0, 3}, {0x0dc1, 0x0dc6, 1},
    {0x0e01, 0x0e30, 1}, {0x0e32, 0x0e33, 1}, {0x0e40, 0x0e46, 1}, {0x0e81, 0x0e82, 1},
    {0x0e84, 0x0e86, 2}, {0x0e87, 0x0e8a, 1}, {0x0e8c, 0x0ea3, 1}, {0x0ea5, 0x0ea7, 2},
    {0x0ea8, 0x0eb0, 1}, {0x0eb2, 0x0eb3, 1}, {0x0ebd, 0x0ec0, 3}, {0x0ec1, 0x0ec4, 1},
    {0x0ec6, 0x0edc, 22}, {0x0edd, 0x0edf, 1}, {0x0f00, 0x0f40, 64}, {0x0f41, 0x0f47, 1},
    {0x0f49, 0x0f6c, 1}, {0x0f88, 0x0f8c, 1}, {0x1000, 0x102a, 1}, {0x103f, 0x1050, 17},
    {0x1051, 0x1055, 1}, {0x105a, 0x105d, 1}, {0x1061, 0x1065, 4}, {0x1066, 0x106e, 8},
    {0x106f, 0x1070, 1}, {0x1075, 0x1081, 1}, {0x108e, 0x10a0, 18}, {0x10a1, 0x10c5, 1},
    {0x10c7, 0x10cd, 6}, {0x10d0, 0x10fa, 1}, {0x10fc, 0x1248, 1}, {0x124a, 0x124d, 1},
    {0x1250, 0x1256, 1}, {0x1258, 0x125a, 2}, {0x125b, 0x125d, 1}, {0x1260, 0x1288, 1},
    {0x128a, 0x128d, 1}, {0x1290, 0x12b0, 1}, {0x12b2, 0x12b5, 1}, {0x12b8, 0x12be, 1},
    {0x12c0, 0x12c2, 2}, {0x12c3, 0x12c5, 1}, {0x12c8, 0x12d6, 1}, {0x12d8, 0x1310, 1},
    {0x1312, 0x1315, 1}, {0x1318, 0x135a, 1}, {0x1380, 0x138f, 1}, {0x13a0, 0x13f5, 1},
    {0x13f8, 0x13fd, 1}, {0x1401, 0x166c, 1}, {0x166f, 0x167f, 1}, {0x1681, 0x169a, 1},
    {0x16a0, 0x16ea, 1}, {0x16f1, 0x16f8, 1}, {0x1700, 0x170c, 1}, {0x170e, 0x1711, 1},
    {0x1720, 0x1731, 1}, {0x1740, 0x1751, 1}, {0x1760, 0x176c, 1}, {0x176e, 0x1770, 1},
    {0x1780, 0x17b3, 1}, {0x17d7, 0x17dc, 5}, {0x1820, 0x1878, 1}, {0x1880, 0x1884, 1},
    {0x1887, 0x18a8, 1}, {0x18aa, 0x18b0, 6}, {0x18b1, 0x18f5, 1}, {0x1900, 0x191e, 1},
    {0x1950, 0x196d, 1}, {0x1970, 0x1974, 1}, {0x1980, 0x19ab, 1}, {0x19b0, 0x19c9, 1},
    {0x1a00, 0x1a16, 1}, {0x1a20, 0x1a54, 1}, {0x1aa7, 0x1b05, 94}, {0x1b06, 0x1b33, 1},
    {0x1b45, 0x1b4b, 1}, {0x1b83, 0x1ba0, 1}, {0x1bae, 0x1baf, 1}, {0x1bba, 0x1be5, 1},
    {0x1c00, 0x1c23, 1}, {0x1c4d, 0x1c4f, 1}, {0x1c5a, 0x1c7d, 1}, {0x1c80, 0x1c88, 1},
    {0x1c90, 0x1cba, 1}, {0x1cbd, 0x1cbf, 1}, {0x1ce9, 0x1cec, 1}, {0x1cee, 0x1cf3, 1},
    {0x1cf5, 0x1cf6, 1}, {0x1cfa, 0x1d00, 6}, {0x1d01, 0x1dbf, 1}, {0x1e00, 0x1f15, 1},
    {0x1f18, 0x1f1d, 1}, {0x1f20, 0x1f45, 1}, {0x1f48, 0x1f4d, 1}, {0x1f50, 0x1f57, 1},
    {0x1f59, 0x1f5f, 2}, {0x1f60, 0x1f7d, 1}, {0x1f80, 0x1fb4, 1}, {0x1fb6, 0x1fbc, 1},
    {0x1fbe, 0x1fc2, 4}, {0x1fc3, 0x1fc4, 1}, {0x1fc6, 0x1fcc, 1}, {0x1fd0, 0x1fd3, 1},
    {0x1fd6, 0x1fdb, 1}, {0x1fe0, 0x1fec, 1}, {0x1ff2, 0x1ff4, 1}, {0x1ff6, 0x1ffc, 1},
    {0x2071, 0x207f, 14}, {0x2090, 0x209c, 1}, {0x2102, 0x2107, 5}, {0x210a, 0x2113, 1},
    {0x2115, 0x2119, 4}, {0x211a, 0x211d, 1}, {0x2124, 0x212a, 2}, {0x212b, 0x212d, 1},
    {0x212f, 0x2139, 1}, {0x213c, 0x213f, 1}, {0x2145, 0x2149, 1}, {0x214e, 0x2183, 53},
    {0x2184, 0x2c00, 2684}, {0x2c01, 0x2c2e, 1}, {0x2c30, 0x2c5e, 1}, {0x2c60, 0x2ce4, 1},
    {0x2ceb, 0x2cee, 1}, {0x2cf2, 0x2cf3, 1}, {0x2d00, 0x2d25, 1}, {0x2d27, 0x2d2d, 6},
    {0x2d30, 0x2d67, 1}, {0x2d6f, 0x2d80, 17}, {0x2d81, 0x2d96, 1}, {0x2da0, 0x2da6, 1},
    {0x2da8, 0x2dae, 1}, {0x2db0, 0x2db6, 1}, {0x2db8, 0x2dbe, 1}, {0x2dc0, 0x2dc6, 1},
    {0x2dc8, 0x2dce, 1}, {0x2dd0, 0x2dd6, 1}, {0x2dd8, 0x2dde, 1}, {0x2e2f, 0x3005, 470},
    {0x3006, 0x3031, 43}, {0x3032, 0x3035, 1}, {0x303b, 0x303c, 1}, {0x3041, 0x3096, 1},
    {0x309d, 0x309f, 1}, {0x30a1, 0x30fa, 1}, {0x30fc, 0x30ff, 1}, {0x3105, 0x312f, 1},
    {0x3131, 0x318e, 1}, {0x31a0, 0x31bf, 1}, {0x31f0, 0x31ff, 1}, {0x3400, 0x4dbf, 1},
    {0x4e00, 0x9ffc, 1}, {0xa000, 0xa48c, 1}, {0xa4d0, 0xa4fd, 1}, {0xa500, 0xa60c, 1},
    {0xa610, 0xa61f, 1}, {0xa62a, 0xa62b, 1}, {0xa640, 0xa66e, 1}, {0xa67f, 0xa69d, 1},
    {0xa6a0, 0xa6e5, 1}, {0xa717, 0xa71f, 1}, {0xa722, 0xa788, 1}, {0xa78b, 0xa7bf, 1},
    {0xa7c2, 0xa7ca, 1}, {0xa7f5, 0xa801, 1}, {0xa803, 0xa805, 1}, {0xa807, 0xa80a, 1},
    {0xa80c, 0xa822, 1}, {0xa840, 0xa873, 1}, {0xa882, 0xa8b3, 1}, {0xa8f2, 0xa8f7, 1},
    {0xa8fb, 0xa8fd, 2}, {0xa8fe, 0xa90a, 12}, {0xa90b, 0xa925, 1}, {0xa930, 0xa946, 1},
    {0xa960, 0xa97c, 1}, {0xa984, 0xa9b2, 1}, {0xa9cf, 0xa9e0, 17}, {0xa9e1, 0xa9e4, 1},
    {0xa9e6, 0xa9ef, 1}, {0xa9fa, 0xa9fe, 1}, {0xaa00, 0xaa28, 1}, {0xaa40, 0xaa42, 1},
    {0xaa44, 0xaa4b, 1}, {0xaa60, 0xaa76, 1}, {0xaa7a, 0xaa7e, 4}, {0xaa7f, 0xaaaf, 1},
    {0xaab1, 0xaab5, 4}, {0xaab6, 0xaab9, 3}, {0xaaba, 0xaabd, 1}, {0xaac0, 0xaac2, 2},
    {0xaadb, 0xaadd, 1}, {0xaae0, 0xaaea, 1}, {0xaaf2, 0xaaf4, 1}, {0xab01, 0xab06, 1},
    {0xab09, 0xab0e, 1}, {0xab11, 0xab16, 1}, {0xab20, 0xab26, 1}, {0xab28, 0xab2e, 1},
    {0xab30, 0xab5a, 1}, {0xab5c, 0xab69, 1}, {0xab70, 0xabe2, 1}, {0xac00, 0xd7a3, 1},
    {0xd7b0, 0xd7c6, 1}, {0xd7cb, 0xd7fb, 1}, {0xf900, 0xfa6d, 1}, {0xfa70, 0xfad9, 1},
    {0xfb00, 0xfb06, 1}, {0xfb13, 0xfb17, 1}, {0xfb1d, 0xfb1f, 2}, {0xfb20, 0xfb28, 1},
    {0xfb2a, 0xfb36, 1}, {0xfb38, 0xfb3c, 1}, {0xfb3e, 0xfb40, 2}, {0xfb41, 0xfb43, 2},
    {0xfb44, 0xfb46, 2}, {0xfb47, 0xfbb1, 1}, {0xfbd3, 0xfd3d, 1}, {0xfd50, 0xfd8f, 1},
    {0xfd92, 0xfdc7, 1}, {0xfdf0, 0xfdfb, 1}, {0xfe70, 0xfe74, 1}, {0xfe76, 0xfefc, 1},
    {0xff21, 0xff3a, 1}, {0xff41, 0xff5a, 1}, {0xff66, 0xffbe, 1}, {0xffc2, 0xffc7, 1},
    {0xffca, 0xffcf, 1}, {0xffd2, 0xffd7, 1}, {0xffda, 0xffdc, 1},

    {0x10000, 0x1000b, 1}, {0x1000d, 0x10026, 1}, {0x10028, 0x1003a, 1}, {0x1003c, 0x1003d, 1},
    {0x1003f, 0x1004d, 1}, {0x10050, 0x1005d, 1}, {0x10080, 0x100fa, 1}, {0x10280, 0x1029c, 1},
    {0x102a0, 0x102d0, 1}, {0x10300, 0x1031f, 1}, {0x1032d, 0x10340, 1}, {0x10342, 0x10349, 1},
    {0x10350, 0x10375, 1}, {0x10380, 0x1039d, 1}, {0x103a0, 0x103c3, 1}, {0x103c8, 0x103cf, 1},
    {0x10400, 0x1049d, 1}, {0x104b0, 0x104d3, 1}, {0x104d8, 0x104fb, 1}, {0x10500, 0x10527, 1},
    {0x10530, 0x10563, 1}, {0x10600, 0x10736, 1}, {0x10740, 0x10755, 1}, {0x10760, 0x10767, 1},
    {0x10800, 0x10805, 1}, {0x10808, 0x1080a, 2}, {0x1080b, 0x10835, 1}, {0x10837, 0x10838, 1},
    {0x1083c, 0x1083f, 3}, {0x10840, 0x10855, 1}, {0x10860, 0x10876, 1}, {0x10880, 0x1089e, 1},
    {0x108e0, 0x108f2, 1}, {0x108f4, 0x108f5, 1}, {0x10900, 0x10915, 1}, {0x10920, 0x10939, 1},
    {0x10980, 0x109b7, 1}, {0x109be, 0x109bf, 1}, {0x10a00, 0x10a10, 16}, {0x10a11, 0x10a13, 1},
    {0x10a15, 0x10a17, 1}, {0x10a19, 0x10a35, 1}, {0x10a60, 0x10a7c, 1}, {0x10a80, 0x10a9c, 1},
    {0x10ac0, 0x10ac7, 1}, {0x10ac9, 0x10ae4, 1}, {0x10b00, 0x10b35, 1}, {0x10b40, 0x10b55, 1},
    {0x10b60, 0x10b72, 1}, {0x10b80, 0x10b91, 1}, {0x10c00, 0x10c48, 1}, {0x10c80, 0x10cb2, 1},
    {0x10cc0, 0x10cf2, 1}, {0x10d00, 0x10d23, 1}, {0x10e80, 0x10ea9, 1}, {0x10eb0, 0x10eb1, 1},
    {0x10f00, 0x10f1c, 1}, {0x10f27, 0x10f30, 9}, {0x10f31, 0x10f45, 1}, {0x10fb0, 0x10fc4, 1},
    {0x10fe0, 0x10ff6, 1}, {0x11003, 0x11037, 1}, {0x11083, 0x110af, 1}, {0x110d0, 0x110e8, 1},
    {0x11103, 0x11126, 1}, {0x11144, 0x11147, 3}, {0x11150, 0x11172, 1}, {0x11176, 0x11183, 13},
    {0x11184, 0x111b2, 1}, {0x111c1, 0x111c4, 1}, {0x111da, 0x111dc, 2}, {0x11200, 0x11211, 1},
    {0x11213, 0x1122b, 1}, {0x11280, 0x11286, 1}, {0x11288, 0x1128a, 2}, {0x1128b, 0x1128d, 1},
    {0x1128f, 0x1129d, 1}, {0x1129f, 0x112a8, 1}, {0x112b0, 0x112de, 1}, {0x11305, 0x1130c, 1},
    {0x1130f, 0x11310, 1}, {0x11313, 0x11328, 1}, {0x1132a, 0x11330, 1}, {0x11332, 0x11333, 1},
    {0x11335, 0x11339, 1}, {0x1133d, 0x11350, 19}, {0x1135d, 0x11361, 1}, {0x11400, 0x11434, 1},
    {0x11447, 0x1144a, 1}, {0x1145f, 0x11461, 1}, {0x11480, 0x114af, 1}, {0x114c4, 0x114c5, 1},
    {0x114c7, 0x11580, 185}, {0x11581, 0x115ae, 1}, {0x115d8, 0x115db, 1}, {0x11600, 0x1162f, 1},
    {0x11644, 0x11680, 60}, {0x11681, 0x116aa, 1}, {0x116b8, 0x11700, 72}, {0x11701, 0x1171a, 1},
    {0x11800, 0x1182b, 1}, {0x118a0, 0x118df, 1}, {0x118ff, 0x11906, 1}, {0x11909, 0x1190c, 3},
    {0x1190d, 0x11913, 1}, {0x11915, 0x11916, 1}, {0x11918, 0x1192f, 1}, {0x1193f, 0x11941, 2},
    {0x119a0, 0x119a7, 1}, {0x119aa, 0x119d0, 1}, {0x119e1, 0x119e3, 2}, {0x11a00, 0x11a0b, 11},
    {0x11a0c, 0x11a32, 1}, {0x11a3a, 0x11a50, 22}, {0x11a5c, 0x11a89, 1}, {0x11a9d, 0x11ac0, 35},
    {0x11ac1, 0x11af8, 1}, {0x11c00, 0x11c08, 1}, {0x11c0a, 0x11c2e, 1}, {0x11c40, 0x11c72, 50},
    {0x11c73, 0x11c8f, 1}, {0x11d00, 0x11d06, 1}, {0x11d08, 0x11d09, 1}, {0x11d0b, 0x11d30, 1},
    {0x11d46, 0x11d60, 26}, {0x11d61, 0x11d65, 1}, {0x11d67, 0x11d68, 1}, {0x11d6a, 0x11d89, 1},
    {0x11d98, 0x11ee0, 328}, {0x11ee1, 0x11ef2, 1}, {0x11fb0, 0x12000, 80}, {0x12001, 0x12399, 1},
    {0x12480, 0x12543, 1}, {0x13000, 0x1342e, 1}, {0x14400, 0x14646, 1}, {0x16800, 0x16a38, 1},
    {0x16a40, 0x16a5e, 1}, {0x16ad0, 0x16aed, 1}, {0x16b00, 0x16b2f, 1}, {0x16b40, 0x16b43, 1},
    {0x16b63, 0x16b77, 1}, {0x16b7d, 0x16b8f, 1}, {0x16e40, 0x16e7f, 1}, {0x16f00, 0x16f4a, 1},
    {0x16f50, 0x16f93, 67}, {0x16f94, 0x16f9f, 1}, {0x16fe0, 0x16fe1, 1}, {0x16fe3, 0x17000, 29},
    {0x17001, 0x187f7, 1}, {0x18800, 0x18cd5, 1}, {0x18d00, 0x18d08, 1}, {0x1b000, 0x1b11e, 1},
    {0x1b150, 0x1b152, 1}, {0x1b164, 0x1b167, 1}, {0x1b170, 0x1b2fb, 1}, {0x1bc00, 0x1bc6a, 1},
    {0x1bc70, 0x1bc7c, 1}, {0x1bc80, 0x1bc88, 1}, {0x1bc90, 0x1bc99, 1}, {0x1d400, 0x1d454, 1},
    {0x1d456, 0x1d49c, 1}, {0x1d49e, 0x1d49f, 1}, {0x1d4a2, 0x1d4a5, 3}, {0x1d4a6, 0x1d4a9, 3},
    {0x1d4aa, 0x1d4ac, 1}, {0x1d4ae, 0x1d4b9, 1}, {0x1d4bb, 0x1d4bd, 2}, {0x1d4be, 0x1d4c3, 1},
    {0x1d4c5, 0x1d505, 1}, {0x1d507, 0x1d50a, 1}, {0x1d50d, 0x1d514, 1}, {0x1d516, 0x1d51c, 1},
    {0x1d51e, 0x1d539, 1}, {0x1d53b, 0x1d53e, 1}, {0x1d540, 0x1d544, 1}, {0x1d546, 0x1d54a, 4},
    {0x1d54b, 0x1d550, 1}, {0x1d552, 0x1d6a5, 1}, {0x1d6a8, 0x1d6c0, 1}, {0x1d6c2, 0x1d6da, 1},
    {0x1d6dc, 0x1d6fa, 1}, {0x1d6fc, 0x1d714, 1}, {0x1d716, 0x1d734, 1}, {0x1d736, 0x1d74e, 1},
    {0x1d750, 0x1d76e, 1}, {0x1d770, 0x1d788, 1}, {0x1d78a, 0x1d7a8, 1}, {0x1d7aa, 0x1d7c2, 1},
    {0x1d7c4, 0x1d7cb, 1}, {0x1e100, 0x1e12c, 1}, {0x1e137, 0x1e13d, 1}, {0x1e14e, 0x1e2c0, 370},
    {0x1e2c1, 0x1e2eb, 1}, {0x1e800, 0x1e8c4, 1}, {0x1e900, 0x1e943, 1}, {0x1e94b, 0x1ee00, 1205},
    {0x1ee01, 0x1ee03, 1}, {0x1ee05, 0x1ee1f, 1}, {0x1ee21, 0x1ee22, 1}, {0x1ee24, 0x1ee27, 3},
    {0x1ee29, 0x1ee32, 1}, {0x1ee34, 0x1ee37, 1}, {0x1ee39, 0x1ee3b, 2}, {0x1ee42, 0x1ee47, 5},
    {0x1ee49, 0x1ee4d, 2}, {0x1ee4e, 0x1ee4f, 1}, {0x1ee51, 0x1ee52, 1}, {0x1ee54, 0x1ee57, 3},
    {0x1ee59, 0x1ee61, 2}, {0x1ee62, 0x1ee64, 2}, {0x1ee67, 0x1ee6a, 1}, {0x1ee6c, 0x1ee72, 1},
    {0x1ee74, 0x1ee77, 1}, {0x1ee79, 0x1ee7c, 1}, {0x1ee7e, 0x1ee80, 2}, {0x1ee81, 0x1ee89, 1},
    {0x1ee8b, 0x1ee9b, 1}, {0x1eea1, 0x1eea3, 1}, {0x1eea5, 0x1eea9, 1}, {0x1eeab, 0x1eebb, 1},
    {0x20000, 0x2a6dd, 1}, {0x2a700, 0x2b734, 1}, {0x2b740, 0x2b81d, 1}, {0x2b820, 0x2cea1, 1},
    {0x2ceb0, 0x2ebe0, 1}, {0x2f800, 0x2fa1d, 1}, {0x30000, 0x3134a, 1}
};
static P4_RuneRange _Ll[] = {
    {0x0061, 0x007a, 1}, {0x00b5, 0x00df, 42}, {0x00e0, 0x00f6, 1}, {0x00f8, 0x00ff, 1},
    {0x0101, 0x0137, 2}, {0x0138, 0x0148, 2}, {0x0149, 0x0177, 2}, {0x017a, 0x017e, 2},
    {0x017f, 0x0180, 1}, {0x0183, 0x0185, 2}, {0x0188, 0x018c, 4}, {0x018d, 0x0192, 5},
    {0x0195, 0x0199, 4}, {0x019a, 0x019b, 1}, {0x019e, 0x01a1, 3}, {0x01a3, 0x01a5, 2},
    {0x01a8, 0x01aa, 2}, {0x01ab, 0x01ad, 2}, {0x01b0, 0x01b4, 4}, {0x01b6, 0x01b9, 3},
    {0x01ba, 0x01bd, 3}, {0x01be, 0x01bf, 1}, {0x01c6, 0x01cc, 3}, {0x01ce, 0x01dc, 2},
    {0x01dd, 0x01ef, 2}, {0x01f0, 0x01f3, 3}, {0x01f5, 0x01f9, 4}, {0x01fb, 0x0233, 2},
    {0x0234, 0x0239, 1}, {0x023c, 0x023f, 3}, {0x0240, 0x0242, 2}, {0x0247, 0x024f, 2},
    {0x0250, 0x0293, 1}, {0x0295, 0x02af, 1}, {0x0371, 0x0373, 2}, {0x0377, 0x037b, 4},
    {0x037c, 0x037d, 1}, {0x0390, 0x03ac, 28}, {0x03ad, 0x03ce, 1}, {0x03d0, 0x03d1, 1},
    {0x03d5, 0x03d7, 1}, {0x03d9, 0x03ef, 2}, {0x03f0, 0x03f3, 1}, {0x03f5, 0x03fb, 3},
    {0x03fc, 0x0430, 52}, {0x0431, 0x045f, 1}, {0x0461, 0x0481, 2}, {0x048b, 0x04bf, 2},
    {0x04c2, 0x04ce, 2}, {0x04cf, 0x052f, 2}, {0x0560, 0x0588, 1}, {0x10d0, 0x10fa, 1},
    {0x10fd, 0x10ff, 1}, {0x13f8, 0x13fd, 1}, {0x1c80, 0x1c88, 1}, {0x1d00, 0x1d2b, 1},
    {0x1d6b, 0x1d77, 1}, {0x1d79, 0x1d9a, 1}, {0x1e01, 0x1e95, 2}, {0x1e96, 0x1e9d, 1},
    {0x1e9f, 0x1eff, 2}, {0x1f00, 0x1f07, 1}, {0x1f10, 0x1f15, 1}, {0x1f20, 0x1f27, 1},
    {0x1f30, 0x1f37, 1}, {0x1f40, 0x1f45, 1}, {0x1f50, 0x1f57, 1}, {0x1f60, 0x1f67, 1},
    {0x1f70, 0x1f7d, 1}, {0x1f80, 0x1f87, 1}, {0x1f90, 0x1f97, 1}, {0x1fa0, 0x1fa7, 1},
    {0x1fb0, 0x1fb4, 1}, {0x1fb6, 0x1fb7, 1}, {0x1fbe, 0x1fc2, 4}, {0x1fc3, 0x1fc4, 1},
    {0x1fc6, 0x1fc7, 1}, {0x1fd0, 0x1fd3, 1}, {0x1fd6, 0x1fd7, 1}, {0x1fe0, 0x1fe7, 1},
    {0x1ff2, 0x1ff4, 1}, {0x1ff6, 0x1ff7, 1}, {0x210a, 0x210e, 4}, {0x210f, 0x2113, 4},
    {0x212f, 0x2139, 5}, {0x213c, 0x213d, 1}, {0x2146, 0x2149, 1}, {0x214e, 0x2184, 54},
    {0x2c30, 0x2c5e, 1}, {0x2c61, 0x2c65, 4}, {0x2c66, 0x2c6c, 2}, {0x2c71, 0x2c73, 2},
    {0x2c74, 0x2c76, 2}, {0x2c77, 0x2c7b, 1}, {0x2c81, 0x2ce3, 2}, {0x2ce4, 0x2cec, 8},
    {0x2cee, 0x2cf3, 5}, {0x2d00, 0x2d25, 1}, {0x2d27, 0x2d2d, 6}, {0xa641, 0xa66d, 2},
    {0xa681, 0xa69b, 2}, {0xa723, 0xa72f, 2}, {0xa730, 0xa731, 1}, {0xa733, 0xa771, 2},
    {0xa772, 0xa778, 1}, {0xa77a, 0xa77c, 2}, {0xa77f, 0xa787, 2}, {0xa78c, 0xa78e, 2},
    {0xa791, 0xa793, 2}, {0xa794, 0xa795, 1}, {0xa797, 0xa7a9, 2}, {0xa7af, 0xa7b5, 6},
    {0xa7b7, 0xa7bf, 2}, {0xa7c3, 0xa7c8, 5}, {0xa7ca, 0xa7f6, 44}, {0xa7fa, 0xab30, 822},
    {0xab31, 0xab5a, 1}, {0xab60, 0xab68, 1}, {0xab70, 0xabbf, 1}, {0xfb00, 0xfb06, 1},
    {0xfb13, 0xfb17, 1}, {0xff41, 0xff5a, 1},
    {0x10428, 0x1044f, 1}, {0x104d8, 0x104fb, 1}, {0x10cc0, 0x10cf2, 1}, {0x118c0, 0x118df, 1},
    {0x16e60, 0x16e7f, 1}, {0x1d41a, 0x1d433, 1}, {0x1d44e, 0x1d454, 1}, {0x1d456, 0x1d467, 1},
    {0x1d482, 0x1d49b, 1}, {0x1d4b6, 0x1d4b9, 1}, {0x1d4bb, 0x1d4bd, 2}, {0x1d4be, 0x1d4c3, 1},
    {0x1d4c5, 0x1d4cf, 1}, {0x1d4ea, 0x1d503, 1}, {0x1d51e, 0x1d537, 1}, {0x1d552, 0x1d56b, 1},
    {0x1d586, 0x1d59f, 1}, {0x1d5ba, 0x1d5d3, 1}, {0x1d5ee, 0x1d607, 1}, {0x1d622, 0x1d63b, 1},
    {0x1d656, 0x1d66f, 1}, {0x1d68a, 0x1d6a5, 1}, {0x1d6c2, 0x1d6da, 1}, {0x1d6dc, 0x1d6e1, 1},
    {0x1d6fc, 0x1d714, 1}, {0x1d716, 0x1d71b, 1}, {0x1d736, 0x1d74e, 1}, {0x1d750, 0x1d755, 1},
    {0x1d770, 0x1d788, 1}, {0x1d78a, 0x1d78f, 1}, {0x1d7aa, 0x1d7c2, 1}, {0x1d7c4, 0x1d7c9, 1},
    {0x1d7cb, 0x1e922, 4439}, {0x1e923, 0x1e943, 1},
};
static P4_RuneRange _Lm[] = {
    {0x02b0, 0x02c1, 1}, {0x02c6, 0x02d1, 1}, {0x02e0, 0x02e4, 1}, {0x02ec, 0x02ee, 2},
    {0x0374, 0x037a, 6}, {0x0559, 0x0640, 231}, {0x06e5, 0x06e6, 1}, {0x07f4, 0x07f5, 1},
    {0x07fa, 0x081a, 32}, {0x0824, 0x0828, 4}, {0x0971, 0x0e46, 1237}, {0x0ec6, 0x10fc, 566},
    {0x17d7, 0x1843, 108}, {0x1aa7, 0x1c78, 465}, {0x1c79, 0x1c7d, 1}, {0x1d2c, 0x1d6a, 1},
    {0x1d78, 0x1d9b, 35}, {0x1d9c, 0x1dbf, 1}, {0x2071, 0x207f, 14}, {0x2090, 0x209c, 1},
    {0x2c7c, 0x2c7d, 1}, {0x2d6f, 0x2e2f, 192}, {0x3005, 0x3031, 44}, {0x3032, 0x3035, 1},
    {0x303b, 0x309d, 98}, {0x309e, 0x30fc, 94}, {0x30fd, 0x30fe, 1}, {0xa015, 0xa4f8, 1251},
    {0xa4f9, 0xa4fd, 1}, {0xa60c, 0xa67f, 115}, {0xa69c, 0xa69d, 1}, {0xa717, 0xa71f, 1},
    {0xa770, 0xa788, 24}, {0xa7f8, 0xa7f9, 1}, {0xa9cf, 0xa9e6, 23}, {0xaa70, 0xaadd, 109},
    {0xaaf3, 0xaaf4, 1}, {0xab5c, 0xab5f, 1}, {0xab69, 0xff70, 21511}, {0xff9e, 0xff9f, 1},
    {0x16b40, 0x16b43, 1}, {0x16f93, 0x16f9f, 1}, {0x16fe0, 0x16fe1, 1}, {0x16fe3, 0x1e137, 29012},
    {0x1e138, 0x1e13d, 1}, {0x1e94b, 0x1e94b, 1},
};
static P4_RuneRange _Lo[] = {
    {0x00aa, 0x00ba, 16}, {0x01bb, 0x01c0, 5}, {0x01c1, 0x01c3, 1}, {0x0294, 0x05d0, 828},
    {0x05d1, 0x05ea, 1}, {0x05ef, 0x05f2, 1}, {0x0620, 0x063f, 1}, {0x0641, 0x064a, 1},
    {0x066e, 0x066f, 1}, {0x0671, 0x06d3, 1}, {0x06d5, 0x06ee, 25}, {0x06ef, 0x06fa, 11},
    {0x06fb, 0x06fc, 1}, {0x06ff, 0x0710, 17}, {0x0712, 0x072f, 1}, {0x074d, 0x07a5, 1},
    {0x07b1, 0x07ca, 25}, {0x07cb, 0x07ea, 1}, {0x0800, 0x0815, 1}, {0x0840, 0x0858, 1},
    {0x0860, 0x086a, 1}, {0x08a0, 0x08b4, 1}, {0x08b6, 0x08c7, 1}, {0x0904, 0x0939, 1},
    {0x093d, 0x0950, 19}, {0x0958, 0x0961, 1}, {0x0972, 0x0980, 1}, {0x0985, 0x098c, 1},
    {0x098f, 0x0990, 1}, {0x0993, 0x09a8, 1}, {0x09aa, 0x09b0, 1}, {0x09b2, 0x09b6, 4},
    {0x09b7, 0x09b9, 1}, {0x09bd, 0x09ce, 17}, {0x09dc, 0x09dd, 1}, {0x09df, 0x09e1, 1},
    {0x09f0, 0x09f1, 1}, {0x09fc, 0x0a05, 9}, {0x0a06, 0x0a0a, 1}, {0x0a0f, 0x0a10, 1},
    {0x0a13, 0x0a28, 1}, {0x0a2a, 0x0a30, 1}, {0x0a32, 0x0a33, 1}, {0x0a35, 0x0a36, 1},
    {0x0a38, 0x0a39, 1}, {0x0a59, 0x0a5c, 1}, {0x0a5e, 0x0a72, 20}, {0x0a73, 0x0a74, 1},
    {0x0a85, 0x0a8d, 1}, {0x0a8f, 0x0a91, 1}, {0x0a93, 0x0aa8, 1}, {0x0aaa, 0x0ab0, 1},
    {0x0ab2, 0x0ab3, 1}, {0x0ab5, 0x0ab9, 1}, {0x0abd, 0x0ad0, 19}, {0x0ae0, 0x0ae1, 1},
    {0x0af9, 0x0b05, 12}, {0x0b06, 0x0b0c, 1}, {0x0b0f, 0x0b10, 1}, {0x0b13, 0x0b28, 1},
    {0x0b2a, 0x0b30, 1}, {0x0b32, 0x0b33, 1}, {0x0b35, 0x0b39, 1}, {0x0b3d, 0x0b5c, 31},
    {0x0b5d, 0x0b5f, 2}, {0x0b60, 0x0b61, 1}, {0x0b71, 0x0b83, 18}, {0x0b85, 0x0b8a, 1},
    {0x0b8e, 0x0b90, 1}, {0x0b92, 0x0b95, 1}, {0x0b99, 0x0b9a, 1}, {0x0b9c, 0x0b9e, 2},
    {0x0b9f, 0x0ba3, 4}, {0x0ba4, 0x0ba8, 4}, {0x0ba9, 0x0baa, 1}, {0x0bae, 0x0bb9, 1},
    {0x0bd0, 0x0c05, 53}, {0x0c06, 0x0c0c, 1}, {0x0c0e, 0x0c10, 1}, {0x0c12, 0x0c28, 1},
    {0x0c2a, 0x0c39, 1}, {0x0c3d, 0x0c58, 27}, {0x0c59, 0x0c5a, 1}, {0x0c60, 0x0c61, 1},
    {0x0c80, 0x0c85, 5}, {0x0c86, 0x0c8c, 1}, {0x0c8e, 0x0c90, 1}, {0x0c92, 0x0ca8, 1},
    {0x0caa, 0x0cb3, 1}, {0x0cb5, 0x0cb9, 1}, {0x0cbd, 0x0cde, 33}, {0x0ce0, 0x0ce1, 1},
    {0x0cf1, 0x0cf2, 1}, {0x0d04, 0x0d0c, 1}, {0x0d0e, 0x0d10, 1}, {0x0d12, 0x0d3a, 1},
    {0x0d3d, 0x0d4e, 17}, {0x0d54, 0x0d56, 1}, {0x0d5f, 0x0d61, 1}, {0x0d7a, 0x0d7f, 1},
    {0x0d85, 0x0d96, 1}, {0x0d9a, 0x0db1, 1}, {0x0db3, 0x0dbb, 1}, {0x0dbd, 0x0dc0, 3},
    {0x0dc1, 0x0dc6, 1}, {0x0e01, 0x0e30, 1}, {0x0e32, 0x0e33, 1}, {0x0e40, 0x0e45, 1},
    {0x0e81, 0x0e82, 1}, {0x0e84, 0x0e86, 2}, {0x0e87, 0x0e8a, 1}, {0x0e8c, 0x0ea3, 1},
    {0x0ea5, 0x0ea7, 2}, {0x0ea8, 0x0eb0, 1}, {0x0eb2, 0x0eb3, 1}, {0x0ebd, 0x0ec0, 3},
    {0x0ec1, 0x0ec4, 1}, {0x0edc, 0x0edf, 1}, {0x0f00, 0x0f40, 64}, {0x0f41, 0x0f47, 1},
    {0x0f49, 0x0f6c, 1}, {0x0f88, 0x0f8c, 1}, {0x1000, 0x102a, 1}, {0x103f, 0x1050, 17},
    {0x1051, 0x1055, 1}, {0x105a, 0x105d, 1}, {0x1061, 0x1065, 4}, {0x1066, 0x106e, 8},
    {0x106f, 0x1070, 1}, {0x1075, 0x1081, 1}, {0x108e, 0x1100, 114}, {0x1101, 0x1248, 1},
    {0x124a, 0x124d, 1}, {0x1250, 0x1256, 1}, {0x1258, 0x125a, 2}, {0x125b, 0x125d, 1},
    {0x1260, 0x1288, 1}, {0x128a, 0x128d, 1}, {0x1290, 0x12b0, 1}, {0x12b2, 0x12b5, 1},
    {0x12b8, 0x12be, 1}, {0x12c0, 0x12c2, 2}, {0x12c3, 0x12c5, 1}, {0x12c8, 0x12d6, 1},
    {0x12d8, 0x1310, 1}, {0x1312, 0x1315, 1}, {0x1318, 0x135a, 1}, {0x1380, 0x138f, 1},
    {0x1401, 0x166c, 1}, {0x166f, 0x167f, 1}, {0x1681, 0x169a, 1}, {0x16a0, 0x16ea, 1},
    {0x16f1, 0x16f8, 1}, {0x1700, 0x170c, 1}, {0x170e, 0x1711, 1}, {0x1720, 0x1731, 1},
    {0x1740, 0x1751, 1}, {0x1760, 0x176c, 1}, {0x176e, 0x1770, 1}, {0x1780, 0x17b3, 1},
    {0x17dc, 0x1820, 68}, {0x1821, 0x1842, 1}, {0x1844, 0x1878, 1}, {0x1880, 0x1884, 1},
    {0x1887, 0x18a8, 1}, {0x18aa, 0x18b0, 6}, {0x18b1, 0x18f5, 1}, {0x1900, 0x191e, 1},
    {0x1950, 0x196d, 1}, {0x1970, 0x1974, 1}, {0x1980, 0x19ab, 1}, {0x19b0, 0x19c9, 1},
    {0x1a00, 0x1a16, 1}, {0x1a20, 0x1a54, 1}, {0x1b05, 0x1b33, 1}, {0x1b45, 0x1b4b, 1},
    {0x1b83, 0x1ba0, 1}, {0x1bae, 0x1baf, 1}, {0x1bba, 0x1be5, 1}, {0x1c00, 0x1c23, 1},
    {0x1c4d, 0x1c4f, 1}, {0x1c5a, 0x1c77, 1}, {0x1ce9, 0x1cec, 1}, {0x1cee, 0x1cf3, 1},
    {0x1cf5, 0x1cf6, 1}, {0x1cfa, 0x2135, 1083}, {0x2136, 0x2138, 1}, {0x2d30, 0x2d67, 1},
    {0x2d80, 0x2d96, 1}, {0x2da0, 0x2da6, 1}, {0x2da8, 0x2dae, 1}, {0x2db0, 0x2db6, 1},
    {0x2db8, 0x2dbe, 1}, {0x2dc0, 0x2dc6, 1}, {0x2dc8, 0x2dce, 1}, {0x2dd0, 0x2dd6, 1},
    {0x2dd8, 0x2dde, 1}, {0x3006, 0x303c, 54}, {0x3041, 0x3096, 1}, {0x309f, 0x30a1, 2},
    {0x30a2, 0x30fa, 1}, {0x30ff, 0x3105, 6}, {0x3106, 0x312f, 1}, {0x3131, 0x318e, 1},
    {0x31a0, 0x31bf, 1}, {0x31f0, 0x31ff, 1}, {0x3400, 0x4dbf, 1}, {0x4e00, 0x9ffc, 1},
    {0xa000, 0xa014, 1}, {0xa016, 0xa48c, 1}, {0xa4d0, 0xa4f7, 1}, {0xa500, 0xa60b, 1},
    {0xa610, 0xa61f, 1}, {0xa62a, 0xa62b, 1}, {0xa66e, 0xa6a0, 50}, {0xa6a1, 0xa6e5, 1},
    {0xa78f, 0xa7f7, 104}, {0xa7fb, 0xa801, 1}, {0xa803, 0xa805, 1}, {0xa807, 0xa80a, 1},
    {0xa80c, 0xa822, 1}, {0xa840, 0xa873, 1}, {0xa882, 0xa8b3, 1}, {0xa8f2, 0xa8f7, 1},
    {0xa8fb, 0xa8fd, 2}, {0xa8fe, 0xa90a, 12}, {0xa90b, 0xa925, 1}, {0xa930, 0xa946, 1},
    {0xa960, 0xa97c, 1}, {0xa984, 0xa9b2, 1}, {0xa9e0, 0xa9e4, 1}, {0xa9e7, 0xa9ef, 1},
    {0xa9fa, 0xa9fe, 1}, {0xaa00, 0xaa28, 1}, {0xaa40, 0xaa42, 1}, {0xaa44, 0xaa4b, 1},
    {0xaa60, 0xaa6f, 1}, {0xaa71, 0xaa76, 1}, {0xaa7a, 0xaa7e, 4}, {0xaa7f, 0xaaaf, 1},
    {0xaab1, 0xaab5, 4}, {0xaab6, 0xaab9, 3}, {0xaaba, 0xaabd, 1}, {0xaac0, 0xaac2, 2},
    {0xaadb, 0xaadc, 1}, {0xaae0, 0xaaea, 1}, {0xaaf2, 0xab01, 15}, {0xab02, 0xab06, 1},
    {0xab09, 0xab0e, 1}, {0xab11, 0xab16, 1}, {0xab20, 0xab26, 1}, {0xab28, 0xab2e, 1},
    {0xabc0, 0xabe2, 1}, {0xac00, 0xd7a3, 1}, {0xd7b0, 0xd7c6, 1}, {0xd7cb, 0xd7fb, 1},
    {0xf900, 0xfa6d, 1}, {0xfa70, 0xfad9, 1}, {0xfb1d, 0xfb1f, 2}, {0xfb20, 0xfb28, 1},
    {0xfb2a, 0xfb36, 1}, {0xfb38, 0xfb3c, 1}, {0xfb3e, 0xfb40, 2}, {0xfb41, 0xfb43, 2},
    {0xfb44, 0xfb46, 2}, {0xfb47, 0xfbb1, 1}, {0xfbd3, 0xfd3d, 1}, {0xfd50, 0xfd8f, 1},
    {0xfd92, 0xfdc7, 1}, {0xfdf0, 0xfdfb, 1}, {0xfe70, 0xfe74, 1}, {0xfe76, 0xfefc, 1},
    {0xff66, 0xff6f, 1}, {0xff71, 0xff9d, 1}, {0xffa0, 0xffbe, 1}, {0xffc2, 0xffc7, 1},
    {0xffca, 0xffcf, 1}, {0xffd2, 0xffd7, 1}, {0xffda, 0xffdc, 1},
    {0x10000, 0x1000b, 1}, {0x1000d, 0x10026, 1}, {0x10028, 0x1003a, 1}, {0x1003c, 0x1003d, 1},
    {0x1003f, 0x1004d, 1}, {0x10050, 0x1005d, 1}, {0x10080, 0x100fa, 1}, {0x10280, 0x1029c, 1},
    {0x102a0, 0x102d0, 1}, {0x10300, 0x1031f, 1}, {0x1032d, 0x10340, 1}, {0x10342, 0x10349, 1},
    {0x10350, 0x10375, 1}, {0x10380, 0x1039d, 1}, {0x103a0, 0x103c3, 1}, {0x103c8, 0x103cf, 1},
    {0x10450, 0x1049d, 1}, {0x10500, 0x10527, 1}, {0x10530, 0x10563, 1}, {0x10600, 0x10736, 1},
    {0x10740, 0x10755, 1}, {0x10760, 0x10767, 1}, {0x10800, 0x10805, 1}, {0x10808, 0x1080a, 2},
    {0x1080b, 0x10835, 1}, {0x10837, 0x10838, 1}, {0x1083c, 0x1083f, 3}, {0x10840, 0x10855, 1},
    {0x10860, 0x10876, 1}, {0x10880, 0x1089e, 1}, {0x108e0, 0x108f2, 1}, {0x108f4, 0x108f5, 1},
    {0x10900, 0x10915, 1}, {0x10920, 0x10939, 1}, {0x10980, 0x109b7, 1}, {0x109be, 0x109bf, 1},
    {0x10a00, 0x10a10, 16}, {0x10a11, 0x10a13, 1}, {0x10a15, 0x10a17, 1}, {0x10a19, 0x10a35, 1},
    {0x10a60, 0x10a7c, 1}, {0x10a80, 0x10a9c, 1}, {0x10ac0, 0x10ac7, 1}, {0x10ac9, 0x10ae4, 1},
    {0x10b00, 0x10b35, 1}, {0x10b40, 0x10b55, 1}, {0x10b60, 0x10b72, 1}, {0x10b80, 0x10b91, 1},
    {0x10c00, 0x10c48, 1}, {0x10d00, 0x10d23, 1}, {0x10e80, 0x10ea9, 1}, {0x10eb0, 0x10eb1, 1},
    {0x10f00, 0x10f1c, 1}, {0x10f27, 0x10f30, 9}, {0x10f31, 0x10f45, 1}, {0x10fb0, 0x10fc4, 1},
    {0x10fe0, 0x10ff6, 1}, {0x11003, 0x11037, 1}, {0x11083, 0x110af, 1}, {0x110d0, 0x110e8, 1},
    {0x11103, 0x11126, 1}, {0x11144, 0x11147, 3}, {0x11150, 0x11172, 1}, {0x11176, 0x11183, 13},
    {0x11184, 0x111b2, 1}, {0x111c1, 0x111c4, 1}, {0x111da, 0x111dc, 2}, {0x11200, 0x11211, 1},
    {0x11213, 0x1122b, 1}, {0x11280, 0x11286, 1}, {0x11288, 0x1128a, 2}, {0x1128b, 0x1128d, 1},
    {0x1128f, 0x1129d, 1}, {0x1129f, 0x112a8, 1}, {0x112b0, 0x112de, 1}, {0x11305, 0x1130c, 1},
    {0x1130f, 0x11310, 1}, {0x11313, 0x11328, 1}, {0x1132a, 0x11330, 1}, {0x11332, 0x11333, 1},
    {0x11335, 0x11339, 1}, {0x1133d, 0x11350, 19}, {0x1135d, 0x11361, 1}, {0x11400, 0x11434, 1},
    {0x11447, 0x1144a, 1}, {0x1145f, 0x11461, 1}, {0x11480, 0x114af, 1}, {0x114c4, 0x114c5, 1},
    {0x114c7, 0x11580, 185}, {0x11581, 0x115ae, 1}, {0x115d8, 0x115db, 1}, {0x11600, 0x1162f, 1},
    {0x11644, 0x11680, 60}, {0x11681, 0x116aa, 1}, {0x116b8, 0x11700, 72}, {0x11701, 0x1171a, 1},
    {0x11800, 0x1182b, 1}, {0x118ff, 0x11906, 1}, {0x11909, 0x1190c, 3}, {0x1190d, 0x11913, 1},
    {0x11915, 0x11916, 1}, {0x11918, 0x1192f, 1}, {0x1193f, 0x11941, 2}, {0x119a0, 0x119a7, 1},
    {0x119aa, 0x119d0, 1}, {0x119e1, 0x119e3, 2}, {0x11a00, 0x11a0b, 11}, {0x11a0c, 0x11a32, 1},
    {0x11a3a, 0x11a50, 22}, {0x11a5c, 0x11a89, 1}, {0x11a9d, 0x11ac0, 35}, {0x11ac1, 0x11af8, 1},
    {0x11c00, 0x11c08, 1}, {0x11c0a, 0x11c2e, 1}, {0x11c40, 0x11c72, 50}, {0x11c73, 0x11c8f, 1},
    {0x11d00, 0x11d06, 1}, {0x11d08, 0x11d09, 1}, {0x11d0b, 0x11d30, 1}, {0x11d46, 0x11d60, 26},
    {0x11d61, 0x11d65, 1}, {0x11d67, 0x11d68, 1}, {0x11d6a, 0x11d89, 1}, {0x11d98, 0x11ee0, 328},
    {0x11ee1, 0x11ef2, 1}, {0x11fb0, 0x12000, 80}, {0x12001, 0x12399, 1}, {0x12480, 0x12543, 1},
    {0x13000, 0x1342e, 1}, {0x14400, 0x14646, 1}, {0x16800, 0x16a38, 1}, {0x16a40, 0x16a5e, 1},
    {0x16ad0, 0x16aed, 1}, {0x16b00, 0x16b2f, 1}, {0x16b63, 0x16b77, 1}, {0x16b7d, 0x16b8f, 1},
    {0x16f00, 0x16f4a, 1}, {0x16f50, 0x17000, 176}, {0x17001, 0x187f7, 1}, {0x18800, 0x18cd5, 1},
    {0x18d00, 0x18d08, 1}, {0x1b000, 0x1b11e, 1}, {0x1b150, 0x1b152, 1}, {0x1b164, 0x1b167, 1},
    {0x1b170, 0x1b2fb, 1}, {0x1bc00, 0x1bc6a, 1}, {0x1bc70, 0x1bc7c, 1}, {0x1bc80, 0x1bc88, 1},
    {0x1bc90, 0x1bc99, 1}, {0x1e100, 0x1e12c, 1}, {0x1e14e, 0x1e2c0, 370}, {0x1e2c1, 0x1e2eb, 1},
    {0x1e800, 0x1e8c4, 1}, {0x1ee00, 0x1ee03, 1}, {0x1ee05, 0x1ee1f, 1}, {0x1ee21, 0x1ee22, 1},
    {0x1ee24, 0x1ee27, 3}, {0x1ee29, 0x1ee32, 1}, {0x1ee34, 0x1ee37, 1}, {0x1ee39, 0x1ee3b, 2},
    {0x1ee42, 0x1ee47, 5}, {0x1ee49, 0x1ee4d, 2}, {0x1ee4e, 0x1ee4f, 1}, {0x1ee51, 0x1ee52, 1},
    {0x1ee54, 0x1ee57, 3}, {0x1ee59, 0x1ee61, 2}, {0x1ee62, 0x1ee64, 2}, {0x1ee67, 0x1ee6a, 1},
    {0x1ee6c, 0x1ee72, 1}, {0x1ee74, 0x1ee77, 1}, {0x1ee79, 0x1ee7c, 1}, {0x1ee7e, 0x1ee80, 2},
    {0x1ee81, 0x1ee89, 1}, {0x1ee8b, 0x1ee9b, 1}, {0x1eea1, 0x1eea3, 1}, {0x1eea5, 0x1eea9, 1},
    {0x1eeab, 0x1eebb, 1}, {0x20000, 0x2a6dd, 1}, {0x2a700, 0x2b734, 1}, {0x2b740, 0x2b81d, 1},
    {0x2b820, 0x2cea1, 1}, {0x2ceb0, 0x2ebe0, 1}, {0x2f800, 0x2fa1d, 1}, {0x30000, 0x3134a, 1},
};
static P4_RuneRange _Lt[] = {
    {0x01c5, 0x01cb, 3}, {0x01f2, 0x1f88, 7574}, {0x1f89, 0x1f8f, 1}, {0x1f98, 0x1f9f, 1},
    {0x1fa8, 0x1faf, 1}, {0x1fbc, 0x1fcc, 16}, {0x1ffc, 0x1ffc, 1},
};
static P4_RuneRange _Lu[] = {
    {0x0041, 0x005a, 1}, {0x00c0, 0x00d6, 1}, {0x00d8, 0x00de, 1}, {0x0100, 0x0136, 2},
    {0x0139, 0x0147, 2}, {0x014a, 0x0178, 2}, {0x0179, 0x017d, 2}, {0x0181, 0x0182, 1},
    {0x0184, 0x0186, 2}, {0x0187, 0x0189, 2}, {0x018a, 0x018b, 1}, {0x018e, 0x0191, 1},
    {0x0193, 0x0194, 1}, {0x0196, 0x0198, 1}, {0x019c, 0x019d, 1}, {0x019f, 0x01a0, 1},
    {0x01a2, 0x01a6, 2}, {0x01a7, 0x01a9, 2}, {0x01ac, 0x01ae, 2}, {0x01af, 0x01b1, 2},
    {0x01b2, 0x01b3, 1}, {0x01b5, 0x01b7, 2}, {0x01b8, 0x01bc, 4}, {0x01c4, 0x01cd, 3},
    {0x01cf, 0x01db, 2}, {0x01de, 0x01ee, 2}, {0x01f1, 0x01f4, 3}, {0x01f6, 0x01f8, 1},
    {0x01fa, 0x0232, 2}, {0x023a, 0x023b, 1}, {0x023d, 0x023e, 1}, {0x0241, 0x0243, 2},
    {0x0244, 0x0246, 1}, {0x0248, 0x024e, 2}, {0x0370, 0x0372, 2}, {0x0376, 0x037f, 9},
    {0x0386, 0x0388, 2}, {0x0389, 0x038a, 1}, {0x038c, 0x038e, 2}, {0x038f, 0x0391, 2},
    {0x0392, 0x03a1, 1}, {0x03a3, 0x03ab, 1}, {0x03cf, 0x03d2, 3}, {0x03d3, 0x03d4, 1},
    {0x03d8, 0x03ee, 2}, {0x03f4, 0x03f7, 3}, {0x03f9, 0x03fa, 1}, {0x03fd, 0x042f, 1},
    {0x0460, 0x0480, 2}, {0x048a, 0x04c0, 2}, {0x04c1, 0x04cd, 2}, {0x04d0, 0x052e, 2},
    {0x0531, 0x0556, 1}, {0x10a0, 0x10c5, 1}, {0x10c7, 0x10cd, 6}, {0x13a0, 0x13f5, 1},
    {0x1c90, 0x1cba, 1}, {0x1cbd, 0x1cbf, 1}, {0x1e00, 0x1e94, 2}, {0x1e9e, 0x1efe, 2},
    {0x1f08, 0x1f0f, 1}, {0x1f18, 0x1f1d, 1}, {0x1f28, 0x1f2f, 1}, {0x1f38, 0x1f3f, 1},
    {0x1f48, 0x1f4d, 1}, {0x1f59, 0x1f5f, 2}, {0x1f68, 0x1f6f, 1}, {0x1fb8, 0x1fbb, 1},
    {0x1fc8, 0x1fcb, 1}, {0x1fd8, 0x1fdb, 1}, {0x1fe8, 0x1fec, 1}, {0x1ff8, 0x1ffb, 1},
    {0x2102, 0x2107, 5}, {0x210b, 0x210d, 1}, {0x2110, 0x2112, 1}, {0x2115, 0x2119, 4},
    {0x211a, 0x211d, 1}, {0x2124, 0x212a, 2}, {0x212b, 0x212d, 1}, {0x2130, 0x2133, 1},
    {0x213e, 0x213f, 1}, {0x2145, 0x2183, 62}, {0x2c00, 0x2c2e, 1}, {0x2c60, 0x2c62, 2},
    {0x2c63, 0x2c64, 1}, {0x2c67, 0x2c6d, 2}, {0x2c6e, 0x2c70, 1}, {0x2c72, 0x2c75, 3},
    {0x2c7e, 0x2c80, 1}, {0x2c82, 0x2ce2, 2}, {0x2ceb, 0x2ced, 2}, {0x2cf2, 0xa640, 31054},
    {0xa642, 0xa66c, 2}, {0xa680, 0xa69a, 2}, {0xa722, 0xa72e, 2}, {0xa732, 0xa76e, 2},
    {0xa779, 0xa77d, 2}, {0xa77e, 0xa786, 2}, {0xa78b, 0xa78d, 2}, {0xa790, 0xa792, 2},
    {0xa796, 0xa7aa, 2}, {0xa7ab, 0xa7ae, 1}, {0xa7b0, 0xa7b4, 1}, {0xa7b6, 0xa7be, 2},
    {0xa7c2, 0xa7c4, 2}, {0xa7c5, 0xa7c7, 1}, {0xa7c9, 0xa7f5, 44}, {0xff21, 0xff3a, 1},
    {0x10400, 0x10427, 1}, {0x104b0, 0x104d3, 1}, {0x10c80, 0x10cb2, 1}, {0x118a0, 0x118bf, 1},
    {0x16e40, 0x16e5f, 1}, {0x1d400, 0x1d419, 1}, {0x1d434, 0x1d44d, 1}, {0x1d468, 0x1d481, 1},
    {0x1d49c, 0x1d49e, 2}, {0x1d49f, 0x1d4a5, 3}, {0x1d4a6, 0x1d4a9, 3}, {0x1d4aa, 0x1d4ac, 1},
    {0x1d4ae, 0x1d4b5, 1}, {0x1d4d0, 0x1d4e9, 1}, {0x1d504, 0x1d505, 1}, {0x1d507, 0x1d50a, 1},
    {0x1d50d, 0x1d514, 1}, {0x1d516, 0x1d51c, 1}, {0x1d538, 0x1d539, 1}, {0x1d53b, 0x1d53e, 1},
    {0x1d540, 0x1d544, 1}, {0x1d546, 0x1d54a, 4}, {0x1d54b, 0x1d550, 1}, {0x1d56c, 0x1d585, 1},
    {0x1d5a0, 0x1d5b9, 1}, {0x1d5d4, 0x1d5ed, 1}, {0x1d608, 0x1d621, 1}, {0x1d63c, 0x1d655, 1},
    {0x1d670, 0x1d689, 1}, {0x1d6a8, 0x1d6c0, 1}, {0x1d6e2, 0x1d6fa, 1}, {0x1d71c, 0x1d734, 1},
    {0x1d756, 0x1d76e, 1}, {0x1d790, 0x1d7a8, 1}, {0x1d7ca, 0x1e900, 4406}, {0x1e901, 0x1e921, 1},
};
static P4_RuneRange _N[] = {
    {0x0030, 0x0039, 1}, {0x00b2, 0x00b3, 1}, {0x00b9, 0x00bc, 3}, {0x00bd, 0x00be, 1},
    {0x0660, 0x0669, 1}, {0x06f0, 0x06f9, 1}, {0x07c0, 0x07c9, 1}, {0x0966, 0x096f, 1},
    {0x09e6, 0x09ef, 1}, {0x09f4, 0x09f9, 1}, {0x0a66, 0x0a6f, 1}, {0x0ae6, 0x0aef, 1},
    {0x0b66, 0x0b6f, 1}, {0x0b72, 0x0b77, 1}, {0x0be6, 0x0bf2, 1}, {0x0c66, 0x0c6f, 1},
    {0x0c78, 0x0c7e, 1}, {0x0ce6, 0x0cef, 1}, {0x0d58, 0x0d5e, 1}, {0x0d66, 0x0d78, 1},
    {0x0de6, 0x0def, 1}, {0x0e50, 0x0e59, 1}, {0x0ed0, 0x0ed9, 1}, {0x0f20, 0x0f33, 1},
    {0x1040, 0x1049, 1}, {0x1090, 0x1099, 1}, {0x1369, 0x137c, 1}, {0x16ee, 0x16f0, 1},
    {0x17e0, 0x17e9, 1}, {0x17f0, 0x17f9, 1}, {0x1810, 0x1819, 1}, {0x1946, 0x194f, 1},
    {0x19d0, 0x19da, 1}, {0x1a80, 0x1a89, 1}, {0x1a90, 0x1a99, 1}, {0x1b50, 0x1b59, 1},
    {0x1bb0, 0x1bb9, 1}, {0x1c40, 0x1c49, 1}, {0x1c50, 0x1c59, 1}, {0x2070, 0x2074, 4},
    {0x2075, 0x2079, 1}, {0x2080, 0x2089, 1}, {0x2150, 0x2182, 1}, {0x2185, 0x2189, 1},
    {0x2460, 0x249b, 1}, {0x24ea, 0x24ff, 1}, {0x2776, 0x2793, 1}, {0x2cfd, 0x3007, 778},
    {0x3021, 0x3029, 1}, {0x3038, 0x303a, 1}, {0x3192, 0x3195, 1}, {0x3220, 0x3229, 1},
    {0x3248, 0x324f, 1}, {0x3251, 0x325f, 1}, {0x3280, 0x3289, 1}, {0x32b1, 0x32bf, 1},
    {0xa620, 0xa629, 1}, {0xa6e6, 0xa6ef, 1}, {0xa830, 0xa835, 1}, {0xa8d0, 0xa8d9, 1},
    {0xa900, 0xa909, 1}, {0xa9d0, 0xa9d9, 1}, {0xa9f0, 0xa9f9, 1}, {0xaa50, 0xaa59, 1},
    {0xabf0, 0xabf9, 1}, {0xff10, 0xff19, 1},
    {0x10107, 0x10133, 1}, {0x10140, 0x10178, 1}, {0x1018a, 0x1018b, 1}, {0x102e1, 0x102fb, 1},
    {0x10320, 0x10323, 1}, {0x10341, 0x1034a, 9}, {0x103d1, 0x103d5, 1}, {0x104a0, 0x104a9, 1},
    {0x10858, 0x1085f, 1}, {0x10879, 0x1087f, 1}, {0x108a7, 0x108af, 1}, {0x108fb, 0x108ff, 1},
    {0x10916, 0x1091b, 1}, {0x109bc, 0x109bd, 1}, {0x109c0, 0x109cf, 1}, {0x109d2, 0x109ff, 1},
    {0x10a40, 0x10a48, 1}, {0x10a7d, 0x10a7e, 1}, {0x10a9d, 0x10a9f, 1}, {0x10aeb, 0x10aef, 1},
    {0x10b58, 0x10b5f, 1}, {0x10b78, 0x10b7f, 1}, {0x10ba9, 0x10baf, 1}, {0x10cfa, 0x10cff, 1},
    {0x10d30, 0x10d39, 1}, {0x10e60, 0x10e7e, 1}, {0x10f1d, 0x10f26, 1}, {0x10f51, 0x10f54, 1},
    {0x10fc5, 0x10fcb, 1}, {0x11052, 0x1106f, 1}, {0x110f0, 0x110f9, 1}, {0x11136, 0x1113f, 1},
    {0x111d0, 0x111d9, 1}, {0x111e1, 0x111f4, 1}, {0x112f0, 0x112f9, 1}, {0x11450, 0x11459, 1},
    {0x114d0, 0x114d9, 1}, {0x11650, 0x11659, 1}, {0x116c0, 0x116c9, 1}, {0x11730, 0x1173b, 1},
    {0x118e0, 0x118f2, 1}, {0x11950, 0x11959, 1}, {0x11c50, 0x11c6c, 1}, {0x11d50, 0x11d59, 1},
    {0x11da0, 0x11da9, 1}, {0x11fc0, 0x11fd4, 1}, {0x12400, 0x1246e, 1}, {0x16a60, 0x16a69, 1},
    {0x16b50, 0x16b59, 1}, {0x16b5b, 0x16b61, 1}, {0x16e80, 0x16e96, 1}, {0x1d2e0, 0x1d2f3, 1},
    {0x1d360, 0x1d378, 1}, {0x1d7ce, 0x1d7ff, 1}, {0x1e140, 0x1e149, 1}, {0x1e2f0, 0x1e2f9, 1},
    {0x1e8c7, 0x1e8cf, 1}, {0x1e950, 0x1e959, 1}, {0x1ec71, 0x1ecab, 1}, {0x1ecad, 0x1ecaf, 1},
    {0x1ecb1, 0x1ecb4, 1}, {0x1ed01, 0x1ed2d, 1}, {0x1ed2f, 0x1ed3d, 1}, {0x1f100, 0x1f10c, 1},
    {0x1fbf0, 0x1fbf9, 1},
};
static P4_RuneRange _Nl[] = {
    {0x16ee, 0x16f0, 1}, {0x2160, 0x2182, 1}, {0x2185, 0x2188, 1}, {0x3007, 0x3021, 26},
    {0x3022, 0x3029, 1}, {0x3038, 0x303a, 1}, {0xa6e6, 0xa6ef, 1},
    {0x10140, 0x10174, 1}, {0x10341, 0x1034a, 9}, {0x103d1, 0x103d5, 1}, {0x12400, 0x1246e, 1},
};
static P4_RuneRange _No[] = {
    {0x00b2, 0x00b3, 1}, {0x00b9, 0x00bc, 3}, {0x00bd, 0x00be, 1}, {0x09f4, 0x09f9, 1},
    {0x0b72, 0x0b77, 1}, {0x0bf0, 0x0bf2, 1}, {0x0c78, 0x0c7e, 1}, {0x0d58, 0x0d5e, 1},
    {0x0d70, 0x0d78, 1}, {0x0f2a, 0x0f33, 1}, {0x1369, 0x137c, 1}, {0x17f0, 0x17f9, 1},
    {0x19da, 0x2070, 1686}, {0x2074, 0x2079, 1}, {0x2080, 0x2089, 1}, {0x2150, 0x215f, 1},
    {0x2189, 0x2460, 727}, {0x2461, 0x249b, 1}, {0x24ea, 0x24ff, 1}, {0x2776, 0x2793, 1},
    {0x2cfd, 0x3192, 1173}, {0x3193, 0x3195, 1}, {0x3220, 0x3229, 1}, {0x3248, 0x324f, 1},
    {0x3251, 0x325f, 1}, {0x3280, 0x3289, 1}, {0x32b1, 0x32bf, 1}, {0xa830, 0xa835, 1},
    {0x10107, 0x10133, 1}, {0x10175, 0x10178, 1}, {0x1018a, 0x1018b, 1}, {0x102e1, 0x102fb, 1},
    {0x10320, 0x10323, 1}, {0x10858, 0x1085f, 1}, {0x10879, 0x1087f, 1}, {0x108a7, 0x108af, 1},
    {0x108fb, 0x108ff, 1}, {0x10916, 0x1091b, 1}, {0x109bc, 0x109bd, 1}, {0x109c0, 0x109cf, 1},
    {0x109d2, 0x109ff, 1}, {0x10a40, 0x10a48, 1}, {0x10a7d, 0x10a7e, 1}, {0x10a9d, 0x10a9f, 1},
    {0x10aeb, 0x10aef, 1}, {0x10b58, 0x10b5f, 1}, {0x10b78, 0x10b7f, 1}, {0x10ba9, 0x10baf, 1},
    {0x10cfa, 0x10cff, 1}, {0x10e60, 0x10e7e, 1}, {0x10f1d, 0x10f26, 1}, {0x10f51, 0x10f54, 1},
    {0x10fc5, 0x10fcb, 1}, {0x11052, 0x11065, 1}, {0x111e1, 0x111f4, 1}, {0x1173a, 0x1173b, 1},
    {0x118ea, 0x118f2, 1}, {0x11c5a, 0x11c6c, 1}, {0x11fc0, 0x11fd4, 1}, {0x16b5b, 0x16b61, 1},
    {0x16e80, 0x16e96, 1}, {0x1d2e0, 0x1d2f3, 1}, {0x1d360, 0x1d378, 1}, {0x1e8c7, 0x1e8cf, 1},
    {0x1ec71, 0x1ecab, 1}, {0x1ecad, 0x1ecaf, 1}, {0x1ecb1, 0x1ecb4, 1}, {0x1ed01, 0x1ed2d, 1},
    {0x1ed2f, 0x1ed3d, 1}, {0x1f100, 0x1f10c, 1},
};
static P4_RuneRange _Nd[] = {
    {0x0030, 0x0039, 1}, {0x0660, 0x0669, 1}, {0x06f0, 0x06f9, 1}, {0x07c0, 0x07c9, 1},
    {0x0966, 0x096f, 1}, {0x09e6, 0x09ef, 1}, {0x0a66, 0x0a6f, 1}, {0x0ae6, 0x0aef, 1},
    {0x0b66, 0x0b6f, 1}, {0x0be6, 0x0bef, 1}, {0x0c66, 0x0c6f, 1}, {0x0ce6, 0x0cef, 1},
    {0x0d66, 0x0d6f, 1}, {0x0de6, 0x0def, 1}, {0x0e50, 0x0e59, 1}, {0x0ed0, 0x0ed9, 1},
    {0x0f20, 0x0f29, 1}, {0x1040, 0x1049, 1}, {0x1090, 0x1099, 1}, {0x17e0, 0x17e9, 1},
    {0x1810, 0x1819, 1}, {0x1946, 0x194f, 1}, {0x19d0, 0x19d9, 1}, {0x1a80, 0x1a89, 1},
    {0x1a90, 0x1a99, 1}, {0x1b50, 0x1b59, 1}, {0x1bb0, 0x1bb9, 1}, {0x1c40, 0x1c49, 1},
    {0x1c50, 0x1c59, 1}, {0xa620, 0xa629, 1}, {0xa8d0, 0xa8d9, 1}, {0xa900, 0xa909, 1},
    {0xa9d0, 0xa9d9, 1}, {0xa9f0, 0xa9f9, 1}, {0xaa50, 0xaa59, 1}, {0xabf0, 0xabf9, 1},
    {0xff10, 0xff19, 1},
    {0x104a0, 0x104a9, 1}, {0x10d30, 0x10d39, 1}, {0x11066, 0x1106f, 1}, {0x110f0, 0x110f9, 1},
    {0x11136, 0x1113f, 1}, {0x111d0, 0x111d9, 1}, {0x112f0, 0x112f9, 1}, {0x11450, 0x11459, 1},
    {0x114d0, 0x114d9, 1}, {0x11650, 0x11659, 1}, {0x116c0, 0x116c9, 1}, {0x11730, 0x11739, 1},
    {0x118e0, 0x118e9, 1}, {0x11950, 0x11959, 1}, {0x11c50, 0x11c59, 1}, {0x11d50, 0x11d59, 1},
    {0x11da0, 0x11da9, 1}, {0x16a60, 0x16a69, 1}, {0x16b50, 0x16b59, 1}, {0x1d7ce, 0x1d7ff, 1},
    {0x1e140, 0x1e149, 1}, {0x1e2f0, 0x1e2f9, 1}, {0x1e950, 0x1e959, 1}, {0x1fbf0, 0x1fbf9, 1},
};
static P4_RuneRange _Z[] = {
    {0x0020, 0x00a0, 128}, {0x1680, 0x2000, 2432}, {0x2001, 0x200a, 1}, {0x2028, 0x2029, 1},
    {0x202f, 0x205f, 48}, {0x3000, 0x3000, 1},
};
static P4_RuneRange _Zl[] = {
    {0x2028, 0x2028, 1},
};
static P4_RuneRange _Zp[] = {
    {0x2029, 0x2029, 1},
};
static P4_RuneRange _Zs[] = {
    {0x0020, 0x00a0, 128}, {0x1680, 0x2000, 2432}, {0x2001, 0x200a, 1}, {0x202f, 0x205f, 48},
    {0x3000, 0x3000, 1},
};


size_t P4_GetRuneSize(P4_Rune ch) {
    if (0 == ((P4_Rune)0xffffff80 & ch)) {
        return 1;
    } else if (0 == ((P4_Rune)0xfffff800 & ch)) {
        return 2;
    } else if (0 == ((P4_Rune)0xffff0000 & ch)) {
        return 3;
    } else { /* e.g.  0 == ((int)0xffe00000 & chr)) */
        return 4;
    }
}

/**
 *
 * @brief   Reads a single UTF-8 code point from the string.
 * @param   s   The string in UTF-8 encoding.
 * @param   c   The pointer to the rune to read.
 * @return  The size of rune, could be 1,2,3,4. When error occurs, read nothing and returning size is 0.
 *
 * Example::
 *
 *     > uint32_t c = 0x0
 *     > P4_ReadRune("你好", &c)
 *     3
 *     > printf("%p %d\n", c, c)
 *     0x4f60 20320
 */
size_t
P4_ReadRune(P4_String s, P4_Rune* c) {
    *c = 0;

    if ((s[0] & 0b10000000) == 0) { /* 1 byte code point, ASCII */
        *c = (s[0] & 0b01111111);
        return 1;
    } else if ((s[0] & 0b11100000) == 0b11000000) { /* 2 byte code point */
        *c = (s[0] & 0b00011111) << 6 | (s[1] & 0b00111111);
        return 2;
    } else if ((s[0] & 0b11110000) == 0b11100000) { /* 3 byte code point */
        *c = (s[0] & 0b00001111) << 12 | (s[1] & 0b00111111) << 6 | (s[2] & 0b00111111);
        return 3;
    } else if ((s[0] & 0b11111000) == 0b11110000) { /* 4 byte code point */
        *c = (s[0] & 0b00000111) << 18 | (s[1] & 0b00111111) << 12 | (s[2] & 0b00111111) << 6 | (s[3] & 0b00111111);
        return 4;
    } else {
        *c = 0x0;
        return 0;
    }
}

void* P4_ConcatRune(void *str, P4_Rune chr, size_t n) {
  char *s = (char *)str;

  if (0 == ((P4_Rune)0xffffff80 & chr)) {
    /* 1-byte/7-bit ascii (0b0xxxxxxx) */
    if (n < 1) {
      return 0;
    }
    s[0] = (char)chr;
    s += 1;
  } else if (0 == ((P4_Rune)0xfffff800 & chr)) {
    /* 2-byte/11-bit utf8 code point (0b110xxxxx 0b10xxxxxx) */
    if (n < 2) {
      return 0;
    }
    s[0] = (char) 0xc0 | (char)((chr >> 6) & 0x1f);
    s[1] = (char) 0x80 | (char)(chr & 0x3f);
    s += 2;
  } else if (0 == ((P4_Rune)0xffff0000 & chr)) {
    /* 3-byte/16-bit utf8 code point (0b1110xxxx 0b10xxxxxx 0b10xxxxxx) */
    if (n < 3) {
      return 0;
    }
    s[0] = (char) 0xe0 | (char)((chr >> 12) & 0x0f);
    s[1] = (char) 0x80 | (char)((chr >> 6) & 0x3f);
    s[2] = (char) 0x80 | (char)(chr & 0x3f);
    s += 3;
  } else { /* if (0 == ((int)0xffe00000 & chr)) { */
    /* 4-byte/21-bit utf8 code point (0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx) */
    if (n < 4) {
      return 0;
    }
    s[0] = (char) 0xf0 | (char)((chr >> 18) & 0x07);
    s[1] = (char) 0x80 | (char)((chr >> 12) & 0x3f);
    s[2] = (char) 0x80 | (char)((chr >> 6) & 0x3f);
    s[3] = (char) 0x80 | (char)(chr & 0x3f);
    s += 4;
  }

  return s;
}

size_t P4_ReadEscapedRune(char* text, P4_Rune* rune) {
    char ch0 = *text;

    if (ch0 == '\0') {
        *rune = 0;
        return 0;
    }

    if (ch0 != '\\')
        return P4_ReadRune(text, rune);

    char ch1 = text[1];

    switch (ch1) {
        case 'b': *rune = 0x8; return 2;
        case 't': *rune = 0x9; return 2;
        case 'n': *rune = 0xa; return 2;
        case 'f': *rune = 0xc; return 2;
        case 'r': *rune = 0xd; return 2;
        case '"': *rune = 0x22; return 2;
        case '/': *rune = 0x2f; return 2;
        case '\\': *rune = 0x5c; return 2;
        case 'x': { /* TODO: may not have enough chars. */
            char chs[3] = {0, 0, 0};
            memcpy(chs, text + 2, 2);
            *rune = strtoul(chs, NULL, 16);
            return 4;
        }
        case 'u': { /* TODO: may not have enough chars. */
            char chs[5] = {0, 0, 0, 0, 0};
            memcpy(chs, text + 2, 4);
            *rune = strtoul(chs, NULL, 16);
            return 6;
        }
        case 'U': { /* TODO: may not have enough chars. */
            char chs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
            memcpy(chs, text + 2, 8);
            *rune = strtoul(chs, NULL, 16);
            return 10;
        }
        default: UNREACHABLE(); return 0;
    }
}

size_t P4_ReadRuneRange(char* text, P4_Slice* slice, size_t* count, P4_RuneRange** ranges) {
    size_t len = get_slice_size(slice);

# define READ_RUNE_RANGE(p,l) \
    if (len == (l) && memcmp(text+slice->start.pos, #p , len) == 0) {\
        *ranges = _ ## p; \
        *count = sizeof(_ ## p) / sizeof(P4_RuneRange); \
        return (l); \
    }

    READ_RUNE_RANGE(Cc, 2);
    READ_RUNE_RANGE(Cf, 2);
    READ_RUNE_RANGE(Co, 2);
    READ_RUNE_RANGE(Cs, 2);
    READ_RUNE_RANGE(C, 1);
    READ_RUNE_RANGE(Ll, 2);
    READ_RUNE_RANGE(Lm, 2);
    READ_RUNE_RANGE(Lo, 2);
    READ_RUNE_RANGE(Lt, 2);
    READ_RUNE_RANGE(Lu, 2);
    READ_RUNE_RANGE(L, 1);
    READ_RUNE_RANGE(Nd, 2);
    READ_RUNE_RANGE(Nl, 2);
    READ_RUNE_RANGE(No, 2);
    READ_RUNE_RANGE(N, 1);
    READ_RUNE_RANGE(Zl, 2);
    READ_RUNE_RANGE(Zp, 2);
    READ_RUNE_RANGE(Zs, 2);
    READ_RUNE_RANGE(Z, 1);

    *count = 0;
    return 0;
}

/**
 * @brief   Get rune in lower case.
 *
 * Modified from https://github.com/sheredom/utf8.h
 */
P4_PRIVATE(P4_Rune)
P4_GetRuneLower(P4_Rune cp) {
    if (((0x0041 <= cp) && (0x005a >= cp)) ||
        ((0x00c0 <= cp) && (0x00d6 >= cp)) ||
        ((0x00d8 <= cp) && (0x00de >= cp)) ||
        ((0x0391 <= cp) && (0x03a1 >= cp)) ||
        ((0x03a3 <= cp) && (0x03ab >= cp)) ||
        ((0x0410 <= cp) && (0x042f >= cp))) {
        cp += 32;
    } else if ((0x0400 <= cp) && (0x040f >= cp)) {
        cp += 80;
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
                ((0x0132 <= cp) && (0x0137 >= cp)) ||
                ((0x014a <= cp) && (0x0177 >= cp)) ||
                ((0x0182 <= cp) && (0x0185 >= cp)) ||
                ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
                ((0x01de <= cp) && (0x01ef >= cp)) ||
                ((0x01f8 <= cp) && (0x021f >= cp)) ||
                ((0x0222 <= cp) && (0x0233 >= cp)) ||
                ((0x0246 <= cp) && (0x024f >= cp)) ||
                ((0x03d8 <= cp) && (0x03ef >= cp)) ||
                ((0x0460 <= cp) && (0x0481 >= cp)) ||
                ((0x048a <= cp) && (0x04ff >= cp))) {
        cp |= 0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
                ((0x0179 <= cp) && (0x017e >= cp)) ||
                ((0x01af <= cp) && (0x01b0 >= cp)) ||
                ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
                ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp += 1;
        cp &= ~0x1;
    } else {
        switch (cp) {
            case 0x0178: cp = 0x00ff; break;
            case 0x0243: cp = 0x0180; break;
            case 0x018e: cp = 0x01dd; break;
            case 0x023d: cp = 0x019a; break;
            case 0x0220: cp = 0x019e; break;
            case 0x01b7: cp = 0x0292; break;
            case 0x01c4: cp = 0x01c6; break;
            case 0x01c7: cp = 0x01c9; break;
            case 0x01ca: cp = 0x01cc; break;
            case 0x01f1: cp = 0x01f3; break;
            case 0x01f7: cp = 0x01bf; break;
            case 0x0187: cp = 0x0188; break;
            case 0x018b: cp = 0x018c; break;
            case 0x0191: cp = 0x0192; break;
            case 0x0198: cp = 0x0199; break;
            case 0x01a7: cp = 0x01a8; break;
            case 0x01ac: cp = 0x01ad; break;
            case 0x01af: cp = 0x01b0; break;
            case 0x01b8: cp = 0x01b9; break;
            case 0x01bc: cp = 0x01bd; break;
            case 0x01f4: cp = 0x01f5; break;
            case 0x023b: cp = 0x023c; break;
            case 0x0241: cp = 0x0242; break;
            case 0x03fd: cp = 0x037b; break;
            case 0x03fe: cp = 0x037c; break;
            case 0x03ff: cp = 0x037d; break;
            case 0x037f: cp = 0x03f3; break;
            case 0x0386: cp = 0x03ac; break;
            case 0x0388: cp = 0x03ad; break;
            case 0x0389: cp = 0x03ae; break;
            case 0x038a: cp = 0x03af; break;
            case 0x038c: cp = 0x03cc; break;
            case 0x038e: cp = 0x03cd; break;
            case 0x038f: cp = 0x03ce; break;
            case 0x0370: cp = 0x0371; break;
            case 0x0372: cp = 0x0373; break;
            case 0x0376: cp = 0x0377; break;
            case 0x03f4: cp = 0x03b8; break;
            case 0x03cf: cp = 0x03d7; break;
            case 0x03f9: cp = 0x03f2; break;
            case 0x03f7: cp = 0x03f8; break;
            case 0x03fa: cp = 0x03fb; break;
            default: break;
        }
    }

    return cp;
}

/**
 * @brief   Get rune in upper case.
 *
 * Modified from https://github.com/sheredom/utf8.h
 */
P4_PRIVATE(P4_Rune)
P4_GetRuneUpper(P4_Rune cp) {
    if (((0x0061 <= cp) && (0x007a >= cp)) ||
        ((0x00e0 <= cp) && (0x00f6 >= cp)) ||
        ((0x00f8 <= cp) && (0x00fe >= cp)) ||
        ((0x03b1 <= cp) && (0x03c1 >= cp)) ||
        ((0x03c3 <= cp) && (0x03cb >= cp)) ||
        ((0x0430 <= cp) && (0x044f >= cp))) {
        cp -= 32;
    } else if ((0x0450 <= cp) && (0x045f >= cp)) {
        cp -= 80;
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
                ((0x0132 <= cp) && (0x0137 >= cp)) ||
                ((0x014a <= cp) && (0x0177 >= cp)) ||
                ((0x0182 <= cp) && (0x0185 >= cp)) ||
                ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
                ((0x01de <= cp) && (0x01ef >= cp)) ||
                ((0x01f8 <= cp) && (0x021f >= cp)) ||
                ((0x0222 <= cp) && (0x0233 >= cp)) ||
                ((0x0246 <= cp) && (0x024f >= cp)) ||
                ((0x03d8 <= cp) && (0x03ef >= cp)) ||
                ((0x0460 <= cp) && (0x0481 >= cp)) ||
                ((0x048a <= cp) && (0x04ff >= cp))) {
        cp &= ~0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
                ((0x0179 <= cp) && (0x017e >= cp)) ||
                ((0x01af <= cp) && (0x01b0 >= cp)) ||
                ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
                ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp -= 1;
        cp |= 0x1;
    } else {
        switch (cp) {
            case 0x00ff: cp = 0x0178; break;
            case 0x0180: cp = 0x0243; break;
            case 0x01dd: cp = 0x018e; break;
            case 0x019a: cp = 0x023d; break;
            case 0x019e: cp = 0x0220; break;
            case 0x0292: cp = 0x01b7; break;
            case 0x01c6: cp = 0x01c4; break;
            case 0x01c9: cp = 0x01c7; break;
            case 0x01cc: cp = 0x01ca; break;
            case 0x01f3: cp = 0x01f1; break;
            case 0x01bf: cp = 0x01f7; break;
            case 0x0188: cp = 0x0187; break;
            case 0x018c: cp = 0x018b; break;
            case 0x0192: cp = 0x0191; break;
            case 0x0199: cp = 0x0198; break;
            case 0x01a8: cp = 0x01a7; break;
            case 0x01ad: cp = 0x01ac; break;
            case 0x01b0: cp = 0x01af; break;
            case 0x01b9: cp = 0x01b8; break;
            case 0x01bd: cp = 0x01bc; break;
            case 0x01f5: cp = 0x01f4; break;
            case 0x023c: cp = 0x023b; break;
            case 0x0242: cp = 0x0241; break;
            case 0x037b: cp = 0x03fd; break;
            case 0x037c: cp = 0x03fe; break;
            case 0x037d: cp = 0x03ff; break;
            case 0x03f3: cp = 0x037f; break;
            case 0x03ac: cp = 0x0386; break;
            case 0x03ad: cp = 0x0388; break;
            case 0x03ae: cp = 0x0389; break;
            case 0x03af: cp = 0x038a; break;
            case 0x03cc: cp = 0x038c; break;
            case 0x03cd: cp = 0x038e; break;
            case 0x03ce: cp = 0x038f; break;
            case 0x0371: cp = 0x0370; break;
            case 0x0373: cp = 0x0372; break;
            case 0x0377: cp = 0x0376; break;
            case 0x03d1: cp = 0x0398; break;
            case 0x03d7: cp = 0x03cf; break;
            case 0x03f2: cp = 0x03f9; break;
            case 0x03f8: cp = 0x03f7; break;
            case 0x03fb: cp = 0x03fa; break;
            default: break;
        }
    }

    return cp;
}

/*
 * Compare case-insensitive string src1 v/s src2.
 *
 * Like strcmp, but works for a case insensitive UTF-8 string.
 * Modified from https://github.com/sheredom/utf8.h
 */
P4_PRIVATE(int)
P4_CaseCmpInsensitive(const void* src1, const void* src2, size_t n) {
    P4_Rune src1_lwr_cp, src2_lwr_cp, src1_upr_cp, src2_upr_cp, src1_orig_cp, src2_orig_cp;

    do {
        const unsigned char *const s1 = (const unsigned char *)src1;
        const unsigned char *const s2 = (const unsigned char *)src2;

        /* check bytes left in n. */
        if (0 == n) return 0;

        if ((1 == n) && ((0xc0 == (0xe0 & *s1)) || (0xc0 == (0xe0 & *s2)))) {
            const P4_Rune c1 = (0xe0 & *s1);
            const P4_Rune c2 = (0xe0 & *s2);
            if (c1 < c2) return c1 - c2;
            else return 0;
        }

        if ((2 >= n) && ((0xe0 == (0xf0 & *s1)) || (0xe0 == (0xf0 & *s2)))) {
            const P4_Rune c1 = (0xf0 & *s1);
            const P4_Rune c2 = (0xf0 & *s2);

            if (c1 < c2) return (int) c1 - (int) c2;
            else return 0;
        }

        if ((3 >= n) && ((0xf0 == (0xf8 & *s1)) || (0xf0 == (0xf8 & *s2)))) {
            const P4_Rune c1 = (0xf8 & *s1);
            const P4_Rune c2 = (0xf8 & *s2);
            if (c1 < c2) return (int) c1 - (int) c2;
            else return 0;
        }

        src1 = src1 + P4_ReadRune((P4_String)src1, &src1_orig_cp);
        src2 = src2 + P4_ReadRune((P4_String)src2, &src2_orig_cp);
        n -= P4_GetRuneSize(src1_orig_cp);

        src1_lwr_cp = P4_GetRuneLower(src1_orig_cp);
        src2_lwr_cp = P4_GetRuneLower(src2_orig_cp);

        src1_upr_cp = P4_GetRuneUpper(src1_orig_cp);
        src2_upr_cp = P4_GetRuneUpper(src2_orig_cp);

        /* check if the lowered codepoints match */
        if ((0 == src1_orig_cp) && (0 == src2_orig_cp))
            return 0;
        else if ((src1_lwr_cp == src2_lwr_cp) || (src1_upr_cp == src2_upr_cp))
            continue;

        /* if they don't match, then we return the difference between the characters */
        return (int) src1_lwr_cp - (int) src2_lwr_cp;
    } while ( 0 < n );

    return 0; /* match! */
}

/*
 * Determine if the corresponding node to `e` should be ignored.
 *
 * 1. Intermediate expr.
 * 2. Bareness expr.
 * 3. Hollowed expr.
 *
 */
P4_PRIVATE(bool)
P4_NeedLift(P4_Source* s, P4_Expression* e) {
    return !is_rule(e) || is_lifted(e) || need_silent(s);
}

/*
 * Clear an error.
 *
 * It allows the parser to keep parsing the text.
 */
P4_PRIVATE(void)
P4_RescueError(P4_Source* s) {
    s->err = P4_Ok;
    memset(s->errmsg, 0, sizeof(s->errmsg));
}


/*
 * Initialize a node.
 */
P4_Node*
P4_CreateNode (const P4_String     str,
               P4_Position*        start,
               P4_Position*        stop,
               P4_String           rule_name) {
    P4_Node* node;

    if ((node=P4_MALLOC(sizeof(P4_Node))) == NULL)
        return NULL;

    node->text         = str;
    node->rule_name    = rule_name;
    node->next         = NULL;
    node->head         = NULL;
    node->tail         = NULL;

    set_slice(&node->slice, start, stop);

    return node;
}


/*
 * Free the node.
 *
 * DANGER: this function does not free children nodes.
 */
P4_PRIVATE(void)
P4_DeleteNodeNode(P4_Node* node) {
    if (node) P4_FREE(node);
}


/*
 * Free all of the children nodes of the node.
 */
P4_PRIVATE(void)
P4_DeleteNodeChildren(P4_Node* node) {
    if (node == NULL)
        return;

    P4_Node*   child   = node->head;
    P4_Node*   tmp     = NULL;

    while (child) {
        tmp = child->next;
        if (child->head)
            P4_DeleteNodeChildren(child);
        P4_DeleteNodeNode(child);
        child = tmp;
    }
}


/*
 * Free the node list and all the children nodes of each
 * node in the node list.
 */
P4_PUBLIC void
P4_DeleteNode(P4_Node* node) {
    P4_Node* tmp = NULL;
    while (node) {
        tmp     = node->next;
        P4_DeleteNodeChildren(node);
        P4_DeleteNodeNode(node);
        node   = tmp;
    }
}

P4_PRIVATE(void)
P4_DeleteNodeUserData(P4_Grammar* grammar, P4_Node* node) {
    if (grammar->free_func == NULL)
        return;

    P4_Node* tmp = node;
    while (tmp != NULL) {
        if (tmp->userdata != NULL)
            grammar->free_func(tmp->userdata);
        P4_DeleteNodeUserData(grammar, tmp->head);
        tmp = tmp->next;
    }
}

/*
 * Push e into s->frame_stack.
 */
P4_PRIVATE(P4_Error)
P4_PushFrame(P4_Source* s, P4_Expression* e) {
    if ((s->frame_stack_size) >= (s->grammar->depth)) {
        return P4_StackError;
    }

    P4_Frame* frame = NULL;
    catch_oom(frame = P4_MALLOC(sizeof(P4_Frame)));

    /* Set silent */
    frame->silent = false;
    P4_Frame* top = s->frame_stack;
    if (!is_scoped(e)) {
        if (
            (top && is_squashed(top->rule))
            || (top && top->silent)
        )
            frame->silent = true;
    }

    /* Set space */
    frame->space = false;
    if (P4_GetWhitespaces(s->grammar) != NULL
            && !s->whitespacing) {
        if (is_scoped(e)) {
            frame->space = true;
        } else if (top) {
            if (!is_tight(e)) {
                frame->space = top->space;
            }
        } else if (!is_tight(e)) {
            frame->space = true;
        }
    }

    /* Set expr & next */
    frame->expr = e;
    frame->next = top;

    /* Set cut */
    frame->cut = false;

    /* Set rule */
    frame->rule = is_rule(e) ? e : top->rule;

    /* Push stack */
    s->frame_stack_size++;
    s->frame_stack = frame;

    return P4_Ok;
}


/*
 * Pop top from s->frames.
 */
P4_PRIVATE(P4_Error)
P4_PopFrame(P4_Source* s, P4_Frame* f) {
    assert(s->frame_stack != NULL, "frame should not be empty");

    P4_Frame* oldtop = s->frame_stack;

    if (oldtop) {
        s->frame_stack = oldtop->next;
        s->frame_stack_size--;
        P4_FREE(oldtop);
    }

    return P4_Ok;
}


P4_PRIVATE(P4_Node*)
P4_MatchLiteral(P4_Source* s, P4_Expression* e) {
    assert(no_error(s), "can't proceed due to a failed match");

    mark_position(s, startpos);

    P4_String str = P4_RemainingText(s);
    P4_Rune rune[2] = {0};
    P4_ReadRune(e->literal, rune);

    if (is_end(s)) {
        P4_MatchRaisef(s, P4_MatchError, "expect %s (char '%s')",
                P4_PeekFrame(s)->rule->name, (char*)rune);
        return NULL;
    }

    size_t len = strlen(e->literal);
    size_t slen = strlen(str);
    if (slen < len) {
        P4_MatchRaisef(s, P4_MatchError, "expect %s (len %zu)",
                s->frame_stack->rule->name, len);
        return NULL;
    }

    if ((!e->sensitive && P4_CaseCmpInsensitive(e->literal, str, len) != 0)
            || (e->sensitive && memcmp(e->literal, str, len) != 0)) {
        P4_MatchRaisef(s, P4_MatchError, "expect %s (char '%s')",
                P4_PeekFrame(s)->rule->name, (char*)rune);
        return NULL;
    }

    P4_Position* endpos= &(P4_Position){ 0 };
    P4_DiffPosition(s->content, startpos, len, endpos);
    set_position(s, endpos);

    if (P4_NeedLift(s, e))
        return NULL;

    P4_Node*  result = NULL;
    catch_oom(result = P4_CreateNode(s->content, startpos, endpos, e->name));

    return result;
}

P4_PRIVATE(P4_Node*)
P4_MatchRange(P4_Source* s, P4_Expression* e) {
    assert(no_error(s), "can't proceed due to a failed match");

    P4_String str = P4_RemainingText(s);
    if (is_end(s)) {
        P4_MatchRaisef(s, P4_MatchError, "expect %s", P4_PeekFrame(s)->rule->name);
        return NULL;
    }

    mark_position(s, startpos);

    uint32_t rune = 0x0;
    size_t size = P4_ReadRune(str, &rune);
    size_t i = 0;
    bool found = false;

    for (i = 0; i < e->ranges_count; i++) {
        if (rune < e->ranges[i].lower
                || rune > e->ranges[i].upper
                || (rune - e->ranges[i].lower) % e->ranges[i].stride != 0) {
            continue;
        } else
            found = true;
    }

    if (!found) {
        P4_MatchRaisef(s, P4_MatchError, "expect %s", P4_PeekFrame(s)->rule->name);
        return NULL;
    }

    P4_Position* endpos= &(P4_Position){ 0 };
    P4_DiffPosition(s->content, startpos, size, endpos);
    set_position(s, endpos);

    if (P4_NeedLift(s, e))
        return NULL;

    P4_Node*  result = NULL;
    catch_oom(result = P4_CreateNode(s->content, startpos, endpos, e->name));

    return result;
}

P4_PRIVATE(P4_Expression*)
P4_GetReference(P4_Source* s, P4_Expression* e) {
    if (e->ref_expr != NULL)
        return e->ref_expr;

    if (e->reference != NULL) {
        e->ref_expr = P4_GetGrammarRule(s->grammar, e->reference);
    }

    return e->ref_expr;
}

P4_PRIVATE(P4_Node*)
P4_MatchReference(P4_Source* s, P4_Expression* e) {
    assert(no_error(s), "can't proceed due to a failed match");

    if (e->ref_expr == NULL && e->reference != NULL) {
        e->ref_expr = P4_GetGrammarRule(s->grammar, e->reference);
    }

    if (e->ref_expr == NULL) {
        P4_MatchRaisef(s, P4_NameError, "expect %s", e->reference);
        return NULL;
    }

    mark_position(s, startpos);
    P4_Node* reftok = P4_Match(s, e->ref_expr);
    mark_position(s, endpos);

    /* Ref matching is terminated when error occurred. */
    if (!no_error(s))
        return NULL;

    /* The referenced node is returned when silenced. */
    if (P4_NeedLift(s, e))
        return reftok;

    /* A single reference expr can be a rule: `e = { ref }` */
    /* In such a case, a node for `e` with single child `ref` is created. */
    /* */
    P4_Node*  result = NULL;
    catch_oom(result = P4_CreateNode(s->content, startpos, endpos, e->name));

    P4_AdoptNode(result->head, result->tail, reftok);
    return result;
}

P4_PRIVATE(P4_Node*)
P4_MatchSequence(P4_Source* s, P4_Expression* e) {
    assert(no_error(s), "can't proceed due to a failed match");

    P4_Expression *member = NULL;
    P4_Node *head = NULL,
             *tail = NULL,
             *tok = NULL,
             *whitespace = NULL;

    autofree P4_Slice* backrefs = P4_MALLOC(sizeof(P4_Slice) * e->count);
    if (backrefs == NULL)
        panic("failed to create slices: out of memory");

    bool space = need_whitespace(s);

    mark_position(s, startpos);

    size_t i = 0;

    for (i = 0; i < e->count; i++) {
        member = e->members[i];

        /* Optional `WHITESPACE` and `COMMENT` are inserted between every member. */
        if (space && i > 0) {
            whitespace = P4_MatchSpacedExpressions(s, NULL);
            if (!no_error(s)) goto finalize;
            P4_AdoptNode(head, tail, whitespace);
        }

        mark_position(s, member_startpos);

        switch (member->kind) {
            case P4_BackReference:
                if (i == member->backref_index) {
                    P4_MatchRaisef(s, P4_IndexError,
                        "expect %s (backref %zu point to self)",
                        P4_PeekFrame(s)->rule->name, i);
                    goto finalize;
                }
                tok = P4_MatchBackReference(s, e, backrefs, member);
                if (!no_error(s)) goto finalize;
                break;
            case P4_Cut:
                tok = P4_MatchCut(s, e);
                break;
            default:
                tok = P4_Match(s, member);
                break;
        }

        /* If any of the sequence members fails, the entire sequence fails. */
        /* Puke the eaten text and free all created nodes. */
        if (!no_error(s)) {
            goto finalize;
        }

        P4_AdoptNode(head, tail, tok);

        mark_position(s, member_endpos);
        set_slice(&backrefs[i], member_startpos, member_endpos);
    }

    if (P4_NeedLift(s, e))
        return head;

    if (is_non_terminal(e) && head != NULL && head->next == NULL) {
        return head;
    }

    mark_position(s, endpos);

    P4_Node*  ret = NULL;
    catch_oom(ret = P4_CreateNode(s->content, startpos, endpos, e->name));

    ret->head = head;
    ret->tail = tail;
    return ret;

finalize:
    set_position(s, startpos);
    P4_DeleteNode(head);
    return NULL;
}

P4_PRIVATE(P4_Node*)
P4_MatchChoice(P4_Source* s, P4_Expression* e) {
    P4_Node* tok = NULL;
    P4_Expression* member = NULL;

    /* A member is attempted if previous yields no match. */
    /* The oneof match matches successfully immediately if any match passes. */
    mark_position(s, startpos);
    size_t i;
    for (i = 0; i < e->count; i++) {
        member = e->members[i];
        tok = P4_Match(s, member);
        if (no_error(s)) break;
        if (no_match(s)) {
            /* retry until the last one. */
            if (i < e->count-1) {
                P4_RescueError(s);
                set_position(s, startpos);
            /* fail when the last one is a no-match. */
            } else {
                P4_MatchRaisef(s, P4_MatchError, "expect %s", P4_PeekFrame(s)->rule->name);
                goto finalize;
            }
        }
    }
    mark_position(s, endpos);

    if (P4_NeedLift(s, e))
        return tok;

    P4_Node* oneof = P4_CreateNode (s->content, startpos, endpos, e->name);
    if (oneof == NULL)
        panic("failed to create node: out of memory");

    P4_AdoptNode(oneof->head, oneof->tail, tok);
    return oneof;

finalize:
    set_position(s, startpos);
    P4_FREE(tok);
    return NULL;
}

/*
 * Repetition matcher function.
 *
 * Returns a node in a greedy fashion.
 *
 * There are seven repetition mode: zeroormore, zerooronce,
 */
P4_PRIVATE(P4_Node*)
P4_MatchRepeat(P4_Source* s, P4_Expression* e) {
    size_t min = SIZE_MAX, max = SIZE_MAX, repeated = 0;

    assert(e->repeat_min != min || e->repeat_max != max,
            "need at least one of min/max");
    assert(e->repeat_expr != NULL, "need repeat expression");
    assert(no_error(s), "can't proceed due to a failed match");

# define IS_REF(e) ((e)->kind == P4_Reference)
# define IS_PROGRESSING(k) ((k)==P4_Positive \
                            || (k)==P4_Negative)

    /* when expression inside repetition is non-progressing, it repeats indefinitely. */
    /* we know negative/positive definitely not progressing, */
    /* and so does a reference to a negative/positive rule. */
    /* Question: we may never list all the cases in this way. How to deal with it better? */
    if (IS_PROGRESSING(e->repeat_expr->kind) ||
            (IS_REF(e->repeat_expr)
             && IS_PROGRESSING(P4_GetReference(s, e->repeat_expr)->kind))) {
        P4_MatchRaisef(s, P4_AdvanceError,
            "expect %s (no progressing in repetition)", P4_PeekFrame(s)->rule->name);
        return NULL;
    }

    min = e->repeat_min;
    max = e->repeat_max;

    bool space = need_whitespace(s);
    mark_position(s, startpos);
    P4_Node *head = NULL, *tail = NULL, *tok = NULL, *whitespace = NULL;

    while (!is_end(s)) {
        mark_position(s, whitespace_startpos);

        /* SPACED rule expressions are inserted between every repetition. */
        if (space && repeated > 0 ) {
            whitespace = P4_MatchSpacedExpressions(s, NULL);
            if (!no_error(s)) goto finalize;
            P4_AdoptNode(head, tail, whitespace);
        }

        tok = P4_Match(s, e->repeat_expr);

        if (no_match(s)) {
            assert(tok == NULL, "failed match should not produce a token");

            /* considering the case: MATCH WHITESPACE MATCH WHITESPACE no_match */
            if (space && repeated > 0){/*              ^          ^ we are here */
                                                                  /* ^ puke extra whitespace */
                set_position(s, whitespace_startpos);
                                               /*           ^ now we are here */
            }

            if (min != SIZE_MAX && repeated < min) {
                P4_MatchRaisef(s, P4_MatchError, "expect %s (insufficient repetitions)", P4_PeekFrame(s)->rule->name);
                goto finalize;
            } else {                       /* sufficient repetitions. */
                P4_RescueError(s);
                break;
            }
        }

        if (!no_error(s))
            goto finalize;

        if (P4_GetPosition(s) == whitespace_startpos->pos) {
            P4_MatchRaisef(s, P4_MatchError,
                    "expect %s (repetition not advancing)", P4_PeekFrame(s)->rule->name);
            goto finalize;
        }

        repeated++;
        P4_AdoptNode(head, tail, tok);

        if (max != SIZE_MAX && repeated == max) { /* enough attempts */
            P4_RescueError(s);
            break;
        }

    }

    /* there should be no error when repetition is successful. */
    assert(no_error(s), "can't proceed due to a failed match");

    /* fails when attempts are excessive, e.g. repeated > max. */
    if (max != SIZE_MAX && repeated > max) {
        P4_MatchRaisef(s, P4_MatchError,
            "expect %s (at most %zu repetitions)",
            P4_PeekFrame(s)->rule->name, max);
        goto finalize;
    }

    if (min != SIZE_MAX && repeated < min) {
        P4_MatchRaisef(s, P4_MatchError,
            "expect %s (at least %zu repetitions)",
            P4_PeekFrame(s)->rule->name, min);
        goto finalize;
    }

    if (P4_GetPosition(s) == startpos->pos) /* success but no node is produced. */
        goto finalize;

    if (P4_NeedLift(s, e))
        return head;

    if (is_non_terminal(e) && head != NULL && head->next == NULL) {
        return head;
    }

    mark_position(s, endpos);

    P4_Node*  ret = NULL;
    catch_oom(ret = P4_CreateNode(s->content, startpos, endpos, e->name));

    P4_AdoptNode(ret->head, ret->tail, head);
    return ret;

/* cleanup before returning NULL. */
/* nodes between head..tail should be freed. */
finalize:
    set_position(s, startpos);
    P4_DeleteNode(head);
    return NULL;
}

P4_PRIVATE(P4_Node*)
P4_MatchPositive(P4_Source* s, P4_Expression* e) {
    assert(no_error(s) && e->ref_expr != NULL, "expression should not be null");

    mark_position(s, startpos);

    P4_Node* node = P4_Match(s, e->ref_expr);
    if (node != NULL)
        P4_DeleteNode(node);

    set_position(s, startpos);

    return NULL;
}

P4_PRIVATE(P4_Node*)
P4_MatchNegative(P4_Source* s, P4_Expression* e) {
    assert(no_error(s) && e->ref_expr != NULL, "expression should not be null");

    mark_position(s, startpos);
    P4_Node* node = P4_Match(s, e->ref_expr);
    set_position(s, startpos);

    if (no_error(s)) {
        P4_DeleteNode(node);
        P4_MatchRaisef(s, P4_MatchError, "expect %s", P4_PeekFrame(s)->rule->name);
    } else if (s->err == P4_MatchError || s->err == P4_CutError) {
        P4_RescueError(s);
    }

    return NULL;
}

P4_PRIVATE(P4_Node*)
P4_MatchCut(P4_Source* s, P4_Expression* e) {
    /* enable flag cut in the top frame. */
    P4_Frame* frame = P4_PeekFrame(s);
    frame->cut = true;
    return NULL;
}

P4_Node*
P4_MatchDispatch(P4_Source* s, P4_Expression* e) {
    P4_Node* result = NULL;

    switch (e->kind) {
        case P4_Literal:
            result = P4_MatchLiteral(s, e);
            break;
        case P4_Range:
            result = P4_MatchRange(s, e);
            break;
        case P4_Reference:
            result = P4_MatchReference(s, e);
            break;
        case P4_Sequence:
            result = P4_MatchSequence(s, e);
            break;
        case P4_Choice:
            result = P4_MatchChoice(s, e);
            break;
        case P4_Positive:
            result = P4_MatchPositive(s, e);
            break;
        case P4_Negative:
            result = P4_MatchNegative(s, e);
            break;
        case P4_Repeat:
            result = P4_MatchRepeat(s, e);
            break;
        case P4_Cut:
            panic("cut can be applied only in sequence.");
        case P4_BackReference:
            panic("backreference can be applied only in sequence.");
        default:
            UNREACHABLE();
            panicf("invalid dispatch kind: %zu.", e->kind);
    }

    return result;
}

/*
 * The match function updates the state given an expression.
 *
 * It returns a node linked list, NULL if no node is generated.
 * State pos will advance if needed.
 * Not-advancing pos / NULL returning node list do not indicate a failed match.
 * It fails when state err/errmsg are set.
 * It propagate the failed match up to the top level.
 */
P4_Node*
P4_Match(P4_Source* s, P4_Expression* e) {
    assert(e != NULL, "expression should not be null");

    P4_Error     err = P4_Ok;
    P4_Node* result = NULL;

    catch_err(s->err);

    catch_err(
        P4_PushFrame(s, e),
        P4_MatchRaisef(s, err, "expect %s (max recursion)", e->name)
    );

    result = P4_MatchDispatch(s, e);

    P4_Frame* frame = P4_PeekFrame(s);
    if (no_match(s) && frame->cut && !s->whitespacing) {
        s->err = P4_CutError;
    }

    P4_PopFrame(s, NULL);

    catch_err(s->err, {
        P4_DeleteNode(result);
        if (e->name != NULL && s->errmsg[0] == 0) {
            P4_MatchRaisef(s, s->err, "expect %s", e->name);
        }
    });

    if (s->grammar->on_match != NULL) {
        catch_err((s->grammar->on_match)(s->grammar, e, result), {
            if (s->errmsg[0] == 0)
                P4_MatchRaisef(s, s->err, "expect %s (match callback failed)", e->name);
            P4_DeleteNode(result);
        });
    }

    return result;

finalize:
    if (s->grammar->on_error != NULL) {
        err = (s->grammar->on_error)(s->grammar, e);
        if (err != P4_Ok && s->errmsg[0] == 0)
            P4_MatchRaisef(s, s->err, "expect %s (error callback failed)", e->name);
    }
    return NULL;
}

P4_PRIVATE(P4_Node*)
P4_MatchSpacedExpressions(P4_Source* s, P4_Expression* e) {
    /* implicit whitespace is guaranteed to be an unnamed rule. */
    /* state flag is guaranteed to be none. */
    assert(no_error(s), "can't proceed due to a failed match");

    if (s->grammar == NULL)
        return NULL;

    P4_Expression* implicit_whitespace = P4_GetWhitespaces(s->grammar);
    assert(implicit_whitespace != NULL, "implicit whitespace should not be null");

    /* (1) Temporarily set implicit whitespace to empty. */
    s->whitespacing = true;

    /* (2) Perform implicit whitespace checks. */
    /*     We won't do implicit whitespace inside an implicit whitespace expr. */
    P4_Node* result = P4_Match(s, implicit_whitespace);
    if (no_match(s))
        P4_RescueError(s);

    /* (3) Set implicit whitespace back. */
    s->whitespacing = false;

    return result;
}

P4_PRIVATE(P4_Node*)
P4_MatchBackReference(P4_Source* s, P4_Expression* e, P4_Slice* backrefs, P4_Expression* backref) {
    assert(backrefs != NULL, "backrefs should not be null");

    size_t index = backref->backref_index;

    if (index > e->count || index < 0) {
        P4_MatchRaisef(s, P4_IndexError,
            "expect %s (backref %zu out of bound)",
            P4_PeekFrame(s)->rule->name, index);
        return NULL;
    }

    P4_Expression* backref_expr = e->members[index];

    if (backref_expr == NULL) {
        P4_MatchRaisef(s, P4_PegError, "expect %s (null backref expr)", P4_PeekFrame(s)->rule->name);
        return NULL;
    }

    if (backref_expr->kind == P4_BackReference) {
        P4_MatchRaisef(s, P4_PegError, "expect %s (backref point to backref)", P4_PeekFrame(s)->rule->name);
        return NULL;
    }

    P4_Slice* slice = &(backrefs[index]);
    /* backrefs is allocated as an array so it shouldn't be null. */
    assert(slice != NULL, "backref should not be null");

    autofree P4_String litstr = NULL;
    catch_oom(litstr = P4_CopySliceString(s->content, slice));

    P4_Expression* litexpr = NULL;
    catch_oom(litexpr = P4_CreateLiteral(litstr, backref->sensitive));

# define NO_OP
# define set_literal_rule_name(n, bref, op) \
    if ((bref)->kind == P4_Reference) {\
        (n) = op((bref)->ref_expr->name); \
    } else if (is_rule((bref))) {\
        (n) = op((bref)->name); \
    } else { \
        (n) = NULL; \
    }

    set_literal_rule_name(litexpr->name, backref_expr, STRDUP);

    P4_Node* tok = P4_MatchLiteral(s, litexpr);

    if (tok != NULL) {
        set_literal_rule_name(tok->rule_name, backref_expr, NO_OP);
    }

    P4_DeleteExpression(litexpr);
    return tok;
}

void
P4_JsonifySourceAst(FILE* stream, P4_Node* node) {
    P4_Node* tmp = node;

    fprintf(stream, "[");
    while (tmp != NULL) {
        fprintf(stream, "{\"slice\":[%lu,%lu]", tmp->slice.start.pos, tmp->slice.stop.pos);
        fprintf(stream, ",\"type\":\"%s\"", tmp->rule_name);
        if (tmp->head != NULL) {
            fprintf(stream, ",\"children\":");
            P4_JsonifySourceAst(stream, tmp->head);
        }
        fprintf(stream, "}");
        if (tmp->next != NULL) fprintf(stream, ",");
        tmp = tmp->next;
    }
    fprintf(stream, "]");
}


P4_PUBLIC P4_Error
P4_InspectSourceAst(P4_Node* node, void* userdata, P4_Error (*inspector)(P4_Node*, void*)) {
    P4_Node* tmp = node;
    P4_Error err = P4_Ok;

    while (tmp != NULL) {
        catch_err(inspector(tmp, userdata));

        if (tmp->head != NULL)
            catch_err(P4_InspectSourceAst(tmp->head, userdata, inspector));

        tmp = tmp->next;
    }

finalize:
    return err;
}

/*
 * Get version string.
 */
P4_PUBLIC P4_String
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

/*
P4_PUBLIC P4_Expression*
P4_CreateNumeric(size_t num) {
    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Numeric;
    expr->num = num;
    return expr;
}
*/

P4_PUBLIC P4_Expression*
P4_CreateLiteral(const P4_String literal, bool sensitive) {
    if (literal == NULL)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Literal;
    expr->flag = 0;
    expr->name = NULL;
    expr->literal = STRDUP(literal);
    expr->sensitive = sensitive;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateRange(P4_Rune lower, P4_Rune upper, size_t stride) {
    if (lower > upper || lower > 0x10ffff || upper > 0x10ffff || lower == 0 || upper == 0)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Range;
    expr->flag = 0;
    expr->name = NULL;
    expr->ranges_count = 1;
    expr->ranges = P4_MALLOC(sizeof(P4_RuneRange));
    expr->ranges[0].lower = lower;
    expr->ranges[0].upper = upper;
    expr->ranges[0].stride = stride;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateRanges(size_t count, P4_RuneRange* ranges) {
    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Range;
    expr->flag = 0;
    expr->name = NULL;
    expr->ranges_count = count;
    expr->ranges = P4_MALLOC(sizeof(P4_RuneRange) * count);

    size_t i = 0;
    for (i = 0; i < count; i++) {
        expr->ranges[i].lower = ranges[i].lower;
        expr->ranges[i].upper = ranges[i].upper;
        expr->ranges[i].stride = ranges[i].stride;
    }

    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateReference(P4_String reference) {
    if (reference == 0)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Reference;
    expr->flag = 0;
    expr->name = NULL;
    expr->reference = STRDUP(reference);
    expr->ref_expr = NULL;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreatePositive(P4_Expression* refexpr) {
    if (refexpr == NULL)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Positive;
    expr->flag = 0;
    expr->name = NULL;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateNegative(P4_Expression* refexpr) {
    if (refexpr == NULL)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Negative;
    expr->flag = 0;
    expr->name = NULL;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateCut() {
    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_Cut;
    expr->flag = 0;
    expr->name = NULL;
    return expr;
}

P4_PRIVATE(P4_Expression*)
P4_CreateContainer(size_t count) {
    if (count == 0)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->flag = 0;
    expr->name = NULL;
    expr->count = count;
    expr->members = P4_MALLOC(sizeof(P4_Expression*) * count);
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateSequence(size_t count) {
    if (count == 0)
        return NULL;

    P4_Expression* expr = P4_CreateContainer(count);

    if (expr == NULL)
        return NULL;

    expr->kind = P4_Sequence;

    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateChoice(size_t count) {
    if (count == 0)
        return NULL;

    P4_Expression* expr = P4_CreateContainer(count);

    if (expr == NULL)
        return NULL;

    expr->kind = P4_Choice;

    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateSequenceWithMembers(size_t count, ...) {
    if (count == 0)
        return NULL;

    P4_Expression* expr = P4_CreateSequence(count);

    if (expr == NULL)
        return NULL;

    va_list members;
    size_t i;
    va_start (members, count);

    for (i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return expr;

finalize:
    P4_DeleteExpression(expr);
    return NULL;
}

P4_PUBLIC P4_Expression*
P4_CreateChoiceWithMembers(size_t count, ...) {
    if (count == 0)
        return NULL;

    P4_Expression* expr = P4_CreateChoice(count);

    if (expr == NULL)
        return NULL;

    va_list members;
    size_t i;
    va_start (members, count);

    for (i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return expr;

finalize:
    P4_DeleteExpression(expr);
    return NULL;
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMinMax(P4_Expression* repeat, size_t min, size_t max) {
    if (repeat == NULL)
        return NULL;

    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->flag = 0;
    expr->name = NULL;
    expr->kind = P4_Repeat;
    expr->repeat_expr = repeat;
    expr->repeat_min = min;
    expr->repeat_max = max;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMin(P4_Expression* repeat, size_t min) {
    return P4_CreateRepeatMinMax(repeat, min, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMax(P4_Expression* repeat, size_t max) {
    return P4_CreateRepeatMinMax(repeat, SIZE_MAX, max);
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatExact(P4_Expression* repeat, size_t minmax) {
    return P4_CreateRepeatMinMax(repeat, minmax, minmax);
}

P4_PUBLIC P4_Expression*
P4_CreateZeroOrOnce(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, 1);
}

P4_PUBLIC P4_Expression*
P4_CreateZeroOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateOnceOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 1, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateBackReference(size_t index, bool sensitive) {
    P4_Expression* expr = P4_MALLOC(sizeof(P4_Expression));
    expr->kind = P4_BackReference;
    expr->flag = 0;
    expr->name = NULL;
    expr->backref_index = index;
    expr->sensitive = sensitive;
    return expr;
}

P4_PUBLIC bool
P4_IsRule(P4_Expression* e) {
    if (e == NULL)
        return false;

    return e->name != NULL;
}

P4_PUBLIC P4_Grammar*    P4_CreateGrammar(void) {
    P4_Grammar* grammar = P4_MALLOC(sizeof(P4_Grammar));
    grammar->rules = kh_init(rules);
    grammar->spaced_count = SIZE_MAX;
    grammar->spaced_rules = NULL;
    grammar->depth = P4_DEFAULT_RECURSION_LIMIT;
    grammar->on_match = NULL;
    grammar->on_error = NULL;
    grammar->free_func = NULL;
    return grammar;
}

P4_PUBLIC void
P4_DeleteGrammar(P4_Grammar* grammar) {
    P4_Expression*  rule;
    if (grammar) {
        if (grammar->spaced_rules) P4_DeleteExpression(grammar->spaced_rules);
        kh_foreach_value(grammar->rules, rule, {
            P4_DeleteExpression(rule);
        });
        kh_destroy(rules, grammar->rules);
        P4_FREE(grammar);
    }
}

P4_PUBLIC P4_Expression*
P4_GetGrammarRule(P4_Grammar* grammar, P4_String name) {
    khint_t k = kh_get(rules, grammar->rules, name);
    bool is_missing = (k == kh_end(grammar->rules));
    return is_missing ? NULL : kh_val(grammar->rules, k);
}

P4_PUBLIC P4_Error
P4_SetGrammarRuleFlag(P4_Grammar* grammar, P4_String name, P4_ExpressionFlag flag) {
    P4_Expression* expr = P4_GetGrammarRule(grammar, name);
    if (expr == NULL)
        return P4_NameError;

    P4_SetExpressionFlag(expr, flag);

    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_SetRecursionLimit(P4_Grammar* grammar, size_t limit) {
    if (grammar == NULL)
        return P4_NullError;

    grammar->depth = limit;

    return P4_Ok;
}

P4_PUBLIC size_t
P4_GetRecursionLimit(P4_Grammar* grammar) {
    return grammar == NULL ? 0 : grammar->depth;
}

P4_Error
P4_SetUserDataFreeFunc(P4_Grammar* grammar, P4_UserDataFreeFunc free_func) {
    if (grammar == NULL)
        return P4_NullError;

    grammar->free_func = free_func;

    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddGrammarRule(P4_Grammar* grammar, P4_String name, P4_Expression* expr) {
    int kret;

    if (grammar == NULL || name == NULL || expr == NULL)
        return P4_NullError;

    if (expr->name == NULL) {
        expr->name = STRDUP(name);
    } else if (expr->name != NULL) {
        if (strcmp(name, expr->name) != 0) {
            P4_FREE(expr->name);
            expr->name = STRDUP(name);
        }
    }

    khint_t k = kh_put(rules, grammar->rules, expr->name, &kret);
    kh_value(grammar->rules, k) = expr;

    return P4_Ok;
}

P4_PUBLIC P4_Source*
P4_CreateSource(P4_String content, P4_String entry_name) {
    P4_Source* source = P4_MALLOC(sizeof(P4_Source));
    source->content = content;
    source->entry_name = STRDUP(entry_name);
    source->pos = 0;
    source->lineno = 1;
    source->offset = 1;
    source->root = NULL;
    source->frame_stack = NULL;
    source->frame_stack_size = 0;
    source->whitespacing = false;

    P4_SetSourceSlice(source, 0, strlen(content));
    P4_RescueError(source);

    return source;
}

P4_Error
P4_SetSourceSlice(P4_Source* source, size_t start, size_t stop) {
    if (source == 0)
        return P4_NullError;

    P4_Position* startpos = &(P4_Position){ .pos=0, .lineno=1, .offset=1 };
    P4_DiffPosition(source->content, startpos, start, startpos);
    set_position(source, startpos);

    P4_Position* endpos = &(P4_Position){ 0 };
    P4_DiffPosition(source->content, startpos, stop-start, endpos);

    set_slice(&source->slice, startpos, endpos);

    return P4_Ok;
}

P4_PUBLIC void
P4_ResetSource(P4_Source* source) {
    if (source == NULL)
        return;

    P4_Frame* tmp = source->frame_stack;
    while(source->frame_stack) {
        tmp = source->frame_stack->next;
        P4_FREE(source->frame_stack);
        source->frame_stack = tmp;
    }

    P4_RescueError(source);

    if (source->root) {
        P4_DeleteNodeUserData(source->grammar, source->root);
        P4_DeleteNode(source->root);
    }

}

P4_PUBLIC void
P4_DeleteSource(P4_Source* source) {
    P4_ResetSource(source);
    P4_FREE(source->entry_name);
    P4_FREE(source);
}

P4_PUBLIC P4_Node*
P4_GetSourceAst(P4_Source* source) {
    return source == NULL ? NULL : source->root;
}

P4_PUBLIC P4_Node*
P4_AcquireSourceAst(P4_Source* source) {
    P4_Node* root = P4_GetSourceAst(source);
    source->root = NULL;
    P4_ResetSource(source);
    return root;
}

P4_PUBLIC size_t
P4_GetSourcePosition(P4_Source* source) {
    return source == NULL ? 0 : source->pos;
}

P4_PUBLIC P4_Error
P4_Parse(P4_Grammar* grammar, P4_Source* source) {
    P4_Error err = P4_Ok;

    catch_err(source->err);

    if (source->root != NULL)
        return P4_Ok;

    source->grammar = grammar;

    P4_Expression* expr     = P4_GetGrammarRule(grammar, source->entry_name);
    P4_Node*       tok      = P4_Match(source, expr);

    source->root            = tok;

finalize:
    return source->err;
}


P4_PUBLIC bool
P4_HasError(P4_Source* source) {
    if (source == NULL) panic("source must not be NULL.");
    return source->err != P4_Ok;
}

P4_PUBLIC P4_Error
P4_GetError(P4_Source* source) {
    if (source == NULL) panic("source must not be NULL.");
    return source->err;
}

P4_String
P4_GetErrorString(P4_Error err) {
    switch (err) {
        case P4_Ok: return "";
        case P4_InternalError: return "InternalError";
        case P4_MatchError: return "MatchError";
        case P4_NameError: return "NameError";
        case P4_AdvanceError: return "AdvanceError";
        case P4_MemoryError: return "MemoryError";
        case P4_ValueError: return "ValueError";
        case P4_IndexError: return "IndexError";
        case P4_KeyError: return "KeyError";
        case P4_NullError: return "NullError";
        case P4_StackError: return "StackError";
        case P4_PegError: return "PegError";
        case P4_CutError: return "CutError";
        default: return "UnknownError";
    }
}

P4_PUBLIC P4_String
P4_GetErrorMessage(P4_Source* source) {
    if (source == NULL || source->errmsg[0] == 0)
        return NULL;

    return source->errmsg;
}

P4_PRIVATE(P4_Error)
P4_SetWhitespaces(P4_Grammar* grammar) {
    P4_Error        err = P4_Ok;
    P4_Expression   *rule = NULL, *rule_ref = NULL;

    /* Get the total number of SPACED rules */
    size_t          count = 0;
    kh_foreach_value(grammar->rules, rule, {
        if (is_spaced(rule)) count++;
    });

    /* Set the total number of SPACED rules */
    grammar->spaced_count = count;

    /* No whitespace rule if count == 0 */
    if (count == 0)
        return P4_Ok;

    /* Create a wrapper repeat expression. */
    P4_Expression*  repeat = NULL;
    catch_oom(repeat = P4_CreateChoice(count));

    /* Add all SPACED rules to the repeat expression. */
    size_t j = 0;
    kh_foreach_value(grammar->rules, rule, {
        if (is_spaced(rule)) {
            catch_oom(rule_ref = P4_CreateReference(rule->name));
            rule_ref->ref_expr = rule;

            catch_err(P4_SetMember(repeat, j, rule_ref));
            j++;
        }
    });

    /* Repeat the choice for zero or more times. */
    catch_oom(grammar->spaced_rules = P4_CreateZeroOrMore(repeat));
    return P4_Ok;

finalize:
    if (repeat != NULL) {
        P4_DeleteExpression(repeat);
    }

    grammar->spaced_rules = NULL;
    grammar->spaced_count = SIZE_MAX;

    return err;
}

P4_PRIVATE(P4_Expression*)
P4_GetWhitespaces(P4_Grammar* g) {
    if (g == NULL)
        return NULL;

    if (g->spaced_count == SIZE_MAX)
        P4_SetWhitespaces(g);

    return g->spaced_rules;
}

P4_PUBLIC void
P4_SetExpressionFlag(P4_Expression* e, P4_ExpressionFlag f) {
    assert(e != NULL, "expression should not be null");
    e->flag |= f;
}

P4_PRIVATE(size_t)
P4_GetPosition(P4_Source* s) {
    return s->pos;
}

P4_PRIVATE(void)
P4_DiffPosition(P4_String str, P4_Position* start, size_t offset, P4_Position* stop) {
    size_t start_pos = start->pos;
    size_t stop_pos = start_pos + offset;
    size_t stop_lineno = start->lineno;
    size_t stop_offset = start->offset;

    size_t n = 0;
    bool eol = false;
    for (n = start_pos; n < stop_pos; n++) {
        if (str[n] == '\n') {
            stop_offset++;
            eol = true;
        } else if (eol) {
            stop_lineno++;
            stop_offset = 1;
            eol = false;
        } else {
            stop_offset++;
        }
    }
    if (eol) {
        stop_lineno++;
        stop_offset = 1;
        eol = false;
    }

    stop->pos = stop_pos;
    stop->lineno = stop_lineno;
    stop->offset = stop_offset;
}

/*
 * Get the remaining text.
 */
P4_PRIVATE(P4_String)
P4_RemainingText(P4_Source* s) {
    return s->content + s->pos;
}

P4_PUBLIC P4_Error
P4_AddLiteral(P4_Grammar* grammar, P4_String name, const P4_String literal, bool sensitive) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateLiteral(literal, sensitive));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRange(P4_Grammar* grammar, P4_String name, P4_Rune lower, P4_Rune upper, size_t stride) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateRange(lower, upper, stride));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRanges(P4_Grammar* grammar, P4_String name, size_t count, P4_RuneRange* ranges) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateRanges(count, ranges));
    return P4_Ok;
}


P4_PUBLIC P4_Error
P4_AddPositive(P4_Grammar* grammar, P4_String name, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, name, P4_CreatePositive(ref_expr));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddNegative(P4_Grammar* grammar, P4_String name, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, name, P4_CreateNegative(ref_expr));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddSequence(P4_Grammar* grammar, P4_String name, size_t size) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, name)->kind = P4_Sequence;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddSequenceWithMembers(P4_Grammar* grammar, P4_String name, size_t count, ...) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateSequence(count));

    size_t i = 0;
    P4_Expression* expr = P4_GetGrammarRule(grammar, name);

    va_list members;
    va_start (members, count);

    for (i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL)
            panicf("failed to set %zuth expression.", i);
    }

    va_end (members);

    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddChoice(P4_Grammar* grammar, P4_String name, size_t size) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, name)->kind = P4_Choice;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddChoiceWithMembers(P4_Grammar* grammar, P4_String name, size_t count, ...) {
    P4_AddSomeGrammarRule(grammar, name, P4_CreateChoice(count));

    P4_Expression* expr = P4_GetGrammarRule(grammar, name);
    size_t i = 0;

    va_list members;
    va_start (members, count);

    for (i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL)
            panicf("failed to set %zuth expression.", i);
    }

    va_end (members);

    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_SetMember(P4_Expression* expr, size_t offset, P4_Expression* member) {
    if (expr == NULL
            || member == NULL
            || expr->members == NULL
            || expr->count == 0) {
        return P4_NullError;
    }

    if (expr->kind != P4_Sequence
            && expr->kind != P4_Choice) {
        return P4_ValueError;
    }

    if (offset < 0
            || offset >= expr->count) {
        return P4_IndexError;
    }

    expr->members[offset] = member;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_SetReferenceMember(P4_Expression* expr, size_t offset, P4_String ref) {
    /* this functions allows adding a member which is simply a reference. */

    P4_Error       err      = P4_Ok;
    P4_Expression* ref_expr = NULL;

    catch_oom(ref_expr = P4_CreateReference(ref));
    catch_err(P4_SetMember(expr, offset, ref_expr), P4_DeleteExpression(ref_expr));

finalize:
    return err;
}

P4_PUBLIC size_t
P4_GetMembersCount(P4_Expression* expr) {
    return expr == NULL ? 0 : expr->count;
}


P4_PUBLIC P4_Expression*
P4_GetMember(P4_Expression* expr, size_t offset) {
    if (expr == NULL
            || expr->members == NULL
            || expr->count == 0) {
        return NULL;
    }

    if ((expr->kind != P4_Sequence) && (expr->kind != P4_Choice)) {
        return NULL;
    }

    if (offset < 0
            || offset >= expr->count) {
        return NULL;
    }

    return expr->members[offset];
}

P4_Expression* P4_CreateStartOfInput() {
    return P4_CreatePositive(P4_CreateRange(1, 0x10ffff, 1));
}

P4_Expression* P4_CreateEndOfInput() {
    return P4_CreateNegative(P4_CreateRange(1, 0x10ffff, 1));
}

P4_Expression* P4_CreateJoin(const P4_String joiner, P4_String reference) {
    return P4_CreateSequenceWithMembers(2,
        P4_CreateReference(reference),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateLiteral(joiner, true),
                P4_CreateReference(reference)
            )
        )
    );
}

P4_PUBLIC void
P4_DeleteExpression(P4_Expression* expr) {
    if (expr == NULL)
        return;

    size_t i;

    switch (expr->kind) {
        case P4_Literal:
            if (expr->literal)
                P4_FREE(expr->literal);
            break;
        case P4_Range:
            P4_FREE(expr->ranges);
            break;
        case P4_Reference:
            /* Case P4_Reference is quite special - it is not the owner of ref_expr.
             * We free the referenced string if exists though. */
            if (expr->reference != NULL)
                P4_FREE(expr->reference);
            break;
        case P4_Positive:
        case P4_Negative:
            if (expr->ref_expr)
                P4_DeleteExpression(expr->ref_expr);
            break;
        case P4_Sequence:
        case P4_Choice:
            for (i = 0; i < expr->count; i++) {
                if (expr->members[i])
                    P4_DeleteExpression(expr->members[i]);
                expr->members[i] = NULL;
            }
            P4_FREE(expr->members);
            expr->members = NULL;
            break;
        case P4_Repeat:
            if (expr->repeat_expr)
                P4_DeleteExpression(expr->repeat_expr);
            break;
        default:
            break;
    }

    if (expr->name)
        P4_FREE(expr->name);

    P4_FREE(expr);
}

P4_PUBLIC P4_Error
P4_AddReference(P4_Grammar* grammar, P4_String name, P4_String ref) {
    if (ref == 0) {
        return P4_NullError;
    }

    P4_AddSomeGrammarRule(grammar, name, P4_CreateReference(ref));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddZeroOrOnce(P4_Grammar* grammar, P4_String name, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateZeroOrOnce(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddZeroOrMore(P4_Grammar* grammar, P4_String name, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateZeroOrMore(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddOnceOrMore(P4_Grammar* grammar, P4_String name, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateOnceOrMore(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMin(P4_Grammar* grammar, P4_String name, P4_Expression* repeat, size_t min) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateRepeatMin(repeat, min));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMax(P4_Grammar* grammar, P4_String name, P4_Expression* repeat, size_t max) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateRepeatMax(repeat, max));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMinMax(P4_Grammar* grammar, P4_String name, P4_Expression* repeat, size_t min, size_t max) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateRepeatMinMax(repeat, min, max));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatExact(P4_Grammar* grammar, P4_String name, P4_Expression* repeat, size_t num) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, name, P4_CreateRepeatExact(repeat, num));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddJoin(P4_Grammar* grammar, P4_String name, const P4_String joiner, P4_String ref) {
    if (ref == 0 || strcmp(name, ref) == 0) {
        return P4_NullError;
    }

    P4_AddSomeGrammarRule(grammar, name, P4_CreateJoin(joiner, ref));
    return P4_Ok;
}

P4_PUBLIC P4_Slice*
P4_GetNodeSlice(P4_Node* node) {
    if (node == NULL)
        return NULL;

    return &(node->slice);
}

P4_PUBLIC size_t
P4_GetNodeChildrenCount(P4_Node* node) {
    P4_Node* child = NULL;
    size_t   count = 0;
    foreach_child(node, child, {
        count++;
    });
    return count;
}

P4_PRIVATE(P4_String)
P4_CopySliceString(P4_String s, P4_Slice* slice) {
    /* return the string covered by the slice.
     * note that caller should free the copied string. */

    size_t    len = get_slice_size(slice);
    P4_String str = P4_MALLOC(len+1);
    strncpy(str, s + slice->start.pos, len);
    str[len] = '\0';

    return str;
}

P4_PUBLIC P4_String
P4_CopyNodeString(P4_Node* node) {
    if (node == NULL)
        return NULL;

    return P4_CopySliceString(node->text, &(node->slice));
}

/*
 * Determine if expression has flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC bool
P4_IsSquashed(P4_Expression* e) {
    return (e->flag & P4_FLAG_SQUASHED) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_LIFTED.
 */
P4_PUBLIC bool
P4_IsLifted(P4_Expression* e) {
    return (e->flag & P4_FLAG_LIFTED) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_TIGHT.
 */
P4_PUBLIC bool
P4_IsTight(P4_Expression* e) {
    return (e->flag & P4_FLAG_TIGHT) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_SPACED.
 */
P4_PUBLIC bool
P4_IsSpaced(P4_Expression* e) {
    return (e->flag & P4_FLAG_SPACED) != 0;
}

/*
 * Set expression flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC bool
P4_IsScoped(P4_Expression* e) {
    return (e->flag & P4_FLAG_SCOPED) != 0;
}

/*
 * Set expression flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC void
P4_SetSquashed(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SQUASHED);
}

/*
 * Set expression flag P4_FLAG_LIFTED.
 */
P4_PUBLIC void
P4_SetLifted(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_LIFTED);
}

/*
 * Set expression flag P4_FLAG_TIGHT.
 */
P4_PUBLIC void
P4_SetTight(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_TIGHT);
}

/*
 * Set expression flag P4_FLAG_SPACED.
 */
P4_PUBLIC void
P4_SetSpaced(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SPACED);
}

/*
 * Set expression flag P4_FLAG_SCOPED.
 */
P4_PUBLIC void
P4_SetScoped(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SCOPED);
}

P4_PUBLIC P4_Error
P4_SetGrammarCallback(P4_Grammar* grammar, P4_MatchCallback matchcb, P4_ErrorCallback errcb) {
    if (grammar == NULL)
        return P4_NullError;

    grammar->on_match = matchcb;
    grammar->on_error = errcb;

    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_RefreshReference(P4_Expression* expr, P4_String name) {
    if (expr == NULL || name == NULL)
        return P4_NullError;

    size_t i = 0;
    P4_Error err = P4_Ok;

    switch(expr->kind) {
        /* clean up reference->ref_expr. */
        case P4_Reference:
            if (strcmp(expr->reference, name) == 0)
                expr->ref_expr = NULL;
            break;

        /* recursively refresh references. */
        case P4_Positive:
        case P4_Negative:
            catch_err(P4_RefreshReference(expr->ref_expr, name));
            break;
        case P4_Sequence:
        case P4_Choice:
            for (i = 0; i < expr->count; i++) {
                catch_err(P4_RefreshReference(expr->members[i], name));
            }
            break;
        case P4_Repeat:
            catch_err(P4_RefreshReference(expr->repeat_expr, name));
            break;
        default:
            break;
    }

finalize:
    return err;
}

P4_PUBLIC P4_Error
P4_ReplaceGrammarRule(P4_Grammar* grammar, P4_String name, P4_Expression* expr) {
    /* ensure inputs are valid. */
    if (grammar == NULL || name == NULL || expr == NULL)
        return P4_NullError;

    /* get the existing rule expr. */
    P4_Expression* oldexpr = P4_GetGrammarRule(grammar, name);
    if (oldexpr == NULL)
        return P4_NameError;

    P4_Error err = P4_Ok;

    /* delete rule expr from grammar rules. */
    khint_t k = kh_get(rules, grammar->rules, name);
    kh_del(rules, grammar->rules, k);
    P4_DeleteExpression(oldexpr);

    /* set the new rule expr name. */
    assert(expr->name == NULL, "expr name should not set by P4_ReplaceGrammarRule().");
    expr->name = STRDUP(name);

    /* ensure references in all rule expressions are refreshed
     * so they can pick up the new rule expr. */
    P4_Expression* rule;
    kh_foreach_value(grammar->rules, rule, {
        if (strcmp(rule->name, name) != 0)
            catch_err(P4_RefreshReference(rule, name))
    });

    /* put rule expr to grammar rules. */
    int kret;
    k = kh_put(rules, grammar->rules, name, &kret);
    kh_value(grammar->rules, k) = expr;

finalize:
    return err;
}

P4_Grammar* P4_CreatePegGrammar () {
    /* build bootstrap peg grammar using low-level api.
     * the bootstrap peg grammar is used to parse grammar from user input. */

    P4_Error    err     = P4_Ok;
    P4_Grammar* grammar = P4_CreateGrammar();

    catch_err(P4_AddChoiceWithMembers(grammar, "number", 2,
        P4_CreateLiteral("0", true),
        P4_CreateSequenceWithMembers(2,
            P4_CreateRange('1', '9', 1),
            P4_CreateZeroOrMore(P4_CreateRange('0', '9', 1))
        )
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "number", P4_FLAG_SQUASHED | P4_FLAG_TIGHT));

    catch_err(P4_AddChoiceWithMembers(grammar, "hexdigit", 3,
        P4_CreateRange('0', '9', 1),
        P4_CreateRange('a', 'f', 1),
        P4_CreateRange('A', 'F', 1)
    ));
    catch_err(P4_AddRepeatExact(grammar, "two_hexdigits",
        P4_CreateReference("hexdigit"), 2));
    catch_err(P4_AddRepeatExact(grammar, "four_hexdigits",
        P4_CreateReference("hexdigit"), 4));
    catch_err(P4_AddRepeatExact(grammar, "eight_hexdigits",
        P4_CreateReference("hexdigit"), 8));

    catch_err(P4_AddChoiceWithMembers(grammar, "char", 4,
        P4_CreateRange(0x20, 0x21, 1), /* Can't be 0x22: double quote " */
        P4_CreateRange(0x23, 0x5b, 1), /* Can't be 0x5c: escape leading \ */
        P4_CreateRange(0x5d, 0x10ffff, 1),
        P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("\\", true),
            P4_CreateChoiceWithMembers(11,
                P4_CreateLiteral("\"", true),
                P4_CreateLiteral("/", true),
                P4_CreateLiteral("\\", true),
                P4_CreateLiteral("b", true),
                P4_CreateLiteral("f", true),
                P4_CreateLiteral("n", true),
                P4_CreateLiteral("r", true),
                P4_CreateLiteral("t", true),
                P4_CreateSequenceWithMembers(3,
                    P4_CreateLiteral("x", true),
                    P4_CreateCut(),
                    P4_CreateReference("two_hexdigits")
                ),
                P4_CreateSequenceWithMembers(3,
                    P4_CreateLiteral("u", true),
                    P4_CreateCut(),
                    P4_CreateReference("four_hexdigits")
                ),
                P4_CreateSequenceWithMembers(3,
                    P4_CreateLiteral("U", true),
                    P4_CreateCut(),
                    P4_CreateReference("eight_hexdigits")
                )
            )
        )
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "char", P4_FLAG_SQUASHED | P4_FLAG_TIGHT));

    catch_err(P4_AddZeroOrMore(grammar, "chars", P4_CreateReference("char")));
    catch_err(P4_AddSequenceWithMembers(grammar, "literal", 4,
        P4_CreateLiteral("\"", true),
        P4_CreateCut(),
        P4_CreateReference("chars"),
        P4_CreateLiteral("\"", true)
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "literal", P4_FLAG_SQUASHED | P4_FLAG_TIGHT));

    catch_err(P4_AddSequenceWithMembers(grammar, "range", 4,
        P4_CreateLiteral("[", true),
        P4_CreateCut(),
        P4_CreateChoiceWithMembers(2,
            P4_CreateSequenceWithMembers(4,
                P4_CreateLiteral("\\p{", true),
                P4_CreateCut(),
                P4_CreateReference("range_category"),
                P4_CreateLiteral("}", true)
            ),
            P4_CreateSequenceWithMembers(4,
                P4_CreateReference("char"),
                P4_CreateLiteral("-", true),
                P4_CreateReference("char"),
                P4_CreateZeroOrOnce(P4_CreateSequenceWithMembers(2,
                    P4_CreateLiteral("..", true),
                    P4_CreateReference("number")
                ))
            )
        ),
        P4_CreateLiteral("]", true)
    ));
    catch_err(P4_AddChoiceWithMembers(grammar, "range_category", 19,
        P4_CreateLiteral("Cc", true),
        P4_CreateLiteral("Cf", true),
        P4_CreateLiteral("Co", true),
        P4_CreateLiteral("Cs", true),
        P4_CreateLiteral("C", true),
        P4_CreateLiteral("Ll", true),
        P4_CreateLiteral("Lm", true),
        P4_CreateLiteral("Lo", true),
        P4_CreateLiteral("Lt", true),
        P4_CreateLiteral("Lu", true),
        P4_CreateLiteral("L", true),
        P4_CreateLiteral("Nl", true),
        P4_CreateLiteral("Nd", true),
        P4_CreateLiteral("No", true),
        P4_CreateLiteral("N", true),
        P4_CreateLiteral("Zl", true),
        P4_CreateLiteral("Zp", true),
        P4_CreateLiteral("Zs", true),
        P4_CreateLiteral("Z", true)
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "reference", 2,
        P4_CreateChoiceWithMembers(3,
            P4_CreateRange('a', 'z', 1),
            P4_CreateRange('A', 'Z', 1),
            P4_CreateLiteral("_", true)
        ),
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(4,
                P4_CreateRange('a', 'z', 1),
                P4_CreateRange('A', 'Z', 1),
                P4_CreateRange('0', '9', 1),
                P4_CreateLiteral("_", true)
            )
        )
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "reference", P4_FLAG_SQUASHED | P4_FLAG_TIGHT));

    catch_err(P4_AddSequenceWithMembers(grammar, "back_reference", 3,
        P4_CreateLiteral("\\", true),
        P4_CreateCut(),
        P4_CreateReference("number")
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "positive", 3,
        P4_CreateLiteral("&", true),
        P4_CreateCut(),
        P4_CreateReference("primary")
    ));
    catch_err(P4_AddSequenceWithMembers(grammar, "negative", 3,
        P4_CreateLiteral("!", true),
        P4_CreateCut(),
        P4_CreateReference("primary")
    ));

    catch_err(P4_AddLiteral(grammar, "onceormore", "+", true));
    catch_err(P4_AddLiteral(grammar, "zeroormore", "*", true));
    catch_err(P4_AddLiteral(grammar, "zerooronce", "?", true));

    catch_err(P4_AddSequenceWithMembers(grammar, "repeatmin", 4,
        P4_CreateLiteral("{", true),
        P4_CreateReference("number"),
        P4_CreateLiteral(",", true),
        P4_CreateLiteral("}", true)
    ));
    catch_err(P4_AddSequenceWithMembers(grammar, "repeatmax", 4,
        P4_CreateLiteral("{", true),
        P4_CreateLiteral(",", true),
        P4_CreateReference("number"),
        P4_CreateLiteral("}", true)
    ));
    catch_err(P4_AddSequenceWithMembers(grammar, "repeatminmax", 5,
        P4_CreateLiteral("{", true),
        P4_CreateReference("number"),
        P4_CreateLiteral(",", true),
        P4_CreateReference("number"),
        P4_CreateLiteral("}", true)
    ));
    catch_err(P4_AddSequenceWithMembers(grammar, "repeatexact", 3,
        P4_CreateLiteral("{", true),
        P4_CreateReference("number"),
        P4_CreateLiteral("}", true)
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "repeat", 2,
        P4_CreateReference("primary"),
        P4_CreateZeroOrOnce(
            P4_CreateChoiceWithMembers(7,
                P4_CreateReference("onceormore"),
                P4_CreateReference("zeroormore"),
                P4_CreateReference("zerooronce"),
                P4_CreateReference("repeatexact"),
                P4_CreateReference("repeatminmax"),
                P4_CreateReference("repeatmin"),
                P4_CreateReference("repeatmax")
            )
        )
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "repeat", P4_FLAG_NON_TERMINAL));

    catch_err(P4_AddLiteral(grammar, "dot", ".", true));
    catch_err(P4_AddLiteral(grammar, "cut", "@cut", true));

    catch_err(P4_AddChoiceWithMembers(grammar, "primary", 10,
        P4_CreateReference("literal"),
        P4_CreateReference("insensitive"),
        P4_CreateReference("range"),
        P4_CreateSequenceWithMembers(2,
            P4_CreateReference("reference"),
            P4_CreateNegative(P4_CreateLiteral("=", true))
        ),
        P4_CreateReference("back_reference"),
        P4_CreateReference("positive"),
        P4_CreateReference("negative"),
        P4_CreateSequenceWithMembers(3,
            P4_CreateLiteral("(", true),
            P4_CreateReference("choice"),
            P4_CreateLiteral(")", true)
        ),
        P4_CreateReference("dot"),
        P4_CreateReference("cut")
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "primary", P4_FLAG_LIFTED));

    catch_err(P4_AddSequenceWithMembers(grammar, "insensitive", 2,
        P4_CreateLiteral("i", true),
        P4_CreateReference("literal")
    ));
    catch_err(P4_SetGrammarRuleFlag(grammar, "insensitive", P4_FLAG_TIGHT));

    catch_err(P4_AddJoin(grammar, "choice", "/", "sequence"));
    catch_err(P4_SetGrammarRuleFlag(grammar, "choice", P4_FLAG_NON_TERMINAL));

    catch_err(P4_AddOnceOrMore(grammar, "sequence", P4_CreateReference("repeat")));
    catch_err(P4_SetGrammarRuleFlag(grammar, "sequence", P4_FLAG_NON_TERMINAL));

    catch_err(P4_AddReference(grammar, "expression", "choice"));
    catch_err(P4_SetGrammarRuleFlag(grammar, "expression", P4_FLAG_LIFTED));

    catch_err(P4_AddReference(grammar, "name", "reference"));
    catch_err(P4_SetGrammarRuleFlag(grammar, "name", P4_FLAG_SQUASHED));

    catch_err(P4_AddSequenceWithMembers(grammar, "decorator", 3,
        P4_CreateLiteral("@", true),
        P4_CreateCut(),
        P4_CreateChoiceWithMembers(6,
            P4_CreateLiteral("squashed", true),
            P4_CreateLiteral("scoped", true),
            P4_CreateLiteral("spaced", true),
            P4_CreateLiteral("lifted", true),
            P4_CreateLiteral("tight", true),
            P4_CreateLiteral("nonterminal", true)
        )
    ));

    catch_err(P4_AddZeroOrMore(grammar, "decorators",
            P4_CreateReference("decorator")
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "rule", 6,
        P4_CreateReference("decorators"),
        P4_CreateReference("name"),
        P4_CreateCut(),
        P4_CreateLiteral("=", true),
        P4_CreateReference("expression"),
        P4_CreateLiteral(";", true)
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "grammar", 3,
        P4_CreateStartOfInput(),
        P4_CreateOnceOrMore(P4_CreateReference("rule")),
        P4_CreateEndOfInput()
    ));

    catch_err(P4_AddChoiceWithMembers(grammar, "whitespace", 4,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral("\n", true)
    ));

    catch_err(P4_SetGrammarRuleFlag(grammar, "whitespace",
        P4_FLAG_LIFTED | P4_FLAG_SPACED
    ));

    catch_err(P4_AddSequenceWithMembers(grammar, "comment", 3,
        P4_CreateLiteral("#", true),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateNegative(P4_CreateLiteral("\n", true)),
                P4_CreateRange(0x1, 0x10ffff, 1)
            )
        ),
        P4_CreateZeroOrOnce(P4_CreateLiteral("\n", true))
    ));

    catch_err(P4_SetGrammarRuleFlag(grammar, "comment",
        P4_FLAG_LIFTED | P4_FLAG_SPACED
    ));

    return grammar;

finalize:
    P4_DeleteGrammar(grammar);
    return NULL;
}

# define MIN(a,b) ((a) < (b) ? (a) : (b))
# define NODE_ERROR_HINT_FMT "char %zu-%zu: %.*s"
# define NODE_ERROR_HINT \
                node->slice.start.pos, \
                node->slice.stop.pos, \
                MIN((int)get_slice_size(&node->slice), 50), \
                node->text + node->slice.start.pos


P4_PRIVATE(P4_Error)
P4_PegEvalFlag(P4_Node* node, P4_ExpressionFlag *flag) {
    P4_String node_str = node->text + node->slice.start.pos;
    size_t    node_len = get_slice_size(&node->slice);

    if (node_len == 9 && memcmp("@squashed", node_str, node_len) == 0)
        *flag = P4_FLAG_SQUASHED;
    else if (node_len == 7 && memcmp("@scoped", node_str, node_len) == 0)
        *flag = P4_FLAG_SCOPED;
    else if (node_len == 7 && memcmp("@spaced", node_str, node_len) == 0)
        *flag = P4_FLAG_SPACED;
    else if (node_len == 7 && memcmp("@lifted", node_str, node_len) == 0)
        *flag = P4_FLAG_LIFTED;
    else if (node_len == 6 && memcmp("@tight", node_str, node_len) == 0)
        *flag = P4_FLAG_TIGHT;
    else if (node_len == 12 && memcmp("@nonterminal", node_str, node_len) == 0)
        *flag = P4_FLAG_NON_TERMINAL;
    else {
        /* P4_CreatePegRule() guarantees only 6 kinds of strings are possible. */
        UNREACHABLE();
        panicf("InternalError: invalid flag: %s" NODE_ERROR_HINT_FMT,
            node_str, NODE_ERROR_HINT);
    }

    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalRuleFlags(P4_Node* node, P4_ExpressionFlag* flag) {
    P4_Error          err        = P4_Ok;
    P4_Node*         child      = NULL;
    P4_ExpressionFlag child_flag = 0;

    for (child = node->head; child != NULL; child = child->next) {
        catch_err(P4_PegEvalFlag(child, &child_flag));
        *flag |= child_flag;
    }
    return P4_Ok;

finalize:
    *flag = 0;
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalNumber(P4_Node* node, size_t* num) {
    P4_String str = NULL;
    catch_oom(str = P4_CopyNodeString(node));
    *num = atol(str);
    P4_FREE(str);
    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalChar(P4_Node* node, P4_Rune* rune) {
    size_t size = P4_ReadEscapedRune(node->text+node->slice.start.pos, rune);

    if (size == 0)
        return P4_ValueError;

    if (node->slice.start.pos+size > node->slice.stop.pos)
        return P4_ValueError;

    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalLiteral(P4_Node* node, P4_Result* result) {
    size_t         i    = 0,
                   size = 0,
                   idx  = 0;
    P4_Error       err  = P4_Ok;
    P4_Rune        rune = 0;
    P4_String      lit  = NULL,
                   cur  = NULL;
    P4_Expression* expr = NULL;

    size_t len = get_slice_size(&node->slice) - 2; /* - 2: remove two quotes */
    if (len <= 0) {
        catch_err(P4_PegError, P4_EvalRaisef(result,
            "literal rule should have at least one character. "
            NODE_ERROR_HINT_FMT, NODE_ERROR_HINT));
    }

    catch_oom(cur = lit = P4_MALLOC((len+1) * sizeof(char)));

    for (i = node->slice.start.pos+1, idx = 0; i < node->slice.stop.pos-1; i += size) {
        size = P4_ReadEscapedRune(node->text+i, &rune);

        if ((rune > 0x10ffff) ||
                (rune == 0) ||
                (size == 0) ||
                (i + size > node->slice.stop.pos-1)) {
            catch_err(P4_PegError, P4_EvalRaisef(result,
                "char %lu is invalid. " NODE_ERROR_HINT_FMT,
                idx, NODE_ERROR_HINT));
        }

        cur = P4_ConcatRune(cur, rune, size);
        idx++;
    }
    *cur = '\0';

    catch_oom(expr = P4_CreateLiteral(lit, true));

finalize:
    P4_FREE(lit);
    result->expr = expr;
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalInsensitiveLiteral(P4_Node* node, P4_Result* result) {
    P4_Error err = P4_Ok;

    /* eval literal and set it as insensitive. */
    catch_err(P4_PegEvalLiteral(node->head, result));
    P4_Expression* expr = unwrap_expr(result);
    assert(expr != NULL, "insensitive literal expr should not be null.");
    expr->sensitive = false;
    return P4_Ok;

finalize:
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalRange(P4_Node* node, P4_Result* result) {
    P4_Error       err  = P4_Ok;
    P4_Expression* expr = NULL;

    if (node->head == node->tail) { /* one single child - [\\p{XX}] */
        P4_RuneRange* ranges = NULL;
        size_t count = 0;

        if (P4_ReadRuneRange(node->head->text, &node->head->slice, &count, &ranges) == 0)
            panicf("ValueError: failed to read code point from source. "
                NODE_ERROR_HINT_FMT, NODE_ERROR_HINT);

        catch_oom(expr = P4_CreateRanges(count, ranges));

    } else { /* two to three children - [lower-upper] or [lower-upper..stride] */
        P4_Rune lower = 0, upper = 0;
        size_t stride = 1;

        P4_Node* lower_node = node->head;
        P4_Node* upper_node = lower_node->next;
        P4_Node* stride_node = upper_node->next;

        catch_err(P4_PegEvalChar(lower_node, &lower));
        catch_err(P4_PegEvalChar(upper_node, &upper));
        if (stride_node) catch_err(P4_PegEvalNumber(stride_node, &stride));

        if (lower > upper) {
            catch_err(P4_PegError, P4_EvalRaisef(result,
                "range lower 0x%x is greater than upper 0x%x. "
                NODE_ERROR_HINT_FMT, lower, upper, NODE_ERROR_HINT));
        }

        if ((lower == 0) || (upper == 0) || (stride == 0)) {
            catch_err(P4_PegError, P4_EvalRaisef(result,
                "range lower 0x%x, upper 0x%x, stride 0x%lx "
                "must be all non-zeros. " NODE_ERROR_HINT_FMT,
                lower, upper, stride, NODE_ERROR_HINT));
        }

        if ((lower > 0x10ffff) || (upper > 0x10ffff)) {
            catch_err(P4_PegError, P4_EvalRaisef(result,
                "range lower 0x%x, upper 0x%x must be "
                "less than 0x10ffff. " NODE_ERROR_HINT_FMT,
                lower, upper, NODE_ERROR_HINT));
        }

        catch_oom(expr = P4_CreateRange(lower, upper, stride));
    }

    result->expr = expr;
    return P4_Ok;

finalize:
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalMembers(P4_Node* node, P4_Expression* expr, P4_Result* result) {
    size_t   i     = 0;
    P4_Error err   = P4_Ok;
    P4_Node* child = NULL;

    /* for each child, eval expr and set it as ith member. */
    foreach_child(node, child, {
        catch_err(P4_PegEvalExpression(child, result));
        catch_err(P4_SetMember(expr, i, unwrap_expr(result)));
        i++;
    });

finalize:
    /* crash if failed to eval & set members. */
    if (err)
        panicf(
            "%s: failed to set %zuth member. " NODE_ERROR_HINT_FMT,
            P4_GetErrorString(err), i, NODE_ERROR_HINT
        );
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalSequence(P4_Node* node, P4_Result* result) {
    P4_Error        err = P4_Ok;
    P4_Expression*  expr = NULL;

    /* create a sequence expr and then eval all members. */
    catch_oom(expr = P4_CreateSequence(P4_GetNodeChildrenCount(node)));
    catch_err(P4_PegEvalMembers(node, expr, result));
    result->expr = expr;
    return P4_Ok;

finalize:
    /* free resources. */
    if (expr) P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalChoice(P4_Node* node, P4_Result* result) {
    P4_Error  err = P4_Ok;
    P4_Expression*  expr = NULL;

    /* create a choice expr and then eval all members. */
    catch_oom(expr = P4_CreateChoice(P4_GetNodeChildrenCount(node)));
    catch_err(P4_PegEvalMembers(node, expr, result));
    result->expr = expr;
    return P4_Ok;

finalize:
    /* free resources. */
    if (expr) P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalPositive(P4_Node* node, P4_Result* result) {
    P4_Error        err  = P4_Ok;
    P4_Expression*  ref  = NULL;
    P4_Expression*  expr = NULL;

    /* eval positive->ref_expr. */
    catch_err(P4_PegEvalExpression(node->head, result));
    ref = unwrap_expr(result);

    /* create a positive expr. */
    catch_oom(expr = P4_CreatePositive(ref));
    result->expr = expr;
    return P4_Ok;

finalize:
    /* free resources. */
    if (ref)  P4_DeleteExpression(ref);
    if (expr) P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalNegative(P4_Node* node, P4_Result* result) {
    P4_Error        err  = P4_Ok;
    P4_Expression*  ref  = NULL;
    P4_Expression*  expr = NULL;

    /* eval negative->ref_expr. */
    catch_err(P4_PegEvalExpression(node->head, result));
    ref = unwrap_expr(result);

    /* create a negative expr. */
    catch_oom(expr = P4_CreateNegative(ref));
    result->expr = expr;
    return P4_Ok;

finalize:
    /* free resources. */
    if (ref)  P4_DeleteExpression(ref);
    if (expr) P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalRepeat(P4_Node* node, P4_Result* result) {
    P4_Error        err  = P4_Ok;
    P4_Expression*  ref  = NULL;
    P4_Expression*  expr = NULL;
    size_t          min  = 0,
                    max  = SIZE_MAX;

    /* eval repeated expr. */
    catch_err(P4_PegEvalExpression(node->head, result));
    ref = unwrap_expr(result);

    /* eval repeat min/max. */
    P4_String       rule_name = node->head->next->rule_name;
    if (strcmp(rule_name, "zeroormore") == 0) {
        min = 0; max = SIZE_MAX;
    } else if (strcmp(rule_name, "zerooronce") == 0) {
        min = 0; max = 1;
    } else if (strcmp(rule_name, "onceormore") == 0) {
        min = 1; max = SIZE_MAX;
    } else if (strcmp(rule_name, "repeatmin") == 0) {
        catch_err(P4_PegEvalNumber(node->head->next->head, &min));
    } else if (strcmp(rule_name, "repeatmax") == 0) {
        catch_err(P4_PegEvalNumber(node->head->next->head, &max));
    } else if (strcmp(rule_name, "repeatminmax") == 0) {
        catch_err(P4_PegEvalNumber(node->head->next->head, &min));
        catch_err(P4_PegEvalNumber(node->head->next->tail, &max));
    } else if (strcmp(rule_name, "repeatexact") == 0) {
        catch_err(P4_PegEvalNumber(node->head->next->head, &min));
        max = min;
    } else {
        UNREACHABLE();
        panicf("InternalError: unknown repeat kind: %s"
            NODE_ERROR_HINT_FMT, node->head->next->rule_name, NODE_ERROR_HINT);
    }

    /* repeat min should be greater than max. */
    if (min > max) {
        catch_err(P4_PegError, P4_EvalRaisef(result,
            "repeat min %lu is greater than max %lu. "
            NODE_ERROR_HINT_FMT, min, max, NODE_ERROR_HINT));
    }

    /* create repeat expr. */
    catch_oom(expr = P4_CreateRepeatMinMax(ref, min, max));
    result->expr = expr;
    return P4_Ok;

finalize:
    /* free resources. */
    if (ref)  P4_DeleteExpression(ref);
    if (expr) P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalDot(P4_Node* node, P4_Result* result) {
    catch_oom(result->expr = P4_CreateRange(0x1, 0x10ffff, 1));
    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalCut(P4_Node* node, P4_Result* result) {
    catch_oom(result->expr = P4_CreateCut());
    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalRuleName(P4_Node* node, P4_String* result) {
    /* get the rule name string length. */
    size_t len = get_slice_size(&node->slice);
    assert(len > 0, "Node slice size should be greater than zero.");

    /* malloc a new string for the rule name. */
    catch_oom(*result = P4_MALLOC((len+1) * sizeof(char)));
    memcpy(*result, node->text + node->slice.start.pos, len);
    (*result)[len] = '\0';

    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_PegEvalReference(P4_Node* node, P4_Result* result) {
    P4_Error       err  = P4_Ok;
    P4_String      ref  = NULL;
    P4_Expression* expr = NULL;

    /* first, eval ref string, then, create ref expr. */
    catch_err(P4_PegEvalRuleName(node, &ref));
    catch_oom(expr = P4_CreateReference(ref));
    result->expr = expr;
    P4_FREE(ref);

    return P4_Ok;

finalize:
    /* free resources. */
    if (ref)    P4_FREE(ref);
    if (expr)   P4_DeleteExpression(expr);

    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalBackReference(P4_Node* node, P4_Result* result) {
    P4_Error       err  = P4_Ok;
    P4_Expression* expr = NULL;
    size_t         idx  = SIZE_MAX;

    catch_err(P4_PegEvalNumber(node->head, &idx));
    catch_oom(expr = P4_CreateBackReference(idx, false));
    result->expr = expr;

    return P4_Ok;

finalize:
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalGrammarRule(P4_Node* node, P4_Result* result) {
    P4_String           rule_name = NULL;
    P4_ExpressionFlag   rule_flag = 0;
    P4_Node*           child     = NULL;
    P4_Error            err       = P4_Ok;
    P4_Expression*      expr      = NULL;

    /* eval rule decorators, rule name, and rule expression
     * from node children. */

    foreach_child(node, child, {
        if (strcmp(child->rule_name, "decorators") == 0)
            catch_err(P4_PegEvalRuleFlags(child, &rule_flag))
        else if (strcmp(child->rule_name, "name") == 0)
            catch_err(P4_PegEvalRuleName(child, &rule_name))
        else
            catch_err(P4_PegEvalExpression(child, result))
    });

    /* we can now unwrap an expr from the result.
     * next, let's set rule name and rule flags. */

    expr = unwrap_expr(result);
    assert(expr != NULL, "failed to eval grammar rule expression");
    expr->name = rule_name;
    expr->flag = rule_flag;

    return P4_Ok;

finalize:
    /* free resources. */
    if (rule_name)  P4_FREE(rule_name);
    if (expr)       P4_DeleteExpression(expr);
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalGrammarReferences(
        P4_Grammar* grammar,
        P4_Expression* expr,
        P4_Result* result) {
# define recursive(e) \
    if ((e)) \
        return P4_PegEvalGrammarReferences(grammar, (e), result);

    size_t   i   = 0;
    P4_Error err = P4_Ok;

    assert(expr != NULL, "reference expr is NULL.");

    switch (expr->kind) {
        /* detect if references are all "resolvable".
         * raise an error if can't find a grammar rule for the reference. */
        case P4_Reference:
            if (!expr->reference ||
                    !P4_GetGrammarRule(grammar, expr->reference)) {
                catch_err(
                    P4_NameError,
                    P4_EvalRaisef(result, "reference %s is undefined", expr->reference)
                );
            }
            break;

        /* recursively check non-reference expressions. */
        case P4_Positive:
        case P4_Negative:
            recursive(expr->ref_expr);
            break;
        case P4_Sequence:
        case P4_Choice:
            for (i = 0; i < expr->count; i++)
                recursive(expr->members[i])
            break;
        case P4_Repeat:
            recursive(expr->repeat_expr);
            break;
        default: break;
    }

finalize:
    return err;
}

P4_PRIVATE(P4_Error)
P4_PegEvalGrammar(P4_Node* node, P4_Result* result) {
    P4_Error        err     = P4_Ok;
    P4_Grammar*     grammar = NULL;
    P4_Expression*  rule = NULL;
    P4_Node*        child = NULL;

    /* create a grammar object. */
    catch_oom(grammar = P4_CreateGrammar());

    /* eval each child to a grammar rule,
     * and then add the rule to grammar object. */
    foreach_child(node, child, {
        catch_err(P4_PegEvalGrammarRule(child, result));
        rule = unwrap_expr(result);
        catch_err(
            P4_AddGrammarRule(grammar, rule->name, rule),
            P4_EvalRaisef(result, "failed to add rule %s.", rule->name)
        );
    });

    /* traverse all grammar rules to resolve references. */
    kh_foreach_value(grammar->rules, rule, {
        catch_err(P4_PegEvalGrammarReferences(grammar, rule, result));
    });

finalize:
    /* free resources. */
    if (err) P4_DeleteGrammar(grammar);

    /* set either the grammar object or NULL to result.*/
    result->grammar = err ? NULL : grammar;

    return err;
}

P4_PUBLIC P4_Error
P4_PegEvalExpression(P4_Node* node, P4_Result* result) {
    /* entry function for eval peg ast. this function dispatch (node)
     * to specific functions based on its rule name. */

    if (strcmp(node->rule_name, "literal")      == 0)
        return P4_PegEvalLiteral(node, result);

    if (strcmp(node->rule_name, "insensitive")  == 0)
        return P4_PegEvalInsensitiveLiteral(node, result);

    if (strcmp(node->rule_name, "range")        == 0)
        return P4_PegEvalRange(node, result);

    if (strcmp(node->rule_name, "sequence")     == 0)
        return P4_PegEvalSequence(node, result);

    if (strcmp(node->rule_name, "choice")       == 0)
        return P4_PegEvalChoice(node, result);

    if (strcmp(node->rule_name, "positive")     == 0)
        return P4_PegEvalPositive(node, result);

    if (strcmp(node->rule_name, "negative")     == 0)
        return P4_PegEvalNegative(node, result);

    if (strcmp(node->rule_name, "repeat")       == 0)
        return P4_PegEvalRepeat(node, result);

    if (strcmp(node->rule_name, "dot")          == 0)
        return P4_PegEvalDot(node, result);

    if (strcmp(node->rule_name, "cut")          == 0)
        return P4_PegEvalCut(node, result);

    if (strcmp(node->rule_name, "reference")    == 0)
        return P4_PegEvalReference(node, result);

    if (strcmp(node->rule_name, "back_reference")    == 0)
        return P4_PegEvalBackReference(node, result);

    UNREACHABLE();
    panicf("Unreachable: node %p is not a peg expression", node);
}

P4_PUBLIC P4_Error
P4_LoadGrammarResult(P4_String rules, P4_Result* result) {
    P4_Grammar*       bootstrap = NULL;
    P4_Source*        rules_src = NULL;
    P4_Node*          rules_tok = NULL;
    P4_Error          err       = P4_Ok;
    P4_Result*        evalres   = &(P4_Result){0};

    /* load PEG bootstrap grammar object. */
    catch_oom(bootstrap = P4_CreatePegGrammar());

    /* load grammar rule source object. */
    catch_oom(rules_src = P4_CreateSource(rules, "grammar"));

    /* parse grammar rule source */
    catch_err(P4_Parse(bootstrap, rules_src), {
        P4_EvalRaisef(result, "%s: grammar syntax error: %s.",
            P4_GetErrorString(err), P4_GetErrorMessage(rules_src));
    });

    /* get grammar rule parse tree. */
    if ((rules_tok = P4_GetSourceAst(rules_src)) == NULL) {
        catch_err(P4_PegError, {
            P4_EvalRaisef(result, "%s: parse tree is null.", P4_GetErrorString(err));
        });
    }

    /* eval grammar rule parse tree to grammar object. */
    catch_err(P4_PegEvalGrammar(rules_tok, evalres), {
        P4_EvalRaisef(result, "%s: %s.", P4_GetErrorString(err), evalres->errmsg);
    });

finalize:
    /* set the grammar object to the result,
     * regardless of evaluated successfully or not. */
    result->grammar = (err == P4_Ok) ? evalres->grammar : NULL;

    /* free resources. */
    if (rules_src) P4_DeleteSource(rules_src);
    if (bootstrap) P4_DeleteGrammar(bootstrap);

    return err;
}

P4_PUBLIC P4_Grammar*
P4_LoadGrammar(P4_String rules) {
    P4_Error    err     = P4_Ok;
    P4_Result*  result  = &(P4_Result){0};

    /* attempts to load the grammar object from rules string. */
    catch_err(P4_LoadGrammarResult(rules, result));
    return result->grammar;

finalize:
    /* terminates the program if failed to load grammar object. */
    panicf("%s\n", result->errmsg);
}

P4_PUBLIC const P4_String
P4_GetRuleName(P4_Expression* expr) {
    return expr->name;
}
