section .bss
    stack_bottom resb 0x100000    ; 스택을 위한 공간

section .text
    global _start

_start:
    ; 스택 초기화
    mov rsp, stack_bottom

    ; kernel.elf를 0x500 주소에 로드
    mov rdi, 0x500                 ; 로드할 주소
    mov rsi, kernel_elf_size       ; kernel.elf의 크기
    mov rdx, kernel_elf            ; kernel.elf의 시작 주소
    call load_and_jump

    ; baash.obj를 0x10000000 주소에 로드
    mov rdi, 0x10000000            ; 로드할 주소
    mov rsi, baash_obj_size        ; baash.obj의 크기
    mov rdx, baash_obj             ; baash.obj의 시작 주소
    call load_and_jump

load_and_jump:
    ; 파일을 메모리에 로드
    mov rax, 0x2                   ; sys_read
    mov rdi, rax                   ; file descriptor (stdin)
    mov rax, 0x0                   ; sys_read
    mov rsi, rdi                   ; buffer (주소)
    mov rdx, 0x1000                ; 읽을 바이트 수
    syscall

    ; 파일을 메모리에 로드한 후, 로드된 코드 실행
    mov rdi, rdi                   ; file descriptor (stdin)
    mov rsi, rsi                   ; buffer (주소)
    mov rdx, rdx                   ; 읽을 바이트 수
    mov rax, 0x3b                  ; sys_execve
    syscall

    ; 에러 발생 시 프로그램 종료
    mov rax, 0x60                  ; sys_exit
    xor rdi, rdi                   ; 에러 코드
    syscall

section .data
kernel_elf db "kernel.elf",0
baash_obj db "baash.obj",0

section .data
kernel_elf_size equ $ - kernel_elf
baash_obj_size equ $ - baash_obj
