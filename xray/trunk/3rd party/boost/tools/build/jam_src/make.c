/*
 * Copyright 1993, 1995 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */


/*  This file is ALSO:
 *  (C) Copyright David Abrahams 2001. Permission to copy, use,
 *  modify, sell and distribute this software is granted provided this
 *  copyright notice appears in all copies. This software is provided
 *  "as is" without express or implied warranty, and with no claim as
 *  to its suitability for any purpose.
 */

/*
 * make.c - bring a target up to date, once rules are in place
 *
 * This modules controls the execution of rules to bring a target and
 * its dependencies up to date.  It is invoked after the targets, rules,
 * et. al. described in rules.h are created by the interpreting of the
 * jam files.
 *
 * This file contains the main make() entry point and the first pass
 * make0().  The second pass, make1(), which actually does the command
 * execution, is in make1.c.
 *
 * External routines:
 *	make() - make a target, given its name
 *
 * Internal routines:
 * 	make0() - bind and scan everything to make a TARGET
 * 	make0sort() - reorder TARGETS chain by their time (newest to oldest)
 *
 * 12/26/93 (seiwald) - allow NOTIME targets to be expanded via $(<), $(>)
 * 01/04/94 (seiwald) - print all targets, bounded, when tracing commands
 * 04/08/94 (seiwald) - progress report now reflects only targets with actions
 * 04/11/94 (seiwald) - Combined deps & headers into deps[2] in TARGET.
 * 12/20/94 (seiwald) - NOTIME renamed NOTFILE.
 * 12/20/94 (seiwald) - make0() headers after determining fate of target, so 
 *			that headers aren't seen as dependents on themselves.
 * 01/19/95 (seiwald) - distinguish between CANTFIND/CANTMAKE targets.
 * 02/02/95 (seiwald) - propagate leaf source time for new LEAVES rule.
 * 02/14/95 (seiwald) - NOUPDATE rule means don't update existing target.
 * 08/22/95 (seiwald) - NOUPDATE targets immune to anyhow (-a) flag.
 * 09/06/00 (seiwald) - NOCARE affects targets with sources/actions.
 * 03/02/01 (seiwald) - reverse NOCARE change.
 * 03/14/02 (seiwald) - TEMPORARY targets no longer take on parents age
 * 03/16/02 (seiwald) - support for -g (reorder builds by source time)
 */

# include "jam.h"

# include "lists.h"
# include "parse.h"
# include "variable.h"
# include "rules.h"

# include "search.h"
# include "newstr.h"
# include "make.h"
# include "headers.h"
# include "command.h"

# ifndef max
# define max( a,b ) ((a)>(b)?(a):(b))
# endif

typedef struct {
	int	temp;
	int	updating;
	int	cantfind;
	int	cantmake;
	int	targets;
	int	made;
} COUNTS ;

static void make0( TARGET *t, time_t ptime, int depth, 
		COUNTS *counts, int anyhow );

static TARGETS *make0sort( TARGETS *c );

#ifdef OPT_GRAPH_DEBUG_EXT
static void dependGraphOutput( TARGET *t, int depth );
#endif

static char *target_fate[] = 
{
	"init",		/* T_FATE_INIT */
	"making", 	/* T_FATE_MAKING */
	"stable", 	/* T_FATE_STABLE */
	"newer",	/* T_FATE_NEWER */
	"temp", 	/* T_FATE_ISTMP */
	"touched", 	/* T_FATE_TOUCHED */
	"missing", 	/* T_FATE_MISSING */
	"old", 		/* T_FATE_OUTDATED */
	"update", 	/* T_FATE_UPDATE */
	"nofind", 	/* T_FATE_CANTFIND */
	"nomake" 	/* T_FATE_CANTMAKE */
} ;

static char *target_bind[] = 
{
	"unbound",
	"missing",
	"parents",
	"exists",
} ;

# define spaces(x) ( "                    " + ( x > 20 ? 0 : 20-x ) )

/*
 * make() - make a target, given its name
 */

int
make( 
	int	n_targets,
	char	**targets,
	int	anyhow )
{
	int i;
	COUNTS counts[1];
	int status = 0;		/* 1 if anything fails */

#ifdef OPT_HEADER_CACHE_EXT
	hcache_init();
#endif

	memset( (char *)counts, 0, sizeof( *counts ) );

    /* First bind all targets with LOCATE_TARGET setting. This is
       needed to correctly handle dependencies to generated headers.       
    */
    bind_explicitly_located_targets();

	for( i = 0; i < n_targets; i++ )
	{
	    TARGET *t = bindtarget( targets[i] );

	    make0( t, (time_t)0, 0, counts, anyhow );
	}
        
#ifdef OPT_GRAPH_DEBUG_EXT
	if( DEBUG_GRAPH )
	{
	    for( i = 0; i < n_targets; i++ )
	    {
		TARGET *t = bindtarget( targets[i] );
		dependGraphOutput( t, 0 );
	    }
	}
#endif

	if( DEBUG_MAKE )
	{
	    if( counts->targets )
		printf( "...found %d target%s...\n", counts->targets,
		        counts->targets > 1 ? "s" : "" );
	    if( counts->temp )
		printf( "...using %d temp target%s...\n", counts->temp,
		        counts->temp > 1 ? "s" : "" );
	    if( counts->updating )
		printf( "...updating %d target%s...\n", counts->updating,
		        counts->updating > 1 ? "s" : "" );
	    if( counts->cantfind )
		printf( "...can't find %d target%s...\n", counts->cantfind,
		        counts->cantfind > 1 ? "s" : "" );
	    if( counts->cantmake )
		printf( "...can't make %d target%s...\n", counts->cantmake,
		        counts->cantmake > 1 ? "s" : "" );
	}

#ifdef OPT_HEADER_CACHE_EXT
	hcache_done();
#endif

	status = counts->cantfind || counts->cantmake;

	for( i = 0; i < n_targets; i++ )
	    status |= make1( bindtarget( targets[i] ) );

	return status;
}

/*
 * make0() - bind and scan everything to make a TARGET
 *
 * Make0() recursively binds a target, searches for #included headers,
 * calls itself on those headers, and calls itself on any dependents.
 */

static void
make0( 
	TARGET	*t,
	time_t	ptime,		/* parent target's timestamp */
	int	depth,		/* for display purposes */
	COUNTS	*counts,	/* for reporting */
	int	anyhow )	/* forcibly touch all (real) targets */
{
	TARGETS	*c;
	int	fate, hfate;
	time_t	last, leaf, hlast, hleaf;
	char	*flag = "";
#ifdef OPT_FIX_TARGET_VARIABLES_EXT
	SETTINGS *saved;
#endif	

#ifdef OPT_GRAPH_DEBUG_EXT
	int	savedFate, oldTimeStamp;
#endif

	if( DEBUG_MAKEPROG )
	    printf( "make\t--\t%s%s\n", spaces( depth ), t->name );

	/* 
	 * Step 1: don't remake if already trying or tried 
	 */

	switch( t->fate )
	{
	case T_FATE_MAKING:
	    printf( "warning: %s depends on itself\n", t->name );
	    return;

        /* Deal with TEMPORARY targets with multiple parents. When a missing
         * TEMPORARY target is determined to be stable, it inherits the
         * timestamp of the parent being checked, and is given a binding of
         * T_BIND_PARENTS. To avoid outdating parents with earlier modification
         * times, we set the target's time to the minimum time of all parents.
         */
        case T_FATE_STABLE:
            if ( t->binding == T_BIND_PARENTS && t->time > ptime && t->flags & T_FLAG_TEMP )
                t->time = ptime;
            return;
            
	default:
	    return;

	case T_FATE_INIT:
	    break;
	}

	t->fate = T_FATE_MAKING;

	/*
	 * Step 2: under the influence of "on target" variables,
	 * bind the target and search for headers.
	 */

	/* Step 2a: set "on target" variables. */

#ifdef OPT_FIX_TARGET_VARIABLES_EXT
	/* we must make a copy of the target's settings before pushing
	   since calling the target's HDRRULE can change these
	   settings. */
	saved = copysettings( t->settings );
	pushsettings( saved );
#else	
	pushsettings( t->settings );
#endif

	/* Step 2b: find and timestamp the target file (if it's a file). */

	if( t->binding == T_BIND_UNBOUND && !( t->flags & T_FLAG_NOTFILE ) )
	{
            t->boundname = search( t->name, &t->time );
	    t->binding = t->time ? T_BIND_EXISTS : T_BIND_MISSING;
	}

	/* If temp file doesn't exist but parent does, use parent */

	if( t->binding == T_BIND_MISSING && t->flags & T_FLAG_TEMP && ptime )
	    t->binding = T_BIND_PARENTS;

	/* Step 2c: If its a file, search for headers. */

#ifdef OPT_SEMAPHORE
	{
	    LIST *var = var_get( "JAM_SEMAPHORE" );
	    if( var )
	    {
		TARGET *semaphore = bindtarget( var->string );

		semaphore->progress = T_MAKE_SEMAPHORE;
		t->semaphore = semaphore;
	    }
	}
#endif

	if( t->binding == T_BIND_EXISTS )
	    headers( t );

	/* Step 2d: reset "on target" variables */

#ifdef OPT_FIX_TARGET_VARIABLES_EXT
	popsettings( saved );
	freesettings( saved );
#else
	popsettings( t->settings );
#endif

	/* 
	 * Pause for a little progress reporting 
	 */

	if( DEBUG_BIND )
	{
	    if( strcmp( t->name, t->boundname ) )
	    {
		printf( "bind\t--\t%s%s: %s\n",
			spaces( depth ), t->name, t->boundname );
	    }

	    switch( t->binding )
	    {
	    case T_BIND_UNBOUND:
	    case T_BIND_MISSING:
	    case T_BIND_PARENTS:
		printf( "time\t--\t%s%s: %s\n",
			spaces( depth ), t->name, target_bind[ t->binding ] );
		break;

	    case T_BIND_EXISTS:
		printf( "time\t--\t%s%s: %s",
			spaces( depth ), t->name, ctime( &t->time ) );
		break;
	    }
	}

	/* 
	 * Step 3: recursively make0() dependents 
	 */

	last = 0;
	leaf = 0;
	fate = T_FATE_STABLE;

	for( c = t->deps[ T_DEPS_DEPENDS ]; c; c = c->next )
	{
	    /* Pass our time or our parent's time down. */

	    int time = t->binding == T_BIND_PARENTS ? ptime : t->time;

	    make0( c->target, time, depth + 1, counts, anyhow );
	    leaf = max( leaf, c->target->leaf );
	    leaf = max( leaf, c->target->hleaf );

	    /* If LEAVES has been applied, we only heed the timestamps of */
	    /* the leaf source nodes. */

	    if( t->flags & T_FLAG_LEAVES )
	    {
		last = leaf;
		continue;
	    }

	    last = max( last, c->target->time );
	    last = max( last, c->target->htime );
#ifdef OPT_GRAPH_DEBUG_EXT
	    if( DEBUG_FATE )
		if( fate < c->target->fate )
		    printf( "fate change  %s from %s to %s by dependency %s\n",
			    t->name,
			    target_fate[fate], target_fate[c->target->fate],
			    c->target->name);
#endif
	    fate = max( fate, c->target->fate );
#ifdef OPT_GRAPH_DEBUG_EXT
	    if( DEBUG_FATE )
		if( fate < c->target->hfate )
		    printf( "fate change  %s from %s to %s by headers in %s\n",
			    t->name,
			    target_fate[fate], target_fate[c->target->hfate],
			    c->target->name);
#endif
	    fate = max( fate, c->target->hfate );
	}

	/* If a NOUPDATE file exists, make dependents eternally old. */

	if( t->flags & T_FLAG_NOUPDATE )
	{
#ifdef OPT_GRAPH_DEBUG_EXT
	    if( DEBUG_FATE )
		if( fate != T_FATE_STABLE )
		    printf( "fate change  %s back to stable, NOUPDATE.\n",
			   t->name);
#endif
	    last = 0;
	    t->time = 0;

	    /*
	     * Don't inherit our fate from our dependents.  Decide fate
	     * based only upon other flags and our binding (done later).
	     */

	    fate = T_FATE_STABLE;
	}

	/* Step 3b: determine fate: rebuild target or what? */

	/* 
	    In English:
		If can't find or make child, can't make target.
		If children changed, make target.
		If target missing, make it.
		If children newer, make target.
		If temp's children newer than parent, make temp.
		If deliberately touched, make it.
		If up-to-date temp file present, use it.
		If target newer than parent, mark target newer.
		Don't propagate child's "newer" status.
	*/
#ifdef OPT_GRAPH_DEBUG_EXT
	savedFate = fate;
	oldTimeStamp = 0;
#endif

	if( fate >= T_FATE_BROKEN )
	{
	    fate = T_FATE_CANTMAKE;
	}
	else if( fate >= T_FATE_SPOIL )
	{
	    fate = T_FATE_UPDATE;
	}
	else if( t->binding == T_BIND_MISSING )
	{
	    fate = T_FATE_MISSING;
	}
	else if( t->binding == T_BIND_EXISTS && last > t->time )
	{
#ifdef OPT_GRAPH_DEBUG_EXT
	    oldTimeStamp = 1;
#endif
	    fate = T_FATE_OUTDATED;
	}
	else if( t->binding == T_BIND_PARENTS && last > ptime )
	{
#ifdef OPT_GRAPH_DEBUG_EXT
	    oldTimeStamp = 1;
#endif
	    fate = T_FATE_OUTDATED;
	}
	else if( t->flags & T_FLAG_TOUCHED )
	{
	    fate = T_FATE_TOUCHED;
	}
	else if( anyhow && !( t->flags & T_FLAG_NOUPDATE ) )
	{
	    fate = T_FATE_TOUCHED;
	}
	else if( t->binding == T_BIND_EXISTS && t->flags & T_FLAG_TEMP )
	{
	    fate = T_FATE_ISTMP;
	}
	else if( t->binding == T_BIND_EXISTS && t->time > ptime )
	{
#ifdef OPT_GRAPH_DEBUG_EXT
	    oldTimeStamp = 1;
#endif
	    fate = T_FATE_NEWER;
	}
	else if( fate == T_FATE_NEWER )
	{
	    fate = T_FATE_STABLE;
	}
#ifdef OPT_GRAPH_DEBUG_EXT
	if( DEBUG_FATE && fate != savedFate )
	    if( savedFate == T_FATE_STABLE )
		printf( "fate change  %s set to %s%s\n",
		       t->name, target_fate[fate],
		       oldTimeStamp ? " (by timestamp)" : "" );
	    else
		printf( "fate change  %s adjusted from %s to %s%s\n",
		       t->name, target_fate[savedFate], target_fate[fate],
		       oldTimeStamp ? " (by timestamp)" : "" );
#endif

	/* Step 3c: handle missing files */
	/* If it's missing and there are no actions to create it, boom. */
	/* If we can't make a target we don't care about, 'sokay */
	/* We could insist that there are updating actions for all missing */
	/* files, but if they have dependents we just pretend it's NOTFILE. */

	if( fate == T_FATE_MISSING && 
		!t->actions && 
		!t->deps[ T_DEPS_DEPENDS ] )
	{
	    if( t->flags & T_FLAG_NOCARE )
	    {
#ifdef OPT_GRAPH_DEBUG_EXT
		if( DEBUG_FATE )
		    printf( "fate change  %s to STABLE from %s, "
			    "no actions, no dependents and don't care\n",
			    t->name, target_fate[fate]);
#endif
		fate = T_FATE_STABLE;
	    }
	    else
	    {
		printf( "don't know how to make %s\n", t->name );

		fate = T_FATE_CANTFIND;
	    }
	}

	/* Step 3d: propagate dependents' time & fate. */
	/* Set leaf time to be our time only if this is a leaf. */

	t->time = max( t->time, last );
	t->leaf = leaf ? leaf : t->time ;
	t->fate = fate;

	/* Step 3e: sort dependents by their update time. */

	if( globs.newestfirst )
	    t->deps[ T_DEPS_DEPENDS ] = make0sort( t->deps[ T_DEPS_DEPENDS ] );

	/*
	 * Step 4: Recursively make0() headers.
	 */

	/* Step 4a: recursively make0() headers */

	hlast = 0;
	hleaf = 0;
	hfate = T_FATE_STABLE;

	for( c = t->deps[ T_DEPS_INCLUDES ]; c; c = c->next )
	{
	    make0( c->target, ptime, depth + 1, counts, anyhow );
	    hlast = max( hlast, c->target->time );
	    hlast = max( hlast, c->target->htime );
	    hleaf = max( hleaf, c->target->leaf );
	    hleaf = max( hleaf, c->target->hleaf );
#ifdef OPT_GRAPH_DEBUG_EXT
	    if( DEBUG_FATE )
		if( hfate < c->target->fate )
		    printf( "hfate change %s from %s to %s by "
			    "dependency %s\n",
			    t->name,
			    target_fate[hfate], target_fate[c->target->fate],
			    c->target->name);
#endif
	    hfate = max( hfate, c->target->fate );
#ifdef OPT_GRAPH_DEBUG_EXT
	    if( DEBUG_FATE )
		if( hfate < c->target->hfate )
		    printf( "hfate change %s from %s to %s by "
			    "headers in %s\n",
			    t->name,
			    target_fate[hfate], target_fate[c->target->hfate],
			    c->target->name);
#endif
	    hfate = max( hfate, c->target->hfate );
	}

	/* Step 4b: propagate dependents' time & fate. */

	t->htime = hlast;
	t->hleaf = hleaf ? hleaf : t->htime;
	t->hfate = hfate;

	/* 
	 * Step 5: a little harmless tabulating for tracing purposes 
	 */

#ifdef OPT_IMPROVED_PATIENCE_EXT
	++counts->targets;
#else	
	if( !( ++counts->targets % 1000 ) && DEBUG_MAKE )
	    printf( "...patience...\n" );
#endif

	if( fate == T_FATE_ISTMP )
	    counts->temp++;
	else if( fate == T_FATE_CANTFIND )
	    counts->cantfind++;
	else if( fate == T_FATE_CANTMAKE && t->actions )
	    counts->cantmake++;
	else if( fate >= T_FATE_BUILD && fate < T_FATE_BROKEN && t->actions )
	    counts->updating++;

	if( !( t->flags & T_FLAG_NOTFILE ) && fate >= T_FATE_SPOIL )
	    flag = "+";
	else if( t->binding == T_BIND_EXISTS && ptime && t->time > ptime )
	    flag = "*";

	if( DEBUG_MAKEPROG )
	    printf( "made%s\t%s\t%s%s\n", 
		flag, target_fate[ t->fate ], 
		spaces( depth ), t->name );
}

#ifdef OPT_GRAPH_DEBUG_EXT

/*
 * dependGraphOutput() - output the DG after make0 has run
 */

static void
dependGraphOutput( TARGET *t, int depth )
{
    TARGETS	*c;

    if (   (t->flags & T_FLAG_VISITED) != 0
	|| !t->name
	|| !t->boundname)
	return;

    t->flags |= T_FLAG_VISITED;

    switch (t->fate)
    {
      case T_FATE_TOUCHED:
      case T_FATE_MISSING:
      case T_FATE_OUTDATED:
      case T_FATE_UPDATE:
	printf( "->%s%2d Name: %s\n", spaces(depth), depth, t->name );
	break;
      default:
	printf( "  %s%2d Name: %s\n", spaces(depth), depth, t->name );
	break;
    }

    if( strcmp (t->name, t->boundname) )
    {
	printf( "  %s    Loc: %s\n", spaces(depth), t->boundname );
    }

    switch( t->fate )
    {
      case T_FATE_STABLE:
	printf( "  %s       : Stable\n", spaces(depth) );
	break;
      case T_FATE_NEWER:
	printf( "  %s       : Newer\n", spaces(depth) );
	break;
      case T_FATE_ISTMP:
	printf( "  %s       : Up to date temp file\n", spaces(depth) );
	break;
      case T_FATE_TOUCHED:
        printf( "  %s       : Been touched, updating it\n", spaces(depth) );
	break;
      case T_FATE_MISSING:
	printf( "  %s       : Missing, creating it\n", spaces(depth) );
	break;
      case T_FATE_OUTDATED:
	printf( "  %s       : Outdated, updating it\n", spaces(depth) );
	break;
      case T_FATE_UPDATE:
	printf( "  %s       : Updating it\n", spaces(depth) );
	break;
      case T_FATE_CANTFIND:
	printf( "  %s       : Can't find it\n", spaces(depth) );
	break;
      case T_FATE_CANTMAKE:
	printf( "  %s       : Can't make it\n", spaces(depth) );
	break;
    }

    if( t->flags & ~T_FLAG_VISITED )
    {
	printf( "  %s       : ", spaces(depth) );
	if( t->flags & T_FLAG_TEMP ) printf ("TEMPORARY ");
	if( t->flags & T_FLAG_NOCARE ) printf ("NOCARE ");
	if( t->flags & T_FLAG_NOTFILE ) printf ("NOTFILE ");
	if( t->flags & T_FLAG_TOUCHED ) printf ("TOUCHED ");
	if( t->flags & T_FLAG_LEAVES ) printf ("LEAVES ");
	if( t->flags & T_FLAG_NOUPDATE ) printf ("NOUPDATE ");
	printf( "\n" );
    }

    for( c = t->deps[ T_DEPS_DEPENDS ]; c; c = c->next )
    {
	printf( "  %s       : Depends on %s (%s)\n", spaces(depth),
	       c->target->name, target_fate[ c->target->fate ] );
    }

    for( c = t->deps[ T_DEPS_INCLUDES ]; c; c = c->next )
    {
	printf( "  %s       : Includes %s (%s)\n", spaces(depth),
	       c->target->name, target_fate[ c->target->fate ] );
    }

    for( c = t->deps[ T_DEPS_DEPENDS ]; c; c = c->next )
    {
	dependGraphOutput( c->target, depth + 1 );
    }

    for( c = t->deps[ T_DEPS_INCLUDES ]; c; c = c->next )
    {
	dependGraphOutput( c->target, depth + 1 );
    }

}

#endif
/*
 * make0sort() - reorder TARGETS chain by their time (newest to oldest)
 */

static TARGETS *
make0sort( TARGETS *chain )
{
	TARGETS *result = 0;

	/* We walk chain, taking each item and inserting it on the */
	/* sorted result, with newest items at the front.  This involves */
	/* updating each TARGETS' c->next and c->tail.  Note that we */
	/* make c->tail a valid prev pointer for every entry.  Normally, */
	/* it is only valid at the head, where prev == tail.  Note also */
	/* that while tail is a loop, next ends at the end of the chain. */

	/* Walk current target list */

	while( chain )
	{
	    TARGETS *c = chain;
	    TARGETS *s = result;

	    chain = chain->next;

	    /* Find point s in result for c */

	    while( s && s->target->time > c->target->time )
		s = s->next;

	    /* Insert c in front of s (might be 0). */
	    /* Don't even think of deciphering this. */

	    c->next = s;			/* good even if s = 0 */
	    if( result == s ) result = c;	/* new head of chain? */
	    if( !s ) s = result;		/* wrap to ensure a next */
	    if( result != c ) s->tail->next = c; /* not head? be prev's next */
	    c->tail = s->tail;			/* take on next's prev */
	    s->tail = c;			/* make next's prev us */
	}

	return result;
}

static LIST *targets_to_update_ = 0;

void mark_target_for_updating(char *target)
{
    targets_to_update_ = list_new( targets_to_update_, target );
}

LIST *targets_to_update()
{
    return targets_to_update_;
}

void clear_targets_to_update()
{
    list_free(targets_to_update_);
    targets_to_update_ = 0;
}
