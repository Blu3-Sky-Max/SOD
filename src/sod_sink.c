#include "sink.h" 




 /* [sink size remover] + [sink_size resetter] */

void sod_sink_remove(void *data, uint32_t id) { 

          sod_info *check=data;
	 
	  for ( uint32_t i=0; check != NULL && check ->  n_sod_sink > 0 && i < check -> n_sod_sink; i++){ 
		  
		  sod_sink *T = check  -> sod_sink[i]; 
		  
		  if ( T  &&  T->  id  == id) { 
#ifdef DEBUG
			  sod_error("REMOVING SINK ID: %i\n\n", T -> id); 
#endif 

		  if ( T -> dev_node){ 
			  spa_hook_remove(& T -> dev_listener);
			  pw_proxy_destroy((struct pw_proxy *)T -> dev_node);
		  } 

		  sod_sink_reset(T); 

		  T -> id =0; 
		  T -> device_id =0; 

		  uint32_t resize = T -> parent -> n_sod_sink - 1; 

		  if ( i != resize) 
			  check  -> sod_sink[i] = check  -> sod_sink[ resize]; 

		  check  -> sod_sink [resize] = NULL;

		  check  -> n_sod_sink--;  

		  free (T); 

		  break; 
 
	  } 



  } 

} 	

/* [emit param] */ 

static void sod_sink_param(void *data, int seq,  uint32_t id, uint32_t index, uint32_t next,const struct spa_pod *param){ 
     
	sod_sink *user_data =data;

     int32_t sod_index, sod_device, direction_;  	

	if ( SPA_PARAM_Route == id  && param != NULL  ) { 


		struct spa_pod_prop *new_prop;
		struct spa_pod_object *new_param_obj=(struct spa_pod_object *)param;


		SPA_POD_OBJECT_FOREACH(new_param_obj,new_prop){

			switch ( new_prop -> key) { 
				case  
					SPA_PARAM_ROUTE_index: { 
						int32_t sod_indx; 
						
						int check=spa_pod_get_int( &new_prop -> value,&sod_indx);

						if( check >=0) 
  
						sod_print("sod_index: %i\n", sod_indx);

						 sod_index = sod_indx; 
					
						break; 
					} 
				
				case 
					SPA_PARAM_ROUTE_device: { 
						int32_t sod_dev;

						int check=spa_pod_get_int(&new_prop -> value,&sod_dev);

						if (check >=0) 

						sod_print("sod_device: %i\n", sod_dev);

						sod_device = sod_dev; 

						break; 
					}
				case
					SPA_PARAM_ROUTE_direction: { 	
						uint32_t sod_direction; 

						if( spa_pod_get_id(&new_prop -> value, &sod_direction) >=0)  
						    sod_print("Direction: %s\n", sod_direction == SPA_DIRECTION_OUTPUT\
								    ? "OUTPUT" : "INPUT" ); 
						direction_=sod_direction; 

					sod_error("%u\n", sod_direction); 	
						break; 	
					} 
				case
					SPA_PARAM_ROUTE_props: { 
						struct spa_pod_prop *p;

			SPA_POD_OBJECT_FOREACH((struct spa_pod_object *)&new_prop->value, p){

				switch (p -> key){
					
					case
						SPA_PROP_channelVolumes: { 
							float sod_vol[32];

					uint32_t g_num=  spa_pod_copy_array(&p -> value, SPA_TYPE_Float,sod_vol,32); 

					for( int32_t i=0; i  <g_num; i++){ 
						sod_print("Volume[%u]: %f\n", i,sod_vol[i]);
					} 
					SOD_PUT;

						break; 
						} 
					case
						SPA_PROP_mute: {
							_Bool sod_sink_mt;

						int check=  spa_pod_get_bool(&p -> value,&sod_sink_mt);
						
						if ( check >=0) 
							sod_print("Mute:%s\n", sod_sink_mt ? "true": "false"); 
						break; 

						} 
				} 

				} 
			
			break; 

					}
			       case
				       SPA_PARAM_ROUTE_profile: {  
				       int32_t profile; 
				        int check=spa_pod_get_int( &new_prop -> value,&profile);

                                                if( check >=0)
#ifdef DEBUG
                                                sod_print("sod_profile: %i\n", profile);
#endif

                                                break;
                                        }





			} 


			} 



	} 

            	
              /* [ Sink Output ] */ 
    if ( sod_index  >=0 &&  sod_device >=0 && user_data  -> sink_ready != TRUE && direction_ == SPA_DIRECTION_OUTPUT){ 
	    
	    user_data -> sink_index = sod_index;

	    user_data -> sink_device = sod_device;


	    user_data -> sink_ready = TRUE; 

	    sod_sink_set_volume( user_data, user_data -> parent ->  T_volume);

    } 


} 


static void sod_sink_info (void *data, const struct pw_device_info *info){ 
        sod_sink * pw_data  = data; 




 sod_print("Device Node id:%i\nDevice Node name:%s\nDevice Node Description:%s\nDevice Id:%lu\n\n", pw_data -> id, pw_data -> Node_name, pw_data -> Node_description, pw_data -> device_id); 


   if ( info ->change_mask & PW_DEVICE_CHANGE_MASK_PARAMS ) 
	   pw_device_enum_params( pw_data -> dev_node, 0,SPA_PARAM_Route, 0, UINT32_MAX, NULL); 

} 




 /* [ Audio sink event] */ 

struct pw_device_events sod_sink_events = { 

	PW_VERSION_DEVICE_EVENTS, 

	.info=sod_sink_info, 

        .param=sod_sink_param, 

}; 



void Audio_sink ( const struct spa_dict *props, uint32_t id , sod_info *data ) { 


	const char *node_name=spa_dict_lookup(props, PW_KEY_NODE_NAME);
	
	const char *ID=spa_dict_lookup(props, PW_KEY_DEVICE_ID);

	const char *Node_D= spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION);

	const char *media_class = spa_dict_lookup(props,PW_KEY_MEDIA_CLASS);

	_Bool Match = FALSE;

	const char *card_profile= spa_dict_lookup(props,"card.profile.device"); 

 if( node_name != NULL && media_class != NULL && strcmp( media_class, "Audio/Sink") == 0 && ID !=NULL ) { 	

         for ( uint32_t i=0; data -> n_sod_sink !=0 && i < data -> n_sod_sink; i++) 
            if ( id == data -> sod_sink[i] -> id){  
	          Match=TRUE; 	 
		  break; 
	    } 

	 if (Match) 
		 return; 

	 sod_error("%s\n\n", card_profile); 

	 long unsigned S_id= strtoul(ID, NULL, 10); 
          

	 sod_sink * D_c = calloc (1, sizeof *D_c);
   

	  if ( !D_c) 
		  return; 

	  D_c -> dev_node = pw_registry_bind(data -> sod_registry,(uint32_t) S_id, PW_TYPE_INTERFACE_Device,PW_VERSION_DEVICE,0); 

	if (! D_c -> dev_node) 
	     return; 	
         D_c -> id = id; 

	 D_c -> device_id = S_id; 

	 D_c -> parent = data;

	 D_c -> sink_index = -1; 

	 D_c -> sink_device = -1; 

	 D_c -> sink_ready = FALSE; 


	 

	    if (Node_D) 
		    strncpy(D_c  -> Node_description, Node_D , sizeof(D_c -> Node_description) -1);

	    if (node_name) 
		     strncpy(D_c  -> Node_name, node_name , sizeof(D_c -> Node_name) -1);



	    pw_device_add_listener(D_c -> dev_node, & D_c -> dev_listener,&sod_sink_events, D_c); 

	    data -> sod_sink[ data -> n_sod_sink ++] = D_c;  



#ifdef DEBUG 
   sod_print("Device Node ID: %i\nDevice Node name: %s\nDevice Node Description:%s\nDevice id: %lu\n\n",id, node_name,Node_D,S_id);  	

#endif
} 




}  
