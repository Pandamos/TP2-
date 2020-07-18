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
#define SE_COPIARON true
#define NO_SE_COPIO false
#define ESPECIE 'E'
#define POKEMON 'P'

//CONTROLAR QUE ARBOL BUSCAR DEVUELVA ARBOL->NODO_RAIZ->ELEMENTO SI EL ELEMENTO ES LA RAIZ
void liberar_pokemon(void* pokemon){
	particular_pokemon_t* pokemon_a_borrar = (particular_pokemon_t*)pokemon;
	free(pokemon_a_borrar);
}

void destruir_pokemones(void* especie_recibida){
	especie_pokemon_t* especie = (especie_pokemon_t*)especie_recibida;
	lista_destruir(especie->pokemones);
	free(especie);
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

	pokedex->ultimos_capturados = lista_crear(liberar_pokemon);
	if (!pokedex->ultimos_capturados){
		free (pokedex);
		return NULL;
	}

	pokedex->ultimos_vistos = lista_crear(liberar_pokemon);
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
				particular_pokemon_t* pokemon_vistos = (particular_pokemon_t*)malloc(sizeof(particular_pokemon_t));
				if (!pokemon_vistos)
					return ERROR;
				else {
					strcpy(pokemon_vistos->nombre, pokemon->nombre);
					pokemon_vistos->nivel = pokemon->nivel;
					pokemon_vistos->capturado = pokemon->capturado;
				}

        int chequeo_agregar = lista_encolar(pokedex->ultimos_vistos, (void*)pokemon_vistos);
				if (chequeo_agregar == ERROR){
					free(pokemon_vistos);
					return ERROR;
				}

				if (pokemon->capturado){
					particular_pokemon_t* pokemon_capturados = (particular_pokemon_t*)malloc(sizeof(particular_pokemon_t));
					if (!pokemon_capturados)
						return ERROR;
					else {
						strcpy(pokemon_capturados->nombre, pokemon->nombre);
						pokemon_capturados->nivel = pokemon->nivel;
						pokemon_capturados->capturado = pokemon->capturado;
					}
          chequeo_agregar = lista_apilar(pokedex->ultimos_capturados, (void*)pokemon_capturados);
          if (chequeo_agregar == ERROR)
	        	return ERROR;
        }

        return EXITO;
}

int actualizar_pokedex(pokedex_t* pokedex, especie_pokemon_t* especie, particular_pokemon_t* pokemon){
	particular_pokemon_t* pokemon_a_insertar = (particular_pokemon_t*)malloc(sizeof(particular_pokemon_t));
	if (!pokemon)
		return ERROR;

	strcpy(pokemon_a_insertar->nombre, pokemon->nombre);
	pokemon_a_insertar->nivel = pokemon->nivel;
	pokemon_a_insertar->capturado = pokemon->capturado;

	int chequeo_historial = actualizar_historial(pokedex, pokemon);
	if (chequeo_historial == ERROR){
		free(pokemon_a_insertar);
		return ERROR;
	}

	especie_pokemon_t* especie_a_insertar = NULL;
	int chequeo_especie = 0;
	int chequeo_lista = 0;

	especie_pokemon_t* especie_encontrada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, (void*)especie);
	if (especie_encontrada){
		chequeo_especie = lista_insertar(especie_encontrada->pokemones, (void*)pokemon_a_insertar);
		if (chequeo_especie == ERROR){
			free(pokemon_a_insertar);
			return ERROR;
		}
	}else {
		especie_a_insertar = (especie_pokemon_t*)malloc(sizeof(especie_pokemon_t));
		if (!especie_a_insertar){
			free(pokemon_a_insertar);
			return ERROR;
		}else {
			especie_a_insertar->pokemones = lista_crear(liberar_pokemon);
			if (!especie_a_insertar->pokemones){
				free(especie_a_insertar);
				free(pokemon_a_insertar);
				return ERROR;
			}
			strcpy(especie_a_insertar->nombre, especie->nombre);
			strcpy(especie_a_insertar->descripcion, especie->descripcion);
			especie_a_insertar->numero = especie->numero;

			chequeo_especie = arbol_insertar(pokedex->pokemones, (void*)especie_a_insertar);
			if (chequeo_especie == ERROR){
				lista_destruir(especie_a_insertar->pokemones);
				free(especie_a_insertar);
				free(pokemon_a_insertar);
			}else {
				chequeo_lista = lista_insertar(especie_a_insertar->pokemones, (void*)pokemon_a_insertar);
				if (chequeo_lista == ERROR){
					free(pokemon_a_insertar);
					return ERROR;
				}
			}
		}
	}

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

	cantidad_leidos = fscanf(mi_pokedex, "%i;%[^;];%[^;];%[^;];%i;%c\n", &especie->numero, especie->nombre, especie->descripcion, pokemon->nombre, &pokemon->nivel, &capturado);
	pokemon->capturado = (capturado == 'S')? true : false;

	while (cantidad_leidos == 6 && chequeo != ERROR){
		chequeo = actualizar_pokedex(pokedex, especie, pokemon);
		cantidad_leidos = fscanf(mi_pokedex, "%i;%[^;];%[^;];%[^;];%i;%c\n", &especie->numero, especie->nombre, especie->descripcion, pokemon->nombre, &pokemon->nivel, &capturado);
		pokemon->capturado = (capturado == 'S')? true : false;
	}

	free(especie);
	free(pokemon);
	fclose(mi_pokedex);

	if (chequeo == ERROR)
		return ERROR;
	else
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

	especie_pokemon_t* especie_a_insertar = (especie_pokemon_t*)malloc(sizeof(especie_pokemon_t));
	if (!especie_a_insertar){
		(*hubo_error) = true;
		return false;
	}else {
		especie_a_insertar->pokemones = lista_crear(liberar_pokemon);
		if (!especie_a_insertar->pokemones){
			free(especie_a_insertar);
			return false;
		}
	}

	especie_anterior = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, (void*)&especie_sin_evolucionar);
	if (!especie_anterior){
		(*hubo_error) = true;
	}else {
		iterador = lista_iterador_crear(especie_anterior->pokemones);
		/*if (!iterador)
			hubo_error*/
		while (lista_iterador_tiene_siguiente(iterador) && !se_encontro){
			pokemon_en_lista = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
			if (pokemon_en_lista){
				if (strcmp(pokemon_en_lista->nombre, nombre_pokemon) == 0)
					se_encontro = true;
				else
					posicion_pokemon++;
			}else
				(*hubo_error) = true;
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
					strcpy(especie_a_insertar->nombre, especie_leida->nombre);
					strcpy(especie_a_insertar->descripcion, especie_leida->descripcion);
					especie_a_insertar->numero = especie_leida->numero;

					chequeo_insertar = arbol_insertar(pokedex->pokemones, (void*)especie_a_insertar);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;

					chequeo_insertar = lista_insertar(especie_a_insertar->pokemones, (void*)pokemon_a_insertar);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;
				}else {
					chequeo_insertar = lista_insertar(especie_encontrada->pokemones, (void*)pokemon_a_insertar);
					if (chequeo_insertar == ERROR)
						(*hubo_error) = true;
				}
			}
			particular_pokemon_t* poke = lista_elemento_en_posicion(especie_anterior->pokemones, (size_t)posicion_pokemon);
			printf("nombre %s\n", poke->nombre);
			chequeo_borrar = lista_borrar_de_posicion(especie_anterior->pokemones, (size_t)posicion_pokemon);
			if (chequeo_borrar == ERROR)
				(*hubo_error) = true;
		}
	}
	if ((*hubo_error)){
		lista_destruir(especie_a_insertar->pokemones);
		free(especie_a_insertar);
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
		especie_leida->pokemones = lista_crear(liberar_pokemon);
		if (!especie_leida->pokemones){
			fclose(evoluciones);
			free(especie_leida);
			return ERROR;
		}
	}

	cantidad_leidos = fscanf(evoluciones, "%i;%[^;];%i;%[^;];%[^\n]\n", &especie_sin_evolucionar.numero, nombre_pokemon, &especie_leida->numero, especie_leida->nombre, especie_leida->descripcion);
	if (cantidad_leidos != 5)
		hubo_error = true;

	while (cantidad_leidos == 5 && !hubo_error){
		se_evoluciono = pokemon_evolucionado_correctamente(especie_leida, especie_sin_evolucionar, nombre_pokemon, pokedex, &hubo_error);
		if (!se_evoluciono)
			hubo_error = true;

		cantidad_leidos = fscanf(evoluciones, "%i;%[^;];%i;%[^;];%[^\n]\n", &especie_sin_evolucionar.numero, nombre_pokemon, &especie_leida->numero, especie_leida->nombre, especie_leida->descripcion);
		if (cantidad_leidos < 5 && cantidad_leidos > 0)
			hubo_error = true;
	}

	fclose(evoluciones);
	lista_destruir(especie_leida->pokemones);
	free(especie_leida);

	if (hubo_error)
		return ERROR;
	else
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
				printf("Especie: %s  Numero de especie: %i  Descripción de especie: %s  Pokemon: %s  Nivel: %i  Capturado: %c\n", especie_encontrada->nombre, especie_encontrada->numero, especie_encontrada->descripcion, pokemon_recibido->nombre, pokemon_recibido->nivel, pokemon_recibido->capturado? fue_capturado : no_fue_capturado);
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

void copiar_informacion(void* especie_recibida, void* archivo, void* booleano){
	bool* hubo_error = (bool*)booleano;
	if (!especie_recibida || !archivo){
		(*hubo_error) = true;
		return;
	}

	especie_pokemon_t* especie = (especie_pokemon_t*)especie_recibida;
	FILE* autoguardado = (FILE*)archivo;
	char es_especie = 'E';
	char es_pokemon = 'P';
	char capturado = 'S';
	char no_fue_capturado = 'N';
	lista_iterador_t* iterador = NULL;
	particular_pokemon_t* pokemon = NULL;

	fprintf(autoguardado, "%c;%i;%s;%s\n", es_especie, especie->numero, especie->nombre, especie->descripcion);
	if (!especie->pokemones)
		return;

	iterador = lista_iterador_crear(especie->pokemones);
	if (!iterador){
		(*hubo_error) = true;
		return;
	}else {
		while (lista_iterador_tiene_siguiente(iterador)){
			pokemon = (particular_pokemon_t*)lista_iterador_siguiente(iterador);
			fprintf(autoguardado, "%c;%s;%i;%c\n", es_pokemon, pokemon->nombre, pokemon->nivel, pokemon->capturado? capturado : no_fue_capturado);
		}
	}
	lista_iterador_destruir(iterador);
}

int pokedex_apagar(pokedex_t* pokedex){
	if (!pokedex || !pokedex->pokemones)
		return ERROR;
 //usar pre order para copiar
 	bool hubo_error = false;
	FILE* autoguardado = fopen("pokedex.txt", "w");
	if (!autoguardado)
		return ERROR;

	fprintf(autoguardado, "%s\n", pokedex->nombre_entrenador);
//fprintf(censo_actualizado, "%s;%i;%i;%i\n", enano_censo.nombre, enano_censo.edad, (enano_censo.cantidad_misiones + 1), enano_censo.id_rango);
	abb_con_cada_elemento(pokedex->pokemones, ABB_RECORRER_PREORDEN, copiar_informacion, (void*)autoguardado, (void*)&hubo_error);

	fclose(autoguardado);
	if (hubo_error)
		return ERROR;
	else
		return EXITO;
}

void agregar_especie(pokedex_t* pokedex, FILE* pokedex_info, bool* hubo_error, especie_pokemon_t** especie_a_utilizar){
	especie_pokemon_t* especie_nueva = (especie_pokemon_t*)malloc(sizeof(especie_pokemon_t));
	if (!especie_nueva){
		(*hubo_error) = true;
		return;
	}else {
		especie_nueva->pokemones = lista_crear(liberar_pokemon);
		if (!especie_nueva->pokemones){
			(*hubo_error) = true;
			free(especie_nueva);
			return;
		}
	}

	int cantidad_leidos = fscanf(pokedex_info, "%[^;];%i;%[^\n]\n", especie_nueva->nombre, &especie_nueva->numero, especie_nueva->descripcion);
	if (cantidad_leidos < 3){
		(*hubo_error) = true;
	}else {
		int chequeo = arbol_insertar(pokedex->pokemones, (void*)especie_nueva);
		if (chequeo == ERROR)
			(*hubo_error) = true;
		else {
			(*especie_a_utilizar) = especie_nueva;
		}
	}

	if ((*hubo_error)){
		lista_destruir(especie_nueva->pokemones);
		free(especie_nueva);
	}
}

void agregar_pokemon(pokedex_t* pokedex, FILE* pokedex_info, bool* hubo_error, especie_pokemon_t* especie_a_utilizar){
	particular_pokemon_t* pokemon = (particular_pokemon_t*)malloc(sizeof(particular_pokemon_t));
	if (!pokemon){
		(*hubo_error) = true;
		return;
	}

	char capturado;

	int cantidad_leidos = fscanf(pokedex_info, "%[^;];%i;%c\n", pokemon->nombre, &pokemon->nivel, &capturado);
	if (cantidad_leidos < 3){
		(*hubo_error) = true;
	}else {
		pokemon->capturado = (capturado == 'S')? true : false;
		int chequeo = lista_insertar(especie_a_utilizar->pokemones, (void*)pokemon);
		if (chequeo == ERROR)
			(*hubo_error) = true;
	}

	if ((*hubo_error)){
		free(pokemon);
	}
}

pokedex_t* pokedex_prender(){
	char nombre_entrenador[MAX_NOMBRE];
	FILE* pokedex_info = fopen("pokedex.txt", "r");
	if (!pokedex_info)
		return NULL;

	int nombre_leido = fscanf(pokedex_info, "%[^\n]\n", nombre_entrenador);
	if (nombre_leido == 0 || nombre_entrenador == '\0')
		return NULL;

	pokedex_t* pokedex = pokedex_crear(nombre_entrenador);
	if (!pokedex)
		return NULL;

	bool hubo_error = false;
	char identificador;
	especie_pokemon_t* especie_a_utilizar = NULL;

	int cantidad_leidos = fscanf(pokedex_info, "%c;", &identificador);
	if (cantidad_leidos == 0){
		pokedex_destruir(pokedex);
		fclose(pokedex_info);
		return NULL;
	}

	while (cantidad_leidos == 1 && !hubo_error){
		if (identificador == ESPECIE){
				agregar_especie(pokedex, pokedex_info, &hubo_error, &especie_a_utilizar);
		}else if (identificador == POKEMON){
				agregar_pokemon(pokedex, pokedex_info, &hubo_error, especie_a_utilizar);
		}else {
			hubo_error = true;
		}
		if (!hubo_error){
			cantidad_leidos = fscanf(pokedex_info, "%c;", &identificador);
			if (cantidad_leidos == 0)
				hubo_error = true;
		}
	}

	fclose(pokedex_info);

	if (hubo_error){
		pokedex_destruir(pokedex);
		return NULL;
	}else
		return pokedex;
}
