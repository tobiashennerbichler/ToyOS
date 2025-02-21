.intel_syntax noprefix

.macro def_exception nr, func=exception_handler
.global wrap_exception_\nr
.type wrap_exception_\nr, @function
wrap_exception_\nr:
    call \func
    iret
.endm

.macro def_exception_err nr, func=exception_handler
.global wrap_exception_\nr
.type wrap_exception_\nr, @function
wrap_exception_\nr:
    call \func
    add esp, 4
    iret
.endm

exception_handler:
    cli
    hlt

def_exception 0