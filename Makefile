all: signal leetify

signal: signal.c
	gcc -Wall -g $< -o $@

leetify: leetify.c
	gcc -Wall -g $< -o $@

clean:
	rm -f leetify signal
