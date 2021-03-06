;    
; File: lib16\s2modes.inc
;
; Descri��o:
;     Parte do Stage2.
;
; Atribui��es:
;     Ativa um modo de inicializa��o definido para o Boot Manager.
;     Op��es:
;     +1 - Inicia o Shell do boot manager.  
;     +2 - Inicia o BL.BIN no modo GUI. 
;
; Vers�o 1.0, 2015, 2016.
;

 
;---------------------------------------
; set_boot_mode:
;     Configura um modo de inicialisa��o do System4Nora.
;     in: al
; 
set_boot_mode:
    mov byte [META$FILE.INIT_MODE], al
	ret 
 
;---------------------------
; get_boot_mode:
;     Pega o modo de inicialisa��o do System4Nora.
;     out: al
;
get_boot_mode:
    mov al, byte [META$FILE.INIT_MODE]
	ret 
	
;--------------------------------------------
; set_video_mode:
;     Configura um modo de video.
;     in: ax
; 
set_video_mode:
    mov word [META$FILE.VIDEO_MODE], ax
	ret 
 
;-------------------------------------------
; get_video_mode:
;     Pega o modo de video.
;     out: ax
;
get_video_mode:
    mov ax, word [META$FILE.VIDEO_MODE]
	ret 
	
	
;==================================================================
; s2modesActivateMode:
;     Ativa o modo escolhido por configura��o. 
;
;+1 - Inicia o Shell do boot manager.  
;+2 - Inicia o BL.BIN no modo GUI. 	
;++

s2modesActivateMode:
   
	call Window.ClearScreen	
	
	pusha
    call Window.StatusBar
    mov si, msg_s2_activating_mode
    call PoeString
    popa
	
	;jmp $
	
	; Pega o modo de inicializa��o no metafile.
	; al = META$FILE.INIT_MODE

.switch:
	xor eax, eax
	call get_boot_mode
	
	cmp al, byte BOOTMODE_SHELL    ;1 - Shell
	je mShell
	
	cmp al, byte BOOTMODE_GUI      ;2 - GUI.
	je mGui 
	
.default:	
	jmp mShell	
	jmp $
	
;=================================================================
;+1 (boot mode shell) -  
;    Inicia a prompt de comandos do Boot Manager. #shell.   

mShell:

	pusha
    call Window.StatusBar
    mov si, msg_modo1
    call PoeString
    popa
	
.videoMode:

	;Set video mode.
	;Modo texto � VGA 3.
	
	xor eax, eax
	mov ax, word 3    
	call set_video_mode
	
	;Screen.
	;call Window.ClearScreen  
	call Cursor.Hide         
	
.bootMode:	

	;Ativar modo de inicializa��o.
	mov al, byte BOOTMODE_SHELL
	call set_boot_mode
	
	; Obs: 
	; @todo: Talvez os argumentos devam ser preparados antes.

	 
	; Inicializ�o do modo protegido.
	; Switching to Protected Mode.
	; msm/pm.inc
	
	jmp pmInit
	jmp $  
	

;==============================================================
;+2 (boot mode gui) - Modo gr�fico.
;
; VESA:
;     + Verifica se suporta vesa.	
;     + Inicia configura��o de video usando vesa.
;
;     Ativa o modo usando BIOS e valores do metafile.
;     @todo: (bugbug) aqui estamos usando vesa para configurar o modo de
; de video, sem saber nada sobre a placa de v�deo.
;
;;
;; * IMPORTANTE:
;;   O OBJETIVO � CHEGARMOS AQUI COM O ARQUIVO BL.BIN CARREGADO NA MEM�RIA
;;   E PASSARMOS O COMANDO PARA ELE, SEM TERMOS QUE CARREGALO USANDO RECURSOS DE 32BIT.
;++	
mGui:
    nop
.videoMode:	


	;Set video mode.
	;;Usa um modo previamente configurado.	
	xor eax, eax
	mov ax, word [META$FILE.VIDEO_MODE]    
	call set_video_mode
	
    ;Salva modo.
	mov al, byte BOOTMODE_GUI

	;Verifica suporte � VESA.
.probeVESA:	
	pusha
    call s2vesaSearchVESA 	
	popa
	jc noVesa 
	
	pusha
    call Window.StatusBar
    mov si, msg_initializing_VESA
    call PoeString
    popa	
	
	;Configura modo de video usando VESA.
.initVESA:
	pusha	
	call s2vesaInitVESA
	popa
	
.setupVideoMode:
	
	;Registra o modo escolhido.
	mov al, byte BOOTMODE_GUI
	call set_boot_mode	
	
	;; Setup registers.
	
    cli
	mov ax, 0 
    mov ds, ax
    mov es, ax
	;mov fs, ax           
	;mov gs, ax        
    xor ax, ax
    mov ss, ax
    mov sp, 0x6000 
	xor dx, dx
	xor cx, cx	
	xor bx, bx
	xor ax, ax
	
	;; #importante
	;; Temos que entrar no modo protegido j� estando 
	;; no modo grafico.

	;;
	;; GDTR
	;;
	
	lgdt [stage2_gdtr]

	
	;;
	;; CR0 - PE
	;;
	
	;Muda PE.
	mov eax, cr0
    or al, 1
    mov cr0, eax


    ;;
	;; Flush pipeline
	;;
	
s2modesFlushPipeline:
    jmp stage2_CODE_SEL:s2modespm32 
	
	;;
	;; ## 32 bit ## 
	;;
	
	[BITS 32]
s2modespm32:

    ;setup registers.	


    mov ax, stage2_DATA_SEL        
	mov ds, ax
	mov es, ax
	;mov fs, stage2_NULL_SEL    
	;mov gs, stage2_NULL_SEL    
	mov ss, ax
	mov esp, dword 0x0009FFFC
    mov edx, dword 0    
    mov ecx, dword 0
	mov ebx, dword 0	
    mov eax, dword 0    
	    
	; LFB address.
	; Width in pixels.
	; Height in pixel.
	; BPP.
	; metafile address
	; ...
	
	xor eax, eax
	mov eax, dword [ModeInfo_PhysBasePtr]  	
	mov dword [BootBlock.lfb], eax	
	
	xor eax, eax
	mov ax, word [ModeInfo_XResolution]    
	mov dword [BootBlock.x], eax 	
	
	xor eax, eax
	mov ax, word [ModeInfo_YResolution]    
	mov dword [BootBlock.y], eax 	
	
	xor eax, eax
	mov al, byte [ModeInfo_BitsPerPixel]   
	mov dword [BootBlock.bpp], eax 

	xor eax, eax
	mov eax, dword META$FILE                 	
	mov dword [BootBlock.metafile_address], eax	
    
	;Continua...
	
	;xor eax, eax
	mov al, byte 'G'                       ;Flag. (useing graphics)	
	mov ebx, dword [ModeInfo_PhysBasePtr]  ;LFB address (endere�o f�sico)
	
	mov ebp, dword BootBlock
	mov ecx, ebp                           ;tabela
	mov edx, ebp                           ;tabela		
		
		
	;;
	;; Aqui estamos em modo protegido e no modo gr�fico, 
	;; vamos testar se as mensagens para esse ambiente 
	;; est�o funcionando.
	;;
	
	;;
	;; @todo: Poder�amos limpar a tela entes de 
	;; apresentarmos essa mensagem.
	;;
	
	;; #debug
	;; Testando mensagem.
	
    mov eax, 0x00ffff00 ;color
	mov esi, s2modes_initmessageinGM
	call lib32PrintString
	
	;#debug
	;#bugbug isso demora.
    ;call _asm_refresh_screen	

	;#debug
	;jmp $
	
    ; Op��o � saltar para uma �rea de inicializa��o em 
	; 32bit modo gr�fico.
	
	;;
	;; GO!
	;;	
    
	;msm/start.inc
	jmp startGUI
	jmp $
	
	
s2modes_initmessageinGM db "s2modes: Initializing PM in GM", 13, 10, 0

;;
;;#####################################################################
;;

;;n�o queremos estragar as rotinas de 16bit.	
[bits 16]	
;N�o suporta VESA, entrar no Shell do Boot Manager.
noVesa:
	mov al, byte BOOTMODE_SHELL
	call set_boot_mode	
	mov al, byte BOOTMODE_SHELL
	jmp s2modesActivateMode
	jmp $
;--

;
;--------------------------------------------
; 
 
;;@todo: Change to msg_mode1_selected and msg_mode2_selected. 
msg_modo1 db "s2: MSM Shell",13,10,0
msg_modo2 db "s2: GUI",13,10,0

;
; End.
;
