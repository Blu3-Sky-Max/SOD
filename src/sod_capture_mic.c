#include "blu3.h" 
#include "capture.h"


#define DEBUG	

/* ignore the pulse  volume control (flooding)  */
const char *capture_ignore = "PulseAudio Volume Control" ;


 /* [capture remover and resize] */ 
void sod_capture_remover(void *user_data, uint32_t id){ 

	sod_info *data = user_data;

   for ( uint32_t i=0; i < data -> n_capture; i++){ 
          sod_capture *rm = data -> captures[i];

	 if( !rm || rm -> id != id)
		continue; 
#ifdef DEBUG
	 sod_error("removing capture -> capture id:%i capture name: %s\n\n", rm -> id, rm -> Node_name);
#endif 
	 if ( rm -> id == id){ 

		 if (rm -> capture_last_state == PW_NODE_STATE_RUNNING &&  data -> capture_running > 0){ 
			rm -> capture_last_state = PW_NODE_STATE_SUSPENDED; 
		       
			if (data -> capture_running > 0) 
				
				data -> capture_running--; 
			
			if ( data -> capture_running == 0) 	       
				sod_auto_unmute_a(data, rm);
		 } 

                  if ( rm -> capture_db_timer) 
			  pw_loop_destroy_source( rm -> sod_capture_loop , rm  -> capture_db_timer);

                 spa_hook_remove(&rm ->  sod_capture_listener);
                 pw_proxy_destroy((struct pw_proxy *)rm -> sod_capture_node);


		 uint32_t m_resize= data -> n_capture -1;

		  if ( i != m_resize) 

			  data -> captures[i] = data -> captures[m_resize]; 
		  data -> captures[m_resize] = NULL; 

		   memset(rm, 0, sizeof(*rm));

		   free (rm);

		  data -> n_capture --; 
		  break; 
	 }  

   } 
}    





struct timespec capture_debounce_time = { 
	.tv_sec=0, 
	.tv_nsec= 300 * 1000 *1000, 

}; 


static void capture_db_func (void *data_,uint64_t exp ){
      
       	sod_capture *data= data_;	

           if ( data -> capture_last_state != data -> capture_pending_state ) { 

		   /* first state is to keep track of running -> id -> running
		    * so capturing running does not ++ for same node  */ 

		   int first_state =  data -> capture_last_state; 

		   data -> capture_last_state = data -> capture_pending_state;

		  sod_print("Node capture name: %s\nNode capture role:%s\nNode capture media_class: %s\nNode id:%u\nNode app Id: %s\nNode capture Last_state: %s\nMonitor:%s\n\n"
				  , data -> Node_name, data -> Media_role, data -> Media_class, data -> id,\
				  data -> Application_id, pw_node_state_as_string(data -> capture_last_state), data -> Monitor );

	    if ( data -> capture_last_state == PW_NODE_STATE_RUNNING && first_state != PW_NODE_STATE_RUNNING ){ 
		  data -> capture_parent -> capture_running++; 

		    sod_mute_app_for_mic(data -> capture_parent, data);
	    } 

	    else
		    if (first_state == PW_NODE_STATE_RUNNING && data -> capture_last_state != PW_NODE_STATE_RUNNING){

			   if( data -> capture_parent -> capture_running > 0) 
			   data -> capture_parent -> capture_running--; 

			  if ( data -> capture_parent  -> capture_running == 0)  
				  sod_auto_unmute_a(data -> capture_parent, data);  
	  
	   }   

     } 

} 
static void sod_capture_event_info (void *data, const struct pw_node_info *info) { 
              
	sod_capture *user_data = data; 


	if ( info -> change_mask & PW_NODE_CHANGE_MASK_STATE && info -> state != user_data -> capture_pending_state){  
		user_data -> capture_pending_state  = info -> state; 

                pw_loop_update_timer(user_data -> sod_capture_loop,user_data -> capture_db_timer, &capture_debounce_time,NULL, FALSE);



	} 





} 



const struct pw_node_events sod_capture_event = {
        PW_VERSION_NODE_EVENTS,

        .info= sod_capture_event_info,
};


void sod_mic_capture(const struct spa_dict *props,uint32_t id,  sod_info *user_data, const char *type) { 

	 const char *Node_name= spa_dict_lookup(props,PW_KEY_NODE_NAME);

	 const char *Media_class= spa_dict_lookup(props,PW_KEY_MEDIA_CLASS);

	 const char *APP_ID= spa_dict_lookup(props, PW_KEY_APP_ID);

	 const char *media_role= spa_dict_lookup (props, PW_KEY_MEDIA_ROLE );

	 const char *stream_monitor = spa_dict_lookup  (props, PW_KEY_STREAM_MONITOR); 
                 
	  if (Node_name !=NULL && Media_class !=NULL && strcmp("Stream/Input/Audio", Media_class) == 0 &&\
	   ! strcmp(Node_name, capture_ignore) == 0 ) { 

		 for ( uint32_t i=0; i < user_data -> n_capture; i++) 
			
			if (  user_data -> captures[i] != NULL && user_data -> captures[i] -> id == id ) 
			      return; 

		 /* [check max size] */ 

		 if ( user_data -> n_capture >= CAP_MAX_SIZE) 
			 return ; 

		 sod_capture *cap= calloc (1, sizeof (*cap) ) ; 

		 if (!cap) 
			 return; 
                     
		 cap -> sod_capture_node = pw_registry_bind( user_data -> sod_registry, id , type,PW_VERSION_NODE,0);
		        
		   if ( ! cap -> sod_capture_node) { 
			   sod_error("Sorry can't Sync cap -> sod_capture_node");
			   
			   free(cap); 
			   return; 
		   } 

		   if ( Node_name ) 
			   strncpy(cap -> Node_name, Node_name, sizeof( cap -> Node_name) -1 ); 
		   if ( Media_class ) 
			   strncpy ( cap -> Media_class, Media_class , sizeof (cap -> Media_class)-1 ); 
		   if ( APP_ID) 
			  strncpy ( cap -> Application_id, APP_ID, sizeof ( cap -> Application_id) -1 ); 
		   if (media_role) 
			  strncpy (cap -> Media_role, media_role, sizeof(cap -> Media_role)-1 );  

		   if (stream_monitor) 
			   strncpy (cap -> Monitor, stream_monitor, sizeof(cap -> Monitor)-1 );




		 cap -> id = id;

		 cap -> sod_capture_loop = user_data -> sod_cached_loop; 

		 cap -> capture_db_timer = pw_loop_add_timer(cap -> sod_capture_loop, capture_db_func, cap);

		 cap -> capture_parent = user_data; 
		 
                 cap -> capture_pending_state = cap -> capture_last_state =  PW_NODE_STATE_CREATING; 

		 user_data -> captures[ user_data -> n_capture++]= cap; 
		 
		 pw_node_add_listener ( cap -> sod_capture_node, &cap -> sod_capture_listener, &sod_capture_event,cap); 



    } 

}




