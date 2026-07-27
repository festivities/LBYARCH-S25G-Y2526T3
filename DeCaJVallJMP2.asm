; VALLE, Jose Edgardo T. - S25G
; De Castro, Jediaelle Denise - S25G

default rel

section .data
    scale           dq  255.0

section .text
    global  imgCvtGrayDoubleToInt

; void imgCvtGrayDoubleToInt(int height, int width, const double *input, unsigned char *output)
;   ECX = height, EDX = width, R8 = input, R9 = output

imgCvtGrayDoubleToInt:
    mov     eax, ecx
    imul    eax, edx
    movsxd  r10, eax                    ;r10 = total pixels
    xor     r11, r11                    ;r11 = index

    movsd   xmm1, [scale]               ;xmm1 = 255.0

.convert_loop:
    cmp     r11, r10
    jge     .done

    movsd   xmm0, [r8 + r11*8]          ;xmm0 = input[index]
    mulsd   xmm0, xmm1                  ;xmm0 = input[index] * 255.0
    cvtsd2si eax, xmm0                  ;round to nearest
    mov     [r9 + r11], al              ;output[index] = low byte

    inc     r11
    jmp     .convert_loop

.done:
    ret
