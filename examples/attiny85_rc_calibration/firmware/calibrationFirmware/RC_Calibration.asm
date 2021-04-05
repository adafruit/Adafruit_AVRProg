;*****************************************************************
;*
;* - Description:  Code is capable of calibrating the RC 
;*                 oscillator of an AVR with tunable oscillator 
;*                 and ISP or JTAG interface.
;*
;*
;* - File:         RC_Calibration.asm
;* - AppNote:      AVR053 - Production calibration of the
;*                          RC oscillator
;*
;* - Author:       Atmel Corporation: http://www.atmel.com
;*                 Support email: avr@atmel.com
;*
;* $Name$
;* $Revision: 61 $
;* $RCSfile$
;* $Date: 2006-03-23 11:14:13 +0100 (to, 23 mar 2006) $
;*****************************************************************


;*****************************************************************
;* Include the matching device and assemble the project
;*****************************************************************
;* Tiny AVR ******************************************************
;.include "Device specific\t12.asm"
;.include "Device specific\t13.asm"
;.include "Device specific\t15.asm"
;.include "Device specific\t2313.asm"
;.include "Device specific\t25.asm"
;.include "Device specific\t45.asm"
.include "Device specific\t85.asm"
;.include "Device specific\t24.asm"
;.include "Device specific\t44.asm"
;.include "Device specific\t84.asm"
;.include "Device specific\t26.asm"
;.include "Device specific\t261.asm"
;.include "Device specific\t461.asm"
;.include "Device specific\t861.asm"
;* Mega AVR ******************************************************
;.include "Device specific\m48.asm"
;.include "Device specific\m88.asm"
;.include "Device specific\m168.asm"
;.include "Device specific\m164P.asm"
;.include "Device specific\m324P.asm"
;.include "Device specific\m644.asm"
;.include "Device specific\m8.asm"
;.include "Device specific\m8515.asm"
;.include "Device specific\m8535.asm"
;.include "Device specific\m16.asm"
;.include "Device specific\m162.asm"
;.include "Device specific\m165.asm"
;.include "Device specific\m165P.asm"
;.include "Device specific\m32.asm"
;.include "Device specific\m325.asm"
;.include "Device specific\m3250.asm"
;.include "Device specific\m645.asm"
;.include "Device specific\m6450.asm"
;.include "Device specific\m64.asm"
;.include "Device specific\m128.asm"
;.include "Device specific\m640.asm"
;.include "Device specific\m1280.asm"
;.include "Device specific\m1281.asm"
;.include "Device specific\m2560.asm"
;.include "Device specific\m2561.asm"
;* LCD AVR *******************************************************
;.include "Device specific\m169.asm"  ; Note that m169(rev.A-E) &
;.include "Device specific\m169P.asm" ; m169P have different osc
;.include "Device specific\m329.asm"
;.include "Device specific\m3290.asm"
;.include "Device specific\m649.asm"
;.include "Device specific\m6490.asm"
;* CAN AVR *******************************************************
;.include "Device specific\can128.asm"
;.include "Device specific\can64.asm"
;.include "Device specific\can32.asm"
;* LIGHTNING AVR *************************************************
;.include "Device specific\pwm3.asm"
;.include "Device specific\pwm2.asm"
;* BATTERY AVRs **************************************************
;.include "Device specific\m406.asm"  ; JTAG only. 1 MHz osc.
;* MATURE AVRs ***************************************************
;.include "Device specific\m163.asm"
;.include "Device specific\m323.asm"
;*****************************************************************


;*****************************************************************
;* Include the file for the interface the the calibration should 
;* be performed on
;*****************************************************************
;.include "Interface specific\isp_STK500_interface.inc"
;.include "Interface specific\isp_AVRISP_interface.inc"
;.include "Interface specific\isp_AVRISP_mkII_interface.inc"
;.include "Interface specific\jtag_interface.inc"
;.include "Interface specific\jtag_mkII_interface.inc"
;.include "Interface specific\jtag_mkII_isp_interface.inc"
.include "Interface specific\isp_Arduino_interface.inc"

;*****************************************************************
;*	Select Target frequency below
;*****************************************************************
.equ	TARGET_FREQ	= 8000000 ;Desired frequency in Hz 
                              ;(1000000 = 1MHz)

;*****************************************************************
;*	Specify the desired accuracy (1% recommended)
;*****************************************************************
.equ	ACCURACY	= 10		;In 1/10 percent (10 is 1%)


;*****************************************************************
;* Included main code...
;*****************************************************************
.include "common\macros.inc"
.include "common\main.asm"
