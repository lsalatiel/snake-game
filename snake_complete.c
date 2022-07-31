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
    // resumo e stats;
    int cont_moves;
    int nopoint_moves;
    int moves_down;
    int moves_up;
    int moves_left;
    int moves_right;
    // heatmap;
    int heatmap[101][101];
} tEstatisticas;

typedef struct{
    tCobra cobra;
    tMapa mapa;
    tEstatisticas stats;
    char lance;
    int pontuacao;
} tJogo;

// funcoes para tratar o mapa;
tJogo LeMapa(tJogo jogo, FILE * pfile);
void ImprimeMapa(tJogo jogo, FILE * pfile_out);

// funcoes de inicializacao;
tCobra IncializaParamCobra(tJogo jogo);
tEstatisticas InicializaParamStats(tJogo jogo);
tEstatisticas InicializaHeatMap(tJogo jogo);
tJogo InicializaParamJogo(tJogo jogo);

// funcoes base;
tJogo JogaJogo(tJogo jogo, FILE * pfile_resumo);
int AcabouJogo(tJogo jogo, FILE * pfile_resumo);
void ImprimeEstadoJogo(tJogo jogo);

// funcoes para tratar estatisticas;
tEstatisticas PreencheHeatMap(tJogo jogo);
void ImprimeHeatMap(tJogo jogo, FILE * pfile_heatmap);
void ImprimeEstatisticas(tJogo jogo, FILE * pfile_stats);
void ImprimeRanking(tJogo jogo, FILE * pfile_rank);

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
tJogo MoveDireita(tJogo jogo);
tJogo MoveEsquerda(tJogo jogo);
tJogo MoveCima(tJogo jogo);
tJogo MoveBaixo(tJogo jogo);
tCobra AtualizaPosCorpo(tJogo jogo, int l, int c);
tJogo MovimentaCorpo(tJogo jogo, int l, int c);
tJogo MataCorpo(tJogo jogo);

// funcoes principais de movimento;
tJogo MovimentaCobra(tJogo jogo);
tJogo CorrigeMovimento(tJogo jogo, FILE * pfile_resumo);


int main(int argc, char * argv[]){
    FILE * pfile_map; FILE * pfile_out; FILE * pfile_resumo; FILE * pfile_stats; FILE * pfile_heatmap; FILE * pfile_rank;
    char filename[1001], ini[1050], resumo[1050], stats[1050], heatmap[1050], rank[1050];
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
    sprintf(ini, "%s/saida/inicializacao.txt", argv[1]);
    pfile_out = fopen(ini,"w");
    if(!pfile_out){
        printf("Error creating %s\n", ini);
        fclose(pfile_map);
        return 1;
    }
    // cria o arquivo resumo.txt;
    sprintf(resumo, "%s/saida/resumo.txt", argv[1]);
    pfile_resumo = fopen(resumo, "w");
    if(!pfile_resumo){
        printf("Error creating file %s\n", resumo);
        fclose(pfile_map);
        fclose(pfile_out);
        return 1;
    }
    // cria o arquivo estatistica.txt;
    sprintf(stats, "%s/saida/estatisticas.txt", argv[1]);
    pfile_stats = fopen(stats, "w");
    if(!pfile_stats){
        printf("Error creating file %s\n", stats);
        fclose(pfile_map);
        fclose(pfile_out);
        fclose(pfile_resumo);
        return 1;
    }
    // cria o arquivo heatmap.txt;
    sprintf(heatmap, "%s/saida/heatmap.txt", argv[1]);
    pfile_heatmap = fopen(heatmap, "w");
    if(!pfile_stats){
        printf("Error creating file %s\n", heatmap);
        fclose(pfile_map);
        fclose(pfile_out);
        fclose(pfile_resumo);
        fclose(pfile_stats);
        return 1;
    }
    // cria o arquivo ranking.txt;
    sprintf(rank, "%s/saida/ranking.txt", argv[1]);
    pfile_rank = fopen(rank, "w");
    if(!pfile_rank){
        printf("Error creating file %s\n", rank);
        fclose(pfile_map);
        fclose(pfile_out);
        fclose(pfile_resumo);
        fclose(pfile_stats);
        fclose(pfile_heatmap);
        return 1;
    }

    // cria e inicializa parametros do jogo;
    tJogo jogo;
    jogo = InicializaParamJogo(jogo);

    // le e imprime mapa;
    fscanf(pfile_map, "%d %d", &jogo.mapa.linha, &jogo.mapa.coluna);

    jogo = LeMapa(jogo, pfile_map);
    ImprimeMapa(jogo, pfile_out);

    // joga jogo enquanto nao o jogo nao acabou;
    while(!AcabouJogo(jogo, pfile_resumo)){
        jogo = JogaJogo(jogo, pfile_resumo);
    }

    ImprimeRanking(jogo, pfile_rank);
    ImprimeEstatisticas(jogo, pfile_stats);
    ImprimeHeatMap(jogo, pfile_heatmap);

    fclose(pfile_map);
    fclose(pfile_out);
    fclose(pfile_resumo);
    fclose(pfile_stats);
    fclose(pfile_heatmap);
    fclose(pfile_rank);

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
                jogo.stats = PreencheHeatMap(jogo);
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
    fprintf(pfile_out, "A cobra comecara o jogo na linha %d e coluna %d\n", linha_ini, coluna_ini);
}

tJogo InicializaParamJogo(tJogo jogo){
    jogo.pontuacao = 0;
    jogo.lance = '\0';
    jogo.stats = InicializaParamStats(jogo);
    jogo.cobra = IncializaParamCobra(jogo);

    return jogo;
}

tCobra IncializaParamCobra(tJogo jogo){
    jogo.cobra.pos_cabeca = RIGHT;
    jogo.cobra.tam_corpo = 0;
    jogo.cobra.morreu = 0;

    return jogo.cobra;
}

tEstatisticas InicializaParamStats(tJogo jogo){
    jogo.stats.cont_moves = 0;
    jogo.stats.nopoint_moves = 0;
    jogo.stats.moves_down = 0;
    jogo.stats.moves_up = 0;
    jogo.stats.moves_left = 0;
    jogo.stats.moves_right = 0;
    jogo.stats = InicializaHeatMap(jogo);

    return jogo.stats;
}

tEstatisticas InicializaHeatMap(tJogo jogo){
    int i, j;

    for(i = 1; i <= jogo.mapa.linha; i++){
        for(j = 1; j <= jogo.mapa.coluna; j++){
            jogo.stats.heatmap[i][j] = 0;
        }
    }

    return jogo.stats;
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
        jogo.stats.moves_right++;
    }
    else if(cabeca == LEFT){ // CASO 2;
        jogo = MoveEsquerda(jogo);
        jogo.stats.moves_left++;
    }
    else if(cabeca == UP){ // CASO 3;
        jogo = MoveCima(jogo);
        jogo.stats.moves_up++;
    }
    else if(cabeca == DOWN){ // CASO 4;
        jogo = MoveBaixo(jogo);
        jogo.stats.moves_down++;
    }

    jogo = MovimentaCorpo(jogo, save_linha, save_coluna);

    return jogo;
}

tJogo CorrigeMovimento(tJogo jogo, FILE * pfile_resumo){ // corrige o movimento da cabeca e atualiza variaveis;
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;
    // atualiza as variaveis de acordo com a proxima posicao da cobra;
    if(EhParede(jogo, l, c) || EhCorpo(jogo, l, c)){
        jogo.cobra.morreu = 1;
        jogo.cobra.pos_cabeca = MORTA;
        jogo = MataCorpo(jogo);
        fprintf(pfile_resumo, "Movimento %d (%c) resultou no fim de jogo por conta de colisao\n", jogo.stats.cont_moves, jogo.lance);
        jogo.stats.nopoint_moves++;
    }
    else if(EhComida(jogo, l, c)){
        jogo.pontuacao++;
        jogo.cobra.tam_corpo++;
        if(jogo.cobra.tam_corpo != jogo.mapa.qtd_comida){
        fprintf(pfile_resumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d\n", jogo.stats.cont_moves, jogo.lance, jogo.cobra.tam_corpo + 1);
        }
    }
    else if(EhDinheiro(jogo, l, c)){
        jogo.pontuacao += 10;
        fprintf(pfile_resumo, "Movimento %d (%c) gerou dinheiro\n", jogo.stats.cont_moves, jogo.lance);
    }
    else{
        jogo.stats.nopoint_moves++;
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

int AcabouJogo(tJogo jogo, FILE * pfile_resumo){
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
        if(jogo.cobra.tam_corpo == jogo.mapa.qtd_comida){
            fprintf(pfile_resumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d, terminando o jogo\n", jogo.stats.cont_moves, jogo.lance, jogo.cobra.tam_corpo + 1);
        }
        return 1;
    }
    printf("\n");
    return 0;
}

tJogo JogaJogo(tJogo jogo, FILE * pfile_resumo){
    jogo.lance = ObtemLance(jogo);
    jogo.stats.cont_moves++;
    
    jogo.cobra = MudaPosCabeca(jogo);
    jogo = MovimentaCobra(jogo);
    jogo = CorrigeMovimento(jogo, pfile_resumo);
    ImprimeEstadoJogo(jogo);
    jogo.stats = PreencheHeatMap(jogo);
    
    return jogo;
}

void ImprimeEstatisticas(tJogo jogo, FILE * pfile_stats){
    fprintf(pfile_stats, "Numero de movimentos: %d\n", jogo.stats.cont_moves);
    fprintf(pfile_stats, "Numero de movimentos sem pontuar: %d\n", jogo.stats.nopoint_moves);
    fprintf(pfile_stats, "Numero de movimentos para baixo: %d\n", jogo.stats.moves_down);
    fprintf(pfile_stats, "Numero de movimentos para cima: %d\n", jogo.stats.moves_up);
    fprintf(pfile_stats, "Numero de movimentos para esquerda: %d\n", jogo.stats.moves_left);
    fprintf(pfile_stats, "Numero de movimentos para direita: %d\n", jogo.stats.moves_right);
}

tEstatisticas PreencheHeatMap(tJogo jogo){
    int l = jogo.cobra.linha, c = jogo.cobra.coluna;
    
    jogo.stats.heatmap[l][c]++;

    return jogo.stats;
}

void ImprimeRanking(tJogo jogo, FILE * pfile_rank){
    int i, j, aux, maior;
    // obtem o maior numero de vezes que a cobra passou em uma especifica posicao;
    for(i = 1; i <= jogo.mapa.linha; i++){
        for(j = 1; j <= jogo.mapa.coluna; j++){
            if(i == 1 && j == 1){
                maior = jogo.stats.heatmap[i][j];
            }
            else if(jogo.stats.heatmap[i][j] > maior){
                maior = jogo.stats.heatmap[i][j];
            }
        }
    }
    // printa do maior para o menor;
    for(aux = maior; aux > 0; aux--){
        for(i = 1; i <= jogo.mapa.linha; i++){
            for(j = 1; j <= jogo.mapa.coluna; j++){
                if(jogo.stats.heatmap[i][j] == aux){
                    fprintf(pfile_rank, "(%d, %d) - %d\n", i - 1, j - 1, jogo.stats.heatmap[i][j]);
                }
            }
        }
    }
}

void ImprimeHeatMap(tJogo jogo, FILE * pfile_heatmap){
    int i, j;

    for(i = 1; i <= jogo.mapa.linha; i++){
        for(j = 1; j <= jogo.mapa.coluna; j++){
            if(j == jogo.mapa.coluna){
                fprintf(pfile_heatmap, "%d", jogo.stats.heatmap[i][j]);
            }
            else{
                fprintf(pfile_heatmap, "%d ", jogo.stats.heatmap[i][j]);
            }
        }
        fprintf(pfile_heatmap, "\n");
    }
}
