# FreeRTOS RISC-V 从 M 态移植到 S 态

## 1. 文档目的

本文对比下面两套 FreeRTOS RISC-V GCC 端口，并说明如何从零开始将 FreeRTOS 从 Machine Mode（M 态）移植到 Supervisor Mode（S 态）。

M 态参考端口：

- `trusted_domain/source/GCC/RISC-V/port.c`
- `trusted_domain/source/GCC/RISC-V/portASM.S`
- `trusted_domain/source/GCC/RISC-V/portmacro.h`

项目中的 S 态端口：

- `trusted_domain/FreeRTOS-Kernel/portable/GCC/RISC-V/port.c`
- `trusted_domain/FreeRTOS-Kernel/portable/GCC/RISC-V/portASM.S`
- `trusted_domain/FreeRTOS-Kernel/portable/GCC/RISC-V/portmacro.h`

两套文件并非完全相同版本：参考端口文件头是 FreeRTOS Kernel V10.4.4，项目端口文件头是 V10.4.3 LTS Patch 3。因此，diff 中有一部分属于版本差异、平台适配和注释调整，并非 M 态到 S 态所必需的修改。

## 2. 总体架构变化

M 态 FreeRTOS 可以直接访问机器级 CSR、CLINT 和机器中断：

```text
FreeRTOS（M 态）
  ├── mtvec/mstatus/mepc/mcause
  ├── mie.MTIE/MEIE
  ├── 直接读写 mtime/mtimecmp
  └── mret
```

S 态 FreeRTOS 不能直接管理机器级资源，需要在 M 态保留 OpenSBI 或自定义运行时：

```text
FreeRTOS（S 态）
  ├── stvec/sstatus/sepc/scause
  ├── sie.STIE/SSIE/SEIE
  ├── 通过 SBI 请求 Timer 和 IPI 服务
  └── sret
          │
          │ ecall/SBI
          ▼
OpenSBI（M 态）
  ├── 配置 PMP 和特权级切换
  ├── 管理 mtime/mtimecmp
  ├── 管理机器级 IPI
  ├── 将中断转换或委托给 S 态
  └── 保护 M 态固件
```

完整启动流程应为：

```text
CPU Reset（M 态）
    ↓
低级 Boot
    ↓
OpenSBI（M 态）
    ├── 初始化 CLINT/ACLINT、PLIC
    ├── 配置 PMP/Domain
    ├── 配置异常和中断委托
    ├── 提供 SBI TIME/IPI 服务
    └── 设置 mstatus.MPP=S，执行 mret
            ↓
FreeRTOS startup.S（S 态）
    ├── 初始化 gp/sp
    ├── 初始化 .data/.bss
    ├── 初始化 S 态 CSR
    └── 调用 main()
            ↓
vTaskStartScheduler()
            ↓
FreeRTOS 任务
```

## 3. M 态与 S 态 CSR 对照

| 功能 | M 态端口 | S 态端口 | 说明 |
|---|---|---|---|
| Trap 入口 | `mtvec` | `stvec` | 保存统一 Trap handler 地址 |
| 全局中断开关 | `mstatus.MIE`，bit 3 | `sstatus.SIE`，bit 1 | 控制当前特权级全局中断 |
| Trap 前中断状态 | `mstatus.MPIE` | `sstatus.SPIE` | Trap 返回后恢复中断状态 |
| Trap 前特权级 | `mstatus.MPP` | `sstatus.SPP` | 决定 `mret/sret` 返回到哪个特权级 |
| Trap 返回地址 | `mepc` | `sepc` | 保存被打断指令地址 |
| Trap 原因 | `mcause` | `scause` | 区分中断和同步异常 |
| Trap 附加信息 | `mtval` | `stval` | 非法指令、缺页或访问异常信息 |
| 中断使能 | `mie` | `sie` | 控制定时器、软件和外部中断 |
| 中断挂起 | `mip` | `sip` | 保存中断 Pending 状态 |
| Trap 返回 | `mret` | `sret` | 恢复 PC、特权级和中断状态 |

常用中断编号变化：

| 中断 | M 态 cause | S 态 cause |
|---|---:|---:|
| Software interrupt | 3 | 1 |
| Timer interrupt | 7 | 5 |
| External interrupt | 11 | 9 |

`mcause/scause` 的最高位表示它是否为异步中断：

```text
最高位 = 1：异步中断
最高位 = 0：同步异常
其余位：cause 编号
```

## 4. `port.c` 修改分析

### 4.1 添加 SBI 支持

S 态版本新增：

```c
#include "sbi.h"
#include "riscv_asm.h"
```

`sbi.h` 用于调用 OpenSBI 提供的 TIME、IPI 等扩展，`riscv_asm.h` 提供 CSR 操作宏。

### 4.2 Tick 从直接操作 CLINT 改为 SBI

M 态端口直接管理机器定时器：

```text
读取 mtime
    ↓
计算 ullNextTime
    ↓
写入当前 hart 的 mtimecmp
    ↓
产生 Machine timer interrupt（cause 7）
```

S 态版本使用：

```c
static uint64_t get_ticks(void)
{
    uint64_t time_elapsed;

    __asm__ __volatile__(
        "rdtime %0"
        : "=r"(time_elapsed));

    return time_elapsed;
}

void vPortSetupTimerInterrupt(void)
{
    sbi_set_timer(get_ticks() + uxTimerIncrementsForOneTick);
}
```

其运行路径是：

```text
FreeRTOS S 态调用 sbi_set_timer()
    ↓ ecall
OpenSBI M 态设置 mtimecmp
    ↓ 到期
产生 Machine timer interrupt
    ↓
OpenSBI 处理机器定时器并注入 STIP
    ↓
FreeRTOS 收到 Supervisor timer interrupt（cause 5）
```

这样修改的原因是：即使 PMP 允许 S 态访问 CLINT MMIO，机器定时器产生的仍是 M 态中断，而不是 FreeRTOS 需要的 S 态 Timer 中断。使用 SBI 可以让 OpenSBI完成 M Timer 到 S Timer 的转换。

### 4.3 `configCPU_CLOCK_HZ` 的实际含义

当前 Tick 增量计算为：

```c
uxTimerIncrementsForOneTick =
    configCPU_CLOCK_HZ / configTICK_RATE_HZ;
```

这里需要的是 `time` CSR 的计数频率，即设备树中的 `timebase-frequency`，不一定是真正的 CPU 主频。

当前工程中：

```text
timebase-frequency = 10 MHz
configCPU_CLOCK_HZ = 10 MHz
```

所以当前值能够匹配。移植到其他平台时应重新确认。

### 4.4 当前 Timer 实现可能产生漂移

当前每次 Tick 都按下面方式设置下次中断：

```c
sbi_set_timer(rdtime() + tick_increment);
```

如果 Trap 和调度处理耗时为 `delay`，实际周期将变成：

```text
tick_increment + delay
```

延迟会不断累积。更稳定的实现是保存绝对时间：

```c
ullNextTime += uxTimerIncrementsForOneTick;
sbi_set_timer(ullNextTime);
```

必要时还需要处理系统长时间关中断造成的 Tick 补偿。

### 4.5 调度器中断使能修改

M 态端口使用 `mie`：

```asm
mie.MTIE = 1
mie.MEIE = 1
```

S 态版本改为：

```c
csr_set(CSR_SIE, SIP_STIP);
csr_set(CSR_SIE, SIP_SSIP);
```

含义是：

- `STIE`：允许 Supervisor timer interrupt，用于 Tick。
- `SSIE`：允许 Supervisor software interrupt，用于主动任务切换。

当前没有使能 `SEIE`。如果需要 PLIC 外部中断，还要执行：

```c
csr_set(CSR_SIE, SIP_SEIP);
```

同时必须初始化 PLIC 的 Supervisor context。

### 4.6 Trap 入口检查修改

M 态检查：

```c
csrr mtvec
```

S 态检查：

```c
csrr stvec
```

并检查低两位是否为 0，以确认 `stvec` 使用 Direct 模式。当前汇编端口只有一个统一 Trap 入口，不支持直接切换成 Vectored 模式。

### 4.7 增加 IPI 清除函数

S 态端口新增：

```c
void vPortClearIpiInterrupt(void)
{
    sbi_clear_ipi();
}
```

当前 OpenSBI 构建启用了 `CONFIG_SBI_ECALL_LEGACY=y`，所以 legacy `CLEAR_IPI` 可以工作。

更直接的方式是清除 `sip.SSIP`：

```asm
csrc sip, 2
```

这样可以减少一次 S 态到 M 态的切换，并降低对 SBI legacy 扩展的依赖。

### 4.8 `prvTaskExitError()`

S 态版本提供 `prvTaskExitError()`，当任务函数错误返回时：

1. 触发断言。
2. 关闭 S 态中断。
3. 进入死循环。

任务应使用：

```c
vTaskDelete(NULL);
```

结束自身，而不能直接 `return`。

## 5. `portmacro.h` 修改分析

### 5.1 主动任务切换机制

M 态原始实现：

```c
#define portYIELD() __asm volatile("ecall")
```

M 态执行 `ecall` 会产生 Machine environment call。M 态 FreeRTOS Trap handler 将 cause 11 解释为主动任务切换。

S 态不能继续把裸 `ecall` 当成 FreeRTOS 私有系统调用，因为 S 态的 `ecall` 是 SBI 调用入口，会进入 OpenSBI。

项目改成：

```c
#define portYIELD() \
    sbi_send_ipi((const unsigned long *)(0x1 << PRIM_HART))
```

执行流程：

```text
任务调用 taskYIELD()
    ↓
SBI IPI 扩展给当前 hart 发送 IPI
    ↓
OpenSBI 设置 SSIP
    ↓
FreeRTOS Trap handler 收到 cause 1
    ↓
清除 SSIP
    ↓
vTaskSwitchContext()
```

当前方案依赖 `PRIM_HART=7`，只适用于 FreeRTOS 固定运行在 hart 7 的配置。

建议至少把位移改为：

```c
1UL << PRIM_HART
```

对于通用多 hart 端口，应保存或获取当前 hart ID，并正确构造 SBI hart mask，而不是在端口层硬编码。

### 5.2 临界区中断控制

M 态：

```c
#define portDISABLE_INTERRUPTS() __asm volatile("csrc mstatus, 8")
#define portENABLE_INTERRUPTS()  __asm volatile("csrs mstatus, 8")
```

S 态：

```c
#define portDISABLE_INTERRUPTS() __asm volatile("csrc sstatus, 2")
#define portENABLE_INTERRUPTS()  __asm volatile("csrs sstatus, 2")
```

其中：

```text
mstatus.MIE = bit 3 = 0x8
sstatus.SIE = bit 1 = 0x2
```

FreeRTOS 通过 TCB 中的临界区嵌套计数，保证多层 `taskENTER_CRITICAL()` 不会过早打开中断。

### 5.3 CLINT 配置检查

M 态端口需要：

```c
configMTIME_BASE_ADDRESS
configMTIMECMP_BASE_ADDRESS
```

S 态端口已经不直接访问 `mtimecmp`，所以项目中注释了相关检查。

当前 `FreeRTOSConfig.h` 仍定义了 CLINT 地址，但 S 态 Tick 实现没有使用。建议将其移除或设为 0，并单独定义 Timer timebase，避免配置含义混乱。

### 5.4 与特权级无直接关系的修改

以下改动不是 M 态到 S 态所必需：

- 直接包含 `quard_star.h` 和 `FreeRTOSConfig.h`。
- 使用 C 循环实现 `ucPortCountLeadingZeros()`。
- 删除芯片扩展上下文宏。
- 修改注释和文件头版本。
- 修改 `portEND_SWITCHING_ISR` 的宏形式。

建议保留更安全的宏写法：

```c
#define portEND_SWITCHING_ISR(xSwitchRequired)    \
    do                                             \
    {                                              \
        if ((xSwitchRequired) != pdFALSE)          \
        {                                          \
            vTaskSwitchContext();                  \
        }                                          \
    } while (0)
```

## 6. `portASM.S` 修改分析

### 6.1 上下文布局

端口使用：

```asm
#define portCONTEXT_SIZE (30 * portWORD_SIZE)
```

任务栈帧包含：

```text
slot 0       sepc/任务返回 PC
slot 1       x1/ra
slot 2       x5/t0
...
slot 28      x31/t6
slot 29      sstatus
```

没有保存：

- `x2/sp`：它就是当前任务栈指针。
- `x3/gp`：假设所有任务共享同一全局地址空间。
- `x4/tp`：假设线程指针固定或未使用。

任务上下文保存后，将任务 `sp` 写入当前 TCB：

```asm
load_x  t0, pxCurrentTCB
store_x sp, 0(t0)
```

FreeRTOS TCB 的第一个成员就是任务栈顶指针。

### 6.2 CSR 保存修改

M 态：

```asm
csrr t0, mstatus
csrr t0, mepc
csrr a0, mcause
```

S 态：

```asm
csrr t0, sstatus
csrr t0, sepc
csrr a0, scause
```

如果 S 态代码访问 `mstatus/mepc/mcause`，CPU 会产生非法指令异常。

### 6.3 异步中断判断

端口通过右移 cause 最高位判断 Trap 类型：

```asm
srli a2, a0, __riscv_xlen - 1
beq  a2, x0, handle_synchronous
```

异步中断中，`sepc` 指向被打断的指令，返回时不应主动增加。

同步异常是否应该增加 `sepc`，必须根据具体异常类型决定。

### 6.4 Supervisor software interrupt

S 态软件中断 cause 为 1：

```asm
interrupt_bit | 1
```

处理逻辑：

```asm
load_x sp, xISRStackTop
jal vPortClearIpiInterrupt
jal vTaskSwitchContext
j processed_source
```

这里先切换到 ISR 专用栈，避免 C 函数和调度器继续消耗当前任务栈。

### 6.5 Supervisor timer interrupt

S 态 Timer cause 为 5：

```asm
interrupt_bit | 5
```

处理逻辑：

```asm
load_x sp, xISRStackTop
jal vPortSetupTimerInterrupt
jal xTaskIncrementTick
beqz a0, processed_source
jal vTaskSwitchContext
```

`xTaskIncrementTick()` 返回非零，表示有更高优先级任务变为 Ready，需要进行上下文切换。

### 6.6 Supervisor external interrupt

S 态外部中断 cause 为 9：

```asm
interrupt_bit | 9
```

端口调用：

```asm
jal handle_interrupt
```

真正的 `handle_interrupt()` 应完成：

1. 从 PLIC Supervisor claim 寄存器读取 IRQ。
2. 调用对应设备 ISR。
3. 向 PLIC complete 寄存器写回 IRQ。
4. 根据 ISR 结果决定是否切换任务。

当前弱符号默认实现会进入死循环，不能作为正式外部中断处理器。

### 6.7 上下文恢复

M 态恢复：

```asm
csrw mepc, t0
csrw mstatus, t0
mret
```

S 态恢复：

```asm
csrw sepc, t0
csrw sstatus, t0
sret
```

`sret` 会：

1. 从 `sepc` 取得返回 PC。
2. 根据 `sstatus.SPP` 决定返回 S 态还是 U 态。
3. 将 `SPIE` 恢复到 `SIE`。
4. 更新 `SPIE/SPP`。

### 6.8 第一个任务启动

S 态版本首先设置：

```asm
la   t0, freertos_risc_v_trap_handler
csrw stvec, t0
```

然后从 `pxCurrentTCB` 取出第一个任务栈，恢复寄存器并设置：

```asm
sstatus.SIE = 1
```

最后通过 `ret` 跳转到任务入口。

后续任务切换发生在 Trap 上下文中，使用 `sret` 返回任务。

### 6.9 初始任务 `sstatus`

M 态任务初始状态使用：

```text
MPIE = 1
MPP  = M
合计 0x1880
```

S 态任务初始状态使用：

```text
SPIE = 1，bit 5，0x020
SPP  = 1，bit 8，0x100
SIE  = 0，bit 1 清零
合计 0x120
```

对应代码：

```asm
csrr t0, sstatus
andi t0, t0, ~0x2
addi t1, x0, 0x120
or   t0, t0, t1
```

`SPP=1` 保证上下文恢复执行 `sret` 后仍然返回 S 态。

### 6.10 同步异常处理风险

当前端口对所有同步异常执行：

```asm
addi a1, a1, 4
store_x a1, 0(sp)
```

存在两个问题：

1. 开启 RVC 后，异常指令可能只有 2 字节。
2. 非法指令、访问异常等通常不能直接跳过。

建议由 `handle_trap()` 根据 `scause` 决定是否修改保存的 `sepc`。未知异常应记录：

```text
scause
sepc
stval
sstatus
```

然后停止系统，而不是无条件继续执行下一条指令。

### 6.11 浮点上下文

当前编译架构包含 `F/D`，但端口没有保存：

- `f0-f31`
- `fcsr`
- 与浮点状态相关的 `sstatus.FS`

因此必须选择一种策略：

1. FreeRTOS 和所有任务使用 soft-float，不执行浮点指令。
2. 为任务增加完整浮点上下文保存和恢复。
3. 实现基于 `sstatus.FS` 的惰性浮点上下文切换。

在没有实现浮点上下文前，不应允许多个任务使用硬件浮点。

## 7. 从零开始移植的推荐步骤

### 步骤 1：固定参考版本

选择与 FreeRTOS 内核完全一致的 RISC-V M 态端口作为起点，不要混用 10.4.4 与 10.4.3 LTS 文件。

建议新建独立目录，例如：

```text
portable/GCC/RISC-V-S-mode/
```

避免覆盖仍可能需要的官方 M 态端口。

### 步骤 2：确认 M 态运行时

选择：

- OpenSBI；或
- 自定义 M 态 Monitor。

M 态运行时必须提供：

- 进入 S 态的能力。
- PMP/内存访问配置。
- TIME SBI 服务。
- IPI SBI 服务。
- 必要的异常和中断委托。
- `time` CSR 访问权限。

### 步骤 3：配置 OpenSBI Domain

S 态 FreeRTOS 对应 Domain 必须满足：

```dts
next-mode = <1>;
next-addr = <FreeRTOS 实际入口地址>;
```

并授权访问：

- 代码、数据、堆和栈。
- UART。
- PLIC Supervisor context。
- 业务外设 MMIO。

### 步骤 4：统一地址

下面三项必须完全一致：

```text
OpenSBI next-addr
Boot Loader 实际复制/加载地址
link.lds 中的链接地址
```

如果使用 raw binary，不能把链接到一个地址的镜像随意复制到另一个地址，除非整个程序被设计为位置无关代码且数据地址也经过正确处理。

### 步骤 5：编写 S 态 startup

启动代码应完成：

1. 使用 OpenSBI 传入的 `a0` 获取 hart ID。
2. 初始化 `gp`。
3. 为目标 hart 设置栈。
4. 清除或关闭 `sie`。
5. 初始化 `.data`。
6. 清零 `.bss`。
7. 必要时设置 `satp=0` 并执行 `sfence.vma`。
8. 调用 `main()`。

S 态不能使用 `mhartid`，除非 M 态运行时提供了其他访问机制。

### 步骤 6：修改临界区和 CSR

完成：

```text
mstatus.MIE → sstatus.SIE
mtvec       → stvec
mepc        → sepc
mcause      → scause
mtval       → stval
mret        → sret
```

### 步骤 7：实现 S 态 Tick

使用：

```text
rdtime + SBI TIME
```

不能继续依赖 M 态 `mtimecmp` 中断处理路径。

### 步骤 8：实现主动 Yield

可选择：

- SBI 自 IPI，产生 SSIP。
- 自定义 SBI 扩展，在 M 态辅助触发调度事件。
- 在明确改变 SBI/异常委托模型后，使用专门同步 Trap。

本项目选择 SBI 自 IPI。

### 步骤 9：实现 Trap handler

必须正确处理：

- SSIP/cause 1：主动任务切换。
- STIP/cause 5：Tick。
- SEIP/cause 9：外设中断。
- 同步异常：诊断或按需处理。

### 步骤 10：处理外部中断

配置：

- `sie.SEIE`
- PLIC priority
- PLIC enable
- PLIC threshold
- claim/complete

确认 OpenSBI 不会把目标设备中断留在 M 态处理。

### 步骤 11：处理浮点和扩展寄存器

根据实际 ISA 和编译选项处理：

- F/D 浮点寄存器。
- Vector 寄存器。
- 芯片私有寄存器。

上下文保存范围必须与任务可能使用的硬件状态一致。

### 步骤 12：逐级验证

推荐测试顺序：

1. 确认 OpenSBI 以 S 态跳到 `_start`。
2. 确认 `gp/sp/.data/.bss` 正确。
3. 确认能够写 `stvec/sstatus/sie`。
4. 启动一个不依赖 Tick 的任务。
5. 验证 `taskYIELD()` 和 SSIP。
6. 验证 STIP 和 Tick 递增。
7. 验证 `vTaskDelay()`。
8. 验证抢占调度。
9. 验证临界区嵌套。
10. 验证 PLIC 外部中断。
11. 验证长时间运行时的 Tick 漂移。
12. 验证任务栈和 ISR 栈溢出检测。

## 8. 当前工程问题清单

### 8.1 Trusted Domain 被配置成 U 态

当前 `dts/quard_star_sbi.dts`：

```dts
tdomain: trusted-domain {
    next-addr = <0x0 0xB0000000>;
    next-mode = <0x0>;
};
```

OpenSBI 中：

```text
0 = U mode
1 = S mode
```

而 FreeRTOS 启动代码会使用 `sie/sstatus/stvec`。如果实际以 U 态进入，第一条 S 态 CSR 指令就会产生非法指令异常。

如果目标确实是 S 态 FreeRTOS，应设置：

```dts
next-mode = <1>;
```

### 8.2 入口地址与链接地址不一致

当前设备树和 Boot 使用：

```text
0xB0000000
```

而 `trusted_domain/link.lds` 使用：

```text
ROM = 0xBF800000
RAM = 0xBF900000
```

必须选择一套地址，并同步修改：

- OpenSBI Domain `next-addr`。
- Boot Loader 复制地址。
- 链接脚本 ROM/RAM 地址。
- 固件打包偏移与加载逻辑。

### 8.3 顶层构建脚本仍引用 `startup.s`

当前 `build.sh` 编译：

```text
trusted_domain/startup.s
```

但当前启动文件是：

```text
trusted_domain/startup.S
```

原小写文件已经删除。

此外，顶层脚本只链接 `startup.o`，没有链接：

- FreeRTOS Kernel
- `port.c`
- `portASM.S`
- heap
- driver
- `main.c`

完整 FreeRTOS 构建目前在 `trusted_domain/Makefile` 中。顶层构建应调用该 Makefile，并将生成的 `trusted_fw.bin` 写入最终固件。

### 8.4 外部中断没有使能

当前只使能：

```text
SSIE
STIE
```

如果需要 UART 或其他 PLIC 中断，还要使能：

```text
SEIE
```

并提供真实的 `handle_interrupt()`。

### 8.5 Timer 周期可能漂移

当前使用：

```text
next = now + period
```

建议改为：

```text
next += period
```

从而避免中断处理耗时累积。

### 8.6 同步异常不能统一跳过 4 字节

当前所有同步异常都会让保存的 PC 增加 4。开启 RVC 或遇到不可恢复异常时，这种处理不安全。

应由异常处理函数根据 `scause` 和指令长度决定是否修改 `sepc`。

### 8.7 浮点上下文未保存

当前架构选项包含 F/D，但 FreeRTOS 上下文只有整数寄存器。应禁止任务使用硬件浮点，或补充浮点上下文切换。

### 8.8 `configTASK_RETURN_ADDRESS` 没有真正生效

虽然 `port.c` 定义了 `portTASK_RETURN_ADDRESS`，但 S 态汇编仍直接写入 `prvTaskExitError`。

如果希望支持用户覆盖任务返回地址，需要统一 C 和汇编之间的符号定义。

## 9. 建议的最终运行链

```text
hart 7 Reset/M 态
    ↓
Boot Loader 将 OpenSBI 和 FreeRTOS 加载到各自链接地址
    ↓
OpenSBI 创建 trusted domain
    ├── possible-harts = hart 7
    ├── next-mode = S
    ├── next-addr = FreeRTOS _start
    └── 授权 RAM、UART、PLIC 和业务 MMIO
    ↓
OpenSBI mret 到 FreeRTOS startup.S
    ↓
FreeRTOS 初始化内存并进入 main()
    ↓
vTaskStartScheduler()
    ├── stvec = freertos_risc_v_trap_handler
    ├── SBI 设置第一次 Timer
    ├── sie.STIE = 1
    ├── sie.SSIE = 1
    └── 必要时 sie.SEIE = 1
    ↓
运行第一个任务
    ├── Tick：STIP/cause 5
    ├── Yield：SSIP/cause 1
    └── 外设：SEIP/cause 9
```

## 10. 总结

本项目三份端口文件已经完成了主要的 S 态转换：

- 机器级 CSR 改为 Supervisor CSR。
- `mret` 改为 `sret`。
- Machine timer 改为 SBI TIME 和 Supervisor timer interrupt。
- M 态 `ecall` Yield 改为 SBI 自 IPI 和 Supervisor software interrupt。
- Trap cause 从 M 态编号改为 S 态编号。
- 初始任务状态从 `MPP/MPIE` 改为 `SPP/SPIE`。

但 FreeRTOS 能否真正以 S 态运行，不只由这三个端口文件决定。OpenSBI Domain、PMP 权限、入口特权级、镜像加载地址、链接地址、PLIC 配置和构建脚本必须形成一致的完整启动链。

当前最优先需要解决的是：

1. 将 trusted domain 的 `next-mode` 与 S 态端口统一。
2. 统一 `next-addr`、Boot 加载地址和链接地址。
3. 让顶层构建真正编译并打包完整 FreeRTOS 固件。
4. 根据需求补充 SEIE/PLIC、异常诊断和浮点上下文。
