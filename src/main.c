#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *get_reg(char var) {
	switch (var) {
	case 'a': return "eax";
	case 'b': return "ebx";
	case 'c': return "ecx";
	case 'd': return "edx";
	default: return NULL;
	}
}

void logic_op(char left_var, const char *expr) {
	const char *reg_stanga = get_reg(left_var);
	char *op = strpbrk(expr, "+-*/^<&|>");
	const char *reg_dreapta = NULL;
	char *val_start = NULL;
	if (op) {
		char operatie = *op;
		val_start = op + 1;
		if (operatie == '<' && *(op + 1) == '<') {
			val_start = op + 2;
			while (isspace(*val_start))
				val_start++;
			if (isdigit(*val_start))
				printf("SHL %s, %d\n", reg_stanga, atoi(val_start));
			return;
		}
		if (operatie == '>' && *(op + 1) == '>') {
			val_start = op + 2;
			while (isspace(*val_start))
				val_start++;
			if (isdigit(*val_start))
				printf("SHR %s, %d\n", reg_stanga, atoi(val_start));
			return;
		}
		while (isspace(*val_start))
			val_start++;
		int este_numar = isdigit(*val_start);
		int este_var = isalpha(*val_start);
		int valoare = este_numar ? atoi(val_start) : 0;
		reg_dreapta = este_var ? get_reg(*val_start) : NULL;
		if (operatie == '/') {
			if (este_numar) {
				if (valoare == 0) {
					printf("Error\n");
					return;
				}
				printf("MOV eax, %s\n", reg_stanga);
				printf("DIV %d\n", valoare);
				printf("MOV %s, eax\n", reg_stanga);
			} else if (reg_dreapta) {
				printf("MOV eax, %s\n", reg_stanga);
				printf("DIV %s\n", reg_dreapta);
				printf("MOV %s, eax\n", reg_stanga);
			}
			return;
		}
		if (operatie == '*') {
			if (este_numar) {
				if (strcmp(reg_stanga, "eax") != 0)
					printf("MOV eax, %s\n", reg_stanga);
				printf("MUL %d\n", valoare);
				if (strcmp(reg_stanga, "eax") != 0)
					printf("MOV %s, eax\n", reg_stanga);
			} else if (reg_dreapta) {
				printf("MOV eax, %s\n", reg_stanga);
				printf("MUL %s\n", reg_dreapta);
				printf("MOV %s, eax\n", reg_stanga);
			}
			return;
		}
		if (strchr("+-&|^", operatie)) {
			const char *mnemonic = NULL;
			switch (operatie) {
			case '+':
			mnemonic = "ADD";
			break;
			case '-':
			mnemonic = "SUB";
			break;
			case '&':
			mnemonic = "AND";
			break;
			case '|':
			mnemonic = "OR";
			break;
			case '^':
			mnemonic = "XOR";
			break;
			}
			if (mnemonic) {
				if (este_numar)
					printf("%s %s, %d\n", mnemonic, reg_stanga, valoare);
				else if (reg_dreapta)
					printf("%s %s, %s\n", mnemonic, reg_stanga, reg_dreapta);
			}
			return;
		}
	}
	while (isspace(*expr))
		expr++;
	if (isalpha(*expr)) {
		const char *reg_dreapta = get_reg(*expr);
		if (reg_dreapta)
			printf("MOV %s, %s\n", reg_stanga, reg_dreapta);
	} else if (isdigit(*expr)) {
		printf("MOV %s, %d\n", reg_stanga, atoi(expr));
	}
}

void if_instruction(char *cond) {
	if (!cond || !*cond)
		return;
	char *op_pos = strpbrk(cond, "><=");
	if (!op_pos)
		return;
	char comp = *op_pos;
	char next = *(op_pos + 1);
	int is_double = (next == '=');
	char left_var = cond[0];
	const char *left_reg = get_reg(left_var);
	if (!left_reg)
		return;
	char *right_op = op_pos + (is_double ? 2 : 1);
	while (isspace(*right_op))
		right_op++;
	printf("CMP %s, %s\n", left_reg, right_op);
	switch (comp) {
	case '>':
	printf(is_double ? "JL end_label\n" : "JLE end_label\n");
	break;
	case '<':
	printf(is_double ? "JG end_label\n" : "JGE end_label\n");
	break;
	case '=':
	if (is_double)
		printf("JNE end_label\n");
	break;
	}
}

void while_instruction(char *cond) {
	if (!cond || !*cond)
		return;
	char *oper_pos = strpbrk(cond, "><=");
	if (!oper_pos)
		return;
	char compar = *oper_pos;
	char next_op = *(oper_pos + 1);
	int is_double = (next_op == '=');
	char left_var = cond[0];
	const char *left_reg = get_reg(left_var);
	if (!left_reg)
		return;
	char *right_op = oper_pos + (is_double ? 2 : 1);
	while (isspace(*right_op))
		right_op++;
	printf("start_loop:\n");
	printf("CMP %s, %s\n", left_reg, right_op);
	if (compar == '>')
		printf(is_double ? "JL end_label\n" : "JLE end_label\n");
	else if (compar == '<')
		printf(is_double ? "JG end_label\n" : "JGE end_label\n");
	else if (compar == '=' && is_double)
		printf("JNE end_label\n");
}

int aux_for;
char for_buff[128];
void for_instruction(char *line_cont) {
	if (!line_cont)
		return;
	char *open_paren = strchr(line_cont, '(');
	char *close_paren = strchr(line_cont, ')');
	if (!open_paren || !close_paren)
		return;
	*close_paren = '\0';
	char *init = strtok(open_paren + 1, ";");
	char *cond = strtok(NULL, ";");
	char *incr = strtok(NULL, ";");
	if (!init || !cond || !incr)
		return;
	while (isspace(*init))
		init++;
	char init_var = *init;
	char *eq_sign_init = strchr(init, '=');
	if (eq_sign_init) {
		eq_sign_init++;
		while (isspace(*eq_sign_init))
			eq_sign_init++;
		logic_op(init_var, eq_sign_init);
	}
	printf("start_loop:\n");
	while (isspace(*cond))
		cond++;
	if_instruction(cond);
	while (isspace(*incr))
		incr++;
	char *eq_sign_incr = strchr(incr, '=');
	if (eq_sign_incr) {
		char var = *incr;
		char *rhs = eq_sign_incr + 1;
		while (isspace(*rhs))
			rhs++;
		char temp_expr[64];
		strncpy(temp_expr, rhs, sizeof(temp_expr) - 1);
		temp_expr[sizeof(temp_expr) - 1] = '\0';
		temp_expr[strcspn(temp_expr, "\r\n")] = '\0';
		snprintf(for_buff, sizeof(for_buff), "%c=%s", var, temp_expr);
	}
	aux_for = 1;
}

int main(void) {
	char input_buff[256];
	int count_if = 0;
	int aux_if = 0;
	int aux_while = 0;
	while (fgets(input_buff, sizeof(input_buff), stdin)) {
		char *line = input_buff;
		while (isspace(*line))
			line++;
		if (strncmp(line, "if (", 4) == 0) {
			char *cond = line + 4;
			char *end = strchr(cond, ')');
			if (!end)
				continue;
			*end = '\0';
			if_instruction(cond);
			aux_if = 1;
			count_if++;
			continue;
		}
		if (strncmp(line, "while (", 7) == 0) {
			char *cond = line + 7;
			char *end = strchr(cond, ')');
			if (!end)
				continue;
			*end = '\0';
			while_instruction(cond);
			aux_while = 1;
			continue;
		}
		if (strncmp(line, "for (", 5) == 0) {
			for_instruction(line);
			continue;
		}
		if (aux_while && strchr(line, '}')) {
			printf("JMP start_loop\n");
			printf("end_label:\n");
			aux_while = 0;
			continue;
		}
		if (aux_if && strchr(line, '}')) {
			printf("end_label:\n");
			aux_if = 0;
			continue;
		}
		if (aux_for && strchr(line, '}')) {
			char *eq = strchr(for_buff, '=');
			if (eq) {
				char *var = for_buff;
				while (isspace(*var))
					var++;
				char lhs = *var;
				char *rhs = eq + 1;
				while (isspace(*rhs))
					rhs++;
				logic_op(lhs, rhs);
			}
			printf("JMP start_loop\n");
			printf("end_loop:\n");
			aux_for = 0;
			continue;
		}
		if (isalpha(*line)) {
			char *eq = strchr(line, '=');
			if (eq) {
				char var = *line;
				char *rhs = eq + 1;
				while (isspace(*rhs))
					rhs++;
				logic_op(var, rhs);
			}
		}
	}

	return 0;
}
