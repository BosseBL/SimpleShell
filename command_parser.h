//
//  parser.h
//  cmd
//
//  Created by Daniel Lindfors on 10/12/14.
//  Copyright (c) 2014 Daniel Lindfors. All rights reserved.
//

#ifndef __cmd__parser__
#define __cmd__parser__

#include <stdio.h>

#endif /* defined(__cmd__parser__) */



/*
void parse(char**, char*);
char** get_next_exec();
void load_parser(char**);
void cleanup_parser();
int has_last_exec();
int has_first_exec();
int count_next_set();
char get_next_type();
*/

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
