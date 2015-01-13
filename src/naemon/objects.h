#ifndef _OBJECTS_H
#define _OBJECTS_H

#if !defined (_NAEMON_H_INSIDE) && !defined (NAEMON_COMPILATION)
#error "Only <naemon/naemon.h> can be included directly."
#endif

#include "common.h"
#include "objects_common.h"
#include "objects_contact.h"
#include "objects_command.h"
#include "objects_timeperiod.h"
#include "objectlist.h"

NAGIOS_BEGIN_DECL


/*************** CURRENT OBJECT REVISION **************/
#define CURRENT_OBJECT_STRUCTURE_VERSION        402     /* increment when changes are made to data structures... */
/* Nagios 3 starts at 300, Nagios 4 at 400, etc. */


/***************** INDEXES ****************/
#define NUM_HASHED_OBJECT_TYPES      8
#define NUM_OBJECT_TYPES            14
#define NUM_INDEXED_OBJECT_TYPES    12

#define OBJTYPE_HOST                 0
#define OBJTYPE_SERVICE              1
#define OBJTYPE_COMMAND              2
#define OBJTYPE_TIMEPERIOD           3
#define OBJTYPE_CONTACT              4
#define OBJTYPE_CONTACTGROUP         5
#define OBJTYPE_HOSTGROUP            6
#define OBJTYPE_SERVICEGROUP         7
#define OBJTYPE_HOSTDEPENDENCY       8
#define OBJTYPE_SERVICEDEPENDENCY    9
#define OBJTYPE_HOSTESCALATION      10
#define OBJTYPE_SERVICEESCALATION   11
#define OBJTYPE_HOSTEXTINFO         12
#define OBJTYPE_SERVICEEXTINFO      13

/*** BACKWARD COMPAT JUNK. drop this on next major release ***/
#define NUM_OBJECT_SKIPLISTS        NUM_INDEXED_OBJECY_TYPES
#define HOST_SKIPLIST               OBJTYPE_HOST
#define SERVICE_SKIPLIST            OBJTYPE_SERVICE
#define COMMAND_SKIPLIST            OBJTYPE_COMMAND
#define TIMEPERIOD_SKIPLIST         OBJTYPE_TIMEPERIOD
#define CONTACT_SKIPLIST            OBJTYPE_CONTACT
#define CONTACTGROUP_SKIPLIST       OBJTYPE_CONTACTGROUP
#define HOSTGROUP_SKIPLIST          OBJTYPE_HOSTGROUP
#define SERVICEGROUP_SKIPLIST       OBJTYPE_SERVICEGROUP
#define HOSTDEPENDENCY_SKIPLIST     OBJTYPE_HOSTDEPENDENCY
#define SERVICEDEPENDENCY_SKIPLIST  OBJTYPE_SERVICEDEPENDENCY
/****************** DATA STRUCTURES *******************/

const struct flag_map service_flag_map[9];
const struct flag_map host_flag_map[7];

/* @todo Remove typedef's of non-opaque types in next major release */
typedef struct host host;
typedef struct service service;


/* NOTIFY_LIST structure */
typedef struct notify_list {
	struct contact *contact;
	struct notify_list *next;
} notification;


/*
 * *name can be "Nagios Core", "Merlin", "mod_gearman" or "DNX", fe.
 * source_name gets passed the 'source' pointer from check_result
 * and must return a non-free()'able string useful for printing what
 * we need to determine exactly where the check was received from,
 * such as "mod_gearman worker@10.11.12.13", or "Nagios Core command
 * file worker" (for passive checks submitted locally), which will be
 * stashed with hosts and services and used as the "CHECKSOURCE" macro.
 */
struct check_engine {
	char *name;         /* "Nagios Core", "Merlin", "Mod Gearman" fe */
	const char *(*source_name)(void *);
	void (*clean_result)(void *);
};

struct check_output {
	char *short_output;
	char *long_output;
	char *perf_data;
};

/* CHECK_RESULT structure */
typedef struct check_result {
	int object_check_type;                          /* is this a service or a host check? */
	char *host_name;                                /* host name */
	char *service_description;                      /* service description */
	int check_type;					/* was this an active or passive service check? */
	int check_options;
	int scheduled_check;                            /* was this a scheduled or an on-demand check? */
	char *output_file;                              /* what file is the output stored in? */
	FILE *output_file_fp;
	double latency;
	struct timeval start_time;			/* time the service check was initiated */
	struct timeval finish_time;			/* time the service check was completed */
	int early_timeout;                              /* did the service check timeout? */
	int exited_ok;					/* did the plugin check return okay? */
	int return_code;				/* plugin return code */
	char *output;	                                /* plugin output */
	struct rusage rusage;			/* resource usage by this check */
	struct check_engine *engine;	/* where did we get this check from? */
	void *source;					/* engine handles this */
} check_result;


/* DBUF structure - dynamic string storage */
typedef struct dbuf {
	char *buf;
	unsigned long used_size;
	unsigned long allocated_size;
	unsigned long chunk_size;
} dbuf;


#define CHECK_STATS_BUCKETS                  15

/* used for tracking host and service check statistics */
typedef struct check_stats {
	int current_bucket;
	int bucket[CHECK_STATS_BUCKETS];
	int overflow_bucket;
	int minute_stats[3];
	time_t last_update;
} check_stats;


/* CONTACTGROUP structure */
typedef struct contactgroup {
	unsigned int id;
	char	*group_name;
	char    *alias;
	struct contactsmember *members;
	struct contactgroup *next;
} contactgroup;


/* CONTACTGROUPSMEMBER structure */
typedef struct contactgroupsmember {
	char    *group_name;
	struct contactgroup *group_ptr;
	struct contactgroupsmember *next;
} contactgroupsmember;


/* SERVICESMEMBER structure */
typedef struct servicesmember {
	char    *host_name;
	char    *service_description;
	struct service *service_ptr;
	struct servicesmember *next;
} servicesmember;


/* HOSTSMEMBER structure */
typedef struct hostsmember {
	char    *host_name;
	struct host    *host_ptr;
	struct hostsmember *next;
} hostsmember;


/* HOSTGROUP structure */
typedef struct hostgroup {
	unsigned int id;
	char 	*group_name;
	char    *alias;
	struct hostsmember *members;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	struct	hostgroup *next;
} hostgroup;


/* HOST structure */
struct host {
	unsigned int id;
	char    *name;
	char    *display_name;
	char	*alias;
	char    *address;
	struct hostsmember *parent_hosts;
	struct hostsmember *child_hosts;
	struct servicesmember *services;
	char    *check_command;
	int     initial_state;
	double  check_interval;
	double  retry_interval;
	int     max_attempts;
	char    *event_handler;
	struct contactgroupsmember *contact_groups;
	struct contactsmember *contacts;
	double  notification_interval;
	double  first_notification_delay;
	unsigned int notification_options;
	unsigned int hourly_value;
	char	*notification_period;
	char    *check_period;
	int     flap_detection_enabled;
	double  low_flap_threshold;
	double  high_flap_threshold;
	int     flap_detection_options;
	unsigned int stalking_options;
	int     check_freshness;
	int     freshness_threshold;
	int     process_performance_data;
	int     checks_enabled;
	const char *check_source;
	int     accept_passive_checks;
	int     event_handler_enabled;
	int     retain_status_information;
	int     retain_nonstatus_information;
	int     obsess;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	char    *icon_image;
	char    *icon_image_alt;
	char    *statusmap_image; /* used by lots of graphing tools */
	char    *vrml_image;
	int     have_2d_coords;
	int     x_2d;
	int     y_2d;
	int     have_3d_coords;
	double  x_3d;
	double  y_3d;
	double  z_3d;
	int     should_be_drawn;
	customvariablesmember *custom_variables;
	int     problem_has_been_acknowledged;
	int     acknowledgement_type;
	int     check_type;
	int     current_state;
	int     last_state;
	int     last_hard_state;
	char	*plugin_output;
	char    *long_plugin_output;
	char    *perf_data;
	int     state_type;
	int     current_attempt;
	unsigned long current_event_id;
	unsigned long last_event_id;
	unsigned long current_problem_id;
	unsigned long last_problem_id;
	double  latency;
	double  execution_time;
	int     is_executing;
	int     check_options;
	int     notifications_enabled;
	time_t  last_notification;
	time_t  next_notification;
	time_t  next_check;
	int     reserved2; /* placeholder var to not break interface, was should_be_scheduled before */
	time_t  last_check;
	time_t	last_state_change;
	time_t	last_hard_state_change;
	time_t  last_time_up;
	time_t  last_time_down;
	time_t  last_time_unreachable;
	int     has_been_checked;
	int     is_being_freshened;
	int     notified_on;
	int     current_notification_number;
	int     no_more_notifications;
	unsigned long current_notification_id;
	int     check_flapping_recovery_notification;
	int     scheduled_downtime_depth;
	int     pending_flex_downtime;
	int     state_history[MAX_STATE_HISTORY_ENTRIES];    /* flap detection */
	int     state_history_index;
	time_t  last_state_history_update;
	int     is_flapping;
	unsigned long flapping_comment_id;
	double  percent_state_change;
	int     total_services;
	unsigned long total_service_check_interval;
	unsigned long modified_attributes;
	struct command *event_handler_ptr;
	struct command *check_command_ptr;
	struct timeperiod *check_period_ptr;
	struct timeperiod *notification_period_ptr;
	struct objectlist *hostgroups_ptr;
	/* objects we depend upon */
	struct objectlist *exec_deps, *notify_deps;
	struct objectlist *escalation_list;
	struct  host *next;
	struct timed_event *next_check_event;
};


/* SERVICEGROUP structure */
typedef struct servicegroup {
	unsigned int id;
	char 	*group_name;
	char    *alias;
	struct servicesmember *members;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	struct	servicegroup *next;
} servicegroup;


/* SERVICE structure */
struct service {
	unsigned int id;
	char	*host_name;
	char	*description;
	char    *display_name;
	struct servicesmember *parents;
	struct servicesmember *children;
	char    *check_command;
	char    *event_handler;
	int     initial_state;
	double	check_interval;
	double  retry_interval;
	int	max_attempts;
	int     reserved1;   /* placeholder var to not break interface, was parallelize before */
	struct contactgroupsmember *contact_groups;
	struct contactsmember *contacts;
	double	notification_interval;
	double  first_notification_delay;
	unsigned int notification_options;
	unsigned int stalking_options;
	unsigned int hourly_value;
	int     is_volatile;
	char	*notification_period;
	char	*check_period;
	int     flap_detection_enabled;
	double  low_flap_threshold;
	double  high_flap_threshold;
	unsigned int flap_detection_options;
	int     process_performance_data;
	int     check_freshness;
	int     freshness_threshold;
	int     accept_passive_checks;
	int     event_handler_enabled;
	int	checks_enabled;
	const char *check_source;
	int     retain_status_information;
	int     retain_nonstatus_information;
	int     notifications_enabled;
	int     obsess;
	char    *notes;
	char    *notes_url;
	char    *action_url;
	char    *icon_image;
	char    *icon_image_alt;
	struct customvariablesmember *custom_variables;
	int     problem_has_been_acknowledged;
	int     acknowledgement_type;
	int     host_problem_at_last_check;
	int     check_type;
	int	current_state;
	int	last_state;
	int	last_hard_state;
	char	*plugin_output;
	char    *long_plugin_output;
	char    *perf_data;
	int     state_type;
	time_t	next_check;
	int     reserved2; /* placeholder var to not break interface, was should_be_scheduled before */
	time_t	last_check;
	int	current_attempt;
	unsigned long current_event_id;
	unsigned long last_event_id;
	unsigned long current_problem_id;
	unsigned long last_problem_id;
	time_t	last_notification;
	time_t  next_notification;
	int     no_more_notifications;
	int     check_flapping_recovery_notification;
	time_t	last_state_change;
	time_t	last_hard_state_change;
	time_t  last_time_ok;
	time_t  last_time_warning;
	time_t  last_time_unknown;
	time_t  last_time_critical;
	int     has_been_checked;
	int     is_being_freshened;
	unsigned int notified_on;
	int     current_notification_number;
	unsigned long current_notification_id;
	double  latency;
	double  execution_time;
	int     is_executing;
	int     check_options;
	int     scheduled_downtime_depth;
	int     pending_flex_downtime;
	int     state_history[MAX_STATE_HISTORY_ENTRIES];    /* flap detection */
	int     state_history_index;
	int     is_flapping;
	unsigned long flapping_comment_id;
	double  percent_state_change;
	unsigned long modified_attributes;
	struct host *host_ptr;
	struct command *event_handler_ptr;
	char *event_handler_args;
	struct command *check_command_ptr;
	char *check_command_args;
	struct timeperiod *check_period_ptr;
	struct timeperiod *notification_period_ptr;
	struct objectlist *servicegroups_ptr;
	struct objectlist *exec_deps, *notify_deps;
	struct objectlist *escalation_list;
	struct service *next;
	struct timed_event *next_check_event;
};


/* SERVICE ESCALATION structure */
typedef struct serviceescalation {
	unsigned int id;
	char    *host_name;
	char    *description;
	int     first_notification;
	int     last_notification;
	double  notification_interval;
	char    *escalation_period;
	int     escalation_options;
	struct contactgroupsmember *contact_groups;
	struct contactsmember *contacts;
	struct service *service_ptr;
	struct timeperiod *escalation_period_ptr;
} serviceescalation;

/* SERVICE DEPENDENCY structure */
typedef struct servicedependency {
	unsigned int id;
	int     dependency_type;
	char    *dependent_host_name;
	char    *dependent_service_description;
	char    *host_name;
	char    *service_description;
	char    *dependency_period;
	int     inherits_parent;
	int     failure_options;
	struct service *master_service_ptr;
	struct service *dependent_service_ptr;
	struct timeperiod *dependency_period_ptr;
} servicedependency;


/* HOST ESCALATION structure */
typedef struct hostescalation {
	unsigned int id;
	char    *host_name;
	int     first_notification;
	int     last_notification;
	double  notification_interval;
	char    *escalation_period;
	int     escalation_options;
	struct contactgroupsmember *contact_groups;
	struct contactsmember *contacts;
	struct host    *host_ptr;
	struct timeperiod *escalation_period_ptr;
} hostescalation;


/* HOST DEPENDENCY structure */
typedef struct hostdependency {
	unsigned int id;
	int     dependency_type;
	char    *dependent_host_name;
	char    *host_name;
	char    *dependency_period;
	int     inherits_parent;
	int     failure_options;
	struct host    *master_host_ptr;
	struct host    *dependent_host_ptr;
	struct timeperiod *dependency_period_ptr;
} hostdependency;

extern struct host *host_list;
extern struct service *service_list;
extern struct hostgroup *hostgroup_list;
extern struct servicegroup *servicegroup_list;
extern struct contactgroup *contactgroup_list;
extern struct hostescalation *hostescalation_list;
extern struct serviceescalation *serviceescalation_list;
extern struct host **host_ary;
extern struct service **service_ary;
extern struct hostgroup **hostgroup_ary;
extern struct servicegroup **servicegroup_ary;
extern struct contactgroup **contactgroup_ary;
extern struct hostescalation **hostescalation_ary;
extern struct hostdependency **hostdependency_ary;
extern struct serviceescalation **serviceescalation_ary;
extern struct servicedependency **servicedependency_ary;


/********************* FUNCTIONS **********************/


/* silly helpers useful pretty much all over the place */
const char *service_state_name(int state);
const char *host_state_name(int state);
/**** Top-level input functions ****/
int read_object_config_data(const char *, int);     /* reads all external configuration data of specific types */


/**** Object Creation Functions ****/
struct host *add_host(char *name, char *display_name, char *alias, char *address, char *check_period, int initial_state, double check_interval, double retry_interval, int max_attempts, int notification_options, double notification_interval, double first_notification_delay, char *notification_period, int notifications_enabled, char *check_command, int checks_enabled, int accept_passive_checks, char *event_handler, int event_handler_enabled, int flap_detection_enabled, double low_flap_threshold, double high_flap_threshold, int flap_detection_options, int stalking_options, int process_perfdata, int check_freshness, int freshness_threshold, char *notes, char *notes_url, char *action_url, char *icon_image, char *icon_image_alt, char *vrml_image, char *statusmap_image, int x_2d, int y_2d, int have_2d_coords, double x_3d, double y_3d, double z_3d, int have_3d_coords, int should_be_drawn, int retain_status_information, int retain_nonstatus_information, int obsess_over_host, unsigned int hourly_value);
struct hostsmember *add_parent_host_to_host(host *, char *);							/* adds a parent host to a host definition */
struct servicesmember *add_parent_service_to_service(service *, char *host_name, char *description);
struct hostsmember *add_child_link_to_host(host *, host *);						       /* adds a child host to a host definition */
struct contactgroupsmember *add_contactgroup_to_host(host *, char *);					       /* adds a contactgroup to a host definition */
struct contactsmember *add_contact_to_host(host *, char *);                                                    /* adds a contact to a host definition */
struct customvariablesmember *add_custom_variable_to_host(host *, char *, char *);                             /* adds a custom variable to a host definition */
struct hostgroup *add_hostgroup(char *, char *, char *, char *, char *);						/* adds a hostgroup definition */
struct hostsmember *add_host_to_hostgroup(hostgroup *, char *);						/* adds a host to a hostgroup definition */
struct servicegroup *add_servicegroup(char *, char *, char *, char *, char *);                                 /* adds a servicegroup definition */
struct servicesmember *add_service_to_servicegroup(servicegroup *, char *, char *);                            /* adds a service to a servicegroup definition */
struct contactgroup *add_contactgroup(char *, char *);								/* adds a contactgroup definition */
struct contactsmember *add_contact_to_contactgroup(contactgroup *, char *);					/* adds a contact to a contact group definition */
struct service *add_service(char *host_name, char *description, char *display_name, char *check_period, int initial_state, int max_attempts, int accept_passive_checks, double check_interval, double retry_interval, double notification_interval, double first_notification_delay, char *notification_period, int notification_options, int notifications_enabled, int is_volatile, char *event_handler, int event_handler_enabled, char *check_command, int checks_enabled, int flap_detection_enabled, double low_flap_threshold, double high_flap_threshold, int flap_detection_options, int stalking_options, int process_perfdata, int check_freshness, int freshness_threshold, char *notes, char *notes_url, char *action_url, char *icon_image, char *icon_image_alt, int retain_status_information, int retain_nonstatus_information, int obsess_over_service, unsigned int hourly_value);
struct contactgroupsmember *add_contactgroup_to_service(service *, char *);					/* adds a contact group to a service definition */
struct contactsmember *add_contact_to_service(service *, char *);                                              /* adds a contact to a host definition */
struct serviceescalation *add_serviceescalation(char *host_name, char *description, int first_notification, int last_notification, double notification_interval, char *escalation_period, int escalation_options);
struct contactgroupsmember *add_contactgroup_to_serviceescalation(serviceescalation *, char *);                /* adds a contact group to a service escalation definition */
struct contactsmember *add_contact_to_serviceescalation(serviceescalation *, char *);                          /* adds a contact to a service escalation definition */
struct customvariablesmember *add_custom_variable_to_service(service *, char *, char *);                       /* adds a custom variable to a service definition */
struct servicedependency *add_service_dependency(char *dependent_host_name, char *dependent_service_description, char *host_name, char *service_description, int dependency_type, int inherits_parent, int failure_options, char *dependency_period);
struct hostdependency *add_host_dependency(char *dependent_host_name, char *host_name, int dependency_type, int inherits_parent, int failure_options, char *dependency_period);
struct hostescalation *add_hostescalation(char *host_name, int first_notification, int last_notification, double notification_interval, char *escalation_period, int escalation_options);
struct contactsmember *add_contact_to_hostescalation(hostescalation *, char *);                                /* adds a contact to a host escalation definition */
struct contactgroupsmember *add_contactgroup_to_hostescalation(hostescalation *, char *);                      /* adds a contact group to a host escalation definition */

struct servicesmember *add_service_link_to_host(host *, service *);


int get_host_count(void);
int get_service_count(void);


int create_object_tables(unsigned int *);

/**** Object Search Functions ****/
struct host *find_host(const char *);
struct hostgroup *find_hostgroup(const char *);
struct servicegroup *find_servicegroup(const char *);
struct contactgroup *find_contactgroup(const char *);
struct service *find_service(const char *, const char *);


/**** Object Query Functions ****/
unsigned int host_services_value(struct host *h);
int is_host_immediate_child_of_host(struct host *, struct host *);	               /* checks if a host is an immediate child of another host */
int is_host_primary_immediate_child_of_host(struct host *, struct host *);            /* checks if a host is an immediate child (and primary child) of another host */
int is_host_immediate_parent_of_host(struct host *, struct host *);	               /* checks if a host is an immediate child of another host */
int is_host_member_of_hostgroup(struct hostgroup *, struct host *);		       /* tests whether or not a host is a member of a specific hostgroup */
int is_host_member_of_servicegroup(struct servicegroup *, struct host *);	       /* tests whether or not a service is a member of a specific servicegroup */
int is_service_member_of_servicegroup(struct servicegroup *, struct service *);	/* tests whether or not a service is a member of a specific servicegroup */
int is_contact_member_of_contactgroup(struct contactgroup *, struct contact *);	/* tests whether or not a contact is a member of a specific contact group */
int is_contact_for_host(struct host *, struct contact *);			       /* tests whether or not a contact is a contact member for a specific host */
int is_escalated_contact_for_host(struct host *, struct contact *);                   /* checks whether or not a contact is an escalated contact for a specific host */
int is_contact_for_service(struct service *, struct contact *);		       /* tests whether or not a contact is a contact member for a specific service */
int is_escalated_contact_for_service(struct service *, struct contact *);             /* checks whether or not a contact is an escalated contact for a specific service */

int number_of_immediate_child_hosts(struct host *);		                /* counts the number of immediate child hosts for a particular host */
int number_of_total_child_hosts(struct host *);				/* counts the number of total child hosts for a particular host */
int number_of_immediate_parent_hosts(struct host *);				/* counts the number of immediate parents hosts for a particular host */

void fcache_contactgrouplist(FILE *fp, const char *prefix, struct contactgroupsmember *list);
void fcache_hostlist(FILE *fp, const char *prefix, struct hostsmember *list);
void fcache_contactgroup(FILE *fp, struct contactgroup *temp_contactgroup);
void fcache_hostgroup(FILE *fp, struct hostgroup *temp_hostgroup);
void fcache_servicegroup(FILE *fp, struct servicegroup *temp_servicegroup);
void fcache_host(FILE *fp, struct host *temp_host);
void fcache_service(FILE *fp, struct service *temp_service);
void fcache_servicedependency(FILE *fp, struct servicedependency *temp_servicedependency);
void fcache_serviceescalation(FILE *fp, struct serviceescalation *temp_serviceescalation);
void fcache_hostdependency(FILE *fp, struct hostdependency *temp_hostdependency);
void fcache_hostescalation(FILE *fp, struct hostescalation *temp_hostescalation);
int fcache_objects(char *cache_file);


/**** Object Cleanup Functions ****/
int free_object_data(void);                             /* frees all allocated memory for the object definitions */

NAGIOS_END_DECL
#endif
