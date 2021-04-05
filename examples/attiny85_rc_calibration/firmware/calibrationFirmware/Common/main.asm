;*****************************************************************
;*
;* - Description:  Code is part of the RC_Calibration.asm source
;*
;* - File:         main.asm
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 3901 $
;* $RCSfile$
;* $Date: 2008-04-30 14:31:21 +0200 (on, 30 apr 2008) $
;*****************************************************************




;*****************************************************************
;* Definitions
;*****************************************************************
;Register assignments
.def	Zero		= r0
.def	One			= r1
.def	DetectCountL	= r2
.def	DetectCountH	= r3
.def	OVF_flag_clr	= r4
.def	temp		= r16
.def	temp1		= r17
.def	temp2		= r18
.def	temp3		= r19
.def	funcDataPass	= r20
.def	oscStepSize		= r21
.def	oscTrialValue	= r22
.def	default_OSCCAL = r23
.def	calibPassesLeft		= r24

;Definition of port and pins
.equ	PRGR_HANDSHAKE_PIN	= MISO
.equ	REF_FREQ_PIN		= MOSI
.equ	ISP_PIN_REG		    = CAL_PIN
.equ	ISP_PORT_REG		= CAL_PORT
.equ	ISP_DDR_REG			= CAL_DDR

;Definition of calibration constants
.equ	CCYCLES		= 40		;Number of C-cycles used as calibration period (a decrease is not recommended, increase with care)
.equ	TIMING_ERROR	= 2		;Edge detection can be up to 2 CPU cycles delayed
.equ	BEST_FIT	= ((((TARGET_FREQ*CCYCLES)*2) /CALIB_CLOCK_FREQ )+1)/2	;Number of CPU cycles in CCYCLES calibration cycles (rounded - worst case rounding error is 0.045%)
.equ	LIMIT		= (BEST_FIT*ACCURACY)/1000							;Max allowed CPU cycles off best fit. Truncated to not introduce additional error
.equ 	HIGH_LIMIT	=  BEST_FIT + LIMIT - TIMING_ERROR	;CPU cycles count limit, corrected for worstcase timing error
.equ 	LOW_LIMIT	=  BEST_FIT - LIMIT + TIMING_ERROR	;CPU cycles count limit, corrected for worstcase timing error

.equ	FREQ_WITHIN_LIM = 0x00	;Value = 0
.equ	TOO_FAST		= 0x01	;Value = 1
.equ	TOO_SLOW		= 0xFF	;Value =-1

; Constant depending on Oscillator version
.ifndef OSC_VER
	.error " Oscillator version not defined"
.endif
.ifdef OSC_VER
	.if OSC_VER == 4
		.equ	INITIAL_STEP_SIZE	= 0x40	;OSCCAL tuning range is 7-bit
		.equ	NUM_CALIB_PASSES	= 1		;Number of calibration passes
	.elif OSC_VER == 5
		.equ	INITIAL_STEP_SIZE	= 0X40	;OSCCAL tuning range is 7 bit * 2
		.equ	NUM_CALIB_PASSES	= 2		;Number of calibration passes.
	.else
		.equ	INITIAL_STEP_SIZE	= 0x80	;OSCCAL tuning range is 8-bit
		.equ	NUM_CALIB_PASSES	= 1		;Number of calibration passes.
	.endif
.endif
;*****************************************************************
;* Vector table
;*****************************************************************
.CSEG
.org ADDR_START
	rjmp	Reset

;*****************************************************************
;* Main code
;*****************************************************************
.CSEG 
.org ADDR_MAIN

Reset:
	clr		Zero
	ldi		temp, 0x01
	mov		One, temp

Init_Stack:
;If the device uses SRAM stack, init it, otherwise don't
.ifdef	SP
	.equ	SPL = SP
.endif
.ifdef SPL
	ldi		temp, low (RAMEND)
	out		SPL, temp
	.ifdef SPH
		ldi		temp, high(RAMEND)
		out		SPH, temp
	.endif
.endif

Init_interface:
;In case the JTAG pins are used the JTAG interface must be disabled first
.ifdef JTAG_CALIB
	ldi	temp, 1<<JTD
	out	MCUCSR, temp
	out	MCUCSR, temp
.endif	

Init_IO:
	SETB	ISP_PORT_REG, REF_FREQ_PIN, temp		;Enable pull-up
	SETB	ISP_PORT_REG, PRGR_HANDSHAKE_PIN, temp	;Enable pull-up / output high
	SETB	ISP_DDR_REG,  PRGR_HANDSHAKE_PIN, temp	;Configure handshake IO direction


CalibrationBegin:
	rcall	newCalibrateRc

	ldi		temp, high(EEPROM_ADDRESS)
	out		EEARH, temp
	ldi		temp, low (EEPROM_ADDRESS)
	out		EEARL, temp
	
	LOAD	temp, OSCCAL
	out		EEDR, temp

	ldi		temp, 1<<EEMWE
	out		EECR, temp
	ldi		temp, (1<<EEMWE) | (1<<EEWE)
	out		EECR, temp
	nop

WaitForEepromReady:
	sbic	EECR,EEWE
	rjmp	WaitForEepromReady

;------------------------------------------
; "Calibration-OK" Handshaking
;------------------------------------------

.equ	HANDSHAKE_CYCLES	= 8


CalibSuccessful:
	ldi		temp1, HANDSHAKE_CYCLES	;Number of togglings in OK-handshake

;	SETB	ISP_PORT_REG, PRGR_HANDSHAKE_PIN, temp		;Configure handshake port
;	SETB	ISP_DDR_REG, PRGR_HANDSHAKE_PIN, temp		;Configure handshake IO direction

	ldi		temp, (1<<PRGR_HANDSHAKE_PIN)				;Init for later use to toggle pin

CCS_WaitForPinHigh:
	in		temp2, ISP_PIN_REG
	sbrs	temp2, REF_FREQ_PIN
	rjmp	CCS_WaitForPinHigh

	LOAD	temp3, ISP_PORT_REG
 	eor		temp3, temp
CCS_WaitForPinLow:
	in		temp2, ISP_PIN_REG
	sbrc	temp2, REF_FREQ_PIN
	rjmp	CCS_WaitForPinLow							;Synchronize to C-clock
 	STORE	ISP_PORT_REG, temp3
 
Handshake_loop:
	dec	temp1
	brne	CCS_WaitForPinHigh
	nop

StopHere:
;In case the JTAG pins are used the JTAG interface must be reenabled again
.ifdef JTAG_CALIB
	ldi	temp, ~(1<<JTD)
	out	MCUCSR, temp
	out	MCUCSR, temp
.endif	

;To test the frequency of the device enable the test code below
; 	SBI_GEN 	ISP_DDR_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" high
;	SBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	SBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
;	CBI_GEN 	ISP_PORT_REG,PRGR_HANDSHAKE_PIN, temp	;Drive "MISO" low
EOP:
	rjmp 	EOP             ;Finally, wait here until ISP programmer reprograms the device
 
;*****************************************************************
;* Calibration function:
;*
;* Make a binary search to find the best OSCCAL value. If the result
;* of the binary search is not within the frequency limits the 4 
;* nearest OSCCAL values are tested to find match.
;*
;*****************************************************************
newCalibrateRc:
	clr	oscTrialValue
	ldi calibPassesLeft, NUM_CALIB_PASSES - 1

	; Temporary fix of JTAGICEmkII clock generation bug
	INTERFACE_BUGFIX

BinSearch:
	ldi		oscStepSize, INITIAL_STEP_SIZE	; MSB of OSCCAL tuning range set (128 or 64 depending on osc version)
	add		oscTrialValue, oscStepSize
BinSearchLoop:
	STORE	OSCCAL, oscTrialValue	;Device specific macro used due to device differences
	nop
	nop
	rcall	DetermineClockSpeed
	cpi		funcDataPass, FREQ_WITHIN_LIM		;If freq OK
	breq	FreqHit								; then end binary search
	lsr		oscStepSize							; ELSE continue search
	tst		oscStepSize

	breq	TryNeighbourghhoodValues				;If oscStepSize is 0 (and no hit is made), try neighbourgh values
	mov		temp,oscStepSize
	cpi		funcDataPass, TOO_FAST
	brne	AddStep
NegateNextStep:
	neg		temp		
AddStep:
	add		oscTrialValue, temp
	rjmp	BinSearchLoop

TryNeighbourghhoodValues:
;test the nearest 3 OSCCAL values to find match (don't test the one just tested)
; One-step-away neighbor
	cpi		funcDataPass, TOO_FAST
	breq	DownstairsNeighbor
	ldi		oscStepSize, 1
	rjmp	AddOscStep
DownstairsNeighbor:
	ldi		oscStepSize, -1
AddOscStep:
	add		oscTrialValue, oscStepSize
	STORE	OSCCAL, oscTrialValue	;Device specific macro used due to device differences
	nop
	nop
	rcall	DetermineClockSpeed
	cpi		funcDataPass, FREQ_WITHIN_LIM		;If freq OK
	breq	FreqHit								; then end binary search
;Closest two-step-away neighbor (in same direction as previous step)
	add		oscTrialValue, oscStepSize
	STORE	OSCCAL, oscTrialValue	;Device specific macro used due to device differences
	nop
	nop
	rcall	DetermineClockSpeed
	cpi		funcDataPass, FREQ_WITHIN_LIM		;If freq OK
	breq	FreqHit								; then end binary search
;Other two-step-away neighbor (4 steps in the opposire direction)
	neg		oscStepSize		;Negate the step (and thereby the direction)
	lsl		oscStepSize		;"Multiply" by 2
	lsl		oscStepSize		;"Multiply" by 2 - in total 4
	add		oscTrialValue, oscStepSize
	STORE	OSCCAL, oscTrialValue	;Device specific macro used due to device differences
	nop
	nop
	rcall	DetermineClockSpeed
	cpi		funcDataPass, FREQ_WITHIN_LIM		;If freq OK
	breq	FreqHit								; then end binary search												
												
CalibPassCompleted:							;One calibration pass is complete, check if one more pass in upper half of OSCCAL should be performed
	tst 	calibPassesLeft
	breq	CalibrationFailure				;If all calibration passes are complete, and no OSCCAL value is found, the calibration failed
	dec		calibPassesLeft
	ldi		oscTrialValue, 0x80
	rjmp 	BinSearch

CalibrationFailure:
	SETB	ISP_DDR_REG,PRGR_HANDSHAKE_PIN, temp		;Drive the MISO line low to signal failure
	rjmp	StopHere							;jmp to the end of the calibration code
		
FreqHit:
	mov		funcDataPass, oscTrialValue			;Pass on the final OSCCAL value
	ret

;*****************************************************************
;* TestClock function:
;*
;* Uses TimerCounter0 and OVF to time 40 calibration clock cycles.
;* Returns the result (TOO_FAST, TOO_SLOW) in funcDataPass
;*****************************************************************
DetermineClockSpeed:

Prepare_Detection:
	clr		detectCountH
	ldi		temp, (1<<TOV0)
	mov		OVF_flag_clr, temp
	ldi 	temp, (CCYCLES - 2)
	out		TCNT0, Zero
	ldi		temp1, (0<<CS02) | (0<<CS01) | (1<<CS00)	;Warning - this depend on sys clock
	;	out		TCCR0, temp

WaitForPinHigh:
	SKBS	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	WaitForPinHigh		; Will not get here before REF_FREQ_PIN starts toggling

WaitForPinLow:
	SKBC	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	WaitForPinLow		; Will not get here before REF_FREQ_PIN starts toggling
	out		TCCR0,temp1

; ---- cycle CC01 ----
DetectHighCC01:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectHighLoopCC01
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectHighLoopCC01:
	SKBS	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectHighLoopCC01

DetectLowCC01:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectLowLoopCC01
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectLowLoopCC01:
	SKBC	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectLowLoopCC01

; ---- cycle CC02 to CC39 implemented as loop----
DetectHighCCxx:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectHighLoopCCxx
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectHighLoopCCxx:
	SKBS	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectHighLoopCCxx

DetectLowCCxx:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectLowLoopCCxx
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectLowLoopCCxx:
	SKBC	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectLowLoopCCxx

	dec		temp
	brne	DetectHighCCxx

; ---- cycle CC40 ----
DetectHighCC40:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectHighLoopCC40
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectHighLoopCC40:
	SKBS	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectHighLoopCC40

DetectLowCC40:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	DetectLowLoopCC40
	add		detectCountH, One
	out		TIFR, OVF_flag_clr
DetectLowLoopCC40:
	SKBC	ISP_PIN_REG, REF_FREQ_PIN, temp3
	rjmp	DetectLowLoopCC40

Detect_End:
	out		TCCR0, Zero		;Stop timer
CountLastOVF:
	in		temp2, TIFR
	sbrs	temp2, TOV0
	rjmp	MergeTimerHighAndLow
	sec
	adc		detectCountH, Zero
	andi	temp2, (1<<TOV0)
	out		TIFR, temp2	
MergeTimerHighAndLow:
	in		detectCountL, TCNT0

CompareToLimits:
	ldi		temp1, low(HIGH_LIMIT)
	ldi		temp2, high(HIGH_LIMIT)
	cp		detectCountL, temp1
	cpc		detectCountH, temp2
	brge   	TooFast				;branch if detect >= HIGH_LIMIT (system clock too fast, since count is too high)

	ldi		temp1, low(LOW_LIMIT)
	ldi		temp2, high(LOW_LIMIT)
	cp		temp1,detectCountL
	cpc		temp2, detectCountH
	brge   	TooSlow				;branch if detect <= LOW_LIMIT (system clock too slow, since count is too low)
RightInTheEye:
	ldi		funcDataPass, FREQ_WITHIN_LIM
	ret
TooFast:
	ldi		funcDataPass, TOO_FAST
	ret
TooSlow:
	ldi		funcDataPass, TOO_SLOW	;TOO_SLOW is a value and not a bitmask -> funcDataPass = 0x00
	ret
