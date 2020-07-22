#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lista.h"
#include "abb.h"
#include "pokedex.h"

#define INICIAR_POKEDEX 'I'
#define GUARDAR_POKEDEX 'G'
#define SALIR_PROGRAMA 'S'
#define AYUDA 'H'
#define AVISTAR_POKEMON 'A'
#define EVOLUCION_POKEMON 'E'
#define CAPTURAS_RECIENTES 'C'
#define VISTAS_RECIENTES 'V'
#define INFO_ESPECIE 'M'
#define INFO_POKEMON 'P'
#define POKEDEX_ENCENDIDA true
#define POKEDEX_APAGADA false
#define ERROR -1
#define EJECUTADO true
#define NO_EJECUTADO false

/*
* Recibe un booleano para conocer el estado de la pokedex y muestra los comandos disponibles hasta el momento.
*/
void mostrar_ayuda(bool pokedex_iniciada);

/*
* Recibe un puntero a una pokedex nula y la crea. Si describir es verdadero solo muestra la información del comando.
*/
bool comando_iniciar_pokedex(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("I : Inicia la pokedex\n");
    return NO_EJECUTADO;
  }
  else if (letra != INICIAR_POKEDEX)
    return NO_EJECUTADO;

  (*pokedex) = pokedex_prender();
  if (!(*pokedex)){
    printf("Ocurrió un error al encender la pokedex, revisá el archivo pokedex.txt\n");
    return EJECUTADO;
  }
  printf("- Se encendió la pokedex -\n\n");
  return EJECUTADO;
}

/*
* Recibe 
*/
bool comando_guardar_pokedex(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("G : Guarda la información de la pokedex\n");
    return NO_EJECUTADO;
  }
  else if (letra != GUARDAR_POKEDEX)
    return NO_EJECUTADO;

  int chequeo_apagar = pokedex_apagar(*pokedex);
  if (chequeo_apagar == ERROR){
    printf("Ocurrió un error al guardar la información, por favor revisá los archivos cargados\n");
    return EJECUTADO;
  }else {
    printf("- Se guardó la información correctamente -\n\n");
    return EJECUTADO;
  }
}

bool comando_salir_programa(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("S : Apaga la pokedex (no guarda datos)\n");
    return NO_EJECUTADO;
  }
  return NO_EJECUTADO;
}

bool comando_ayuda(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("H : Mostrar comandos disponibles\n");
    return NO_EJECUTADO;
  }
  else if (letra != AYUDA)
    return NO_EJECUTADO;

  if ((*pokedex))
    mostrar_ayuda(POKEDEX_ENCENDIDA);
  else
    mostrar_ayuda(POKEDEX_APAGADA);

  return EJECUTADO;
}

bool comando_avistar_pokemon(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("A : Carga los pokemones avistados en la pokedex\n");
    return NO_EJECUTADO;
  }
  else if (!(*pokedex) || letra != AVISTAR_POKEMON)
    return NO_EJECUTADO;

  char ruta_archivo[MAX_RUTA];
  printf("Por favor ingrese el nombre del archivo si se encuentra en la misma carpeta que el programa, o la ruta del mismo en caso contrario:\n");
  scanf(" %s", ruta_archivo);

  int chequeo_avistar = pokedex_avistar(*pokedex, ruta_archivo);
  if (chequeo_avistar == ERROR){
    printf("\nOcurrió un error al avistar, por favor revisá la ruta/nombre del archivo ingresado\n\n");
    return EJECUTADO;
  }else {
    printf("\n- Se agregaron a la pokedex los pokemones avistados correctamente -\n\n");
    return EJECUTADO;
  }
}

bool comando_evolucion_pokemon(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("E : Evoluciona los pokemones capturados en la pokedex\n");
    return NO_EJECUTADO;
  }
  else if (!(*pokedex) || letra != EVOLUCION_POKEMON)
    return NO_EJECUTADO;

  char ruta_archivo[MAX_RUTA];
  printf("Por favor ingrese el nombre del archivo si se encuentra en la misma carpeta que el programa, o la ruta del mismo en caso contrario:\n");
  scanf(" %s", ruta_archivo);

  int chequeo_avistar = pokedex_evolucionar(*pokedex, ruta_archivo);
  if (chequeo_avistar == ERROR){
    printf("\nOcurrió un error al evolucionar, por favor revisá la ruta/nombre del archivo ingresado\n\n");
    return EJECUTADO;
  }else {
    printf("\n- Se evolucionaron los pokemones capturados en la pokedex -\n\n");
    return EJECUTADO;
  }
}

bool comando_capturas_recientes(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("C : Muestra el historial de pokemones capturados recientemente\n");
    return NO_EJECUTADO;
  }
  else if (!(*pokedex) || letra != CAPTURAS_RECIENTES)
    return NO_EJECUTADO;
  else if (lista_vacia((*pokedex)->ultimos_capturados)){
    printf("No se capturaron pokemones recientemente\n\n");
    return EJECUTADO;
  }

  pokedex_ultimos_capturados(*pokedex);
  printf("\n");

  return EJECUTADO;
}

bool comando_vistas_recientes(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("V : Muestra el historial de pokemones vistos recientemente\n");
    return NO_EJECUTADO;
  }
  else if (!(*pokedex) || letra != VISTAS_RECIENTES)
    return NO_EJECUTADO;
  else if (lista_vacia((*pokedex)->ultimos_vistos)){
    printf("No se avistaron pokemones recientemente\n\n");
    return EJECUTADO;
  }
  pokedex_ultimos_vistos(*pokedex);
  printf("\n");

  return EJECUTADO;
}

bool comando_info_especie(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("M : Muestra la información de la especie solicitada\n");
    return NO_EJECUTADO;
  }
  else if (letra != INFO_ESPECIE)
    return NO_EJECUTADO;
  else if (arbol_vacio((*pokedex)->pokemones)){
    printf("\nLa pokedex no tiene especies ni pokemones cargados\n\n");
    return EJECUTADO;
  }

  int numero_especie = 0;
  printf("Por favor ingresá el número de la especie por la cual deseas consultar:\n");
  scanf("%i", &numero_especie);

  printf("\n");
  pokedex_informacion(*pokedex, numero_especie, "");
  printf("\n");

  return EJECUTADO;
}

bool comando_info_pokemon(char letra, pokedex_t** pokedex, bool describir){
  if (describir){
    printf("P : Muestra la información del pokemon solicitado\n");
    return NO_EJECUTADO;
  }
  if (letra != INFO_POKEMON)
    return NO_EJECUTADO;
  else if (arbol_vacio((*pokedex)->pokemones)){
    printf("\nLa pokedex no tiene especies ni pokemones cargados\n\n");
    return EJECUTADO;
  }

  int numero_especie = 0;
  char nombre_pokemon[MAX_NOMBRE];
  printf("Por favor ingresá el número de la especie por la cual deseas consultar:\n");
  scanf("%i", &numero_especie);
  printf("\nAhora ingresá el nombre del pokemon perteneciente a esa especie por el cual deseas consultar:\n");
  scanf(" %s", nombre_pokemon);

  printf("\n");
  pokedex_informacion(*pokedex, numero_especie, nombre_pokemon);
  printf("\n");

  return EJECUTADO;
}

bool (*comandos[])(char, pokedex_t**, bool) = {comando_iniciar_pokedex, comando_guardar_pokedex, comando_info_especie, comando_info_pokemon, comando_salir_programa, comando_avistar_pokemon, comando_capturas_recientes, comando_vistas_recientes, comando_evolucion_pokemon, comando_ayuda};

void mostrar_ayuda(bool pokedex_iniciada){
  if (pokedex_iniciada){
    for (int i = 1; comandos[i]; i++)
      comandos[i](0, NULL, true);
    printf("\n");
  }else {
    printf("Por el momento estos son tus comandos disponibles:\n\n");
    printf("I : Inicia la pokedex\n");
    printf("H : Mostrar comandos disponibles\n");
    printf("S : Apaga la pokedex (no guarda datos)\n\n");
  }
}

void despachar_comando(char letra, pokedex_t** pokedex){
  bool ejecutado = false;
  for (int i = 0; !ejecutado && comandos[i]; i++)
    ejecutado = comandos[i](letra, pokedex, false);
  if (!ejecutado)
    printf("Comando inválido\n\n");
}

int main(){
  bool se_termino = false;
  bool primer_vuelta = true;
  pokedex_t* pokedex = NULL;
  char letra;

  printf("\nBienvenido, a continuación se le muestran la totalidad decomandos disponibles a lo largo del programa. Luego se le mostrarán los que tiene disponibles hasta el momento.\n\n");
  mostrar_ayuda(POKEDEX_ENCENDIDA);
  printf("\n");
  mostrar_ayuda(POKEDEX_APAGADA);
  printf("\n");

  while (!se_termino){
    printf("Ingresá un comando por favor\n");
    scanf(" %c", &letra);
    printf("\n");
    if (letra == AYUDA){
      if (primer_vuelta)
        mostrar_ayuda(POKEDEX_APAGADA);
      else
        mostrar_ayuda(POKEDEX_ENCENDIDA);
    }else if (letra == SALIR_PROGRAMA){
      printf("Gracias por utilizar la pokedex, hasta la próxima.\n");
      pokedex_destruir(pokedex);
      se_termino = true;
    }else if (letra == INICIAR_POKEDEX){
      comando_iniciar_pokedex(letra, &pokedex, false);
      primer_vuelta = false;
    }else if (!primer_vuelta){
      despachar_comando(letra, &pokedex);
    }else
      printf("Comando inválido\n\n");
  }

  return 0;
}
