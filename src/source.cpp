/*
 * source.c
 *
 *  Created on: 31 ene. 2020
 *      Author: tutoriales
 */
#include "obs.h"
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs.hpp>
#include <stdio.h>
#include <windows.h>
#include <util/darray.h>
#include <map>
#include <cstddef>
#include <string>
#define BUFSIZE 512
/*template<typename T, void addref(T), void release(T)> class OBSRef;

using OBSSource = OBSRef<obs_source_t *, obs_source_addref, obs_source_release>;/**/
static const char *cogerNombre(void *data){return "Nombre mi yo";}
static FILE *fp=NULL;
static OBSSource escena=NULL;
static void abrirLog(){
	if(fp==NULL){
		fp = fopen ("log.txt","a+");
	}
}
static void cerrarLog(){
	if(fp!=NULL){
		fclose (fp);
		fp=NULL;
	}
}
static void hacerLog(const char *texto){
	if(fp==NULL){
		abrirLog();
	}
	fprintf (fp, texto);
	fprintf (fp, "\n");
	fflush(fp);
	cerrarLog();
}
void hacerLogError(){
	LPWSTR pMessage = L"";
    const DWORD size = 100+1;
    WCHAR buffer[size];

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
					   NULL,
					   0,
					   0,
					   buffer,
					   size,
					   NULL)){
		hacerLog("ERROR NO RECUPERABLE");
		hacerLog((char *)TEXT(""+(int)GetLastError()));
		return;
	}
	hacerLog((char *)buffer);
}
static void activarFuente(obs_source_t *parent,
	       obs_source_t *child, void *param){

	/*
	struct obs_scene *scene = data;
	struct obs_scene_item *item;
	struct obs_scene_item *next;

	full_lock(scene);
	item = scene->first_item;

	while (item) {
		next = item->next;

		obs_sceneitem_addref(item);
		if (!active || os_atomic_load_long(&item->active_refs) > 0)
			enum_callback(scene->source, item->source, param);
		obs_sceneitem_release(item);

		item = next;
	}

	full_unlock(scene);
	*/
	//hacerLog(std::to_string(param).c_str());
	if(param==NULL){
		hacerLog("No hay parametros");
		return;
	}
	std::map<std::string,std::string> parametros=*((std::map<std::string,std::string>*)param);
	if(child!=NULL){
		//hacerLog("Hijo");
		hacerLog(("--"+std::string(obs_source_get_name(child))+"-- = --"+parametros["nombre"]+"--").c_str());
		if(strcmp(obs_source_get_name(child),parametros["nombre"].c_str())==0){
			hacerLog("Encontré");
			obs_source_set_enabled(child,strcmp(parametros["activar"].c_str(),"true")==0?true:false);//!obs_source_enabled(child)
			//child->hide();
		}
	}

};
static bool pararPipe=true;
DWORD WINAPI procesarHilo(LPVOID lpvParam);
DWORD WINAPI lanzarPIPE(LPVOID parametro){
	static HANDLE pipe = INVALID_HANDLE_VALUE, hiloPipe=NULL;
	//Esto solo funciona en Windows, Para otro sistema operativo habría que buscar una alternativa similar
	while(!pararPipe){

		pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\control_directo_temp"),
					  PIPE_ACCESS_DUPLEX,       // read/write access
					  PIPE_TYPE_MESSAGE |       // message type pipe
					  PIPE_READMODE_MESSAGE ,  // message-read mode
					  //PIPE_NOWAIT,                // Not blocking mode
					  PIPE_UNLIMITED_INSTANCES, // max. instances
					  BUFSIZE,                  // output buffer size
					  BUFSIZE,                  // input buffer size
					  10000,                        // client time-out
					  NULL);                    // default security attribute
		if (pipe!= INVALID_HANDLE_VALUE){
			bool conectado=ConnectNamedPipe(pipe, NULL);
			DWORD  dwThreadId = 0;
			if (conectado){
			   hiloPipe=CreateThread(
				NULL,              // no security attribute
				0,                 // default stack size
				procesarHilo,    // thread proc
				(LPVOID) pipe,    // thread parameter
				0,                 // not suspended
				&dwThreadId);      // returns thread ID
		   }else{
			   hacerLogError();
			   hacerLog("No se ha podido conectar el PIPE");
		   }
		}else{
			hacerLog("No se ha podido crear el PIPE");
		}
	}
	hacerLog("Se cierra el pipe");
	if(hiloPipe!=NULL){
		CloseHandle(hiloPipe);
	}
	if(pipe!= INVALID_HANDLE_VALUE){
		CloseHandle(pipe);
	}
	return 1;
}
void procesarComando(char *);
DWORD WINAPI procesarHilo(LPVOID lpvParam){
	   HANDLE hHeap      = GetProcessHeap();
	   TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
	   TCHAR* pchReply   = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));

	   DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	   BOOL fSuccess = FALSE;
	   HANDLE hPipe  = NULL;

	   hacerLog("Creamos el hilo");
	   // Do some extra error checking since the app will keep running even if this
	   // thread fails.

	   if (lpvParam == NULL){
	       hacerLog( "\nERROR - Pipe Server Failure:\n");
	       hacerLog( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
	       hacerLog( "   InstanceThread exitting.\n");
	       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
	       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
	       return (DWORD)-1;
	   }

	   if (pchRequest == NULL)
	   {
		   hacerLog( "\nERROR - Pipe Server Failure:\n");
		   hacerLog( "   InstanceThread got an unexpected NULL heap allocation.\n");
		   hacerLog( "   InstanceThread exitting.\n");
	       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
	       return (DWORD)-1;
	   }

	   if (pchReply == NULL)
	   {
		   hacerLog( "\nERROR - Pipe Server Failure:\n");
		   hacerLog( "   InstanceThread got an unexpected NULL heap allocation.\n");
		   hacerLog( "   InstanceThread exitting.\n");
	       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
	       return (DWORD)-1;
	   }

	   // Print verbose messages. In production code, this should be for debugging only.
	   hacerLog("InstanceThread created, receiving and processing messages.\n");

	   // The thread's parameter is a handle to a pipe object instance.

	   hPipe = (HANDLE) lpvParam;

	   // Loop until done reading
	   //while (1) {
	   // Read client requests from the pipe. This simplistic code only allows messages
	   // up to BUFSIZE characters in length.
	      fSuccess = ReadFile(
	         hPipe,        // handle to pipe
	         pchRequest,    // buffer to receive data
	         BUFSIZE*sizeof(TCHAR), // size of buffer
	         &cbBytesRead, // number of bytes read
	         NULL);        // not overlapped I/O

	      if (!fSuccess || cbBytesRead == 0)  {
	    	  //hacerLog("Hay errores a la hora de leer el dato");
	          //break;
	      }else{
	    	   procesarComando((char*)pchRequest);
	    	   pchRequest[0]='\0';
	      }
	  //}

	// Flush the pipe to allow the client to read the pipe's contents
	// before disconnecting. Then disconnect the pipe, and close the
	// handle to this pipe instance.

	   FlushFileBuffers(hPipe);
	   DisconnectNamedPipe(hPipe);
	   CloseHandle(hPipe);

	   HeapFree(hHeap, 0, pchRequest);
	   HeapFree(hHeap, 0, pchReply);

	   return 1;
}

void procesarComando(char * comando) {
	//Recibimos siempre dos letras con la acción a realizar y una segunda parte con datos.
	hacerLog("empezamos el procesamiento del comando");
	int longitudComando=(int)strlen(comando);
	hacerLog(comando);

	if(longitudComando>=4 && comando[0]=='C' && comando[1]=='E'){
		char * nombreEscena=new char[longitudComando-3+1];
		int j=0;
		for(int i=3;i<longitudComando && j<99;i++){
			nombreEscena[j++]=comando[i];
		}
		nombreEscena[j]='\0';

		struct obs_frontend_source_list list = {0};
		obs_frontend_get_scenes(&list);
		for (size_t i = 0; i < list.sources.num; i++) {
			OBSSource source = list.sources.array[i];
			if(strcmp(obs_source_get_name(source),nombreEscena)==0){
				obs_frontend_set_current_scene(source);
			}
		}

		da_free(list.sources);
		hacerLog("La escena escogida es ");
		hacerLog(nombreEscena);
		delete nombreEscena;
	}else if(longitudComando>=4 && comando[0]=='C' && comando[1]=='F'){
		//CF Nombre_funte:activar
		char * nombreFuente=new char[longitudComando-3+1];
		char * estadoFuente=new char[50];

		int j=0;
		for(int i=3;i<longitudComando && j<99 && comando[i]!=':';i++){
			nombreFuente[j++]=comando[i];
		}
		nombreFuente[j]='\0';
		int i=j+4;
		j=0;
		for(;i<longitudComando ;i++){
			estadoFuente[j++]=comando[i];
		}
		estadoFuente[j]='\0';

		std::map<std::string,std::string> *parametros=new std::map<std::string,std::string>();

		(*parametros)["nombre"]=std::string(nombreFuente);
		(*parametros)["activar"]=std::string(estadoFuente);/**/

		hacerLog("buscamos las fuentes");

		if(escena==NULL){
			escena=obs_frontend_get_current_scene();
			if(escena!=NULL){
				hacerLog(obs_source_get_name(escena));
			}else{
				hacerLog("no tengo escena");
			}
		}

		obs_source_enum_active_sources(escena,activarFuente,(void *)parametros);

		//hacerLog("La escena escogida es ");
		hacerLog(nombreFuente);
		hacerLog(estadoFuente);
		delete nombreFuente;
	}
}
static void *create(obs_data_t *settings, obs_source_t *source){
	abrirLog();
	hacerLog("\n\nCreamos el plugin");

	/*char ** nombres=obs_frontend_get_scene_names();

	int escenas=0;
	while(*nombres){
		if(nombres!=0){
			hacerLog(*(nombres));
		}else{
			hacerLog("Estoy a cero");
		}
		nombres++;
		escenas++;
	}/**/
	//hacerLog(escenas);

	escena=obs_frontend_get_current_scene();
	hacerLog("------");
	if(escena!=NULL){
		hacerLog(obs_source_get_name(escena));
	}else{
		hacerLog("no tengo escena");
	}
	pararPipe=false;
	DWORD  dwThreadId = 0;
	CreateThread(
		NULL,              // no security attribute
		0,                 // default stack size
		lanzarPIPE,    // thread proc
		NULL,    // thread parameter
		0,                 // not suspended
		&dwThreadId);



	return NULL;
}
static void show(void *data){


}
static void destroy(void *data){
	cerrarLog();

	pararPipe=true;
}

static void update(void *data, obs_data_t *settings){}
static uint32_t ancho(void *data){return 500;}
static uint32_t alto(void *data){return 100;}
static void renderizado(void *data, gs_effect_t *effect){}
/*static void enum_active_sources(void *data,
				    obs_source_enum_proc_t enum_callback,
				    void *param){

}*/
struct obs_source_info bocinaSource={};
void registrarBocina(){
	bocinaSource.id= "mi_yo";
	bocinaSource.type= OBS_SOURCE_TYPE_INPUT;
	bocinaSource.output_flags= OBS_SOURCE_AUDIO;
	bocinaSource.get_name= cogerNombre;
	bocinaSource.create= create;
	bocinaSource.destroy= destroy;
	bocinaSource.update= update;
	bocinaSource.video_render= renderizado;
	bocinaSource.get_width= alto;
	bocinaSource.get_height= ancho;
	bocinaSource.show= show;/**/

}
