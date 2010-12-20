/* finite state machine example */

#include <stdio.h>

#define MAXARG 20
#define STATE_BEGIN 0
#define STATE_OPEN_PAREN 1
#define STATE_CLOSE_PAREN 2
#define STATE_PROC 3
#define STATE_ARG 4
#define STATE_END 5
#define STATE_ERROR 6

void parse(char *buf);

int main(void) {
	parse("(   (+) 2 (- 6    1) (/ 5 (* 4 4)))");
	return 0;
}

void parse(char *buf) {
	int state = STATE_BEGIN;	
	int layer = 0;
	char *ptr = buf;
	char arg[MAXARG];
	int i = 0; 		

	memset(arg, 0, MAXARG);
	state = STATE_BEGIN;
	do {
		if (state == STATE_BEGIN) {
			/* go to first open paren */
			if (*ptr == '(') {
				printf("going up\n");
				layer++;
				state = STATE_OPEN_PAREN;
			}
		}
		else if (state == STATE_OPEN_PAREN) {
			if (*ptr == ')')
				state = STATE_ERROR;
			else if (*ptr == '(') {
				printf("going up\n");
				layer++;
				state = STATE_OPEN_PAREN;
			}
			else if (*ptr != ' ') {
				arg[i++] = *ptr;
				state = STATE_PROC;
			}
		}
		else if (state == STATE_CLOSE_PAREN) {
			if (layer < 0)
				state = STATE_ERROR;
			else if (layer == 0)
				break;
			/* go to next arg */
			if (*ptr == ')') {
				printf("going down\n");
				layer--;
				state = STATE_CLOSE_PAREN;
			}
			else if (*ptr == '(') {
				printf("going up\n");
				layer++;
				state = STATE_OPEN_PAREN;
			}
			else if (*ptr != ' ') {
				arg[i++] = *ptr;
				state = STATE_ARG;
			}
		}
		else if (state == STATE_PROC || STATE_ARG) {
			if (arg[0] != '\0' && (*ptr == ' ' || *ptr == ')')) {
				/* print buffer */
				if (state == STATE_PROC)
					printf("proc: %s\n", arg);
				else
					printf("arg: %s\n", arg);
				/* flush buffer */
				memset(arg, 0, MAXARG);
				i = 0;
				/* read next argument or close paren */
				state = STATE_ARG;
			}
			if (*ptr == ')') {
				printf("going down\n");
				layer--;
				state = STATE_CLOSE_PAREN;
			}
			else if (*ptr == '(') {
				printf("going up\n");
				layer++;
				state = STATE_OPEN_PAREN;
			}
			else if (*ptr != ' ')
				arg[i++] = *ptr;
		}
		ptr++;
	} while (state != STATE_ERROR);

	if (state == STATE_ERROR) 
		printf("Something went wrong\n");
}
