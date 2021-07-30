/*! \addtogroup disk 
* @{ 
*/

#include <pc88-c.h>

/** \param dest - Destination to load data in RAM
 \param srcTrack - Source track on disk 
 \param srcSector - Source sector on disk 
 \param numSecs - Number of sectors to copy (bytes*256)
 \param drive - Drive number (default 0) 
*/
void DiskLoad(u8* dest, u8 srcTrack, u8 srcSector, u8 numSecs, u8 drive) __naked 
{
    /* As-is from IPL.BIN, taken from Maroon's page.
       This is copied in during the auto-load sequence to C0xx. */
    
    dest; srcTrack; srcSector; numSecs; drive;
    __asm 
        ld iy, #0               ; iy 0 contains n. bytes in arguments
        add iy, sp              ; fix stack pointer 
        ld l, 2 (iy)     
        ld h, 3 (iy)            ; dest
        ld d, 4 (iy)            ; track 
        ld e, 5 (iy)            ; sector 
        ld b, 6 (iy)            ; nm secs
        ld c, 7 (iy)            ; drive no. 
    DISK_Load:
        ld			a,#0x02					; cmd 2, read data
        call		DISK_SdCmd
        ld			a,b						; sector num
        cp			#17
        jr			c,.load02
        ld			a,#17					; if 17, go to next track 
        sub			e
    .load02:
        call		DISK_SdData1
        ld			a,c						; drive #
        call		DISK_SdData1
        ld			a,d						; trk
        call		DISK_SdData1
        ld			a,e						; sectr
        call		DISK_SdData1

        ld			a,#0x12					; cmd 18 fast send data - required to continue load loop from disk 
        call		DISK_SdCmd

    .loop01:
        push		bc
        ld			b,#128					; 128 * 2bytes
    .loop02:
        call		DISK_RdData2
        djnz		.loop02					; loop in sectr

        inc			e
        ld			a,e
        cp			#17						; next track if 17..
        jr			z,.next
        pop			bc
        djnz		.loop01					; sector loop
        ret                                 ; all done!

    ; Quick sub to increase the load counter... 
    .next:
        inc			d						; from next track
        ld			e,#1					; sector from 1 
        pop			bc
        dec			b
        jr			nz,DISK_Load
        ret
    ; send cmd to subsys 
    ; a is command symbol
    ;  7   6   5   4   3   2   1   0
    ;  W   W   W   W   R   R   R   R
    ; ATN DAC RFD DAV  -  DAC RFD DAV
    DISK_SdCmd:
        push		af                          ; Push the queued disk command to the stack
        ld			a,#0b00001111				; Attention=1 
        out			(0xFF),a                    ; out to control port 
    .wait1:
        in			a,(0xFE)                    ; Read in from port C ...
        bit			1,a						    ; wait until Ready for Data bit is set 
        jr			z,.wait1
        ld			a,#0b00001110				; Attention=0
        out			(0xFF),a                    ;  (to ctl port)
        jr			DISK_SdData1sub             ; continue to send data routine 

    ;get 1 byte 
    DISK_SdData1:
        push		af
    .wait01:
        in			a,(0xFE)                    ; port C... 
        bit			#1,a						; Ready for Data ?
        jr			z,.wait01
    DISK_SdData1sub:
        pop			af                          
        out			(0xFD),a					; output the pushed cmd to port B
        ld			a,#0b00001001				; Data Validate 
        out			(0xFF),a                    ; (to ctl port)
    .wait2:
        in			a,(0xFE)                    ; port C 
        bit			2,a						    ; Data Accepted?
        jr			z,.wait2
        ld			a,#0b00001000				; Data Validate off  
        out			(0xFF),a                    ; 
    .wait3:
        in			a,(0xfe)                    ; in from C 
        bit			2,a						    ; Data Accepted? (bit 2)
        jr			nz,.wait3
        ret                                     ; OK!


    ;get 2 bytes from 0xfc, stores them in (hl) 
    DISK_RdData2:
        ld			a,#0b00001011				; Ready for Data
        out			(0xFF),a
    .wait10:
        in			a,(0xFE)
        rrca								    ; Data Valid?
        jr			nc,.wait10

        ld			a,#0b00001010				; Ready for Data=0
        out			(0xFF),a

        in			a,(0xFC)                    ; get data 
        ld			(hl),a
        inc			hl
        ld			a,#0b00001101				; Data Valid=1
        out			(0xFF),a
    .wait20:
        in			a,(0xFE)
        rrca
        jr			c,.wait20

        in			a,(0xFC)
        ld			(hl),a
        inc			hl
        ld			a,#0b00001100				; Data Accepted
        out			(0xFF),a
        ret

    __endasm;
}

/*! @} */
