#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"




//executes the ones that does not have any dependencies and goes up.
void execute_all_from_lowest(Rule *rule) {
	Dependency *dep = rule->dependencies;
	Action *action_ptr = rule->actions;


	while (dep != NULL) {
		if (dep->rule->dependencies != NULL) {
			execute_all_from_lowest(dep->rule);		
		} 
		dep = dep->next_dep;
	}
	
	while (action_ptr != NULL) {
		int result = fork();
		if (result == 0) {
			if (execvp((action_ptr->args)[0], action_ptr->args) == -1) {
				exit(1);
			} 
		} else {		
			int status;
			waitpid(result, &status, 0);
			status = WEXITSTATUS(status); 
			if (status == 1) {
				exit(1);
			}
			action_ptr = action_ptr->next_act;
		}
	}
}

//Returns the rule having the dependency we are looking for in 
//the dependencies list.
Rule *rule_pointing_dep(Rule *from, char *target) {
	Dependency *dep_ptr = from->dependencies;
	
	while (dep_ptr != NULL) {
		if (target == dep_ptr->rule->target) { //comparing the depency to target	
			Rule *to_return = malloc(sizeof(Rule));	
			to_return = from;		
			return to_return;
		} else {
			rule_pointing_dep(from->dependencies->rule, target);
		}
		dep_ptr = dep_ptr->next_dep;
	}
	return NULL;
}

//Execute a single rule's action.
void execute_actions(Rule *rule) {
	Action *action_ptr = rule->actions;
	while (action_ptr != NULL) {
		int result = fork();
		if (result == 0) {
			if (execvp((action_ptr->args)[0], action_ptr->args) == -1) {
				exit(1);
			}
			exit(1);
		} else {
			int status;
			waitpid(result, &status, 0);
			status = WEXITSTATUS(status); 
			if (status == 1) {
				exit(1);
			}
			action_ptr = action_ptr->next_act;
		}
	}
}

//Checking if the file by dependency is modified more recent than the target.
int compare_if_modified(Rule *rule, Dependency *dep_ptr) {

	struct stat *rule_stat = malloc(sizeof(struct stat)); 
    struct stat *dep_rule_stat = malloc(sizeof(struct stat));
	
	//Error in using stat
	if (stat(rule->target, rule_stat) == -1) {
		perror("stat");
		exit(1);
	}
	long int target_sec = (*rule_stat).st_mtim.tv_sec;
	long int target_nsec = (*rule_stat).st_mtim.tv_nsec;

 
	//Compare and return 1 if modified, 0 if not.
	if (stat(dep_ptr->rule->target, dep_rule_stat) == -1) {
	    perror("stat");
      	exit(1);
    }
	long int dep_sec = (*dep_rule_stat).st_mtim.tv_sec;
	long int dep_nsec = (*dep_rule_stat).st_mtim.tv_nsec;

	if (target_sec < dep_sec) {
		return 1;
	}
	else if (target_sec == dep_sec) {
		if (target_nsec < dep_nsec) {
			return 1;    
		} 
		else {
			return 0;
		}
	}
	return 0;
}



void run_make(char *target, Rule *rules, int pflag) {
    /*If the requested target is not available in the rules data structure
     * target_rule becomes the very first target. 
     */

    Rule *target_rule;
    if (target == NULL) {
        target_rule = rules; 
    } else {
        target_rule = search_rule(rules, target);
    }
    Dependency *dep_ptr = target_rule->dependencies;

	if (strncmp(target_rule->target, "all", 3) == 0) {
		while (dep_ptr != NULL) {
			run_make(dep_ptr->rule->target, rules, pflag);
			dep_ptr= dep_ptr->next_dep;
		}
		if (target_rule->actions != NULL) {
			execute_actions(target_rule);
		}	
	} else {
    	struct stat target_rule_stat;

   	 	//Obtaining the stat struct from the requested target. 
		// This is the case where target do not exist at all.
    	if (stat(target_rule->target, &target_rule_stat) == -1 && 
			target_rule->actions != NULL){

			//Dependencies not available just execute the action line from the
			//target itself.
			if (pflag == 0) {
				if (dep_ptr == NULL) {
					execute_actions(rules);
				} else {
					//Dependencies available execute the action lines
					//from each dependencies.
					execute_all_from_lowest(target_rule);
				}
			} else if (pflag == 1) {
				while (dep_ptr != NULL) {
					int result = fork();
					if (result == 0) {
						run_make(dep_ptr->rule->target, rules, pflag);
						exit(0);
					} else {
						int status;
						waitpid(result, &status, 0);
						status = WEXITSTATUS(status); 
						if (status == 1) {
							exit(1);
						}
								
					}
					dep_ptr = dep_ptr->next_dep;	
				}
				execute_actions(target_rule);
			}
    	} else {
			//Dependencies available.
			Action *action_ptr = target_rule->actions; //Used by parent process

    	    if (dep_ptr == NULL) {
				execute_actions(target_rule);
    	    } else {
				if (pflag == 0) {
					while (dep_ptr != NULL) {
						//apply pflag here
						
						run_make(dep_ptr->rule->target, rules, pflag);
						if (compare_if_modified(target_rule, dep_ptr) == 1) {
							execute_all_from_lowest(target_rule);
						}
						dep_ptr = dep_ptr->next_dep;
					}
				}
				//parallel make.
				else if (pflag == 1) {
					while (dep_ptr != NULL) {
						int result = fork();
						if (result == 0) {
							run_make(dep_ptr->rule->target, rules, pflag);		
							exit(0);
						} else {
							int status;
							waitpid(result, &status, 0);
							status = WEXITSTATUS(status); 
							if (status == 1) {
								exit(1);
							}
							action_ptr = action_ptr->next_act;
							if (compare_if_modified(target_rule, dep_ptr) == 1) {
								execute_all_from_lowest(target_rule);
							}
							dep_ptr = dep_ptr->next_dep;
						}
					}
				}
    	    }
    	}
	}



    return ;
}

