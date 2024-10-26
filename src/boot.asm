[BITS 16]   ;16 bit code
org 0x7C00 ;starting addr of program
boot:
	;set seg regs
	xor ax, ax
	mov gs, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov ds, ax

	mov ax, 0x8000
	mov sp, ax
	xor ax,ax          ;clear ax
	mov ds,ax          ;set destination segment to 0
	mov [drive_num],dl ;save drive number
	mov ah, 0x00       ;setup video mode
	mov al, 0x13       ;320 by 200 256 color vga graphics
	int 0x10           ;video mode interupt

	mov ah, 0x02       ;read sectors
	mov al, 0x7F       ;79 sectors (i didnt count how big my program is)
	mov ch, 0x00       ;cylinder 0
	mov cl, 2          ;sector number
	mov dh, 0          ;head number 0
	mov dl,[drive_num] ;drive read from
	xor bx, bx         ;clr bx
	mov bx, 0x1000    ;segment num 0x1000*0x10:B
	mov es, bx        ;load segment into es (extra seg reg)
	xor bx, bx        ;clear bx since its offset A:bx
	int 0x13          ;disk service interupt
	xor bx, bx        ;clr bx
	mov es, bx        ;reset es to 0
	jc hang           ;error check
	
	cli               ;disable interupts to prevent crash (no idt in proj)
	;enable a20 line
	in al,0x92
	or al,2
	out 0x92,al

	lgdt [gdtr]
	mov eax, cr0
	or al, 1      ;prepare for protected mode
	mov cr0, eax  ;set protected mode bit
	jmp 0x8:flush ;set data segments for protected mode
hang:
	hlt
	jmp hang
drive_num:
	dw 0x0000
gdt:
	;null descriptor
	dw 0x0000
	dw 0x0000
	dw 0x0000
	dw 0x0000
	;code descriptor
	dw 0xFFFF
	dw 0x0000
	dw 0x9A00
	dw 0x004F
	;data descriptor
	dw 0xFFFF
	dw 0x0000
	dw 0x9200
	dw 0x004F
gdt_end:
align 4
gdtr:
	dw gdt_end-gdt-1 ;limit
	dd gdt           ;base
[BITS 32]
hang2:
	hlt
	jmp hang
flush:
	mov ax, 0x10
	mov gs, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov ds, ax
	jmp protmode ;32 bits!
protmode:
	mov edi,0xb8000     ;memory maped grpahics
	mov eax, 0x02490248 ;2 16bit words vga color then ascii char
	stosd               ;HI
	jmp 0x10000         ;jump into where i loaded _start
;makes sector bootable
times 510 -($-$$) db 0
dw 0xAA55 ;magic number
