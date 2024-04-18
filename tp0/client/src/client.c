#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info (logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();


	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config

	log_info(logger, "LEI LA IP %s y el puerto %s",ip, puerto);


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje("Hola servidor soy el cliente", conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
	printf("FIN DEL PROGRAMA");
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("cliente.log", "CLIENTE.TP0",1,LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		perror ("NO SE PUDO CARGAR EL LOG");
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");
	if(nuevo_config == NULL)
	{
		perror ("NO SE PUDO CARGAR EL CONFIG");
		exit(EXIT_FAILURE);
	}

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("Valor a loggear: ");
	log_info(logger, "Log desde consola: %s", leido);

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strcmp(leido, "\0") != 0)
	{
	leido = readline("Valor a loggear: ");
	log_info(logger, "Log desde consola: %s", leido);
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	while(1)
	{
		leido = readline("> ");

		agregar_a_paquete(paquete, leido, strlen(leido) + 1);

		if(string_is_empty(leido))
		{
			free(leido);
			break;
		}
	}

	enviar_paquete(paquete, conexion);
	
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
	eliminar_paquete(paquete);

}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */

	 log_destroy(logger);
	 config_destroy(config);
}


t_log* log_create(char* file, char *program_name, bool is_active_console, t_log_level detail) {
	t_log* logger = malloc(sizeof(t_log));

	if (logger == NULL) {
		perror("Cannot create logger");
		return NULL;
	}

	FILE *file_opened = NULL;

	if (file != NULL) {
		file_opened = txt_open_for_append(file);

		if (file_opened == NULL) {
			perror("Cannot create/open log file");
			free(logger);
			return NULL;
		}
	}

	logger->file = file_opened;
	logger->is_active_console = is_active_console;
	logger->detail = detail;
	logger->pid = process_getpid();
	logger->program_name = string_duplicate(program_name);
	return logger;
}