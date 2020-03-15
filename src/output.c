/*
 * output.c
 *
 *  Created on: 31 ene. 2020
 *      Author: tutoriales
 */

#include <obs-module.h>

static const char *ffmpeg_output_getname(void *unused){	return "Hola que pasa";}


static void *ffmpeg_output_create(obs_data_t *settings, obs_output_t *output){}

static void ffmpeg_output_destroy(void *data){}


static void receive_video(void *param, struct video_data *frame){}
static void receive_audio(void *param, size_t mix_idx, struct audio_data *frame){}
static bool ffmpeg_output_start(void *data){	return true;}
static void ffmpeg_output_stop(void *data, uint64_t ts){}
static uint64_t ffmpeg_output_total_bytes(void *data){	return 0;}

extern struct obs_output_info bocinaOutput = {
	.id = "mi_output_bocina",
	.flags = OBS_OUTPUT_AUDIO,
	.get_name = ffmpeg_output_getname,
	.create = ffmpeg_output_create,
	.destroy = ffmpeg_output_destroy,
	.start = ffmpeg_output_start,
	.stop = ffmpeg_output_stop,
	.raw_video = receive_video,
	.raw_audio2 = receive_audio,
	.get_total_bytes = ffmpeg_output_total_bytes,
};
