#!/bin/bash

generate_levels_dat()
{
	list=$1
	actdir=$2
	outf=$actdir"/levels.dat"

	echo "" > $outf
	for F in $(< $list) ; do
		file=$actdir"/levels/level"$F".dat"
		cat $file >> $outf
		echo "----------------------------------------------------------------------" >> $outf
	done
	echo "*** End of Ship Data ***" >> $outf
	echo "" >> $outf
}

generate_droids()
{
	list=$1
	actdir=$2
	outf=$actdir"/ReturnOfTux.droids"

	for F in $(< $list) ; do
		file=$actdir"/droids/level"$F".droids"
		echo "** Beginning of new Level **" >> $outf
		cat $file >> $outf
		echo "** End of this levels Special Forces data **" >> $outf
		echo "--------------------------------------------------------" >> $outf
	done
	echo "*** End of Droid Data ***" >> $outf
}

generate_events()
{
	list=$1
	actdir=$2
	outf=$actdir"/events.dat"

	cat events_dat.header > $outf
	for F in $(< $list) ; do
		file=$actdir"/events/level"$F".events"
		if [[ ! -f $file ]] ; then
			echo "##############" >> $outf
			echo "## Level "$F" ##" >> $outf
			echo "##############" >> $outf
			echo "" >> $outf
		else
			echo "##############" >> $outf
			cat $file >> $outf
		fi
	done
	cat events_dat.footer >> $outf
}

generate_quests()
{
	list=$1
	from=$2
	actdir=$3
	outf=$actdir"/quests.dat"

	cat quests_dat.header > $outf
	while read F ; do
		file="\""$from"/"$F"\""
		eval "cat $file >> $outf"
		echo "" >> $outf
	done < $list
	echo "*** END OF QUEST LIST *** LEAVE THIS TERMINATOR IN HERE ***" >> $outf
	echo "" >> $outf
}

generate_all()
{
	act=$1

	echo "Generate "$act"/levels.dat"
	generate_levels_dat tmp/$act/levels.lst tmp/$act

	echo "Generate "$act"/ReturnOfTux.droids"
	generate_droids tmp/$act/levels.lst tmp/$act

	echo "Generate "$act"/events.dat"
	generate_events tmp/$act/levels.lst tmp/$act

	echo "Get list of "$act" dialogs"
	[[ ! -f tmp/$act/terminals.lst ]] && touch tmp/$act/terminals.lst
	cat tmp/$act/dialogs.lst tmp/$act/terminals.lst | sort | uniq > tmp/$act/dialogs.tmp
	cp tmp/$act/dialogs.tmp tmp/$act/dialogs.lst
	while [[ -s tmp/$act/dialogs.tmp ]] ; do
		[[ -f tmp/$act/dialogs.tmp2 ]] && rm tmp/$act/dialogs.tmp2
		for F in $(< tmp/$act/dialogs.tmp) ; do
			if [[ ! -f ../base/dialogs/$F.lua ]] ; then
				grep "trade_with" act1/dialogs/$F.lua | awk '{ split($0, tokens, "\""); print tokens[2]; }' >> tmp/$act/dialogs.tmp2
				grep "create_droid" act1/dialogs/$F.lua | awk '{ split($0, tokens, "\""); print tokens[8]; }' >> tmp/$act/dialogs.tmp2
				grep "sell_item" act1/dialogs/$F.lua | awk '{ split($0, tokens, "\""); print tokens[4]; }' >> tmp/$act/dialogs.tmp2
			fi
		done
		rm -f tmp/$act/dialogs.tmp
		if [[ -s tmp/$act/dialogs.tmp2 ]] ; then
			for F in $(cat tmp/$act/dialogs.tmp2 | sort | uniq) ; do
				if ! $(grep -q $F tmp/$act/dialogs.lst) ; then
					echo $F >> tmp/$act/dialogs.lst
					echo $F >> tmp/$act/dialogs.tmp
				fi
			done
			rm -f tmp/$act/dialogs.tmp2
		fi
	done
	[[ -f tmp/$act/dialogs.tmp2 ]] && rm tmp/$act/dialogs.tmp2
	
	echo "Copy "$act" droids dialogs"
	for F in `cat tmp/$act/dialogs.lst` ; do
		if [[ ! -f ../base/dialogs/$F.lua ]] ; then
			cp act1/dialogs/$F.lua tmp/$act/dialogs
			#TODO: adapt levelnum in scripts, if needed
		fi
	done

	echo "Extract "$act" quest names"
	grep add_quest tmp/$act/dialogs/*.lua | awk '{ patsplit($0, tokens, "\"[^\"]*\""); split(tokens[1], name, "\""); print name[2]; }' >> tmp/$act/quests.lst
	grep add_quest tmp/$act/events.dat | awk '{ patsplit($0, tokens, "\"[^\"]*\""); split(tokens[1], name, "\""); print name[2]; }' >> tmp/$act/quests.lst
 	
	echo "Generate "$act" quests.dat"
	generate_quests tmp/$act/quests.lst tmp/quests tmp/$act
}

#####################

[ -d tmp ] && rm -rf tmp
mkdir -p tmp/act1/{levels,droids,events,dialogs}
mkdir -p tmp/act2/{levels,droids,events,dialogs}
mkdir -p tmp/quests

echo "Split levels"
awk -v act2lvl=75 -f extract_levels.awk < act1/levels.dat

echo "Split droids"
awk -v act2lvl=75 -f extract_droids.awk < act1/ReturnOfTux.droids

echo "Split events"
awk -v act2lvl=75 -f extract_events.awk < act1/events.dat

echo "Split quests"
awk -v act2lvl=75 -f extract_quests.awk < act1/quests.dat

generate_all act1
generate_all act2

echo ""
echo "Data files have been generated in tmp/act1 and tmp/act2"

# Some 'manual' steps
cp -R act1/titles tmp/act1
mkdir -p tmp/act2/titles
cp act1/lvledit_exceptions.dat tmp/act1
cp lvledit_dat.header tmp/act2/lvledit_exceptions.dat

echo "Some manual steps could be needed:"
echo "  - move act2 related titles from tmp/act1/titles to tmp/act2/titles"
echo "  - extract act2 related lvledit exceptions from tmp/act1/lvledit_exceptions.dat"
echo "    and copy them into tmp/act2/lvledit_exceptions.dat"


rm -rf tmp/act1/{levels,droids,events}
rm -rf tmp/act2/{levels,droids,events}
rm -f tmp/act1/*.lst
rm -f tmp/act2/*.lst
rm -rf tmp/quests


