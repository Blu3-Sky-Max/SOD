



#ifndef  AUDIO_SINK__ 

#define AUDIO_SINK__ 

#include "blu3.h" 
#include <stdlib.h>

typedef struct { 


   uint32_t  id ;                  /* node id */ 

   char Node_name [128];            /* Node name */ 

   char Node_description[512];       /* node description */ 
   
   uint64_t device_id;             /* sink id */ 
   
   struct pw_device *dev_node;         /* sink object */ 
   
   struct spa_hook  dev_listener;        /* sink event subcriber */ 





}sod_sink;


void Audio_sink ( const struct spa_dict *, uint32_t id, sod_info *data); 






#endif /* [AUDIO_SINK__] */ 
