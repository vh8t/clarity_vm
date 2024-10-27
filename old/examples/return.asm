.section text
  .global _start

_start:
  mov r0, 1  ;; When vm exits, it exits with value from r0
  hlt
