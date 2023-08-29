/* iaed-23 - ist1102079 - project2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definicao Constantes */

#define VERDADE 1
#define FALSO 0
#define NAO_ENCONTRADO -1

#define ERRO_PARSING 1

/* Definicao de Tipos de Dados */

typedef struct Ligacao Ligacao;

typedef struct {
    char *nome;
    double custoTotal;
    double duracaoTotal;
    Ligacao **ligacoes;
    int numLigacoes;
} Carreira;

typedef struct {
    char *nome;
    double latitude, longitude;
    Carreira **carreiras;
    int numCarreiras;
} Paragem;

struct Ligacao {
    Carreira *carreira;
    Paragem *origem, *destino;
    double custo, duracao;
};


/* Funções para parsing do input */

/* Le espaços. Devolve 0 se chegou ao final de linha ou 1 caso contrário. */

int leEspacos() {
    int c;
    while ((c = getchar()) == ' ' || c == '\t');
    if (c == '\n') 
        return 0;
    ungetc(c, stdin);
    return 1;
}

/* Le um nome para a string que recebe como parametro. */

void leNome(char **s) {
    int i = 0, c;
    (*s)[0] = getchar();
    if ((*s)[0] != '"') {
        i = 1;
        while ((c = getchar()) != ' ' && c != '\t' && c != '\n') {
            if ((*s = (char*)realloc(*s, (i + 2) * sizeof(char))) == NULL) {
                printf("No memory.\n");
                exit(0);
            }
            (*s)[i++] = c;
        }
        ungetc(c, stdin);
    }
    else {
        while((c = getchar()) != '"') {
            if ((*s = (char*)realloc(*s, (i + 2) * sizeof(char))) == NULL) {
                printf("No memory.\n");
                exit(0);
            }
            (*s)[i++] = c;
        }
    }   
    (*s)[i++] = '\0';
}

/* Le todo o texto até ao final de linha. */

void leAteFinalLinha() {
    char c = getchar();
    while (c != '\n')
        c = getchar();
}


/* Fnções para tratar comandos */

/* Funções para tratar as carreiras */

/* Mostra no output a carreira com indice i */

void mostraCarreira(int i, Carreira ***_carreiras) {
    int numLigacoes = (*_carreiras)[i]->numLigacoes;

    printf("%s ", (*_carreiras)[i]->nome);
    if (numLigacoes > 0) {
        printf("%s %s ", (*_carreiras)[i]->ligacoes[0]->origem->nome,
                        (*_carreiras)[i]->ligacoes[numLigacoes-1]->destino->nome);
        printf("%d ", numLigacoes+1);
        printf("%.2f ", (*_carreiras)[i]->custoTotal);
        printf("%.2f\n", (*_carreiras)[i]->duracaoTotal);
    }
    else 
        printf("%d %.2f %.2f\n", 0, 0.0, 0.0);
}

/* Mostra as ligações da carreira com indice i. */

void mostraLigacoesCarreira(Carreira *carreira, int inverso) {
    int l, numLigacoes = carreira->numLigacoes;

    if (numLigacoes == 0) 
        return;
    if (inverso == FALSO) {
        for (l = 0; l < numLigacoes; l++) {
            printf("%s, ", carreira->ligacoes[l]->origem->nome);
        }
        printf("%s\n", carreira->ligacoes[numLigacoes-1]->destino->nome);
    }
    else {
        for (l = numLigacoes-1; l >= 0; l--) {
            printf("%s, ", carreira->ligacoes[l]->origem->nome);
        }
        printf("%s\n", carreira->ligacoes[numLigacoes-1]->destino->nome);
    }
}

/* Mostra todas as carreiras. */

void listaCarreiras(int _numCarreiras, Carreira ***_carreiras) {
    int i;

    for (i = 0; i < _numCarreiras; i++)
        mostraCarreira(i, _carreiras);
}

/* Procura uma carreira por nome.
   Devolve o indice da carreira ou NAO_ENCONTRADO se não existe. */

Carreira *encontraCarreira(char *nomeCarreira, Carreira ***_carreiras, int _numCarreiras) {
    int i;

    for (i = 0; i < _numCarreiras; i++)
        if (strcmp((*_carreiras)[i]->nome, nomeCarreira) == 0)
            return (*_carreiras)[i];
    return NULL;
}

int encontraCarreiraIndex(char *nomeCarreira, Carreira ***_carreiras, int _numCarreiras) {
    int i;

    for (i = 0; i < _numCarreiras; i++)
        if (strcmp((*_carreiras)[i]->nome, nomeCarreira) == 0)
            return i;
    return NAO_ENCONTRADO;
}

/* Procura se uma paragem existe numa carreira.
   Se existir, devolve o indice da primeira ligação que usa
   a paragem. Caso contrário, devolve NAO_ENCONTRADO. */

Ligacao *encontraParagemCarreira(Carreira *carreira, Paragem *paragem) {
    int numLigacoes = carreira->numLigacoes;
    int i;

    for (i = 0; i < numLigacoes; i++)
        if (carreira->ligacoes[i]->origem == paragem)
            return carreira->ligacoes[i];
    if (carreira->ligacoes[numLigacoes-1]->destino == paragem)
        return carreira->ligacoes[numLigacoes-1];
    return NULL;
}

/* Cria uma carreira nova. */

void criaCarreira(char *nomeCarreira, Carreira ****_carreiras, int **_numCarreiras) {
    if (**_numCarreiras == 0)
        **_carreiras = (Carreira**)malloc(sizeof(Carreira*));
    else
        **_carreiras = (Carreira**)realloc(**_carreiras, (**_numCarreiras + 1) * sizeof(Carreira*));
    (**_carreiras)[**_numCarreiras] = (Carreira*)malloc(sizeof(Carreira));
    (**_carreiras)[**_numCarreiras]->nome = (char*)malloc((strlen(nomeCarreira) + 1) * sizeof(char));
    (**_carreiras)[**_numCarreiras]->custoTotal = 0.0;
    (**_carreiras)[**_numCarreiras]->duracaoTotal = 0.0;
    (**_carreiras)[**_numCarreiras]->numLigacoes = 0;
    strcpy((**_carreiras)[**_numCarreiras]->nome, nomeCarreira);
    (**_numCarreiras)++;
}

/* Verifica se a string é um prefixo de tamanho pelo menos 3 da
   palavra inverso. */

int verificaInversoOk(char *s) {
    char inv[] = "inverso";
    int size = strlen(s), i;

    if (size < 3 || size > 7)
        return FALSO;
    for (i = 0; i < size; i++)
        if (inv[i] != s[i])
            return FALSO;
    return VERDADE;
}

/* Função para tratar do comando 'c'. */

void carreiras(Carreira ***_carreiras, int *_numCarreiras) {
    char *s, *buffer;
    Carreira *carreira;
    int fimLinha = leEspacos();
    
    if (!fimLinha) {
        listaCarreiras(*_numCarreiras, _carreiras);
        return;
    }
    s = (char*)malloc(2 * sizeof(char));
    leNome(&s);
    carreira = encontraCarreira(s, _carreiras, *_numCarreiras);
    fimLinha = leEspacos();
    if (!fimLinha) {
        if (carreira == NULL)
            criaCarreira(s, &_carreiras, &_numCarreiras);
        else
            mostraLigacoesCarreira(carreira, FALSO);
    }
    else {
        buffer = (char*)malloc(2 * sizeof(char));
        leNome(&buffer);
        if (verificaInversoOk(buffer))
            mostraLigacoesCarreira(carreira, VERDADE);
        else
            printf("incorrect sort option.\n");
        free(buffer);
        leAteFinalLinha();
    }
    free(s);
}

/* Funções para tratar as paragens */

/* Mostra uma paragem. */

void mostraParagem(int i, Paragem ***_paragens) {
    printf("%s: %16.12f %16.12f %d\n", (*_paragens)[i]->nome, (*_paragens)[i]->latitude, 
            (*_paragens)[i]->longitude, (*_paragens)[i]->numCarreiras);
}

/* Mostra todas as paragens. */

void listaParagens(Paragem ***_paragens, int _numParagens) {
    int i;
    for (i = 0; i < _numParagens; i++)
        mostraParagem(i, _paragens);
}

/* Verifica se existe uma paragem com um determinado nome.
   Se existir devolve o indice. Caso contrário, devolve NAO_ENCONTRADO. */

Paragem *encontraParagem(char *nomeParagem, Paragem ***_paragens, int _numParagens) {
    int i;

    for (i = 0; i < _numParagens; i++)
        if (strcmp(nomeParagem, (*_paragens)[i]->nome) == 0)
            return (*_paragens)[i];
    return NULL;
}

/* Cria uma nova paragem. */

void criaParagem(char *nomeParagem, double latitude, double longitude, Paragem ****_paragens, int **_numParagens) {
    if (**_numParagens == 0)
        **_paragens = (Paragem**)malloc(sizeof(Paragem*));
    else
        **_paragens = (Paragem**)realloc(**_paragens, (**_numParagens + 1) * sizeof(Paragem*));
    (**_paragens)[**_numParagens] = (Paragem*)malloc(sizeof(Paragem));
    (**_paragens)[**_numParagens]->nome = (char*)malloc((strlen(nomeParagem) + 1) * sizeof(char));
    (**_paragens)[**_numParagens]->latitude = latitude;
    (**_paragens)[**_numParagens]->longitude = longitude;
    (**_paragens)[**_numParagens]->numCarreiras = 0;
    strcpy((**_paragens)[**_numParagens]->nome, nomeParagem);
    (**_numParagens)++;
}

/* Função para tratar o comando 'p'. */

void paragens(Paragem ***_paragens, int *_numParagens) {
    char *s;
    Paragem *paragem;
    int fimLinha = leEspacos();
    
    if (!fimLinha) {
        listaParagens(_paragens, *_numParagens);
        return;
    }
    s = (char*)malloc(2 * sizeof(char));
    leNome(&s);
    fimLinha = leEspacos();
    if (!fimLinha) {
        if ((paragem = encontraParagem(s, _paragens, *_numParagens)) == NULL)
            printf("%s: no such stop.\n", s);
        else   
            printf("%16.12f %16.12f\n", paragem->latitude, paragem->longitude);
    }
    else {
        if (encontraParagem(s, _paragens, *_numParagens) == NULL) {
            double latitude, longitude;
            if (scanf("%lf%lf", &latitude, &longitude) >= 0) {}
            criaParagem(s, latitude, longitude, &_paragens, &_numParagens);
        }
        else
            printf("%s: stop already exists.\n", s);
        leAteFinalLinha();
    }
    free(s);
}

/* Funções para tratar as Ligações */

/* Adiciona uma nova ligação. */

Ligacao *criaLigacao(Carreira *carreira, Paragem *paragemOrigem, Paragem *paragemDestino,
                 double custo, double duracao) {
    Ligacao *ligacao;
    ligacao = (Ligacao*)malloc(sizeof(Ligacao));
    ligacao->carreira = carreira;
    ligacao->origem = paragemOrigem;
    ligacao->destino = paragemDestino;
    ligacao->custo = custo;
    ligacao->duracao = duracao;
    return ligacao;
}

/* Aloca memoria para a lista de ligacoes. */

void alocaMemoriaIdLigacao(Ligacao ***ligacoes, int numLigacoes) {
    if (numLigacoes == 0)
        *ligacoes = (Ligacao**)malloc(sizeof(Ligacao*));
    else
        *ligacoes = (Ligacao**)realloc(*ligacoes, (numLigacoes + 1) * sizeof(Ligacao*));
}

/* Acrescenta uma nova ligação no fim de uma carreira. */

void acrescentaLigacaoFim(Carreira *carreira, Ligacao *ligacao) {
    int numLigacoesCarreira = carreira->numLigacoes;

    alocaMemoriaIdLigacao(&(carreira->ligacoes), numLigacoesCarreira);
    carreira->ligacoes[numLigacoesCarreira] = ligacao;
    carreira->custoTotal += ligacao->custo;
    carreira->duracaoTotal += ligacao->duracao;
    carreira->numLigacoes++;
}

/* Acrescenta uma nova ligação no início de uma carreira. */

void acrescentaLigacaoInicio(Carreira *carreira, Ligacao *ligacao) {
    int numLigacoesCarreira = carreira->numLigacoes;
    int i;

    alocaMemoriaIdLigacao(&(carreira->ligacoes), numLigacoesCarreira);
    for (i = numLigacoesCarreira-1; i >= 0; i--)
        carreira->ligacoes[i+1] = carreira->ligacoes[i];
    carreira->ligacoes[0] = ligacao;
    carreira->custoTotal += ligacao->custo;
    carreira->duracaoTotal += ligacao->duracao;
    carreira->numLigacoes++;
    
}

/* Adiciona que existe uma nova carreira associada a uma paragem. */

void adicionaCarreiraParagem(Paragem *paragem, Carreira *carreira) {
    int numCarreiras = paragem->numCarreiras; 
    if (numCarreiras == 0)
        paragem->carreiras = (Carreira**)malloc((numCarreiras + 1) * sizeof(Carreira*));
    else
        paragem->carreiras = (Carreira**)realloc(paragem->carreiras, (numCarreiras + 1) * sizeof(Carreira*));
    paragem->carreiras[paragem->numCarreiras] = carreira;
    paragem->numCarreiras++;
}

/* Adiciona uma nova ligação a uma carreira. */

void adicionaLigacao(Carreira *carreira, Paragem *paragemOrigem, Paragem *paragemDestino,
                     double custo, double duracao) {
    int numLigacoesCarreira = carreira->numLigacoes;
    Ligacao *ligacao;
    
    /* Adiciona primeira ligação da carreira */
    if (numLigacoesCarreira == 0) { 
        adicionaCarreiraParagem(paragemOrigem, carreira);
        if (paragemOrigem != paragemDestino)
            adicionaCarreiraParagem(paragemDestino, carreira);
        ligacao = criaLigacao(carreira, paragemOrigem, paragemDestino, custo, duracao);
        acrescentaLigacaoFim(carreira, ligacao);
    }  
    else {
        if (paragemOrigem == carreira->ligacoes[numLigacoesCarreira-1]->destino) {
            if (encontraParagemCarreira(carreira, paragemDestino) == NULL)
                adicionaCarreiraParagem(paragemDestino, carreira);
            ligacao = criaLigacao(carreira, paragemOrigem, paragemDestino, custo, duracao);
            acrescentaLigacaoFim(carreira, ligacao);
        }
        else if (paragemDestino == carreira->ligacoes[0]->origem) {
            if (encontraParagemCarreira(carreira, paragemOrigem) == NULL)
                adicionaCarreiraParagem(paragemOrigem, carreira);
            ligacao = criaLigacao(carreira, paragemOrigem, paragemDestino, custo, duracao);
            acrescentaLigacaoInicio(carreira, ligacao);
        }
        else 
            printf("link cannot be associated with bus line.\n");
    }
}

/* Função para tratar o comando 'l'. */

void ligacoes(Paragem ***_paragens, Carreira ***_carreiras, int _numCarreiras, int _numParagens) {
    char *nomeCarreira, *nomeOrigem, *nomeDestino;
    double custo, duracao;
    Carreira *carreira;
    Paragem *origem, *destino;
    nomeCarreira = (char*)malloc(2 * sizeof(char));
    nomeOrigem = (char*)malloc(2 * sizeof(char));
    nomeDestino = (char*)malloc(2 * sizeof(char));
    leEspacos();
    leNome(&nomeCarreira);
    leEspacos();
    leNome(&nomeOrigem);
    leEspacos();
    leNome(&nomeDestino);
    if (scanf("%lf%lf", &custo, &duracao)) {}
    leAteFinalLinha();

    carreira = encontraCarreira(nomeCarreira, _carreiras, _numCarreiras);
    if (carreira == NULL)
        printf("%s: no such line.\n", nomeCarreira);
    else {
        origem = encontraParagem(nomeOrigem, _paragens, _numParagens);
        if (origem == NULL) 
	    printf("%s: no such stop.\n", nomeOrigem);
	    else {
	        destino = encontraParagem(nomeDestino, _paragens, _numParagens);
	        if (destino == NULL)
	            printf("%s: no such stop.\n", nomeDestino);
	        else if (custo < 0.0 || duracao < 0.0)
	            printf("negative cost or duration.\n");
		    else 
		        adicionaLigacao(carreira, origem, destino, custo, duracao);
	    }
    }
    free(nomeCarreira);
    free(nomeOrigem);
    free(nomeDestino);
}

/* Funções para tratar as interseções */

/* Aplica Bubble Sort a um vector de identificadores de carreiras. */

void ordenaCarreiras(Carreira ***carreiras, int numCarreiras) {
    Carreira *aux;
    int i, j, houveTroca = VERDADE;
    for (i = 0; houveTroca && i < numCarreiras-1; i++) {
        houveTroca = FALSO;
	    for (j = 0; j < numCarreiras-1-i; j++) {
	        if (strcmp((*carreiras)[j]->nome, (*carreiras)[j+1]->nome) > 0) {
	            aux = (*carreiras)[j];
	            (*carreiras)[j] = (*carreiras)[j+1];
		        (*carreiras)[j+1] = aux;
		        houveTroca = VERDADE;
	        }
	    }
    }
}

/* Função para tratar o comando 'i'. */

void intersecoes(Paragem **_paragens, int _numParagens) {
    int i, c, numCarreiras;

    leAteFinalLinha();

    for (i = 0; i < _numParagens; i++) {
        numCarreiras = _paragens[i]->numCarreiras;
        if (numCarreiras > 1) {
            printf("%s %d:", _paragens[i]->nome, numCarreiras);
            ordenaCarreiras(&(_paragens[i]->carreiras), numCarreiras);
            for (c = 0; c < numCarreiras; c++) {
                printf(" %s", _paragens[i]->carreiras[c]->nome);
            }
             printf("\n");
        }
    }
}

void removeCarreiraParagens(Ligacao *ligacao, Paragem ****_paragens, int _numParagens, int ultimo) {
    int i, j, k;
    for (i = 0; i < _numParagens; i++) {
        if ((**_paragens)[i] == ligacao->origem) {
            for (j = 0; j < (**_paragens)[i]->numCarreiras; j++) {
                if ((**_paragens)[i]->carreiras[j] == ligacao->carreira) {
                        for (k = j; k < (**_paragens)[i]->numCarreiras-1; k++)
                        (**_paragens)[i]->carreiras[k] = (**_paragens)[i]->carreiras[k+1];
                    ((**_paragens)[i]->numCarreiras)--;
                    (**_paragens)[i]->carreiras = (Carreira**)realloc((**_paragens)[i]->carreiras, (**_paragens)[i]->numCarreiras * sizeof(Carreira*));
                    break;
                }
            }
        }
    }
    if (ultimo == VERDADE)
        for (i = 0; i < _numParagens; i++)
            if ((**_paragens)[i] == ligacao->destino) {
                for (j = 0; j < (**_paragens)[i]->numCarreiras; j++) {
                    if ((**_paragens)[i]->carreiras[j] == ligacao->carreira) {
                        for (k = j; k < (**_paragens)[i]->numCarreiras-1; k++)
                            (**_paragens)[i]->carreiras[k] = (**_paragens)[i]->carreiras[k+1];
                        ((**_paragens)[i]->numCarreiras)--;
                        (**_paragens)[i]->carreiras = (Carreira**)realloc((**_paragens)[i]->carreiras, (**_paragens)[i]->numCarreiras * sizeof(Carreira*));
                        break;
                    }
                }
            }
}

/* Função para tratar o comando r */

void removeCarreira(Carreira ***_carreiras, Paragem ***_paragens, int *_numCarreiras, int _numParagens) {
    char *nomeCarreira;
    int i, l, index, numLigacoes;
    Carreira *carreira;
    Ligacao *ligacao;
    nomeCarreira = (char*)malloc(2 * sizeof(char));
    leEspacos();
    leNome(&nomeCarreira);
    if ((carreira = encontraCarreira(nomeCarreira, _carreiras, *_numCarreiras)) == NULL)
        printf("%s: no such line.\n", nomeCarreira);
    else {
        numLigacoes = carreira->numLigacoes;
        for (i = 0; i < numLigacoes; i++) {
            ligacao = carreira->ligacoes[i];
            if (i == numLigacoes - 1)
                removeCarreiraParagens(ligacao, &_paragens, _numParagens, VERDADE);
            else
                removeCarreiraParagens(ligacao, &_paragens, _numParagens, FALSO);
            free(ligacao);
            for (l = i; l < numLigacoes - 1; l++)
                carreira->ligacoes[l] = carreira->ligacoes[l+1];
            carreira->ligacoes = (Ligacao**)realloc(carreira->ligacoes, --numLigacoes * sizeof(Ligacao*));
            i--;
        }
        index = encontraCarreiraIndex(nomeCarreira, _carreiras, *_numCarreiras);
        free(carreira->nome);
        if (*_numCarreiras > 0)
            free(carreira->ligacoes);
        for  (i = index; i < *_numCarreiras-1; i++) {
            (*_carreiras)[i] = (*_carreiras)[i+1];
        }
        free(carreira);
        *_carreiras = (Carreira**)realloc(*_carreiras, --(*_numCarreiras) * sizeof(Carreira*));
    }
    free(nomeCarreira);
}

void eliminaParagem(Paragem ***_paragens, int *_numParagens) {
    char *nomeParagem;
    int i, j, k, l;
    Paragem *paragem;
    Carreira *carreira;
    Ligacao *ligacao;
    if ((nomeParagem = (char*)malloc(2 * sizeof(char))) == NULL) {
        printf("No memory.\n");
        exit(1);
    }
    leEspacos();
    leNome(&nomeParagem);
    if ((paragem = encontraParagem(nomeParagem, _paragens, *_numParagens)) == NULL)
            printf("%s: no such stop.\n", nomeParagem);
    else {
        for (i = 0; i < *_numParagens; i++) {
            if ((*_paragens)[i] == paragem) {
                for (j = 0; j < (*_paragens)[i]->numCarreiras; j++) {
                    carreira = (*_paragens)[i]->carreiras[j];
                    for (k = 0; k < carreira->numLigacoes; k++) {
                        ligacao = carreira->ligacoes[k];
                        if (k == 0 && ligacao->origem == paragem) {
                            carreira->custoTotal -= ligacao->custo;
                            carreira->duracaoTotal -= ligacao->duracao;
                            free(ligacao);
                            for (l = k; l < carreira->numLigacoes-1; l++)
                                carreira->ligacoes[l] = carreira->ligacoes[l+1];
                            carreira->ligacoes = (Ligacao**)realloc(carreira->ligacoes, --carreira->numLigacoes * sizeof(Ligacao*));
                            k--;
                        }
                        else if (ligacao->destino == paragem) {
                            if (k == carreira->numLigacoes - 1) {
                                carreira->custoTotal -= ligacao->custo;
                                carreira->duracaoTotal -= ligacao->duracao;
                            }
                            else {
                                carreira->ligacoes[k+1]->custo += ligacao->custo;
                                carreira->ligacoes[k+1]->duracao += ligacao->duracao;
                                carreira->ligacoes[k+1]->origem = ligacao->origem;
                            }
                            free(ligacao);
                            for (l = k; l < carreira->numLigacoes-1; l++)
                                carreira->ligacoes[l] = carreira->ligacoes[l+1];
                            carreira->ligacoes = (Ligacao**)realloc(carreira->ligacoes, --carreira->numLigacoes * sizeof(Ligacao*));
                            k--;
                        }
                    }
                }
                free(paragem->nome);
                if (paragem->numCarreiras > 0)
                    free(paragem->carreiras);
                for (j = i; j < *_numParagens-1; j++)
                    (*_paragens)[j] = (*_paragens)[j+1];
                free(paragem);
                *_paragens = (Paragem**)realloc(*_paragens, --(*_numParagens) * sizeof(Paragem*));
                break;
            }
        }
    }
    free(nomeParagem);
}


void apagaTudo(Carreira ***_carreiras, Paragem ***_paragens, int *_numCarreiras, int *_numParagens) {
    int i;
    for (i = 0; i < *_numCarreiras; i++) {
        free((*_carreiras)[i]->nome);
        if ((*_carreiras)[i]->numLigacoes > 0)
            free((*_carreiras)[i]->ligacoes);
        free((*_carreiras)[i]);
    }
    if (*_numCarreiras > 0)
        free(*_carreiras);
    *_numCarreiras = 0;
    for (i = 0; i < *_numParagens; i++) {
        free((*_paragens)[i]->nome);
        if ((*_paragens)[i]->numCarreiras > 0)
            free((*_paragens)[i]->carreiras);
        free((*_paragens)[i]);
    }
    if (*_numParagens > 0)
        free(*_paragens);
    *_numParagens = 0;
}

/* Função MAIN */

int main() {
    int c;
    int _numParagens = 0;
    int _numCarreiras = 0;

    Paragem **_paragens = NULL;
    Carreira **_carreiras = NULL;

    do {
        c = getchar();
        switch(c) {
            case 'c':
                carreiras(&_carreiras, &_numCarreiras);
                break;
            case 'p':
                paragens(&_paragens, &_numParagens);
                break;
            case 'l':
                ligacoes(&_paragens, &_carreiras, _numCarreiras, _numParagens);
                break;
            case 'i':
                intersecoes(_paragens, _numParagens);
                break;
            case 'r':
                removeCarreira(&_carreiras, &_paragens, &_numCarreiras, _numParagens);
                break;
            case 'e':
                eliminaParagem(&_paragens, &_numParagens);
                break;
	        case 'q':
            case 'a':
                apagaTudo(&_carreiras, &_paragens, &_numCarreiras, &_numParagens);
                break;
            default:
	        /* Ignorar linhas em branco */
	        if (c == ' ' || c == '\t' || c == '\n') break;
        }
    } while (c != 'q');
    return 0;
}
