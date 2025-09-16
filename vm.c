/*
Assignment :
vm . c - Implement a P - machine virtual machine
Authors : Andres Palau
Language : C ( only )
To Compile :
gcc - O2 - Wall - std = c11 -o vm vm . c
To Execute ( on Eustis ) :
./ vm input . txt
where :
input . txt is the name of the file containing PM /0 instructions ;
each line has three integers ( OP L M )
Notes :
- Implements the PM /0 virtual machine described in the homework
instructions .
- No dynamic memory allocation or pointer arithmetic .
- Does not implement any VM instruction using a separate function .
- Runs on Eustis .
Class : COP 3402 - Systems Software - Fall 2025
Instructor : Dr . Jie Lin
Due Date : Friday , September 12 th , 2025
*/

#include <stdio.h>
#include <stdlib.h>

// Globally declared stack based machine that we will initialize downward
int PAS[500];

// Globally declared registers and int that keeps traack of highest point in the stack
int PC;
int BP;
int SP;
int base0;



// IR Struct

typedef struct instruction {
    int OP;
    int L;
    int M;
} instruction;

instruction IR;

int base(int bp, int L) {
    int b = bp;
    while (L > 0) {
        b = PAS[b]; // tracker
        L--;
    }
    return b;
}

void printMachine(void) {
    // lookup tables
    const char *opCode[] = {"", "LIT", "OPR", "LOD", "STO", "CAL","INC","JMP", "JPC", "SYS"};
    const char *subOps[] = {"RTN", "ADD", "SUB", "MUL", "DIV", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ"};
    
    // If OP is 2, print what is in lookup table
    if (IR.OP == 2) {
        // If M is out of bounds of 0-10
        if (IR.M > 10 || IR.M < 0) {
            printf("M is out of range!\n");
            return;
        }
        printf("%-4s %3d %3d %5d %5d %5d", subOps[IR.M], IR.L, IR.M, PC, BP, SP);
    } else {
        // If OP is not within bounds of 1-9
        if (IR.OP < 1 || IR.OP > 9) {
            printf("OP is out of range!\n");
            return;
        }
        printf("%-4s %3d %3d %5d %5d %5d", opCode[IR.OP], IR.L, IR.M, PC, BP, SP);
    }
    // Print stack contents - show the full context
    if (SP <= base0) {
        // First, print the main activation record data
        for (int i = SP; i <= base0; i++) {
            printf(" %d", PAS[i]);
        }
        
        // If we're in a procedure call, show the AR separator and info
        if (BP != base0 && BP >= 0 && BP < 500) {
            printf(" | %d %d %d", PAS[BP], PAS[BP-1], PAS[BP-2]);
            // Add any local variables from the current procedure
            if (SP < BP - 2) {
                for (int i = BP - 3; i >= SP; i--) {
                    printf(" %d", PAS[i]);
                }
            }
        }
    }
    printf("\n");
}

int main (int argc, char *argv[]) {
    // Checks to make sure that only one argument is accepted
    if (argc != 2) {
        printf("Only one argument is allowed!\n");
        return 1;
    }
    
    // Reads from the file and checks that file is properly read
    FILE *ptrFile;
    ptrFile = fopen(argv[1], "r");
    
    if (ptrFile == NULL) {
        printf("Error opening file, please try again.\n");
        return 1;
    }
    
    // index counter to keep track of what index we are in
    int counter = 499;
    
    // reads from file 3 ints at a time
    while (fscanf(ptrFile,"%d %d %d", &IR.OP, &IR.L, &IR.M) == 3) {
        
        if (counter - 2 < 0) {
            printf("PAS Overflow has occurred.\n");
            fclose(ptrFile);
            return 1;
        }
        
        
        PAS[counter] = IR.OP;
        PAS[counter - 1] = IR.L;
        PAS[counter - 2] = IR.M;
        counter -= 3;
    }
    
    // set registers and top-of-stack placement
    fclose(ptrFile);
    PC = 499;
    SP = counter + 1;
    BP = SP - 1;
    base0 = BP;
    
    //Initial register values printed before instruction executes
    printf("   L   M   PC   BP   SP   stack\n");
    printf("Initial values:               %5d %5d %5d\n", PC, BP, SP);
    
    // copy instruction triple from PAS into IR
    int checker = 1;
    while(checker) {
        if (PC - 2 < 0) {
            printf("PC is out of bounds!\n");
            break;
        }
        
        IR.OP = PAS[PC];
        IR.L = PAS[PC - 1];
        IR.M = PAS[PC - 2];
        PC = PC - 3;
        
        if (IR.OP == 1) {  //LIT
            SP --;
            PAS[SP] = IR.M;
        }
        else if (IR.OP == 9 && IR.M == 1) {// SYS 0 1
            printf("Output result is : %d\n", PAS[SP]);
            SP++;
        }
        else if (IR.OP == 9 && IR.M == 2) {// SYS 0 2
            int number;
            printf("Please Enter an Integer : ");
            scanf("%d", &number);
            SP--;
            PAS[SP] = number;
        }
        else if (IR.OP == 9 && IR.M == 3) {
            checker = 0;
        }
        else if (IR.OP == 2) {     // OPR
            if (IR.M == 0) {
                int oldBP = BP;
                PC = PAS[oldBP - 2];
                BP = PAS[oldBP - 1];
                SP = oldBP + 1;
            }
            else if (IR.M == 1) {  // ADD
                PAS[SP + 1] += PAS[SP];
                SP++;
            }
            else if (IR.M == 2) { // SUB
                PAS[SP + 1] -= PAS[SP];
                SP++;
            }
            else if (IR.M == 3) { // MULT
                PAS[SP + 1] *= PAS[SP];
                SP++;
            }
            else if (IR.M == 4) { // DIV
                PAS[SP + 1] /= PAS[SP];
                SP++;
            }
            else if (IR.M == 5) { // EQUALS
                PAS[SP + 1] = (PAS[SP + 1] == PAS[SP]);
                SP++;
            }
            else if (IR.M == 6) {// NEQ
                PAS[SP + 1] = (PAS[SP + 1] != PAS[SP]);
                SP++;
            }
            else if (IR.M == 7) { // LSS
                PAS[SP + 1] = (PAS[SP + 1] < PAS[SP]);
                SP++;
            }
            else if (IR.M == 8) { // L.EQ
                PAS[SP + 1] = (PAS[SP + 1] <= PAS[SP]);
                SP++;
            }
            else if (IR.M == 9) { // GTR
                PAS[SP + 1] = (PAS[SP + 1] > PAS[SP]);
                SP++;
            }
            else if (IR.M == 10) { // G.EQ
                PAS[SP + 1] = (PAS[SP + 1] >= PAS[SP]);
                SP++;
            }
        }
        else if (IR.OP == 3) { // LOD L M
            SP --;
            PAS[SP] = PAS[base(BP, IR.L) - IR.M];
        }
        else if (IR.OP == 4) { // STO L M
            PAS[base(BP, IR.L) - IR.M] = PAS[SP];
            SP++;
        }
        else if (IR.OP == 5) {// CAL L M
            SP = SP - 3;
            PAS[SP + 2] = base(BP, IR.L); // static link
            PAS[SP + 1] = BP;             // dynamic link
            PAS[SP] = PC;
            BP = SP + 2;
            PC = 499 - IR.M;
        }
        else if (IR.OP == 6) { // INC 0 M
            SP = SP - IR.M;
            for (int i = SP; i < SP + IR.M; i++) { //Formatting 0's
                PAS[i] = 0;
            }
        }
        else if (IR.OP == 7) { // JUMP 0 M
            PC = 499 - IR.M;
        }
        else if (IR.OP == 8) { // JPC 0 M
            if (PAS[SP] == 0) {
                PC = 499 - IR.M;
            }
            SP++;
        }
        else {
            printf("OpCode unknown %d\n", IR.OP);
            checker = 0;
        }
        printMachine();
    }
}


