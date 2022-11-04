/*
 * alsarec.h
 *
 *  Created on: Feb 12, 2020
 *      Author: hinoeng
 */

#ifndef ALSAREC_H_
#define ALSAREC_H_

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "allocator.h"

#define WAVH_FMT_PCM        0x0001
#define WAVH_FMT_IEEE_FLOAT 0x0003
#define WAVH_FMT_ALAW       0x0006
#define WAVH_FMT_MULAW      0x0007
#define WAVH_FMT_EXTENSIBLE 0xfffe
#define WAVH_SPS_10MS       0x0064
#define WAVH_SPS_100MS      0x000a
#define DEFAULT_HWDEV "hw:0"

typedef struct __WAVH_FileFormat {
    uint32_t ckID;  /*RIFF*/
    uint32_t ckSize; /**/
    uint32_t wvID;  /*WAVE*/
} WAVH_FFMT;

typedef struct __WAVH_Format {
    uint32_t ckID;                /* Format ID "fmt " */
    uint32_t ckSize;              /* Chunk size 16,18 or 40 */
    uint16_t wFormatTag;          /* Format code */
    uint16_t nChannels;           /* Number of interleaved channels */
    uint32_t nSamplesPerSec;      /* Sampling rate blocks per seconds*/
    uint32_t nAvgBytesPerSec;     /* Data rate */
    uint16_t nBlockAlign;         /* Data block size (bytes)*/
    uint16_t wBitsPerSample;      /* Bits per sample */
#ifdef WAVH_ENABLED_EXTENSIBLE
    uint16_t cbSize;              /* Size of the extension 0 or 22 */
    uint16_t wValidBitsPerSample; /* Number of valid bits */
    uint32_t dwChannelMask;       /* Speaker position mask */
    uint16_t SubFormat[4];        /* GUID including data format */
#endif
} WAVH_FMT;

typedef struct __WAVH_Fact {
    uint32_t ckID;                /* Chunk ID "fact" */
    uint32_t ckSize;              /* Chunk size : minumum 4 */
    uint32_t dwSampleLength;      /* Number of samples per channel */
} WAVH_FACT;

typedef struct __WAVH_Data {
    uint32_t ckID;                /* Chunk ID "data" */
    uint32_t ckSize;              /* Chunk size n */

} WAVH_DATA;


typedef struct __WAVH_PCM {
  WAVH_FFMT pcm_iffmt;
  WAVH_FMT  pcm_ifmt;
  WAVH_DATA pcm_idata;
} WAVH_PCM16_T;

typedef struct __wavh_handle {
	blist_t *head;
	pthread_t pcm_id;
	pthread_t wrt_id;
	int buf_count;
	ssize_t buf_sz;
} wavh_handle_t;

typedef enum __alsarec_state {
	ALSAREC_STATE_STOP,
	ALSAREC_STATE_RECORDING,
	ALSAREC_STATE_PAUSED
} alsarec_state_t;

#define ALSAREC_STATE_NEG(_st) 0x3&(~_st)

#define __WAVH_IDNUM_FMT(a,b,c,d)      (d << 24 | c << 16 | b << 8 | a)
#define __WAVH_CHKID_SET(ptype,a,b,c,d) ptype.ckID = __WAVH_IDNUM_FMT(a,b,c,d)
#define __WAVH_WAVID_SET(ptype,a,b,c,d) ptype.wvID = __WAVH_IDNUM_FMT(a,b,c,d)
#define __WAVH_WAVTP_SET(ptype, t)      ptype.wFormatTag = t
#define __WAVH_PCMBPS_SET(ptype)        ptype.nBlockAlign = (((ptype.wBitsPerSample+7)/8)*ptype.nChannels)
#define WAVH_CHKID_IFFMT(ptype) __WAVH_CHKID_SET(ptype.pcm_iffmt,'R','I','F','F')
#define WAVH_CHKID_IFMT(ptype)  __WAVH_CHKID_SET(ptype.pcm_ifmt,'f','m','t',' ')
#define WAVH_CHKID_IFACT(ptype) __WAVH_CHKID_SET(ptype.pcm_ifact,'f','a','c','t')
#define WAVH_CHKID_IDATA(ptype) __WAVH_CHKID_SET(ptype.pcm_idata,'d','a','t','a')
#define WAVH_WAVID_IFFMT(ptype) __WAVH_WAVID_SET(ptype.pcm_iffmt,'W','A','V','E')
#define WAVH_PCMTP_IFMT(ptype)  __WAVH_WAVTP_SET(ptype.pcm_ifmt, WAVH_FMT_PCM)
#define WAVH_PCMBA_IFMT(ptype)  __WAVH_PCMBPS_SET(ptype.pcm_ifmt)

#define WAVH_IDNUM_RIFF 		__WAVH_IDNUM_FMT('R', 'I', 'F', 'F')
#define WAVH_IDNUM_GFMT 		__WAVH_IDNUM_FMT('f', 'm', 't', ' ')
#define WAVH_IDNUM_FACT 		__WAVH_IDNUM_FMT('f', 'a', 'c', 't')
#define WAVH_IDNUM_DATA 		__WAVH_IDNUM_FMT('d', 'a', 't', 'a')
#define WAVH_IDNUM_WAVE 		__WAVH_IDNUM_FMT('W', 'A', 'V', 'E')

#define WAVH_INIT_IFFMT(wavh)  \
  WAVH_CHKID_IFFMT(wavh);      \
  WAVH_WAVID_IFFMT(wavh);      \
  WAVH_CHKID_IFMT(wavh);       \
  WAVH_PCMTP_IFMT(wavh);       \
  WAVH_CHKID_IDATA(wavh);

#define WAVH_BLALG_SET(wavh)                           \
  wavh.pcm_ifmt.nBlockAlign = (((wavh.pcm_ifmt.wBitsPerSample+7)/8)*wavh.pcm_ifmt.nChannels);   \
  wavh.pcm_ifmt.nAvgBytesPerSec = wavh.pcm_ifmt.nBlockAlign * wavh.pcm_ifmt.nSamplesPerSec

#define WAVH_CHKSZ_SET(wavh, samples)                           \
  wavh.pcm_ifmt.ckSize  = 16;                                   \
  wavh.pcm_idata.ckSize = samples * wavh.pcm_ifmt.nBlockAlign;  \
  wavh.pcm_iffmt.ckSize = 12  + wavh.pcm_ifmt.ckSize + (wavh.pcm_idata.ckSize + 8)

void alsarec_init_signal(void);
void alsarec_key_event(void *key);
#endif /* ALSAREC_H_ */
