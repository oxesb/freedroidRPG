BEGIN { cpt = 0; outf = ""; }

/^## Level/ {
	cpt = 0;
	lvlnumber = $3;
	if (lvlnumber < act2lvl) {
		sublvl = 0;
		outf = "tmp/act1/events/level"lvlnumber".events";
	} else {
		sublvl = act2lvl;
		lvlnumber = lvlnumber - sublvl;
		outf = "tmp/act2/events/level"lvlnumber".events";
	}
	$3 = lvlnumber;
}

/^##############$/ {
	cpt = cpt + 1;
	if (cpt == 2) {
		outf = "";
		cpt = 0;
		next;
	}
}

/Enemy level=/ {
	split($0, tokens, "=");
	gsub(tokens[2], tokens[2] - sublvl);
}

/Exiting level=/ {
	split($0, tokens, "=");
	gsub(tokens[2], tokens[2] - sublvl);
}

/Entering level=/ {
	split($0, tokens, "=");
	gsub(tokens[2], tokens[2] - sublvl);
}

/add_obstacle/ {
	patsplit($0, tokens, "add_obstacle.[^,]*", seps)
	split(tokens[1], lvlnum, "(");
	$0 = seps[0] "add_obstacle(" lvlnum[2]-sublvl "" seps[1];
}

/add_volatile_obstacle/ {
	patsplit($0, tokens, "add_volatile_obstacle.[^,]*", seps)
	split(tokens[1], lvlnum, "(");
	$0 = seps[0] "add_volatile_obstacle(" lvlnum[2]-sublvl "" seps[1];
}

/^** End of Mission Event Section **/ {
	outf = "";
}

{ if (outf != "") print $0 > outf; }

END {}

