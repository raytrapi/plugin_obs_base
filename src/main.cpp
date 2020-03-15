/*
 * main2.cpp
 *
 *  Created on: 29 ene. 2020
 *      Author: tutoriales
 */
#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs_bocina", "es-ES")

//extern struct obs_output_info  bocinaOutput;
extern struct obs_source_info bocinaSource;
void registrarBocina();
bool obs_module_load(void){
	registrarBocina();
	obs_register_source(&bocinaSource);
	//obs_register_output(&bocinaOutput);
	return true;
}
