        .nolist
        .list

        .equ    kSof,2
        .equ    kSinPiQ,0x5A82
        .equ    kInvSqrt2,kSinPiQ

        .equ    PSVPAG,0x0034
        .equ    CORCON,0x0044
        .equ    MODCON,0x0046
        .equ    XBREV,0x0050
        .equ    XMODSRT,0x0048
        .equ    XMODEND,0x004A
        .equ    YMODSRT,0x004C
        .equ    YMODEND,0x004E

        .equ    SATA_ON,1<<7
        .equ    SATB_ON,1<<6
        .equ    SATDW_ON,1<<5
        .equ    ACCSAT_ON,1<<4
        .equ    FRACT_SET,SATA_ON|SATB_ON|SATDW_ON|ACCSAT_ON    

        .equ    RND_OFF,~(1<<1)
        .equ    IF_OFF,~(1<<0)
        .equ    FRACT_RESET,RND_OFF&IF_OFF

        .equ    FRACT_MODE,FRACT_SET&FRACT_RESET

        .ifdef  TOOLVERS                       
        .equ    FRACT_MODE,FRACT_MODE&0x3ff   
        .endif 

        .equ    PSV_ON,1<<2  
                           

        .macro  fractsetup      wx 
        mov     #FRACT_MODE,\wx
        mov     \wx,CORCON
        .endm   

        .macro  psvaccess       wx 
        mov     CORCON,\wx
        ior     #PSV_ON,\wx
        mov     \wx,CORCON
        .endm   

        .equ    COEFFS_IN_DATA,0xFF00   
        .equ    oNumCoeffs,0  
        .equ    oCoeffsBase,(oNumCoeffs+kSof)
        .equ    oCoeffsEnd,(oCoeffsBase+kSof)
        .equ    oCoeffsPage,(oCoeffsEnd+kSof)
        .equ    oDelayBase,(oCoeffsPage+kSof)
        .equ    oDelayEnd,(oDelayBase+kSof)
        .equ    oDelay,(oDelayEnd+kSof)
        
