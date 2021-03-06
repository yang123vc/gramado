;
; File: browser.inc
;
; Descri��o:
;     Arquivo principal do Browser do system4Nora.
;
; Sobre o Browser:
; ===============
;     O Browser � a interface gr�fica. � a �rea de trabalho onde
; aparecer�o as p�ginas em html, os atalhos para as aplica��es ou os 
; gerenciadores do sistema.
;
; Obs: O Browser s� funciona se a inicializa��o habilitou o modo gr�fico.
;      sen�o ele retorna. Ele avalia se modo gr�fico � um modo v�lido.
; Oferece informa��es sobre o modo gr�fico atual. Pode gerar um request 
; ao kernel para que o kernel volte ao modo real e troque o modo de video. 
; Ou apenas modificar o metafile de inicializa��o e pedir pra reiniciar a 
; m�quina.
;		
; Vers�o 1.0, 2015. 
;



;------------------------------
; Browser_Main:
;     Fun��o principal do Browser do System4Nora.
;
;
Browser_Main:
	pushad
	
	;Check Mode.
	xor eax, eax
	mov al, byte [BOOT.modo] 	
	
	cmp al, byte 2    ;Check for graphics.   
	jne Browser_fail
	
	
	;Test background
	;call _background_teste
	
	
	;
    ; Create window.	
    ; a = x
    ; b = y
    ; c = width
    ; d = height
    ; edi = color
    ;
	
	;mov eax, 100    ;x.
	;mov ebx, 100    ;y.
	;mov ecx, 320    ;largura.
	;mov edx, 200    ;altura.	
    ;mov edi, dword 0xC0C0C000    ;cor.    	
	;call CreateWindow

	
	;mov eax, 0xff0000 ;cor
	;mov ebx, 100
	;mov ecx, 100 
    ;call _gui_direct_putpixel

	;mov eax, 0x00ff00 ;cor
	;mov ebx, 200
	;mov ecx, 200 
    ;call _gui_direct_putpixel

	;mov eax, 0x0000ff ;cor
	;mov ebx, 300
	;mov ecx, 300 
    ;call _gui_direct_putpixel

	;;h line.
	;mov eax, 400 ;x1
	;mov ebx, 450 ;x2
	;mov ecx, 400 ;y
	;mov edx,  dword  0xFFFF00 	
	;call draw_horizontal_line

	;mov eax, 200 ;x1
	;mov ebx, 300 ;x2
	;mov ecx, 200 ;y
	;mov edx, 0xFF0000 	
	;call draw_horizontal_line

    ;
    ; Draw rect.	
    ; a = x1
    ; b = x2
    ; c = y1
    ; d = y2
    ; edi = color
    ;
	
	;Icone
	;mov eax, 50 ;x1
	;mov ebx, 100 ;x2
	;mov ecx, 500 ;y1
	;mov edx, 550 ;y2 	
    ;mov edi, dword 0xc0c0c000    
	;call draw_rect	
	
	;mov eax, 40 ;x1
	;mov ebx, 56 ;x2
	;mov ecx, 516 ;y1
	;mov edx, 532 ;y2 	
    ;mov edi,  dword  0xF8FF00    
	;call draw_rect	
	
	;String
	;Obs se realmente estiver em modo grafico 
	;essa string n�o funciona.
	;mov esi, msg_browser_testing
	;call os_output	
	
    ;	
    ; Argumentos:
    ;     eax = Endre�o do arquivo .BMP. 
    ;     ebx = x. (x no LFB).
    ;     ecx = y. (y no LFB).
    ;     edx = null.
    ;
	;mov eax, 0
	;mov ebx, 320    ;(x no LFB).
	;mov ecx, 200    ;(y no LFB).	
	;call _my_direct_load_bitmap_16x16 

	
;
; Loop.
;	
.message_loop:	
	;Get key.
	call os_input_key    ;Pega o char gerado pela irq1.
	jnc .halt_and_more	         ;No key entered, halt until an interrupt is received.

	call Browser_Procedure
 
	jmp .message_loop		
.halt_and_more:
    hlt
    jmp .message_loop	
	
	
;----------------------------
; Browser_Procedure:
; 
;     Procedimento de janela para o Browser.
;
; IN: AL = Scancode.
;	
Browser_Procedure:

	;Enter.
	cmp al, 0x1C		;If 'Enter' key pressed, finish.
	je .p_1    
	
	;Backspace.
	cmp al, 0x0E			
	je .p_2
	
	;In ASCII range (32 - 126)?
	cmp al, 32			
	jl .p_3
	
	;Limite.
	cmp al, 126
	jg .p_4

    ;Default:
    jmp	.done
	
	;(rrggbbaa)
	
.p_1:
    mov edi, dword 0xc0c0c000    ;gray.	
	jmp .draw
.p_2:
    mov edi, dword 0xa0a0a000    ;green
	jmp .draw
.p_3:
    mov edi, dword 0x80808000    ;blue
	jmp .draw
.p_4:
    mov edi, dword 0x00000000    ;0.
	jmp .draw
.p_5:
    mov edi, dword 0x00000000    ;0.
	jmp .draw
	
.draw:	
	mov eax, 0    ;x.
	mov ebx, 0    ;y.
	mov ecx, 800  ;largura.
	mov edx, 600  ;altura.	
    ;mov edi, dword 0    ;cor.    	
	call CreateWindow
	
	call _asm_refresh_screen
	
.done:
	ret

	
	
;----------------------
; Browser_fail:
;
;	
Browser_fail:
	mov esi, msg_browser_no_graphics
	call os_output	
    jmp $

	
;-----------------------------------------------
; Browser_done:
;	
Browser_done:	
    ;Cria uma janelinha no canto superior esquerdo
	;para sinalizar o fim da execu��o.
	mov eax, 0     ;x.
	mov ebx, 0     ;y.
	mov ecx, 100   ;largura.
	mov edx, 50    ;altura.	
    mov edi, dword 0    ;cor.    	
	call CreateWindow

	call _asm_refresh_screen
	jmp $
	
	
	
;
; Browser messages.
;
	
msg_browser_testing     db '[Browser:] Starting on gm',13,10,0
msg_browser_no_graphics db '[Browser fail>] Starting on tm',13,10,0
msg_browser_procedure   db '[Browser:] Procedure test',13,10,0

;
;fim.
;
