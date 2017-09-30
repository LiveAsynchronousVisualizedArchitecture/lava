	.text
	.def	 first;
	.scl	2;
	.type	32;
	.endef
	.globl	first                   # -- Begin function first
	.p2align	4, 0x90
first:                                  # @first
# BB#0:
	movl	$85, %eax
	retq
                                        # -- End function
	.def	 main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.Lcfi0:
.seh_proc main
# BB#0:
	subq	$56, %rsp
.Lcfi1:
	.seh_stackalloc 56
.Lcfi2:
	.seh_endprologue
	movl	$0, 52(%rsp)
	movq	%rdx, 40(%rsp)
	movl	%ecx, 36(%rsp)
	callq	first
	xorl	%ecx, %ecx
	movl	%eax, 32(%rsp)
	movl	%ecx, %eax
	addq	$56, %rsp
	retq
	.seh_handlerdata
	.text
.Lcfi3:
	.seh_endproc
                                        # -- End function

