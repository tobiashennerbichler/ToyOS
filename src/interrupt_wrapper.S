.intel_syntax noprefix

.extern exception_handler

.macro def_exception nr, func=exception_handler
.global wrap_exception_\nr
.type wrap_exception_\nr, @function
wrap_exception_\nr:
    push \nr
    call \func
    add esp, 4
    iret
.endm

.macro def_exception_err nr, func=exception_handler
.global wrap_exception_\nr
.type wrap_exception_\nr, @function
wrap_exception_\nr:
    push \nr
    call \func
    add esp, 8
    iret
.endm

# https://wiki.osdev.org/Exceptions
def_exception 0
def_exception 1
def_exception 2
def_exception 3
def_exception 4
def_exception 5
def_exception 6
def_exception 7
def_exception_err 8
def_exception 9
def_exception_err 10
def_exception_err 11
def_exception_err 12
def_exception_err 13
def_exception_err 14
def_exception 15
def_exception 16
def_exception_err 17
def_exception 18
def_exception 19
def_exception 20
def_exception_err 21
def_exception 22
def_exception 23
def_exception 24
def_exception 25
def_exception 26
def_exception 27
def_exception 28
def_exception_err 29
def_exception_err 30
def_exception 31

.global exception_table
exception_table:
    .irp i,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    .long wrap_exception_\i
    .endr