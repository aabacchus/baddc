#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RETURN_IF_FEWER_THAN(n) if (stack_len() < n) { fprintf(stderr, "stack too short!\n"); return; }

#define STACK_LEN 50

int stack[STACK_LEN] = {0};
int *stackp = stack - 1;

int ibase = 10;
int obase = 10;

void oprint(int a) {
	char fmt[4] = {'%', 0,  '\n', '\0'};
	switch (obase) {
		case 16:
			fmt[1] = 'X';
			break;
		case 10:
			fmt[1] = 'd';
			break;
		case 8:
			fmt[1] = 'o';
			break;
		default:
			fprintf(stderr, "output base %d not supported!\n", obase);
			return;
	}
	printf(fmt, a);
}

void push(int a) {
	stackp++;
	if (stackp - stack >= STACK_LEN) {
		fprintf(stderr, "stack overflow!\n");
		stackp--;
	} else {
		*stackp = a;
	}
}

int pop(void) {
	if (stackp == stack - 1) {
		fprintf(stderr, "no value on stack!\n");
		return 0;
	}
	return *stackp--;
}

int peek(void) {
	if (stackp == stack - 1) {
		fprintf(stderr, "no value on stack!\n");
		return 0;
	}
	return *stackp;
}

int stack_len(void) {
	return 1 + (stackp - stack);
}

void view_stack(void) {
	for (int *p = stackp; p - stack >= 0; p--)
		oprint(*p);
}

void dup(void) {
	RETURN_IF_FEWER_THAN(1);
	push(peek());
}

void clear(void) {
	stackp = stack - 1;
}

void add(void) {
	RETURN_IF_FEWER_THAN(2);
	push(pop() + pop());
}

void subtract(void) {
	RETURN_IF_FEWER_THAN(2);
	int a = pop();
	push(-a + pop());
}

void multiply(void) {
	RETURN_IF_FEWER_THAN(2);
	push(pop() * pop());
}

void divide(void) {
	RETURN_IF_FEWER_THAN(2);
	int div = pop();
	if (div == 0) {
		fprintf(stderr, "division by 0!\n");
		pop();
		push(0);
		return;
	}
	push(pop() / div);
}

void i_base(void) {
	RETURN_IF_FEWER_THAN(1);
	int i = pop();
	if (i > 16 || i < 1)
		fprintf(stderr, "input base %d not supported!\n", i);
	else
		ibase = i;
}

void I_base(void) {
	push(ibase);
}

void o_base(void) {
	RETURN_IF_FEWER_THAN(1);
	int o = pop();
	if (o != 8 && o != 10 && o != 16)
		fprintf(stderr, "output base %d not supported!\n", o);
	else
		obase = o;
}

void O_base(void) {
	push(obase);
}

void mod(void) {
	RETURN_IF_FEWER_THAN(2);
	int a = pop();
	if (a == 0) {
		fprintf(stderr, "division by 0!\n");
		pop();
		push(0);
		return;
	}
	push(pop() % a);
}

void power(void) {
	RETURN_IF_FEWER_THAN(2);
	int a = pop();
	push((int)pow((double)pop(), (double)a));
}

void root(void) {
	RETURN_IF_FEWER_THAN(1);
	int a = pop();
	if (a < 0) {
		fprintf(stderr, "root of negative number!\n");
	}
	push((int)sqrt((double)a));
}

void drop(void) {
	RETURN_IF_FEWER_THAN(1);
	pop();
}

struct {
	char op;
	void (*f)(void);
} ops[] = {
	{ 'f', view_stack },
	{ 'd', dup },
	{ 'c', clear },
	{ '+', add },
	{ '-', subtract },
	{ '*', multiply },
	{ '/', divide },
	{ 'i', i_base },
	{ 'I', I_base },
	{ 'o', o_base },
	{ 'O', O_base },
	{ '%', mod },
	{ '^', power },
	{ 'v', root },
	{ 'r', drop },
};

#define NUM_OPS ((sizeof(ops)) / (sizeof(*ops)))

int main(int argc, char **argv) {
	FILE *fp = NULL;
	if (argc == 1) {
		fp = stdin;
	} else {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			perror(argv[1]);
			return 1;
		}
	}

	char *buf = NULL;
	size_t buflen = 0;
	ssize_t n = 0;

	int curnum = 0;
	int numready = 0;
	int neg = 1;
	while ((n = getline(&buf, &buflen, fp)) != -1) {
		for (ssize_t i = 0; i < n - 1; i++) {
			if (buf[i] == 'q')
				goto end;
			if (buf[i] == '_')
				neg = -1;
			else if (buf[i] == '.')
				fprintf(stderr, "no floating point please!\n");
			else if (isdigit(buf[i])) {
				curnum = (curnum * ibase) + (buf[i] - '0');
				numready = 1;
			} else if (buf[i] >= 'A' && buf[i] <= 'F') {
				curnum = (curnum * ibase) + (buf[i] - 'A' + 10);
				numready = 1;
			} else {
				if (numready) {
					push(curnum * neg);
					numready = 0;
					neg = 1;
				}
				curnum = 0;
				if (isspace(buf[i])) {
					continue;
				} if (isdigit(buf[i])) {
					push(buf[i] - '0');
				} else if (buf[i] == 'p') {
					if (stack_len() < 1)
						fprintf(stderr, "stack too short!\n");
					else
						oprint(peek());
				} else {
					for (size_t j = 0; j < NUM_OPS; j++) {
						if (buf[i] == ops[j].op) {
							ops[j].f();
						}
					}
				}
			}
		}
	}
end:
	free(buf);
	if (fp != stdin)
		fclose(fp);

	return 0;
}
