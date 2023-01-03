.code

asm_getcf PROC
	mov EAX,6
	lock ADD QWORD PTR [RSP], 50
	ret
asm_getcf endp
end