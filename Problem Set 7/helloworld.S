.text

	.global _start

_start:
	movl	$1,%rax
	movl	$1,%rdi
	movl	$message,%rsi
	movl	$13,%rdx
	syscall
	movl	$60,%rax
	xor	%rdi,%rdi
	syscall

.data

message:	
	.ascii	"Hello, world!\n"
	len = . - message
