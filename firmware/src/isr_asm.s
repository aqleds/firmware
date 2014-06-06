
.equ __33FJ16GS502, 1
.include "p33FJ16GS502.inc"

; Buck 1 Maximum Duty cycle for voltage mode control
.equ Buck1MaxDC, 2400 ; 2024

; Buck 1 Minimum Duty cycle for voltage mode control
.equ Buck1MinDC, 72

; Buck 2 Maximum Duty cycle for voltage mode control
.equ Buck2MaxDC, 2400

; Buck 2 Minimum Duty cycle for voltage mode control
.equ Buck2MinDC, 72

; Boost Maximum Duty cycle for voltage mode control
.equ Buck3MaxDC, 2400

; Boost Minimum Duty cycle for voltage mode control
.equ Buck3MinDC, 72

.equ    offsetabcCoefficients, 0
.equ    offsetcontrolHistory, 2
.equ    offsetcontrolOutput, 4
.equ    offsetmeasuredOutput, 6
.equ    offsetcontrolReference, 8

.data
.text

.global __ADCP0Interrupt
.global __ADCP1Interrupt
.global __ADCP2Interrupt

__ADCP0Interrupt:
    push w0
    push w1

    mov ADCBUF1,w1
    mov #_Buck1Current,w0
    mov w1,[w0]



    mov #_Buck1VoltagePID, w0

    mov ADCBUF0, w1
    sl  w1, #5, w1						; Since only a 10-bit ADC shift left 
										; by 5 leaving the 16-bit 0 for a positive #

    mov w1, [w0+#offsetmeasuredOutput]
    call _PID  							; Call PID routine
    mov.w [w0+#offsetcontrolOutput], w1 ; Clamp PID output to allowed limits
    
    asr w1, #3, w0						; Scaling factor for voltage mode control

    mov.w #Buck1MinDC, w1				; Saturate to minimum duty cycle
    cpsgt w0, w1
    mov.w w1, w0
    mov.w #Buck1MaxDC, w1				; Saturate to maximum duty cycle
    cpslt w0, w1
    mov.w w1, w0

    mov.w w0, PDC1						; Update new Duty Cycle
    mov.w w0, TRIG1						; Update new trigger value to correspond to new duty cycle




    bclr	ADSTAT,	#0					; Clear Pair 0 conversion status bit
    bclr	IFS6, #14					; Clear Pair 0 Interrupt Flag


    pop w1
    pop w0
    retfie								; Return from interrupt

__ADCP1Interrupt:
    push w0
    push w1 
    push w2

    mov ADCBUF3,w1
    mov #_Buck2Current,w0
    mov w1,[w0]

    mov #_Buck2VoltagePID, w0			
    mov ADCBUF2, w1
    sl  w1, #5, w1

    mov w1, [w0+#offsetmeasuredOutput]
    call _PID  							; Call PID routine
    mov.w [w0+#offsetcontrolOutput], w1 ; Clamp PID output to allowed limits
    asr w1, #3, w0

    mov.w #Buck2MinDC, w1				; saturate to minimum duty cycle
    cpsgt w0, w1
    mov.w w1, w0
    mov.w #Buck2MaxDC, w1
    cpslt w0, w1
    mov.w w1, w0
				
    mov.w w0, PDC2						; Update new Duty Cycle
    mov.w w0, TRIG2						; Update new trigger value to correspond to new duty cycle

    bclr	ADSTAT,	#1					; Clear Pair 1 conversion status bit
    bclr	IFS6, #15					; Clear Pair 1 Interrupt Flag

    pop w2
    pop w1
    pop w0
    retfie			 					; Return from interrupt
    
__ADCP2Interrupt:
    push w0
    push w1 
    push w2

    mov ADCBUF5,w1
    mov #_Buck3Current,w0
    mov w1,[w0]

    mov #_Buck3VoltagePID, w0
    mov ADCBUF4, w1
    sl  w1, #5, w1

    mov w1, [w0+#offsetmeasuredOutput]
    call _PID  							; Call PID routine
    mov.w [w0+#offsetcontrolOutput], w1 ; Clamp PID output to allowed limits
    asr w1, #3, w0

    mov.w #Buck3MinDC, w1				; saturate to minimum duty cycle
    cpsgt w0, w1
    mov.w w1, w0
    mov.w #Buck3MaxDC, w1
    cpslt w0, w1
    mov.w w1, w0
				
    mov.w w0, PDC3						; Update new Duty Cycle
    mov.w w0, TRIG3						; Update new trigger value to correspond to new duty cycle

    bclr	ADSTAT,	#2					; Clear Pair 2 conversion status bit
    bclr	IFS7, #0					; Clear Pair 2 Interrupt Flag

    pop w2
    pop w1
    pop w0
    retfie			 					; Return from interrupt

.end
