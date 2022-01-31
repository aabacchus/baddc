#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STACK_LEN 50

int stack[STACK_LEN] = {0};
int *stackp = stack - 1;

int ibase = 10;
int obase = 10;

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

void view_stack(void) {
	for (int *p = stackp; p - stack >= 0; p--)
		printf("%d\n", *p);
}

void dup(void) {
	push(peek());
}

void clear(void) {
	stackp = stack - 1;
}

void add(void) {
	push(pop() + pop());
}

void subtract(void) {
	int a = pop();
	push(-a + pop());
}

void multiply(void) {
	push(pop() * pop());
}

void divide(void) {
	int div = pop();
	push(pop() / div);
}

void i_base(void) {
	ibase = pop();
}

void I_base(void) {
	push(ibase);
}

void o_base(void) {
	obase = pop();
}

void O_base(void) {
	push(obase);
}


struct map {
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
};

#define NUM_OPS ((sizeof(ops)) / (sizeof(*ops)))

int main(void) {
	char *buf = NULL;
	size_t buflen = 0;
	ssize_t n = 0;

	int curnum = 0;
	int numready = 0;
	int neg = 1;
	while ((n = getline(&buf, &buflen, stdin)) != -1) {
		for (ssize_t i = 0; i < n - 1; i++) {
			if (buf[i] == 'q')
				goto end;
			if (buf[i] == '_')
				neg = -1;
			else if (buf[i] == '.')
				fprintf(stderr, "no floating point please\n");
			else if (isdigit(buf[i])) {
				curnum = (curnum * ibase) + (buf[i] - '0');
				numready = 1;
			} else {
				if (numready) {
					push(curnum * neg);
					numready = 0;
					neg = 1;
				}
				curnum = 0;
				if (isspace(buf[i])) {
					//fprintf(stderr, "space ");
					continue;
				} if (isdigit(buf[i])) {
					//fprintf(stderr, "push('%d') ", buf[i]);
					push(buf[i] - '0');
				} else if (buf[i] == 'p') {
					//fprintf(stderr, "peek ");
					printf("%d\n", peek());
				} else {
					for (size_t j = 0; j < NUM_OPS; j++) {
						if (buf[i] == ops[j].op) {
							//fprintf(stderr, "execute '%c' ", buf[i]);
							ops[j].f();
						}
					}
				}
			}
		}
	}
end:
	free(buf);

	return 0;
}
