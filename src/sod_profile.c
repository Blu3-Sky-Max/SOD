
#include "blu3.h" 
#include "sink.h"


/* 
#define DEBUG 

*/


void  sod_apply_profile(const struct sod_profile *p , sod_info * data){ 

	 if ( p -> Volume >= 1 ) { 
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

 /* [set sink volume  from profile ] */ 
void sod_sink_set_volume(sod_sink *p, float watch_) { 


	if ( p != NULL && p -> sink_index >= 0 && p -> sink_device >=0 && p -> sink_ready ){


	/* [ control watch_ for lowest volume and highest volume] */ 

	if ( watch_ <= 0.015) 
	 watch_ = 0.015; 


	if ( watch_ >= 1.53 ) 
		watch_= 1.53; 

		

	uint8_t buffer[1024];

	 struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	struct  spa_pod_frame  frame[2];

   /* [ using two float cause it stereo ] */ 	

	float sod_vol[2]= { watch_, watch_}; 

	spa_pod_builder_push_object (&build_buffer, &frame[0], SPA_TYPE_OBJECT_ParamRoute, SPA_PARAM_Route);

       spa_pod_builder_add (&build_buffer, SPA_PARAM_ROUTE_index, SPA_POD_Int( p -> sink_index), \
		       
		       SPA_PARAM_ROUTE_device, SPA_POD_Int( p -> sink_device), \
		       
		       SPA_PARAM_ROUTE_save, SPA_POD_Bool (TRUE), \
		       
		       0); 		    
     
     spa_pod_builder_prop( &build_buffer, SPA_PARAM_ROUTE_props, 0 ); 
     
     spa_pod_builder_push_object(&build_buffer,&frame[1],SPA_TYPE_OBJECT_Props,SPA_PARAM_Props); 

     spa_pod_builder_add (&build_buffer,SPA_PROP_channelVolumes, SPA_POD_Array(sizeof(float),SPA_TYPE_Float, sizeof(sod_vol)/\
			     sizeof(sod_vol[0]), sod_vol), 0) ;

    spa_pod_builder_pop(&build_buffer, &frame[1]); 

   struct spa_pod *parami= spa_pod_builder_pop( &build_buffer, &frame[0]); 
  
   pw_device_set_param( p -> dev_node,SPA_PARAM_Route, 0, parami);  

       	
	}
} 
