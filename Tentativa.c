/*
 * Tentativa.c
 * 
 * Copyright 2018 Francisco Guilherme Alves Pires <up201504951@B02AY0416>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

//#include <stlib.h>
#include <stdio.h>

int divisoes;

void inicializacao(){ 
	//Definir o numero de divisoes da habitacao
	
	FILE *config;
	
	char extra[2];
	config = fopen("MsgCreatorConf.txt", "rt");
	
	// Leitura direta do ficheiro de configuracao do simulador
	fscanf(config, "%s %d", extra, &divisoes);
	fclose(config);
}

int main()
{
	char leitura[100];
	FILE *dados;
	
	//char RawV[divisoes][4], RawT[divisoes][4], RawH[divisoes][4], RawVL[divisoes][4], RawIL[divisoes][4];
	//int V[divisoes], T[divisoes], H[divisoes], VL[divisoes], IL[divisoes];
	
	inicializacao();

	dados = fopen("Leituras.txt", "rt");
	fgets(leitura, sizeof(leitura), dados);
	 
	
	 
	
	
	
	return 0;
}

