#include "lista.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define ULTIMO_ELEMENTO lista->cantidad_elementos-1
#define ANTERIOR_A_POSICION posicion-1
#define NO_TIENE_SIGUIENTE NULL
#define ERROR_MEMORIA NULL
#define FALLO_ITERADOR NULL
#define FALLO_LISTA NULL

typedef struct nodo{
  void* elemento;
  struct nodo* siguiente;
}nodo_t;

struct lista{
  nodo_t* tope;
  nodo_t* primero;
  size_t cantidad_elementos;
  lista_liberar_elemento destructor;
};

struct lista_iterador{
  nodo_t* indice;
  lista_t* lista;
};

const int ERROR = -1;
const int EXITO = 0;
const int PRIMERA = 0;
const int VACIA = 0;
const int UN_ELEMENTO = 1;
const int INICIO_ITERACION = 0;
const size_t PRIMER_ELEMENTO = 0;
const size_t NO_EXISTE_LISTA = 0;
const bool ERROR_ITERADOR = false;
const bool ESTA_VACIA = true;
const bool ULTIMO_ELEMENTO_VALIDO = true;

lista_t* lista_crear(lista_liberar_elemento destructor){
  lista_t* lista = (lista_t*)malloc(sizeof(lista_t));
  if (!lista)
    return ERROR_MEMORIA;

  lista->tope = NULL;
  lista->primero = NULL;
  lista->cantidad_elementos = 0;
  lista->destructor = destructor;

  return lista;
}

int lista_insertar(lista_t* lista, void* elemento){
  if (!lista)
    return ERROR;

  nodo_t* nodo_nuevo = (nodo_t*)malloc(sizeof(nodo_t));
  if (!nodo_nuevo)
    return ERROR;

  nodo_nuevo->elemento = elemento;
  nodo_nuevo->siguiente = NULL;

  if (lista_vacia(lista))
    lista->primero = nodo_nuevo;
  else
    lista->tope->siguiente = nodo_nuevo;

  lista->tope = nodo_nuevo;
  lista->cantidad_elementos ++;

  return EXITO;
}

void agregar_nodo(lista_t* lista, size_t posicion, nodo_t* nodo_nuevo, nodo_t* nodo_iterador){
  int i = 0;

  if (posicion == PRIMERA){
    nodo_nuevo->siguiente = lista->primero;
    lista->primero = nodo_nuevo;
  }else {
    while (i < ANTERIOR_A_POSICION){
      nodo_iterador = nodo_iterador->siguiente;
      i ++;
    }
    nodo_nuevo->siguiente = nodo_iterador->siguiente;
    nodo_iterador->siguiente = nodo_nuevo;
  }

  lista->cantidad_elementos ++;
}

int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
  if (!lista)
    return ERROR;

  if (lista_vacia(lista) || (posicion >= lista->cantidad_elementos))
    return lista_insertar(lista, elemento);

  nodo_t* nodo_iterador = lista->primero;
  nodo_t* nodo_nuevo = (nodo_t*)malloc(sizeof(nodo_t));
  if (!nodo_nuevo)
    return ERROR;

  nodo_nuevo->elemento = elemento;
  nodo_nuevo->siguiente = NULL;

  agregar_nodo(lista, posicion, nodo_nuevo, nodo_iterador);

  return EXITO;
}

int lista_borrar(lista_t* lista){
  if (!lista || lista_vacia(lista))
    return ERROR;

  if (lista->cantidad_elementos == UN_ELEMENTO)
    return lista_desencolar(lista);

  nodo_t* nodo_borrador = lista->tope;
  nodo_t* nodo_iterador = lista->primero;

  while (nodo_iterador->siguiente != lista->tope){
    nodo_iterador = nodo_iterador->siguiente;
  }
  nodo_iterador->siguiente = NULL;
  lista->tope = nodo_iterador;
  if (lista->destructor)
    lista->destructor(nodo_borrador->elemento);
  free (nodo_borrador);

  lista->cantidad_elementos --;

  return EXITO;
}

void borrar_nodo(lista_t* lista, size_t posicion, nodo_t* nodo_borrador, nodo_t* nodo_iterador){
  int i = 0;

  while (i < ANTERIOR_A_POSICION){
    nodo_iterador = nodo_iterador->siguiente;
    i ++;
  }

  nodo_borrador = nodo_iterador->siguiente;
  nodo_iterador->siguiente = nodo_borrador->siguiente;

  if (lista->destructor)
    lista->destructor(nodo_borrador->elemento);
  free (nodo_borrador);

  lista->cantidad_elementos --;
}

int lista_borrar_de_posicion(lista_t* lista, size_t posicion){
  if (!lista || lista_vacia(lista))
    return ERROR;

  if (posicion >= lista->cantidad_elementos || posicion == ULTIMO_ELEMENTO)
    return lista_borrar(lista);

  if (posicion == PRIMER_ELEMENTO)
    return lista_desencolar(lista);

  nodo_t* nodo_iterador = lista->primero;
  nodo_t* nodo_borrador = NULL;

  borrar_nodo(lista, posicion, nodo_borrador, nodo_iterador);

  return EXITO;
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
  if (!lista || posicion >= lista->cantidad_elementos)
    return FALLO_LISTA;

  if (posicion == PRIMER_ELEMENTO)
    return lista_primero(lista);

  if (posicion == ULTIMO_ELEMENTO)
    return lista_ultimo(lista);

  int i = 0;
  nodo_t* nodo_iterador = lista->primero;

  while (i < posicion){
    nodo_iterador = nodo_iterador->siguiente;
    i ++;
  }

  return nodo_iterador->elemento;
}

void* lista_ultimo(lista_t* lista){
  return lista_tope(lista);
}

bool lista_vacia(lista_t* lista){
  if (!lista)
    return ESTA_VACIA;
  else
    return lista->cantidad_elementos == VACIA;
}

size_t lista_elementos(lista_t* lista){
  if (!lista)
    return NO_EXISTE_LISTA;
  else
    return lista->cantidad_elementos;
}

int lista_apilar(lista_t* lista, void* elemento){
  return lista_insertar(lista, elemento);
}

int lista_desapilar(lista_t* lista){
  return lista_borrar(lista);
}

void* lista_tope(lista_t* lista){
  if (!lista || lista_vacia(lista))
    return FALLO_LISTA;
  else
    return lista->tope->elemento;
}

int lista_encolar(lista_t* lista, void* elemento){
  return lista_insertar(lista, elemento);
}

int lista_desencolar(lista_t* lista){
  if (!lista || lista_vacia(lista))
    return ERROR;

  if (lista->cantidad_elementos == UN_ELEMENTO){
    if (lista->destructor)
      lista->destructor(lista->primero->elemento);
    free (lista->primero);
    lista->primero = NULL;
    lista->tope = NULL;
    lista->cantidad_elementos --;
    return EXITO;
  }

  nodo_t* nodo_borrador = lista->primero;

  lista->primero = lista->primero->siguiente;
  if (lista->destructor)
    lista->destructor(nodo_borrador->elemento);
  free (nodo_borrador);

  lista->cantidad_elementos --;

  return EXITO;
}

void* lista_primero(lista_t* lista){
  if (!lista || lista_vacia(lista))
    return FALLO_LISTA;
  else
    return lista->primero->elemento;
}

void lista_destruir(lista_t* lista){
  if (!lista)
    return;

  if (lista_vacia(lista)){
    free (lista);
    return;
  }

  nodo_t* nodo_borrador = NULL;

  while (lista->primero->siguiente != NO_TIENE_SIGUIENTE){
    nodo_borrador = lista->primero;
    lista->primero = lista->primero->siguiente;
    if (lista->destructor)
      lista->destructor(nodo_borrador->elemento);
    free (nodo_borrador);
  }
  if (lista->destructor)
    lista->destructor(lista->primero->elemento);
  free (lista->primero);
  free (lista);
}

lista_iterador_t* lista_iterador_crear(lista_t* lista){
  if (!lista || lista_vacia(lista))
    return FALLO_LISTA;

  lista_iterador_t* iterador = (lista_iterador_t*)malloc(sizeof(lista_iterador_t));
  if (!iterador)
    return ERROR_MEMORIA;

  iterador->lista = lista;
  iterador->indice = lista->primero;

  return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
  if (!iterador)
    return ERROR_ITERADOR;
  else if (!(iterador->indice))
    return ERROR_ITERADOR;

  if (iterador->indice == iterador->lista->tope)
    return ULTIMO_ELEMENTO_VALIDO;

  return iterador->indice->siguiente != NO_TIENE_SIGUIENTE;
}

void* lista_iterador_siguiente(lista_iterador_t* iterador){
  if (!iterador)
    return FALLO_ITERADOR;
  else if (!(iterador->indice))
    return FALLO_ITERADOR;
  else if (!(iterador->indice->siguiente) && iterador->indice != iterador->lista->tope)
    return FALLO_ITERADOR;

  void* elemento_actual = iterador->indice->elemento;

  if (iterador->indice == iterador->lista->tope)
    iterador->indice = NULL;
  else
    iterador->indice = iterador->indice->siguiente;

  return elemento_actual;
}

void lista_iterador_destruir(lista_iterador_t* iterador){
  free (iterador);
}

void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto){
  if (!lista || !funcion || lista_vacia(lista))
    return;

  for (int i = INICIO_ITERACION; i < lista->cantidad_elementos; i++)
    funcion(lista_elemento_en_posicion(lista, (size_t)i), contexto);
}
