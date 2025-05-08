.section .text
.global custom_setjmp
.type   custom_setjmp, %function
custom_setjmp:
MOV             X16, SP
STP             X19, X20, [X0]
STP             X21, X22, [X0,#0x10]
STP             X23, X24, [X0,#0x20]
STP             X25, X26, [X0,#0x30]
STP             X27, X28, [X0,#0x40]
STP             X29, X30, [X0,#0x50]
STR             X16, [X0,#0x60]
STP             D8, D9, [X0,#0x70]
STP             D10, D11, [X0,#0x80]
STP             D12, D13, [X0,#0x90]
STP             D14, D15, [X0,#0xA0]
MOV             W0, #0
RET

.global custom_longjmp
.type   custom_longjmp, %function
custom_longjmp:
LDP             X19, X20, [X0]
LDP             X21, X22, [X0,#0x10]
LDP             X23, X24, [X0,#0x20]
LDP             X25, X26, [X0,#0x30]
LDP             X27, X28, [X0,#0x40]
LDP             X29, X30, [X0,#0x50]
LDR             X16, [X0,#0x60]
LDP             D8, D9, [X0,#0x70]
LDP             D10, D11, [X0,#0x80]
LDP             D12, D13, [X0,#0x90]
LDP             D14, D15, [X0,#0xA0]
MOV             SP, X16
CMP             W1, #0
CINC            W0, W1, EQ
BR              X30
