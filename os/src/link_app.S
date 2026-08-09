.align 3
.section .data
.global _num_app
_num_app:
.quad 4
.quad app_0_start
.quad app_1_start
.quad app_2_start
.quad app_3_start
.quad app_3_end
.align 3
.global _name_app
_name_app:
.string "shell"
.string "time"
.string "write"
.string "xec"
.align 3
.global app_0_start
.global app_0_end
app_0_start:
.incbin "../user/bin/shell"
app_0_end:
.align 3
.global app_1_start
.global app_1_end
app_1_start:
.incbin "../user/bin/time"
app_1_end:
.align 3
.global app_2_start
.global app_2_end
app_2_start:
.incbin "../user/bin/write"
app_2_end:
.align 3
.global app_3_start
.global app_3_end
app_3_start:
.incbin "../user/bin/xec"
app_3_end: