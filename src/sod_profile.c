
#include "blu3.h" 

/* 
#define DEBUG 

*/ 
void  sod_apply_profile(const struct sod_profile *p , sod_info * data){ 

	 if ( p -> Volume > 1 ) { 
		  data -> T_mic_on= p -> mic_on;
		  data -> T_volume = p -> Volume / 100.0f; 
	 } 
	 else { 
		 data -> T_mic_on= p -> mic_on; 
		 data -> T_volume = p -> Volume;
	 } 




} 


   /* [Apply users profile volume from  config ] */ 
void sod_set_volume(sod_app_handler *sod_data, sod_info *V_info ) {

#ifdef DEBUG
        sod_error("mute status: %d ID: %i\n",V_info -> T_mic_on, sod_data -> id); 	
	 
#endif 
	 uint8_t buffer[1024]; 
	 struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	 const struct spa_pod *set_param=spa_pod_builder_add_object(&build_buffer,\
			 SPA_TYPE_OBJECT_Props, SPA_PARAM_Props,\
			 SPA_PROP_volume, SPA_POD_Float(V_info -> T_volume )
			 );
	 pw_node_set_param((struct pw_node *)sod_data -> sod_node, SPA_PARAM_Props, 0, set_param);

} 


  
          /* [Apply users profile mic from  config ] */  
void sod_mic_apply_profile(sod_mic *ptr, _Bool status ){ 

	uint8_t buffer[128];

         struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

         const struct spa_pod *set_param_mic=spa_pod_builder_add_object(&build_buffer,\
                         SPA_TYPE_OBJECT_Props, SPA_PARAM_Props,\
                         SPA_PROP_mute, SPA_POD_Bool(!status)
                         );
         pw_node_set_param(ptr -> mic_node, SPA_PARAM_Props, 0, set_param_mic);



} 


