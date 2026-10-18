#include "blu3.h"
#include "mic.h"
#include "sink.h" 


/* 
#define DEBUG
*/



static void sod_newest_node(sod_app_handler  *running) {

	sod_info *newest_node= running -> parent; 

	if ( newest_node != NULL &&  running -> sod_capture_auto_mt ==  FALSE && newest_node -> capture_running == 0) { 
		sod_mute_node(running, FALSE); 
		running -> sod_auto_mt = FALSE; 
	} 

	 /* mute every other tracked node that is still running */
	for ( size_t i=0; newest_node != NULL && i< newest_node -> n_counter; i++){ 
             sod_app_handler *other = newest_node -> sod_node_tracker[i]; 

	     if(other == running) 
		     continue; 

	     if ( other -> last_state == PW_NODE_STATE_RUNNING && !other -> sod_auto_mt){ 
		     sod_mute_node(other, TRUE);
		      other -> sod_auto_mt = TRUE; 
	     } 

	} 

} 

static void sod_unmuted_auto_mt( sod_info *restore_mt){ 

	 /* [check if  non-auto-muted node is still running] */ 
  
	 for (size_t i =0; i < restore_mt ->  n_counter; i++) { 
		 sod_app_handler * chek = restore_mt -> sod_node_tracker [i]; 
		  if (chek != NULL && ! chek -> sod_auto_mt &&  chek -> last_state == PW_NODE_STATE_RUNNING ) 
			  return; 
	 }  /* [ end check non_auto_muted ] */ 



	 /* [check if auto_muted is available ]
	  * meaning check if there's any node sod 
	  * muted  */ 

	 for ( size_t i= restore_mt -> n_counter; i-- > 0;){



		 sod_app_handler * chek = restore_mt ->   sod_node_tracker [i];
		 if ( chek != NULL &&  chek -> sod_auto_mt && chek -> last_state ==PW_NODE_STATE_RUNNING \
				 && restore_mt -> sod_node_tracker[i] -> sod_capture_auto_mt == FALSE){ 
			  sod_mute_node(chek, FALSE);
			  chek -> sod_auto_mt = FALSE;
			 break;  

		 } 




	 } 





} 

/* [option] */
struct option sod_option[] = {
        {"profile",required_argument,NULL, 'p'},
        {NULL}, 

};


/* show_case */
const char  *show_case= RED"usage:"RESET "sod --profile <profile_modes>\n" \
                          CYAN "\n<profile_modes>\n"RESET "     night  library gaming office meeting night\n";


/* [debounce timer] */
struct timespec debounce_time = {
        .tv_sec=0,        /* sec */
        .tv_nsec=500 * 1000 * 1000,      /* nano sec */


                };


/* [Apps tracking ] */
const char *sod_track_app[] = {
    "Google Chrome",

    "Firefox",

    "Boxes", 

    "spotify" 
};




   /* [debounce func] */ 
static void db_s(void *data,uint64_t expirations ){ 
   sod_app_handler *sod_db=data;


      if ( sod_db -> last_state != sod_db -> pending_state){ 
	      sod_db -> last_state = sod_db -> pending_state;
    

 
#ifdef DEBUG
 sod_error("Last_state: %d pending_state:%d\n",sod_db -> last_state, sod_db -> pending_state); 
#endif
 sod_print("Node id: %i\nNumber_of_input_port: %i\nNUmber_of_output_port: %i\nNode state:%s\nNode name:%s\nMedia class:%s\nMedia name: %s\n\n",\

                  sod_db -> id, sod_db -> n_input_ports,sod_db ->n_output_ports,\

		  pw_node_state_as_string(sod_db -> last_state),sod_db -> Node_name, sod_db -> Media_class,\
		  sod_db -> media_name);

               /* [mic first check] */ 
             sod_capture_w(sod_db ->parent , sod_db);

               
             if ( sod_db -> last_state == PW_NODE_STATE_RUNNING)
		     sod_newest_node(sod_db); 
	     else 

		     sod_unmuted_auto_mt(sod_db -> parent); 
      }
      
      
      
      
      
      if (  sod_db -> pending_param){  
	      pw_node_enum_params((struct pw_node *)sod_db->sod_node, 0, SPA_PARAM_Props, 0, -1, NULL);
	      sod_db -> pending_param = FALSE;
      }  

} 
 
 
 /*   [Emit/trigger when a node as been  removed] */ 

static void sod_registry_remover(void *user_data, uint32_t id){



	 sod_info *sod_remove_data=user_data;

	for (size_t i=0; i< sod_remove_data -> n_counter; i++){ 
	      sod_app_handler *remover=  sod_remove_data -> sod_node_tracker[i]; 
              if (id == remover -> id){

#ifdef DEBUG
		     sod_error("State id=%u %d -> %d\n", remover->id, remover->last_state, remover->pending_state);
#endif 
		     /* node (Boxes) running (3) ->(1) suspend to remove.
		      * send a report back to reporter
		      *
		      * what happened is when you pause on boxes or 
		      * remove the playing sound it ignite the remover
		      * event from running instead of sending a callback 
		      * 
		      */  
             if ( remover -> last_state == PW_NODE_STATE_RUNNING) { 
		     remover -> pending_state=PW_NODE_STATE_SUSPENDED; 

			     db_s(remover,1); 
	     }
              
	      if ( remover ->  sod_auto_mt)
                      sod_mute_node( remover, FALSE);
                       

          if (remover -> db_timer != NULL)
		 /* destory timer on set on the node */  
		  pw_loop_destroy_source(remover -> sod_update_loop, remover ->db_timer);
	   
         spa_hook_remove(&remover -> sod_node_hook); 

	 /* type cast, was passed to listener as pw_node */ 
         pw_proxy_destroy((struct pw_proxy *)remover ->sod_node); 	 
	 
         

	 /* resizing the n_counter and  swap last active node
	  *  into the removed node , shrink count
	  */ 

	 size_t resize=sod_remove_data -> n_counter - 1; 

	 if (i !=resize){ 

		 sod_remove_data -> sod_node_tracker[i] = sod_remove_data -> sod_node_tracker[resize];
		 
	 } 
		
	     sod_remove_data -> sod_node_tracker[resize]=NULL ;

	     sod_remove_data -> n_counter--;

		/* turn the location to zero when emit */ 

            memset(remover, 0, sizeof(*remover));

	     free (remover); 
	     break;


	      }

	      }

	 
	  /* [mic source remover] + [resize]  */ 

	     sod_mic_remover(user_data , id); 

	     /* [mic Input remover] + [resize] */
	    sod_capture_remover(user_data, id);

	    /* [sink reset] */ 

	    sod_sink_remove(user_data, id); 


} 


/* [sode_node_param] .info */
static void sod_node_param(void *data, int seq,uint32_t id, uint32_t index, uint32_t next,const struct spa_pod *param){ 

      sod_app_handler *sod_data=data; 

/* need some properties to get the volume and mute and unmute status
 * passing the address of param to the properties here to get 
 * the info of each node   
 */

     if ( id == SPA_PARAM_Props && param != NULL ){  
  
     struct spa_pod_prop *new_prop; 
     struct spa_pod_object *new_param_obj=(struct spa_pod_object *)param; 

     /* accessing the object needed here */

      
     SPA_POD_OBJECT_FOREACH(new_param_obj,new_prop){ 

		     switch(new_prop -> key){ 
		       case
			       /*        { SPA_PROP_volume, SPA_TYPE_Float, SPA_TYPE_INFO_PROPS_BASE "volume", NULL }
				*        from props-types.h, 
				*        same to others
				*/  
			       SPA_PROP_volume: { 
				       float volume; 
				       spa_pod_get_float( &new_prop -> value,&volume);
				       sod_print("Volume: %f ID: %i\n", volume,  sod_data -> id); 

				       break; 
			       }  
		       case
			      SPA_PROP_mute: { 
				     bool mute; 
				     spa_pod_get_bool(&new_prop->value , &mute); 
				     sod_print( "Mute:%s\n\n",mute ? "true" : "false"); 
				     break; 
			      }
		      case 
			     SPA_PROP_monitorVolumes: { 
				    uint32_t size; 
				   spa_pod_get_array(&new_prop->value, &size);
				   break;   
		     } 
     } 



     } 
}

}




 /* [sode_node_event] .info */ 
static void sod_node_info(void *user_data, const struct pw_node_info *info){ 

	 sod_app_handler *data=user_data;

	 

if (info->change_mask & PW_NODE_CHANGE_MASK_PROPS  && info->props != NULL ) { 




 const char *node_name=spa_dict_lookup(info->props, "node.name");
 const char *Media_class=spa_dict_lookup(info->props,"media.class");
 const char *media_name= spa_dict_lookup(info-> props,  "media.name" );  

  if (node_name)
	 strncpy(data -> Node_name, node_name, sizeof(data -> Node_name)-1); 
			if (Media_class)
		strncpy(data ->Media_class,Media_class, sizeof(data->Media_class)-1);
			if (media_name) 
				strncpy(data -> media_name,media_name, sizeof(data -> media_name)-1); 

		data->n_input_ports=info -> n_input_ports;	
		
		data -> n_output_ports=info -> n_output_ports;

}

      /*[sending request for param changed info] */ 
  if ( info -> change_mask & PW_NODE_CHANGE_MASK_PARAMS ){ 
	 data -> pending_param= TRUE; 
	  pw_loop_update_timer(data -> sod_update_loop, data -> db_timer, &debounce_time,NULL,FALSE);

  }   


    /* check if call back change_mask is same as MASK_STATE
     * PW_NODE_CHANGE_MASK_STATE  is (1 << 2) which is bitfied
     * so if the call back is same as 0x4 and bitwise calculation
     * return  0x4 same as (1 << 2)then it hit and add timer
     *  to it */ 
  if (info -> change_mask & PW_NODE_CHANGE_MASK_STATE) 
	if ( info -> state != data -> pending_state){
	       
		data -> pending_state=info -> state;

               /* delay the timer to enter the last state */
                pw_loop_update_timer(data -> sod_update_loop, data -> db_timer, &debounce_time,NULL,FALSE);
	       
	         if (info -> state ==  PW_NODE_STATE_RUNNING ) 
		for (size_t i=0; i < data -> parent-> n_counter; i++){ 

			if ( data -> parent -> sod_node_tracker[i] -> sod_auto_mt == TRUE ) 
				continue;
                              
                      sod_app_handler *ctrl_mt = data-> parent -> sod_node_tracker[i]; 

		      if ( ctrl_mt -> id == data -> id ) 
			      continue; 


		      if ( ctrl_mt -> last_state == PW_NODE_STATE_RUNNING ){

			     sod_mute_node(ctrl_mt, TRUE);
			   ctrl_mt ->  sod_auto_mt=TRUE; 
		      } 
		}   




		} 
} 








static const struct pw_node_events sod_node_event = { 
	PW_VERSION_NODE_EVENTS, 

	.info=sod_node_info,


        .param=sod_node_param, 
    


}; 




  /* [registry event func] + [node binding] */ 

static void sod_registry_func(void *user_data, uint32_t id, uint32_t permissions, const char *type, uint32_t version,const struct spa_dict *props){ 
     sod_info *sod_data=user_data;
    _Bool App_match, already_tracked;


    
     
     if ( strcmp(type,PW_TYPE_INTERFACE_Node) == 0) { 
	      

     const char *Media_check=spa_dict_lookup(props,"media.class");
     const char *Node_name=spa_dict_lookup(props,"node.name");


         /* [microphone source] */ 
      if (Media_check != NULL && Node_name && strcmp(Media_check, "Audio/Source") == 0)
	       sod_mic_controller(props, id, sod_data, type);
           

        /* [microphone capture] */ 
    if (Media_check != NULL && Node_name != NULL && strcmp (Media_check, "Stream/Input/Audio") == 0  )
		  sod_mic_capture(props , id , sod_data , type);   
                

          /* [audio sink] */     
    if  ( Media_check !=NULL && strcmp(Media_check, "Audio/Sink") == 0) 
	    Audio_sink(props, id, sod_data);
                

     App_match=already_tracked=FALSE;

      for (int i=0; i< TRACK_APP_SIZE; i++)
	       
	     if ( Node_name != NULL && strcmp(Node_name, sod_track_app[i]) ==0){  
		    App_match=TRUE;
	    break; 
      }  
       
       /* when call back hit the interface (pipewire:inferace:node) 
	* and media_check is on real output device not dummy,  
	* node name is one of the listed app playing music
	*
	* [then it bind] 
	* 
	*/ 

      if ( Media_check != NULL && strcmp(Media_check, "Stream/Output/Audio") == 0  && \
		   App_match !=FALSE  && Node_name != NULL  ){

	      for(uint32_t i=0; i<sod_data -> n_counter; i++) 
		      if ( sod_data->sod_node_tracker[i]->id == id){ 
			     already_tracked=TRUE;  
			      break;
		      } 

	      if (already_tracked)
		      return; 
                  
	     /* [created new size if capacity is filled up] */ 

	     if ( sod_data -> n_counter >= sod_data ->  Track_Capacity){
		    size_t New_T_C, Old_T_C;

		    Old_T_C= sod_data -> Track_Capacity; 

		    New_T_C= sod_data -> Track_Capacity * 3; 

		    sod_app_handler **pass_new_stg= realloc( sod_data -> sod_node_tracker, New_T_C * sizeof(sod_app_handler) );

		    if (!pass_new_stg){ 
			   sod_error("Couldn't Reallocated A New Size\n"); 
			  return;  
		    }
                  /* realloc keeps the existing pointers but leaves the newly added
		   *  slots uninitialized, so zero only the new part [Old_T_C, New_T_C]    
		   */
		   memset(pass_new_stg + Old_T_C, 0 , (New_T_C - Old_T_C) * sizeof(sod_app_handler *));   


			 sod_data -> sod_node_tracker = pass_new_stg; 
			 sod_data -> Track_Capacity = New_T_C;  

	     }  

	      /* [created a node bind per each app] 
	       * base on the app in conf.h  */ 
	      if (sod_data -> n_counter < sod_data -> Track_Capacity) {

		       sod_app_handler *slot_ctrl=calloc(1, sizeof(* slot_ctrl)); 
		        

		         if (!slot_ctrl){
                                 sod_error("Couldn't give A size to slot_ctrl\n");
				  return; 
			 } 

		      sod_data->sod_node_tracker[sod_data -> n_counter++]= slot_ctrl;



			    slot_ctrl -> id=id;
			    slot_ctrl -> sod_node=pw_registry_bind(sod_data -> sod_registry,id, type,\
					    PW_VERSION_NODE,0);
			    slot_ctrl -> Version = version;

			    slot_ctrl -> sod_update_loop=sod_data -> sod_cached_loop; 
			     
			    /* add/set  timer per each node controling flooding */ 
			    
			   slot_ctrl -> db_timer=pw_loop_add_timer(slot_ctrl->sod_update_loop ,db_s, slot_ctrl); 

			  slot_ctrl -> parent = sod_data; 
	      
	                 slot_ctrl -> sod_auto_mt=FALSE;

			 slot_ctrl -> sod_capture_auto_mt = FALSE; 

	      pw_node_add_listener((struct pw_node *)slot_ctrl -> sod_node, &slot_ctrl -> sod_node_hook, &sod_node_event, slot_ctrl);




	      }  
      } 

} 
} 


 /* [registry event] */ 
static const struct pw_registry_events  sod_registry_events = { 

	PW_VERSION_REGISTRY_EVENTS, 

	.global=sod_registry_func,

	.global_remove=sod_registry_remover, 






};







int main(int __argc, char *__argv[]) { 

sigset_t set; 

sigemptyset(&set); 

sigaddset(&set, SIGINT); 

sigaddset(&set,SIGTERM);

pthread_sigmask(SIG_BLOCK, &set, NULL); 

sod_info sod_data; 
 pw_init(NULL, NULL);

 int char_c ,  Match_index= -1; 

if  (__argc != 3 ){  
       sod_print(show_case); 

 
       exit(INC_ARG);        
} 

if ( (char_c=getopt_long(__argc,__argv,"p:", sod_option, NULL) ) !=-1)  { 

	if ( char_c != 'p' ){ 
            sod_print(show_case); 
	    exit (BAD_OPTION); 

	} 

	for (int i=0; i<N_PROFILE; i++)
		if (strcmp(optarg,profiles[i].P_name)==0){ 

			Match_index = i;
		       break; 
		} 	       
} 


 if (Match_index == -1) { 
	sod_print(show_case);
       
	exit(UNKOWN_PROFILE); 
 }



 spa_zero(sod_data); 

 sod_data.Track_Capacity=TRACK_APP_SIZE; 
 sod_data.sod_node_tracker= calloc(sod_data.Track_Capacity, sizeof(*sod_data.sod_node_tracker) );

  if (!sod_data.sod_node_tracker){

	  sod_error("Can't Allocate The Requested  Size"); 
	  exit(ALLOC_FAIL);

  }  

sod_sink *sink_pri = calloc (1,sizeof *sink_pri); 

 if ( !sink_pri){ 
	 sod_print("Sorry can't allocate for sink\n"); 
	 
	 return ALLOC_FAIL; 
 } 

 /* index and device can be zero but can't be negative */ 

sink_pri -> sink_index = -1; 

sink_pri -> sink_device = -1; 

sink_pri -> sink_ready=FALSE; 

sink_pri -> parent= &sod_data; 


sink_pri -> device_id=0;  

sod_data.sod_sink = sink_pri; 


 sod_apply_profile(&profiles[Match_index] , &sod_data) ; 

sod_data.sod_loop=pw_main_loop_new(NULL); 
sod_data.sod_cached_loop=pw_main_loop_get_loop(sod_data.sod_loop); 
sod_data.sod_context=pw_context_new(pw_main_loop_get_loop(sod_data.sod_loop),NULL,0);
sod_data. sod_core=pw_context_connect(sod_data.sod_context,NULL,0); 

 
struct spa_source *sod_SigInt= pw_loop_add_signal(sod_data.sod_cached_loop, SIGINT, sod_signal, sod_data.sod_loop); 
struct spa_source *sod_SigTerm= pw_loop_add_signal(sod_data.sod_cached_loop, SIGTERM, sod_signal, sod_data.sod_loop);
  

/* verify if proxy is allocated  */

 if (sod_data.sod_core == NULL){ 
   sod_error("Core is not a proxy\n"); 

    exit(ALLOC_FAIL); 

 } 

sod_data.sod_registry= pw_core_get_registry(sod_data.sod_core, PW_VERSION_REGISTRY, 0);      /* registry our proxy */

pw_registry_add_listener(sod_data.sod_registry,&sod_data.sod_registry_listener, &sod_registry_events ,&sod_data);  /* add proxy to registry listener */


pw_main_loop_run(sod_data.sod_loop); 

 
spa_hook_remove(&sod_data.sod_registry_listener);
pw_proxy_destroy((struct pw_proxy *)sod_data.sod_registry);
pw_core_disconnect(sod_data.sod_core);
pw_context_destroy(sod_data.sod_context);
pw_loop_destroy_source (sod_data.sod_cached_loop, sod_SigInt); 
pw_loop_destroy_source (sod_data.sod_cached_loop, sod_SigTerm); 
 pw_main_loop_destroy ( sod_data.sod_loop); 


 free(sod_data.sod_node_tracker);
sod_data.sod_node_tracker = NULL;





}

