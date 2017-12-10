target: control.c client.c
	gcc control.c -o control
	gcc client.c -o client
run: control client
	./control
clean: control client
	rm control
	rm client
