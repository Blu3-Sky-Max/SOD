#include "blu3.h"



#define DEBUG 

/* [ control ouptut per node ] */ 
void sod_mute_node(sod_app_handler *slot_mt, bool muted  ) { 

	 uint8_t buffer[512];
         struct spa_pod_builder build_buffer = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
         const struct spa_pod *mute_param=spa_pod_builder_add_object(&build_buffer,\
                         SPA_TYPE_OBJECT_Props, SPA_PARAM_Props,\
                         SPA_PROP_mute,   SPA_POD_Bool(muted));
         pw_node_set_param((struct pw_node *)slot_mt -> sod_node, SPA_PARAM_Props, 0, mute_param);



}

   /* [ mute all app while mic running] */ 

void sod_mute_app_for_mic(sod_info *capture_p, sod_capture *cap){ 

	if ( capture_p != NULL &&  cap -> capture_last_state == PW_NODE_STATE_RUNNING ) {  

	       for ( size_t i=0; capture_p ->sod_node_tracker[i] !=NULL && i < capture_p -> n_counter; ++i)   

		      if( capture_p -> sod_node_tracker[i] -> last_state == PW_NODE_STATE_RUNNING && \
				      ! capture_p -> sod_node_tracker[i] -> sod_capture_auto_mt  ) {  
			     
			   if  ( ! capture_p -> sod_node_tracker[i] ->sod_auto_mt ){   
			              sod_mute_node( capture_p -> sod_node_tracker[i], TRUE); 
		      
			      capture_p -> sod_node_tracker[i]-> sod_capture_auto_mt= TRUE;
			   } 

		      } 
	       } 



       } 



 /* [ auto unmute all app mic muted idle/suspend] */ 
void sod_auto_unmute_a(sod_info *capture_r, sod_capture *cap_r){ 

	 if ( capture_r  != NULL && capture_r -> n_counter != 0   && cap_r ->  capture_last_state != PW_NODE_STATE_RUNNING ){ 

		 for ( size_t i=capture_r -> n_counter;  i -- > 0 ; ) { 
			 sod_app_handler *T_ctrl = capture_r -> sod_node_tracker[i];

#if defined DEBUG 
			 sod_error("Restoring Id: %u Restoring app sod_capture_auto_state: %d\n", T_ctrl -> id, T_ctrl -> sod_capture_auto_mt) ; 
#endif
		             
		       if (T_ctrl == NULL || T_ctrl -> sod_capture_auto_mt == FALSE ) 
			      continue ; 

			  T_ctrl -> sod_capture_auto_mt= FALSE;

		               

			  if ( T_ctrl -> sod_auto_mt == FALSE) 

                              sod_mute_node( T_ctrl,FALSE);

                              



                      }


            } 


} 

/* [ check if mic is running] */ 
void sod_capture_w (sod_info *p, sod_app_handler *arr){ 

	if( p != NULL && p -> capture_running > 0 && arr -> last_state == PW_NODE_STATE_RUNNING){ 
		
		if ( arr ->  sod_capture_auto_mt ) 
			return;

           sod_mute_node (arr, TRUE);
	   arr -> sod_capture_auto_mt = TRUE;

	} 
} 

