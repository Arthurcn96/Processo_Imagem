#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>


typedef struct {
	int altura, largura, threshold;
	unsigned char ***pixel;
} Imagem;

unsigned char*** aloca(int n, int m);

double MyClock() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

// Aloca memoria para uma matriz de dimencoes (N x M x 3)
unsigned char*** aloca(int n, int m) {
    int i, j;
    unsigned char ***M;
    M = (unsigned char***) malloc(n * sizeof(unsigned char**));
    for(i = 0; i < n; i++){
        M[i] = (unsigned char**) malloc(m * sizeof(unsigned char*));
        for(j = 0; j < m; j++){
        	M[i][j] = (unsigned char*) malloc(3 * sizeof(unsigned char));
        	M[i][j][0] = 0; //Vermelho
        	M[i][j][1] = 0; //Verde
        	M[i][j][2] = 0; //Azul
        }
    }
    if (!M) {
		fprintf(stderr, "Erro na alocacao da memoria\n");
		exit(1);
	}

    return M;
}

Imagem *readPNM(const char *nomeArquivo) {
	char buff[16];
	Imagem *img;
	FILE *fp;
	int c;
    fp = fopen(nomeArquivo, "r");

    if (fp == NULL) {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }
    if (!fgets(buff, sizeof(buff), fp)) {
		perror("stdin");
		exit(1);
	}

	if (buff[0] != 'P' || buff[1] != '3') {
		fprintf(stderr, "Formato da imagem invalido (deve ser 'P3')\n");
		exit(1);
	}

	img = (Imagem *) malloc(sizeof(Imagem));
	if (!img) {
		fprintf(stderr, "Erro na alocacao da memoria\n");
		exit(1);
	}

	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n')
			;
		c = getc(fp);
	}

	ungetc(c, fp);
	if (fscanf(fp, "%d %d", &img->largura, &img->altura) != 2) {
		fprintf(stderr, "Tamanho da imagem invalido\n");
		exit(1);
	}

	if (fscanf(fp, "%d", &img->threshold) != 1) {
		fprintf(stderr, "Componente rgb invalido\n");
		exit(1);
	}

	while (fgetc(fp) != '\n')
		;

	img->pixel = aloca(img->altura+2, img->largura+2);

	if (!img) {
		fprintf(stderr, "Erro na alocacao da memoria\n");
		exit(1);
	}
	int r, g, b;
	for(int i = 1; i <= img->altura; i++){
        for(int j = 1; j <= img->largura; j++){
            fscanf(fp, "%d %d %d", &r, &g, &b);
            img->pixel[i][j][0] = (unsigned char)r;
            img->pixel[i][j][1] = (unsigned char)g;
            img->pixel[i][j][2] = (unsigned char)b;
            //printf("%d %d %d ", r, g, b);
        }
        //printf("\n");
    }

	return img;
}



void salvaPNM(Imagem *img, char nome[20]){
	FILE *arquivo;
	int c, rgb_comp_color;
    arquivo = fopen(nome, "w");
    fprintf(arquivo, "P3\n");
    fprintf(arquivo, "%d %d %d\n", img->largura, img->altura, img->threshold);
    for(int i = 1; i <= img->altura; i++)
      for(int j = 1; j <= img->largura; j++)
      	for(int k = 0; k < 3; k++)
      		fprintf(arquivo, "%d\n", (int)img->pixel[i][j][k]);
}

//FILTROS #####################################################################
void escurece(Imagem *imagem, int fator){
	for(int i = 1; i <= imagem->altura; i++)
      for(int j = 1; j <= imagem->largura; j++)
      	for(int k = 0; k < 3; k++) {
	        int valor = (int)imagem->pixel[i][j][k];   //pega o valor do píxel
	        valor -= fator;              //escurece o píxel
	        if (valor<0)                 //se der negativo, deixa preto
	        	valor = 0;
            valor=(valor<0?0:valor);
	        imagem->pixel[i][j][k] = (unsigned char)valor;   //modifica o píxel
      	}

salvaPNM(imagem,"escuro.pnm");
}

void sobel(Imagem *sobel,Imagem *imagem){
	int aux1,aux2;
	unsigned char aux;
	for(int i=2;i<sobel->altura-2;i++)
	 for(int j=2;j<sobel->largura-2;j++)
	    for (int k=0; k<3; k++) {


				aux1=1*(int)sobel->pixel[i-1][j-1][k] + 2*(int)sobel->pixel[i-1][j][k] + 1*(int)sobel->pixel[i-1][j+1][k]
             + (-1)*(int)sobel->pixel[i+1][j-1][k] + (-2)*(int)sobel->pixel[i+1][j][k] + (-1)*(int)sobel->pixel[i+1][j+1][k];

				aux1 = aux1<0?0:aux1;
				aux1 = aux1>255?255:aux1;

        aux2=1*(int)sobel->pixel[i-1][j-1][k] + (-1)*(int)sobel->pixel[i-1][j+1][k] + 2*(int)sobel->pixel[i][j-1][k]
             + (-2)*(int)sobel->pixel[i][j+1][k] + 1*(int)sobel->pixel[i+1][j-1][k] + (-1)*(int)sobel->pixel[i+1][j+1][k];

				aux2 = aux2<0?0:aux2;
 				aux2 = aux2>255?255:aux2;

							imagem->pixel[i][j][k]=(unsigned char)(sqrt(aux1*aux1 + aux2*aux2)); //Modifica o pixel
  }
	salvaPNM(imagem,"sobel.pnm");
}

void realcar(Imagem *realcar,Imagem *imagem){
	int aux1,aux2;
	for(int i=1;i<realcar->altura-1;i++)
	 for(int j=1;j<realcar->largura-1;j++)
	    for (int k=0; k<3; k++) {

				aux1=0*(int)realcar->pixel[i-1][j-1][k] + (-1)*(int)realcar->pixel[i-1][j][k] +   0*(int)realcar->pixel[i-1][j+1][k]
						+(-1)*(int)realcar->pixel[i][j-1][k] +    (5)*(int)realcar->pixel[i][j][k] +    (-1)*(int)realcar->pixel[i][j][k]
          +(0)*(int)realcar->pixel[i+1][j-1][k] + (-1)*(int)realcar->pixel[i+1][j][k] + (0)*(int)realcar->pixel[i+1][j+1][k];


		 aux1 = aux1*2;
		 aux1 = aux1<0?0:aux1;
		 aux1 = aux1>255?255:aux1;

        imagem->pixel[i][j][k]=(unsigned char)(sqrt(aux1*aux1)); //Modifica o pixel
  }
	salvaPNM(imagem,"realcar.pnm");
}

void bordas(Imagem *bordas,Imagem *imagem){
	int aux1,aux2;
	for(int i=1;i<bordas->altura-1;i++)
	 for(int j=1;j<bordas->largura-1;j++)
	    for (int k=0; k<3; k++) {

				aux1=(-1)*(int)bordas->pixel[i-1][j][k]
						+(-1)*(int)bordas->pixel[i][j-1][k] +    (4)*(int)bordas->pixel[i][j][k] +    (-1)*(int)bordas->pixel[i][j][k]
          +(-1)*(int)bordas->pixel[i+1][j][k];

		 aux1 = aux1<0?0:aux1;
		 aux1 = aux1>255?255:aux1;

        imagem->pixel[i][j][k]=(unsigned char)(sqrt(aux1*aux1)); //Modifica o pixel
  }
	salvaPNM(imagem,"bordas.pnm");
}

void meuFiltro(Imagem *meufiltro,Imagem *imagem){//NITIDEz
	int aux1,aux2;
	for(int i=1;i<meufiltro->altura-1;i++)
	 for(int j=1;j<meufiltro->largura-1;j++)
	    for (int k=0; k<3; k++) {

				aux1=(-2)*(int)meufiltro->pixel[i-1][j-1][k] + (-1)*(int)meufiltro->pixel[i-1][j][k] +   (-2)*(int)meufiltro->pixel[i-1][j+1][k]
						+(-1)*(int)meufiltro->pixel[i][j-1][k] +    (12)*(int)meufiltro->pixel[i][j][k] +    (-1)*(int)meufiltro->pixel[i][j][k]
          +(-2)*(int)meufiltro->pixel[i+1][j-1][k] + (-1)*(int)meufiltro->pixel[i+1][j][k] + (-1)*(int)meufiltro->pixel[i+1][j+1][k];

		 aux1 = aux1<0?0:aux1;
		 aux1 = aux1>255?255:aux1;

        imagem->pixel[i][j][k]=(unsigned char)(sqrt(aux1*aux1)); //Modifica o pixel
  }
	salvaPNM(imagem,"meufiltro.pnm");
}
int main(int argc, char *argv[]) {

	if (argc != 2) {
        fprintf(stderr, "use: prog --help\n");
        exit(1);
    }

		Imagem *imagem = readPNM(argv[1]);
		Imagem *copia = readPNM(argv[1]);


	//INICIO DO FORK()##########################################################

					// operação



						double inicio = MyClock();

					int pid = fork();

					   if (pid < 0) { /* ocorreu erro na execução do Fork */
					        fprintf(stderr, "fork failed\n");
					        exit(1);
					   }
					   else if (pid == 0) { /* processo filho */
					       int pidf = fork();
					       if (pidf < 0) {
					            fprintf(stderr, "fork failed\n");
					            exit(1);
					       }
					       else if(pidf == 0){
					            // operação
											bordas(imagem,copia);
					       }
					       else{
					            // operação
											sobel(imagem,copia);
					       }
					   }
					   else { /* processo pai */
					       int pidp = fork();
					       if (pidp < 0) {
					            fprintf(stderr, "fork failed\n");
					            exit(1);
					       }
					       else if(pidp == 0){
					            // operação
											realcar(imagem,copia);
					       }
					       else{
					            // operação
							 				meuFiltro(imagem,copia);
					       }
					       wait(NULL); /* pai espera o término dos filhos */
					       if(pidp != 0){
					            double tempoPar = (MyClock()-inicio)/CLOCKS_PER_SEC;
					            printf(" %.5lf+", tempoPar);
					       }
					   }
	free(imagem);
	free(copia);

	return 0;
}
