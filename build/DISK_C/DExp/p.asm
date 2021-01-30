; public _protect_mode
; _protect_mode proc near
;     ; push bp
;     ; mov bp, sp
;     ; push di
;     ; mov ax, 6[bp]
;     ; mov word ptr _global_init, ax
;     ; mov word ptr _global, 100
;     ; pop di
;     ; pop bp
;     ; ret
;     ; ------------------
;     ; 禁止CPU级别的中断
;     cli

;     ; ------------------
;     ; 打开A20
;     in 		al,0x92
;     or 		al,0000_0010B  ; 设置第1位为1
;     out 	0x92,al


;     ; ------------------
;     ; 进入保护模式
;     mov 	EAX,CR0
;     or 		EAX,0x00000001 ; 设置第0位为1
;     mov 	CR0,EAX
; _protect_mode endp
; end


public _protect_mode
public _var_from_asm
extrn _global:byte, _global_init:byte

_DATA	segment use16 word public 'DATA'
    _var_from_asm label word
        db 10
        db 0
_DATA ends

_TEXT segment use16 byte public 'CODE'
    assume CS:_TEXT, DS:_DATA

_protect_mode proc near
    ; ------------------
    ; 禁止CPU级别的中断
    cli

    ; ------------------
    ; 打开A20
    in 		al,0x92
    or 		al,0000_0010B  ; 设置第1位为1
    out 	0x92,al


    ; ------------------
    ; 进入保护模式
    mov 	EAX,CR0
    or 		EAX,0x00000001 ; 设置第0位为1
    mov 	CR0,EAX
    ret
_protect_mode endp
_TEXT ends
end