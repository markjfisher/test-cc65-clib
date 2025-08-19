.exportzp OSW_A			:= $ef
.exportzp OSW_X			:= $f0
.exportzp OSW_Y			:= $f1

			.addr	$0190				; OSBYTE variables base address		 &236	*FX166/7

;*************************************************************************
;*									 *
;*	 OSBYTE	 01   ENTRY POINT					 *
;*									 *
;*	 READ/WRITE USER FLAG (&281)					 *
;*									 *
;*	    AND								 *
;*									 *
;*	 OSBYTE	 06   ENTRY POINT					 *
;*									 *
;*	 SET PRINTER IGNORE CHARACTER					 *
;*									 *
;*************************************************************************
; A contains osbyte number

_OSBYTE_1_6:		ora	#$f0				; A=osbyte +&F0
			bne	_BE99A				; JUMP to E99A

; ... other stuff...


_BE99A:			stx	OSW_X				; store X

;*************************************************************************
;*									 *
;*	 OSBYTE	 A6-FF	 ENTRY POINT					 *
;*									 *
;*	 READ/ WRITE SYSTEM VARIABLE OSBYTE NO. +&190			 *
;*									 *
;*************************************************************************

_OSBYTE_166_255:	tay					; Y=A
			lda	$0190,Y				; i.e. A=&190 +osbyte call!
			tax					; preserve this
			and	OSW_Y				; new value = OLD value AND Y EOR X!
			eor	OSW_X				; 
			sta	$0190,Y				; store it
			lda	$0191,Y				; get value of next byte into A
			tay					; Y=A
_BE9AC:			rts					; and exit

