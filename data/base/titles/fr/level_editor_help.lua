---------------------------------------------------------------------
-- This file is part of Freedroid
--
-- Freedroid is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- Freedroid is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Freedroid; see the file COPYING. If not, write to the
-- Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
-- MA 02111-1307 USA
----------------------------------------------------------------------

title_screen{
song = "Bleostrada.ogg",
text = [[
            L’ÉDITEUR DE NIVEAUX DE FREEDROIDRPG

=== INTRODUCTION ===

FreedroidRPG s'accompagne d'un éditeur de niveau intégré. Cette éditeur de niveau permet de contrôler tous les aspects d'une carte de FreedroidRPG et de la sauvegarder.

Vous pouvez y accéder depuis le menu principale (choisissez "Editeur de Niveau") ou en executant la commande "freedroidRPG -e".

    --- Infobulles ---
Pour activer/désactiver les descriptions de l'interface pendant le survol de la souris, cliquez sur l'icône de bulle de discours blanc situé près du bord de la fenêtre de droite (rangée inférieure de bouton).

    --- Détail sommaire ---
Un détail sommaire sur les obstacles et les objets sera affichés si vous cliquez droit sur eux dans le sélecteur d'objet supérieur.

    --- Navigation ---
Pour modifier le niveau actuel, cliquez sur le numéro de niveau à proximité de la minicarte dans le coin inférieur droit, ou sélectionner le niveau désiré dans le menu de l'éditeur (décrit plus loin).

    --- Modifier une carte ---
Il y a quatre modes de modification : Modification des obstacles, Modification du sol, modification des objets, et modification des trajectoires.

Le bouton sélectionné en bas à gauche indique les objets que vous pouvez sélectionner ou placer.
Lorsqu'un bouton est sélectionné, et vous êtes en mode placement, l'objet que vous placerez est indiqué par le ruban en haut de l'écran. La sélection dans le ruban est divisée par onglets immédiatement dessous.

Vous pouvez sélectionner le type d'obstacle que vous souhaitez placer sur la carte dans le sélecteur d'objet supérieur. Il suffit de cliquer dessus pour le sélectionner. Les Obstacles sont divisés en groupes pour fournir la meilleure vue d'ensemble.

En appuyant sur espace, vous entrerez dans le mode de sélection étant indiqué par le curseur changeant. Vous ne pouvez sélectionner que des groupes d'objets représentés par le mode activé.
Remarque importante : vous ne pourrez sélectionner les choses qui sont inclus dans le mode actuel. Si vous êtes en mode de l'obstacle, vous ne serez pas en mesure de sélectionner des éléments ou des carreaux de sol.


        Mode édition d'obstacle :

Pour sélectionner ce mode, cliquez sur le bouton intitulé 'Obstacle' dans le sélecteur de catégorie dans la zone en bas à gauche.
En ayant sélectionné un obstacle, cliquer à un endroit sur la carte pour le placer à la position du curseur.
Si la position du click est un peu imprécise, vous pouvez aussi utiliser le clavier numérique pour placer les obstacles.
Cliquer sur le bouton le plus à gauche (celui avec une petite grille) des cinq boutons sur le haut du sélecteur de catégorie, pour afficher une grille numérotée. Utiliser un clic gauche pour afficher ou non la grille, et un clic droit pour changer le type de grille.
Les numéros correspondent à la disposition des chiffres sur votre clavier numérique. Un appui sur '1' place l'obstacle qui est en surbrillance dans le sélecteur d'objet à la position du chiffre '1' de la grille violette.
Placer une ligne de murs de cette façon peut être un peu inefficace, vous pouvez donc plutôt simplement maintenir appuyé le bouton gauche de la souris et une ligne de murs sera placée au fur et à mesure du déplacement du curseur, si un objet de type mur est sélectionné. Cela fonctionne avec la plupart des murs disponibles dans FreedroidRPG.
Pendant que le bouton gauche de la souris est appuyé pour placer des murs, un click avec le bouton droit de la souris enlèvera tous les murs qui ont été dessinés depuis le moment où le bouton gauche de la souris a été appuyé.
Il y a quelques objets spéciaux. Les murs de verre et les murs de briques craquelées, mais également les barils et les caisses, qui peuvent être détruits en quelques coups, les deux derniers pouvant également libérer des articles. Les coffres peuvent être ouverts et peuvent aussi contenir des objets.
Le symbole représentant des traces de pas dans un cercle rouge barré n'est pas réellement un objet mais une pure zone de blocage invisible ('un rectangle de collision'). Les rectangles de collisions sont un constituant essentiel de chaque objet, puisqu'ils vous empêchent de les traverser, contrairement aux points de navigations et aux tuiles de sol.

            Sélection d'obstacles

En maintenant appuyé le bouton gauche de la souris, vous pouvez sélectionner un rectangle d'obstacles. Après avoir relâché le bouton de souris, les obstacles sélectionnés seront affichés dans une couleur différente pour indiquer leur sélection. Pour sélectionner des obstacles qui ne peuvent être atteints par un rectangle de sélection, appuyez sur 'Ctrl' et cliquez sur l'obstacle ou sélectionnez un autre rectangle d'obstacles.
Plusieurs obstacles ont éventuellement pu être sélectionnés par un seul clic. Vous pouvez parcourir ces obstacles en cliquant sur l'icône représentant un fauteuil et une étagère, ou en utilisant la touche 'n'.
L'icône de la poubelle permet de supprimer l'obstacle sélectionné.
Vous pouvez aussi couper (Ctrl+x, qui peut aussi être utilisé pour supprimer des obstacles en ne les collant pas de nouveau :)), copier (Ctrl+c) et coller (ctrl+v) les obstacles coupés ou copiés.
Vous pouvez aussi déplacer les obstacles sélectionnés en maintenant la touche 'maj' gauche appuyée tout en faisant glisser les obstacles sur la carte. Cependant, cette manipulation peut être relativement imprécise.

            Placement d'objets dans les Coffres

Il suffit de sélectionner le coffre désiré et de cliquer sur le bouton le plus à gauche de la rangée la plus haute.
Vous serez redirigés vers un écran qui ressemble à l'écran des boutiques.
Un couteau sera affiché (qui n'est en fait pas réellement placé dans le coffre), sélectionnez-le et cliquez sur le bouton 'sell' (vendre).
Sélectionner les objets que vous voulez voir apparaître quand le joueur ouvre le coffre.
Ces objets seront affichés sur la barre du haut de l'écran de boutique.
Pour supprimer un de ces objets, sélectionnez-le et cliquez sur le bouton 'buy' (acheter).
La croix rouge permet de fermer l'écran.

            Ajouter du Texte à un Panneau

Sélectionnez le panneau et ajoutez-lui un (court) texte en utilisant une étiquette d'obstacle (bouton avec un 'O').  Enregistrez la carte et sortez.
Ouvrez le fichier de définition des niveaux (map/levels.dat) et cherchez l'étiquette d'obstacle que vous avez utilisée. Modifiez la ligne située juste au dessus, en remplaçant "type=30" par "type=32" et enregistrez.
Maintenant, en cliquant sur le panneau dans le jeu, votre court message sera affiché.

            Ajouter un Dialogue à un Terminal

Sélectionnez le terminal et ajoutez-lui le nom de dialogue que vous voulez en utilisant une étiquette d'obstacle (bouton avec un 'O'). Enregistrez la carte et sortez.
Ouvrez le fichier de définition des niveaux (maps/levels.dat) et cherchez l'étiquette d’obstacle que vous avez utilisée.
Modifiez la ligne située juste au dessus en remplaçant "type=30" par "type=32" et enregistrez. Maintenant, en cliquant sur le terminal dans le jeu, le dialogue choisi sera démarré.

        Mode d'édition du sol :

Le mode d'édition du sol fonctionne de manière similaire au mode d'édition d'obstacle. Vous pouvez sélectionner différents types de sol en utilisant le sélecteur d'objet.
Pour remplir une région avec un même type de sol, sélectionnez d'abord la tuile à utiliser, puis laissez appuyé le bouton gauche de la souris et déplacez-là jusqu'à couvrir la région désirée. Les tuiles de sol sont placées sur la couche de sol courante.
Il n'y a aucun sol qui soit d'un type particulier, ce ne sont que des éléments de décor.

La visibilité d'une couche de sol peut être contrôlée par un bouton avec une icône représentant plusieurs couches. Le bouton n'est affiché que pour les niveaux disposant d'un sol multi-couches.
Un clic gauche sur le bouton alterne entre l'affichage d'une seule couche de sol et l'affichage de toutes les couches. Un clic droit change la couche de sol courante.

            Sélectionner le type de sol

La sélection est aussi simple que pour les obstacles. Les tuiles de sol peuvent être déplacée avec la méthode décrite plus haut.
Pour les niveaux avec un sol multi-couches, seuls les couches visibles sont sélectionnés. Quand une seule couche de sol est visible, seules les tuiles de la couche courante sont sélectionnées.

Afin d'avoir une vue d'ensemble uniquement du sol, cliquez sur le bouton avec une icône de lampe afin de ne pas afficher les obstacles. Un clic supplémentaire permettra de ré-afficher les obstacles.
L'icône avec un rectangle turquoise affiche les rectangles de collisions. Ces rectangles indiquent la zone de blocage d'un obstacle. Tux ne peut pas traverser un telle zone.
Si vous activez cet affichage et que vous testez votre niveau (explication plus tard); les rectangles restent affichés ce qui est utile pour tester si le joueur peut passer ou non à travers un trou.

        Mode d'édition des objets :

Vous pouvez aussi placer sur la carte des objets qui pourront être utilisés par le joueur.
Ces objets pourront être ramassés par le joueur. Ils peuvent être portés, certains peuvent même être utilisés ou servir à l'équipement.
Certains objets permettent d'avancer dans l'histoire, d'autres amènent des bonus au joueur, quand d'autres n'ont absolument aucune utilité.
Sélectionner le mode d'édition des objets et cliquer sur l'un des objets affichés dans le sélecteur. Pour certains objets, vous devez spécifier une quantité avant qu'ils soient déposés.
Vous pouvez la définir en cliquant sur les touches fléchées ou en déplaçant la boule bleue vers la gauche ou la droite.
Utilisez 'g' pour avoir une meilleure vue d'ensemble des objets disponibles (peut aussi être utilisé pour le dépôt, les objets seront déposés à l'emplacement de la mire de curseur). Appuyez sur 'Échap' pour annuler le processus sans déposer un quelconque objet.
Vous pouvez aussi cliquer sur l'icône avec les bottes dans un cercle barré pour faire la même chose.


        Mode d'édition des points de passage :

Actuellement, les robots (c'est-à-dire tous les personnages non-joueurs) se déplacent sur les cartes en utilisant des points de passage prédéfinis.
Pour placer un point de passage, appuyez sur la touche 'w'. Cela place ou enlève un point de passage au niveau de la mire de curseur.
Vous pouvez aussi cliquer sur la carte à la position où vous voulez avoir un point de passage, si ce mode est activé. Un clic à un autre endroit place un autre point de passage et connecte automatiquement le dernier point sélectionné avec celui-ci.
Cliquer sur un point de passage déjà existant vous permet de le connecter avec un autre point (il suffit de cliquer l'autre point, pour ce faire).
Cependant, il y a une différence entre ces deux méthodes de placement. Quand vous connectez deux points de passage avec le clavier, les connexions sont unidirectionnelles.
Cela signifie que lorsque vous créez une connexion du point A au point B, le robot ne pourra marcher que du point A vers le point B et pas dans l'autre sens.
Vous pouvez enlever une connexion unidirectionnelle en la 'recouvrant' avec une autre connexion allant dans la même direction que celle que vous voulez enlever (cela ne fonctionne pas avec des connexions bidirectionnelles).
Les connexions bidirectionnelles sont automatiquement supprimées en utilisant la méthode de connexion par clic.
Note importante : Il n'est pas possible de connecter des points de passage appartenant à des cartes différentes !
Les points de passage sont également utilisés pour placer les robots engendrés aléatoirement. Cependant ce peut être inapproprié pour certains points. 
Il y a les points de passage 'normaux' qui sont blancs, utilisés pour régénérer des robots et les 'spéciaux', qui sont rouges, utilisés par les PNJs. Les normaux sont utilisés pour les robots générés, les rouges sont utilisés pour les PNJs.
Vous pouvez choisir entre ces différentes types de point de passage dans la barre de sélection en haut.  Pour changer un point normal en en rouge, ou l'inverse, appuyez sur maj+w.
Merci de veiller à ce que les chemins entre points de passage ne soient pas bloqués par des obstacles situés entre deux points.
Pour vérifier cela automatiquement pour une carte, vous pouvez utiliser le validateur de niveau, comme expliqué ci-dessous.


        Placer des Étiquettes :

Il y a deux sortes de tables : les étiquettes de carte et les étiquettes d'obstacles.
Assurez-vous que chaque identifiant d'étiquette est unique.
Utiliser un identifiant vide permet de supprimer l'étiquette en question.


            Placer des étiquettes de carte

Les étiquettes de carte sont utilisées pour définir les positions de départ des PNJs (voir ReturnOfTux.droids), les événements qui se produisent lorsque Tux passe sur elles (voir events.dat), ou les positions utilisées pour le déplacement des PNJs par l'intermédiaire de scripts lua (events, quests, et les dialogues).
Pour définir une nouvelle étiquette de carte, appuyez sur la touche 'm' du clavier ou cliquez le bouton avec un signe M. Il vous sera demander l'identifiant de l'étiquette de carte. Notez qu'un cercle de plusieurs couleurs apparaîtra sur la tuile de sol à laquelle a été attribuée une étiquette de carte..
L'étiquette de carte sera automatiquement placée sur la tuile qui est au milieu de l'écran.
Vous pouvez activer ou désactiver l'affichage des robtos/PNJs en cliquant sur le bouton avec l'image d'un 302.

            Placer des étiquettes d'obstacle

Les étiquettes d'obstacles sont importantes pour permettre de marquer les obstacles afin de réagir à des événements (par exemple durant une quête). Si, par exemple, un événement est supposé enlevé un obstacle mural particulier, alors cet obstacle doit posséder un nom ou identifiant, afin qu'il puisse être référencé dans la définition de l'événement.
Vous pouvez aussi les utiliser pour ajouter des dialogues à des obstacles, afin de pouvoir leur parler comme à des PNJs.
Pour placer une étiquette sur un obstacle, vous devez d'abord sélectionner cet obstacle (voir l'explication ci-dessus sur le mode d'édition d'obstacle).
En cliquant sur l'icône avec un 'O', il vous sera demandé la nouvelle étiquette à attacher à cet obstacle.

Vous pouvez alterner l'affichage des étiquettes de carte en utilisant l'icône avec le cercle-étiquette.

        Enregistrement des cartes :

Pour enregistrer une carte, cliquez sur la petite icône représentant une disquette en haut à droite de l'écran de l'éditeur. L'icône représentant une porte permet de quitter l'éditeur.
Vous pouvez faire de même par le menu, ouvert en appuyant sur la touche 'Échap'.


Conseils généraux :

	Afficher la vue d'ensemble
Pour modifier le facteur de zoom, appuyez sur la touche 'o' ou cliquez sur l'icône représentant une loupe.
Essayez un clic gauche ou droit pour accéder aux différents facteurs de zoom.


	Le menu de l'éditeur

Vous pouvez accéder à ce menu en appuyant sur 'Échap'.

		"Niveau :"
Vous permet de facilement naviguer vers les autres niveaux. Vous pouvez utiliser les touches fléchées lorsque cette option est sélectionnée
afin de passer au niveau suivant ou précédent (par rapport aux numéros de niveau), ou, en cliquant sur l'option, vous pouvez saisir le numéro du niveau désiré et pressez 'Entrée'.

		Options de niveau
				Niveau :	Voir ci-dessus pour l'explication
				Nom :	Le nom du niveau, affiché avec les coordonnées GPS dans le coin haut droit de l'écran de jeu. Vous pouvez désactiver l'affichage du GPS dans le jeu, avec le menu d'options. 
				Taille :	Vous pouvez agrandir ou réduire la taille du niveau. Sélectionnez le bord sur lequel vous voulez ajouter/enlever une ligne de tuiles et cliquez sur les touches fléchées <- or ->.
				Couches de sol : Pour changer le nombre de couches de sol pour le niveau courant, utilisez les touches fléchées <- or ->.
				Interface de bordure :	Ici, vous pouvez définir les niveaux qui doivent être connectés au niveau courant. Entrez le numéro de niveau pour chaque bord respectif.
								Un niveau ne peut avoir qu'un seul niveau adjacent (avec lequel il est contact sur les bords) dans chacune des directions cardinales (Nord, Sud, Ouest, Est).
				Donjon aléatoire :	En activant cette option, la carte générera automatiquement un donjon. Vous définissez le nombre de télé-porteurs vers et depuis cette carte en cliquant sur cette option.
								Les donjons générés aléatoirement contiendront tout ce qui est nécessaire, comme les points de passage, les robots et les obstacles.
				Classe d'objet lâché pour les obstacles :	Défini quelle doit être la classe des objets déposés par les barils/coffres/caisses.
				Blocus téléportation :	Rend (im)possible de se téléporter depuis le niveau.
				Paire de télé-porteurs :	C'est nécessaire si vous créez un donjon qui n'est pas directement connecté à une autre carte. Vous pouvez définir ici le type de sorties et d'entrées d'un donjon généré aléatoirement. 
				Lumière :			Quelle quantité de lumière aimeriez-vous avoir ? Appuyez sur espace pour passer entre les modes Ambiante (luminosité générale sur la carte actuelle) et Bonus (lumière émise par certains obstacles, comme des lampes ou des champignons).
				Musique d'ambiance :	Ici, vous pouvez définir une piste musicale qui sera jouée pendant que le joueur se déplace sur la carte. Les pistes disponibles sont situées dans ./sound/music/.
									Entrez le nom de fichier, y compris l'extension .ogg.
				Endurance course infinie :	Si définie à 'oui', le niveau d'endurance de Tux ne diminuera pas pendant qu'il court sur la carte. Ce ne devrait être utilisé que si le niveau ne contient pas de PNJs hostiles, comme par exemple le niveau 0, La Ville.
				Ajout/Suppr Niveau :		Permet d'ajouter un nouveau niveau ou de supprimer le niveau courant.

		Options Avancées
Ici, vous pouvez lancer l'exécution du validateur de niveaux.
Le validateur de niveaux vérifie tous les chemins connectant les points de passages, pour assurer qu'ils ne sont pas bloqués par des obstacles. Les informations détaillées sur les chemins bloqués sont reportées dans le terminal, en supposant que vous ayez lancé le jeu en utilisant un terminal, ou dans un fichier de sortie d'erreur global.
Il peut également vérifier si des obstacles sont placés de manière critique près des bords de la carte.
Il devrait TOUJOURS être exécuté avant de considérer qu'une carte est terminée.
"freedroidRPG -b leveltest" permet également d'effectuer ce test.

		Tester le niveau
Vous permet de tester vos modifications facilement.
Si vous quittez ce mode, les changements effectués sur les obstacles pendant le jeu, la destruction de caisses par exemple, seront inversés pour revenir à l'état correspondant au moment où vous avez commencé à tester le niveau.




Touches :
espace					bascule entre les modes placement et sélection
w						place un point de passage
maj+w					bascule le mode d'ajout de point de passage entre 'robot aléatoire' et 'PNJ'
échap					accède au menu
touches clavier numérique 1-9	utilisées pour placer des obstacles à la position respective de la grille
n						cycle entre les obstacles sélectionnés ('next')
z						annule la dernière action
y						répète la dernière action annulée
c						établi un chemin entre des points de passage
ctrl+x ou 'retour arrière'		coupe le(s) objet(s) sélectionné(s), peut être utilisé pour supprimer des objets en ne les recollant pas par la suite
ctrl+c					copie les objets selectionnés
ctrl+v					colle les objets coupés/copiés
alt+maj				tire/déplace l'objet sélectionné à l'aide de la souris
touches fléchées				défile la carte
ctrl+touches fléchées			défile la carte par grand pas
molette de la souris				défile les obstacles du sélecteur d'objets
ctrl+page haut/page bas	défile les obstacles du sélecteur d'objets
g						ouvre l'écran des objets à déposer
t						alterne le mode de transparence 3x3 autour de la mire de curseur
m						ajoute/modifie une étiquette de carte à l'emplacement de la mire de curseur ou de la tuile sélectionnée
o						zoom
tab						bascule vers le mode suivant d'édition
maj+tab				bascule vers le mode précédent d'édition
f						bascule vers l'ongle d'objets suivant
shift+f					bascule vers l'ongle d'objets précédent


Si vous rencontrez des problèmes avec l'éditeur de niveaux, merci de nous contacter.
De même, n'hésitez pas a nous envoyer des cartes si vous en faites quelques unes, on ne mords pas. ;-)
]]
}
