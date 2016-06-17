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
            L’ÉDITEUR DE NIVEAU DE FREEDROIDRPG

=== INTRODUCTION ===

FreedroidRPG s'accompagne d'un éditeur de niveau intégré. Cette éditeur de niveau permet de contrôler tous les aspects d'une carte de FreedroidRPG et de la sauvegarder.

Vous pouvez y accéder depuis le menu principale (choisissez "Editeur de Niveau") ou en executant la commande "freedroidRPG -e".

    --- Tooltips ---
Pour activer/désactiver les descriptions de l'interface pendant le survol de la souris, cliquez sur l'icône de bulle de discours blanc situé près du bord de la fenêtre de droite (rangée inférieure de bouton).

    --- Summary Details ---
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


        Obstacle edit mode:

In order to select this mode, click on button that says 'Obstacle' of the category selector on the left lower area.
Having selected an obstacle, just click somewhere on the map to place it at the cursor's position.
Since clicking is a little imprecise, you can also use your numberpad to place obstacles.
Click on the leftmost (it shows a small grid) of the five button above category selector to have a grid with numbers displayed. Use left click for switching the grid on and off and right click for changing the grid mode.
These numbers refer to the numbers of your numberpad if you have one. Pressing '1' will place the obstacle that is highlighted in the object selector at the position of the digit '1' on the purple grid.
Since placing a line of walls that way is quite inefficient, you can simply hold down the left mouse button and a line of walls is placed as you move the cursor if you have a wall object selected. This works with the most common walls in FreedroidRPG.
While holding the left mouse button down and placing walls, a click with the right mouse button will remove all the walls you drew after starting to hold down the left mouse button.
There are some special objects. Glass walls and cracked brick walls, but also barrels and crates can be destroyed with a few strikes, while the latter two may also release items. Chests can be opened and may contain items, too.
The symbol with the crossed out footsteps is not really an object but pure and invisible blocking area ('collision rectangle'). Collision rectangles are the very core of each object since they prevent you from just walking through them as it possible for waypoints or floor tiles.

            Selecting obstacles

Holding down the left mouse button you can select a rectangle of obstacles. After releasing the mouse button, selected obstacles will turn a different color indicating that they are selected. To select obstacles that are not in the range of such a selection rectangle, hold down 'Ctrl' and click the obstacle or select another rectangle of them.
You may have automatically selected several obstacles with one click. You can switch between the obstacles clicking the icon with the chair and the shelf on it, or pressing 'n'.
The icon with the trash can delete the selected obstacle.
You can also cut (Ctrl+x, can also be used to delete obstacles by just not pasting them again ;) ), copy (Ctrl+c) and paste (Ctrl+v) cut or copied obstacles.
You can move selected obstacles holding down the left shift key while dragging the obstacle around. However, this may be quite imprecise.

            Placing objects in Chests

Simply select the desired chest and click the most left button in the upper button row.
You will be forwarded to a screen that looks like the shop screen.
There will be a knife displayed (which is actually not placed in the chest by the way) select it and click on the 'sell' button.
Select the items you want to be dropped when the player opens the chest.
These items will be displayed in the upper shop bar.
In order to remove one of these items, simply select it and click on 'buy'.
The red cross gets you out of the screen.

            Ajouter du Texte à un panneau

Sélectionnez le panneau et ajoutez une indication d'obstacle avec panneau.  enregistrez la carte et sortez.
Open the level file (map/levels.dat) and find the new obstacle label. Change the line above the text from 'type=30' to 'type=32' and save.
Now when you click on the sign in the game your short message will appear.

            Adding a Dialog to a Terminal

Select the terminal and add an obstacle label with the dialog name you wish to use. Save the map and exit.
Open the level file (map/levels.dat) and find the new obstacle label.
Change the line above the text from 'type=30' to 'type=32' and save. Now when you click on the terminal in the game it will start the dialog you selected.

        Floor edit mode:

The floor edit mode works quite similar to the obstacle edit mode. You can select different types of floors at the object selector.
To fill a region with a single floor tile, first select the tile to use, then click and drag the left mouse button until it covers the desired region. The floor tiles are placed on the current floor layer.
There are no floors that are special in any way, they are pure decoration.

The visibility of floor layers can be controlled by a button with the layer icon. The button is only displayed for levels with multilayer floors.
Left click on the button switches between a single floor layer displayed and all floor layers displayed. Right click on the button changes the current floor layer.

            Selecting floor types

Selecting is as easy as in the the obstacle mode. Floor tiles can be moved to with the method described above.
For levels with multilayer floors only visible floor layers are selected. When a single floor layer is visible, only the tiles in the current floor layer are selected.

In order to have a look at the floor only, click the lamp icon to have no obstacles displayed. Another click will let obstacles appear again.
The icon with the turquoise rectangle displays collision rectangles. These rectangles indicate the blocking-area of an obstacle. Tux can't walk on such an area.
If you turn it on and playtest (explained later) your map, the rectangles are still displayed if activated which is quite useful for testing whether the player can pass a gap or not.

        Item edit mode:

You can place items to be used by the player on the map, too.
Items are objects that the player can pick up. They can be they can be carried, some even be used or equipped.
Some items are used to move the plot forward, others provide bonuses to the player, while still others do nothing at all.
Select the item mode and click on an item displayed at the object selector. For some items, you must specify an amount before they are placed.
You can set it by clicking the arrow buttons or dragging the blue orb to the left or the right.
Press 'g' to have a better overview of what items are available (can also be used for dropping, items will be dropped at the crosshair). Hit 'Esc' to abort the process without dropping any items.
You can also click the icon with the crossed-out boots to perform this.


        Waypoint edit mode:

Currently, droids (meaning all non-player characters) move around on levels using predefined waypoints.
To plant a waypoint, press the 'w' key. This will toggle the waypoint on the rectangle under the crosshair.
You can also click the map at a position you want to have a waypoint having this mode activated. Another click somewhere else plants another waypoint and automatically connects the previous selected one with it.
Clicking on a preexisting waypoint lets you connect it with another one (just click the other one, too, to do it).
However, there is a difference between those two planting methods. When you connect two waypoints using the keyboard, the connections will be unidirectional.
That means that when you make a connection from waypoint A to waypoint B, the bot will only be able to walk from A to B but not back.
You can remove an unidirectional connection by 'overlying' it with another one going into the very direction as the one you want to delete (this does not work with bidirectional connections!).
Bidirectional connections are however automatically done using the click method to connect waypoints.
Important note: It is not possible to connect waypoints on different maps with each other!
Waypoints are also used to position randomly spawned bots. However this might be inappropriate for some waypoints.
There are 'normal' ones which are white, for respawning bots and 'special', purple ones which should be used for NPCs. The normal ones are used for spawned bots, the purple ones should be used for NPCs.
You can select these different types of waypoints in the upper selection bar. To turn a normal waypoint into a purple one or back again, press shift+w.
Please make sure that paths between waypoints are not blocked by an obstacle in between of two waypoints.
To automatically check a entire map for this, you can use the map level validator which is explained later.


        Planting Labels:

There are two kinds of tables: map labels and obstacle labels.
Please make sure that each label ID is unique.
Giving an empty string will delete the respective label.


            Planting map labels

Map labels are used to define starting locations of NPCs (see ReturnOfTux.droids), events that occur when Tux moves over them (see events.dat), or locations used for movement of NPCs through the lua script files (events, quests, and the dialogs).
To define a new map label, press the 'm' key on the keyboard or click the button with the M on the sign on it. You will be prompted for the map label. Note that there will be a colorful circle appearing on any map tile that has been fitted with a map label.
The map label will be automatically planted on the tile in the middle of the screen.
You can switch the displaying of droids/NPCs on or off pressing the button with the 302 bot on it.

            Planting obstacle labels

Obstacle labels are important so that some obstacles can be marked for events to happen (for example during a quest). If e.g. an event is supposed to remove a special wall obstacle, then this obstacle must be given a name or ID first, so it can be referred to later in the definition of the event.
You can also use them to add dialogs to obstacles, so you can talk to them as they were NPCs.
To plant a label on an obstacle, you must first mark this obstacle (see obstacle mode explanation above).
Clicking the icon with the sign and the O on it you will be prompted for the new label to attach to this obstacle.

You can toggle display of map labels using the small icon with the label-circle on it.

        Saving maps:

In order to save a map, click the small disk icon in the upper right area of the editor screen. The door icon lets you exit the editor.
You can also do this via the menu that is opened by pressing the 'Esc' key.


General tips:

	Getting overview
In order to change the zoom factor, press the 'o' key or click the icon with the magnifying glass on it.
Try left and right clicking in order to access different zoom factors.


	The editor menu

You can access this menu by pressing ESC.

		"Level:"
Here you can easily navigate to other levels. You can either use the arrow keys having this option selected
in order to switch to the next or previous (refers to level numbers) level, or, clicking on it, enter the number of the desired level and press enter.

		Level options
				Level:	See above for explanation
				Name:	The map name displayed at the GPS in the upper right corner of the game screen. You can disable the GPS in-game using the options menu.
				Size:	You can increase or reduce the size of your level. Select the desired edge where you want to add/remove a line of tiles and click the <- or -> arrow buttons.
				Floor layers: In order to change the number of floor layers for the current level, use the <- or -> arrow buttons.
				Edge interface:	Here you can set the levels that shall be next to the current level. Enter the level number for the respective edge.
								A level can only have one adjacent level (one it touches edges with) in each of the four cardinal directions (North, South, West, East).
				Random dungeon:	If you set this option to 'Yes', the map will automatically generate a dungeon. You set the number of teleporters to and from this map clicking on the option.
								Randomly generated dungeons automatically will have everything necessary, like waypoints, bots, and obstacles, set.
				Item drop class for obstacles:	Set of what item class items dropped by barrels/chests/crates should be.
				Teleport blockade:	Make it (im)possible to teleport away from a level.
				Teleport pair:	This important if you make a dungeon that is not directly connected to another map. You can set the number of exits and entrances for a randomly generated dungeon here.
				Light:			How much light would you like to have? Press space to switch between ambient (general brightness of the present map) and bonus (light emitted by some obstacles, such as lamps or mushrooms) mode.
				Background music:	Here you can set a music track to be played while the player walks around on the map. Possible tracks can be found in ./sound/music/ .
									Just enter the file name including the .ogg extension.
				Infinite running Stamina:	If you have this set to "yes", Tux' stamina will not decrease while running across the map. This should only be used if the level has no hostile NPCs on it, like on level 0, the Town, for example.
				add/rem level:		Lets you add a new level or remove the current level.

		Advanced options
Here you can run the map level validator.
The map level validator checks all the paths between connected waypoints to ensure they are not blocked by obstacles. More detailed output explaining which paths are blocked can be found in the terminal, given the case that the game is run using it, or a global error output file.
It can also check if you have obstacles near map borders in a critial way.
This should ALWAYS be run before calling a map finished.
"freedroidRPG -b leveltest" does also run this check.

		Playtest mapfile
Vous permet de tester vos modifications facilement.
If you leave this mode, obstacle changes that were made while playing, destroying crates for example, will be reverted to the time where you started playtesting.




Touches :
espace					inverse le mode pose/selection
w						plant waypoint
shift+w					toggle mode for waypoints to 'random bot' or 'NPC'
echap					accéde au menu
numberpad digits 1-9	used to plant obstacles at the respective positions of the grid
n						cycle through selected obstacles (next)
z						annule la dernière action
y						répète la dernière action annulé
c						set paths between waypoints
ctrl+x or backspace		cut selected object(s), can be used to delete objects by not pasting afterwards
ctrl+c					copie les objets selectionnés
ctrl+v					colle les objets coupés/copiés
alt+shift				drag/move selected object using the mouse
arrow keys				scroll around the map
ctrl+arrow keys			scroll around in bigger steps
mousewheel				scroll through obstacles of the object selector
ctrl+pageup/page down	scroll through obstacles of the object selector
g						access drop item screen
t						toggle 3x3 transparency around the crosshair
m						add/edit a map label at the position of the crosshair or the selected tile
o						zoom
tab						switch to the next editing mode
shift+tab				switch to the previous editing mode
f						switch to the next object tab
shift+f					switch to the previous object tab


Si vous rencontrez des problèmes avec l'éditeur de nivaux, merci de nous contacter.
De même, n'hésitez pas a nous envoyer des cartes si vous en faites quelques unes, on ne mords pas. ;-)
]]
}
