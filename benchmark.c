/* ============================================================================
 *  BENCHMARK comparativo: Fenwick Tree (O(log n)) x Vetor linear (O(n)).
 *
 *  Para cada tamanho n o programa mede, tirando a media de varias repeticoes:
 *     - construcao da BIT                  O(n)
 *     - n atualizacoes na BIT              O(n log n)
 *     - n consultas de prefixo na BIT      O(n log n)
 *     - n atualizacoes no vetor linear     O(n)         (referencia)
 *     - n consultas de prefixo lineares    O(n^2)       (referencia)
 *  e calcula o ganho de desempenho da BIT.
 *
 *  Ao final gera o arquivo "benchmark.csv" para plotagem de graficos.
 *
 *  Compilar:  gcc -O2 -Wall -o benchmark benchmark.c
 *  Executar:  ./benchmark
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOWBIT(i) ((i) & (-(i)))

/* ----------------------------------------------------------------------------
 *  Fenwick Tree (versao enxuta, so o necessario para o benchmark).
 * --------------------------------------------------------------------------*/
typedef struct {
    long long *bit;
    int        n;
} Fenwick;

static Fenwick *fenwick_criar(int n) {
    Fenwick *f = malloc(sizeof(Fenwick));
    f->n   = n;
    f->bit = calloc(n + 1, sizeof(long long));
    return f;
}

static void fenwick_destruir(Fenwick *f) {
    if (f) { free(f->bit); free(f); }
}

static void fenwick_atualizar(Fenwick *f, int i, long long delta) {
    for (; i <= f->n; i += LOWBIT(i))
        f->bit[i] += delta;
}

static long long fenwick_prefixo(Fenwick *f, int i) {
    long long soma = 0;
    for (; i > 0; i -= LOWBIT(i))
        soma += f->bit[i];
    return soma;
}

/* ----------------------------------------------------------------------------
 *  Utilitarios de medicao.
 * --------------------------------------------------------------------------*/

/* tempo em milissegundos entre dois instantes de clock() */
static double ms(clock_t a, clock_t b) {
    return 1000.0 * (b - a) / CLOCKS_PER_SEC;
}

/* resultado de uma rodada de benchmark para um dado n */
typedef struct {
    int    n;
    double t_constr;       /* construcao da BIT                */
    double t_upd_bit;      /* n atualizacoes na BIT            */
    double t_qry_bit;      /* n consultas na BIT               */
    double t_upd_lin;      /* n atualizacoes no vetor linear   */
    double t_qry_lin;      /* n consultas lineares             */
} Resultado;

/* ----------------------------------------------------------------------------
 *  Executa UMA rodada de benchmark para um tamanho n.
 *
 *  Para isolar cada operacao, geramos previamente os mesmos indices aleatorios
 *  e usamos os mesmos dados para a BIT e para o vetor linear (comparacao justa).
 *  A variavel 'sink' (volatile) impede que o compilador descarte os calculos.
 * --------------------------------------------------------------------------*/
static Resultado rodar(int n, int q) {
    Resultado r = {0};
    r.n = n;

    long long *valores = malloc((n + 1) * sizeof(long long));
    int       *indices = malloc(q       * sizeof(int));
    for (int i = 1; i <= n; i++) valores[i] = (rand() % 100) + 1;
    for (int i = 0; i < q; i++)  indices[i] = (rand() % n) + 1;

    volatile long long sink = 0;
    clock_t t0, t1;

    /* ---------- Fenwick Tree ---------- */
    Fenwick *f = fenwick_criar(n);

    t0 = clock();
    for (int i = 1; i <= n; i++) fenwick_atualizar(f, i, valores[i]);
    t1 = clock();
    r.t_constr = ms(t0, t1);

    t0 = clock();
    for (int i = 0; i < q; i++) fenwick_atualizar(f, indices[i], +1);
    t1 = clock();
    r.t_upd_bit = ms(t0, t1);

    t0 = clock();
    for (int i = 0; i < q; i++) sink += fenwick_prefixo(f, indices[i]);
    t1 = clock();
    r.t_qry_bit = ms(t0, t1);

    fenwick_destruir(f);

    /* ---------- Vetor linear (referencia) ---------- */
    long long *base = calloc(n + 1, sizeof(long long));
    for (int i = 1; i <= n; i++) base[i] = valores[i];

    t0 = clock();
    for (int i = 0; i < q; i++) base[indices[i]] += 1;   /* update O(1) */
    t1 = clock();
    r.t_upd_lin = ms(t0, t1);

    t0 = clock();
    for (int i = 0; i < q; i++) {
        long long s = 0;
        for (int j = 1; j <= indices[i]; j++) s += base[j];  /* O(n) */
        sink += s;
    }
    t1 = clock();
    r.t_qry_lin = ms(t0, t1);

    free(valores); free(indices); free(base);
    (void) sink;
    return r;
}

/* ----------------------------------------------------------------------------
 *  Executa 'reps' rodadas e retorna a MEDIA dos tempos.
 * --------------------------------------------------------------------------*/
static Resultado media(int n, int q, int reps) {
    Resultado acc = {0};
    acc.n = n;
    for (int k = 0; k < reps; k++) {
        Resultado r = rodar(n, q);
        acc.t_constr  += r.t_constr;
        acc.t_upd_bit += r.t_upd_bit;
        acc.t_qry_bit += r.t_qry_bit;
        acc.t_upd_lin += r.t_upd_lin;
        acc.t_qry_lin += r.t_qry_lin;
    }
    acc.t_constr  /= reps;
    acc.t_upd_bit /= reps;
    acc.t_qry_bit /= reps;
    acc.t_upd_lin /= reps;
    acc.t_qry_lin /= reps;
    return acc;
}

/* ============================================================================
 *  MAIN
 * ==========================================================================*/
int main(void) {
    srand(12345); /* semente fixa -> reproducao na apresentacao */

    /* Tamanhos exigidos (1.000 e 10.000) + tamanhos maiores para evidenciar
     * o crescimento assintotico. */
    int tamanhos[] = {1000, 10000, 50000, 100000};
    int qtd        = sizeof(tamanhos) / sizeof(tamanhos[0]);

    /* Repeticoes para tirar a media. Reduz para os n grandes (sao mais caros). */
    printf("\n");
    printf("===========================================================================\n");
    printf("  BENCHMARK - FENWICK TREE (BIT)  x  VETOR LINEAR\n");
    printf("  Tempos em milissegundos (media de varias repeticoes)\n");
    printf("===========================================================================\n");
    printf("%-9s | %-9s | %-10s | %-10s | %-11s | %-8s\n",
           "n", "constr", "upd BIT", "qry BIT", "qry LINEAR", "GANHO");
    printf("---------------------------------------------------------------------------\n");

    FILE *csv = fopen("benchmark.csv", "w");
    if (csv) {
        fprintf(csv, "n,construcao_bit_ms,update_bit_ms,query_bit_ms,"
                     "update_linear_ms,query_linear_ms,ganho_consulta\n");
    }

    for (int i = 0; i < qtd; i++) {
        int n    = tamanhos[i];
        int q    = n;                  /* numero de operacoes = n */
        int reps = (n <= 10000) ? 20 : 5;

        Resultado r = media(n, q, reps);
        double ganho = (r.t_qry_bit > 0) ? r.t_qry_lin / r.t_qry_bit : 0.0;

        printf("%-9d | %8.3f | %9.3f | %9.3f | %10.3f | %6.1fx\n",
               r.n, r.t_constr, r.t_upd_bit, r.t_qry_bit, r.t_qry_lin, ganho);

        if (csv) {
            fprintf(csv, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f\n",
                    r.n, r.t_constr, r.t_upd_bit, r.t_qry_bit,
                    r.t_upd_lin, r.t_qry_lin, ganho);
        }
    }

    printf("---------------------------------------------------------------------------\n");
    if (csv) {
        fclose(csv);
        printf("\nArquivo 'benchmark.csv' gerado (para plotar graficos nos slides).\n");
    }

    printf("\nObservacao: ao multiplicar n por 10, o tempo das consultas LINEARES\n");
    printf("cresce ~100x (comportamento O(n^2) no total), enquanto a BIT cresce de\n");
    printf("forma quase linear (O(n log n)). Por isso o GANHO aumenta com n.\n\n");

    return 0;
}
