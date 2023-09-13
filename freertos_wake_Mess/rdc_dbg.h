#ifndef __RDC_DBG_H__
#define __RDC_DBG_H__


typedef struct __rdc_profile_t__ {
    rdc_master_t id;
    const char *name;
} rdc_profile_t;



#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void RDC_DBG_PrintAllMasterAccessPolicy(void);
void RDC_DBG_PrintAllMemAccessPolicy(void);
void RDC_DBG_PrintAllPeriphAccessPolicy(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif
