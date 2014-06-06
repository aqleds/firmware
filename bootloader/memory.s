.include "p33FJ16GS502.inc"


.global _load_addr
_load_addr:
	mov	w0,TBLPAG
	mov	w1,w1
	
	return
	
.global _write_pgm_memory
_write_pgm_memory:
	mov	w0,NVMCON
	mov	#0x55,w0
	mov	w0,NVMKEY
	mov	#0xAA,w0
	mov	w0,NVMKEY
	bset NVMCON,#WR
	nop
	nop
1:	btsc NVMCON,#WR	
	bra 1b
	
	return

.global _write_pgm_latch
_write_pgm_latch:
	mov	w0,TBLPAG
	tblwtl w3,[w1]
	tblwth w2,[w1]
	
	return

.global _read_pgm_latch
_read_pgm_latch:
	mov	w0,TBLPAG
	tblrdl [w1],w0
	tblrdh [w1],w1
	
	return

.global _erase_pgm_page
_erase_pgm_page:

	
	push    TBLPAG
	mov	w2,NVMCON
					
	mov     w0,TBLPAG
	tblwtl  w1,[w1]

	
	mov	#0x55,w0
	mov	w0,NVMKEY
	mov	#0xAA,w0
	mov	w0,NVMKEY
	bset NVMCON,#WR
	nop	
	nop

erase_pgm_page_wait:
	btsc NVMCON,#WR	
	bra erase_pgm_page_wait
	
	pop     TBLPAG
	return

;***************************************************************	
.end
