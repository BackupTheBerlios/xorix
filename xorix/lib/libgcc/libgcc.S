
/*
Xorix GCC Library
Copyright (C) 2002 Ingmar Friedrichsen <ingmar@xorix.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/*
This library is based on the AMD Athlon(TM) Processor 
x86 Code Optimization Guide (22007), which was written
by Advanced Micro Devices, Inc. AMD makes NO WARRANTY.
*/

.globl __divdi3,__moddi3
.globl __udivdi3,__umoddi3

.align 4
__divdi3:
		pushl %ebx
		pushl %esi
		pushl %edi
		movl 28(%esp),%ecx
		movl 24(%esp),%ebx
		movl 20(%esp),%edx
		movl 16(%esp),%eax
		movl %ecx,%esi
		xorl %edx,%esi
		sarl $31,%esi
		movl %edx,%edi
		sarl $31,%edi
		xorl %edi,%eax
		xorl %edi,%edx
		subl %edi,%eax
		sbbl %edi,%edx
		movl %ecx,%edi
		sarl $31,%edi
		xorl %edi,%ebx
		xorl %edi,%ecx
		subl %edi,%ebx
		sbbl %edi,%ecx
		jnz 1f
		cmpl %ebx,%edx
		jae 0f
		divl %ebx
		movl %ecx,%edx
		xorl %esi,%eax
		xorl %esi,%edx
		subl %esi,%eax
		sbbl %esi,%edx
		popl %edi
		popl %esi
		popl %ebx
		ret
0:		movl %eax,%ecx
		movl %edx,%eax
		xorl %edx,%edx
		divl %ebx
		xchgl %ecx,%eax
		divl %ebx
		movl %ecx,%edx
		jmp 2f
1:		subl $12,%esp
		movl %eax,(%esp)
		movl %ebx,4(%esp)
		movl %edx,8(%esp)
		movl %ecx,%edi
		shrl $1,%edx
		rcrl $1,%eax
		rorl $1,%edi
		rcrl $1,%ebx
		bsrl %ecx,%ecx
		shrdl %cl,%edi,%ebx
		shrdl %cl,%edx,%eax
		shrl %cl,%edx
		roll $1,%edi
		divl %ebx
		movl (%esp),%ebx
		movl %eax,%ecx
		imull %eax,%edi
		mull 4(%esp)
		addl %edi,%edx
		subl %eax,%ebx
		movl %ecx,%eax
		movl 8(%esp),%ecx
		sbbl %edx,%ecx
		sbbl $0,%eax
		xorl %edx,%edx
		addl $12,%esp
2:		xorl %esi,%eax
		xorl %esi,%edx
		subl %esi,%eax
		sbbl %esi,%edx
		popl %edi
		popl %esi
		popl %ebx
		ret

.align 4
__moddi3:
		pushl %ebx
		pushl %esi
		pushl %edi
		movl 28(%esp),%ecx
		movl 24(%esp),%ebx
		movl 20(%esp),%edx
		movl 16(%esp),%eax
		movl %edx,%esi
		sarl $31,%esi
		movl %edx,%edi
		sarl $31,%edi
		xorl %edi,%eax
		xorl %edi,%edx
		subl %edi,%eax
		sbbl %edi,%edx
		movl %ecx,%edi
		sarl $31,%edi
		xorl %edi,%ebx
		xorl %edi,%ecx
		subl %edi,%ebx
		sbbl %edi,%ecx
		jnz 1f
		cmpl %ebx,%edx
		jae 0f
		divl %ebx
		movl %edx,%eax
		movl %ecx,%edx
		xorl %esi,%eax
		xorl %esi,%edx
		subl %esi,%eax
		sbbl %esi,%edx
		popl %edi
		popl %esi
		popl %ebx
		ret
0:		movl %eax,%ecx
		movl %edx,%eax
		xorl %edx,%edx
		divl %ebx
		movl %ecx,%eax
		divl %ebx
		movl %edx,%eax
		xorl %edx,%edx
		jmp 2f
1:		subl $16,%esp
		movl %eax,(%esp)
		movl %ebx,4(%esp)
		movl %edx,8(%esp)
		movl %ecx,12(%esp)
		movl %ecx,%edi
		shrl $1,%edx
		rcrl $1,%eax
		rorl $1,%edi
		rcrl $1,%ebx
		bsrl %ecx,%ecx
		shrdl %cl,%edi,%ebx
		shrdl %cl,%edx,%eax
		shrl %cl,%edx
		roll $1,%edi
		divl %ebx
		movl (%esp),%ebx
		movl %eax,%ecx
		imull %eax,%edi
		mull 4(%esp)
		addl %edi,%edx
		subl %eax,%ebx
		movl 8(%esp),%ecx
		sbbl %edx,%ecx
		sbbl %eax,%eax
		movl 12(%esp),%edx
		andl %eax,%edx
		andl 4(%esp),%eax
		addl %ebx,%eax
		addl %ecx,%edx
		addl $16,%esp
2:		xorl %esi,%eax
		xorl %esi,%edx
		subl %esi,%eax
		sbbl %esi,%edx
		popl %edi
		popl %esi
		popl %ebx
		ret

.align 4
__udivdi3:
		pushl %ebx
		movl 20(%esp),%ecx
		movl 16(%esp),%ebx
		movl 12(%esp),%edx
		movl 8(%esp),%eax
		testl %ecx,%ecx
		jnz 1f
		cmpl %ebx,%edx
		jae 0f
		divl %ebx
		movl %ecx,%edx
		popl %ebx
		ret
0:		movl %eax,%ecx
		movl %edx,%eax
		xorl %edx,%edx
		divl %ebx
		xchgl %ecx,%eax
		divl %ebx
		movl %ecx,%edx
		popl %ebx
		ret
1:		pushl %edi
		movl %ecx,%edi
		shrl $1,%edx
		rcrl $1,%eax
		rorl $1,%edi
		rcrl $1,%ebx
		bsrl %ecx,%ecx
		shrdl %cl,%edi,%ebx
		shrdl %cl,%edx,%eax
		shrl %cl,%edx
		roll $1,%edi
		divl %ebx
		movl 12(%esp),%ebx
		movl %eax,%ecx
		imull %eax,%edi
		mull 20(%esp)
		addl %edi,%edx
		subl %eax,%ebx
		movl %ecx,%eax
		movl 16(%esp),%ecx
		sbbl %edx,%ecx
		sbbl $0,%eax
		xorl %edx,%edx
		popl %edi
		popl %ebx
		ret

.align 4
__umoddi3:
		pushl %ebx
		movl 20(%esp),%ecx
		movl 16(%esp),%ebx
		movl 12(%esp),%edx
		movl 8(%esp),%eax
		testl %ecx,%ecx
		jnz 1f
		cmpl %ebx,%edx
		jae 0f
		divl %ebx
		movl %edx,%eax
		movl %ecx,%edx
		popl %ebx
		ret
0:		movl %eax,%ecx
		movl %edx,%eax
		xorl %edx,%edx
		divl %ebx
		movl %ecx,%eax
		divl %ebx
		movl %edx,%eax
		xorl %edx,%edx
		popl %ebx
		ret
1:		pushl %edi
		movl %ecx,%edi
		shrl $1,%edx
		rcrl $1,%eax
		rorl $1,%edi
		rcrl $1,%ebx
		bsrl %ecx,%ecx
		shrdl %cl,%edi,%ebx
		shrdl %cl,%edx,%eax
		shrl %cl,%edx
		roll $1,%edi
		divl %ebx
		movl 12(%esp),%ebx
		movl %eax,%ecx
		imull %eax,%edi
		mull 20(%esp)
		addl %edi,%edx
		subl %eax,%ebx
		movl 16(%esp),%ecx
		movl 20(%esp),%eax
		sbbl %edx,%ecx
		sbbl %edx,%edx
		andl %edx,%eax
		andl 24(%esp),%edx
		addl %ebx,%eax
		adcl %ecx,%edx
		popl %edi
		popl %ebx
		ret



