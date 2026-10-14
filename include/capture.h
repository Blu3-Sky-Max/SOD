

#ifndef SOD__CAPTURED__ 

#define  SOD__CAPTURED__
#include  <time.h> 



struct sod_info_s;

void sod_mic_capture(const struct spa_dict *props,uint32_t id, struct sod_info_s *user_data, const char *type); 

typedef struct { 

	uint32_t id;


	char Node_name[256]; 

	char Media_class[256]; 

	char  Application_id[512];

	char Media_role [512]; 

	char Monitor[1000];
	
	enum pw_node_state capture_last_state;

	enum pw_node_state capture_pending_state;

	struct pw_loop * sod_capture_loop;

	struct pw_node * sod_capture_node; 

	struct spa_source *capture_db_timer; 

	struct spa_hook sod_capture_listener;
	
	struct sod_info_s *capture_parent; 





}sod_capture; 




/* size of capture */ 

#define CAP_MAX_SIZE 15 

void capture_db_func (void *data_,uint64_t exp ); 

void sod_auto_unmute_a ( struct sod_info_s * , sod_capture * );

void sod_mute_app_for_mic( struct sod_info_s *cap, sod_capture *sod_c);

void sod_capture_remover(void *user_data, uint32_t id);


#endif
