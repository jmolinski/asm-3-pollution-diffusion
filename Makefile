all: pollution_diffusion

pollution_diffusion: pollution_diffusion.c start_step.o
	gcc -std=c99 -g -o $@ $^ -lm

start_step.o: start_step.s
	as -o $@ $<

clean:
	rm -vf pollution_diffusion *.o
