


#ifndef MIC_CONTROLLER__

#define MIC_CONTROLLER__

#include "blu3.h"


void sod_mic_apply_profile(sod_mic *ptr, _Bool status ); 

void sod_mute_app_for_mic(sod_info *mic_p, sod_mic *sod_mic);

void sod_auto_unmute_a (sod_info *mic_p, sod_mic *sod_mic);

void sod_mic_remover (void *data, uint32_t id);

void sod_mic_controller(const struct spa_dict *props, uint32_t id, sod_info *user_data_,const char *type);

#endif 
