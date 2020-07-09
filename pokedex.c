#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "abb.h"
#include "pokedex.h"

#define ERROR -1
#define EXITO 1
#define MAYOR 1
#define MENOR -1
#define IGUALES 0

void destruir_pokemones(void* especie_recibida){
	especie_pokemon_t* especie = (especie_pokemon_t*)especie_recibida;
	lista_destruir(especie->pokemones);
}

int comparar_especies(void* especie_uno, void* especie_dos){
	especie_pokemon_t* primera = (especie_pokemon_t*)especie_uno;
	especie_pokemon_t* segunda = (especie_pokemon_t*)especie_dos;

	if (primera->numero > segunda->numero)
		return MAYOR;
	else if (primera->numero < segunda->numero)
		return MENOR;
	else 
		return IGUALES;
}

pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
	if (!entrenador)
		return NULL;

	pokedex_t* pokedex = (pokedex_t*)malloc(sizeof(pokedex_t));
	if (!pokedex)
		return NULL;

	strcpy(pokedex->nombre_entrenador, entrenador);
	
	pokedex->ultimos_capturados = lista_crear();
	if (!pokedex->ultimos_capturados){
		free (pokedex);
		return NULL;
	}

	pokedex->ultimos_vistos = lista_crear();
	if (!pokedex->ultimos_vistos){
		lista_destruir(pokedex->ultimos_capturados);
		free (pokedex);
		return NULL;
	}

	pokedex->pokemones = arbol_crear(comparar_especies, destruir_pokemones);
	if (!pokedex->pokemones){
		lista_destruir(pokedex->ultimos_capturados);
		lista_destruir(pokedex->ultimos_vistos);
		free(pokedex);
		return NULL;
	}		

	return pokedex;
}

int actualizar_historial(pokedex_t* pokedex, particular_pokemon_t* pokemon){
        int chequeo_agregar = lista_encolar(pokedex->ultimos_vistos, (void*)pokemon);
	if (chequeo_agregar == ERROR)
                return ERROR;
        else if (pokemon->capturado){
                chequeo_agregar = lista_apilar(pokedex->ultimos_capturados, (void*)pokemon);
                if (chequeo_agregar == ERROR)
                        return ERROR;
        }

        return EXITO;
}

int actualizar_pokedex(pokedex_t* pokedex, especie_pokemon_t* especie, particular_pokemon_t* pokemon){
	int chequeo_historial = actualizar_historial(pokedex, pokemon);
	if (chequeo_historial == ERROR)
		return ERROR;

	int chequeo_especie = 0;
	
	especie_pokemon_t* especie_encontrada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, (void*)especie);
	if (especie_encontrada)
		chequeo_especie = lista_insertar(especie_encontrada->pokemones, (void*)pokemon);
	else
		chequeo_especie = arbol_insertar(pokedex->pokemones, (void*)especie);

	if (chequeo_especie == ERROR)
		return ERROR;
	else
		return EXITO;
}

int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
	if (!pokedex || !ruta_archivo)
		return ERROR;

	FILE* mi_pokedex = fopen(ruta_archivo, "r");
	if (!pokedex)
		return ERROR;
	
	char capturado;
	int chequeo = 0;
	int cantidad_leidos = 0;

	especie_pokemon_t* especie = malloc(sizeof(especie_pokemon_t));
	if (!especie){
		fclose(mi_pokedex);
		return ERROR;
	}

	particular_pokemon_t* pokemon = malloc(sizeof(particular_pokemon_t));
	if (!pokemon){
		fclose(mi_pokedex);
		free(especie);
		return ERROR;
	}

	cantidad_leidos = fscanf(mi_pokedex, "%i;%s;%[^;];%s;%i;%c", &especie->numero, especie->nombre, especie->descripcion, pokemon->nombre, &pokemon->nivel, &capturado);
	pokemon->capturado = (capturado == 'S')? true : false;

	while (cantidad_leidos != EOF && chequeo != ERROR){
		chequeo = actualizar_pokedex(pokedex, especie, pokemon);
		cantidad_leidos = fscanf(mi_pokedex, "%i;%s;%[^;];%s;%i;%c", &especie->numero, especie->nombre, especie->descripcion, pokemon->nombre, &pokemon->nivel, &capturado);
		pokemon->capturado = (capturado == 'S')? true : false;
	}
	
	if (chequeo == ERROR){
		free(especie);
		free(pokemon);
		return ERROR;
	}

	fclose(mi_pokedex);

	return EXITO;
}


