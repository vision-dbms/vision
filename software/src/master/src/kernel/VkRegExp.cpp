/*****  Vision Kernel Regular Expression Interface  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

#include "Vk.h"
#include "VkRegExp.h"


/******************************
 ******************************
 *****  'regexp' Sources  *****
 ******************************
 ******************************/

/*
 * VkRegExp
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 *	This version has alterations for ISO C usage. These changes are
 *	Copyright 1992 DataFocus Incorporated. All rights reserved.
 * 
 *  This version has alterations for usage within vision software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 */

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart	char that must begin a match; '\0' if none obvious
 * reganch	is the match anchored (at beginning-of-line only)?
 * regmust	string (pointer into program) that match must include, or NULL
 * regmlen	length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that New_VkRegExp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in Execute() needs it and VkRegExp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition	number	opnd?	meaning */
#define	END	0	/* no	End of program. */
#define	BOL	1	/* no	Match "" at beginning of line. */
#define	EOL	2	/* no	Match "" at end of line. */
#define	ANY	3	/* no	Match any one character. */
#define	ANYOF	4	/* str	Match any character in this string. */
#define	ANYBUT	5	/* str	Match any character not in this string. */
#define	BRANCH	6	/* node	Match this alternative, or the next... */
#define	BACK	7	/* no	Match "", "next" ptr points backward. */
#define	EXACTLY	8	/* str	Match this string. */
#define	NOTHING	9	/* no	Match empty string. */
#define	STAR	10	/* node	Match this (simple) thing 0 or more times. */
#define	PLUS	11	/* node	Match this (simple) thing 1 or more times. */
#define	OPEN	20	/* no	Mark this point in input as start of #n. */
			/*	OPEN+1 is number 1, etc. */
#define	CLOSE	30	/* no	Analogous to OPEN. */

/*
 * Opcode notes:
 *
 * BRANCH	The set of branches constituting a single choice are hooked
 *		together with their "next" pointers, since precedence prevents
 *		anything being concatenated to any individual branch.  The
 *		"next" pointer of the last BRANCH in a choice points to the
 *		thing following the whole choice.  This is also where the
 *		final "next" pointer of each individual branch points; each
 *		branch starts with the operand node of a BRANCH node.
 *
 * BACK		Normal "next" pointers all implicitly point forward; BACK
 *		exists to make loop structures possible.
 *
 * STAR,PLUS	'?', and complex '*' and '+', are implemented as circular
 *		BRANCH structures using BACK.  Simple cases (one character
 *		per match) are implemented with STAR and PLUS for speed
 *		and to minimize recursive plunges.
 *
 * OPEN,CLOSE	...are numbered at compile time.
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define	OPERAND(p)	((p) + 3)

/*
 * From 'regmagic.h'...
 */
#define	MAGIC	0234


/*
 * Utility definitions.
 */
#ifndef CHARBITS
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#else
#define	UCHARAT(p)	((int)*(p)&CHARBITS)
#endif

#define	FAIL(m)	{ return(NULL); }
#define	ISMULT(c)	((c) == '*' || (c) == '+' || (c) == '?')
#define	META	"^$.[()|?+*\\"

/*
 * Flags to be passed up and down.
 */
#define	HASWIDTH	01	/* Known never to match null string. */
#define	SIMPLE		02	/* Simple enough to be STAR/PLUS operand. */
#define	SPSTART		04	/* Starts with * or +. */
#define	WORST		0	/* Worst case. */


/*
 - New_VkRegExp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
VkRegExp::VkRegExp (char const* exp) : m_bValid (false) {
	REGISTER char *scan;
	REGISTER char *longest;
	REGISTER int len;
	int flags;

	if (exp == NULL) {
	    m_vsInfo.setTo ("NULL argument");
	    return;
	}

	/* First pass: determine size, legality. */
	regparse = exp;
	regnpar = 1;
	regsize = 0L;
	regcode = &regdummy;
	regc((char)MAGIC);
	if (reg(0, &flags) == NULL) {
	    m_vsInfo.setTo ("Bad Input");
	    return;
	}

	/* Allocate space. */
	program = programStorage.storage (regsize);
	if (IsNil (program)) {
	    m_vsInfo.setTo ("Out Of Space");
	    return;
	}

	/* Second pass: emit code. */
	regparse = exp;
	regnpar = 1;
	regcode = program;
	regc((char)MAGIC);
	if (reg(0, &flags) == NULL) {
	    m_vsInfo.setTo ("Bad Input");
	    return;
	}

	/* Dig out information for optimizations. */
	regstart = '\0';	/* Worst-case defaults. */
	reganch = 0;
	regmust = NULL;
	regmlen = 0;
	scan = program+1;			/* First BRANCH. */
	if (OP(regnext(scan)) == END) {		/* Only one top-level choice. */
		scan = OPERAND(scan);

		/* Starting-point info. */
		if (OP(scan) == EXACTLY)
			regstart = *OPERAND(scan);
		else if (OP(scan) == BOL)
			reganch++;

		/*
		 * If there's something expensive in the r.e., find the
		 * longest literal string that must appear and make it the
		 * regmust.  Resolve ties in favor of later strings, since
		 * the regstart check works with the beginning of the r.e.
		 * and avoiding duplication strengthens checking.  Not a
		 * strong reason, but sufficient in the absence of others.
		 */
		if (flags&SPSTART) {
			longest = NULL;
			len = 0;
			for (; scan != NULL; scan = regnext(scan))
				if (OP(scan) == EXACTLY && strlen(OPERAND(scan)) >= (size_t) len) {
					longest = OPERAND(scan);
					len = strlen(OPERAND(scan));
				}
			regmust = longest;
			regmlen = len;
		}
	}

	m_bValid = true;
}

VkRegExp::~VkRegExp () {
}

/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
char *VkRegExp::reg (int paren, int *flagp) {
	REGISTER char *ret;
	REGISTER char *br;
	REGISTER char *ender;
	REGISTER int parno;
	int flags;

	*flagp = HASWIDTH;	/* Tentatively. */

	/* Make an OPEN node, if parenthesized. */
	if (paren) {
		if (regnpar >= Query::NSUBEXP)
			return (NULL);	/* FAIL("too many ()"); */
		parno = regnpar;
		regnpar++;
		ret = regnode((char) (OPEN+parno));
	} else
		ret = NULL;

	/* Pick up the branches, linking them together. */
	br = regbranch(&flags);
	if (br == NULL)
		return(NULL);
	if (ret != NULL)
		regtail(ret, br);	/* OPEN -> first. */
	else
		ret = br;
	if (!(flags&HASWIDTH))
		*flagp &= ~HASWIDTH;
	*flagp |= flags&SPSTART;
	while (*regparse == '|') {
		regparse++;
		br = regbranch(&flags);
		if (br == NULL)
			return(NULL);
		regtail(ret, br);	/* BRANCH -> BRANCH. */
		if (!(flags&HASWIDTH))
			*flagp &= ~HASWIDTH;
		*flagp |= flags&SPSTART;
	}

	/* Make a closing node, and hook it on the end. */
	ender = regnode((char)((paren) ? CLOSE+parno : END));	
	regtail(ret, ender);

	/* Hook the tails of the branches to the closing node. */
	for (br = ret; br != NULL; br = regnext(br))
		regoptail(br, ender);

	/* Check for proper termination. */
	if (paren && *regparse++ != ')') {
		FAIL("unmatched ()");
	} else if (!paren && *regparse != '\0') {
		if (*regparse == ')') {
			FAIL("unmatched ()");
		} else
			FAIL("junk on end");	/* "Can't happen". */
		/* NOTREACHED */
	}

	return(ret);
}

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
char *VkRegExp::regbranch (int *flagp) {
	REGISTER char *ret;
	REGISTER char *chain;
	REGISTER char *latest;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	ret = regnode(BRANCH);
	chain = NULL;
	while (*regparse != '\0' && *regparse != '|' && *regparse != ')') {
		latest = regpiece(&flags);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
		if (chain == NULL)	/* First piece. */
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest);
		chain = latest;
	}
	if (chain == NULL)	/* Loop ran zero times. */
		(void) regnode(NOTHING);

	return(ret);
}

/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
char *VkRegExp::regpiece (int *flagp) {
	REGISTER char *ret;
	REGISTER char op;
	REGISTER char *next;
	int flags;

	ret = regatom(&flags);
	if (ret == NULL)
		return(NULL);

	op = *regparse;
	if (!ISMULT(op)) {
		*flagp = flags;
		return(ret);
	}

	if (!(flags&HASWIDTH) && op != '?')
		FAIL("*+ operand could be empty");
	*flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

	if (op == '*' && (flags&SIMPLE))
		reginsert(STAR, ret);
	else if (op == '*') {
		/* Emit x* as (x&|), where & means "self". */
		reginsert(BRANCH, ret);			/* Either x */
		regoptail(ret, regnode(BACK));		/* and loop */
		regoptail(ret, ret);			/* back */
		regtail(ret, regnode(BRANCH));		/* or */
		regtail(ret, regnode(NOTHING));		/* null. */
	} else if (op == '+' && (flags&SIMPLE))
		reginsert(PLUS, ret);
	else if (op == '+') {
		/* Emit x+ as x(&|), where & means "self". */
		next = regnode(BRANCH);			/* Either */
		regtail(ret, next);
		regtail(regnode(BACK), ret);		/* loop back */
		regtail(next, regnode(BRANCH));		/* or */
		regtail(ret, regnode(NOTHING));		/* null. */
	} else if (op == '?') {
		/* Emit x? as (x|) */
		reginsert(BRANCH, ret);			/* Either x */
		regtail(ret, regnode(BRANCH));		/* or */
		next = regnode(NOTHING);		/* null. */
		regtail(ret, next);
		regoptail(ret, next);
	}
	regparse++;
	if (ISMULT(*regparse))
		FAIL("nested *?+");

	return(ret);
}

/*
 - regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed characters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
char * VkRegExp::regatom (int *flagp) {
	REGISTER char *ret;
	int flags;

	*flagp = WORST;		/* Tentatively. */

	switch (*regparse++) {
	case '^':
		ret = regnode(BOL);
		break;
	case '$':
		ret = regnode(EOL);
		break;
	case '.':
		ret = regnode(ANY);
		*flagp |= HASWIDTH|SIMPLE;
		break;
	case '[': {
			REGISTER int xClass;
			REGISTER int classend;

			if (*regparse == '^') {	/* Complement of range. */
				ret = regnode(ANYBUT);
				regparse++;
			} else
				ret = regnode(ANYOF);
			if (*regparse == ']' || *regparse == '-')
				regc(*regparse++);
			while (*regparse != '\0' && *regparse != ']') {
				if (*regparse == '-') {
					regparse++;
					if (*regparse == ']' || *regparse == '\0')
						regc('-');
					else {
						xClass = UCHARAT(regparse-2)+1;
						classend = UCHARAT(regparse);
						if (xClass > classend+1)
							FAIL("invalid [] range");
						for (; xClass <= classend; xClass++)
							regc((char) xClass);
						regparse++;
					}
				} else
					regc(*regparse++);
			}
			regc('\0');
			if (*regparse != ']')
				FAIL("unmatched []");
			regparse++;
			*flagp |= HASWIDTH|SIMPLE;
		}
		break;
	case '(':
		ret = reg(1, &flags);
		if (ret == NULL)
			return(NULL);
		*flagp |= flags&(HASWIDTH|SPSTART);
		break;
	case '\0':
	case '|':
	case ')':
		FAIL("internal urp");	/* Supposed to be caught earlier. */
		break;
	case '?':
	case '+':
	case '*':
		FAIL("?+* follows nothing");
		break;
	case '\\':
		if (*regparse == '\0')
			FAIL("trailing \\");
		ret = regnode(EXACTLY);
		regc(*regparse++);
		regc('\0');
		*flagp |= HASWIDTH|SIMPLE;
		break;
	default: {
			REGISTER int len;
			REGISTER char ender;

			regparse--;
			len = strcspn(regparse, META);
			if (len <= 0)
				FAIL("internal disaster");
			ender = *(regparse+len);
			if (len > 1 && ISMULT(ender))
				len--;		/* Back off clear of ?+* operand. */
			*flagp |= HASWIDTH;
			if (len == 1)
				*flagp |= SIMPLE;
			ret = regnode(EXACTLY);
			while (len > 0) {
				regc(*regparse++);
				len--;
			}
			regc('\0');
		}
		break;
	}

	return(ret);
}

/*
 - regnode - emit a node
 */
char *VkRegExp::regnode (char op)	/* Location. */
{
	REGISTER char *ret;
	REGISTER char *ptr;

	ret = regcode;
	if (ret == &regdummy) {
		regsize += 3;
		return(ret);
	}

	ptr = ret;
	*ptr++ = op;
	*ptr++ = '\0';		/* Null "next" pointer. */
	*ptr++ = '\0';
	regcode = ptr;

	return(ret);
}

/*
 - regc - emit (if appropriate) a byte of code
 */
void VkRegExp::regc (char b)
{
	if (regcode != &regdummy)
		*regcode++ = b;
	else
		regsize++;
}

/*
 - reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
void VkRegExp::reginsert (char op, char *opnd) {
	REGISTER char *src;
	REGISTER char *dst;
	REGISTER char *place;

	if (regcode == &regdummy) {
		regsize += 3;
		return;
	}

	src = regcode;
	regcode += 3;
	dst = regcode;
	while (src > opnd)
		*--dst = *--src;

	place = opnd;		/* Op node, where operand used to be. */
	*place++ = op;
	*place++ = '\0';
	*place++ = '\0';
}

/*
 - regtail - set the next-pointer at the end of a node chain
 */
void VkRegExp::regtail (char *p, char *val) {
	REGISTER char *scan;
	REGISTER char *temp;
	REGISTER int offset;

	if (p == &regdummy)
		return;

	/* Find last node. */
	scan = p;
	for (;;) {
		temp = regnext(scan);
		if (temp == NULL)
			break;
		scan = temp;
	}

	if (OP(scan) == BACK)
		offset = scan - val;
	else
		offset = val - scan;
	*(scan+1) = (char) ((offset>>8)&0377);
	*(scan+2) = (char) (offset&0377);
}

/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */
void VkRegExp::regoptail (char *p, char *val) {
	/* "Operandless" and "op != BRANCH" are synonymous in practice. */
	if (p == NULL || p == &regdummy || OP(p) != BRANCH)
		return;
	regtail(OPERAND(p), val);
}

/*
 - VkRegExp::Execute - match a regexp against a string
 */

bool VkRegExp::Execute (VString const& rString) const {
    Query iQuery (rString);
    return Execute (iQuery);
}

bool VkRegExp::Execute (Query& rQuery) const {
	/* Check validity of program. */
    if (!isValid() || UCHARAT(program) != MAGIC) {
		return false;	/* FAIL ("corrupted program"); */
	}

	/* If there is a "must appear" string, look for it. */
	if (regmust != NULL) {
		REGISTER char const* s = rQuery.string ();
		while ((s = strchr(s, regmust[0])) != NULL) {
			if (strncmp(s, regmust, regmlen) == 0)
				break;	/* Found it. */
			s++;
		}
		if (s == NULL)	/* Not present. */
			return false;
	}

	/* Simplest case:  anchored match need be tried only once. */
	if (reganch)
	    return regtry(rQuery, rQuery.string ());

	/* Messy cases:  unanchored match. */
	REGISTER char const* s = rQuery.string ();
	if (regstart != '\0')
		/* We know what char it must start with. */
		while ((s = strchr(s, regstart)) != NULL) {
		    if (regtry(rQuery,s))
			return true;
		    s++;
		}
	else
		/* We don't -- general case. */
		do {
		    if (regtry(rQuery, s))
			return true;
		} while (*s++ != '\0');

	/* Failure. */
	return false;
}

/*
 - regtry - try match at specific point
 */
bool			/* false failure, true success */
VkRegExp::regtry (Query& rQuery, char const* string) const {
	REGISTER int i;
	REGISTER char const** sp = rQuery.m_startp;
	REGISTER char const** ep = rQuery.m_endp;

	rQuery.reginput = string;

	for (i = Query::NSUBEXP; i > 0; i--) {
		*sp++ = NULL;
		*ep++ = NULL;
	}
	if (regmatch(rQuery, program + 1)) {
		rQuery.m_startp[0] = string;
		rQuery.m_endp[0] = rQuery.reginput;
		return true;
	} else
		return false;
}

/*
 - regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.
 */
bool			/* false failure, true success */
VkRegExp::regmatch (Query& rQuery, char *prog) const {
	char const **querySP = rQuery.m_startp;	/* Pointer to startp array. */
	char const **queryEP = rQuery.m_endp;	/* Ditto for endp. */

	REGISTER char *scan = prog;	/* Current node. */
	while (scan != NULL) {
	    char *next = regnext(scan);	/* Next node. */
		switch (OP(scan)) {
		case BOL:
			if (rQuery.reginput != rQuery.string ())
				return false;
			break;
		case EOL:
			if (*rQuery.reginput != '\0')
				return false;
			break;
		case ANY:
			if (*rQuery.reginput == '\0')
				return false;
			rQuery.reginput++;
			break;
		case EXACTLY: {
				REGISTER int len;
				REGISTER char *opnd;

				opnd = OPERAND(scan);
				/* Inline the first character, for speed. */
				if (*opnd != *rQuery.reginput)
					return false;
				len = strlen(opnd);
				if (len > 1 && strncmp(opnd, rQuery.reginput, len) != 0)
					return false;
				rQuery.reginput += len;
			}
			break;
		case ANYOF:
 			if (*rQuery.reginput == '\0' || strchr(OPERAND(scan), *rQuery.reginput) == NULL)
				return false;
			rQuery.reginput++;
			break;
		case ANYBUT:
 			if (*rQuery.reginput == '\0' || strchr(OPERAND(scan), *rQuery.reginput) != NULL)
				return false;
			rQuery.reginput++;
			break;
		case NOTHING:
			break;
		case BACK:
			break;
		case OPEN+1:
		case OPEN+2:
		case OPEN+3:
		case OPEN+4:
		case OPEN+5:
		case OPEN+6:
		case OPEN+7:
		case OPEN+8:
		case OPEN+9: {
				REGISTER int no;
				REGISTER char const*save;

				no = OP(scan) - OPEN;
				save = rQuery.reginput;

				if (regmatch(rQuery, next)) {
					/*
					 * Don't set startp if some later
					 * invocation of the same parentheses
					 * already has.
					 */
					if (querySP[no] == NULL)
						querySP[no] = save;
					return true;
				} else
					return false;
			}
			break;
		case CLOSE+1:
		case CLOSE+2:
		case CLOSE+3:
		case CLOSE+4:
		case CLOSE+5:
		case CLOSE+6:
		case CLOSE+7:
		case CLOSE+8:
		case CLOSE+9: {
				REGISTER int no;
				REGISTER char const*save;

				no = OP(scan) - CLOSE;
				save = rQuery.reginput;

				if (regmatch(rQuery, next)) {
					/*
					 * Don't set endp if some later
					 * invocation of the same parentheses
					 * already has.
					 */
					if (queryEP[no] == NULL)
						queryEP[no] = save;
					return true;
				} else
					return false;
			}
			break;
		case BRANCH: {
				REGISTER char const*save;

				if (OP(next) != BRANCH)		/* No choice. */
					next = OPERAND(scan);	/* Avoid recursion. */
				else {
					do {
						save = rQuery.reginput;
						if (regmatch(rQuery, OPERAND(scan)))
							return true;
						rQuery.reginput = save;
						scan = regnext(scan);
					} while (scan != NULL && OP(scan) == BRANCH);
					return false;
					/* NOTREACHED */
				}
			}
			break;
		case STAR:
		case PLUS: {
				REGISTER char nextch;
				REGISTER int no;
				REGISTER char const*save;
				REGISTER int min;

				/*
				 * Lookahead to avoid useless match attempts
				 * when we know what character comes next.
				 */
				nextch = '\0';
				if (OP(next) == EXACTLY)
					nextch = *OPERAND(next);
				min = (OP(scan) == STAR) ? 0 : 1;
				save = rQuery.reginput;
				no = regrepeat(rQuery, OPERAND(scan));
				while (no >= min) {
					/* If it could work, try it. */
					if (nextch == '\0' || *rQuery.reginput == nextch)
					    if (regmatch(rQuery, next))
						return true;
					/* Couldn't or didn't -- back up. */
					no--;
					rQuery.reginput = save + no;
				}
				return false;
			}
			break;
		case END:
			return true;	/* Success! */

		default:
			return false;	/* FAIL ("memory corruption"); */

		}

		scan = next;
	}

	/*
	 * We get here only if there's trouble -- normally "case END" is
	 * the terminating point.
	 */
	return false;	/* FAIL ("corrupted pointers"); */
}

/*
 - regrepeat - repeatedly match something simple, report how many
 */
int VkRegExp::regrepeat (Query& rQuery, char *p) const {
	REGISTER int count = 0;

	REGISTER char const *scan = rQuery.reginput;
	REGISTER char *opnd = OPERAND(p);
	switch (OP(p)) {
	case ANY:
		count = strlen(scan);
		scan += count;
		break;
	case EXACTLY:
		while (*opnd == *scan) {
			count++;
			scan++;
		}
		break;
	case ANYOF:
		while (*scan != '\0' && strchr(opnd, *scan) != NULL) {
			count++;
			scan++;
		}
		break;
	case ANYBUT:
		while (*scan != '\0' && strchr(opnd, *scan) == NULL) {
			count++;
			scan++;
		}
		break;
	default:		/* Oh dear.  Called inappropriately. */
		/* regerror("internal foulup"); */
		count = 0;	/* Best compromise. */
		break;
	}
	rQuery.reginput = scan;

	return(count);
}

/*
 - regnext - dig the "next" pointer out of a node
 */
char *VkRegExp::regnext (REGISTER char *p) const {
	REGISTER int offset;

	if (p == &regdummy)
		return(NULL);

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return(p-offset);
	else
		return(p+offset);
}


/*****************************
 *****************************
 *****                   *****
 *****  VkRegExp::Query  *****
 *****                   *****
 *****************************
 *****************************/

VkRegExp::Query::Query (VString const& rString) : m_iString (rString) {
}

VkRegExp::Query::~Query () {
}

bool VkRegExp::Query::getMatch (VString& rResult, unsigned int xMatch) const {
    if (xMatch >= NSUBEXP || !m_startp[xMatch] || !m_endp[xMatch])
	return false;

    rResult.setTo (m_startp[xMatch], m_endp[xMatch] - m_startp[xMatch]);

    return true;
}

bool VkRegExp::Query::getMatch (char const*& rpStart, char const*& rpEnd, unsigned int xMatch) const {
    if (xMatch >= NSUBEXP || !m_startp[xMatch] || !m_endp[xMatch])
	return false;

    rpStart = m_startp[xMatch];
    rpEnd = m_endp[xMatch];
    return true;
}

bool VkRegExp::Query::getMatch (unsigned int& rxStart, unsigned int& rxEnd, unsigned int xMatch) const {
    if (xMatch >= NSUBEXP || !m_startp[xMatch] || !m_endp[xMatch])
	return false;

    rxStart = m_startp[xMatch] - m_iString.content ();
    rxEnd = m_endp[xMatch] - m_startp[xMatch];
    return true;
}
