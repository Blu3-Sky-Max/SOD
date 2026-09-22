#include "blu3.h"


/* 
#define DEBUG
*/

/* [option] */
struct option sod_option[] = {
        {"profile",required_argument,NULL, 'p'},
        {0},

};


/* show_case */
const char  *show_case= RED"usage:"RESET "sod --profile <profile_modes>\n" \
                          CYAN "\n<profile_modes>\n"RESET "     night  library gaming office meeting night\n";


/* [debounce timer] */
struct timespec debounce_time = {
        .tv_sec=0,        /* sec */
        .tv_nsec=300 * 1000 * 1000,      /* nano sec */


                };


/* [Apps tracking ] */
const char *sod_track_app[] = {
    "Google Chrome",
    "Firefox",
    "Boxes"
};




   /* [debounce func] */ 
static void db_s(void *data,uint64_t expirations ){ 
   sod_app_handler *sod_db=data;

  sod_db -> last_state = sod_db -> pending_state;

#ifdef DEBUG
 fprintf(stdout,"Last_state: %d pending_state:%d\n",sod_db -> last_state, sod_db -> pending_state); 
#endif
 fprintf(stdout,"Node id: %i\nNumber_of_input_port: %i\nNUmber_of_output_port: %i\nNode state:%s\nNode name:%s\nMedia class:%s\n ",\

                  sod_db -> id, sod_db -> n_input_ports,sod_db ->n_output_ports,\
		  pw_node_state_as_string(sod_db -> last_state),sod_db -> Node_name, sod_db -> Media_class);




} 


 /*   [Emit/trigger when a node as been  removed] */ 

static void sod_registry_remover(void *user_data, uint32_t id){



	 sod_info *sod_remove_data=user_data;

	for (size_t i=0; i< sod_remove_data -> n_counter; i++){ 
	      sod_app_handler *remover= & sod_remove_data -> sod_node_tracker[i]; 
              if (id == remover -> id){
#ifdef DEBUG
		     fprintf(stderr, "State id=%u %d -> %d\n", remover->id, remover->last_state, remover->pending_state);
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
          if (remover -> db_timer != NULL )
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
		 sod_remove_data -> n_counter--;

		/* turn the location to zero incase 
		 * if user pause and play within 5
		 * sec   
		 */  

            memset(remover, 0, sizeof(*remover));
	     break;


	      }

	      }

} 


/* [sode_node_param] .info */
static void sod_node_param(void *data, int seq,uint32_t id, uint32_t index, uint32_t next,const struct spa_pod *param){ 

     sod_info *sod_data=data; 

/* need some properties to get the volume and mute and unmute
 * passing the address of param to the properties here to get 
 * the object needed  
 */ 
  
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
				       fprintf(stdout,"Volume: %f\n", volume); 
				       break; 
			       }  
		       case
			      SPA_PROP_mute: { 
				     bool mute; 
				     spa_pod_get_bool(&new_prop->value , &mute); 
				     fprintf(stdout, "Mute:%s\n",mute ? "true" : "false"); 
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
 /* [sode_node_event] .info */ 
static void sod_node_info(void *user_data, const struct pw_node_info *info){ 

	 sod_app_handler *data=user_data;

	 

if (info->change_mask & PW_NODE_CHANGE_MASK_PROPS  && info->props != NULL ) { 




 const char *node_name=spa_dict_lookup(info->props, "node.name");
 const char *Media_class=spa_dict_lookup(info->props,"media.class");

  if (node_name)
	 strncpy(data -> Node_name, node_name, sizeof(data -> Node_name)); 
			if (Media_class)
		strncpy(data ->Media_class,Media_class, sizeof(data->Media_class));

		data->n_input_ports=info -> n_input_ports;	
		
		data -> n_output_ports=info -> n_output_ports; 



}
    /* check if call back change_mask is same as MASK_STATE
     * PW_NODE_CHANGE_MASK_STATE  is (1 << 2) which is bitfied
     * so if the call back is same as 0x4 and bitwise calculation
     * return  0x4 same as (1 << 2)then it hit and add timer
     *  to it */ 
  if (info -> change_mask & PW_NODE_CHANGE_MASK_STATE) 
	if ( info -> state != data -> pending_state){
	       
		data -> pending_state=info -> state;

               /* delopy the timer to enter the last state */
                pw_loop_update_timer(data -> sod_update_loop, data -> db_timer, &debounce_time,NULL,FALSE);

        }

 
} 








static const struct pw_node_events sod_node_event = { 
	PW_VERSION_NODE_EVENTS, 

	.info=sod_node_info,


        .param=sod_node_param, 
    


}; 




  /* [registry event func] + [info binding] */ 

static void sod_registry_func(void *user_data, uint32_t id, uint32_t permissions, const char *type, uint32_t version,const struct spa_dict *props){ 
     sod_info *sod_data=user_data;
    _Bool App_match, already_tracked; 
     
     if ( strcmp(type,PW_TYPE_INTERFACE_Node) == 0) { 
	      

     const char *Media_check=spa_dict_lookup(props,"media.class");
     const char *Node_name=spa_dict_lookup(props,"node.name");
    
    
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

	      for(uint32_t i=0; i<TRACK_APP_SIZE; i++) 
		      if ( sod_data->sod_node_tracker[i].id == id){ 
			     already_tracked=TRUE;  
			      break;
		      } 

	      if (already_tracked)
		      return; 
                   

	      /* [created a node bind per each app] 
	       * base on the app in conf.h  */ 
	      if (sod_data -> n_counter < TRACK_APP_SIZE) { 
		      sod_app_handler *slot_ctrl= &sod_data->sod_node_tracker[sod_data -> n_counter++];



			    slot_ctrl -> id=id;
			    slot_ctrl -> sod_node=pw_registry_bind(sod_data -> sod_registry,id, type,\
					    PW_VERSION_NODE,0);
			    slot_ctrl -> Version = version;

			    slot_ctrl -> sod_update_loop=sod_data -> sod_cached_loop; 
			     
			    /* add/set  timer per each node controling flooding */ 
			    
			   slot_ctrl -> db_timer=pw_loop_add_timer(slot_ctrl->sod_update_loop ,db_s, slot_ctrl);  
	      
	      
	      
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

sod_info sod_data; 
 pw_init(NULL, NULL);

 int char_c ,  Match_index= -1; 

if  (__argc != 3 ){  
       fprintf(stdout, show_case); 

 
       exit(EXIT_FAILURE);        
} 

if ( (char_c=getopt_long(__argc,__argv,"p:", sod_option, NULL) ) !=-1)  

	for (int i=0; i<N_PROFILE; i++)
		if (strcmp(optarg,profiles[i].P_name)==0){ 

			Match_index = i;
		       break; 
		} 	       

 if (Match_index == -1) { 
	fprintf(stdout,show_case);
       
	exit(19); 
 }



 spa_zero(sod_data); 



 sod_apply_profile(&profiles[Match_index] , &sod_data) ; 

sod_data.sod_loop=pw_main_loop_new(NULL); 
sod_data.sod_cached_loop=pw_main_loop_get_loop(sod_data.sod_loop); 
sod_data.sod_context=pw_context_new(pw_main_loop_get_loop(sod_data.sod_loop),NULL,0);
sod_data. sod_core=pw_context_connect(sod_data.sod_context,NULL,0); 

/* verify if proxy is allocated  */

 if (sod_data.sod_core == NULL){ 
   fprintf(stdout,"Core is not a proxy\n"); 

    exit(9); 

 } 

sod_data.sod_registry= pw_core_get_registry(sod_data.sod_core, PW_VERSION_REGISTRY, 0);      /* registry our proxy */

pw_registry_add_listener(sod_data.sod_registry,&sod_data.sod_registry_listener, &sod_registry_events ,&sod_data);  /* add proxy to registry listener */


pw_main_loop_run(sod_data.sod_loop); 












} 
