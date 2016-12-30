BEGIN { in_quest = 0; outf = ""; }

/^** Start of this mission target subsection **/ {
	in_quest = 1;
	quest_content = $0;
	next;
}

/^** End of this mission target subsection **/ {
	quest_content = quest_content "\n" $0;
	in_quest = 0;
	if (outf != "") print quest_content > outf;
	outf = "";
}

in_quest && /^Mission Name=/ {
	split($0, tokens, "\"");
	outf = "tmp/quests/"tokens[2];
}

in_quest==1 { quest_content = quest_content "\n" $0; }

