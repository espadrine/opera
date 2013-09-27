// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
// Copyright (c) 2013 Opera Software ASA. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

.arch armv6			// Support ARMv6 instructions
.syntax unified		// Allow both ARM and Thumb instructions
.text

// Note: This code assumes that the machine is configured for little endian
// format.

// Note that only 4 arguments are allowed. For more, set up a stack frame after the 4th.
.macro FUNC_PROLOGUE_ARM func_name
	.align 6						// Align the code to a 64 (2^6) byte boundary.
	.arm							// Use ARM instruction set.
#ifdef __ELF__
	.globl \func_name				// extern "C" name.
	.hidden \func_name				// don't export from .so files.
\func_name\():						// Function address.
#else
	.globl _\func_name				// extern "C" name.
_\func_name\():						// Function address.
#endif
.endm

.macro FUNC_EPILOGUE
	bx			lr					// Return to caller.
.endm


// ConvRGBA - Convert a pixel in place from RGBA to BGRA (or vice versa)
//
// Parameters:
//  reg - source and destination register
//
// Input:
//  r11 - must be set to 0x00ff00ff
//
// Output:
//  reg - result
//
// Side effects:
//  r12
.macro ConvRGBA reg
	and				r12, \reg, r11				// r12 = (..BB..RR)
	bic				\reg, \reg, r11				// reg = (AA..GG..)
	orr				\reg, \reg, r12, lsr #16	// reg = (AA..GGBB)
	orr				\reg, \reg, r12, asl #16	// reg = (AARRGGBB)
.endm

// ConvRGBA2 - Convert two pixels from RGBA to BGRA (or vice versa)
//
// Parameters:
//  reg1 - source and destination register 1
//  reg2 - source and destination register 2
//
// Input:
//  r11 - must be set to 0x00ff00ff
//
// Output:
//  reg1 - result for sample 1
//  reg2 - result for sample 2
//
// Side effects:
//  r12, r14
.macro ConvRGBA2 reg1 reg2
	and				r12, \reg1, r11				// r12  = (..BB..RR)
	and				r14, \reg2, r11				// r14  = (..BB..RR)
	bic				\reg1, \reg1, r11			// reg1 = (AA..GG..)
	bic				\reg2, \reg2, r11			// reg2 = (AA..GG..)
	orr				\reg1, \reg1, r12, lsr #16	// reg1 = (AA..GGBB)
	orr				\reg2, \reg2, r14, lsr #16	// reg2 = (AA..GGBB)
	orr				\reg1, \reg1, r12, asl #16	// reg1 = (AARRGGBB)
	orr				\reg2, \reg2, r14, asl #16	// reg2 = (AARRGGBB)
.endm


//------------------------------------------------------------------------------
// SwizzleColors()
//
// Description:
//  Swizzle the color components of all pixels in a row.
//
// Signature:
//  void SwizzleColors(void* row, unsigned num);
//   r0 = row
//   r1 = num
//------------------------------------------------------------------------------
FUNC_PROLOGUE_ARM SwizzleColors
	push			{r4-r11,r14}

	// Load constant mask
	mov				r11, #0xff
	orr				r11, r11, r11, asl #16		// r11 = 0x00ff00ff

	// Align to a cache line boundary
	cmp				r1, #0
sc_head_loop:
	beq				sc_done						// If num == 0...
	ands			r12, r0, #31				// Are we aligned yet?
	beq				sc_aligned
	ldr				r3, [r0]					// r3 = *row (AABBGGRR)
	subs			r1, #1						// num--
	ConvRGBA		r3
	str				r3, [r0], #4				// *row++ = r3 (AARRGGBB)
	b				sc_head_loop

sc_aligned:
	subs			r1, r1, #8
	bcc				sc_tail

	// Preload source data
	pld				[r0, #16*4]
sc_main_loop:

	ldm				r0, {r3-r10}				// Load row0..row7 (AABBGGRR)
	subs			r1, r1, #8

	ConvRGBA2		r3, r4

	// Preload source data
	pld				[r0, #16*4]

	ConvRGBA2		r5, r6
	ConvRGBA2		r7, r8
	ConvRGBA2		r9, r10

	stmia			r0!, {r3-r10}				// Store to row0..row7 (AARRGGBB)
	bcs				sc_main_loop

sc_tail:
	adds			r1, r1, #8
	beq				sc_done

sc_tail_loop:
	ldr				r3, [r0]					// r3 = *row++ (AABBGGRR)
	subs			r1, #1						// num--
	ConvRGBA		r3
	str				r3, [r0], #4				// *row++ = r3 (AARRGGBB)
	bne				sc_tail_loop

sc_done:
	pop				{r4-r11,r14}
FUNC_EPILOGUE


//------------------------------------------------------------------------------
// SwapRows()
//
// Description:
//  Swap two pixel rows.
//
// Signature:
//  void SwapRows(void* rowA, void* rowB, unsigned num);
//   r0 = rowA
//   r1 = rowB
//   r2 = num
//------------------------------------------------------------------------------
FUNC_PROLOGUE_ARM SwapRows
	push			{r4-r10}

	// Align rowA to a cache line boundary (might not help much?)
	cmp				r2, #0
sr_head_loop:
	beq				sr_done						// If num == 0...
	ands			r12, r0, #31				// Are we aligned yet?
	beq				sr_aligned
	ldr				r3, [r0]					// r3 = *rowA
	ldr				r4, [r1]					// r4 = *rowB
	subs			r2, #1						// num--
	str				r3, [r1], #4				// *rowB++ = r3
	str				r4, [r0], #4				// *rowA++ = r4
	b				sr_head_loop

sr_aligned:
	subs			r2, r2, #8
	bcc				sr_tail

	pld				[r0, #16*4]					// Preload data
	pld				[r1, #16*4]
sr_main_loop:

	// Swap four pixels
	ldm				r0, {r3-r6}					// Load rowA0..rowA3
	pld				[r0, #(16+8)*4]
	ldm				r1, {r7-r10}				// Load rowB0..rowB3
	pld				[r1, #(16+8)*4]
	stmia			r0!, {r7-r10}				// Store to rowA0..rowA3
	stmia			r1!, {r3-r6}				// Store to rowB0..rowB3

	// Swap four pixels
	ldm				r0, {r3-r6}					// Load rowA4..rowA7
	ldm				r1, {r7-r10}				// Load rowB4..rowB7
	subs			r2, r2, #8					// num -= 8
	stmia			r0!, {r7-r10}				// Store to rowA4..rowA7
	stmia			r1!, {r3-r6}				// Store to rowB4..rowB7

	bcs				sr_main_loop

sr_tail:
	adds			r2, r2, #8
	beq				sr_done

sr_tail_loop:
	ldr				r3, [r0]					// r3 = *rowA
	ldr				r4, [r1]					// r4 = *rowB
	subs			r2, #1						// num--
	str				r3, [r1], #4				// *rowB++ = r3
	str				r4, [r0], #4				// *rowA++ = r4
	bne				sr_tail_loop

sr_done:
	pop				{r4-r10}
FUNC_EPILOGUE


//------------------------------------------------------------------------------
// SwapRowsAndSwizzle()
//
// Description:
//  Swap two pixel rows, and swizzle color format of each pixel in the process.
//
// Signature:
//  void SwapRowsAndSwizzle(void* rowA, void* rowB, unsigned num);
//   r0 = rowA
//   r1 = rowB
//   r2 = num
//------------------------------------------------------------------------------
FUNC_PROLOGUE_ARM SwapRowsAndSwizzle
	push			{r4-r11,r14}

	// Load constant mask
	mov				r11, #0xff
	orr				r11, r11, r11, asl #16		// r11 = 0x00ff00ff

	// Align rowA to a cache line boundary (might not help much?)
	cmp				r2, #0
srs_head_loop:
	beq				srs_done					// If cnt == 0...
	ands			r12, r0, #31				// Are we aligned yet?
	beq				srs_aligned
	ldr				r3, [r0]					// r3 = *rowA (AABBGGRR)
	ldr				r4, [r1]					// r4 = *rowB (AABBGGRR)
	subs			r2, #1						// num--
	ConvRGBA2		r3, r4
	str				r3, [r1], #4				// *rowB++ = r3 (AARRGGBB)
	str				r4, [r0], #4				// *rowA++ = r4 (AARRGGBB)
	b				srs_head_loop

srs_aligned:
	subs			r2, r2, #8
	bcc				srs_tail

	pld				[r0, #16*4]					// Preload data
	pld				[r1, #16*4]
srs_main_loop:

	// Convert & swap four pixels
	ldm				r0, {r3-r6}					// Load rowA0..rowA3 (AABBGGRR)
	pld				[r0, #(16+8)*4]
	ldm				r1, {r7-r10}				// Load rowB0..rowB3 (AABBGGRR)

	ConvRGBA2		r3, r4
	pld				[r1, #(16+8)*4]
	ConvRGBA2		r7, r8
	ConvRGBA2		r9, r10
	ConvRGBA2		r5, r6

	stmia			r0!, {r7-r10}				// Store to rowA0..rowA3 (AARRGGBB)
	stmia			r1!, {r3-r6}				// Store to rowB0..rowB3 (AARRGGBB)

	// Convert & swap four pixels
	ldm				r0, {r3-r6}					// Load rowA4..rowA7 (AABBGGRR)
	ldm				r1, {r7-r10}				// Load rowB4..rowB7 (AABBGGRR)

	ConvRGBA2		r3, r4
	subs			r2, r2, #8					// num -= 8
	ConvRGBA2		r7, r8
	ConvRGBA2		r9, r10
	ConvRGBA2		r5, r6

	stmia			r0!, {r7-r10}				// Store to rowA4..rowA7 (AARRGGBB)
	stmia			r1!, {r3-r6}				// Store to rowB4..rowB7 (AARRGGBB)

	bcs				srs_main_loop

srs_tail:
	adds			r2, r2, #8
	beq				srs_done

srs_tail_loop:
	ldr				r3, [r0]					// r3 = *rowA (AABBGGRR)
	ldr				r4, [r1]					// r4 = *rowB (AABBGGRR)
	subs			r2, #1						// num--
	ConvRGBA2		r3, r4
	str				r3, [r1], #4				// *rowB++ = r3 (AARRGGBB)
	str				r4, [r0], #4				// *rowA++ = r4 (AARRGGBB)
	bne				srs_tail_loop

srs_done:
	pop				{r4-r11,r14}
FUNC_EPILOGUE

