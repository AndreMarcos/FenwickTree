# Fenwick Tree (Binary Indexed Tree)

Implementação em C de uma Árvore de Fenwick (Binary Indexed Tree), estrutura que
realiza **atualizações** e **consultas de soma acumulada** em tempo **O(log n)**.

---

## Como compilar e executar

Precisa apenas do `gcc`.

### Programa principal (demonstração)
```bash
gcc -O2 -Wall -o fenwick fenwick.c
./fenwick
```
Menu interativo:
- `1` — Demonstração manual (exemplo do relatório, n = 8)
- `2` — Experimento com 1.000 elementos
- `3` — Experimento com 10.000 elementos
- `4` — Roda os dois experimentos
- `0` — Sair

### Benchmark comparativo (BIT × vetor linear)
```bash
gcc -O2 -Wall -o benchmark benchmark.c
./benchmark
```
Testa n = 1.000, 10.000, 50.000 e 100.000, tira a média de várias repetições
e gera o arquivo `benchmark.csv`.

### Gerar as imagens (opcional)
Requer Python 3 + matplotlib (já há um venv na pasta):
```bash
.venv/bin/python gerar_imagens.py
```

---

## Operações implementadas

| Operação | Função | Complexidade |
|----------|--------|:------------:|
| Construção a partir de vetor | `fenwick_construir` | O(n) |
| Atualização (inserção/remoção) | `fenwick_atualizar` | O(log n) |
| Soma de prefixo `[1..i]` | `fenwick_prefixo` | O(log n) |
| Soma de intervalo `[l..r]` | `fenwick_intervalo` | O(log n) |
| Valor de uma posição | `fenwick_valor` | O(log n) |
| Busca do k-ésimo (lower_bound) | `fenwick_buscar_kesimo` | O(log n) |

> A "remoção" na BIT é uma atualização com valor negativo: `fenwick_atualizar(f, i, -v)`.

---

## Resultados do benchmark

| n | Consulta BIT (O(log n)) | Consulta Linear (O(n)) | Ganho |
|---|:---:|:---:|:---:|
| 1.000 | ~0,01 ms | ~0,24 ms | ~21× |
| 10.000 | ~0,14 ms | ~27 ms | ~196× |
| 50.000 | ~0,74 ms | ~704 ms | ~952× |
| 100.000 | ~1,7 ms | ~2.902 ms | ~1.700× |

*(Os números variam conforme a máquina; o importante é o padrão: o ganho cresce com n.)*
