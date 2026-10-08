#include "mic.h" 



  /* [debounce func] */ 
static void mic_db_func(void *data,uint64_t exp ) { 

         sod_mic *sod_mic=data;

	if ( sod_mic -> mic_pending_state != sod_mic -> mic_last_state){  
                 sod_mic -> mic_last_state = sod_mic -> mic_pending_state; 

	  sod_print("Mic_id : %u\nMic_state:%s\nMic name:%s\nMic description:%s\n\n"\
			  ,sod_mic ->mic_id,pw_node_state_as_string( sod_mic -> mic_last_state) ,sod_mic ->\
			  mic_name, sod_mic -> mic_description);


	} 



} 
static void sod_mic_event_info(void *data, const struct pw_node_info *info) { 

             sod_mic *mic_data=data; 

	     if ( info -> change_mask & PW_NODE_CHANGE_MASK_STATE && mic_data -> mic_pending_state != info -> state) { 

		     mic_data -> mic_pending_state= info -> state;

		     /* [use same timer of the app to mic] */ 
		     pw_loop_update_timer(mic_data -> sod_mic_loop, mic_data -> mic_db_timer, &debounce_time,\
				  NULL,FALSE);    

	     } 



} 

struct pw_node_events sod_mic_event = { 
	PW_VERSION_NODE_EVENTS, 

	.info= sod_mic_event_info, 
}; 



void sod_mic_controller (const struct spa_dict *props,uint32_t id,  sod_info *user_data, const char *type) { 
         
	sod_info *user_data_=user_data; 

    const char *Node_name=spa_dict_lookup(props,PW_KEY_NODE_NAME);
   const char *Media_class=spa_dict_lookup(props,PW_KEY_MEDIA_CLASS);
   const char *Node_description=spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION); 

     if(Node_name !=NULL && Media_class != NULL && strcmp(Media_class, "Audio/Source")==0  && \
      Node_description != NULL  ){ 


	     for  ( uint32_t i=0; i< user_data_ -> mic_count; i++) 
		     if ( id == user_data_ -> mics[i] -> mic_id)
			     return;
 
	     /* [check max grow] */
	     if ( user_data_ -> mic_count >= MIC_MAX_SIZE) 
		     return; 
          

         sod_mic *p_mic= calloc ( 1,  sizeof(*p_mic)); 

          if (p_mic == NULL)
	    return; 

                       /* [sync mic_node] */ 
         p_mic -> mic_node =pw_registry_bind( user_data_ -> sod_registry,id, type,PW_VERSION_NODE,0);  	  
          if ( ! p_mic -> mic_node ){ 
		 sod_error("Sorry can't Sync p_mic -> mic_node"); 
		 
		free (p_mic); 
		  return; 
	  }

	  if (Node_name) 

	  strncpy(p_mic -> mic_name, Node_name, sizeof(p_mic -> mic_name) -1);

          if (Node_description) 
     strncpy(p_mic -> mic_description, Node_description, sizeof(p_mic ->mic_description ) -1);

 	  p_mic -> mic_id = id;

	  p_mic -> sod_mic_loop= user_data_ -> sod_cached_loop;
	  
	  
	  
	  p_mic -> mic_parent = user_data; 

	 /* [add debounce timer for each mic node] */ 

	 p_mic -> mic_db_timer = pw_loop_add_timer(p_mic -> sod_mic_loop, mic_db_func, p_mic);  

	  user_data_ -> mics[user_data_ -> mic_count++] = p_mic;


	  pw_node_add_listener ( p_mic -> mic_node, &p_mic -> mic_node_listener, &sod_mic_event,p_mic);   

	  
          /* [set profile mic] */


          sod_mic_apply_profile(p_mic, user_data_ -> T_mic_on);



#ifdef DEBUG 

	     sod_print("Mic id: %d\nMic name: %s\nMic description: %s\n\n", p_mic -> mic_id,\
		     p_mic -> mic_name, p_mic -> mic_description );  
#endif

     }  

}


     /* [mic size remover] + [mic_size resetter] */ 

void sod_mic_remover (void *data, uint32_t id){
        sod_info *sod_remove_data=data;	

        for (uint32_t i=0; i < sod_remove_data -> mic_count; i++) {

                   sod_mic *remover_mic = sod_remove_data -> mics[i];



                   if ( remover_mic -> mic_id == id){


                sod_error("removing -> mic id:%i mic name: %s\n", remover_mic -> mic_id, remover_mic -> mic_name);
                      if ( remover_mic -> mic_db_timer) 
			       pw_loop_destroy_source(remover_mic ->sod_mic_loop, remover_mic ->mic_db_timer);

		 spa_hook_remove(&remover_mic ->  mic_node_listener);
		 pw_proxy_destroy((struct pw_proxy *)remover_mic ->mic_node);


                           uint32_t m_resize= sod_remove_data -> mic_count -1;

                           if( i != m_resize)
                                  sod_remove_data -> mics[i]= sod_remove_data -> mics[m_resize];

			   sod_remove_data -> mics[m_resize]=NULL; 

                          memset(remover_mic, 0, sizeof(*remover_mic));

			  free(remover_mic); 

                        sod_remove_data -> mic_count --;
			break; 

                   }


	} 
} 


