#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	float temperatura;
	float humidade;
	float luz;
	float tensao;
	float infraVerm;
} sensores;

typedef struct {
	int ar_condicionado; // Ligado aquecer: 1 ; Ligado arrefecer: -1 ; Desligado: 0;
	int humidificador; // Ligado humidificar: 1 ; Ligado desumidificar: -1; Desligado: 0;
	int persianas; // Fechar: 0 ; Abrir: 1;
	int luzes; //Desligar: 0 ; Ligar: 1;
} atuadores;

sensores sens[6];
atuadores atua[6];
volatile  char matrix[1000];

 
//Funcoes para calcular valores reais das variaveis
float calcularT(int rawTemp){
	float tempAtual;
	tempAtual = (float)(-39.6 + (0.01 * (float)rawTemp));
	
	//printf("Temperatura: %.2f C\n", tempAtual);
	
	return tempAtual;
}
float calcularVL(int rawViL){
	float vlatual;
	vlatual = (float)(0.625*1000000*((float)rawViL/4096)*(1.5/100000)*1000);
	
	//printf("Luz Atual: %.2f lux\n", vlatual);
	
	return vlatual;
}
float calcularH(int rawH, float temp){
	float Hatual, Hrel;
	
	Hrel =  (float)(-2.0468+(0.0367*((float)rawH))+(-1.5955*0.000001*((float)rawH)*((float)rawH)));
	
	Hatual = (float)((temp - 25)*(0.01 + (0.00008*(float)rawH))+Hrel);
	
	//printf("Humidade: %.2f %% \n", Hatual);
	
	return Hatual;
}
float calcularV(int tensao_decimal){
	float v_atual;
	v_atual =  (float)(((float)tensao_decimal/4096)*1.5*2);
	
	//printf("Tensao: %.2f V\n", v_atual);
	
	return v_atual;
}
float calcularIL(int infravermelho_decimal){
	float IL_atual;
	IL_atual = (float)(0.769*100000*((float)infravermelho_decimal/4096)*(1.5/100000)*1000);
	
	//printf("Luz Infravermelha: %.2f lux\n", IL_atual);
	
	return IL_atual;
}



//Comparacao entre os valores de refencia e os valores atuais lidos 			
																				
void tratamento_temperatura(int* temp_ref, float temp_atual, int divisao,int *atuador){
	
	
	if (temp_atual > temp_ref[divisao])
	{		*atuador = -1;
			strcat(matrix,"[255,0,0],"); //vermelho sensor
	}
	
	 if (temp_atual == temp_ref[divisao])
		{	*atuador = 0;
			strcat(matrix,"[0,255,0],");// verde sensor
		}
	
	if (temp_atual < temp_ref[divisao])
		{	*atuador = 1;
			strcat(matrix,"[0,0,255],"); //azul sensor	
		}
	
}
void tratamento_humidade(int *H_ref, float hum_atual, int divisao,int *atuador){
	
	
	if(hum_atual > H_ref[divisao])
	{	*atuador = -1;
		strcat(matrix,"[255,0,0],"); //vermelho sensor
	}	
	
	if (hum_atual < H_ref[divisao])
	{	*atuador = 1;
		strcat(matrix,"[0,0,255],"); //azul sensor
	}	
	
	 if (hum_atual == H_ref[divisao])
	{	*atuador = 0;
	    strcat(matrix,"[0,255,0],");// verde sensor
		
	}	
	
}
void tratamento_luz(int* VL_ref, float Luz_atual, int divisao,int *atuador){
	
	
	if( Luz_atual > VL_ref[divisao])
	{	*atuador = -1;
		strcat(matrix,"[255,0,0],"); //vermelho sensor
	}	
	
	 if( Luz_atual < VL_ref[divisao])
	{	*atuador = 1;
		strcat(matrix,"[0,0,255],"); //azul sensor
	}	
	
	if( Luz_atual == VL_ref[divisao])
	{	*atuador = 0;
	    strcat(matrix,"[0,255,0],");// verde sensor
		
	}	
	
}

//Ler informaçao dos sensores e converter para decimal
void Ler_parametros(char *str, int *mID,int*RawH,int*RawVL,int*RawT, int* RawV, int* RawIL){
    
    int i;
	char moteID[10]=" ",hum[10]=" ",temp[10]=" ",light[10]=" ", tensao[10]=" ", infraverm[10]=" ";
    char * pch;
    pch = strtok (str," ");
    i=1;
	
	while(pch!=NULL){	

		if(i>5 && i<8){  //ler moteiD e conversao para decimal
			strcat(moteID,pch);
			*mID=(int)strtol(moteID,NULL,16);
			
		}
			
		if(i>12 && i<15){ // ler visible light e conversao para decimal
			
			strcat(light,pch);
			*RawVL=(int)strtol(light,NULL,16);
			
		
		}
			
		if(i>16 && i<19){ //ler Temperatura e conversao para decimal
			
			strcat(temp,pch);
			*RawT=(int)strtol(temp,NULL,16);
			
		}
			
		if(i>18 && i<21){ //ler humidade e conversao para decimal
			
			strcat(hum,pch);
			*RawH=(int)strtol(hum,NULL,16);
			
		}
		if(i>10 && i<13){ //ler tensao e conversao para decimal
			
			strcat(tensao,pch);
			*RawV=(int)strtol(tensao,NULL,16);
		
		}	
		if(i>14 && i<17){ //ler radiçao infravermelha e conversao para decimal
			
			strcat(infraverm,pch);
			*RawIL=(int)strtol(infraverm,NULL,16);
		}		
	
	//	printf("%s\n",pch);
		pch = strtok(NULL," ");
		i++;
}		
}	


int main()
{
	char leitura[100];
	FILE *dados,*matrix_file ;
	int  RawT, RawH, RawVL, mID, RawV, RawIL,atuador;
	
	
	int i;
	
	//Vetores de refencia, por enquanto estao definidos no codigo  // REVER!!!!!!!
	int Tref[6]= {0, 24 , 25, 25, 23, 26};
	int Href[6] = {0,30,35,40,45,50};
	int VLref[6] = {0, 1000, 1250, 1500, 1750, 2000};
	
	
	float Tp, Hp, VLp, Vp, ILp;
	
	dados = fopen("Leituras.txt", "rt");
	//Temos a informacao do simulador referente a uma linha de codigo
	
	
	//Executa enquanto le valores do ficheiro
	while(fgets(leitura, sizeof(leitura), dados) != NULL){
	
	
	//Leitura e conversao para decimal dos dados provenientes do sensor
	Ler_parametros(leitura, &mID,&RawH,&RawVL,&RawT, &RawV, &RawIL);
	
	
	
	//Calcular o valor real de cada variavel, atraves de funcoes pre-definidas. Retorna float
	VLp = calcularVL(RawVL);
	Tp = calcularT(RawT);
	Hp = calcularH(RawH, Tp);
	Vp = calcularV(RawV);
	ILp = calcularIL(RawIL);

	
	//Atribuicao dos valores à posicao da estrutura Sensores
	sens[mID].temperatura = Tp;
	sens[mID].humidade = Hp;
	sens[mID].luz = VLp;
	sens[mID].infraVerm = ILp;
	sens[mID].tensao = Vp;
	
	if(mID==1)
		strcat(matrix,"[");
	/*else if(mID==1)
		for(i=0;i<8
	*/
	
	
	//Comparacao entre os valores lidos e os valores de referencia para o Mote a ser processado 
		tratamento_temperatura(&Tref[0], Tp, mID,&atuador);
	
		if(atuador == 1)
		{	atua[mID].ar_condicionado = 1;  //Ligar aquecedor
			strcat(matrix,"[0,255,0],");// verde atuador ativo
			
		}	
		
		else  {if(atuador == -1)
		{	atua[mID].ar_condicionado = -1;  // Ligar ar condicionado (frio)
			strcat(matrix,"[0,255,0],");// verde atuador ativo
		}	
		else  
		{	atua[mID].ar_condicionado = 0;
			
			strcat(matrix,"[255,255,255],");//branco atuador desativado
		}
	    }
		
		tratamento_humidade(&Href[0], Hp, mID,&atuador);
	
		if(atuador == 1)
		{	atua[mID].humidificador = -1;  //Ligar desumidificador
			strcat(matrix,"[0,255,0],");// verde atuador ativo
		}
			
		else{ if(atuador == -1)
		{	atua[mID].humidificador = 1;  //Ligar humidificador
			strcat(matrix,"[0,255,0],");// verde atuador ativo
		}
		
		else
		{       atua[mID].humidificador = 0; //Desligar humidificador
			
			strcat(matrix,"[255,255,255],");//branco atuador desativado	
		}
	}
		tratamento_luz(&VLref[0], VLp, mID,&atuador);
	
		if(atuador==1){
			atua[mID].persianas = 1;  //Fechar estores
			atua[mID].luzes = 1;      //Apagar luzes
			strcat(matrix,"[0,255,0],");// verde atuador ativo
			strcat(matrix,"[0,255,0],");// verde atuador ativo
		}
	
		else {if (atuador==-1){
			atua[mID].persianas = -1;  //Abrir estores
			atua[mID].luzes = -1;      //Ligar Luzes
			strcat(matrix,"[0,255,0],");// verde atuador ativo
			strcat(matrix,"[0,255,0],");// verde atuador ativo
		}
		
		else{ 
		atua[mID].persianas = 0; //Fecha estores (por definicao)
		atua[mID].luzes= 0;      //Apaga luzes (por definicao)	
		strcat(matrix,"[255,255,255],");//branco atuador desativado	
		strcat(matrix,"[255,255,255],");//branco atuador desativado	
		}
	 }
		
		if(mID!=5){					//preenche colunas a preto
			for(i=0;i<11;i++)
			strcat(matrix,"[0,0,0],");
		}
		else{	
			strcat(matrix,"[0,0,0],[0,0,0]]");	
			 matrix_file=fopen("matrix.txt","w");
			fprintf(matrix_file,"%s",matrix);
			//printf("%s\n", matrix);
			strcpy(matrix , "");
			 fclose(matrix_file);
		}
		/*
		printf("Temperatura na divisao %d: %.2f \n", mID, sens[mID].temperatura);
		printf("Humidade na divisao %d: %.2f \n", mID, sens[mID].humidade);
		printf("Luz na divisao %d: %.2f \n \n", mID, sens[mID].luz);
		
		printf("Aquecedor na divisao %d: %d \n", mID, atua[mID].ar_condicionado);
		printf("Humidificador na divisao %d: %d \n", mID, atua[mID].humidificador);
		printf("Luzes na divisao %d: %d \n", mID, atua[mID].luzes);
		printf("Persianas na divisao %d: %d \n \n", mID, atua[mID].persianas);
		*/
	
	   
	       
	    
	   
		
}
	    fclose(dados); 
	    printf(" Disposicao dos sensores e atuadores na matriz:\n");
	    printf(" Linhas:\n 1-> Sensor de temperatura\n 2-> Ar condicionado \n 3-> Sensor de Humidade \n 4-> Humidificador \n 5-> Sensor de luminosidade \n 6-> Estoros \n 7-> Luzes\n");
	    printf(" Cada coluna corresponde a um mote e estao ordenados por ordem crescente");
	    return 0;
}
