;;
;; File: crt0.asm
;;
;; Entry point.
;;

[bits 32]


extern _gramc_main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; __crt0Main:
;;     Entry point.
;;
global __crt0Main
__crt0Main:

    ;main.c
	call _gramc_main
	
	;;#todo:
	;;é aqui que precisamos chamr o exit usndo o retorno;
	
hang:
    pause
    jmp hang


