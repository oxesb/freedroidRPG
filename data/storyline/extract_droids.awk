BEGIN { outf = ""; }

/^Level=/ {
	split($0, tokens, "=");
	lvlnumber = tokens[2];
	if (lvlnumber < act2lvl) {
		sublvl = 0;
		outf = "tmp/act1/droids/level"lvlnumber".droids";
		dlgf = "tmp/act1/dialogs.lst";
	} else {
		sublvl = act2lvl;
		lvlnumber = lvlnumber - sublvl;
		outf = "tmp/act2/droids/level"lvlnumber".droids";
		dlgf = "tmp/act2/dialogs.lst";
	}
	$0 = "Level="lvlnumber;
}

/^** End of this levels Special Forces data **/ {
	outf = "";
	next;
}

/UseDialog=/ {
	patsplit($0, tokens, "UseDialog=\"[^\"]*\"");
	split(tokens[1], dialog, "=");
	gsub("\"", "", dialog[2]);
	print dialog[2] > dlgf;
}

{ if (outf != "") print $0 > outf; }

END {}

