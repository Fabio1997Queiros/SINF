#include <stdlib.h>
#include <stdio.h>
#include <string.h>

volatile int motes=5;
volatile int byte=4;
 
 
 //Funcoes para calcular valores reais das variaveis
float calcularT(int rawTemp){
	float tempAtual;
	tempAtual = -39.6 + (0.01 * rawTemp);
	
	return tempAtual;
}
float calcularVL(int rawViL){
	float vlatual;
	vlatual = 0.625*1000000*(rawViL/4096)*(1.5/100000)*1000;
	
	return vlatual;
}
float calcularH(int rawH, float temp){
	float Hatual, Hrel;
	
	Hrel =  -2.0468+(0.0367*rawH)+(-1.5955*0.000001*rawH*rawH);
	
	Hatual = (temp - 25)*(0.01 + (0.00008*Hrel))+17.69;
	
	return Hatual;
}
float calcularIL(int rawIL){
	float ilatual;
	
	ilatual = 0.769*100000*(rawIL/4096)*(1.5/100000)*1000;
	return ilatual;
}
float calcularV(int rawV){
	float vatual;
	
	vatual = ((rawV/4096)*1.5*2);
	return vatual;
}

//Comparacao entre os valores de refencia e os valores atuais lidos
int tratamento_temperatura(int T_ref, float temp_atual, int divisao){
	int atuador;
	
	if (temp_atual > T_ref[divisao-1])
			atuador = -1;
		
		else if (temp_atual == T_ref[divisao-1])
			atuador = 0;
		
		else if (temp_atual < T_ref[divisao-1])
			atuador = 1;
		
	return atuador;
}
int tratamento_humidade(int H_ref, float hum_atual, int divisao){
	int atuador;
	
	if(hum_atual > H_ref[divisao-1])
		atuador = 1;
	else if (hum_atual < H_ref[divisao-1])
		atuador = -1;
	else if (hum_atual == H_ref[divisao-1])
		atuador = 0;
		
	return atuador;
}
int tratamento_luz(int VL_ref, float Luz_atual, int divisao){
	int atuador;
	
	if( Luz_atual > VL_ref[divisao-1])
		atuador = 1;
	else if( Luz_atual < VL_ref[divisao-1])
		atuador = -1;
	else if( Luz_atual == VL_ref[divisao-1])
		atuador = 0;
		
	return atuador;
}

//Ler informaçao dos sensores e converter para decimal
void Ler_parametros(char *str, int *mID,int*RawH,int*RawVL,int*RawT){
    
    int i;
	char moteID[10]=" ",hum[10]=" ",temp[10]=" ",light[10]=" ";
    char * pch;
    pch = strtok (str," ");
    i=1;
	
	while(pch!=NULL){	

		if(i>5 && i<8){  //ler moteiD
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
	
		printf("%s\n",pch);
		pch = strtok(NULL," ");
		i++;
}	
		
		printf("moteID=%s\n",moteID);
		printf("light=%s\n",light);
		printf("temp=%s\n",temp);
		printf("hum=%s\n",hum);
		
}	

void inicializacao(){ 
	//Definir o numero de divisoes da habitacao
	
	//INCOMPLETA
	
	FILE *config;
	
	char extra[2];
	config = fopen("MsgCreatorConf.txt", "rt");
	
	// Leitura direta do ficheiro de configuracao do simuladorprintf("%s\n",pch);
	fscanf(config, "%s %d", extra, &motes);
	fclose(config);
}

int main()
{
	char leitura[100];
	FILE *dados;
	int aux1, currMote;
	char RawV[byte], RawT[byte], RawH[byte], RawVL[byte], RawIL[byte], mID[byte];
	
	//Vetores de refencia, por enquanto estao definidos no codigo
	int T_ref[motes]= {24 , 25, 25, 23, 26} , H_ref[motes] = {50 , 60 , 55, 50 , 60} , IL_ref[motes] = {1750 , 1750, 1750, 1750, 1750}, VL_ref[motes] = {1750, 1750, 1750, 1750, 1750};
	
	int  T, H, VL;
	float Tp,Hp,VLp;
	
	
	inicializacao();

	dados = fopen("Leituras.txt", "rt");
	fgets(leitura, sizeof(leitura), dados);
	//Temos a informacao do simulador referente a uma linha de codigo
	
	
	//Enquanto ler valores do ficheiro, executa (Não sei se isto salta uma linha do ficheiro)
	while( fgets(leitura, sizeof(leitura), dados) != EOF){
	
	
	//Passagem da informacao para os vetores referentes aos valores de cada sensor
	
	aux1=0;
	//Ler o valor do moteID do simulador e passar para o vetor mID
		for(i=12;i<17;i++){	
			if(i==14)
				i++;
			
			strcpy(mID[aux1], leitura[i]);
		
			aux1++;
	}
		
	aux1=0;
	//Ler o valor do RawV do simulador e passar para o vetor RawV
			for(i=27;i<32;i++){	
				if(i==29)
					i++;
			
			strcpy(RawV[aux1], leitura[i]);
			aux1++;
	}
	
	aux1=0;
	//Ler o valor do RawVL do simulador e passar para o vetor RawVL
			for(i=33;i<38;i++){	
				if(i==35)
					i++;
			
			strcpy(RawVL[aux1], leitura[i]);
			aux1++;
	}
	
	aux1=0;
	//Ler o valor do RawIL do simulador e passar para o vetor RawIL
			for(i=39;i<44;i++){	
				if(i==41)
					i++;
			
			strcpy(RawIL[aux1], leitura[i]);
			aux1++;
	}
	aux1=0;
	//Ler o valor do RawT do simulador e passar para o vetor RawT
			for(i=45;i<50;i++){	
				if(i==47)
					i++;
			
			strcpy(RawT[aux1], leitura[i]);
			aux1++;
	}
	
	aux1=0;
	//Ler o valor do RawH do simulador e passar para o vetor RawH
			for(i=51;i<56;i++){	
				if(i==53)
					i++;
			
			strcpy(RawH[aux1], leitura[i]);
			aux1++;
	}
	
	
	//Conversao das strings Hexadecimais para Int
	currMote = hexa2dec(mID);
	VL = hexa2dec(RawVL);
	T = hexa2dec(RawT);
	H = hexa2dec(RawH);
	
	//Calcular o valor real de cada variavel, atraves de funcoes pre-definidas. Retorna float
	Vp = calcularT(V);
	VLp = calcularVL(VL);
	ILp = calcularIL(IL);
	Tp = calcularT(T);
	Hp = calcularH(H, Tp);
	
	//Comparacao entre os valores lidos e os valores de referencia para o Mote a ser processado
	
	if(tratamento_temperatura(T_ref, Tp, currMote) == 1)
		//Ligar aquecedor
	else if(tratamento_temperatura(T_ref, Tp, currMote) == 0)
		//Desligar aquecedor / ar condicionado
	else if (tratamento_temperatura(T_ref, Tp, currMote) == -1)
		// Ligar ar condicionado (frio)
	
	if(tratamento_humidade(H_ref, Hp, currMote) == 1)
		//Ligar deshumificador
	else if(tratamento_humidade(H_ref, Hp, currMote) == -1)
		//Ligar humificador
	else if(tratamento_humidade(H_ref, Hp, currMote) == 0)
		//Desligar ambos
		
	if(tratamento_luz(VL_ref, VLp, currMote)==1)
		//Fechar estores
	else if (tratamento_luz(VL_ref, VLp, currMote)==-1)
		//Abrir estores
	else if(tratamento_luz(VL_ref, VLp, currMote)==0)
		//Nao faz nada?
		
	

	 
}

	fclose(dados);
	return 0;
}

