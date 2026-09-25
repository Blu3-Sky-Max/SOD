#include "blu3.h"




 /* [ control ouptut per node ] */ 
void sod_mute_node(sod_app_handler *slot_mt, bool muted  ) { 

	 uint8_t buffer[512];
         struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
         const struct spa_pod *mute_param=spa_pod_builder_add_object(&build_buffer,\
                         SPA_TYPE_OBJECT_Props, SPA_PARAM_Props,\
                         SPA_PROP_mute,   SPA_POD_Bool(muted));
         pw_node_set_param((struct pw_node *)slot_mt -> sod_node, SPA_PARAM_Props, 0, mute_param);



} 
