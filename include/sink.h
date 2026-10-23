



#ifndef  AUDIO_SINK__ 

#define AUDIO_SINK__ 

#include "blu3.h" 


#define SINK_MAX  16 


typedef struct sink { 


   uint32_t  id ;                  /* node id */ 

   char Node_name [128];            /* Node name */ 

   char Node_description[512];       /* node description */ 
   
   uint64_t device_id;             /* sink id */ 
   
   struct pw_device *dev_node;         /* sink object */ 
   
   struct spa_hook  dev_listener;        /* sink event subcriber */

   int32_t sink_index; 

   int32_t sink_device;  

   _Bool    sink_ready; 


   struct sod_info_s *parent; 


}sod_sink;

void sod_sink_reset ( sod_sink *set);

void Audio_sink ( const struct spa_dict *, uint32_t id, sod_info *data); 

void sod_sink_set_volume(sod_sink *p, float watch_); 


 void sod_sink_remove(void *data, uint32_t id); 

#endif /* [AUDIO_SINK__] */ 
