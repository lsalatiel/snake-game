#include <stdio.h>

// definindo partes do mapa;
#define VAZIO ' '
#define PAREDE '#'
#define COMIDA '*'
#define DINHEIRO '$'

// definindo estados da cobra;
#define RIGHT '>'
#define LEFT '<'
#define UP '^'
#define DOWN 'v'
#define CORPO 'o'
#define MORTA 'X'

// definindo lances;
#define CONTINUE 'c'
#define HORARIO 'h'
#define ANTIHORARIO 'a'

typedef struct{
    char matriz[101][101];
    int linha;
    int coluna;
    int qtd_comida;
} tMapa;

typedef struct{
    char pos_cabeca; // posicao da rotacao da cabeca (RIGHT, LEFT, UP, DOWN);
    int linha; // linha da posicao da cabeca;
    int coluna; // coluna da posicao da cabeca;
    int tam_corpo; // tamanho do corpo;
    int linha_corpo[100]; // linha da poiscao do corpo;
    int coluna_corpo[100]; // coluna da posicao do corpo;
    int morreu; // 1 se morreu, 0 se esta viva;
} tCobra;

typedef struct{
    char lance;
    tCobra cobra;
    int pontuacao;
    tMapa mapa;
} tJogo;

// funcoes para tratar o mapa;
tJogo LeMapa(tJogo jogo, FILE * pfile);
void ImprimeMapa(tJogo jogo, FILE * pfile_out);

// funcoes de inicializacao;
tCobra IncializaParamCobra(tJogo jogo);
tJogo InicializaParamJogo(tJogo jogo);

// funcoes base;
tJogo JogaJogo(tJogo jogo);
int AcabouJogo(tJogo jogo);
void ImprimeEstadoJogo(tJogo jogo);

// funcoes que retornam variavel;
char ObtemPosCabeca(tJogo jogo);
char ObtemLance(tJogo jogo);

// funcoes que verificam algo e retornam 1 ou 0 (V ou F);
int VerificaMorteCobra(tJogo jogo);
int UltrapassaLimites(tJogo jogo);
int EhParede(tJogo jogo, int l, int c);
int EhComida(tJogo jogo, int l, int c);
int EhCorpo(tJogo jogo, int l, int c);
int EhVazio(tJogo jogo, int l, int c);

// funcoes secundarias de movimento;
tCobra MudaPosCabeca(tJogo jogo);
tCobra AtualizaPosCorpo(tJogo jogo, int l, int c);
tJogo MovimentaCorpo(tJogo jogo, int l, int c);
tJogo MataCorpo(tJogo jogo);

// funcoes principais de movimento;
tJogo MovimentaCobra(tJogo jogo);
tJogo CorrigeMovimento(tJogo jogo);


int main(int argc, char * argv[]){
    FILE * pfile_map; FILE * pfile_out;
    char filename[1001], saida[1050];
    // verifica se o diretorio foi informado;
    if(argc <= 1){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }
    // abre o mapa.txt pelo caminho informado;
    sprintf(filename, "%s/mapa.txt", argv[1]);
    pfile_map = fopen(filename, "r");
    if(!pfile_map){
        printf("Nao foi possivel ler o arquivo %s\n", filename);
        return 1;
    }
    // cria o arquivo inicializacao.txt;
    sprintf(saida, "%s/saida/inicializacao.txt", argv[1]);
    pfile_out = fopen(saida,"w");
    if(!pfile_out){
        printf("Error creating file\n");
        fclose(pfile_map);
        return 1;
    }
    // le e imprime mapa;
    tJogo jogo;
    fscanf(pfile_map, "%d %d", &jogo.mapa.linha, &jogo.mapa.coluna);

    jogo = LeMapa(jogo, pfile_map);
    ImprimeMapa(jogo, pfile_out);

    // cria e inicializa parametros do jogo;
    jogo = InicializaParamJogo(jogo);
    while(!AcabouJogo(jogo)){ // joga jogo enquanto nao o jogo nao acabou;
        jogo = JogaJogo(jogo);
    }

    fclose(pfile_map);
    fclose(pfile_out);

    return 0;
}
// lendo o mapa;
tJogo LeMapa(tJogo jogo, FILE * pfile){
    int i, j, linha = jogo.mapa.linha, coluna = jogo.mapa.coluna;
    int cont_comida = 0;

    fscanf(pfile, "%*c"); // consome o \n;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            fscanf(pfile, "%c", &jogo.mapa.matriz[i][j]);
            if(jogo.mapa.matriz[i][j] == COMIDA){
                cont_comida++;
            }
            if(jogo.mapa.matriz[i][j] == RIGHT){
                jogo.cobra.linha = i;
                jogo.cobra.coluna = j;
            }
        }
        fscanf(pfile, "%*c"); // consome o \n;
    }
    jogo.mapa.qtd_comida = cont_comida;

    return jogo;
}

void ImprimeMapa(tJogo jogo, FILE * pfile_out){
    int i, j, linha = jogo.mapa.linha, coluna = jogo.mapa.coluna;
    int linha_ini, coluna_ini;

    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            fprintf(pfile_out, "%c", jogo.mapa.matriz[i][j]);
            if(jogo.mapa.matriz[i][j] == RIGHT){
                linha_ini = i;
                coluna_ini = j;
            }
        }
        fprintf(pfile_out, "\n");
    }
    fprintf(pfile_out, "A cobra comecara o jogo na linha %d e coluna %d", linha_ini, coluna_ini);
}

tJogo InicializaParamJogo(tJogo jogo){
    jogo.pontuacao = 0;
    jogo.lance = '\0';
    jogo.cobra = IncializaParamCobra(jogo);

    return jogo;
}

tCobra IncializaParamCobra(tJogo jogo){
    jogo.cobra.pos_cabeca = RIGHT;
    jogo.cobra.tam_corpo = 0;
    jogo.cobra.morreu = 0;

    return jogo.cobra;
}

char ObtemPosCabeca(tJogo jogo){
    return jogo.cobra.pos_cabeca;
}

char ObtemLance(tJogo jogo){
    scanf("%c", &jogo.lance);
    scanf("%*c");

    return jogo.lance;
}

int VerificaMorteCobra(tJogo jogo){
    return jogo.cobra.morreu;
}

int UltrapassaLimites(tJogo jogo){
    return jogo.cobra.linha > jogo.mapa.linha || jogo.cobra.linha < 1 || jogo.cobra.coluna > jogo.mapa.coluna || jogo.cobra.coluna < 1;
}

int EhParede(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == PAREDE;
}

int EhComida(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == COMIDA;
}

int EhCorpo(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == CORPO;
}

int EhDinheiro(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == DINHEIRO;
}

int EhVazio(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == VAZIO;
}

tCobra AtualizaPosCorpo(tJogo jogo, int l, int c){
    jogo.cobra.linha_corpo[jogo.cobra.tam_corpo] = l;
    jogo.cobra.coluna_corpo[jogo.cobra.tam_corpo] = c;

    return jogo.cobra;
}

tCobra MudaPosCabeca(tJogo jogo){ // rotaciona a cabeca da cobra de acordo com o lance;
    char cabeca = ObtemPosCabeca(jogo);

    if(jogo.lance == HORARIO){
        switch (cabeca){
            case RIGHT:
                cabeca = DOWN;
                break;

            case DOWN:
                cabeca = LEFT;
                break;

            case LEFT:
                cabeca = UP;
                break;

            case UP:
                cabeca = RIGHT;
                break;
        }
    }
    else if(jogo.lance == ANTIHORARIO){
        switch (cabeca){
            case RIGHT:
                cabeca = UP;
                break;

            case DOWN:
                cabeca = RIGHT;
                break;

            case LEFT:
                cabeca = DOWN;
                break;

            case UP:
                cabeca = LEFT;
                break;
        }
    }

    jogo.cobra.pos_cabeca = cabeca;

    return jogo.cobra;
}

tJogo MovimentaCorpo(tJogo jogo, int l, int c){
    int i, aux1, aux2;
    if(jogo.mapa.matriz[l][c] != CORPO){ /* a posicao deve ser diferente de CORPO pois se na posicao a frente estiver uma comida,
                                            a funcao AtualizaPosCorpo coloca um corpo no lugar e nao eh necessario mover o corpo
                                            quando a cobra passa pela COMIDA;*/
        for(i = 0; i < jogo.cobra.tam_corpo; i++){
            if(!i){
                jogo.mapa.matriz[jogo.cobra.linha_corpo[i]][jogo.cobra.coluna_corpo[i]] = VAZIO; // limpa a ultima posicao do corpo no mapa;
            }
            if(i == jogo.cobra.tam_corpo - 1){ // a primeira posicao do corpo ocupara a antiga posicao da cabeca;
                jogo.cobra.linha_corpo[i] = l;
                jogo.cobra.coluna_corpo[i] = c;
                jogo.mapa.matriz[l][c] = CORPO;
                break;
            }
            // o corpo i se desloca para a posicao do corpo i + 1;
            jogo.cobra.linha_corpo[i] = jogo.cobra.linha_corpo[i + 1];
            jogo.cobra.coluna_corpo[i] = jogo.cobra.coluna_corpo[i + 1];
            aux1 = jogo.cobra.linha_corpo[i + 1];
            aux2 = jogo.cobra.coluna_corpo[i + 1];
            // atualiza no mapa;
            jogo.mapa.matriz[aux1][aux2] = CORPO;
        }
    }

    return jogo;
}

tJogo MataCorpo(tJogo jogo){
    int i, aux1, aux2;

    for(i = 0; i < jogo.cobra.tam_corpo; i++){ // varre todas as posicoes do corpo, colocando um 'X' em cada uma;
        aux1 = jogo.cobra.linha_corpo[i];
        aux2 = jogo.cobra.coluna_corpo[i];
        jogo.mapa.matriz[aux1][aux2] = MORTA;
    }

    return jogo;
}

tJogo MoveDireita(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;

    jogo.cobra.coluna += 1;

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra.coluna = 1;

        c = jogo.cobra.coluna;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l][jogo.mapa.coluna] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l, jogo.mapa.coluna);
        }
        else{
            jogo.mapa.matriz[l][jogo.mapa.coluna] = VAZIO;
        }
    }
    else{
        c = jogo.cobra.coluna;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l][c - 1] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l, c - 1);
        }
        else{
            jogo.mapa.matriz[l][c - 1] = VAZIO;
        }
    }

    return jogo;
}

tJogo MoveEsquerda(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;
    
    jogo.cobra.coluna -= 1;

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra.coluna = jogo.mapa.coluna;

        c = jogo.cobra.coluna;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l][1] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l, 1);
        }
        else{
            jogo.mapa.matriz[l][1] = VAZIO;
        }
    }
    else{
        c = jogo.cobra.coluna;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l][c + 1] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l, c + 1);
        }
        else{
            jogo.mapa.matriz[l][c + 1] = VAZIO;
        }
    }

    return jogo;
}

tJogo MoveCima(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;

    jogo.cobra.linha -= 1;
    
    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra.linha = jogo.mapa.linha;

        l = jogo.cobra.linha;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[1][c] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, 1, c);
        }
        else{
            jogo.mapa.matriz[1][c] = VAZIO;
        }
    }
    else{
        l = jogo.cobra.linha;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l + 1][c] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l + 1, c);
        }
        else{
            jogo.mapa.matriz[l + 1][c] = VAZIO;
        }
    }

    return jogo;
}

tJogo MoveBaixo(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;
    
    jogo.cobra.linha += 1;

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra.linha = 1;
            
        l = jogo.cobra.linha;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[jogo.mapa.linha][c] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, jogo.mapa.linha, c);
        }
        else{
            jogo.mapa.matriz[jogo.mapa.linha][c] = VAZIO;
        }
    }
    else{
        l = jogo.cobra.linha;

        if(EhComida(jogo, l, c)){
            jogo.mapa.matriz[l - 1][c] = CORPO;
            jogo.cobra = AtualizaPosCorpo(jogo, l - 1, c);
        }
        else{
            jogo.mapa.matriz[l - 1][c] = VAZIO;
        }
    }

    return jogo;
}

tJogo MovimentaCobra(tJogo jogo){
    int save_linha = jogo.cobra.linha, save_coluna = jogo.cobra.coluna; // guarda a posicao da cabeca da cobra sem as mudancas a serem feitas;

    char cabeca = ObtemPosCabeca(jogo);

    if(cabeca == RIGHT){ // CASO 1;
        jogo = MoveDireita(jogo);
    }
    else if(cabeca == LEFT){ // CASO 2;
        jogo = MoveEsquerda(jogo);
    }
    else if(cabeca == UP){ // CASO 3;
        jogo = MoveCima(jogo);
    }
    else if(cabeca == DOWN){ // CASO 4;
        jogo = MoveBaixo(jogo);
    }

    jogo = MovimentaCorpo(jogo, save_linha, save_coluna);

    return jogo;
}

tJogo CorrigeMovimento(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;
    // atualiza as variaveis de acordo com a proxima posicao da cobra;
    if(EhParede(jogo, l, c) || EhCorpo(jogo, l, c)){
        jogo.cobra.morreu = 1;
        jogo.cobra.pos_cabeca = MORTA;
        jogo = MataCorpo(jogo);
    }
    else if(EhComida(jogo, l, c)){
        jogo.pontuacao++;
        jogo.cobra.tam_corpo++;
    }
    else if(EhDinheiro(jogo, l, c)){
        jogo.pontuacao += 10;
    }
    // atualiza a posicao da cabeca da cobra;
    jogo.mapa.matriz[l][c] = jogo.cobra.pos_cabeca;

    return jogo;
}

void ImprimeEstadoJogo(tJogo jogo){
    int i, j;
    int l = jogo.mapa.linha, c = jogo.mapa.coluna;

    printf("Estado do jogo apos o movimento '%c':\n", jogo.lance);
    for(i = 1; i <= l; i++){
        for(j = 1; j <= c; j++){
            printf("%c", jogo.mapa.matriz[i][j]);
        }
        printf("\n");
    }
    printf("Pontuacao: %d\n", jogo.pontuacao);
}

int AcabouJogo(tJogo jogo){
    if(VerificaMorteCobra(jogo)){
        printf("Game over!\n");
        printf("Pontuacao final: %d\n", jogo.pontuacao);
        return 1;
    }
    // verifica se o tamanho do corpo eh igual a quantidade de comida existente no mapa nao modificado;
    // verifica se existe comida no mapa;
    if(jogo.cobra.tam_corpo == jogo.mapa.qtd_comida || jogo.mapa.qtd_comida == 0){
        printf("Voce venceu!\n");
        printf("Pontuacao final: %d\n", jogo.pontuacao);
        return 1;
    }
    printf("\n");
    return 0;
}

tJogo JogaJogo(tJogo jogo){
    jogo.lance = ObtemLance(jogo);
    jogo.cobra = MudaPosCabeca(jogo);
    jogo = MovimentaCobra(jogo);
    jogo = CorrigeMovimento(jogo);
    ImprimeEstadoJogo(jogo);
    
    return jogo;
}
