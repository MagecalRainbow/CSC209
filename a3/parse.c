#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }    
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to 
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;
    
    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);
            
            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");
            
            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* Searches for the rule that matches the given target name. 
 * The input has to be the target in a target line, and the name of the rule 
 * we would like to compare.
 * Returns the rule we are looking for on successful search, NULL on the
 * unsuccessful search.
 */
Rule *search_rule(Rule *target_rule, char *target_name) {
    if (target_rule->target == target_name) {
        return target_rule;
    } else {
        Dependency *dep = target_rule->dependencies;
        while (dep != NULL) {
            if (strcmp(dep->rule->target, target_name) == 0) {
                return dep->rule;
            } else {
                dep = dep->next_dep;
            }
        }
        if (target_rule->next_rule == NULL) {
            return NULL;
        }
    }
    return search_rule(target_rule->next_rule, target_name);
}


/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line 
       and should be ignored. 
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {
    char line[MAXLINE];
    Rule *rules = malloc(sizeof(Rule));

    //Getting the very first line of the Makefile. 

    if (fgets(line, MAXLINE, fp) == NULL) {
        return NULL;
    } 

    while (is_comment_or_empty(line) == 1) {
        if (fgets(line, MAXLINE, fp) == NULL) {
            return NULL;
        }
    }

    if (is_comment_or_empty(line) == 0) {
        char *dep_word = strtok(line, " :\n");
        rules->target = malloc(sizeof(dep_word));
        strncpy(rules->target, dep_word, strlen(dep_word));
        dep_word = strtok(NULL, " :\n");
        if (dep_word != NULL) {
            rules->dependencies = malloc(sizeof(Dependency));
            rules->dependencies->rule = malloc(sizeof(Rule));
            rules->dependencies->rule->target = malloc(sizeof(dep_word));
            strncpy(rules->dependencies->rule->target, dep_word, 
                    strlen(dep_word));
            Dependency *dep_ptr = rules->dependencies;
            dep_word = strtok(NULL, " \n");
            while (dep_word != NULL) {
				//make a helper function out of below
                dep_ptr->next_dep = malloc(sizeof(Dependency));
                dep_ptr->next_dep->rule = malloc(sizeof(Rule));
                dep_ptr->next_dep->rule->target = malloc(sizeof(dep_word));
                strncpy(dep_ptr->next_dep->rule->target, 
						dep_word, strlen(dep_word));
				dep_ptr->rule->next_rule = (dep_ptr->next_dep->rule);
				dep_ptr = dep_ptr->next_dep;			
                dep_word = strtok(NULL, " \n");
            }
        }
    } else {
        return NULL;
    }


    //target in the target-dependency line.
    Rule *target_rule_ptr = rules;  
    Action *action_ptr = NULL;


    
    while (fgets(line, MAXLINE, fp) != NULL) {
        //Proceed to parsing when the line is not comment or empty
        if (is_comment_or_empty(line) == 0 && strncmp(line, "\n", 1) != 0) {
            
            //If the starts with tab, parse according.
            if (strncmp(line, "\t", 1) == 0) {
                if (action_ptr == NULL) {
                    target_rule_ptr->actions = malloc(sizeof(Action));
                    target_rule_ptr->actions->args = build_args(line);
                    action_ptr = target_rule_ptr->actions;

                } else {
                    action_ptr->next_act = malloc(sizeof(Action));
                    action_ptr->next_act->args = build_args(line);
                    action_ptr = action_ptr->next_act;
                } 
            }
            //Else the line has to be target line.
            else {
                action_ptr = NULL;
                //This is the first word of a target line - target.
                char *word_holder = strtok(line, " :\n");

                target_rule_ptr->next_rule = search_rule(rules, word_holder);
                if (target_rule_ptr->next_rule == NULL) {
                    target_rule_ptr->next_rule = malloc(sizeof(Rule));
                    target_rule_ptr = target_rule_ptr->next_rule;
                    target_rule_ptr->target = malloc(sizeof(word_holder));
                    strncpy(target_rule_ptr->target, word_holder, strlen(word_holder));
                } else {
                    target_rule_ptr = target_rule_ptr->next_rule;
                } 
                
                Dependency *dep_ptr;
                word_holder = strtok(NULL, " :\n");
                if (word_holder != NULL) {
                    target_rule_ptr->dependencies = malloc(sizeof(Dependency));
                    dep_ptr = target_rule_ptr->dependencies;
                    dep_ptr->rule = malloc(sizeof(Rule));
                    dep_ptr->rule->target = malloc(sizeof(word_holder));
                    strncpy(dep_ptr->rule->target, word_holder, 
                            strlen(word_holder));
                    word_holder = strtok(NULL, " \n");
                    while (word_holder != NULL) {
						//make a helper function out of below
						dep_ptr->next_dep = malloc(sizeof(Dependency));
                		dep_ptr->next_dep->rule = malloc(sizeof(Rule));
                		dep_ptr->next_dep->rule->target = malloc(sizeof(word_holder));
                		strncpy(dep_ptr->next_dep->rule->target, 
								word_holder, strlen(word_holder));
						dep_ptr->rule->next_rule = (dep_ptr->next_dep->rule);
						dep_ptr = dep_ptr->next_dep;	
                        word_holder = strtok(NULL, " \n");
                    } 
                }
            }
        } 
    }

    return rules;
}

