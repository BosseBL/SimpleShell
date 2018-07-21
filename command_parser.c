//
//  parser.c
//  cmd
//
//  Created by Daniel Lindfors on 10/12/14.
//  Copyright (c) 2014 Daniel Lindfors. All rights reserved.
//

#include <string.h>			/* definierar bl.a. strcpy() */
#include <stdlib.h>			/* definierar bl.a. malloc() och free() */
#include <errno.h>			/* definierar bl.a. errno */
#include <stdio.h>			/* definierar bl.a. fprintf() och stderr */


#define TRUE ( 1 )
#define FALSE ( 0 )


int current_element;		/* start index för nästa serie element i elements */
int n_of_elements;			/* antalet element i elements */
char **elements;			/* string array med de element som ska behandlas */
char **param;				/* senaste serien element som gavs av get_next_param() */
char *separator;			/* de sträng som ska användas som separator i elements */





/* --------------- Public (finns i parser.h) ---------------*/



/*	int get_next_param(char** *radr);
 *	tar som parameter adressen till en string array radr.
 *	Funktionen tar fram nästa serie med element som separeras av en separator eller
 *	slutet på elements. Denna serie paras i param. Serien avslutas alltid med NULL.
 *
 *	serien görs tillgänglig för användaren genom att
 *	addressen till param läggs sparas på addressen radr.
 *
 *	funktionen returnerar -1 vid syntaxfel.
 *	Annars returneras antalet element i param exklusive det avslutande NULL elementet.
 */
int get_next_param(char** *radr);


/*	int init_parser(int argc, char** argv);
 *	initierar parser modulen med string array argv.
 *	argc förväntas vara antalet element i argv.
 *	argv förväntas ha det nuvarande programmet som första parameter och
 *	en sträng som ska användas som separator i de kommande elementen som andra parameter.
 *	(argv ex. [[./program],[separator],......] )
 *	Om argc är mindre än 2 returneras -1.
 *	annars returneras 0.
 */
int init_parser(int argc, char** argv);


/*	void cleanup_parser();
 *	Frigör eventuellt minne som allokerats åt param med free()
 *	och sätter param till NULL
 */
void cleanup_parser();


/*	int has_last_param();
 *	Kontrollerar om den nuvarande serien med element i param är de sista
 *	i elements. Returnerar 1 om sant, Annars 0
 */
int has_last_param();


/*	int has_first_param();
 *	Kontrollerar om den nuvarande serien med element i param är den första
 *	i elements. Returnerar 1 om sant. Annars 0.
 * 	Funktionen returnerar också 0 om param är NULL.
 *	0 returneras också om parser initierats men get_next_param() inte använts än.
 */
int has_first_param();






#include "command_parser.h"
#include <stdlib.h>
#include <string.h>


/*	parser.c
 *	Definierar modulen parser.
 *	modulen hanterar en string array med serier av element separerade med en separator.
 *	Nästa serie med element i denna string array kan hämtas i form av en ny string array.
 */



/* samtliga standard-inkluderingar och modul-deklarationer */


/* deklarerad i parser_private.h och parser.h */
int get_next_param(char** *radr) {
    
    /* Om adressen radr är NULL returneras -1 */
    if(radr == NULL) {
        fprintf(stderr, "in get_next_param(), NULL pointer,\n");
        errno = EINVAL; return -1;
    }
    
    /* Frigör tidigare allokerat utrymme som param pekar till */
    free(param);
    param = NULL;
    
    /* räknar antal element fram tills slutet av elements eller nästa separator */
    int n = 0;
    while(TRUE) {
        /* om inte vid slutet av elements */
        if( current_element + n < n_of_elements ) {
            /* om inte separator vid nuvarande position */
            if( strcmp(elements[current_element + n], "|") && strcmp(elements[current_element + n], "&") ) {
                /* öka räknaren och loopar en gång till */
                n++;
                continue;
            }
            /* om elements innehåller två separatorer i rad. avbryter */
            if( n == 0) {
                fprintf(stderr, "in get_next_param(), two consecutive separators %s found,\n",
                        separator);
                errno = EINVAL; return -1;
            }
        }
        /* om inget av ovanstående så har vi räknat färdigt och bryter loopen */
        break;
    }
    
    /* allokerar utrymme för räknade element samt avslutande null */
    param = (char**)malloc((n+1) * sizeof(char*));
    if(param == NULL) {
        fprintf(stderr, "in get_next_param(), couldn't allocate memory,\n"); return -1;
    }
    
    /* lägger till elementen och avslutande null i det allokerade utrymmet */
    int i;
    for(i = 0 ; i < n ; i++) {
        *(param+i) = elements[current_element + i];
    }
    *(param + i) = NULL;
    
    current_element += n + 1;	/* avancerar current_element förbi eventuell separator*/
    *radr = param;				/* gör elementen tillgängliga på adress radr */
    return n;					/* returnerar antalet element i param */
}


/* deklarerad i parser_private.h och parser.h */
int init_parser(int argc, char **argv) {
    elements = argv;
    n_of_elements = argc;
    param = NULL;
    current_element = 0;
    separator = "|";
    return 0;
}


/* deklarerad i parser_private.h och parser.h */
void cleanup_parser() {
    /* frigör eventuellt allokerat utrymme */
    free(param);
    param = NULL;
}


/* deklarerad i parser_private.h och parser.h */
int has_last_param() {
    /* om nuvarande element index är större eller lika med antalet element */
    if(current_element >= n_of_elements) return TRUE;
    else return FALSE;
}


/* deklarerad i parser_private.h och parser.h */
int has_first_param() {
    /* Om param är NULL har inga element matats fram sedan initiering */
    if(param == NULL) return FALSE;
    
    int i;
    while (param[i] != NULL) i++;
    /* backar nuvarande element index med antalet element i param och jämför med 2 */
    if(current_element - i == 1) return TRUE;
    else return FALSE;
}





