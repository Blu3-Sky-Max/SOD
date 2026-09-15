#include <pipewire/pipewire.h> 
#include "blu3.h"


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
		     } 
     } 




}

 /* [sode_node_event] .info */ 
static void sod_node_info(void *user_data, const struct pw_node_info *info){ 


	sod_info *data=user_data;

  fprintf(stdout,"Node id: %i\nNumber_of_input_port: %i\nNUmber_of_output_port: %i\nNode state:%s\n ", info -> id,\
		  info -> n_input_ports,info ->n_output_ports, pw_node_state_as_string(info->state),  );   	  




} 






static const struct pw_node_events sod_node_event = { 
	PW_VERSION_NODE_EVENTS, 

	.info=sod_node_info,


        .param=sod_node_param, 
    


}; 




  /* [registry event func] + [info getting] */ 

static void sod_registry_func(void *user_data, uint32_t id, uint32_t permissions, const char *type, uint32_t version,const struct spa_dict *props){ 
     sod_info *sod_data=user_data; 
     
      if(sod_data -> sod_info != NULL) 
	      return; 

       /* when call back hit the interface then it bind */ 
      if (strcmp(type,PW_TYPE_INTERFACE_Node) == 0){
	      sod_data -> sod_node=pw_registry_bind(sod_data -> sod.registry,id, type,PW_VERSION_NODE,0);
	      
	      
	      
	      pw_node_add_listener(sod_data -> sod_node, &sod_data -> sod_node_hook, &sod_node_event, sod_data); 
      } 

} 



 /* [registry event] */ 
static const struct pw_registry_events  sod_registry_events = { 

	PW_VERSION_REGISTRY_EVENTS, 

	.global=sod_registry_func, 




};







int main(int __argc, char *__argv[]) { 

sod_info sod_data; 
 pw_init(&__argc, &__argv); 

sod_data. sod_loop=pw_main_loop_new(NULL); 
sod_data.sod_context=pw_context_new(pw_main_loop_get_loop(sod_data.sod_loop),NULL,0);
sod_data. sod_core=pw_context_connect(sod_data.sod_context,NULL,0); 

/* verify if proxy is filed up */

 if (sod_data.sod_core == NULL){ 
   fprintf(stdout,"Core is not a proxy\n"); 

    exit(9); 

 } 

sod_data.sod_registry= pw_core_get_registry(sod_data.sod_core, PW_VERSION_REGISTRY, 0);      /* registry our proxy */

pw_registry_add_listener(sod_data.sod_registry,&sod_data.sod_registry_listener, &sod_registry_events ,&sod_data);  /* add proxy to registry listener */


pw_main_loop_run(sod_data.sod_loop); 












} 
