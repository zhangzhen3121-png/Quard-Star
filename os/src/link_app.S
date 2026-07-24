.align 3
.section .data
.global _num_app
_num_app:
.quad 2
.quad app_0_start
.quad app_1_start
.quad app_1_end
.align 3
.global _name_app
_name_app:
.string "time"
.string "write"
.align 3
.global app_0_start
.global app_0_end
app_0_start:
.incbin "../user/bin/time"
app_0_end:
.align 3
.global app_1_start
.global app_1_end
app_1_start:
.incbin "../user/bin/write"
app_1_end: