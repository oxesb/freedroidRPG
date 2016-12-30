BEGIN { in_obs_ext = 0; in_terminal = 0; outf = ""; dlst = ""; }

/^Levelnumber:/ {
	lvlnumber = $2;
	if (lvlnumber < act2lvl) {
		sublvl = 0;
		print lvlnumber > "tmp/act1/levels.lst";
		outf = "tmp/act1/levels/level"lvlnumber".dat";
		dlst = "tmp/act1/terminals.lst";
	} else {
		sublvl = act2lvl;
		lvlnumber = lvlnumber - sublvl;
		print lvlnumber > "tmp/act2/levels.lst";
		outf = "tmp/act2/levels/level"lvlnumber".dat";
		dlst = "tmp/act2/terminals.lst";
	}
	$2 = lvlnumber;
}

/^jump target north:/ {
	lvlnorth = $4;
	if (lvlnorth != -1) $4 = lvlnorth - sublvl;
}

/^jump target south:/ {
	lvlsouth = $4;
	if (lvlsouth != -1) $4 = lvlsouth - sublvl;
}

/^jump target east:/ {
	lvleast = $4;
	if (lvleast != -1) $4 = lvleast - sublvl;
}

/^jump target west:/ {
	lvlwest = $4;
	if (lvlwest != -1) $4 = lvlwest - sublvl;
}

/^obstacleextensions/ {
	in_obs_ext = 1;
}

/^\/obstacleextensions/ {
	in_obs_ext = 0;
}

in_obs_ext == 1 && /^idx=.* type=32/ {
	in_terminal = 1;
}

in_terminal == 1 && /^}/ {
	in_terminal = 0;
}

in_terminal == 1 && /^.*".*"/ {
	split($0, tokens, "\"");
	print tokens[2] > dlst;
}

/^----------/ {
	outf = "";
	next;
}

{ if (outf != "") print $0 > outf; }

END {}

