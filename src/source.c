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
template<typename T, void addref(T), void release(T)> class OBSRef;

using OBSSource = OBSRef<obs_source_t *, obs_source_addref, obs_source_release>;
static const char *cogerNombre(void *data){return "Mi Bocina";}
static FILE *fp=NULL;
static void abrirLog(){
	if(fp==NULL){
		fp = fopen ("H:\\Desarrollo\\obs\\dep\\rundir\\Debug\\bin\\64bit\\log.txt","a+");
	}
}
static void cerrarLog(){
	if(fp!=NULL){
		fclose (fp);
		fp=NULL;
	}
}
static void hacerLog(char *texto){
	fprintf (fp, texto);
	fprintf (fp, "\r\n");
}
static void *create(obs_data_t *settings, obs_source_t *source){
	abrirLog();
	hacerLog("Creamos el plugin");

	char ** nombres=obs_frontend_get_scene_names();

	int escenas=0;
	while(*nombres){
		if(nombres!=0){
			hacerLog(*(nombres));
		}else{
			hacerLog("Estoy a cero");
		}
		nombres++;
		escenas++;
	}
	//hacerLog(escenas);

	OBSSource escena=obs_frontend_get_current_scene(void);

	if(escena!=NULL){
		hacerLog(obs_source_get_name(escena));
	}


	return NULL;
}
static void show(void *data){


}
static void destroy(void *data){
	cerrarLog();
}
static void update(void *data, obs_data_t *settings){}
static uint32_t ancho(void *data){return 500;}
static uint32_t alto(void *data){return 100;}
static void renderizado(void *data, gs_effect_t *effect){}
/*static void enum_active_sources(void *data,
				    obs_source_enum_proc_t enum_callback,
				    void *param){

}*/

struct obs_source_info bocinaSource= {
        .id           = "mi_bocina",
        .type         = OBS_SOURCE_TYPE_INPUT,
        .output_flags = OBS_SOURCE_AUDIO,
        .get_name     = cogerNombre,
        .create       = create,
        .destroy      = destroy,
        .update       = update,
        .video_render = renderizado,
        .get_width    = alto,
        .get_height   = ancho,
		.show	      = show,
		//.enum_active_sources=enum_active_sources;
};
