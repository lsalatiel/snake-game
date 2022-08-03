#include <stdio.h>

// definindo partes do mapa;
#define VAZIO ' '
#define PAREDE '#'
#define COMIDA '*'
#define DINHEIRO '$'
#define TUNEL '@'

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
    int linha_tunel[2];
    int coluna_tunel[2];
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
tMapa LeMapa(tJogo jogo, FILE * pfile);
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
int ObtemTamCorpo(tJogo jogo);
int ObtemLinhaCorpo(tJogo jogo, int i);
int ObtemColunaCorpo(tJogo jogo, int i);
int ObtemLinhaCabeca(tJogo jogo);
int ObtemColunaCabeca(tJogo jogo);
int ObtemLinhaMapa(tJogo jogo);
int ObtemColunaMapa(tJogo jogo);
int ObtemQtdComida(tJogo jogo);
int ObtemQtdMoves(tJogo jogo);

// funcoes que atualizam variaveis;
tCobra AtualizaPosInicial(tJogo jogo);
tCobra AtualizaPosCorpo(tJogo jogo, int l, int c);
tCobra AtualizaLinhaCorpo(tJogo jogo, int i, int atualiza);
tCobra AtualizaColunaCorpo(tJogo jogo, int i, int atualiza);
tCobra AtualizaLinhaCabeca(tJogo jogo, int atualiza);
tCobra AtualizaColunaCabeca(tJogo jogo, int atualiza);
tCobra AtualizaMorteCobra(tJogo jogo, int check);
tCobra AtualizaPosCabeca(tJogo jogo, char check);
tCobra AdicionaTamCorpo(tJogo jogo);
tMapa AtualizaMapa(tJogo jogo, int l, int c, char caract);
tEstatisticas AdicionaContMove(tJogo jogo);
tEstatisticas AdicionaNoPointMove(tJogo jogo);
tEstatisticas AdicionaMoveRight(tJogo jogo);
tEstatisticas AdicionaMoveLeft(tJogo jogo);
tEstatisticas AdicionaMoveUp(tJogo jogo);
tEstatisticas AdicionaMoveDown(tJogo jogo);

// funcoes que verificam algo e retornam 1 ou 0 (V ou F);
int VerificaMorteCobra(tJogo jogo);
int UltrapassaLimites(tJogo jogo);
int EhParede(tJogo jogo, int l, int c);
int EhComida(tJogo jogo, int l, int c);
int EhCabecaInicial(tJogo jogo, int l, int c);
int EhCorpo(tJogo jogo, int l, int c);
int EhVazio(tJogo jogo, int l, int c);
int EhTunel(tJogo jogo, int l, int c);

// funcoes secundarias de movimento;
tCobra MudaPosCabeca(tJogo jogo);
tJogo MoveDireita(tJogo jogo);
tJogo MoveEsquerda(tJogo jogo);
tJogo MoveCima(tJogo jogo);
tJogo MoveBaixo(tJogo jogo);
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

    tJogo jogo;

    // le e imprime mapa;
    jogo.mapa = LeMapa(jogo, pfile_map);
    ImprimeMapa(jogo, pfile_out);

    // inicializa parametros do jogo, alem do mapa;
    jogo = InicializaParamJogo(jogo);

    // joga jogo enquanto nao o jogo nao acabou;
    while(!AcabouJogo(jogo, pfile_resumo)){
        jogo = JogaJogo(jogo, pfile_resumo);
    }
    // imprime estatisticas em seus respectivos arquivos;
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
tMapa LeMapa(tJogo jogo, FILE * pfile){
    // le linha e coluna;
    fscanf(pfile, "%d %d", &jogo.mapa.linha, &jogo.mapa.coluna);

    int i, j, linha = jogo.mapa.linha, coluna = jogo.mapa.coluna;
    int cont_comida = 0;
    int cont_tunel = 0;

    fscanf(pfile, "%*c"); // consome o \n;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            fscanf(pfile, "%c", &jogo.mapa.matriz[i][j]); // le cada elemento do mapa;
            if(EhComida(jogo, i, j)){
                cont_comida++;
            }
            if(EhTunel(jogo, i, j)){ // armazena as posicoes dos 2 tuneis;
                if(!cont_tunel){
                    jogo.mapa.linha_tunel[0] = i;
                    jogo.mapa.coluna_tunel[0] = j;
                    cont_tunel++;
                }
                else{
                    jogo.mapa.linha_tunel[1] = i;
                    jogo.mapa.coluna_tunel[1] = j;
                }
            }
        }
        fscanf(pfile, "%*c"); // consome o \n;
    }
    jogo.mapa.qtd_comida = cont_comida;

    return jogo.mapa;
}

void ImprimeMapa(tJogo jogo, FILE * pfile_out){ // imprime mapa no arquivo inicializacao.txt;
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
    jogo.cobra = IncializaParamCobra(jogo);
    jogo.stats = InicializaParamStats(jogo);

    return jogo;
}

tCobra IncializaParamCobra(tJogo jogo){
    jogo.cobra.pos_cabeca = RIGHT;
    jogo.cobra.tam_corpo = 0;
    jogo.cobra.morreu = 0;
    jogo.cobra = AtualizaPosInicial(jogo);

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

    int linha = ObtemLinhaMapa(jogo), coluna = ObtemColunaMapa(jogo);
    // inicializa cada posicao do heatmap com 0;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            jogo.stats.heatmap[i][j] = 0;
        }
    }
    // preenche com a posicao inicial da cobra;
    jogo.stats = PreencheHeatMap(jogo);

    return jogo.stats;
}

tCobra AtualizaPosInicial(tJogo jogo){
    int i, j, linha = ObtemLinhaMapa(jogo), coluna = ObtemColunaMapa(jogo);
    // pega a posicao inicial da cobra;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            if(EhCabecaInicial(jogo, i, j)){
                jogo.cobra = AtualizaLinhaCabeca(jogo, i);
                jogo.cobra = AtualizaColunaCabeca(jogo, j);
            }
        }
    }

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

int ObtemTamCorpo(tJogo jogo){
    return jogo.cobra.tam_corpo;
}

int ObtemLinhaCorpo(tJogo jogo, int i){
    return jogo.cobra.linha_corpo[i];
}

int ObtemColunaCorpo(tJogo jogo, int i){
    return jogo.cobra.coluna_corpo[i];
}

int ObtemLinhaCabeca(tJogo jogo){
    return jogo.cobra.linha;
}

int ObtemColunaCabeca(tJogo jogo){
    return jogo.cobra.coluna;
}

int ObtemLinhaMapa(tJogo jogo){
    return jogo.mapa.linha;
}

int ObtemColunaMapa(tJogo jogo){
    return jogo.mapa.coluna;
}

int ObtemQtdComida(tJogo jogo){
    return jogo.mapa.qtd_comida;
}

int ObtemQtdMoves(tJogo jogo){
    return jogo.stats.cont_moves;
}

int ObtemLinhaTunel(tJogo jogo, int pos){
    return jogo.mapa.linha_tunel[pos];
}

int ObtemColunaTunel(tJogo jogo, int pos){
    return jogo.mapa.coluna_tunel[pos];
}

tCobra AtualizaPosCorpo(tJogo jogo, int l, int c){
    jogo.cobra.linha_corpo[jogo.cobra.tam_corpo] = l;
    jogo.cobra.coluna_corpo[jogo.cobra.tam_corpo] = c;

    return jogo.cobra;
}

tCobra AtualizaLinhaCorpo(tJogo jogo, int i, int atualiza){
    jogo.cobra.linha_corpo[i] = atualiza;

    return jogo.cobra;
}

tCobra AtualizaColunaCorpo(tJogo jogo, int i, int atualiza){
    jogo.cobra.coluna_corpo[i] = atualiza;

    return jogo.cobra;
}

tCobra AtualizaLinhaCabeca(tJogo jogo, int atualiza){
    jogo.cobra.linha = atualiza;

    return jogo.cobra;
}

tCobra AtualizaColunaCabeca(tJogo jogo, int atualiza){
    jogo.cobra.coluna = atualiza;

    return jogo.cobra;
}

tMapa AtualizaMapa(tJogo jogo, int l, int c, char caract){
    jogo.mapa.matriz[l][c] = caract;

    return jogo.mapa;
}

tCobra AtualizaMorteCobra(tJogo jogo, int check){
    jogo.cobra.morreu = check;

    return jogo.cobra;
}

tCobra AtualizaPosCabeca(tJogo jogo, char check){
    jogo.cobra.pos_cabeca = check;

    return jogo.cobra;
}

tCobra AdicionaTamCorpo(tJogo jogo){
    jogo.cobra.tam_corpo++;

    return jogo.cobra;
}

tEstatisticas AdicionaContMove(tJogo jogo){
    jogo.stats.cont_moves++;

    return jogo.stats;
}

tEstatisticas AdicionaNoPointMove(tJogo jogo){
    jogo.stats.nopoint_moves++;

    return jogo.stats;
}

tEstatisticas AdicionaMoveRight(tJogo jogo){
    jogo.stats.moves_right++;

    return jogo.stats;
}

tEstatisticas AdicionaMoveLeft(tJogo jogo){
    jogo.stats.moves_left++;

    return jogo.stats;
}

tEstatisticas AdicionaMoveUp(tJogo jogo){
    jogo.stats.moves_up++;

    return jogo.stats;
}

tEstatisticas AdicionaMoveDown(tJogo jogo){
    jogo.stats.moves_down++;

    return jogo.stats;
}

int IdentificaTunel(tJogo jogo, int l, int c){ // retorna V se for o tunel mais proximo da posicao 0 0 e F se for o outro;
    return l == jogo.mapa.linha_tunel[0] && c == jogo.mapa.coluna_tunel[0];
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

int EhCabecaInicial(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == RIGHT;
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

int EhTunel(tJogo jogo, int l, int c){
    return jogo.mapa.matriz[l][c] == TUNEL;
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
    int tam;
    int l2, c2;

    tam = ObtemTamCorpo(jogo);

    if(!EhCorpo(jogo, l, c)){ /* a posicao deve ser diferente de CORPO pois se na posicao a frente estiver uma comida,
                                a funcao AtualizaPosCorpo coloca um corpo no lugar e nao eh necessario mover o corpo
                                quando a cobra passa pela COMIDA;*/
        for(i = 0; i < tam; i++){
            if(!i){
                l2 = ObtemLinhaCorpo(jogo, i);
                c2 = ObtemColunaCorpo(jogo, i);
                jogo.mapa = AtualizaMapa(jogo, l2, c2, VAZIO); // limpa a ultima posicao do corpo no mapa;
            }
            if(i == tam - 1){ // a primeira posicao do corpo ocupara a antiga posicao da cabeca;
                jogo.cobra = AtualizaLinhaCorpo(jogo, i, l);
                jogo.cobra = AtualizaColunaCorpo(jogo, i, c);
                jogo.mapa = AtualizaMapa(jogo, l, c, CORPO);
                break;
            }
            // o corpo i se desloca para a posicao do corpo i + 1;
            l2 = ObtemLinhaCorpo(jogo, i + 1);
            c2 = ObtemColunaCorpo(jogo, i + 1);
            jogo.cobra = AtualizaLinhaCorpo(jogo, i, l2);
            jogo.cobra = AtualizaColunaCorpo(jogo, i, c2);

            // atualiza no mapa;
            jogo.mapa = AtualizaMapa(jogo, l2, c2, CORPO);
        }
    }

    return jogo;
}

tJogo MataCorpo(tJogo jogo){
    int i, aux1, aux2;
    int tam;

    tam = ObtemTamCorpo(jogo);

    for(i = 0; i < tam; i++){ // varre todas as posicoes do corpo, colocando um 'X' em cada uma;
        aux1 = ObtemLinhaCorpo(jogo, i);
        aux2 = ObtemColunaCorpo(jogo, i);
        jogo.mapa = AtualizaMapa(jogo, aux1, aux2, MORTA);
    }

    return jogo;
}

tJogo MoveDireita(tJogo jogo){
    int l, c;
    // salva as posicoes nao alteradas da cabeca;
    int save_linha = ObtemLinhaCabeca(jogo);
    int save_coluna = ObtemColunaCabeca(jogo);
    
    l = ObtemLinhaCabeca(jogo);
    c = ObtemColunaCabeca(jogo);
    c += 1; // provavel posicao futura da cabeca da cobra;

    jogo.cobra = AtualizaColunaCabeca(jogo, c);

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaColunaCabeca(jogo, 1);

        c = ObtemColunaCabeca(jogo);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
    }
    if(EhTunel(jogo, l, c)){
        c = ObtemColunaCabeca(jogo);

        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }
        // atualiza a futura posicao da cabeca da cobra;
        l = ObtemLinhaTunel(jogo, ident_tunel2);
        c = ObtemColunaTunel(jogo, ident_tunel2);
        c += 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo, l);
        jogo.cobra = AtualizaColunaCabeca(jogo, c);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
        if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaColunaCabeca(jogo, 1);

            c = ObtemColunaCabeca(jogo);

            if(EhComida(jogo, l, c)){
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
                jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
            }
        }
    }
    else{
        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
    }

    return jogo;
}

tJogo MoveEsquerda(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    int save_linha = ObtemLinhaCabeca(jogo);
    int save_coluna = ObtemColunaCabeca(jogo);

    linha_mapa = ObtemLinhaMapa(jogo);
    coluna_mapa = ObtemColunaMapa(jogo);

    l = ObtemLinhaCabeca(jogo);
    c = ObtemColunaCabeca(jogo);
    c -= 1;

    jogo.cobra = AtualizaColunaCabeca(jogo, c);

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaColunaCabeca(jogo, coluna_mapa);

        c = ObtemColunaCabeca(jogo);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo, ident_tunel2);
        c = ObtemColunaTunel(jogo, ident_tunel2);
        c -= 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo, l);
        jogo.cobra = AtualizaColunaCabeca(jogo, c);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else if(!UltrapassaLimites(jogo)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaColunaCabeca(jogo, coluna_mapa);

            c = ObtemColunaCabeca(jogo);

            if(EhComida(jogo, l, c)){
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MoveCima(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    linha_mapa = ObtemLinhaMapa(jogo);
    coluna_mapa = ObtemColunaMapa(jogo);

    int save_linha = ObtemLinhaCabeca(jogo);
    int save_coluna = ObtemColunaCabeca(jogo);

    l = ObtemLinhaCabeca(jogo);
    c = ObtemColunaCabeca(jogo);
    l -= 1;

    jogo.cobra = AtualizaLinhaCabeca(jogo, l);
    
    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaLinhaCabeca(jogo, linha_mapa);

        l = ObtemLinhaCabeca(jogo);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo, ident_tunel2);
        c = ObtemColunaTunel(jogo, ident_tunel2);
        l -= 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo, l);
        jogo.cobra = AtualizaColunaCabeca(jogo, c);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaLinhaCabeca(jogo, linha_mapa);

            l = ObtemLinhaCabeca(jogo);

            if(EhComida(jogo, l, c)){
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MoveBaixo(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    int save_linha = ObtemLinhaCabeca(jogo);
    int save_coluna = ObtemColunaCabeca(jogo);

    linha_mapa = ObtemLinhaMapa(jogo);
    coluna_mapa = ObtemColunaMapa(jogo);

    l = ObtemLinhaCabeca(jogo);
    c = ObtemColunaCabeca(jogo);
    l += 1;

    jogo.cobra = AtualizaLinhaCabeca(jogo, l);

    if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaLinhaCabeca(jogo, 1);
            
        l = ObtemLinhaCabeca(jogo);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo, ident_tunel2);
        c = ObtemColunaTunel(jogo, ident_tunel2);
        l += 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo, l);
        jogo.cobra = AtualizaColunaCabeca(jogo, c);

        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaLinhaCabeca(jogo, 1);

            l = ObtemLinhaCabeca(jogo);

            if(EhComida(jogo, l, c)){
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo, l, c)){
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MovimentaCobra(tJogo jogo){
    int save_linha = ObtemLinhaCabeca(jogo), save_coluna = ObtemColunaCabeca(jogo); // guarda a posicao da cabeca da cobra sem as mudancas a serem feitas;

    char cabeca = ObtemPosCabeca(jogo);

    if(cabeca == RIGHT){ // CASO 1;
        jogo = MoveDireita(jogo);
        jogo.stats = AdicionaMoveRight(jogo);
    }
    else if(cabeca == LEFT){ // CASO 2;
        jogo = MoveEsquerda(jogo);
        jogo.stats = AdicionaMoveLeft(jogo);
    }
    else if(cabeca == UP){ // CASO 3;
        jogo = MoveCima(jogo);
        jogo.stats = AdicionaMoveUp(jogo);
    }
    else if(cabeca == DOWN){ // CASO 4;
        jogo = MoveBaixo(jogo);
        jogo.stats = AdicionaMoveDown(jogo);
    }

    jogo = MovimentaCorpo(jogo, save_linha, save_coluna);

    return jogo;
}

tJogo CorrigeMovimento(tJogo jogo, FILE * pfile_resumo){ // corrige o movimento da cabeca e atualiza variaveis;
    int l, c, tam, qtd_comida, qtd_moves;

    l = ObtemLinhaCabeca(jogo);
    c = ObtemColunaCabeca(jogo);
    qtd_moves = ObtemQtdMoves(jogo);

    // atualiza as variaveis de acordo com a proxima posicao da cobra;
    if(EhParede(jogo, l, c) || EhCorpo(jogo, l, c)){
        jogo.cobra = AtualizaMorteCobra(jogo, 1);
        jogo.cobra = AtualizaPosCabeca(jogo, MORTA);
        jogo = MataCorpo(jogo);

        fprintf(pfile_resumo, "Movimento %d (%c) resultou no fim de jogo por conta de colisao\n", qtd_moves, jogo.lance);
        jogo.stats = AdicionaNoPointMove(jogo);
    }
    else if(EhComida(jogo, l, c)){
        jogo.pontuacao++;
        jogo.cobra = AdicionaTamCorpo(jogo);

        tam = ObtemTamCorpo(jogo);
        qtd_comida = ObtemQtdComida(jogo);

        if(tam != qtd_comida){
        fprintf(pfile_resumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d\n", qtd_moves, jogo.lance, tam + 1);
        }
    }
    else if(EhDinheiro(jogo, l, c)){
        jogo.pontuacao += 10;
        fprintf(pfile_resumo, "Movimento %d (%c) gerou dinheiro\n", qtd_moves, jogo.lance);
    }
    else{
        jogo.stats = AdicionaNoPointMove(jogo);
    }
    // atualiza a posicao da cabeca da cobra no mapa;
    char poscabeca = ObtemPosCabeca(jogo);
    jogo.mapa = AtualizaMapa(jogo, l, c, poscabeca);

    return jogo;
}

void ImprimeEstadoJogo(tJogo jogo){
    int i, j;
    int l = ObtemLinhaMapa(jogo), c = ObtemColunaMapa(jogo);

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
    jogo.stats = AdicionaContMove(jogo);
    //jogo.stats.cont_moves++;
    
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
    int l = ObtemLinhaCabeca(jogo), c = ObtemColunaCabeca(jogo);
    
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
