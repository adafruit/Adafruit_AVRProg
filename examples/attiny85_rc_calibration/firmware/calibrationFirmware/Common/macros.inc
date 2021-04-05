;*****************************************************************
;*
;* - Description:  Source file from application note AVR001. 
;*                 Defines a number of macros that makes it easier
;*                 to access IO registers (and SRAM locations). 
;*
;* - File:         macros.inc
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 56 $
;* $RCSfile$
;* $Date: 2006-02-16 17:44:45 +0100 (to, 16 feb 2006) $
;*****************************************************************


;*********************************************************
;*	BIT access anywhere in IO or data space
;*	SETB - SET Bit in IO of data space
;*	CLRB - CLeaR Bit in IO of data space
;*********************************************************

.MACRO SETB 		;Arguments: Address, Bit, Register
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if	@0>0x3F
		lds	@2, @0
		sbr	@2, (1<<@1)
		sts	@0, @2
	.else
		.if @0>0x1F
			in	@2, @0
			sbr	@2, (1<<@1)
			out	@0, @2
		.else
			sbi	@0, @1
		.endif
	.endif
.ENDMACRO

.MACRO CLRB 		;Arguments: Address, Bit, Register
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if	@0>0x3F
		lds	@2, @0
		cbr	@2, (1<<@1)
		sts	@0, @2
	.else
		.if @0>0x1F
			in	@2, @0
			cbr	@2, (1<<@1)
			out	@0, @2
		.else
			cbi	@0, @1
		.endif
	.endif
.ENDMACRO

;*********************************************************
;*	Bit test anywhere in IO or data space
;*  SKBS : SKip if Bit Set
;*  SKBC : SKip if Bit Cleared
;*********************************************************
.MACRO SKBS  		;Arguments: Address, Bit, Register
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if	@0>0x3F
		lds		@2, @0
		sbrs	@2, @1
	.else
		.if @0>0x1F
			in		@2, @0
			sbrs	@2, @1
		.else
			sbis	@0, @1
		.endif
	.endif
.ENDMACRO

.MACRO SKBC  		;Arguments: Address, Bit, Register
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if	@0>0x3F
		lds		@2, @0
		sbrc	@2, @1
	.else
		.if @0>0x1F
			in		@2, @0
			sbrc	@2, @1
		.else
			sbic	@0, @1
		.endif
	.endif
.ENDMACRO

;*********************************************************
;*	Byte access anywhere in IO or data space
;* 	STORE - Store register in IO or data space
;* 	LOAD  - Load register from IO or data space
;*********************************************************

.MACRO STORE 		;Arguments: Address, Register
	.if	@0>0x3F
		sts	@0, @1
	.else
		out	@0, @1
	.endif
.ENDMACRO

.MACRO LOAD 		;Arguments: Register, Address
	.if	@1>0x3F
		lds	@0, @1
	.else
		in	@0, @1
	.endif
.ENDMACRO
