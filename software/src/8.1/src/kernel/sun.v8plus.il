!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!  xarch=v8plus Inline Assembly Templates !!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! 	__int32 compareAndSwap32 (
!!!!	    __int32 volatile*	pValue		! ... register %o0
!!!!	    __int32		iExpected	! ... register %o1
!!!!	    __int32		iReplacement	! ... register %o2
!!!!	)
!!!!
!!!!    Returns the original *pValue.		! ... register %o0.
!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.inline compareAndSwap32, 12
	cas	[%o0], %o1, %o2	! ... atomic compare and swap (%o2 = old [%o0], [%0] = old %o2 iff old [%o0] == %o1).
	mov	%o2, %o0	! ... returning the original value.
.end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! 	__int64 compareAndSwap64 (
!!!!	    __int64 volatile*	pValue		! ... register %o0
!!!!	    __int64		iExpected	! ... register pair %o1:%o2 as %o1 * 2^32 + %o2
!!!!	    __int64		iReplacement	! ... register pair %o3:%o4 as %o3 * 2^32 + %o4
!!!!	)
!!!!
!!!!    Returns the original *pValue.		! ... register pair %o0:%o1 as %o0 * 2^32 + %o1.
!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.inline compareAndSwap64, 20
	sllx	%o1,  32, %o1	! ... make a 64-bit integer in %o1 containing iExpected.
	add	%o1, %o2, %o1	! 
	sllx	%o3,  32, %o2	! ... make a 64-bit integer in %o2 containing iReplacement.
	add	%o2, %o4, %o2	!
	casx	[%o0], %o1, %o2	! ... atomic compare and swap (%o2 = old [%o0], [%0] = old %o2 iff old [%o0] == %o1).
	sllx	%o2, 32, %o1	! ... returning the original value ...
	srlx	%o1, 32, %o1	! ... with %o1 = result % (2^32)
	srlx	%o2, 32, %o0	! ... and  %o0 = result / (2^32)
.end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! 	__int32 fetchAndAdd32 (
!!!!	    __int32 volatile&	rValue		! ... register %o0
!!!!	    __int32		iAddend		! ... register %o1
!!!!	)
!!!!
!!!!	Returns initial value in register %o0.
!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.inline fetchAndAdd32, 8
1:
	ld	[%o0], %o2	! %o2 = rValue
	add	%o2, %o1, %o3	! %o3 = rValue + iAddend
	cas	[%o0], %o2, %o3	! atomic compare and swap [%o0] and %o3 iff [%o0] == %o2
	cmp	%o2, %o3	! value set?
	bne	1b		! no? try again
	nop
	mov	%o3, %o0	! yes? return initial value
.end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! 	__int64 fetchAndAdd64 (
!!!!	    __int64 volatile&	rValue		! ... register %o0
!!!!	    __int64		iAddend		! ... register pair %o1:%o2 as %o1 * 2^32 + %o2.
!!!!	)
!!!!
!!!!	Returns initial value in register pair %o0:%o2 as %o0 * 2^32 + %o1
!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.inline fetchAndAdd64, 12
	sllx	%o1,  32, %o1	! make a single single 64 bit integer from low order 32-bits of %o1 and %o2 in %o1:
	add	%o1, %o2, %o1	! %o1 = iAddend[bits 63...0] (one 64-bit integer)
1:
	ldx	[%o0], %o2	! %o2 = rValue
	add	%o2, %o1, %o3	! %o3 = rValue + iAddend
	casx	[%o0], %o2, %o3	! atomic compare and swap [%o0] and %o3 iff [%o0] == %o2
	cmp	%o2, %o3	! value set?
	bne	1b		! no? try again
	nop			! yes? return inital value in register pair %o0:%o1, ...
	sllx	%o3, 32, %o1
	srlx	%o1, 32, %o1	! ... with %o1 = initialValue % (2^32)
	srlx	%o3, 32, %o0	! ...  and %o0 = initialValue / (2^32)
.end

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!
!!!!    void membar            ();  // #LoadLoad  | #LoadStore  | #StoreLoad | #StoreStore
!!!!    void membar_acquire    ();  // #LoadLoad  | #LoadStore
!!!!    void membar_release    ();  // #LoadStore | #StoreStore
!!!!    void membar_produce    ();  // #StoreStore
!!!!    void membar_consume    ();  // #LoadLoad
!!!!    void membar_loadload   ();
!!!!    void membar_loadstore  ();
!!!!    void membar_storeload  ();
!!!!    void membar_storestore ();
!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.inline membar, 0
	membar	#LoadLoad  | #LoadStore  | #StoreLoad | #StoreStore
.end

.inline membar_acquire, 0
	membar	#LoadLoad  | #LoadStore
.end

.inline membar_release, 0
	membar	#LoadStore | #StoreStore
.end

.inline membar_produce, 0
	membar	#StoreStore
.end

.inline membar_consume, 0
	membar	#LoadLoad
.end

.inline membar_loadload, 0
	membar	#LoadLoad
.end

.inline membar_loadstore, 0
	membar	#LoadStore
.end

.inline membar_storeload, 0
	membar	#StoreLoad
.end

.inline membar_storestore, 0
	membar	#StoreStore
.end
