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
    int cont_moves; // contador de movimentos;
    int nopoint_moves; // contador de movimentos sem pontuar;
    int moves_down; // contador de movimentos pra baixo;
    int moves_up; // contador de movimentos pra cima;
    int moves_left; // contador de movimentos pra esquerda;
    int moves_right; // contador de movimentos pra direita;
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
tMapa LeMapa(tMapa mapa, FILE * pfile);
void ImprimeMapa(tMapa mapa, FILE * pfile_out);

// funcoes de inicializacao;
tCobra IncializaParamCobra(tCobra cobra);
tEstatisticas InicializaParamStats(tEstatisticas stats);
tEstatisticas InicializaHeatMap(tEstatisticas stats, tMapa mapa, tCobra cobra);
tJogo InicializaParamJogo(tJogo jogo);

// funcoes base;
tJogo JogaJogo(tJogo jogo, FILE * pfile_resumo);
int AcabouJogo(tJogo jogo, FILE * pfile_resumo);
void ImprimeEstadoJogo(tJogo jogo, tMapa mapa);

// funcoes para tratar estatisticas;
tEstatisticas PreencheHeatMap(tCobra cobra, tEstatisticas stats);
void ImprimeHeatMap(tMapa mapa, tEstatisticas stats, FILE * pfile_heatmap);
void ImprimeEstatisticas(tEstatisticas stats, FILE * pfile_stats);
void ImprimeRanking(tMapa mapa, tEstatisticas stats, FILE * pfile_rank);

// funcoes que retornam variavel;
char ObtemPosCabeca(tCobra cobra);
char ObtemLance(tJogo jogo);
int ObtemTamCorpo(tCobra cobra);
int ObtemLinhaCorpo(tCobra cobra, int i);
int ObtemColunaCorpo(tCobra cobra, int i);
int ObtemLinhaCabeca(tCobra cobra);
int ObtemColunaCabeca(tCobra cobra);
int ObtemLinhaMapa(tMapa mapa);
int ObtemColunaMapa(tMapa cobra);
int ObtemQtdComida(tMapa mapa);
int ObtemQtdMoves(tEstatisticas stats);

// funcoes que atualizam variaveis;
tCobra AtualizaPosInicial(tCobra cobra, tMapa mapa);
tCobra AtualizaPosCorpo(tCobra cobra, int l, int c);
tCobra AtualizaLinhaCorpo(tCobra cobra, int i, int atualiza);
tCobra AtualizaColunaCorpo(tCobra cobra, int i, int atualiza);
tCobra AtualizaLinhaCabeca(tCobra cobra, int atualiza);
tCobra AtualizaColunaCabeca(tCobra cobra, int atualiza);
tCobra AtualizaMorteCobra(tCobra cobra, int check);
tCobra AtualizaPosCabeca(tCobra cobra, char check);
tCobra AdicionaTamCorpo(tCobra cobra);
tMapa AtualizaMapa(tMapa mapa, int l, int c, char caract);
tEstatisticas AdicionaContMove(tEstatisticas stats);
tEstatisticas AdicionaNoPointMove(tEstatisticas stats);
tEstatisticas AdicionaMoveRight(tEstatisticas stats);
tEstatisticas AdicionaMoveLeft(tEstatisticas stats);
tEstatisticas AdicionaMoveUp(tEstatisticas stats);
tEstatisticas AdicionaMoveDown(tEstatisticas stats);

// funcoes que verificam algo e retornam 1 ou 0 (V ou F);
int VerificaMorteCobra(tCobra cobra);
int UltrapassaLimites(tCobra cobra, tMapa mapa);
int EhParede(tMapa mapa, int l, int c);
int EhComida(tMapa mapa, int l, int c);
int EhCabecaInicial(tMapa mapa, int l, int c);
int EhCorpo(tMapa mapa, int l, int c);
int EhVazio(tMapa mapa, int l, int c);
int EhTunel(tMapa mapa, int l, int c);

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
    jogo.mapa = LeMapa(jogo.mapa, pfile_map);
    ImprimeMapa(jogo.mapa, pfile_out);

    // inicializa parametros do jogo, alem do mapa;
    jogo = InicializaParamJogo(jogo);

    // joga jogo enquanto nao o jogo nao acabou;
    while(!AcabouJogo(jogo, pfile_resumo)){
        jogo = JogaJogo(jogo, pfile_resumo);
    }
    // imprime estatisticas em seus respectivos arquivos;
    ImprimeRanking(jogo.mapa, jogo.stats, pfile_rank);
    ImprimeEstatisticas(jogo.stats, pfile_stats);
    ImprimeHeatMap(jogo.mapa, jogo.stats, pfile_heatmap);

    fclose(pfile_map);
    fclose(pfile_out);
    fclose(pfile_resumo);
    fclose(pfile_stats);
    fclose(pfile_heatmap);
    fclose(pfile_rank);

    return 0;
}
// lendo o mapa;
tMapa LeMapa(tMapa mapa, FILE * pfile){
    // le linha e coluna;
    fscanf(pfile, "%d %d", &mapa.linha, &mapa.coluna);

    int i, j, linha = mapa.linha, coluna = mapa.coluna;
    int cont_comida = 0;
    int cont_tunel = 0;

    fscanf(pfile, "%*c"); // consome o \n;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            fscanf(pfile, "%c", &mapa.matriz[i][j]); // le cada elemento do mapa;
            if(EhComida(mapa, i, j)){
                cont_comida++;
            }
            if(EhTunel(mapa, i, j)){ // armazena as posicoes dos 2 tuneis;
                if(!cont_tunel){
                    mapa.linha_tunel[0] = i;
                    mapa.coluna_tunel[0] = j;
                    cont_tunel++;
                }
                else{
                    mapa.linha_tunel[1] = i;
                    mapa.coluna_tunel[1] = j;
                }
            }
        }
        fscanf(pfile, "%*c"); // consome o \n;
    }
    mapa.qtd_comida = cont_comida;

    return mapa;
}

void ImprimeMapa(tMapa mapa, FILE * pfile_out){ // imprime mapa no arquivo inicializacao.txt;
    int i, j, linha = mapa.linha, coluna = mapa.coluna;
    int linha_ini, coluna_ini;

    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            fprintf(pfile_out, "%c", mapa.matriz[i][j]);
            if(mapa.matriz[i][j] == RIGHT){
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
    jogo.cobra = IncializaParamCobra(jogo.cobra);
    jogo.cobra = AtualizaPosInicial(jogo.cobra, jogo.mapa);
    jogo.stats = InicializaParamStats(jogo.stats);
    jogo.stats = InicializaHeatMap(jogo.stats, jogo.mapa, jogo.cobra);

    return jogo;
}

tCobra IncializaParamCobra(tCobra cobra){
    cobra.pos_cabeca = RIGHT;
    cobra.tam_corpo = 0;
    cobra.morreu = 0;

    return cobra;
}

tEstatisticas InicializaParamStats(tEstatisticas stats){
    stats.cont_moves = 0;
    stats.nopoint_moves = 0;
    stats.moves_down = 0;
    stats.moves_up = 0;
    stats.moves_left = 0;
    stats.moves_right = 0;

    return stats;
}

tEstatisticas InicializaHeatMap(tEstatisticas stats, tMapa mapa, tCobra cobra){
    int i, j;

    int linha = ObtemLinhaMapa(mapa), coluna = ObtemColunaMapa(mapa);
    // inicializa cada posicao do heatmap com 0;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            stats.heatmap[i][j] = 0;
        }
    }
    // preenche com a posicao inicial da cobra;
    stats = PreencheHeatMap(cobra, stats);

    return stats;
}

tCobra AtualizaPosInicial(tCobra cobra, tMapa mapa){
    int i, j, linha = ObtemLinhaMapa(mapa), coluna = ObtemColunaMapa(mapa);
    // pega a posicao inicial da cobra;
    for(i = 1; i <= linha; i++){
        for(j = 1; j <= coluna; j++){
            if(EhCabecaInicial(mapa, i, j)){
                cobra = AtualizaLinhaCabeca(cobra, i);
                cobra = AtualizaColunaCabeca(cobra, j);
            }
        }
    }

    return cobra;
}

char ObtemPosCabeca(tCobra cobra){
    return cobra.pos_cabeca;
}

char ObtemLance(tJogo jogo){
    scanf("%c", &jogo.lance);
    scanf("%*c");

    return jogo.lance;
}

int ObtemTamCorpo(tCobra cobra){
    return cobra.tam_corpo;
}

int ObtemLinhaCorpo(tCobra cobra, int i){
    return cobra.linha_corpo[i];
}

int ObtemColunaCorpo(tCobra cobra, int i){
    return cobra.coluna_corpo[i];
}

int ObtemLinhaCabeca(tCobra cobra){
    return cobra.linha;
}

int ObtemColunaCabeca(tCobra cobra){
    return cobra.coluna;
}

int ObtemLinhaMapa(tMapa mapa){
    return mapa.linha;
}

int ObtemColunaMapa(tMapa mapa){
    return mapa.coluna;
}

int ObtemQtdComida(tMapa mapa){
    return mapa.qtd_comida;
}

int ObtemQtdMoves(tEstatisticas stats){
    return stats.cont_moves;
}

int ObtemLinhaTunel(tMapa mapa, int pos){
    return mapa.linha_tunel[pos];
}

int ObtemColunaTunel(tMapa mapa, int pos){
    return mapa.coluna_tunel[pos];
}

tCobra AtualizaPosCorpo(tCobra cobra, int l, int c){
    cobra.linha_corpo[cobra.tam_corpo] = l;
    cobra.coluna_corpo[cobra.tam_corpo] = c;

    return cobra;
}

tCobra AtualizaLinhaCorpo(tCobra cobra, int i, int atualiza){
    cobra.linha_corpo[i] = atualiza;

    return cobra;
}

tCobra AtualizaColunaCorpo(tCobra cobra, int i, int atualiza){
    cobra.coluna_corpo[i] = atualiza;

    return cobra;
}

tCobra AtualizaLinhaCabeca(tCobra cobra, int atualiza){
    cobra.linha = atualiza;

    return cobra;
}

tCobra AtualizaColunaCabeca(tCobra cobra, int atualiza){
    cobra.coluna = atualiza;

    return cobra;
}

tMapa AtualizaMapa(tMapa mapa, int l, int c, char caract){
    mapa.matriz[l][c] = caract;

    return mapa;
}

tCobra AtualizaMorteCobra(tCobra cobra, int check){
    cobra.morreu = check;

    return cobra;
}

tCobra AtualizaPosCabeca(tCobra cobra, char check){
    cobra.pos_cabeca = check;

    return cobra;
}

tCobra AdicionaTamCorpo(tCobra cobra){
    cobra.tam_corpo++;

    return cobra;
}

tEstatisticas AdicionaContMove(tEstatisticas stats){
    stats.cont_moves++;

    return stats;
}

tEstatisticas AdicionaNoPointMove(tEstatisticas stats){
    stats.nopoint_moves++;

    return stats;
}

tEstatisticas AdicionaMoveRight(tEstatisticas stats){
    stats.moves_right++;

    return stats;
}

tEstatisticas AdicionaMoveLeft(tEstatisticas stats){
    stats.moves_left++;

    return stats;
}

tEstatisticas AdicionaMoveUp(tEstatisticas stats){
    stats.moves_up++;

    return stats;
}

tEstatisticas AdicionaMoveDown(tEstatisticas stats){
    stats.moves_down++;

    return stats;
}

int IdentificaTunel(tMapa mapa, int l, int c){ // retorna V se for o tunel mais proximo da posicao 0 0 e F se for o outro;
    return l == mapa.linha_tunel[0] && c == mapa.coluna_tunel[0];
}

int VerificaMorteCobra(tCobra cobra){
    return cobra.morreu;
}

int UltrapassaLimites(tCobra cobra, tMapa mapa){
    return cobra.linha > mapa.linha || cobra.linha < 1 || cobra.coluna > mapa.coluna || cobra.coluna < 1;
}

int EhParede(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == PAREDE;
}

int EhComida(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == COMIDA;
}

int EhCabecaInicial(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == RIGHT;
}

int EhCorpo(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == CORPO;
}

int EhDinheiro(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == DINHEIRO;
}

int EhVazio(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == VAZIO;
}

int EhTunel(tMapa mapa, int l, int c){
    return mapa.matriz[l][c] == TUNEL;
}

tCobra MudaPosCabeca(tJogo jogo){ // rotaciona a cabeca da cobra de acordo com o lance;
    tCobra cobra;
    cobra = jogo.cobra;
    char cabeca = ObtemPosCabeca(cobra);

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

    cobra.pos_cabeca = cabeca;
    jogo.cobra = cobra;

    return jogo.cobra;
}

tJogo MovimentaCorpo(tJogo jogo, int l, int c){
    int i, aux1, aux2;
    int tam;
    int l2, c2;

    tam = ObtemTamCorpo(jogo.cobra);

    if(!EhCorpo(jogo.mapa, l, c)){ /* a posicao deve ser diferente de CORPO pois se na posicao a frente estiver uma comida,
                                a funcao AtualizaPosCorpo coloca um corpo no lugar e nao eh necessario mover o corpo
                                quando a cobra passa pela COMIDA;*/
        for(i = 0; i < tam; i++){
            if(!i){
                l2 = ObtemLinhaCorpo(jogo.cobra, i);
                c2 = ObtemColunaCorpo(jogo.cobra, i);
                jogo.mapa = AtualizaMapa(jogo.mapa, l2, c2, VAZIO); // limpa a ultima posicao do corpo no mapa;
            }
            if(i == tam - 1){ // a primeira posicao do corpo ocupara a antiga posicao da cabeca;
                jogo.cobra = AtualizaLinhaCorpo(jogo.cobra, i, l);
                jogo.cobra = AtualizaColunaCorpo(jogo.cobra, i, c);
                jogo.mapa = AtualizaMapa(jogo.mapa, l, c, CORPO);
                break;
            }
            // o corpo i se desloca para a posicao do corpo i + 1;
            l2 = ObtemLinhaCorpo(jogo.cobra, i + 1);
            c2 = ObtemColunaCorpo(jogo.cobra, i + 1);
            jogo.cobra = AtualizaLinhaCorpo(jogo.cobra, i, l2);
            jogo.cobra = AtualizaColunaCorpo(jogo.cobra, i, c2);

            // atualiza no mapa;
            jogo.mapa = AtualizaMapa(jogo.mapa, l2, c2, CORPO);
        }
    }

    return jogo;
}

tJogo MataCorpo(tJogo jogo){
    int i, aux1, aux2;
    int tam;

    tam = ObtemTamCorpo(jogo.cobra);

    for(i = 0; i < tam; i++){ // varre todas as posicoes do corpo, colocando um 'X' em cada uma;
        aux1 = ObtemLinhaCorpo(jogo.cobra, i);
        aux2 = ObtemColunaCorpo(jogo.cobra, i);
        jogo.mapa = AtualizaMapa(jogo.mapa, aux1, aux2, MORTA);
    }

    return jogo;
}

tJogo MoveDireita(tJogo jogo){
    int l, c;
    // salva as posicoes nao alteradas da cabeca;
    int save_linha = ObtemLinhaCabeca(jogo.cobra);
    int save_coluna = ObtemColunaCabeca(jogo.cobra);
    
    l = ObtemLinhaCabeca(jogo.cobra);
    c = ObtemColunaCabeca(jogo.cobra);
    c += 1; // provavel posicao futura da cabeca da cobra;

    jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

    if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, 1); // atualiza a posicao da cabeca pra coluna 1;

        c = ObtemColunaCabeca(jogo.cobra);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
    }
    if(EhTunel(jogo.mapa, l, c)){
        c = ObtemColunaCabeca(jogo.cobra);

        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo.mapa, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }
        // atualiza a futura posicao da cabeca da cobra;
        l = ObtemLinhaTunel(jogo.mapa, ident_tunel2);
        c = ObtemColunaTunel(jogo.mapa, ident_tunel2);
        c += 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
        if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaColunaCabeca(jogo.cobra, 1);

            c = ObtemColunaCabeca(jogo.cobra);

            if(EhComida(jogo.mapa, l, c)){
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
                jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
            }
        }
    }
    else{
        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO); // coloca CORPO na antiga posicao da cabeca da cobra;
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO); // coloca VAZIO na antiga posicao da cabeca da cobra;
        }
    }

    return jogo;
}

tJogo MoveEsquerda(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    int save_linha = ObtemLinhaCabeca(jogo.cobra);
    int save_coluna = ObtemColunaCabeca(jogo.cobra);

    linha_mapa = ObtemLinhaMapa(jogo.mapa);
    coluna_mapa = ObtemColunaMapa(jogo.mapa);

    l = ObtemLinhaCabeca(jogo.cobra);
    c = ObtemColunaCabeca(jogo.cobra);
    c -= 1;

    jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

    if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, coluna_mapa);

        c = ObtemColunaCabeca(jogo.cobra);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo.mapa, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo.mapa, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo.mapa, ident_tunel2);
        c = ObtemColunaTunel(jogo.mapa, ident_tunel2);
        c -= 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else if(!UltrapassaLimites(jogo.cobra, jogo.mapa)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaColunaCabeca(jogo.cobra, coluna_mapa);

            c = ObtemColunaCabeca(jogo.cobra);

            if(EhComida(jogo.mapa, l, c)){
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MoveCima(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    linha_mapa = ObtemLinhaMapa(jogo.mapa);
    coluna_mapa = ObtemColunaMapa(jogo.mapa);

    int save_linha = ObtemLinhaCabeca(jogo.cobra);
    int save_coluna = ObtemColunaCabeca(jogo.cobra);

    l = ObtemLinhaCabeca(jogo.cobra);
    c = ObtemColunaCabeca(jogo.cobra);
    l -= 1;

    jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);
    
    if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, linha_mapa);

        l = ObtemLinhaCabeca(jogo.cobra);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo.mapa, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo.mapa, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo.mapa, ident_tunel2);
        c = ObtemColunaTunel(jogo.mapa, ident_tunel2);
        l -= 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, linha_mapa);

            l = ObtemLinhaCabeca(jogo.cobra);

            if(EhComida(jogo.mapa, l, c)){
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MoveBaixo(tJogo jogo){
    int l, c, linha_mapa, coluna_mapa;

    int save_linha = ObtemLinhaCabeca(jogo.cobra);
    int save_coluna = ObtemColunaCabeca(jogo.cobra);

    linha_mapa = ObtemLinhaMapa(jogo.mapa);
    coluna_mapa = ObtemColunaMapa(jogo.mapa);

    l = ObtemLinhaCabeca(jogo.cobra);
    c = ObtemColunaCabeca(jogo.cobra);
    l += 1;

    jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);

    if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, 1);
            
        l = ObtemLinhaCabeca(jogo.cobra);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }
    else if(EhTunel(jogo.mapa, l, c)){
        int ident_tunel1, ident_tunel2;

        if(IdentificaTunel(jogo.mapa, l, c)){ // a cobra esta no tunel mais proximo da posicao 0 0;
            ident_tunel1 = 0; // tunel destino da cobra;
            ident_tunel2 = 1; // tunel que a cobra "esta";
        }
        else{ // a cobra esta no tunel mais longe da posicao 0 0;
            ident_tunel1 = 1;
            ident_tunel2 = 0;
        }

        l = ObtemLinhaTunel(jogo.mapa, ident_tunel2);
        c = ObtemColunaTunel(jogo.mapa, ident_tunel2);
        l += 1;

        jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, l);
        jogo.cobra = AtualizaColunaCabeca(jogo.cobra, c);

        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
        if(UltrapassaLimites(jogo.cobra, jogo.mapa)){ // verifica se a cobra ultrapassa os limites do mapa;
            jogo.cobra = AtualizaLinhaCabeca(jogo.cobra, 1);

            l = ObtemLinhaCabeca(jogo.cobra);

            if(EhComida(jogo.mapa, l, c)){
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
                jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
            }
            else{
                jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
            }
        }
    }
    else{
        if(EhComida(jogo.mapa, l, c)){
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, CORPO);
            jogo.cobra = AtualizaPosCorpo(jogo.cobra, save_linha, save_coluna);
        }
        else{
            jogo.mapa = AtualizaMapa(jogo.mapa, save_linha, save_coluna, VAZIO);
        }
    }

    return jogo;
}

tJogo MovimentaCobra(tJogo jogo){
    int save_linha = ObtemLinhaCabeca(jogo.cobra), save_coluna = ObtemColunaCabeca(jogo.cobra); // guarda a posicao da cabeca da cobra sem as mudancas a serem feitas;

    char cabeca = ObtemPosCabeca(jogo.cobra);

    if(cabeca == RIGHT){ // CASO 1;
        jogo = MoveDireita(jogo);
        jogo.stats = AdicionaMoveRight(jogo.stats);
    }
    else if(cabeca == LEFT){ // CASO 2;
        jogo = MoveEsquerda(jogo);
        jogo.stats = AdicionaMoveLeft(jogo.stats);
    }
    else if(cabeca == UP){ // CASO 3;
        jogo = MoveCima(jogo);
        jogo.stats = AdicionaMoveUp(jogo.stats);
    }
    else if(cabeca == DOWN){ // CASO 4;
        jogo = MoveBaixo(jogo);
        jogo.stats = AdicionaMoveDown(jogo.stats);
    }

    jogo = MovimentaCorpo(jogo, save_linha, save_coluna);

    return jogo;
}

tJogo CorrigeMovimento(tJogo jogo, FILE * pfile_resumo){ // corrige o movimento da cabeca e atualiza variaveis;
    int l, c, tam, qtd_comida, qtd_moves;

    l = ObtemLinhaCabeca(jogo.cobra);
    c = ObtemColunaCabeca(jogo.cobra);
    qtd_moves = ObtemQtdMoves(jogo.stats);

    // atualiza as variaveis de acordo com a proxima posicao da cobra;
    if(EhParede(jogo.mapa, l, c) || EhCorpo(jogo.mapa, l, c)){
        jogo.cobra = AtualizaMorteCobra(jogo.cobra, 1);
        jogo.cobra = AtualizaPosCabeca(jogo.cobra, MORTA);
        jogo = MataCorpo(jogo);
        // printa resumo;
        fprintf(pfile_resumo, "Movimento %d (%c) resultou no fim de jogo por conta de colisao\n", qtd_moves, jogo.lance);
        jogo.stats = AdicionaNoPointMove(jogo.stats);
    }
    else if(EhComida(jogo.mapa, l, c)){
        jogo.pontuacao++;
        jogo.cobra = AdicionaTamCorpo(jogo.cobra);

        tam = ObtemTamCorpo(jogo.cobra);
        qtd_comida = ObtemQtdComida(jogo.mapa);

        if(tam != qtd_comida){
            // printa resumo;
            fprintf(pfile_resumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d\n", qtd_moves, jogo.lance, tam + 1);
        }
    }
    else if(EhDinheiro(jogo.mapa, l, c)){
        jogo.pontuacao += 10;
        // printa resumo;
        fprintf(pfile_resumo, "Movimento %d (%c) gerou dinheiro\n", qtd_moves, jogo.lance);
    }
    else{
        jogo.stats = AdicionaNoPointMove(jogo.stats);
    }
    // atualiza a posicao da cabeca da cobra no mapa;
    char poscabeca = ObtemPosCabeca(jogo.cobra);
    jogo.mapa = AtualizaMapa(jogo.mapa, l, c, poscabeca);

    return jogo;
}

void ImprimeEstadoJogo(tJogo jogo, tMapa mapa){
    int i, j;
    int l = ObtemLinhaMapa(jogo.mapa), c = ObtemColunaMapa(jogo.mapa);

    printf("Estado do jogo apos o movimento '%c':\n", jogo.lance);
    for(i = 1; i <= l; i++){
        for(j = 1; j <= c; j++){
            printf("%c", mapa.matriz[i][j]);
        }
        printf("\n");
    }
    printf("Pontuacao: %d\n", jogo.pontuacao);
}

tJogo JogaJogo(tJogo jogo, FILE * pfile_resumo){
    jogo.lance = ObtemLance(jogo);
    jogo.stats = AdicionaContMove(jogo.stats);
    
    jogo.cobra = MudaPosCabeca(jogo);
    jogo = MovimentaCobra(jogo);
    jogo = CorrigeMovimento(jogo, pfile_resumo);
    ImprimeEstadoJogo(jogo, jogo.mapa);
    jogo.stats = PreencheHeatMap(jogo.cobra, jogo.stats);
    
    return jogo;
}

int AcabouJogo(tJogo jogo, FILE * pfile_resumo){
    if(VerificaMorteCobra(jogo.cobra)){
        printf("Game over!\n");
        printf("Pontuacao final: %d\n", jogo.pontuacao);
        return 1;
    }

    int tam_corpo = ObtemTamCorpo(jogo.cobra);
    int qtd_comida = ObtemQtdComida(jogo.mapa);
    int cont_moves = ObtemQtdMoves(jogo.stats);

    // verifica se o tamanho do corpo eh igual a quantidade de comida existente no mapa nao modificado;
    // verifica se existe comida no mapa;
    if(tam_corpo == qtd_comida || qtd_comida == 0){
        printf("Voce venceu!\n");
        printf("Pontuacao final: %d\n", jogo.pontuacao);
        if(tam_corpo == qtd_comida){
            fprintf(pfile_resumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d, terminando o jogo\n", cont_moves, jogo.lance, tam_corpo + 1);
        }
        return 1;
    }
    printf("\n");
    return 0;
}

void ImprimeEstatisticas(tEstatisticas stats, FILE * pfile_stats){
    fprintf(pfile_stats, "Numero de movimentos: %d\n", stats.cont_moves);
    fprintf(pfile_stats, "Numero de movimentos sem pontuar: %d\n", stats.nopoint_moves);
    fprintf(pfile_stats, "Numero de movimentos para baixo: %d\n", stats.moves_down);
    fprintf(pfile_stats, "Numero de movimentos para cima: %d\n", stats.moves_up);
    fprintf(pfile_stats, "Numero de movimentos para esquerda: %d\n", stats.moves_left);
    fprintf(pfile_stats, "Numero de movimentos para direita: %d\n", stats.moves_right);
}

tEstatisticas PreencheHeatMap(tCobra cobra, tEstatisticas stats){
    int l = ObtemLinhaCabeca(cobra), c = ObtemColunaCabeca(cobra);
    
    stats.heatmap[l][c]++;

    return stats;
}

void ImprimeHeatMap(tMapa mapa, tEstatisticas stats, FILE * pfile_heatmap){
    int i, j;

    for(i = 1; i <= mapa.linha; i++){
        for(j = 1; j <= mapa.coluna; j++){
            if(j == mapa.coluna){
                fprintf(pfile_heatmap, "%d", stats.heatmap[i][j]);
            }
            else{
                fprintf(pfile_heatmap, "%d ", stats.heatmap[i][j]);
            }
        }
        fprintf(pfile_heatmap, "\n");
    }
}

void ImprimeRanking(tMapa mapa, tEstatisticas stats, FILE * pfile_rank){
    int i, j, aux, maior;
    // obtem o maior numero de vezes que a cobra passou em uma especifica posicao;
    for(i = 1; i <= mapa.linha; i++){
        for(j = 1; j <= mapa.coluna; j++){
            if(i == 1 && j == 1){
                maior = stats.heatmap[i][j];
            }
            else if(stats.heatmap[i][j] > maior){
                maior = stats.heatmap[i][j];
            }
        }
    }
    // printa do maior para o menor;
    for(aux = maior; aux > 0; aux--){
        for(i = 1; i <= mapa.linha; i++){
            for(j = 1; j <= mapa.coluna; j++){
                if(stats.heatmap[i][j] == aux){
                    fprintf(pfile_rank, "(%d, %d) - %d\n", i - 1, j - 1, stats.heatmap[i][j]);
                }
            }
        }
    }
}
