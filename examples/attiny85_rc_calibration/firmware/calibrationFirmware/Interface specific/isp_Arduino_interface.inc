;*****************************************************************
;*
;* - Description:  AVRISP mkII Interface definition file for 
;*                 RC Calibration
;*
;* - File:         isp_AVRISP_mkII_interface.inc
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
;*	Specify Calibration clock frequency
;*****************************************************************
.EQU	CALIB_CLOCK_FREQ	= 32787	;Calibration Clock frequency in Hz
									;16MHz/(2*244) CHANGE WITH CARE???

;*****************************************************************
;*	Pin and port definitions
;*****************************************************************
.EQU	MISO 		= ISP_CAL_MISO
.EQU	MOSI		= ISP_CAL_MOSI

.EQU	CAL_PORT	= ISP_CAL_PORT_REG
.EQU	CAL_DDR		= ISP_CAL_DDR_REG
.EQU	CAL_PIN		= ISP_CAL_PIN_REG

;*****************************************************************
;*	Macro used to disable and enable JTAG functions as these 
;*	override IO functions. Macro contains nop if not using JTAG.
;*****************************************************************
.MACRO	OPEN_INTERFACE_ACCESS
	nop
.ENDMACRO

.MACRO	CLOSE_INTERFACE_ACCESS
	nop
.ENDMACRO

.MACRO	INTERFACE_BUGFIX
	nop
.ENDMACRO
