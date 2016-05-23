#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

int RandomCoefficient(void) {
	
	return((RAND_MAX/2 - rand()) % 65535);

}

void RandomJunk(char *buf) {
	char tmp[100];

	sprintf(tmp, ".long 0x%04x02eb", rand()%65535);
	//sprintf(buf, "%s\tasm __volatile__ (\".long 0xaaaa02eb\");", buf);
	sprintf(buf, "%s\tasm __volatile__ (\"%s\");\n", buf, tmp);

}

uint32_t CoefficientCount = 0;
void ObscureCoefficient(char *Formulas, char *Variables, int C) {
	uint8_t option;
	uint8_t shift;
	uint8_t done = 0;
	int xor = rand()%255;

	while (!done) {
		option = rand() % 4;
		switch (option) {
			case 0:
				sprintf(Variables, "%s\tC[%d] = %d;\n", Variables, CoefficientCount, abs(C));
				RandomJunk(Variables);
				sprintf(Formulas, "%s(C[%d])", Formulas, CoefficientCount++);
				done = 1;
				break;
			case 1:
				shift = (rand() % 4) + 1;
				sprintf(Variables, "%s\tC[%d] = %d;\n", Variables, CoefficientCount, abs(C)<<shift);
				sprintf(Variables, "%s\tC[%d] = C[%d]>>%d;\n", Variables, CoefficientCount, CoefficientCount, shift);
				sprintf(Formulas, "%s(C[%d])", Formulas, CoefficientCount++);
				done = 1;
				break;
			case 2:
				if (abs(C) % 2 == 0) {
					RandomJunk(Variables);
					sprintf(Variables, "%s\tC[%d] = %d;\n", Variables, CoefficientCount, abs(C)>>1);
					sprintf(Variables, "%s\tC[%d] = C[%d]<<1;\n", Variables, CoefficientCount, CoefficientCount);
					sprintf(Formulas, "%s(C[%d])", Formulas, CoefficientCount++);
					done = 1;
				}
				break;
			case 3:
				RandomJunk(Variables);
				sprintf(Variables, "%s\tC[%d] = %d;\n", Variables, CoefficientCount, abs(C)^xor);
				sprintf(Variables, "%s\tC[%d] = C[%d]^%d;\n", Variables, CoefficientCount, CoefficientCount, xor);
				sprintf(Formulas, "%s(C[%d])", Formulas, CoefficientCount++);
				done = 1;
				break;
		}
			
	}

}

void CreateLinearSystem(unsigned char *buf) {
	uint16_t NumVariables = strlen(buf);
	uint16_t NumEquations = NumVariables;
	uint16_t i, j;
	int Coefficient;
	int Sum;
	char Variables[32768];
	char Formulas[32768];

	printf("#include <stdio.h>\n");
	printf("#include <stdint.h>\n");

	printf("uint8_t CheckSolution(int32_t Var[]) {\n");
		printf("\tvolatile int32_t C[%d];\n", NumVariables * NumEquations);

	for (j = 0; j < NumEquations; j++) {
		Sum = 0;
		sprintf(Formulas, "%s\tif (", Formulas);
		for (i = 0; i < NumVariables; i++) {
			Coefficient = RandomCoefficient();
//			fprintf(stderr, "Coefficients: %d\n",  Coefficient);
			Sum += buf[i]*Coefficient;
			if (i == 0) {
				if (Coefficient >= 0) {
					ObscureCoefficient(Formulas, Variables, Coefficient);
				} else {
					sprintf(Formulas,"%s- ", Formulas);
					ObscureCoefficient(Formulas, Variables, Coefficient);
				}
			} else {
				if (Coefficient >= 0) {
					sprintf(Formulas,"%s+ ", Formulas);
					ObscureCoefficient(Formulas, Variables, Coefficient);
				} else {
					sprintf(Formulas,"%s- ", Formulas);
					ObscureCoefficient(Formulas, Variables, Coefficient);
				}
			}
			sprintf(Formulas,"%s*Var[%d] ", Formulas, i);
		}
		sprintf(Formulas, "%s != %d", Formulas, Sum);
		sprintf(Formulas, "%s) {\n", Formulas);
		RandomJunk(Formulas);
		sprintf(Formulas, "%s\t\treturn(0);\n", Formulas);
		sprintf(Formulas, "%s\t}\n", Formulas);
	}
	printf("%s\n", Variables);
	printf("%s\n", Formulas);
	printf("\treturn(1);\n");
	printf("}\n");
	printf("\n");

	// create main
	printf("int main(void) {\n");
	printf("\tint32_t Var[%d];\n", NumVariables);
	printf("\n");

	for (i = 0; i < NumVariables; i++) {
        	printf("\tprintf(\"Var[%d]: \");\n", i);
        	printf("\tscanf(\"%%d\", &Var[%d]);\n", i);
		printf("\n");
	}
        printf("\tif (CheckSolution(Var)) {\n");

        printf("\t\tprintf(\"The flag is: ");
	for (i = 0; i < NumVariables; i++) {
		printf("%%c");
	}
	printf("\\n\",");
	for (i = 0; i < NumVariables; i++) {
		printf("Var[%d]", i);
		if (i != NumVariables-1) {
			printf(",");
		}
	}
	printf(");\n");
        printf("\t} else {\n");
	printf("\t\tprintf(\"Wrong\\n\");\n");
        printf("\t}\n");
	printf("\n");
        printf("\treturn(0);\n");
	printf("}\n");

}

int main(void) {
	unsigned char buf[100];

	bzero(buf, 100);
	printf("The flag is: ");
	fgets(buf, 99, stdin);
	buf[strlen(buf)-1] = '\0';

	srand(*((unsigned int *)buf));

	CreateLinearSystem(buf);

	return(0);
}
