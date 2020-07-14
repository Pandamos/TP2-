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
#define FALLO_EVOLUCION false
#define EVOLUCIONO true
#define	LEIDOS_NECESARIOS 5
#define NO_LEYO 0


//CONTROLAR QUE ARBOL BUSCAR DEVUELVA ARBOL->NODO_RAIZ->ELEMENTO SI EL ELEMENTO ES LA RAIZ

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

bool pokemon_evolucionado_correctamente(especie_pokemon_t* especie_leida, especie_pokemon_t especie_sin_evolucionar, char nombre_pokemon[MAX_NOMBRE], pokedex_t* pokedex, bool* hubo_error){
	if (!especie_leida || !nombre_pokemon || !pokedex || !hubo_error)
		return FALLO_EVOLUCION;

	int chequeo_borrar = 0;
	int posicion_pokemon = 0;
	int chequeo_insertar = 0;
	lista_iterador_t* iterador = NULL;
	especie_pokemon_t* especie_anterior = NULL;
	especie_pokemon_t* especie_encontrada = NULL;
	particular_pokemon_t* pokemon_en_lista = NULL;
	particular_pokemon_t* pokemon_a_insertar = NULL;
	bool se_encontro = false;

	especie_anterior = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, (void*)&especie_sin_evolucionar);
	if (!especie_anterior){
		(*hubo_error) = true;
	}else {
		iterador = lista_iterador_crear(especie_anterior->pokemones);
		while (lista_iterador_tiene_siguiente(iterador) && !se_encontro){
			pokemon_en_lista = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
			if (pokemon_en_lista){
				if (strcmp(pokemon_en_lista->nombre, nombre_pokemon) == 0)
					se_encontro = true;
			}else
					posicion_pokemon++;
		}
		lista_iterador_destruir(iterador);

		if (!se_encontro)
			(*hubo_error) = true;
		else {
			pokemon_a_insertar = (particular_pokemon_t*)malloc(sizeof(particular_pokemon_t));
			if (!pokemon_a_insertar)
				(*hubo_error) = true;
			else {
				pokemon_a_insertar->nivel = pokemon_en_lista->nivel;
				pokemon_a_insertar->capturado = pokemon_en_lista->capturado;
				strcpy(pokemon_a_insertar->nombre, pokemon_en_lista->nombre);

				especie_encontrada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, (void*)especie_leida);
				if (!especie_encontrada){
					chequeo_insertar = arbol_insertar(pokedex->pokemones, (void*)especie_leida);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;

					chequeo_insertar = lista_insertar(especie_leida->pokemones, (void*)pokemon_a_insertar);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;
				}else {
					chequeo_insertar = lista_insertar(especie_encontrada->pokemones, (void*)pokemon_a_insertar);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;
				}
			}
			chequeo_borrar = lista_borrar_de_posicion(especie_anterior->pokemones, (size_t)posicion_pokemon);
			if (chequeo_borrar == ERROR)
				(*hubo_error) = true;
		}
	}
	if ((*hubo_error)){
		free(pokemon_a_insertar);
		return FALLO_EVOLUCION;
	}
	else
		return EVOLUCIONO;
}

int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
	if (!pokedex || !ruta_archivo || !pokedex->pokemones)
		return ERROR;

	FILE* evoluciones = fopen(ruta_archivo, "r");
	if (!evoluciones)
		return ERROR;

	char nombre_pokemon[MAX_NOMBRE];
	int cantidad_leidos = 0;
	bool hubo_error = false;
	bool se_evoluciono = false;
	especie_pokemon_t especie_sin_evolucionar;

	especie_pokemon_t* especie_leida = (especie_pokemon_t*)malloc(sizeof(especie_pokemon_t));
	if (!especie_leida){
		fclose(evoluciones);
		return ERROR;
	}else {
		especie_leida->pokemones = lista_crear();
		if (!especie_leida->pokemones){
			fclose(evoluciones);
			free(especie_leida);
			return ERROR;
		}
	}

	cantidad_leidos = fscanf(evoluciones, "%i;%s;%i;%s;%[^;]\n", &especie_sin_evolucionar.numero, nombre_pokemon, &especie_leida->numero, especie_leida->nombre, especie_leida->descripcion);
	if (cantidad_leidos == NO_LEYO || cantidad_leidos < LEIDOS_NECESARIOS)
		hubo_error = true;

	while (cantidad_leidos != EOF && !hubo_error){
		se_evoluciono = pokemon_evolucionado_correctamente(especie_leida, especie_sin_evolucionar, nombre_pokemon, pokedex, &hubo_error);
		if (!se_evoluciono)
			hubo_error = true;

		cantidad_leidos = fscanf(evoluciones, "%i;%s;%i;%s;%[^;]\n", &especie_sin_evolucionar.numero, nombre_pokemon, &especie_leida->numero, especie_leida->nombre, especie_leida->descripcion);
		if (cantidad_leidos == NO_LEYO || cantidad_leidos < LEIDOS_NECESARIOS)
			hubo_error = true;
	}

	fclose(evoluciones);
	lista_destruir(especie_leida->pokemones);
	free(especie_leida);

	if (hubo_error || (cantidad_leidos > NO_LEYO && cantidad_leidos < LEIDOS_NECESARIOS)){
		return ERROR;
	}else
		return EXITO;
}

void pokedex_ultimos_capturados(pokedex_t* pokedex){
	if (!pokedex)
		return;

	particular_pokemon_t* pokemon = NULL;

	while (!lista_vacia(pokedex->ultimos_capturados)){
		pokemon = (particular_pokemon_t*)lista_ultimo(pokedex->ultimos_capturados);

		printf("Nombre: %s  Nivel: %i\n", pokemon->nombre, pokemon->nivel);

		lista_desapilar(pokedex->ultimos_capturados);
	}
}

void pokedex_ultimos_vistos(pokedex_t* pokedex){
	if (!pokedex)
		return;

	particular_pokemon_t* pokemon = NULL;

	while (!lista_vacia(pokedex->ultimos_vistos)){
		pokemon = (particular_pokemon_t*)lista_primero(pokedex->ultimos_vistos);

		printf("Nombre: %s  Nivel: %i\n", pokemon->nombre, pokemon->nivel);

		lista_desencolar(pokedex->ultimos_vistos);
	}
}

void pokedex_destruir(pokedex_t* pokedex){
	if (!pokedex)
		return;

	arbol_destruir(pokedex->pokemones);
	lista_destruir(pokedex->ultimos_vistos);
	lista_destruir(pokedex->ultimos_capturados);

	free(pokedex);
}

void mostrar_pokemon(void* pokemon_recibido, void* cosa){
	cosa = cosa;
	if (!pokemon_recibido)
		return;

	char fue_capturado = 'S';
	char no_fue_capturado = 'N';

	particular_pokemon_t* pokemon = (particular_pokemon_t*)pokemon_recibido;
	printf("Pokemon: %s  Nivel: %i  Capturado: %c\n", pokemon->nombre, pokemon->nivel, pokemon->capturado? fue_capturado : no_fue_capturado);
}

void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
	especie_pokemon_t* especie_a_buscar = (especie_pokemon_t*)malloc(sizeof(especie_pokemon_t));

	especie_a_buscar->numero = numero_pokemon;

	lista_iterador_t* iterador_lista = NULL;
	particular_pokemon_t* pokemon_recibido = NULL;
	bool pokemon_encontrado = false;
	char fue_capturado = 'S';
	char no_fue_capturado = 'N';

	especie_pokemon_t* especie_encontrada = arbol_buscar(pokedex->pokemones, (void*)especie_a_buscar);
	if (especie_encontrada){
		if (nombre_pokemon == 0){
			lista_con_cada_elemento(especie_encontrada->pokemones, mostrar_pokemon, NULL);
			return;
		}

		iterador_lista = lista_iterador_crear(especie_encontrada->pokemones);
		if (!iterador_lista){
			free(especie_a_buscar);
			return;
		}

		while (lista_iterador_tiene_siguiente(iterador_lista) && !pokemon_encontrado){
			pokemon_recibido = (particular_pokemon_t*)lista_iterador_siguiente(iterador_lista);
			if (strcmp(nombre_pokemon, pokemon_recibido->nombre) == IGUALES){
				printf("Especie: %s  Numero de especie: %i  Descripción de especie: %s  Pokemon: %s  Nivel: %i  Capturado: %c", especie_encontrada->nombre, especie_encontrada->numero, especie_encontrada->descripcion, pokemon_recibido->nombre, pokemon_recibido->nivel, pokemon_recibido->capturado? fue_capturado : no_fue_capturado);
				pokemon_encontrado = true;
			}
		}

		lista_iterador_destruir(iterador_lista);

		if (!pokemon_encontrado){
			printf("El pokemon consultado es desconocido\n");
		}
	}else
		printf("La especie consultada es desconocida\n");

	free(especie_a_buscar);
}
