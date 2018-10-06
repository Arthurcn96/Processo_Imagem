# Processo_Imagem
Programa criado na disciplina de Processamento paralelo ministrada pelo professor [Oberlan C. Romão](https://www.ime.usp.br/~oberlan/). O objetivo do programa é ler, aplicar alguns filtros (em paralelo) e gravar imagens digitais.

As imagens usadas no EP estão codificadas no formato PNM, que é um formato bitmap, pixel a pixel, sem nenhum tipo de compressão. Imagens desse tipo podem ser lidas e gravadas por alguns editores de imagens, como o GIMP.

O código deve ser compilado usando 
- `gcc PNM.c -o programa -lm`

Para rodar basta utilizar:
- `./programa imagem.pnm`

Onde a imagem deve estar no formato pnm.

## Exemplo
#### Imagem original
![Imagem Original](https://github.com/Arthurcn96/Processo_Imagem/blob/master/Img/wall.jpg)

#### Imagem filtrada
![Imagem Filtrada](https://github.com/Arthurcn96/Processo_Imagem/blob/master/Img/filtrado.png)

