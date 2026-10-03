#include "mic.h" 



void sod_mic_controller (const struct spa_dict *props,uint32_t id,  sod_info *user_data_) { 
         
	sod_mic mic; 
    const char *Node_name=spa_dict_lookup(props,"node.name");
   const char *Media_class=spa_dict_lookup(props,"media.class");

     if(Node_name !=NULL && strcmp(Media_class, "Audio/Source")==0 && Media_class !=NULL ){ 
          mic.mic_id = id; 

	  strncpy(mic.mic_name, Node_name, sizeof(mic. mic_name)-1); 
	     

	     sod_print("node name:%s\nmic id:%d\n",  mic.mic_name, mic.mic_id  );  

                



     }  







} 
