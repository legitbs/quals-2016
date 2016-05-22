#include "pillpusher.h"

meta onering;

int read_delim( int fd, char *buf, char delim, int maxlen )
{
	int index = 0;
	char c;

	if ( buf == NULL ) {
		return 0;
	}

	while ( index < maxlen ) {
		if ( read( fd, &c, 1) <= 0 ) {
			printf("[ERROR] failed to read bytes\n");
			exit(0);
		}

		if ( c == delim ) {
			return index;
		}

		buf[index] = c;

		index++;
	}

	return index;
}

int read_integer( int fd )
{
	char buf[12];
	int value;

	memset( buf, 0, 12);

	read_delim( fd, buf, '\n', 11);

	value = atoi( buf );

	return value;
}

void add_pill_to_meta( pill *np)
{
	int index;
	pill **temp = NULL;

	if ( np == NULL ) {
		return;
	}

	/// Make sure the array has space
	if ( onering.drugs == NULL ) {
		onering.drugs = malloc( sizeof(pill*) * 4 );

		if ( !onering.drugs ) {
			printf("Game over man. Game over.\n");
			exit(0);
		}

		memset( onering.drugs, 0, sizeof( pill* ) * 4);
		onering.drug_max = 4;
	}

	for ( index = 0; index < onering.drug_max; index++ ) {
		if ( onering.drugs[index] == NULL ) {
			onering.drugs[index] = np;
			return;
		}
	}

	/// If a location was not found then reallocate
	temp = malloc( sizeof(pill *) * onering.drug_max*2);

	if ( !temp ) {
		printf("Wrong turn at Albuquerque.\n");
		exit(0);
	}

	memset( temp, 0, sizeof(pill *) * onering.drug_max*2);
	memcpy( temp, onering.drugs, sizeof(pill *) * onering.drug_max);
	free( onering.drugs);
	onering.drugs = temp;
	onering.drug_max *= 2;

	for ( index = 0; index < onering.drug_max; index++) {
		if ( onering.drugs[index] == NULL ) {
			onering.drugs[index] = np;
			return;
		}
	}

	printf("I should not be here yo..\n");

	return;
}

void add_pill_treatment( pill *np, char *t, int length )
{
	char **temp;

	if ( np == NULL || t == NULL ) {
		return;
	}

	/// Make sure there are enough slots
	if ( np->treats == NULL ) {
		np->treats = malloc( sizeof(char*)*4);

		if ( np->treats == NULL ) {
			printf("Stuff went bananas yo.\n");
			exit(0);
		}

		memset( np->treats, 0, sizeof(char*)*4);
		np->tc_max = 4;
		np->treats_count = 0;
	} else if ( np->treats_count == np->tc_max ) {
		temp = malloc( sizeof( char * ) * (np->tc_max * 2 ) );

		if ( temp == NULL ) {
			printf("It hit da fan\n");
			exit(0);
		}

		memset( temp, 0, sizeof( char * ) * (np->tc_max * 2 ));

		memcpy( temp, np->treats, sizeof( char * ) * (np->treats_count));

		/// The original is no longer needed so free it
		free(np->treats);

		/// Update the treats pointer and the max
		np->treats = temp;

		np->tc_max *= 2;
	}
	
	np->treats[ np->treats_count ] = malloc( length + 1);

	if ( np->treats[ np->treats_count] == NULL ) {
		printf("Something went down\n");
		exit(0);
	}

	memset( np->treats[ np->treats_count], 0, length + 1 );
	memcpy_r( np->treats[ np->treats_count], t, length );

	np->treats_count += 1;

	return;
}

void add_pill_interaction( pill *np, char *t )
{
	char **temp;
	int length;

	if ( np == NULL || t == NULL ) {
		return;
	}

	/// Make sure there are enough slots
	if ( np->interactions == NULL ) {
		np->interactions = malloc( sizeof(char*)*4);

		if ( np->interactions == NULL ) {
			printf("That ain't write\n");
			exit(0);
		}

		memset( np->interactions, 0, sizeof(char*)*4);
		np->ic_max = 4;
		np->interactions_count = 0;
	} else if ( np->interactions_count == np->ic_max ) {
		temp = malloc( sizeof( char * ) * (np->ic_max * 2 ) );

		if ( temp == NULL ) {
			printf("Just Nope\n");
			exit(0);
		}

		memset( temp, 0, sizeof( char * ) * (np->ic_max * 2 ));

		memcpy( temp, np->interactions, sizeof( char * ) * (np->interactions_count));

		/// The original is no longer needed so free it
		free(np->interactions);

		/// Update the treats pointer and the max
		np->interactions = temp;

		np->ic_max *= 2;
	}
	
	length = strlen(t);

	np->interactions[ np->interactions_count ] = malloc( length + 1);

	if ( np->interactions[ np->interactions_count] == NULL ) {
		printf("oh noze\n");
		exit(0);
	}

	memset( np->interactions[ np->interactions_count], 0, length + 1 );
	strcpy( np->interactions[ np->interactions_count], t );

	np->interactions_count += 1;

	return;
}

void add_pill_sideeffect( pill *np, char *t )
{
	char **temp;
	int length;

	if ( np == NULL || t == NULL ) {
		return;
	}

	/// Make sure there are enough slots
	if ( np->side_effects == NULL ) {
		np->side_effects = malloc( sizeof(char*)*4);

		if ( np->side_effects == NULL ) {
			printf("I got 99 problems\n");
			exit(0);
		}

		memset( np->side_effects, 0, sizeof(char*)*4);
		np->sec_max = 4;
		np->side_effect_count = 0;
	} else if ( np->side_effect_count == np->sec_max ) {
		temp = malloc( sizeof( char * ) * (np->sec_max * 2 ) );

		if ( temp == NULL ) {
			printf("Oops I did it again\n");
			exit(0);
		}

		memset( temp, 0, sizeof( char * ) * (np->sec_max * 2 ));

		memcpy( temp, np->side_effects, sizeof( char * ) * (np->side_effect_count));

		/// The original is no longer needed so free it
		free(np->side_effects);

		/// Update the treats pointer and the max
		np->side_effects = temp;

		np->sec_max *= 2;
	}
	
	length = strlen(t);

	np->side_effects[ np->side_effect_count ] = malloc( length + 1);

	if ( np->side_effects[ np->side_effect_count] == NULL ) {
		printf("Kanye\n");
		exit(0);
	}

	memset( np->side_effects[ np->side_effect_count], 0, length + 1 );
	strcpy( np->side_effects[ np->side_effect_count], t );

	np->side_effect_count += 1;

	return;
}

int check_pill_in_list( char *nm )
{
	int index = 0;

	if ( nm == NULL ) {
		return 0;
	}

	while ( index < onering.drug_max) {
		if (onering.drugs[index] == NULL ) {
			index += 1;
			continue;
		}

		if ( strncmp( onering.drugs[index]->name, nm, 256) == 0 ) {
			return 1;
		}

		index++;
	}

	return 0;
}

int add_pill( void )
{
	/// This can be used to leak a heap address but I don't think that it gets you much
	/// The purpose is to be able to create a drug name longer than 256 bytes for the later
	////	overflow
	struct {
		char data[256];
		pill *new_pill;
	} locals;
	
	int reslut = 0;
	int length = 0;
	
	memset( locals.data, 0, 256);

	printf("Pill Name: ");

	read_delim( STDIN, locals.data, '\n', 256);

	if ( check_pill_in_list( locals.data ) ) {
		printf("No home slice it is already there\n");
		return;
	}

	locals.new_pill = malloc( sizeof(pill) );

	length = strlen(locals.data);


	if ( locals.new_pill == NULL ) {
		printf("Done gone and screwed up.\n");
		exit(0);
	}

	memset( locals.new_pill, 0, sizeof(pill) );

	locals.new_pill->name = malloc( length + 1 );

	if ( locals.new_pill->name == NULL ) {
		printf("Didn't work\n");
		exit(0);
	}

	memset( locals.new_pill->name, 0, length+1 );

	memcpy_r( locals.new_pill->name, locals.data, strlen(locals.data) );

	printf("Dosage: ");
	memset( locals.data, 0, 256);

	length = 0;
	read_delim( STDIN, locals.data, '\n', 256);

	locals.new_pill->dose = atoi( locals.data );

	printf("Schedule: ");
	memset( locals.data, 0, 256);
	read_delim( STDIN, locals.data, '\n', 256);
	locals.new_pill->schedule = atoi( locals.data );

	printf("List what this pill treats. Blank line to quit.\n");

	while ( 1 ) {
		printf(": ");
		memset( locals.data, 0, 256);
		length = read_delim( STDIN, locals.data, '\n', 256);

		if ( length == 0 ) {
			break;
		}

		add_pill_treatment( locals.new_pill, locals.data, length );
	}

	printf("List other pills this interacts with. Blank line to quit.\n");

	while ( 1 ) {
		printf(": " );
		memset( locals.data, 0, 256);
		if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
			break;
		}

		add_pill_interaction( locals.new_pill, locals.data );
	}

	printf("List all side effects. Blank line to quit.\n");

	while ( 1 ) {
		printf(": " );
		memset( locals.data, 0, 256);
		if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
			break;
		}

		add_pill_sideeffect( locals.new_pill, locals.data );
	}

	add_pill_to_meta( locals.new_pill );

	return;
}

void print_pill( pill *p )
{
	int i;

	if ( p == NULL ) {
		return;
	}

	printf("Name: %s\n", p->name);
	printf("\tDosage: %d\n", p->dose);
	printf("\tSchedule: %d\n", p->schedule);

	printf("\tTreats: \n");
	for (i = 0; i< p->treats_count; i++) {
		if ( p->treats[i] == NULL ) {
			continue;
		}
		printf("\t\t%d) %s\n", i+1, p->treats[i]);
	}

	printf("\tSide Effects: \n");
	for (i = 0; i< p->side_effect_count; i++) {
		if ( p->side_effects[i] == NULL ) {
			continue;
		}
		printf("\t\t%d) %s\n", i+1, p->side_effects[i]);
	}

	printf("\tInteractions: \n");
	for (i = 0; i< p->interactions_count; i++) {
		if ( p->interactions[i] == NULL ) {
			continue;
		}
		printf("\t\t%d) %s\n", i+1, p->interactions[i]);
	}

	printf("\n");
	return;
}

int list_pills( void )
{
	int index = 0;

	if ( onering.drugs == NULL ) {
		printf("No brains. You need pills first.\n");
		return 0;
	}

	while ( index < onering.drug_max ) {
		print_pill( onering.drugs[index]);
		index++;
	}

	return 1;
}

void random_pill( void )
{
	pill *np = NULL;
	int count = 0;
	unsigned long int r = 0;
	unsigned long int p = 0;
	int total = 0;
	char data[256];
	int i;

	np = malloc( sizeof(pill));

	if ( !np ) {
		printf("Stellaaaa.\n");
		exit(0);
	}

	memset(np, 0, sizeof(pill));

	/// Randomly select a name
	while ( pills[count] ) {
		count++;
	}

	i = 0;

	do {
		r = random();

		p = r % count;

		i++;
	} while ( check_pill_in_list( pills[p]) && i < count );

	if ( i == count ) {
		printf("To many pills pushed bra.\n");
		return;
	}

	count = strlen( pills[p] );

	np->name = malloc( count + 1);

	if ( !np->name){
		printf("Not gonna be yuge..\n");
		exit(0);
	}

	memset( np->name, 0, count + 1);
	strcpy( np->name, pills[p] );

	count = 0;
	while ( symptoms[count] ) {
		count++;
	}

	r = random();
	total = (r % 5) + 1;

	for( i = 0; i < total; i++ ) {
		r = random();
		p = r % count;

		memset( data, 0, 256);
		strcpy( data, symptoms[p]);

		add_pill_treatment( np, data, strlen(data));
	}

	count = 0;
	while ( side_effects[count] ) {
		count++;
	}

	r = random();
	total = (r % 5) + 1;

	for( i = 0; i < total; i++ ) {
		r = random();
		p = r % count;

		memset( data, 0, 256);
		strcpy( data, side_effects[p]);

		add_pill_sideeffect( np, data);
	}

	count = 0;
	while ( pills[count] ) {
		count++;
	}

	r = random();
	total = (r % 5) + 1;

	for( i = 0; i < total; i++ ) {
		r = random();
		p = r % count;

		memset( data, 0, 256);
		strcpy( data, pills[p]);

		/// Don't interact with itself
		if ( strncmp( data, np->name, 256) == 0 ) {
			total += 1;
			continue;
		}

		add_pill_interaction( np, data);
	}

	r = random();
	total = (r % 10) + 1;
	np->dose = total;

	r = random();
	total = (r % 5) + 1;
	np->schedule = total;

	add_pill_to_meta( np );

	return;
}

pill *get_pill( char *name )
{
	pill *walker = NULL;
	int index = 0;

	if ( name == NULL ) {
		return NULL;
	}

	while ( index < onering.drug_max) {
		walker = onering.drugs[index];

		if ( walker ) {
			if ( strncmp( walker->name, name, 256) == 0 ) {
				return walker;
			}
		}

		index++;
	}

	return NULL;
}

void print_side_effects( pill *p )
{
	int index = 0;

	if ( p == NULL ) {
		return;
	}

	printf("Side Effects: \n");
	for ( index = 0; index < p->side_effect_count; index++ ) {
		if ( !(p->side_effects[index]) ) {
			continue;
		}

		printf("%d) %s\n", index + 1, p->side_effects[index]);
	}

	printf("\n");

	return;
}

void remove_side_effect( pill *p, int index )
{
	int rem = 0;

	if ( p == NULL || index <= 0 ) {
		return;
	}

	if ( p->side_effect_count < index ) {
		printf("Out of scope yo.\n");
		return;
	}

	index -= 1;

	if ( p->side_effects[index] == NULL ) {
		printf("Nahh, not valid\n");
		return;
	}

	free( p->side_effects[index] );
	p->side_effects[index] = NULL;

	for ( rem = index; rem < p->sec_max-1; rem ++ ) {
		p->side_effects[rem] = p->side_effects[rem+1];
	}

	p->side_effects[p->sec_max-1] = NULL;

	p->side_effect_count -= 1;

}

void add_side_effect( pill *p )
{
	struct {
		char data[256];
		int val;
	} locals;
	char **temp;
	int new_max = 0;

	if ( p == NULL ) {
		return;
	}
	locals.val = 0;

	/// update list buffer if needed
	if ( p->sec_max <= p->side_effect_count ) {
		new_max = p->sec_max * 2;

		temp = malloc( sizeof(char*) * new_max);

		if ( !temp ) {
			printf("Fail\n");
			exit(0);
		}

		memset( temp, 0, sizeof(char*)*new_max);

		memcpy( temp, p->side_effects, sizeof( char *) * p->sec_max);

		free( p->side_effects);

		p->side_effects = temp;
		p->sec_max = new_max;
	}

	memset( locals.data, 0, 256);

	printf("New side effect: ");

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		return;
	}

	p->side_effects[ p->side_effect_count ] = malloc( strlen( locals.data ) + 1);

	if ( !p->side_effects[ p->side_effect_count ] ) {
		printf("Still failed\n");
		exit(0);
	}

	memset( p->side_effects[ p->side_effect_count ], 0, strlen(locals.data) + 1 );
	strcpy( p->side_effects[ p->side_effect_count ], locals.data);

	p->side_effect_count++;

	return;
}


/// This function is used for the stack leak. So do not fix it with locals.
void edit_side_effect( pill *p, int index )
{
	char data[256];

	if ( p == NULL || index <= 0 ) {
		return;
	}

	if ( p->side_effect_count < index ) {
		printf("To hi bro.\n");
		return;
	}

	index -= 1;

	if ( p->side_effects[index] == NULL ) {
		printf("Far out\n");
		return;
	}

	memset( data, 0, 256);
	printf("Update: ");
	if ( read_delim( STDIN, data, '\n', 256) == 0 ) {
		printf("I don't want empty stuff\n");
		return;
	}

	set_effect( p, data, index);

	return;
}

void set_effect( pill *p, char *data, int index )
{	
	char *t = NULL;

	if ( !p || !data ) {
		return;
	}

	if ( !p->side_effects ) {
		return;
	}

	if ( p->side_effects[index] ) {
		free( p->side_effects[index]);
	}

	t = malloc( strlen(data) + 1);

	if ( !t ) {
		printf("Not good boys.\n");
		exit(0);
	}

	memset( t, 0, strlen(data) + 1);

	strcpy( t, data );

	p->side_effects[index] = t;

	return;
}

void update_pill_side_effect( pill *p )
{
	int sel = 0;

	if ( p == NULL ) {
		return;
	}

	while ( 1 ) {
		printf("1) Remove side effect\n");
		printf("2) Add Side effect\n");
		printf("3) Update Side effect\n");
		printf("4) Done.\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				print_side_effects( p );
				printf("-> ");
				sel = read_integer( STDIN );
				remove_side_effect( p, sel );
				break;
			case 2:
				add_side_effect(p);
				break;
			case 3:
				print_side_effects( p );
				printf("-> ");
				sel = read_integer( STDIN );
				edit_side_effect( p, sel );
				break;
			case 4:
				return;
				break;
			default:
				printf("You mad bro?\n");
				break;
		};

	}
}

void remove_interaction( pill *p, int index )
{
	int rem = 0;

	if ( p == NULL || index <= 0 ) {
		return;
	}

	if ( p->interactions_count < index ) {
		printf("Not happening\n");
		return;
	}

	index -= 1;

	if ( p->interactions[index] == NULL ) {
		printf("Come at me bro\n");
		return;
	}

	free( p->interactions[index] );
	p->interactions[index] = NULL;

	for ( rem = index; rem < p->ic_max-1; rem ++ ) {
		p->interactions[rem] = p->interactions[rem+1];
	}

	p->interactions[p->ic_max-1] = NULL;

	p->interactions_count -= 1;
}

void add_interaction( pill *p )
{
	struct {
		char data[256];
		int val;
	} locals;
	char **temp;
	int new_max = 0;

	if ( p == NULL ) {
		return;
	}

	locals.val = 0;

	/// update list buffer if needed
	if ( p->ic_max <= p->interactions_count ) {
		new_max = p->ic_max * 2;

		temp = malloc( sizeof(char*) * new_max);

		if ( !temp ) {
			printf("Nunya\n");
			exit(0);
		}

		memset( temp, 0, sizeof(char*)*new_max);

		memcpy( temp, p->interactions, sizeof( char *) * p->ic_max);

		free( p->interactions);

		p->interactions = temp;
		p->ic_max = new_max;
	}

	memset( locals.data, 0, 256);

	printf("New interaction: ");

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		return;
	}

	p->interactions[ p->interactions_count ] = malloc( strlen( locals.data ) + 1);

	if ( !p->interactions[ p->interactions_count ] ) {
		printf("Still failed\n");
		exit(0);
	}

	memset( p->interactions[ p->interactions_count ], 0, strlen(locals.data) + 1 );
	strcpy( p->interactions[ p->interactions_count ], locals.data);

	p->interactions_count++;

	return;
}

void edit_interaction( pill *p, int index )
{
	struct {
		char data[256];
		int val;
	} locals;

	if ( p == NULL || index <= 0 ) {
		return;
	}

	locals.val = 0;

	if ( p->interactions_count < index ) {
		printf("You krunk\n");
		return;
	}

	index -= 1;

	if ( p->interactions[index] == NULL ) {
		printf("Blank\n");
		return;
	}

	memset( locals.data, 0, 256);
	printf("Update: ");
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("Gimme dataz\n");
		return;
	}

	free( p->interactions[index] );
	p->interactions[index] = malloc( strlen(locals.data) + 1);

	if ( !p->interactions[index] ) {
		printf("nope.\n");
		exit(0);
	}

	memset( p->interactions[index], 0, strlen(locals.data) + 1);

	strcpy( p->interactions[index], locals.data );

	return;
}

void print_interactions( pill *p )
{
	int index = 0;

	if ( p == NULL ) {
		return;
	}

	printf("Interactions: \n");
	for ( index = 0; index < p->interactions_count; index++ ) {
		if ( !(p->interactions[index]) ) {
			continue;
		}

		printf("\t%d) %s\n", index + 1, p->interactions[index]);
	}

	printf("\n");

	return;
}

void update_pill_interactions( pill *p )
{
	int sel = 0;

	if ( p == NULL ) {
		return;
	}

	while ( 1 ) {
		printf("1) Remove interaction\n");
		printf("2) Add Interaction\n");
		printf("3) Update Interaction\n");
		printf("4) Done.\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				print_interactions( p );
				printf("-> ");
				sel = read_integer( STDIN );
				remove_interaction( p, sel );
				break;
			case 2:
				add_interaction(p);
				break;
			case 3:
				print_interactions( p );
				printf("-> ");
				sel = read_integer( STDIN );
				edit_interaction( p, sel );
				break;
			case 4:
				return;
				break;
			default:
				printf("Trippin ballz\n");
				break;
		};

	}
}

void print_treatments( pill *p )
{
	int index = 0;

	if ( p == NULL ) {
		return;
	}

	printf("Treatments: \n");
	for ( index = 0; index < p->treats_count; index++ ) {
		if ( !(p->treats[index]) ) {
			continue;
		}

		printf("\t%d) %s\n", index + 1, p->treats[index]);
	}

	printf("\n");

	return;
}

void remove_treatment( pill *p, int index )
{
	int rem = 0;

	if ( p == NULL || index <= 0 ) {
		return;
	}

	if ( p->treats_count < index ) {
		printf("Dont hate\n");
		return;
	}

	index -= 1;

	if ( p->treats[index] == NULL ) {
		printf("What\n");
		return;
	}

	free( p->treats[index] );
	p->treats[index] = NULL;

	for ( rem = index; rem < p->tc_max-1; rem ++ ) {
		p->treats[rem] = p->treats[rem+1];
	}

	p->treats[p->tc_max-1] = NULL;

	p->treats_count -= 1;
}

void add_treatment( pill *p )
{
	struct {
		char data[256];
		int val;
	} locals;
	char **temp;
	int new_max = 0;

	if ( p == NULL ) {
		return;
	}

	locals.val = 0;

	/// update list buffer if needed
	if ( p->tc_max <= p->treats_count ) {
		new_max = p->tc_max * 2;

		temp = malloc( sizeof(char*) * new_max);

		if ( !temp ) {
			printf("Not a chance\n");
			exit(0);
		}

		memset( temp, 0, sizeof(char*)*new_max);

		memcpy( temp, p->treats, sizeof( char *) * p->tc_max);

		free( p->treats);

		p->treats = temp;
		p->tc_max = new_max;
	}

	memset( locals.data, 0, 256);

	printf("New Treatment: ");

	new_max = read_delim( STDIN, locals.data, '\n', 256);

	if ( new_max == 0 ) {
		return;
	}

	p->treats[ p->treats_count ] = malloc( new_max + 1);

	if ( !p->treats[ p->treats_count ] ) {
		printf("What hatin\n");
		exit(0);
	}

	memset( p->treats[ p->treats_count ], 0, strlen(locals.data) + 1 );
	memcpy_r( p->treats[ p->treats_count ], locals.data, new_max);

	p->treats_count++;

	return;
}

void edit_treatment( pill *p, int index )
{
	struct {
		char data[256];
		int val;
	} locals;

	if ( p == NULL || index <= 0 ) {
		return;
	}

	locals.val = 0;

	if ( p->treats_count < index ) {
		printf("Try again\n");
		return;
	}

	index -= 1;

	if ( p->treats[index] == NULL ) {
		printf("Empty\n");
		return;
	}

	memset( locals.data, 0, 256);
	printf("Update: ");
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("No luck\n");
		return;
	}

	free( p->treats[index] );
	p->treats[index] = malloc( strlen(locals.data) + 1);

	if ( !p->treats[index] ) {
		printf("nope.\n");
		exit(0);
	}

	memset( p->treats[index], 0, strlen(locals.data) + 1);

	strcpy( p->treats[index], locals.data );

	return;
}

void update_pill_treatments( pill *p )
{
	int sel = 0;

	if ( p == NULL ) {
		return;
	}

	while ( 1 ) {
		printf("1) Remove Treatment\n");
		printf("2) Add Treatment\n");
		printf("3) Update Treatment\n");
		printf("4) Done.\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				print_treatments( p );
				printf("-> ");
				sel = read_integer( STDIN );
				remove_treatment( p, sel );
				break;
			case 2:
				add_treatment(p);
				break;
			case 3:
				print_treatments( p );
				printf("-> ");
				sel = read_integer( STDIN );
				edit_treatment( p, sel );
				break;
			case 4:
				return;
				break;
			default:
				printf("Can't read a menu?\n");
				break;
		};

	}
}

void modify_pill( void )
{
	struct {
		char data[256];
		int val;
	} locals;

	pill *modme = NULL;
	locals.val = 0;

	printf("List Pills? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			list_pills();
		}
	}

	memset(locals.data, 0, 256);

	printf("Which pill to modify: " );

	read_delim( STDIN, locals.data, '\n', 256);

	modme = get_pill( locals.data );

	if ( modme == NULL ) {
		printf("Outlook points to no.\n");
		return;
	}

	printf("Modify Dosage? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			printf("New Dosage: ");
			memset( locals.data, 0, 256);
			read_delim( STDIN, locals.data, '\n', 256);
			modme->dose = atoi( locals.data );
		}
	}

	printf("Modify Schedule? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			printf("New Schedule: ");
			memset( locals.data, 0, 256);
			read_delim( STDIN, locals.data, '\n', 256);
			modme->schedule = atoi( locals.data );
		}
	}

	printf("Modify Side Effects? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			update_pill_side_effect(modme);
		}
	}
	
	printf("Modify Interactions? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			update_pill_interactions(modme);
		}
	}

	printf("Modify Treatments? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			update_pill_treatments(modme);
		}
	}
}

void lose_pills( void )
{
	int index = 0;
	int counter = 0;
	int j = 0;
	int k = 0;
	pill *temp = NULL;

	printf("About to get raided huh.\n");

	for (index = 0; index < onering.drug_max; index++ ) {
		if ( onering.drugs[index] == NULL ) {
			continue;
		}
		printf("%d) %s\n", index + 1, onering.drugs[index]->name);
	}

	printf("-> ");

	index = read_integer(STDIN);

	if ( index <= 0 || index > onering.drug_max) {
		printf("Careful sampling your own product.\n");
		return;
	}

	index -= 1;

	if ( onering.drugs[index] == NULL ) {
		printf("Just go away.\n");
		return;
	}

	/// Remove the pointers from the pharmacy
	temp = onering.drugs[index];

	for ( counter = 0; counter < onering.pharma_max; counter++ ) {
		if ( onering.pharmacies[counter] == NULL ) {
			continue;
		}

		for ( j = 0; j < onering.pharmacies[counter]->pill_max; j++) {
			if ( onering.pharmacies[counter]->stock[j] == NULL ) {
				continue;
			}

			if ( temp == onering.pharmacies[counter]->stock[j]) {
				onering.pharmacies[counter]->stock[j] = NULL;

				for ( k = j; k < onering.pharmacies[counter]->pill_max - 1; k++) {
					onering.pharmacies[counter]->stock[k] = onering.pharmacies[counter]->stock[k+1];
				}

				onering.pharmacies[counter]->stock[k] = NULL;

				// counter has to be reduced by 1 to account for the removal
				onering.pharmacies[counter]->pill_cnt--;
				j--;
			}
		}
	}

	/// Free the treatments
	for ( counter = 0; counter < onering.drugs[index]->tc_max; counter++ ) {
		if ( onering.drugs[index]->treats[counter] ) {
			free( onering.drugs[index]->treats[counter] );
		}
	}

	free( onering.drugs[index]->treats );

	/// Free the interactions
	for ( counter = 0; counter < onering.drugs[index]->ic_max; counter++ ) {
		if ( onering.drugs[index]->interactions[counter] ) {
			free( onering.drugs[index]->interactions[counter] );
		}
	}

	free( onering.drugs[index]->interactions );

	/// Free the Side effects
	for ( counter = 0; counter < onering.drugs[index]->sec_max; counter++ ) {
		if ( onering.drugs[index]->side_effects[counter] ) {
			free( onering.drugs[index]->side_effects[counter] );
		}
	}

	free( onering.drugs[index]->side_effects );

	free( onering.drugs[index]->name);
	free( onering.drugs[index]);
	onering.drugs[index] = NULL;

	for( counter = index; counter < onering.drug_max - 1; counter++) {
		onering.drugs[counter] = onering.drugs[counter+1];
	}

	onering.drugs[ onering.drug_max -1] = NULL;

	return;
}

void pill_menu( void )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Add a Pill\n");
		printf("2) Random Pill\n");
		printf("3) List Pills\n");
		printf("4) Modify Pill\n");
		printf("5) \"Lose\" Pills\n");
		printf("6) Leave Menu\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				add_pill();			
				break;
			case 2:
				random_pill();
				break;
			case 3:
				list_pills();
				break;
			case 4:
				modify_pill();
				break;
			case 5:
				lose_pills();
				break;
			case 6:
				return;
				break;
			default:
				printf("You must be on dope.\n");
				break;
		}

	}

	return;
}

void add_pill_to_pharmacy( pharmacy *ph, pill *pi )
{
	pill **temp;
	int index = 0;

	if ( ph == NULL || pi == NULL ) {
		return;
	}

	// base case
	if ( ph->stock == NULL ) {
		ph->stock = malloc( sizeof(pill*)*4);

		if ( ph->stock == NULL ) {
			printf("It's broke please fix it.\n");
			exit(0);
		}

		memset( ph->stock, 0, sizeof(pill*)*4);

		ph->pill_max = 4;
	}

	/// Increase buffer.
	if ( ph->pill_cnt == ph->pill_max ) {
		temp = malloc( sizeof(pill*) * ph->pill_max * 2);

		if ( temp == NULL ) {
			printf("Can't take this shit no mo.\n");
			exit(0);
		}

		memset( temp, 0, sizeof( pill*) * ph->pill_max * 2);

		memcpy( temp, ph->stock, sizeof(pill*) * ph->pill_max);

		free(ph->stock);
		ph->stock = temp;
		ph->pill_max *= 2;
	}

	/// Make sure that the pill isn't already there
	for ( index =0 ; index < ph->pill_cnt; index++) {
		if ( ph->stock[index] == pi ) {
			printf("Yo already got that pill bro. Don't overdo it.\n");
			return;
		}
	}

	ph->stock[ph->pill_cnt] = pi;
	ph->pill_cnt += 1;

	return;
}

pharmacist *get_doc( char *name )
{
	int i = 0;

	if ( name == NULL ) {
		return NULL;
	}

	for ( i = 0; i < onering.doc_max; i++) {
		if ( onering.docs[i] == NULL ) {
			continue;
		}

		if ( strncmp( onering.docs[i]->name, name, 256) == 0 ) {
			return onering.docs[i];
		}
	}

	return NULL;
}

void list_pharmacists( void )
{
	int i =0 ;

	for ( i = 0; i < onering.doc_max; i++  ) {
		if ( onering.docs[i] == NULL ) {
			continue;
		}

		printf("%d) %s -- %d\n", i+1, onering.docs[i]->name, onering.docs[i]->certifications);
	}

	printf("\n");
}

void add_staff_to_pharmacy( pharmacy *ph, pharmacist *doc )
{
	pharmacist **temp;
	int index = 0;

	if ( ph == NULL || doc == NULL ) {
		return;
	}

	// base case
	if ( ph->staff == NULL ) {
		ph->staff = malloc( sizeof(pharmacist*)*4);

		if ( ph->staff == NULL ) {
			printf("No room at the inn\n");
			exit(0);
		}

		memset( ph->staff, 0, sizeof(pharmacist*)*4);

		ph->staff_max = 4;
	}

	/// Increase buffer.
	if ( ph->staff_cnt == ph->staff_max ) {
		temp = malloc( sizeof(pharmacist*) * ph->staff_max * 2);

		if ( temp == NULL ) {
			printf("Staring down the barrel.\n");
			exit(0);
		}

		memset( temp, 0, sizeof( pharmacist*) * ph->staff_max * 2);

		memcpy( temp, ph->staff, sizeof(pharmacist*) * ph->staff_max);

		free(ph->staff);
		ph->staff = temp;
		ph->staff_max *= 2;
	}

	/// Make sure that the pill isn't already there
	for ( index =0 ; index < ph->staff_cnt; index++) {
		if ( ph->staff[index] == doc ) {
			printf("Swimmin through the ashes\n");
			return;
		}
	}

	ph->staff[ph->staff_cnt] = doc;
	ph->staff_cnt += 1;

	return;
}

void add_pharmacy_to_meta( pharmacy *ph)
{
	int index;
	pharmacy **temp = NULL;

	if ( ph == NULL ) {
		return;
	}

	if ( onering.pharmacies == NULL ) {
		onering.pharmacies = malloc( sizeof(pharmacy*) * 4 );

		if ( !onering.pharmacies ) {
			printf("No real reason to accept the way\n");
			exit(0);
		}

		memset( onering.pharmacies, 0, sizeof( pharmacy* ) * 4);
		onering.pharma_max = 4;
	}

	for ( index = 0; index < onering.pharma_max; index++ ) {
		if ( onering.pharmacies[index] == NULL ) {
			onering.pharmacies[index] = ph;
			return;
		}
	}

	/// If a location was not found then reallocate
	temp = malloc( sizeof(pharmacy *) * onering.pharma_max*2);

	if ( !temp ) {
		printf("As I was going to Saint Ives.\n");
		exit(0);
	}

	memset( temp, 0, sizeof(pharmacy *) * onering.pharma_max*2);
	memcpy( temp, onering.pharmacies, sizeof(pharmacy *) * onering.pharma_max);
	free( onering.pharmacies);
	onering.pharmacies = temp;
	onering.pharma_max *= 2;

	for ( index = 0; index < onering.pharma_max; index++) {
		if ( onering.pharmacies[index] == NULL ) {
			onering.pharmacies[index] = ph;
			return;
		}
	}

	return;
}

void create_pharmacy( void )
{
	struct {
		char data[256];
		int val;
	} locals;
	pharmacy *ph = NULL;
	pill *ap = NULL;
	pharmacist *tdoc = NULL;

	locals.val = 0;

	printf("What is the \"Pharmacy\"'s name? " );

	memset( locals.data, 0, 256);
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("Even your mother is disappointed in you.\n");
		return;
	}

	ph = malloc( sizeof(pharmacy) );

	if ( !ph ) {
		printf("Everything dies in the end\n");
		exit(0);
	}

	memset( ph, 0, sizeof(pharmacy));

	ph->name = malloc( strlen(locals.data) + 1 );

	if ( !ph->name ) {
		printf("Hello darkness my old friend.\n");
		exit(0);
	}

	memset( ph->name, 0, strlen(locals.data) + 1 );
	strcpy( ph->name, locals.data );

	if ( !list_pills() ) {
		free( ph->name );
		free(ph);
		return;
	}

	printf( "Add pills. Blank line to quit.\n");

	memset( locals.data, 0, 256 );
	printf(": " );

	while ( read_delim( STDIN, locals.data, '\n', 256) ) {
		ap = get_pill( locals.data );

		if ( ap == NULL ) {
			printf("Seeing things?\n: ");
			memset( locals.data, 0, 256);
			continue;
		}

		add_pill_to_pharmacy( ph, ap );
		printf(": ");
		memset( locals.data, 0, 256);

	}

	list_pharmacists();

	printf("Add staff. Blank line to quit.\n");

	memset( locals.data, 0, 256);
	printf(": ");

	while( read_delim( STDIN, locals.data, '\n', 256) ) {
		tdoc = get_doc( locals.data );

		if ( tdoc == NULL ) {
			printf("Failing harder.\n: ");
			memset( locals.data, 0, 256);
			continue;
		}

		add_staff_to_pharmacy( ph, tdoc);
		printf(": ");
		memset( locals.data, 0, 256);
	}

	add_pharmacy_to_meta( ph );

	return;
}

void remove_pill_from_pharmacy( pharmacy *p )
{
	int i = 0;
	int rem;

	if ( p == NULL ) {
		return;
	}

	for ( i = 0; i < p->pill_max; i++ ) {
		if ( p->stock[i] == NULL ) {
			continue;
		}

		printf("%d) %s\n", i+1, p->stock[i]->name);
	}

	printf(": ");

	i = read_integer( STDIN );

	if ( i <= 0 || i > p->pill_max) {
		printf("Out of scope\n");
		return;
	}

	i-= 1;
	if ( p->stock[i] == NULL ) {
		printf("Empty\n");
		return;
	}

	for ( rem = i; rem < p->pill_max-1; rem++) {
		p->stock[rem] = p->stock[rem+1];
	}

	p->stock[rem] = NULL;

	p->pill_cnt -= 1;

	return;
}

void update_pill_to_pharmacy( pharmacy *p)
{
	int sel = 0;

	if ( p == NULL ) {
		return;
	}

	for ( sel = 0; sel < onering.drug_max; sel++) {
		if ( onering.drugs[sel] == NULL ) {
			continue;
		}

		printf("%d) %s\n", sel+1, onering.drugs[sel]->name);
	}

	printf(": ");

	sel = read_integer(STDIN);

	if ( sel <=0 || sel > onering.drug_max) {
		printf("Heard Startalk?.\n");
		return;
	}

	sel -= 1;

	if ( onering.drugs[sel] == NULL ) {
		printf("Not available yet.\n");
		return;
	}

	add_pill_to_pharmacy( p, onering.drugs[sel]);

	return;
}

void update_pharmacy_pills( pharmacy *p )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Remove Pill\n");
		printf("2) Add Pill\n");
		printf("3) Done\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch (sel) {
			case 1:
				remove_pill_from_pharmacy( p );
				break;
			case 2:
				update_pill_to_pharmacy(p);
				break;
			case 3:
				return;
				break;
			default:
				printf("596f75205375636b\n");
				break;
		};
	}

	return;
}

void remove_doc_from_pharmacy( pharmacy *p )
{
	int i = 0;
	int rem;

	if ( p == NULL ) {
		return;
	}

	for ( i = 0; i < p->staff_max; i++ ) {
		if ( p->staff[i] == NULL ) {
			continue;
		}

		printf("%d) %s\n", i+1, p->staff[i]->name);
	}

	printf(": ");

	i = read_integer( STDIN );

	if ( i <= 0 || i > p->staff_max) {
		printf("There exist bounds for a reason\n");
		return;
	}

	i-= 1;
	if ( p->staff[i] == NULL ) {
		printf("Why you choose NULL\n");
		return;
	}

	for ( rem = i; rem < p->staff_max-1; rem++) {
		p->staff[rem] = p->staff[rem+1];
	}

	p->staff[rem] = NULL;

	p->staff_cnt -= 1;

	return;
}

void update_staff_to_pharmacy( pharmacy *p )
{
	int sel = 0;

	if ( p == NULL ) {
		return;
	}

	for ( sel = 0; sel < onering.doc_max; sel++) {
		if ( onering.docs[sel] == NULL ) {
			continue;
		}

		printf("%d) %s\n", sel+1, onering.docs[sel]->name);
	}

	printf(": ");

	sel = read_integer(STDIN);

	if ( sel <=0 || sel > onering.doc_max) {
		printf("Names are hard.\n");
		return;
	}

	sel -= 1;

	if ( onering.docs[sel] == NULL ) {
		printf("Add a person now.\n");
		return;
	}

	add_staff_to_pharmacy( p, onering.docs[sel]);


	return;
}

void update_pharmacy_staff( pharmacy *p )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Remove Doctor\n");
		printf("2) Add Doctor\n");
		printf("3) Done\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch (sel) {
			case 1:
				remove_doc_from_pharmacy( p );
				break;
			case 2:
				update_staff_to_pharmacy(p);
				break;
			case 3:
				return;
				break;
			default:
				printf("Mmmm Donuts.\n");
				break;
		};
	}

	return;
}


void update_pharmacy( void )
{
	int i = 0;
	int j = 0;
	struct {
		char data[256];
		int val;
	} locals;
	pharmacy *p = NULL;

	locals.val = 0;

	for ( i = 0; i < onering.pharma_max; i++) {
		if( onering.pharmacies[i] == NULL ) {
			continue;
		}

		printf("%d) %s\n", i+1, onering.pharmacies[i]->name);
	}

	i = 0;
	memset( locals.data, 0, 256);
	printf("Rebranding? : ");
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("No hope. No change.\n");
		return;
	}

	for ( j = 0; j < onering.pharma_max; j++) {
		if ( onering.pharmacies[j] == NULL ) {
			continue;
		}

		if ( strncmp( locals.data, onering.pharmacies[j]->name, 256) == 0 ) {
			p = onering.pharmacies[j];
			break;
		}
	}

	if ( p == NULL ) {
		printf("No esta bueno.\n");
		return;
	}

	i = 0;
	j = 0;

	printf("Update Name? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			printf(": ");
			memset( locals.data, 0, 256);

			if ( read_delim(STDIN, locals.data, '\n', 256) ) {
				free( p->name );
				p->name = malloc( strlen(locals.data) + 1 );

				if (!p->name) {
					printf("Coooooookie.\n");
					exit(0);
				}

				memset( p->name, 0, strlen(locals.data) + 1);
				strcpy( p->name, locals.data );
			}
		}
	}

	/// Update pills
	printf("Update Pill Stock? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			update_pharmacy_pills( p );
		}
	}

	/// Update staff
	printf("Update Staff? (y/N): ");

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
			update_pharmacy_staff( p );
		}
	}
	return;
}

void list_pharmacies( void ) 
{
	int index = 0;
	int j = 0;

	for ( index = 0; index < onering.pharma_max; index++) {
		if ( onering.pharmacies[index] == NULL ) {
			continue;
		}

		printf("Name: %s\n", onering.pharmacies[index]->name);

		if ( onering.pharmacies[index]->stock ) {
			printf("Stock: \n");
			for ( j = 0; j< onering.pharmacies[index]->pill_max; j++ ) {
				if ( onering.pharmacies[index]->stock[j] == NULL ) {
					continue;
				}

				printf("\t%s\n", onering.pharmacies[index]->stock[j]->name);
			}
		}

		if ( onering.pharmacies[index]->staff ) {
			printf("Staff: \n");
			for ( j = 0; j< onering.pharmacies[index]->staff_max; j++ ) {
				if ( onering.pharmacies[index]->staff[j] == NULL ) {
					continue;
				}

				printf("\t%s\n", onering.pharmacies[index]->staff[j]->name);
			}
		}
	}
	printf("\n");

	return;
}

void delete_pharmacy( void )
{
	int sel = 0;
	int rem = 0;
	pharmacy *p;

	for ( sel = 0; sel < onering.pharma_max; sel++) {
		if ( onering.pharmacies[sel] == NULL ) {
			continue;
		}

		printf("%d) %s\n", sel+1, onering.pharmacies[sel]->name);
	}

	printf(": ");

	sel = read_integer( STDIN );

	if ( sel <= 0 || sel > onering.pharma_max) {
		printf("I clean now.\n");
		return;
	}

	sel -= 1;

	if ( onering.pharmacies[sel] == NULL ) {
		printf("Of all the things I've lost.\n");
		return;
	}

	free( onering.pharmacies[sel]->stock);
	free( onering.pharmacies[sel]->staff);
	free( onering.pharmacies[sel]->name);
	free( onering.pharmacies[sel]);

	for ( rem = sel; rem < onering.pharma_max - 1; rem++) {
		onering.pharmacies[rem] = onering.pharmacies[rem+1];
	}

	onering.pharmacies[rem] = NULL;

	return;
}

void pharmacy_menu( void )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Create Pharmacy\n");
		printf("2) Update Pharmacy\n");
		printf("3) Delete Pharmacy\n");
		printf("4) List Pharmacies\n");
		printf("5) Go up\n");
		printf("-> ");

		sel = read_integer(STDIN);

		switch (sel ) {
			case 1:
				create_pharmacy();
				break;
			case 2:
				update_pharmacy();
				break;
			case 3:
				delete_pharmacy();
				break;
			case 4:
				list_pharmacies();
				break;
			case 5:
				return;
				break;
			default:
				printf("Try again..\n");
				break;
		};
	}

	return;
}

void add_pharmacist_to_meta( pharmacist *ph)
{
	int index;
	pharmacist **temp = NULL;

	if ( ph == NULL ) {
		return;
	}

	/// Make sure the array has space
	if ( onering.docs == NULL ) {
		onering.docs = malloc( sizeof(pharmacist*) * 4 );

		if ( !onering.docs ) {
			printf("Because a vision softly creeping\n");
			exit(0);
		}

		memset( onering.docs, 0, sizeof( pharmacist* ) * 4);
		onering.doc_max = 4;
	}

	for ( index = 0; index < onering.doc_max; index++ ) {
		if ( onering.docs[index] == NULL ) {
			onering.docs[index] = ph;
			return;
		}
	}

	/// If a location was not found then reallocate
	temp = malloc( sizeof(pharmacist *) * onering.doc_max*2);

	if ( !temp ) {
		printf("Left its seeds while I was sleeping\n");
		exit(0);
	}

	memset( temp, 0, sizeof(pharmacist *) * onering.doc_max*2);
	memcpy( temp, onering.docs, sizeof(pharmacist *) * onering.doc_max);
	free( onering.docs);
	onering.docs = temp;
	onering.doc_max *= 2;

	for ( index = 0; index < onering.doc_max; index++) {
		if ( onering.docs[index] == NULL ) {
			onering.docs[index] = ph;
			return;
		}
	}

	printf("And the vision that was planted in my brain\n");

	return;
}

void add_pharmacist( void )
{
	struct {
		char data[256];
		int val;
	} locals;

	pharmacist *ph;

	locals.val = 0;

	memset(locals.data, 0, 256);

	printf("Name: ");

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("No. No. I clean now.\n");
		return;
	}

	ph = malloc( sizeof(pharmacist));

	if ( !ph ) {
		printf("You are fired.\n");
		exit(0);
	}

	memset( ph, 0, sizeof( pharmacist) );

	ph->name = malloc( strlen( locals.data ) + 1 );

	if (!ph->name) {
		printf("I've come to talk with you again\n");
		exit(0);
	}

	memset( ph->name, 0, strlen(locals.data) + 1);

	strcpy( ph->name, locals.data );

	printf("Certification Level: ");
	memset( locals.data, 0, 256);

	read_delim( STDIN, locals.data, '\n', 256);

	ph->certifications = atoi( locals.data );

	add_pharmacist_to_meta( ph );

	return;
}

void remove_pharmacist( void )
{
	int sel = 0;
	int rem = 0;
	int j;
	int k;
	pharmacist *temp;

	list_pharmacists();

	printf("Who died: ");

	sel = read_integer( STDIN);

	if ( sel <= 0 ) {
		printf("Nope.\n");
		return;
	}

	if ( sel > onering.doc_max ) {
		printf("Lol. seriously nope\n");
		return;
	}

	sel -= 1;
	if ( onering.docs[ sel ] == NULL ) {
		printf("Lame nope.\n");
		return;
	}

	/// Remove the pointers for each pharmacist from the pharmacy
	temp = onering.docs[sel];

	/// Loop through each pharmacy
	for ( rem = 0; rem < onering.pharma_max; rem++ ) {
		if ( onering.pharmacies[rem] == NULL ) {
			continue;
		}

		/// Loop through each doctor in a given pharmacy
		for ( j = 0; j < onering.pharmacies[rem]->staff_max; j++) {
			if ( onering.pharmacies[rem]->staff[j] == NULL ) {
				continue;
			}

			if ( temp == onering.pharmacies[rem]->staff[j]) {
				onering.pharmacies[rem]->staff[j] = NULL;

				for ( k = j; k < onering.pharmacies[rem]->staff_max - 1; k++) {
					onering.pharmacies[rem]->staff[k] = onering.pharmacies[rem]->staff[k+1];
				}

				onering.pharmacies[rem]->staff[k] = NULL;

				onering.pharmacies[rem]->staff_cnt--;
				j--;
			}
		}
	}

	free( onering.docs[sel]->name);
	free( onering.docs[sel] );
	onering.docs[sel] = NULL;

	for ( rem = sel; rem < onering.doc_max-1; rem++) {
		onering.docs[rem] = onering.docs[rem+1];
	}

	onering.docs[ onering.doc_max-1] = NULL;

	return;
}

void update_pharmacist( void )
{
	struct {
		char data[256];
		int val;
	} locals;
	int i = 0;
	pharmacist *ph = NULL;

	list_pharmacists();

	locals.val = 0;

	printf("Change who: ");
	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("I need data bruh!\n");
		return;
	}

	ph = get_doc( locals.data );

	if ( ph == NULL ) {
		printf("Yeah...Nope\n");
		return;
	}

	memset( locals.data, 0, 256);
	printf("Change Name? (y/N): ");

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0) {
		printf("Can't get the data in huh?\n");
		return;
	}

	if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
		memset( locals.data, 0, 256);
		printf(": ");
		if ( read_delim(STDIN, locals.data, '\n', 256) == 0 ) {
			printf("No data. No update.\n");
			return;
		}

		free(ph->name);
		ph->name = malloc( strlen(locals.data) + 1 );

		if ( !ph->name ) {
			printf("No more. Just no.\n");
			exit(0);
		}

		memset( ph->name, 0, strlen(locals.data) + 1 );

		strcpy( ph->name, locals.data);
	}
	
	memset( locals.data, 0, 256);
	printf("Change Certification? (y/N): ");

	if ( read_delim( STDIN, locals.data, '\n', 255) == 0) {
		printf("They have pills for that.\n");
		return;
	}

	if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {
		memset( locals.data, 0, 256);
		printf(": ");
		if ( read_delim(STDIN, locals.data, '\n', 256) == 0 ) {
			printf("Gimme the numbers.\n");
			return;
		}

		ph->certifications = atoi( locals.data );
	}

	return;
}

void pharmacist_menu( void )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Add Pharmacist\n");
		printf("2) Remove Pharmacist\n");
		printf("3) Update Pharmacist\n");
		printf("4) List Pharmacists\n");
		printf("5) Back up\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				add_pharmacist();
				break;
			case 2:
				remove_pharmacist();
				break;
			case 3:
				update_pharmacist();
				break;
			case 4:
				list_pharmacists();
				break;
			case 5:
				return;
				break;
			default:
				printf("Unable to handle the peoples?\n");
				break;
		};
	}
	return;
}

void add_patient_to_meta( patient *p)
{
	int index;
	patient **temp = NULL;

	if ( p == NULL ) {
		return;
	}

	/// Make sure the array has space
	if ( onering.patients == NULL ) {
		onering.patients = malloc( sizeof(patient*) * 4 );

		if ( !onering.patients ) {
			printf("Sound of silence\n");
			exit(0);
		}

		memset( onering.patients, 0, sizeof( patient* ) * 4);
		onering.pat_max = 4;
	}

	for ( index = 0; index < onering.pat_max; index++ ) {
		if ( onering.patients[index] == NULL ) {
			onering.patients[index] = p;
			return;
		}
	}

	/// If a location was not found then reallocate
	temp = malloc( sizeof(patient *) * onering.pat_max*2);

	if ( !temp ) {
		printf("And the vision that was planted in my brain \n");
		exit(0);
	}

	memset( temp, 0, sizeof(patient *) * onering.pat_max*2);
	memcpy( temp, onering.patients, sizeof(patient *) * onering.pat_max);
	free( onering.patients);
	onering.patients = temp;
	onering.pat_max *= 2;

	for ( index = 0; index < onering.pat_max; index++) {
		if ( onering.patients[index] == NULL ) {
			onering.patients[index] = p;
			return;
		}
	}

	printf("People writing songs that voices never share\n");

	return;
}

void add_symptom_to_patient( patient *p, char *s )
{
	char **temp;
	int index = 0;

	if ( p == NULL || s == NULL ) {
		return;
	}

	// base case
	if ( p->symptoms == NULL ) {
		p->symptoms = malloc( sizeof(char*)*4);

		if ( p->symptoms == NULL ) {
			printf("I don't know what's going on.\n");
			exit(0);
		}

		memset( p->symptoms, 0, sizeof(char*)*4);

		p->symptom_max = 4;
	}

	/// Increase buffer.
	if ( p->symptom_cnt == p->symptom_max ) {
		temp = malloc( sizeof(char*) * p->symptom_max * 2);

		if ( temp == NULL ) {
			printf("Peace. I'm out.\n");
			exit(0);
		}

		memset( temp, 0, sizeof( char*) * p->symptom_max * 2);

		memcpy( temp, p->symptoms, sizeof(char*) * p->symptom_max);

		free(p->symptoms);
		p->symptoms = temp;
		p->symptom_max *= 2;
	}


	p->symptoms[p->symptom_cnt] = malloc( strlen(s) + 1);

	if ( !p->symptoms[p->symptom_cnt] ) {
		printf("Roll out.\n");
		exit(0);
	}

	memset( p->symptoms[p->symptom_cnt] , 0, strlen(s) + 1);
	strcpy( p->symptoms[p->symptom_cnt] , s);
	p->symptom_cnt += 1;

	return;
}

void add_patient( void )
{
	struct {
		char data[256];
		int val;
	} locals;
	patient *p;

	memset( locals.data, 0, 256);
	locals.val = 0;

	printf("Patient name: ");
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("Go away please.\n");
		return;
	}

	p = malloc( sizeof(patient) );

	if ( !p ) {
		printf("That didn't work\n");
		exit(0);
	}

	memset( p, 0, sizeof(patient));

	p->name = malloc( strlen(locals.data) + 1);

	if ( !p->name ) {
		printf("Why do we keep doing this?\n");
		exit(0);
	}

	memset( p->name, 0, strlen(locals.data ) + 1);
	strcpy( p->name, locals.data);

	memset( locals.data, 0, 256);

	printf("Would you like to enter symptoms? (Y/n): ");

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0) {
		if ( locals.data[0] == 'n' || locals.data[0] == 'N') {
			add_patient_to_meta( p );
			return;
		}
	}

	memset( locals.data, 0, 256);
	printf(": ");

	while ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		add_symptom_to_patient( p, locals.data );
		memset( locals.data, 0, 256);
		printf(": ");
	}

	add_patient_to_meta(p);
	return;
}

void list_patients( void )
{
	int i = 0;
	int j = 0;

	for (i = 0; i < onering.pat_max; i++) {
		if (onering.patients[ i ] ) {
			printf("Name: %s\n", onering.patients[ i ]->name );

			if ( onering.patients[ i ]->symptoms != NULL ) {
				printf("Symptoms: \n");
				for ( j = 0; j < onering.patients[ i ]->symptom_max; j++) {
					if ( onering.patients[ i ]->symptoms[j] ) {
						printf("\t%d) %s\n", j+1, onering.patients[ i ]->symptoms[j]);
					}
				}
			}

			if ( onering.patients[ i ]->scrips != NULL ) {
				printf("Scrips: \n\t%s\n", onering.patients[i]->scrips);
				
			}
		}
	}
	return;
}

patient *get_patient( char *name )
{
	int i = 0;

	for ( i = 0; i < onering.pat_max; i++ ) {
		if ( onering.patients[i] == NULL ) {
			continue;
		}

		if ( strncmp( onering.patients[i]->name, name, 256) == 0 ) {
			return onering.patients[i];
		}
	}

	return NULL;
}

void delete_patient( void )
{
	struct {
		char data[256];
		int val;
	} locals;

	int i = 0;
	int rem = 0;
	patient *w = NULL;

	locals.val = 0;
	memset( locals.data, 0, 256);

	printf("Who OhDeed? ");

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("Must have been you\n");
		return;
	}

	for ( i = 0; i < onering.pat_max; i++ ) {
		if ( onering.patients[i] == NULL ) {
			continue;
		}

		if ( strcmp( onering.patients[i]->name, locals.data) == 0 ) {
			w = onering.patients[i];
			break;
		}
	}

	if ( w == NULL ) {
		printf("Wasting time\n");
		return;
	}

	free( w->name );

	// Free the things
	if ( w->symptoms ) {
		for ( rem = 0; rem < w->symptom_max; rem++) {
			if ( w->symptoms[rem] ) {
				free( w->symptoms[rem]);
			}
		}

		free(w->symptoms);
	}

	if ( w->scrips ) {
		free(w->scrips);
	}

	free( w );
	onering.patients[i] = NULL;

	for ( rem = i; rem < onering.pat_max - 1; rem++ ) {
		onering.patients[rem] = onering.patients[rem+1];
	}

	onering.patients[ onering.pat_max - 1] = NULL;
}

void update_patient( void )
{
	struct {
		char data[256];
		int val;
	} locals;
	patient *p = NULL;

	list_patients();

	locals.val = 0;
	memset( locals.data, 0, 256);

	printf("Which numbnuts? : ");
	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("No name. No patient.\n");
		return;
	}

	p = get_patient( locals.data );

	if ( p == NULL ) {
		printf("Ain't no one here by that name.\n");
		return;
	}

	printf("Update Name? (y/N): ");
	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		if ( locals.data[0] == 'y' || locals.data[0] == 'Y') {

			memset( locals.data, 0, 256);

			printf(": ");
			if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
				printf("Why go this far and give a bad name?\n");
				return;
			}

			if ( p->name) {
				free( p->name);
			}

			p->name = malloc( strlen( locals.data) + 1);

			if ( !p->name ) {
				printf("WHy, just why?\n");
				exit(0);
			}

			memset( p->name, 0, strlen(locals.data) + 1);

			strcpy( p->name, locals.data);

		}
	}

	return;
}

void patient_menu( void )
{
	int sel = 0;

	while ( 1 ) {
		printf("1) Add Patient\n");
		printf("2) Delete Patient\n");
		printf("3) Update Patient\n");
		printf("4) List Patients\n");
		printf("5) Leave menu\n");
		printf("-> ");

		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				add_patient();
				break;
			case 2:
				delete_patient();
				break;
			case 3:
				update_patient();
				break;
			case 4:
				list_patients();
				break;
			case 5:
				return;
				break;
			default:
				printf("Wrong choice mate.\n");
				break;
		};
	}
	return;
}

pharmacy *get_pharmacy( char *data )
{
	int i = 0;

	if ( data == NULL ) {
		return NULL;
	}

	for ( i = 0; i < onering.pharma_max; i++) {
		if ( onering.pharmacies[i] == NULL ) {
			continue;
		}

		if ( strncmp( onering.pharmacies[i]->name, data, 256) == 0) {
			return onering.pharmacies[i];
		}
	}

	return NULL;
}

pharmacy * select_pharmacy( void )
{
	struct {
		char data[256];
		int val;
	} locals; 

	list_pharmacies();
	memset( locals.data, 0, 256);
	locals.val = 0;
	printf(": ");

	if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
		return get_pharmacy( locals.data );
	}

	return NULL;
}

pharmacist *select_doc( pharmacy *store )
{
	int sel;

	if ( !store ) {
		return NULL;
	}

	/// List the staff specific to the pharmacy
	for ( sel =0 ; sel < store->staff_max; sel ++) {
		if ( store->staff[sel] == NULL ) {
			continue;
		}

		printf("%d) %s\n", sel + 1, store->staff[sel]->name);
	}

	printf(": ");

	sel = read_integer( STDIN );

	if ( sel <= 0 || sel > store->staff_max ) {
		printf("Fail\n");
		return NULL;
	}

	return store->staff[sel-1];

}

void list_pharmacy_pills( pharmacy *ph)
{
	int i;

	if ( ph == NULL ) {
		return;
	}

	printf("\nPills available at: %s\n", ph->name);
	for ( i = 0; i < ph->pill_max; i++) {
		if (ph->stock[i] == NULL ) {
			continue;
		}

		printf("\t%d) %s\n", i+1, ph->stock[i]->name);
	}

	printf("\n");
	return;
}

long add_pill_to_scrip( pharmacy *ph, pharmacist *doc, patient *pat, char *scrip)
{
	int i = 0;
	int j = 0;
	long success = 0;
	pill *p = NULL;

	struct {
		char data[256];
		int val;
	} locals;

	locals.val = 0;

	if ( !ph || !doc || !pat || !scrip) {
		return 0;
	}

	memset( locals.data, 0, 256);

	if ( read_delim( STDIN, locals.data, '\n', 256) == 0 ) {
		printf("Blank line.\n");
		return 0;
	}


	/// Loop through each drug
	for ( i = 0; i < ph->pill_max; i++) {
		if ( ph->stock[i] == NULL ) {
			continue;
		}

		if ( strncmp( ph->stock[i]->name, locals.data, 256) == 0 ) {
			printf("strncmp found it.\n");
			p = ph->stock[i];
			break;
		}
	}

	if ( p == NULL ) {
		printf("Invalid\n");
		return 0;
	}

	//// Check symptoms. Pill must at least solve one symptom
	for ( i = 0; i < pat->symptom_cnt && !success; i++) {
		if ( pat->symptoms[i] == NULL ) {
			continue;
		}

		for ( j = 0; j < p->treats_count && !success; j++) {
			if ( strncmp( pat->symptoms[i], p->treats[j], 256) == 0 ) {
				success = 1;
			}
		}

	}

	j = strlen(p->name);

	printf("Len: %d\n", j);

	if ( !success ) {
		printf("Pill solves nothing. You pill pusher.\n");
		return 0;
	}

	/// The pharmacist must also be able to give it out
	if ( p->schedule > doc->certifications) {
		printf("This doc isn't qualified.\n");
		return 0;
	}

	strcat( scrip, p->name );

	scrip[j++] = 0x20;
	scrip[j++] = 0x20;
	scrip[j++] = 0x20;
	scrip[j++] = 0x20;

	return 1;
}

void scrip_menu( void )
{
	
	struct {
		char data[256];
		int val;
		int sel;
		pharmacy *store;
		patient *pat;
		pharmacist *doc;
	} locals;

	memset( locals.data, 0, 256);
	locals.val = 0;
	locals.store = NULL;
	locals.pat = NULL;
	locals.doc = NULL;

	while ( 1 ) {
		printf("Current Pharmacy: ");

		if ( locals.store ) {
			printf("%s\n", locals.store->name);
		}else {
			printf("None\n");
		}

		printf("Current Pharmacist: ");

		if ( locals.doc ) {
			printf("%s\n", locals.doc->name);
		}else {
			printf("None\n");
		}

		printf("Current Patient: ");

		if ( locals.pat ) {
			printf("%s\n", locals.pat->name);
		}else {
			printf("None\n");
		}

		printf("1) Select a Pharmacy\n");
		printf("2) Select a Pharmacist\n");
		printf("3) Select a Patient\n");
		printf("4) Add Scrip\n");
		printf("5) Leave\n");
		printf("-> ");

		locals.sel = read_integer( STDIN );

		switch ( locals.sel ) {
			case 1:
				locals.store = select_pharmacy();

				if ( locals.store == NULL ) {
					printf("Fail\n");
				}

				locals.pat = NULL;
				locals.doc = NULL;
				break;
			case 2:
				if ( locals.store == NULL ) {
					printf("Pick a store first.\n");
					break;
				}

				locals.doc = select_doc( locals.store );

				if ( locals.doc == NULL ) {
					printf("Fail\n");
				}
				break;
			case 3:
				list_patients();
				memset( locals.data, 0, 256);
				printf(": ");

				if ( read_delim( STDIN, locals.data, '\n', 256) != 0 ) {
					locals.pat = get_patient( locals.data );
				}

				if ( locals.pat == NULL ) {
					printf("Fail\n");
				}
				break;
			case 4:
				if ( !locals.store || !locals.doc || !locals.pat) {
					printf("You have to select all the things first.\n");
				} else {
					add_scrip(locals.store, locals.doc, locals.pat);
				}
				break;
			case 5:
				return;
				break;
			default:
				printf("Fail\n");
				break;
		};
	}

	return;
}


/// The overwrite of the eflags register occurs in this function.
void add_scrip( pharmacy *ph, pharmacist *doc, patient *pat)
{
	struct {
		//// the purpose of this is to prevent them from overflowing the previous heap block and then
		////	creating another scrip with a > 262 byte drug name. That would turn this into a stack overflow
		char zeros[512];
		long cnt;
		long success;
		long extra;
		/// Max is a cheat here so that they can control the size field of the heap block.
		long max;
		char scrip[512];
	} locals;

	if ( !ph || !doc || !pat) {
		return;
	}

	/// The size of scrip is meant to hold up to two pill names and allow an overflow of only 1 or two bytes
	///	of the flags field
	/// For the overflow to happen you have to use the max drug name length (256) combined with the 6
	/// Byte leak of the address making it 262 bytes
	/// Add 4 spaces to this makes it 266
	/// Followed by another 256 gives 522 bytes. This allows a 2 byte overflow.
	/// Even with an additional 6 bytes of address I do not think that they can hit the return address

	locals.cnt = 0;
	locals.success = 0;
	locals.max = 0;
	locals.extra = 0;
	memset( locals.zeros, 0, 512);

	printf("How many pills to add: ");

	locals.max = read_integer( 0 );

	if ( locals.max <= 2 ) {
		locals.cnt = locals.max;
	} else {
		locals.cnt = 2;
	}
	memset( locals.scrip, 0, 512);

	while ( locals.cnt ) {

		list_pharmacy_pills( ph );

		printf("Add pill: ");
		
		locals.success = add_pill_to_scrip( ph, doc, pat, locals.scrip );

		if ( locals.success == 1 ) {
			locals.cnt -= 1;
			locals.extra += 1;
		} else if ( locals.success == 0 ) {
			break;
		}

	}

	if ( locals.extra > 0 ) {
		if ( pat->scrips) {
			free( pat->scrips);
		}

		pat->scrips = malloc ( strlen(locals.scrip) + 1);

		if ( !pat->scrips ) {
			printf("Fail\n");
			exit(0);
		}

		memset( pat->scrips, 0, strlen(locals.scrip) + 1);
		memcpy_r( pat->scrips, locals.scrip, strlen(locals.scrip) );
	}

	return;

}

void main_menu( void )
{
	int sel = 0;

	memset( &onering, 0, sizeof(meta));

	while ( 1 ) {
		printf( "1) Pharmacy Menu\n");
		printf( "2) Pill Menu\n");
		printf( "3) Pharmacist Menu\n");
		printf( "4) Patient Menu\n");
		printf( "5) Scrip Menu\n");
		printf( "6) Exit\n");
		printf( "-> ");
		sel = read_integer( STDIN );

		switch ( sel ) {
			case 1:
				pharmacy_menu();
				break;
			case 2:
				pill_menu();
				break;
			case 3:
				pharmacist_menu();
				break;
			case 4:
				patient_menu();
				break;
			case 5:
				scrip_menu();
				break;
			case 6:
				printf("Catcha laterz\n");
				return;
				break;
			default:
				printf("Takin your own product?\n");
				break;
		}


	}
	return;
}

int main( int argc, char **argv)
{

	main_menu();

	return  0;
}
