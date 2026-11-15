


#ifndef PROFILE_H__ 

#define PROFILE_H__ 

#include <getopt.h> 

#define __COLOR__
#include "color.h"

 
/*  [Operating profile] */

#ifndef  _OPERATING_PROFILE__  

#define _OPERATING_PROFILE__ 


#define _OPERATING_VOLUME       

/* volume range  25 - 153 if beyond, sod will control it */ 

/*  caculation behind this goes like: 
 *  e.g library which is 40%, 40 /100
 *  gives 0.4 the cubic root of 0.4 gives
 *  0.736806 so rounding gives 0.74  mutiply 
 *  by 100 give 74%
 *
 *
 *  if you have no calculator with you, you can 
 *  take the 0.4^(1/3) or 0.4^(0.3333) should give
 *  you 0.736806 so  rounding it up gives 0.74
 *  then mutiply by 100 gives 74%
 *
 *  to calculate it you know what to do give the 
 *  covert the raw back 0.736806 ^3 gives .39999 
 *  then we have 40 back
 *
 */ 


#define Library_Vol  40     // 74% 
#define Focus_Vol    9      //45 %
#define Office_Vol   60     // 84%
#define Night_Vol    0.015   // 25%
#define Meeting_Vol  22      // 60%      
#define Gaming_Vol   358     // 153%



struct sod_profile { 

	 const char  *P_name;     /* profile name */

	int Volume;               /* profile volume */ 

	bool mic_on;             /* profile mic setting */ 




}; 


/* [enivronment profile] */ 

static const struct sod_profile profiles[] = { \
	{ "library", Library_Vol, FALSE},    /* Library modes */ 

	{ "focus",  Focus_Vol, TRUE },     /* focus modes */ 
	{ "gaming", Gaming_Vol, TRUE	},    /* gaming modes */ 
	{ "office", Office_Vol, TRUE },   /* office modes */ 
	{ "meeting", Meeting_Vol , TRUE},  /* meeting modes */ 
	{ "night", Night_Vol, FALSE},      /* night modes */ 



	} ; 

#define N_PROFILE sizeof(profiles)/sizeof(profiles[0]) 

#endif /* _OPERATING_PROFILE__ */



/* options */ 
extern struct option sod_option[]; 

/* show_case */
extern const char  *show_case; 	 




#endif /* PROFILE_H__ */
