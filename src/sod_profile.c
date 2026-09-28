
#include "blu3.h" 

/* 
#define DEBUG 

*/ 
void  sod_apply_profile(const struct sod_profile *p , sod_info * data){ 

	 if ( p -> Volume > 1 ) { 
		  data -> T_mic_on= p -> mute;
		  data -> T_volume = p -> Volume / 100.0f; 
	 } 
	 else { 
		 data -> T_mic_on= p -> mute; 
		 data -> T_volume = p -> Volume;
	 } 




} 

void sod_set_volume(sod_app_handler *sod_data, sod_info *V_info ) {

#ifdef DEBUG
        fprintf(stdout,"mute status: %d ID: %i\n",V_info -> T_mic_on, sod_data -> id); 	
	 
#endif 
	 uint8_t buffer[1024]; 
	 struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	 const struct spa_pod *set_param=spa_pod_builder_add_object(&build_buffer,\
			 SPA_TYPE_OBJECT_Props, SPA_PARAM_Props,\
			 SPA_PROP_volume, SPA_POD_Float(V_info -> T_volume ),
			 SPA_PROP_mute,   SPA_POD_Bool(V_info -> T_mic_on));
	 pw_node_set_param((struct pw_node *)sod_data -> sod_node, SPA_PARAM_Props, 0, set_param);

} 


  
           

