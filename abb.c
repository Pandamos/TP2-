#include "abb.h"
#include <stdlib.h>
#include <stdbool.h>

#define NODO_NULL NULL
#define TAMANIO_INVALIDO 0
#define MAXIMO_VALOR_RECORRIDO 2
#define MINIMO_VALOR_RECORRIDO 0
#define NODO_SIN_HIJOS 0
#define NODO_DERECHA 1
#define NODO_IZQUIERDA -1
#define NO_SE_CARGO 0
#define ERROR_BORRAR -1
#define ES_IGUAL 0
#define ES_MAYOR 1
#define ES_MENOR -1
#define EXITO 0
#define ERROR_INSERTAR -1
#define ESTA_VACIO true

abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
	if (!comparador)
		return NULL;

	abb_t* arbol = (abb_t*)malloc(sizeof(abb_t));
	if (!arbol)
		return NULL;

	arbol->nodo_raiz = NULL;
	arbol->comparador = comparador;
	arbol->destructor = destructor;

	return arbol;
}

void* arbol_raiz(abb_t* arbol){
	if (!arbol || arbol_vacio(arbol))
		return NULL;

	return arbol->nodo_raiz->elemento;
}

bool arbol_vacio(abb_t* arbol){
	if (!arbol)
		return ESTA_VACIO;

	return arbol->nodo_raiz == NODO_NULL;
}

/*
 * Crea un nodo para el árbol e inicializa el mismo.
 * Devuelve NULL si no pudo hacerlo.
 */
nodo_abb_t* nodo_crear(){
	nodo_abb_t* nodo = (nodo_abb_t*)malloc(sizeof(nodo_abb_t));
	if (!nodo)
		return NULL;

	nodo->elemento = NULL;
	nodo->izquierda = NULL;
	nodo->derecha = NULL;

	return nodo;
}

/*
 * Recibe un nodo para comparar, el nodo con el nuevo elemento y el comparador.
 * Busca recursivamente donde insertar el nodo con el nuevo elemento.
 * Inserta el nuevo nodo.
 */
void nodo_insertar(nodo_abb_t* nodo_existente, nodo_abb_t* nodo_nuevo, abb_comparador comparador){
	int comparacion_nuevo_existente = comparador(nodo_nuevo->elemento, nodo_existente->elemento);

	if (!nodo_existente->izquierda && !nodo_existente->derecha){
		if (comparacion_nuevo_existente == ES_MENOR)
			nodo_existente->izquierda = nodo_nuevo;
		else
			nodo_existente->derecha = nodo_nuevo;
		return;
	}

	if (!nodo_existente->izquierda || !nodo_existente->derecha){
		if (!nodo_existente->izquierda){
			if (comparacion_nuevo_existente == ES_MENOR)
				nodo_existente->izquierda = nodo_nuevo;
			else
				nodo_insertar(nodo_existente->derecha, nodo_nuevo, comparador);
			return;
		}else if (!nodo_existente->derecha){
			if (comparacion_nuevo_existente == ES_MENOR)
				nodo_insertar(nodo_existente->izquierda, nodo_nuevo, comparador);
			else
				nodo_existente->derecha = nodo_nuevo;
			return;
		}
	}

	if (comparacion_nuevo_existente == ES_MENOR)
		nodo_insertar(nodo_existente->izquierda, nodo_nuevo, comparador);
	else
		nodo_insertar(nodo_existente->derecha, nodo_nuevo, comparador);
}

int arbol_insertar(abb_t* arbol, void* elemento){
	if (!arbol)
		return ERROR_INSERTAR;

	nodo_abb_t* nodo_nuevo = nodo_crear();
	if (!nodo_nuevo)
		return ERROR_INSERTAR;

	nodo_nuevo->elemento = elemento;

	if (arbol_vacio(arbol)){
		arbol->nodo_raiz = nodo_nuevo;
		return EXITO;
	}

	nodo_insertar(arbol->nodo_raiz, nodo_nuevo, arbol->comparador);

	return EXITO;
}

/*
 * Recibe el nodo y el elemento que van a ser comparados, un nodo para almacenar la dirección del nodo encontrado y el comparador.
 * Busca recursivamente al nodo que contiene al elemento solicitado y almacena su dirección.
 * Si el elemento no se encuentra en el árbol la dirección que queda almacenada es NULL.
 */
void buscar_elemento(nodo_abb_t* nodo, void* elemento, nodo_abb_t** nodo_elemento_encontrado, abb_comparador comparador){
	int comparacion_elementos = comparador (elemento, nodo->elemento);

	if (comparacion_elementos == ES_IGUAL){
		(*nodo_elemento_encontrado) = nodo;
		return;
	}else if (comparacion_elementos == ES_MENOR){
		if (nodo->izquierda)
			buscar_elemento(nodo->izquierda, elemento, nodo_elemento_encontrado, comparador);
		else
			return;
	}else if (comparacion_elementos == ES_MAYOR){
		if (nodo->derecha)
			buscar_elemento(nodo->derecha, elemento, nodo_elemento_encontrado, comparador);
		else
			return;
	}
}

void* arbol_buscar(abb_t* arbol, void* elemento){
	if (!arbol || arbol_vacio(arbol) || !elemento)
		return NULL;

	nodo_abb_t* nodo_elemento_encontrado = NULL;

	abb_comparador comparador = arbol->comparador;
	int comparacion = comparador (arbol->nodo_raiz->elemento, elemento);

	if (comparacion == ES_IGUAL)
		return arbol->nodo_raiz->elemento;

	buscar_elemento(arbol->nodo_raiz, elemento, &nodo_elemento_encontrado, arbol->comparador);

	if (nodo_elemento_encontrado)
		return nodo_elemento_encontrado->elemento;
	else
		return NULL;
}

/*
 * Recibe un nodo y un puntero a nodo que va a guardar la dirección del padre del mayor de los nietos.
 * Busca recursivamente al nieto mayor del nodo a borrar y almacena su dirección en el nodo
 * recibido por parámetro.
 */
void buscar_mayor_de_menores(nodo_abb_t* nodo, nodo_abb_t** nodo_padre_del_mayor){
  if (!nodo->derecha)
    return;

  (*nodo_padre_del_mayor) = nodo;
  buscar_mayor_de_menores(nodo->derecha, nodo_padre_del_mayor);
}

/*
 * Recibe el destructor y el nodo padre del nodo a borrar, ubicado del lado derecho del arbol, busca al mayor de los menores para reemplazar al nodo a borrar y libera la memoria
 * utilizada por el mismo.
 */
void usar_mayor_de_menores_der(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_padre_del_mayor = nodo_padre_del_borrado->derecha->izquierda;
  nodo_abb_t* nodo_a_borrar = NULL;
  nodo_abb_t* nodo_mayor_de_menores = NULL;

  buscar_mayor_de_menores(nodo_padre_del_borrado->derecha->izquierda->derecha, &nodo_padre_del_mayor);

  nodo_a_borrar = nodo_padre_del_borrado->derecha;
  nodo_mayor_de_menores = nodo_padre_del_mayor->derecha;

  if (destructor)
    destructor (nodo_padre_del_borrado->derecha->elemento);

  if (nodo_padre_del_mayor->derecha->izquierda)
    nodo_padre_del_mayor->derecha = nodo_padre_del_mayor->derecha->izquierda;
  else
    nodo_padre_del_mayor->derecha = NULL;

  nodo_mayor_de_menores->izquierda = nodo_a_borrar->izquierda;
  nodo_mayor_de_menores->derecha = nodo_a_borrar->derecha;
  nodo_padre_del_borrado->derecha = nodo_mayor_de_menores;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el nodo padre del nodo a borrar, ubicado del lado izquierdo del arbol, busca el mayor de los menores para reemplazar al nodo a borrar y libera
 * la memoria utilizada por el mismo.
 */
void usar_mayor_de_menores_izq(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_padre_del_mayor = nodo_padre_del_borrado->izquierda->izquierda;
  nodo_abb_t* nodo_a_borrar = NULL;
  nodo_abb_t* nodo_mayor_de_menores = NULL;

  buscar_mayor_de_menores(nodo_padre_del_borrado->izquierda->izquierda->derecha, &nodo_padre_del_mayor);

  nodo_a_borrar = nodo_padre_del_borrado->izquierda;
  nodo_mayor_de_menores = nodo_padre_del_mayor->derecha;

  if (destructor)
    destructor (nodo_padre_del_borrado->izquierda->elemento);

  if (nodo_padre_del_mayor->derecha->izquierda)
    nodo_padre_del_mayor->derecha = nodo_padre_del_mayor->derecha->izquierda;
  else
    nodo_padre_del_mayor->derecha = NULL;

  nodo_mayor_de_menores->izquierda = nodo_a_borrar->izquierda;
  nodo_mayor_de_menores->derecha = nodo_a_borrar->derecha;
  nodo_padre_del_borrado->izquierda = nodo_mayor_de_menores;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el nodo padre del nodo a borrar, ubicado del lado derecho del arbol, usa el hijo izquierdo para reemplazarlo. Libera la memoria utilizada por el mismo.
 */
void usar_hijo_izquierdo_der(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_a_borrar = nodo_padre_del_borrado->derecha;

  if (destructor)
    destructor (nodo_a_borrar->elemento);

  nodo_padre_del_borrado->derecha->izquierda->derecha = nodo_a_borrar->derecha;
  nodo_padre_del_borrado->derecha = nodo_padre_del_borrado->derecha->izquierda;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el nodo padre del nodo a borrar, ubicado del lado izquierdo del arbol, usa el hijo izquierdo para reemplazarlo. Libera la memoria utilizada por el mismo.
 */
void usar_hijo_izquierdo_izq(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_a_borrar = nodo_padre_del_borrado->izquierda;

  if (destructor)
    destructor (nodo_a_borrar->elemento);

  nodo_padre_del_borrado->izquierda->izquierda->derecha = nodo_a_borrar->derecha;
  nodo_padre_del_borrado->izquierda = nodo_padre_del_borrado->izquierda->izquierda;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el nodo a borrar ubicado a la derecha del arbol y lo elimina liberando la memoria utilizda por el mismo.
 */
void borrar_elemento_derecho(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_auxiliar = NULL;

  if (!nodo_padre_del_borrado->derecha->derecha && !nodo_padre_del_borrado->derecha->izquierda){
    if (destructor)
      destructor (nodo_padre_del_borrado->derecha->elemento);
    free (nodo_padre_del_borrado->derecha);

    nodo_padre_del_borrado->derecha = NULL;
    return;
  }

  if (!nodo_padre_del_borrado->derecha->derecha || !nodo_padre_del_borrado->derecha->izquierda){
    nodo_auxiliar = nodo_padre_del_borrado->derecha;
    if (destructor)
      destructor (nodo_padre_del_borrado->derecha->elemento);

    if (nodo_padre_del_borrado->derecha->izquierda)
      nodo_padre_del_borrado->derecha = nodo_padre_del_borrado->derecha->izquierda;
    else
      nodo_padre_del_borrado->derecha = nodo_padre_del_borrado->derecha->derecha;

    free (nodo_auxiliar);
    return;
  }

  if (!nodo_padre_del_borrado->derecha->izquierda->derecha)
    usar_hijo_izquierdo_der(nodo_padre_del_borrado, destructor);
  else
    usar_mayor_de_menores_der(nodo_padre_del_borrado, destructor);
}

/*
 * Recibe el destructor y el nodo a borrar ubicado a la izquierda del arbol y lo elimina liberando la memoria utilizada por el mismo.
 */
void borrar_elemento_izquierdo(nodo_abb_t* nodo_padre_del_borrado, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_auxiliar = NULL;

  if (!nodo_padre_del_borrado->izquierda->derecha && !nodo_padre_del_borrado->izquierda->izquierda){
    if (destructor)
      destructor (nodo_padre_del_borrado->izquierda->elemento);
    free (nodo_padre_del_borrado->izquierda);

    nodo_padre_del_borrado->izquierda = NULL;
    return;
  }

  if (!nodo_padre_del_borrado->izquierda->derecha || !nodo_padre_del_borrado->izquierda->izquierda){
    nodo_auxiliar = nodo_padre_del_borrado->izquierda;
    if (destructor)
      destructor (nodo_padre_del_borrado->izquierda->elemento);

    if (nodo_padre_del_borrado->izquierda->izquierda)
      nodo_padre_del_borrado->izquierda = nodo_padre_del_borrado->izquierda->izquierda;
    else
      nodo_padre_del_borrado->izquierda = nodo_padre_del_borrado->izquierda->derecha;

    free (nodo_auxiliar);
    return;
  }

  if (!nodo_padre_del_borrado->izquierda->izquierda->derecha)
    usar_hijo_izquierdo_izq(nodo_padre_del_borrado, destructor);
  else
    usar_mayor_de_menores_izq(nodo_padre_del_borrado, destructor);
}

/*
 * Recibe el nodo raiz, el elemento que se desea eliminar, el comparador y el destructor. Encuentra el padre del nodo a eliminar y lo deriva a eliminar.
 */
void buscar_nodo_y_eliminar(nodo_abb_t* nodo, void* elemento, abb_comparador comparador, abb_liberar_elemento destructor){
  int comparacion_elemento_nodo = comparador (elemento, nodo->elemento);

  if (comparacion_elemento_nodo == ES_MENOR){
    comparacion_elemento_nodo = comparador (elemento, nodo->izquierda->elemento);
    if (comparacion_elemento_nodo == ES_IGUAL)
      borrar_elemento_izquierdo(nodo, destructor);
    else
      buscar_nodo_y_eliminar(nodo->izquierda, elemento, comparador, destructor);
  }else {
    comparacion_elemento_nodo = comparador (elemento, nodo->derecha->elemento);
    if (comparacion_elemento_nodo == ES_IGUAL)
      borrar_elemento_derecho(nodo, destructor);
    else
      buscar_nodo_y_eliminar(nodo->derecha, elemento, comparador, destructor);
  }
}
 /*
  * Recibe el destructor y el arbol; reemplaza la raíz con su hijo izquierdo. Se libera la memoria utilizada por la anterior raíz.
	*/
void usar_hijo_izquierdo_raiz(abb_t* arbol, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_a_borrar = arbol->nodo_raiz;

  if (destructor)
    destructor (nodo_a_borrar->elemento);

  arbol->nodo_raiz->izquierda->derecha = arbol->nodo_raiz->derecha;
  arbol->nodo_raiz = arbol->nodo_raiz->izquierda;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el arbol; reemplaza la raiz por el mayor de los menores. Se libera la memoria utilizada por la anterior raíz.
 */
void usar_mayor_de_menores_raiz(abb_t* arbol, abb_liberar_elemento destructor){
  nodo_abb_t* nodo_padre_del_mayor = arbol->nodo_raiz->izquierda;
  nodo_abb_t* nodo_a_borrar = arbol->nodo_raiz;
  nodo_abb_t* nodo_mayor_de_menores = NULL;

	if (destructor)
		destructor (arbol->nodo_raiz->elemento);

  buscar_mayor_de_menores(nodo_a_borrar->izquierda->derecha, &nodo_padre_del_mayor);

	nodo_mayor_de_menores = nodo_padre_del_mayor->derecha;

  if (nodo_mayor_de_menores->izquierda)
    nodo_padre_del_mayor->derecha = nodo_mayor_de_menores->izquierda;
  else
    nodo_padre_del_mayor->derecha = NULL;

  nodo_mayor_de_menores->izquierda = nodo_a_borrar->izquierda;
  nodo_mayor_de_menores->derecha = nodo_a_borrar->derecha;

  arbol->nodo_raiz = nodo_mayor_de_menores;

  free (nodo_a_borrar);
}

/*
 * Recibe el destructor y el arbol; elimina la raíz.
 */
void eliminar_raiz(abb_t* arbol, abb_liberar_elemento destructor){
  if (!arbol->nodo_raiz->izquierda && !arbol->nodo_raiz->derecha){
    if (destructor)
      destructor (arbol->nodo_raiz->elemento);

    free (arbol->nodo_raiz);

    arbol->nodo_raiz = NULL;
    return;
  }

  if (!arbol->nodo_raiz->izquierda || !arbol->nodo_raiz->derecha){
    nodo_abb_t* nodo_a_borrar = arbol->nodo_raiz;

    if (destructor)
      destructor (arbol->nodo_raiz->elemento);

    if (arbol->nodo_raiz->izquierda)
      arbol->nodo_raiz = arbol->nodo_raiz->izquierda;
    else
      arbol->nodo_raiz = arbol->nodo_raiz->derecha;

    free (nodo_a_borrar);
    return;
  }

  if (!arbol->nodo_raiz->izquierda->derecha)
    usar_hijo_izquierdo_raiz(arbol, destructor);
  else
    usar_mayor_de_menores_raiz(arbol, destructor);
}

int arbol_borrar(abb_t* arbol, void* elemento){
	if (!arbol || arbol_vacio(arbol))
		return ERROR_BORRAR;

  void* elemento_a_buscar = arbol_buscar(arbol, elemento);

  if (!elemento_a_buscar && elemento)
    return ERROR_BORRAR;

  abb_liberar_elemento destructor = arbol->destructor;
  abb_comparador comparador = arbol->comparador;
  int comparacion_elemento_nodo = comparador (elemento, arbol->nodo_raiz->elemento);

  if (comparacion_elemento_nodo == ES_IGUAL)
    eliminar_raiz(arbol, destructor);
  else
    buscar_nodo_y_eliminar(arbol->nodo_raiz, elemento, comparador, destructor);

  return EXITO;
}

/*
 * Recibe el destructor y el nodo raiz; recorre el arbol eliminando cada nodo.
 */
void arbol_borrar_elementos(nodo_abb_t* nodo, abb_liberar_elemento destructor){
	if (nodo->izquierda)
		arbol_borrar_elementos(nodo->izquierda, destructor);

	if (nodo->derecha)
		arbol_borrar_elementos(nodo->derecha, destructor);

	if (destructor)
		destructor (nodo->elemento);

	free (nodo);
}

void arbol_destruir(abb_t* arbol){
	if (!arbol)
		return;

	if (arbol_vacio(arbol)){
		free (arbol);
		return;
	}

	arbol_borrar_elementos(arbol->nodo_raiz, arbol->destructor);
	free (arbol);
}

/*
 * Recibe un nodo, un array valido, el tope del array y un contador. Recorre el arbol inorden cargando el array recibido siempre y cuando no esté lleno.
 */
void cargar_vector_inorden(nodo_abb_t* nodo, void** array, int tamanio_array, int* contador){
	if ((*contador) == tamanio_array)
		return;

	if (nodo->izquierda){
		cargar_vector_inorden(nodo->izquierda, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}

	array[(*contador)] = nodo->elemento;
	(*contador) ++;

	if (nodo->derecha){
		cargar_vector_inorden(nodo->derecha, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}
}

int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array){
	if (!arbol || !array || tamanio_array <= TAMANIO_INVALIDO || arbol_vacio(arbol))
		return NO_SE_CARGO;

	int contador_agregados = 0;

	cargar_vector_inorden(arbol->nodo_raiz, array, tamanio_array, &contador_agregados);

	return contador_agregados;
}

/*
 * Recibe un nodo, un array valido, el tope del array y un contador. Recorre el arbol preorden cargando el array recibido siempre y cuando no esté lleno.
 */
void cargar_vector_preorden(nodo_abb_t* nodo, void** array, int tamanio_array, int* contador){
	if ((*contador) == tamanio_array)
		return;

	array[(*contador)] = nodo->elemento;
	(*contador) ++;

	if (nodo->izquierda){
		cargar_vector_preorden(nodo->izquierda, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}

	if (nodo->derecha){
		cargar_vector_preorden(nodo->derecha, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}
}

int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array){
	if (!arbol || !array || tamanio_array <= TAMANIO_INVALIDO || arbol_vacio(arbol))
		return NO_SE_CARGO;

	int contador_agregados = 0;

	cargar_vector_preorden(arbol->nodo_raiz, array, tamanio_array, &contador_agregados);

	return contador_agregados;
}

/*
 * Recibe un nodo, un array valido, el tope del array y un contador. Recorre el arbol postorden cargando el array recibido siempre y cuando no esté lleno.
 */
void cargar_vector_postorden(nodo_abb_t* nodo, void** array, int tamanio_array, int* contador){
	if ((*contador) == tamanio_array)
		return;

	if (nodo->izquierda){
		cargar_vector_postorden(nodo->izquierda, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}

	if (nodo->derecha){
		cargar_vector_postorden(nodo->derecha, array, tamanio_array, contador);
		if ((*contador) == tamanio_array)
			return;
	}

	array[(*contador)] = nodo->elemento;
	(*contador) ++;
}

int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array){
	if (!arbol || !array || tamanio_array <= TAMANIO_INVALIDO || arbol_vacio(arbol))
		return NO_SE_CARGO;

	int contador_agregados = 0;

	cargar_vector_postorden(arbol->nodo_raiz, array, tamanio_array, &contador_agregados);

	return contador_agregados;
}

/*
 * Recorre el arbol inorden enviando a cada elemento a la función recibida por parámetro, si la función devuelve NULL
 * finaliza el recorrido.
 */
void recorrer_arbol_inorden(nodo_abb_t* nodo, void (*funcion)(void*, void*, void*), void* extra, void* extra_2, bool* resultado_funcion){
	if ((*resultado_funcion))
		return;

	if (nodo->izquierda){
		recorrer_arbol_inorden(nodo->izquierda, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}

	funcion(nodo->elemento, extra, extra_2);

	if (nodo->derecha && !(*resultado_funcion)){
		recorrer_arbol_inorden(nodo->derecha, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}
}

/*
 * Recorre el arbol postorden enviando a cada elemento a la función recibida por parámetro, si la función devuelve NULL
 * finaliza el recorrido.
 */
void recorrer_arbol_postorden(nodo_abb_t* nodo, void (*funcion)(void*, void*, void*), void* extra, void* extra_2, bool* resultado_funcion){
	if ((*resultado_funcion))
		return;

	if (nodo->izquierda){
		recorrer_arbol_postorden(nodo->izquierda, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}

	if (nodo->derecha){
		recorrer_arbol_postorden(nodo->derecha, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}

	funcion(nodo->elemento, extra, extra_2);
}

/*
 * Recorre el arbol preorden enviando a cada elemento a la función recibida por parámetro, si la función devuelve NULL
 * finaliza el recorrido.
 */
void recorrer_arbol_preorden(nodo_abb_t* nodo, void (*funcion)(void*, void*, void*), void* extra, void* extra_2, bool* resultado_funcion){
	if ((*resultado_funcion))
		return;

	funcion(nodo->elemento, extra, extra_2);

	if (nodo->izquierda){
		recorrer_arbol_preorden(nodo->izquierda, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}

	if (nodo->derecha){
		recorrer_arbol_preorden(nodo->derecha, funcion, extra, extra_2, resultado_funcion);
		if ((*resultado_funcion))
			return;
	}
}

void abb_con_cada_elemento(abb_t* arbol, int recorrido, void (*funcion)(void*, void*, void*), void* extra, void* extra_2){
	if (!arbol || !funcion || recorrido > MAXIMO_VALOR_RECORRIDO || recorrido < MINIMO_VALOR_RECORRIDO || arbol_vacio(arbol))
		return;

	bool resultado_funcion = false;

	if (recorrido == ABB_RECORRER_INORDEN)
		recorrer_arbol_inorden(arbol->nodo_raiz, funcion, extra, extra_2, &resultado_funcion);
	else if (recorrido == ABB_RECORRER_PREORDEN)
		recorrer_arbol_preorden(arbol->nodo_raiz, funcion, extra, extra_2, &resultado_funcion);
	else
		recorrer_arbol_postorden(arbol->nodo_raiz, funcion, extra, extra_2, &resultado_funcion);
}
