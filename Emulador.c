Claro! Aqui está a tradução completa do seu código C com os comentários traduzidos para o **inglês**, mantendo a estrutura original:

---

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Types

typedef unsigned char byte;         // 8 bits
typedef unsigned int palavra;       // 32 bits
typedef unsigned long int microinstrucao; // 64 bits, although the architecture uses only 36 bits per microinstruction

// Registers

// João - use struct for better structure
// typedef struct {

palavra MAR = 0, MDR = 0, PC = 0;   // Memory Access
byte MBR = 0;                       // Memory Access

palavra SP = 0, LV = 0, TOS = 0,    // ALU Operation
        OPC = 0, CPP = 0, H = 0;    // ALU Operation

microinstrucao MIR;                // Holds the current microinstruction
palavra MPC = 0;                   // Holds the address for the next microinstruction

// Buses

palavra Barramento_B, Barramento_C;

// Flip-Flops

byte N, Z;

// Microinstruction decoding helpers

byte MIR_B, MIR_Operacao, MIR_Deslocador, MIR_MEM, MIR_pulo;
palavra MIR_C;


// Control Store

microinstrucao Armazenamento[512];

// Main Memory

byte Memoria[100000000];

// Function Prototypes

void carregar_microprogram_de_controle();
void carregar_programa(const char *programa);
void exibir_processos();
void decodificar_microinstrucao();
void atribuir_barramento_B();
void realizar_operacao_ALU();
void atribuir_barramento_C();
void operar_memoria();
void pular();

void binario(void* valor, int tipo);


// Main Loop

int main(int argc, const char *argv[]){
    carregar_microprogram_de_controle();
    carregar_programa(argv[1]);
    while(1){
        exibir_processos();
        MIR = Armazenamento[MPC];

        decodificar_microinstrucao();
        atribuir_barramento_B();
        realizar_operacao_ALU();
        atribuir_barramento_C();
        operar_memoria();
        pular();
    }

    return 0;
}

// Function Implementations

void carregar_microprogram_de_controle(){
    FILE* MicroPrograma;
    MicroPrograma = fopen("microprog.rom", "rb");

    if(MicroPrograma != NULL){
        fread(Armazenamento, sizeof(microinstrucao), 512, MicroPrograma);
        fclose(MicroPrograma);
    }
}

void carregar_programa(const char* prog){
    FILE* Programa;
    palavra tamanho;
    byte tamanho_temp[4];
    Programa = fopen(prog, "rb");

    if(Programa != NULL){
        fread(tamanho_temp, sizeof(byte), 4, Programa); // Reads the size in bytes of the program
        memcpy(&tamanho, tamanho_temp, 4);

        fread(Memoria, sizeof(byte), 20, Programa); // Reads 20 bytes of initialization

        fread(&Memoria[0x0401], sizeof(byte), tamanho - 20, Programa); // Reads the program into memory
    }
}

// Use enum constants instead of 0b11111... to make debugging easier and reduce errors

void decodificar_microinstrucao(){
    MIR_B = (MIR) & 0b1111;
    MIR_MEM = (MIR >> 4) & 0b111;
    MIR_C = (MIR >> 7) & 0b111111111;
    MIR_Operacao = (MIR >> 16) & 0b111111;
    MIR_Deslocador = (MIR >> 22) & 0b11;
    MIR_pulo = (MIR >> 24) & 0b111;
    MPC = (MIR >> 27) & 0b111111111;
}

void atribuir_barramento_B(){
    switch(MIR_B){
        case 0: Barramento_B = MDR;                break;
        case 1: Barramento_B = PC;                 break;
        // Case 2 loads the MBR with sign extension,
        // i.e., it copies the most significant bit of the MBR to the 24 most significant bits of the B bus.
        case 2: Barramento_B = MBR;
            if(MBR & (0b10000000))
                Barramento_B = Barramento_B | (0b111111111111111111111111 << 8);
            break;
        case 3: Barramento_B = MBR;                break;
        case 4: Barramento_B = SP;                 break;
        case 5: Barramento_B = LV;                 break;
        case 6: Barramento_B = CPP;                break;
        case 7: Barramento_B = TOS;                break;
        case 8: Barramento_B = OPC;                break;
        default: Barramento_B = -1;                break;
    }
}

void realizar_operacao_ALU(){
    switch(MIR_Operacao){
        // Each ALU operation is represented by its bit sequence, converted to int for clarity
        case 12: Barramento_C = H & Barramento_B;        break;
        case 17: Barramento_C = 1;                       break;
        case 18: Barramento_C = -1;                      break;
        case 20: Barramento_C = Barramento_B;            break;
        case 24: Barramento_C = H;                       break;
        case 26: Barramento_C = ~H;                      break;
        case 28: Barramento_C = H | Barramento_B;        break;
        case 44: Barramento_C = ~Barramento_B;           break;
        case 53: Barramento_C = Barramento_B + 1;        break;
        case 54: Barramento_C = Barramento_B - 1;        break;
        case 57: Barramento_C = H + 1;                   break;
        case 59: Barramento_C = -H;                      break;
        case 60: Barramento_C = H + Barramento_B;        break;
        case 61: Barramento_C = H + Barramento_B + 1;    break;
        case 63: Barramento_C = Barramento_B - H;        break;
        default: break;
    }

    if(Barramento_C){
        N = 0;
        Z = 1;
    } else{
        N = 1;
        Z = 0;
    }

    switch(MIR_Deslocador){
        case 1: Barramento_C = Barramento_C << 8; break;
        case 2: Barramento_C = Barramento_C >> 1; break;
    }
}

void atribuir_barramento_C(){
    if(MIR_C & 0b000000001)   MAR = Barramento_C;
    if(MIR_C & 0b000000010)   MDR = Barramento_C;
    if(MIR_C & 0b000000100)   PC  = Barramento_C;
    if(MIR_C & 0b000001000)   SP  = Barramento_C;
    if(MIR_C & 0b000010000)   LV  = Barramento_C;
    if(MIR_C & 0b000100000)   CPP = Barramento_C;
    if(MIR_C & 0b001000000)   TOS = Barramento_C;
    if(MIR_C & 0b010000000)   OPC = Barramento_C;
    if(MIR_C & 0b100000000)   H   = Barramento_C;
}

void operar_memoria(){
    // Multiplication by 4 is necessary because MAR and MDR deal with 4-byte word addressing
    if(MIR_MEM & 0b001) MBR = Memoria[PC];
    if(MIR_MEM & 0b010) memcpy(&MDR, &Memoria[MAR*4], 4);
    if(MIR_MEM & 0b100) memcpy(&Memoria[MAR*4], &MDR, 4);
}

void pular(){
    if(MIR_pulo & 0b001) MPC = MPC | (N << 8);
    if(MIR_pulo & 0b010) MPC = MPC | (Z << 8);
    if(MIR_pulo & 0b100) MPC = MPC | (MBR);
}

void exibir_processos(){
    if(LV && SP){
        printf("\t\t  OPERAND STACK\n");
        printf("========================================\n");
        printf("     ADDR");
        printf("\t   VALUE IN BINARY");
        printf(" \t\tVALUE\n");
        for(int i = SP ; i >= LV ; i--){
            palavra valor;
            memcpy(&valor, &Memoria[i*4],4);            
            
            if(i == SP) printf("SP ->");
            else if(i == LV) printf("LV ->");
            else printf("     ");

            printf("%X ",i);
            binario(&valor, 1); printf(" ");            
            printf("%d\n", valor);
        }

        printf("========================================\n");
    }

    if(PC >= 0x0401) {
        printf("\n\t\t\tProgram Area\n");
        printf("========================================\n");
        printf("\t\tBinary");
        printf("\t HEX");
        printf("  BYTE ADDRESS\n");
        for(int i = PC - 2; i <= PC + 3 ; i++){
            if(i == PC
```

