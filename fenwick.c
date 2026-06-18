/* ============================================================================
 *  Implementacao de uma Arvore de Fenwick (BIT) em linguagem C.
 *
 *  Operacoes suportadas:
 *    - construcao a partir de um vetor          O(n)
 *    - atualizacao em ponto (insercao/remocao)  O(log n)
 *    - consulta de soma de prefixo              O(log n)
 *    - consulta de soma em intervalo [l, r]     O(log n)
 *    - consulta do valor de uma posicao         O(log n)
 *    - busca do k-esimo (lower_bound / busca)   O(log n)
 *
 *  Compilar:  gcc -O2 -Wall -o fenwick fenwick.c
 *  Executar:  ./fenwick
 * ----------------------------------------------------------------------------
 *
 *  ESTRUTURA DE COBERTURA (exemplo com n = 8)
 *  ------------------------------------------
 *  Cada bit[i] guarda a soma do bloco [ i - lowbit(i) + 1 .. i ].
 *  O tamanho do bloco e lowbit(i) = i & -i:
 *
 *     i   binario   lowbit   bloco coberto por bit[i]
 *     1    0001       1       [1..1]
 *     2    0010       2       [1..2]
 *     3    0011       1       [3..3]
 *     4    0100       4       [1..4]
 *     5    0101       1       [5..5]
 *     6    0110       2       [5..6]
 *     7    0111       1       [7..7]
 *     8    1000       8       [1..8]
 *
 *  Arvore implicita (cada no soma a faixa indicada):
 *
 *                          bit[8]  = [1..8]
 *              _______________/        \_______________
 *         bit[4] = [1..4]                       bit[6] = [5..6]
 *          /          \                          /          \
 *     bit[2]=[1..2]  bit[3]=[3..3]        bit[5]=[5..5]   bit[7]=[7..7]
 *        /
 *   bit[1]=[1..1]
 *
 *  CONSULTA  prefixo(i):  desce  i -> i - lowbit(i)  ate 0  (soma os blocos).
 *     Ex.: prefixo(7) = bit[7] + bit[6] + bit[4]   (7 -> 6 -> 4 -> 0)
 *                     = [7..7] + [5..6] + [1..4]
 *
 *  ATUALIZA  atualizar(i): sobe  i -> i + lowbit(i)  ate n  (corrige os blocos).
 *     Ex.: atualizar(3,..) toca bit[3] -> bit[4] -> bit[8]  (3 -> 4 -> 8)
 *
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ----------------------------------------------------------------------------
 *  Estrutura da Fenwick Tree.
 *
 *  Usamos indexacao a partir de 1 (1-indexed), pois a aritmetica de bits
 *  (i & -i) so funciona corretamente para indices >= 1.
 *
 *  bit[]  -> vetor que armazena as somas parciais.
 *  n      -> numero de posicoes logicas do vetor original.
 * --------------------------------------------------------------------------*/
typedef struct {
    long long *bit;   /* vetor de somas parciais (1-indexed) */
    int        n;     /* tamanho logico                       */
} Fenwick;

/* lowbit(i): isola o bit menos significativo (LSB) de i.
 * Ex.: i = 12 (1100b)  ->  i & -i = 4 (100b).
 * Esse valor indica o "tamanho do intervalo" que a posicao i cobre.
 *
 * POR QUE 'i & -i' FUNCIONA (complemento de dois):
 *   Em complemento de dois, -i = (~i) + 1. Inverter todos os bits e somar 1
 *   faz com que todos os bits ABAIXO do menor bit 1 de i virem 0->1, o menor
 *   bit 1 permaneca 1, e os bits ACIMA fiquem invertidos. Ao fazer i & -i,
 *   so o menor bit 1 coincide nos dois, sobrando exatamente ele.
 *   Ex.: i = 12 = 0000 1100
 *       -i = 1111 0100   (~i + 1)
 *   i & -i = 0000 0100 = 4  -> o bloco que a posicao 12 cobre tem tamanho 4. */
#define LOWBIT(i) ((i) & (-(i)))

/* ----------------------------------------------------------------------------
 *  fenwick_criar: aloca e zera uma Fenwick Tree de tamanho n.
 * --------------------------------------------------------------------------*/
Fenwick *fenwick_criar(int n) {
    Fenwick *f = (Fenwick *) malloc(sizeof(Fenwick));
    if (!f) { fprintf(stderr, "Erro de alocacao\n"); exit(1); }
    f->n   = n;
    /* n+1 posicoes porque trabalhamos de 1 ate n (a posicao 0 nao e usada). */
    f->bit = (long long *) calloc(n + 1, sizeof(long long));
    if (!f->bit) { fprintf(stderr, "Erro de alocacao\n"); exit(1); }
    return f;
}

void fenwick_destruir(Fenwick *f) {
    if (f) { free(f->bit); free(f); }
}

/* ----------------------------------------------------------------------------
 *  fenwick_atualizar: soma 'delta' a posicao 'i' do vetor original.
 *
 *  - Para INSERIR/ADICIONAR um valor v na posicao i:  delta = +v
 *  - Para REMOVER o valor v da posicao i:              delta = -v
 *
 *  A propagacao sobe pelos indices responsaveis por i somando lowbit(i).
 *  Complexidade: O(log n).
 * --------------------------------------------------------------------------*/
void fenwick_atualizar(Fenwick *f, int i, long long delta) {
    for (; i <= f->n; i += LOWBIT(i))
        f->bit[i] += delta;
}

/* ----------------------------------------------------------------------------
 *  fenwick_prefixo: retorna a soma acumulada das posicoes [1 .. i].
 *
 *  Descemos pelos indices subtraindo lowbit(i), acumulando as somas parciais.
 *  Complexidade: O(log n).
 * --------------------------------------------------------------------------*/
long long fenwick_prefixo(Fenwick *f, int i) {
    long long soma = 0;
    for (; i > 0; i -= LOWBIT(i))
        soma += f->bit[i];
    return soma;
}

/* ----------------------------------------------------------------------------
 *  fenwick_intervalo: retorna a soma das posicoes [l .. r].
 *  Usa a propriedade: soma(l,r) = prefixo(r) - prefixo(l-1).
 *  Complexidade: O(log n).
 * --------------------------------------------------------------------------*/
long long fenwick_intervalo(Fenwick *f, int l, int r) {
    return fenwick_prefixo(f, r) - fenwick_prefixo(f, l - 1);
}

/* ----------------------------------------------------------------------------
 *  fenwick_valor: retorna o valor armazenado em uma unica posicao i.
 *  E o "ponto" do vetor original: valor(i) = prefixo(i) - prefixo(i-1).
 *  Complexidade: O(log n).
 * --------------------------------------------------------------------------*/
long long fenwick_valor(Fenwick *f, int i) {
    return fenwick_intervalo(f, i, i);
}

/* ----------------------------------------------------------------------------
 *  fenwick_construir: monta a BIT a partir de um vetor v[1..n] em O(n).
 *
 *  Em vez de chamar atualizar() n vezes (custaria O(n log n)), processamos
 *  os indices em ordem crescente. Quando chegamos em i, bit[i] ja contem a
 *  soma completa do seu bloco; entao "empurramos" esse total acumulado para
 *  o indice pai (i + lowbit(i)), que e o proximo bloco que contem i. Como
 *  cada posicao contribui para apenas UM pai, sao n operacoes -> O(n). */
void fenwick_construir(Fenwick *f, long long *v) {
    int i, pai;
    for (i = 1; i <= f->n; i++) {
        f->bit[i] += v[i];
        pai = i + LOWBIT(i);
        if (pai <= f->n)
            f->bit[pai] += f->bit[i];
    }
}

/* ----------------------------------------------------------------------------
 *  fenwick_buscar_kesimo: dado que o vetor guarda frequencias (valores >= 0),
 *  encontra o MENOR indice i tal que prefixo(i) >= k  (lower_bound).
 *
 *  Esta e a operacao de "busca" tipica da BIT: localizar em qual posicao
 *  cai o k-esimo elemento de uma distribuicao de frequencias acumuladas.
 *  Usa "binary lifting" sobre os bits -> O(log n).
 *  Retorna -1 se nao existir tal indice (k maior que a soma total).
 *
 *  IDEIA (por que funciona):
 *    Em vez de descer no vetor 1 a 1, tentamos "saltar" em blocos de tamanhos
 *    que sao potencias de 2, do maior para o menor (n, n/2, n/4, ... 1).
 *    Lembre que bit[pos + passo] guarda a soma EXATA do bloco logo a frente
 *    quando 'passo' e uma potencia de 2 alinhada. Entao, a cada salto:
 *      - se a soma desse bloco ainda for MENOR que k, o k-esimo esta depois
 *        dele: aceitamos o salto (pos += passo) e descontamos o que ja passou
 *        (k -= soma do bloco);
 *      - senao, o salto e grande demais: nao saltamos e tentamos um menor.
 *    No fim, 'pos' para no ultimo indice cujo prefixo ainda e < k; logo o
 *    k-esimo elemento esta na posicao seguinte (pos + 1).
 * --------------------------------------------------------------------------*/
int fenwick_buscar_kesimo(Fenwick *f, long long k) {
    int pos = 0;                 /* indice acumulado ate agora                */

    /* maior potencia de 2 que ainda cabe em n (define o 1o tamanho de salto) */
    int log_max = 1;
    while ((1 << (log_max + 1)) <= f->n) log_max++;

    /* tenta saltos cada vez menores: 2^log_max, ..., 4, 2, 1 */
    for (int passo = (1 << log_max); passo > 0; passo >>= 1) {
        if (pos + passo <= f->n && f->bit[pos + passo] < k) {
            pos += passo;            /* aceita o salto                        */
            k   -= f->bit[pos];      /* desconta a soma do bloco percorrido   */
        }
    }
    /* 'pos' = ultimo indice com prefixo < k; o k-esimo esta logo apos ele. */
    return (pos + 1 <= f->n) ? pos + 1 : -1;
}

/* ============================================================================
 *  DEMONSTRACAO MANUAL  (uso interativo durante a apresentacao)
 * ==========================================================================*/
void demonstracao_manual(void) {
    /* Exemplo do relatorio: vetor de 8 posicoes. */
    long long v[9] = {0, 3, 2, -1, 6, 5, 4, -3, 3}; /* v[1..8] */
    int n = 8;

    Fenwick *f = fenwick_criar(n);
    fenwick_construir(f, v);

    printf("\n=== DEMONSTRACAO MANUAL (n = %d) ===\n", n);
    printf("Vetor original (1-indexed): ");
    for (int i = 1; i <= n; i++) printf("%lld ", v[i]);
    printf("\n");

    printf("\nSoma de prefixo [1..5]  = %lld (esperado 15)\n",
           fenwick_prefixo(f, 5));
    printf("Soma do intervalo [3..6] = %lld (esperado 14)\n",
           fenwick_intervalo(f, 3, 6));
    printf("Valor da posicao 4       = %lld (esperado 6)\n",
           fenwick_valor(f, 4));

    printf("\n-> Atualizando: soma +10 na posicao 4 (insercao)\n");
    fenwick_atualizar(f, 4, +10);
    printf("Soma de prefixo [1..5]  = %lld (esperado 25)\n",
           fenwick_prefixo(f, 5));

    printf("\n-> Removendo o valor 10 da posicao 4 (remocao, delta = -10)\n");
    fenwick_atualizar(f, 4, -10);
    printf("Soma de prefixo [1..5]  = %lld (de volta a 15)\n",
           fenwick_prefixo(f, 5));

    /* Exemplo de busca por frequencias. */
    printf("\n--- Busca do k-esimo (vetor de frequencias) ---\n");
    Fenwick *fr = fenwick_criar(10);
    /* posicoes 2,5,5,7,9 marcadas */
    fenwick_atualizar(fr, 2, 1);
    fenwick_atualizar(fr, 5, 2);
    fenwick_atualizar(fr, 7, 1);
    fenwick_atualizar(fr, 9, 1);
    printf("Frequencias acumuladas; busca do 3o elemento -> posicao %d\n",
           fenwick_buscar_kesimo(fr, 3));
    fenwick_destruir(fr);

    fenwick_destruir(f);
}

/* ============================================================================
 *  EXPERIMENTOS  (criterio 5.3: 1.000 e 10.000 elementos, medindo tempo)
 *
 *  Comparamos a Fenwick Tree O(log n) com a abordagem linear ingenua O(n)
 *  para consultas de soma de prefixo, evidenciando o impacto do crescimento
 *  dos dados.
 * ==========================================================================*/

/* gera um inteiro pseudo-aleatorio entre 1 e 100 */
static int valor_aleatorio(void) { return (rand() % 100) + 1; }

void experimento(int n) {
    printf("\n============================================================\n");
    printf("  EXPERIMENTO COM n = %d ELEMENTOS\n", n);
    printf("============================================================\n");

    /* ---- preparacao dos dados ---- */
    long long *v       = (long long *) malloc((n + 1) * sizeof(long long));
    long long *prefixo = (long long *) malloc((n + 1) * sizeof(long long));
    int       *idx     = (int *)       malloc((n)     * sizeof(int));
    int        q       = n;  /* numero de operacoes de consulta */

    for (int i = 1; i <= n; i++) v[i] = valor_aleatorio();
    for (int i = 0; i < q; i++)  idx[i] = (rand() % n) + 1;

    Fenwick *f = fenwick_criar(n);

    clock_t t0, t1;

    /* ---- 1) construcao da BIT ---- */
    t0 = clock();
    fenwick_construir(f, v);
    t1 = clock();
    printf("Construcao da BIT (O(n))           : %.3f ms\n",
           1000.0 * (t1 - t0) / CLOCKS_PER_SEC);

    /* ---- 2) q atualizacoes na BIT  O(q log n) ---- */
    t0 = clock();
    for (int i = 0; i < q; i++)
        fenwick_atualizar(f, idx[i], +1);
    t1 = clock();
    printf("%d atualizacoes na BIT  (O(log n))  : %.3f ms\n", q,
           1000.0 * (t1 - t0) / CLOCKS_PER_SEC);

    /* ---- 3) q consultas de prefixo na BIT  O(q log n) ---- */
    volatile long long acc = 0;  /* volatile evita que o compilador descarte */
    t0 = clock();
    for (int i = 0; i < q; i++)
        acc += fenwick_prefixo(f, idx[i]);
    t1 = clock();
    double tempo_bit = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    printf("%d consultas na BIT     (O(log n))  : %.3f ms\n", q, tempo_bit);

    /* ---- 4) q consultas de prefixo LINEAR (ingenuo) O(q n) ---- */
    /* recalcula a soma percorrendo o vetor a cada consulta */
    long long *base = (long long *) calloc(n + 1, sizeof(long long));
    for (int i = 1; i <= n; i++) base[i] = v[i];
    for (int i = 0; i < q; i++)  base[idx[i]] += 1;

    acc = 0;
    t0 = clock();
    for (int i = 0; i < q; i++) {
        long long s = 0;
        for (int j = 1; j <= idx[i]; j++) s += base[j];
        acc += s;
    }
    t1 = clock();
    double tempo_linear = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    printf("%d consultas LINEARES   (O(n))      : %.3f ms\n", q, tempo_linear);

    if (tempo_bit > 0)
        printf("\n  => A BIT foi cerca de %.1fx mais rapida nas consultas.\n",
               tempo_linear / tempo_bit);

    (void) prefixo; (void) acc;
    free(v); free(prefixo); free(idx); free(base);
    fenwick_destruir(f);
}

/* ============================================================================
 *  MENU PRINCIPAL
 * ==========================================================================*/
int main(void) {
    srand(12345); /* semente fixa -> resultados reproduziveis na apresentacao */

    int opcao;
    do {
        printf("\n=============== FENWICK TREE (BIT) - T2 ===============\n");
        printf(" 1 - Demonstracao manual (exemplo do relatorio)\n");
        printf(" 2 - Experimento com    1.000 elementos\n");
        printf(" 3 - Experimento com   10.000 elementos\n");
        printf(" 4 - Rodar os dois experimentos (1.000 e 10.000)\n");
        printf(" 0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) break;

        switch (opcao) {
            case 1: demonstracao_manual();        break;
            case 2: experimento(1000);            break;
            case 3: experimento(10000);           break;
            case 4: experimento(1000);
                    experimento(10000);           break;
            case 0: printf("Encerrando...\n");    break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
