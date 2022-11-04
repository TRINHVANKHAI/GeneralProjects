/** @mainpage alsarec - HIEC
 * Record audio PCM stereo and save as wav format
 * Run application :  ./arec_pcm_wav <duration in seconds>
 * @author TVK <kai@hinoeng.co.jp>
 * @version 1.0.0
 * Reused code from albanpeignier @ github.com alsa-record-example.c  
 * This is free software under the term of GPL
**/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include "alsarec.h"


WAVH_PCM16_T hpcm;
pthread_t t1;
pthread_t t2;
pthread_t t3;
extern alsarec_state_t alsarec_state;


void *audio_PCM_record(WAVH_FMT *ifmt) {
	  FILE *fp;
	  char* a_buffer;
	  int  a_buffer_sz;
	  char* filename = "arec_pcm_s16_le.wav";
	  int i;
	  fp = fopen(filename, "w");
	  if (!fp) {
		  perror ("Error file open");
		  abort();
	  }
	  fseek(fp, sizeof(hpcm), SEEK_SET);

	  for(;;) {
		  if (holder_get_filled_buffer(&a_buffer, &a_buffer_sz) == 0){
			  a_buffer_sz = a_buffer_sz - (a_buffer_sz % ifmt->nBlockAlign);
			  fwrite(a_buffer, a_buffer_sz, 1, fp);
			  i++;
		  } else if (alsarec_state == ALSAREC_STATE_STOP) {
			  break;
		  }

	  }

	  WAVH_CHKSZ_SET(hpcm, i*a_buffer_sz/ifmt->nBlockAlign);
	  fseek(fp, 0, SEEK_SET);
	  fwrite(&hpcm, sizeof(hpcm), 1, fp);
	  fclose(fp);
	  printf("Recorded audio as: %s \n", filename);
	  return NULL;
}

void *audio_PCM_capture (WAVH_FMT *ifmt) {

	  char* a_buffer;
	  ssize_t a_buffer_sz;
	  int sps = ifmt->nSamplesPerSec / WAVH_SPS_10MS;
	  int err = 0;
	  char *fpcmsps = malloc(ifmt->nAvgBytesPerSec/WAVH_SPS_10MS);
	  snd_pcm_t *capture_handle;
	  snd_pcm_hw_params_t *hw_params;
	  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	  if((err = snd_pcm_open (&capture_handle, DEFAULT_HWDEV, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		  fprintf(stderr, "Cannot open audio device %s (%s) \n", DEFAULT_HWDEV, snd_strerror(err));
	      exit (1);
	  }

	  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		  fprintf(stderr, "Cannot allocate hardware parameter structure (%s) \n", snd_strerror(err));

		  goto Release_Res;
	  }

	  if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
		  fprintf(stderr, "Cannot initialize hardware parameter structure(%s)\n", snd_strerror(err));
		  goto Release_Res;
	  }

	  if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		  fprintf(stderr, "Cannot set access type (%s) \n", snd_strerror(err));
	      goto Release_Res;
	  }

	  if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		  fprintf(stderr, "Cannot set sample format (%s) \n", snd_strerror(err));
		  goto Release_Res;
	  }

	  if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &ifmt->nSamplesPerSec, 0)) < 0) {
		  fprintf(stderr, "Cannot set sample rate (%s) \n", snd_strerror(err));
		  goto Release_Res;
	  }

	  if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, ifmt->nChannels)) < 0) {
		  fprintf(stderr, "Cannot set number of channel (%s) \n", snd_strerror(err));
		  goto Release_Res;
	  }


	  if((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
		  fprintf(stderr, "Cannot set hardware parameters (%s) \n", snd_strerror(err));
		  goto Release_Res;
	  }

	  if((err = snd_pcm_prepare(capture_handle)) < 0) {
		  fprintf(stderr, "Cannot prepare audio interface for use (%s) \n", snd_strerror(err));
		  goto Release_Res;
	  }

	  for(;alsarec_state;) {
		  if (alsarec_state == ALSAREC_STATE_RECORDING) {
			  holder_get_free_buffer(&a_buffer, &a_buffer_sz);
		  } else if (alsarec_state == ALSAREC_STATE_PAUSED) {
			  a_buffer = fpcmsps;
		  }
		  if ((err = snd_pcm_readi(capture_handle, a_buffer, sps)) != sps) {

		      fprintf(stderr, "read from audio interface failed (%s) \n", snd_strerror(err));
		      goto Release_Res;
		  }

	  }

Release_Res:
	  snd_pcm_hw_params_free(hw_params);
	  snd_pcm_close(capture_handle);

	  printf("Free resource and exit\n");
	  return NULL;
}





int audio_setMasterVolume (uint32_t volume) {
  long min, max;

  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  const char* card = "default";
  const char* selem_name = "Master";

  snd_mixer_open(&handle, 0);
  snd_mixer_attach(handle, card);
  snd_mixer_selem_register(handle, NULL, NULL);
  snd_mixer_load(handle);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
  snd_mixer_selem_set_playback_volume_all(elem, volume*max/100);
  snd_mixer_close(handle);
  return 0;
}



#if NO_BUFFER
/**
 * Main class of project alsarec
 *
 * @param argc the number of arguments
 * @param argv the arguments from the commandline
 * @returns exit code of the application
 */

WAVH_PCM16_T hpcm;
struct timeval tv_begin, tv_end;
int main (int argc, char* argv[]) {
  WAVH_INIT_IFFMT(hpcm);
  WAVH_FFMT *iffmt = &hpcm.pcm_iffmt;
  WAVH_FMT  *ifmt  = &hpcm.pcm_ifmt;
  WAVH_DATA *idata = &hpcm.pcm_idata;
  FILE *fp;
  char* a_buffer;
  char* filename = "arec_pcm_s16_le.wav";
  ssize_t a_buffer_sz;
  int sps = 512;
  int err = 0;
  int i;
  int dura = 30;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
  if(argc == 2) {
	  dura = strtol(argv[1], NULL, 10) > 0 ? strtol(argv[1], NULL, 10) : 30;
  }

  ifmt->wBitsPerSample = snd_pcm_format_width(format);
  ifmt->nSamplesPerSec = 44100;
  ifmt->nChannels = 2;
  WAVH_BLALG_SET(hpcm);
  if((err = snd_pcm_open (&capture_handle, DEFAULT_HWDEV, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
	  fprintf(stderr, "Cannot open audio device %s (%s) \n", DEFAULT_HWDEV, snd_strerror(err));
      exit (1);
  }

  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
	  fprintf(stderr, "Cannot allocate hardware parameter structure (%s) \n", snd_strerror(err));

	  goto Release_Res;
  }

  if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
	  fprintf(stderr, "Cannot initialize hardware parameter structure(%s)\n", snd_strerror(err));
	  goto Release_Res;
  }

  if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
	  fprintf(stderr, "Cannot set access type (%s) \n", snd_strerror(err));
      goto Release_Res;
  }

  if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
	  fprintf(stderr, "Cannot set sample format (%s) \n", snd_strerror(err));
	  goto Release_Res;
  }

  if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &ifmt->nSamplesPerSec, 0)) < 0) {
	  fprintf(stderr, "Cannot set sample rate (%s) \n", snd_strerror(err));
	  goto Release_Res;
  }

  if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, ifmt->nChannels)) < 0) {
	  fprintf(stderr, "Cannot set number of channel (%s) \n", snd_strerror(err));
	  goto Release_Res;
  }


  if((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
	  fprintf(stderr, "Cannot set hardware parameters (%s) \n", snd_strerror(err));
	  goto Release_Res;
  }

  if((err = snd_pcm_prepare(capture_handle)) < 0) {
	  fprintf(stderr, "Cannot prepare audio interface for use (%s) \n", snd_strerror(err));
	  goto Release_Res;
  }
  a_buffer_sz = sps * ifmt->nBlockAlign;
  a_buffer = malloc(a_buffer_sz);
  memset(a_buffer, 0x55, a_buffer_sz);

  fp = fopen(filename, "w");
  if (!fp) {
	  perror ("Error file open");
	  abort();
  }
  fseek(fp, sizeof(hpcm), SEEK_SET);

  //for (i = 0; i<10; i++) {
  gettimeofday(&tv_begin, NULL);
  printf("Recording WAV audio: PCM Signed %d bits, Little endian, %d channels\n", ifmt->wBitsPerSample, ifmt->nChannels);
  printf("Please wait until program to stop after %d secs  \n", dura);
  for(i=0;;i++) {
	  if ((err = snd_pcm_readi(capture_handle, a_buffer, sps)) != sps) {
		  //snd_pcm_prepare(capture_handle);
	      fprintf(stderr, "read from audio interface failed (%s) \n", snd_strerror(err));
	      goto Release_Res;
	  }

	  fwrite(a_buffer, a_buffer_sz, 1, fp);
	  gettimeofday(&tv_end, NULL);
	  if((tv_end.tv_sec - tv_begin.tv_sec) > dura) break;

  }

  WAVH_CHKSZ_SET(hpcm, sps*i);
  fseek(fp, 0, SEEK_SET);
  fwrite(&hpcm, sizeof(hpcm), 1, fp);
  fclose(fp);
  printf("Recorded audio as %s \n", filename);
Release_Res:
  snd_pcm_hw_params_free(hw_params);
  snd_pcm_close(capture_handle);
  free(a_buffer);
  printf("Free resource and exit\n");
  return 0;
}
#else

/**
 * Main class of project alsarec
 *
 * @param argc the number of arguments
 * @param argv the arguments from the commandline
 * @returns exit code of the application
 */


int main (int argc, char* argv[]) {
  WAVH_INIT_IFFMT(hpcm);

  WAVH_FMT  *ifmt  = &hpcm.pcm_ifmt;

  ssize_t i_buffer_sz;

  int i=1;
  int dura = 30;
  int sps = 512;
  alsarec_state = ALSAREC_STATE_RECORDING;
  ifmt->wBitsPerSample = 16;
  ifmt->nSamplesPerSec = 44100;
  ifmt->nChannels = 2;

  WAVH_BLALG_SET(hpcm);

  i_buffer_sz = (ifmt->nSamplesPerSec / WAVH_SPS_10MS) * ifmt->nBlockAlign ;
  holder_allocate_buffer(i_buffer_sz, WAVH_SPS_10MS);
  alsarec_init_signal();
  printf("Recording WAV audio: PCM Signed %d bits, Little endian, %d channels\n", ifmt->wBitsPerSample, ifmt->nChannels);
  printf("Press Ctr+C to stop recording\n");

  pthread_create (&t1, NULL, audio_PCM_capture, ifmt);
  pthread_create (&t2, NULL, audio_PCM_record, ifmt);
  pthread_create (&t3, NULL, alsarec_key_event, ifmt);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  holder_deallocate_buffer();

  return 0;
}


#endif



