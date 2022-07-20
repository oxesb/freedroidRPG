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
            EDITOR LIVELLI DI FREEDROIDRPG

=== INTRODUZIONE ===

FreedroidRPG è fornito di un editor di livelli. Questo editor ti permette di controllare qualunque aspetto di una normale mappa di FreedroidRPG e salvarla.

Puoi accedere dal menu principale (clicca "Level Editor") oppure eseguendo 'freedroidRPG -e'.

    --- Suggerimenti ---
Per attivare/disattivare le descrizioni dell'interfaccia mentre il mouse si muove, clicca l'icona con la bolla bianca posizionata vicino al bordo destro della finestra (bottoni fila inferiore).

    --- Riepilogo dettagli ---
Il riepilogo dei dettagli su ostacoli ed oggetti apparirà cliccando con il tasto destro del mouse sopra gli stessi nel selettore oggetti superiore.

    --- Navigazione ---
Per cambiare il livello corrente, clicca il numero del livello nella minimappa nell'angolo a destra, oppure selezionando il livello desiderato dal menu dell'editor (descritto dopo).

    --- Modifica della mappa ---
Ci sono quattro modalità di modifica: modifica degli ostacoli, modifica dei pavimenti, modifica degli oggetti e modifica dei riferimenti.

Il bottone selezionato nella parte inferiore sinistra indica l'oggetto che puoi selezionare o posizionare.
Quando un pulsante è stato selezionato, e sei in modalità di posizionamento, l'oggetto che andrai a posizionare è indicato dal fiocco in cima allo schermo. La selezione del fiocco è divisa nelle schede immediatamente sottostanti lo stesso fiocco.

È possibile selezionare il tipo di ostacolo che si desidera collegare sulla mappa in corrispondenza dell'oggetto superiore. Basta cliccarlo per selezionarlo. Gli ostacoli sono divisi in gruppi per fornire una migliore visione.

Premendo la barra spaziatrice, entrerai nella modalità di selezione indicata dal cambiamento del cursore. Puoi selezionare solamente gruppi di oggetti rappresentati dalla modalita oggetti attualmente attivata.
Nota importante: potrai selezionare solamente le cose che sono incluse nella modalita attualmente selezionata. Se sei nella modalità ostacolo, non potrai selezionare elementi o le piastrelle del pavimento.


        Modalità di modifica ostacolo:

Per selezionare questa modalità, schiaccia sul pulsante "Ostacolo" del selezionatore di categorie nell'area inferiore sinistra.
Avendo selezionato un ostacolo, basta clicckare da qualche parte sulla mappa per posizionarlo in corrispondenza della posizione del cursore.
Dal momento che il click è leggermente impreciso, puoi usare anche il tastierino numerico per posizionare gli ostacoli.
Clicca su quello più a sinistra (mostra una piccola griglia) dei 5 pulsanti sopra al selezionatore di categoria per avere una griglia coi numeri visualizzati. Usa il pulsante sinistro per attivare e disattivare la griglia e il pulsante destro per cambiare la modalità della griglia.
Questi numeri si riferiscono al tuo tastierino numerico se ne hai uno. Schiacciando '1' posizionerai l'ostacolo che è evidenziato nel selezionatore di oggetti in corrispondenza della posizione della cifra '1' sulla griglia viola.
Dal momento che posizionare una linea di pareti in tale maniera è piuttosto inefficace, puoi semplicemente tenere schiacciato il pulsante sinistro del mouse e una linea di pareti viene posizionata mentre sposti il cursore se hai selezionato un oggetto di parete. Ciò vale per le pareti più comuni di FreedroidRPG.
Quando tieni premuto il pulsante sinistro del mouse e stai posizionando le pareti, un click del pulsante destro del mouse rimuoverà tutte le pareti che hai disegnato dopo aver iniziato a tenere premuto il pulsante sinistro del mouse.
Esistono alcuni oggetti speciali. Le Pareti di vetro e le pareti con mattoni rotti, ma anche i barili e le scatole possono essere distrutte con pochi colpi, mentre le ultime due possono anche rilasciare oggetti. Le casse possono essere aperte e anch'esse possono contenere oggetti.
Il simbolo con i passi con la croce sopra non è propriamente un oggetto ma un'area invisibile puramente di blocco ('rettangolo di collissione'). I rettangoli di collisione sono il vero nucleo di ogni oggetto dal momento che ti impediscono di camminare attraverso di essi come è possibile con le piastrelle del pavimento o i punti di passaggio.

            Selezionare gli ostacoli

Tenendo premuto il pulsante sinistro del mouse puoi selezionare un rettangolo di ostacolo. Una volta rilasciato il pulsante del mouse, gli ostacoli selezionati diventeranno di un altro colore indicando di essere stati selezionati. Per selezionare gli ostacoli che non sono nello stessa area di un tale rettangolo di selezione, tieni premuto 'Ctrl' e clicca sull'ostacolo o seleziona un altro rettangolo di questi.
Potresti aver selezionato in maniera automatica numerosi ostacoli con un clic solo. Puoi passare da un ostacolo all'altro cliccando l'icona con la sedia e la mensola su di essa, oppure premendo 'n'.
L'icona col cestino cancella gli ostacoli selezionati.
Puoi anche tagliare (Ctrl+x, può essere utilizzato anche per cancellare gli ostacoli basta non incollarli nuovamente ;) ), copiare (Ctrl+c) e incollare (Ctrl+v) gli ostacoli tagliati o copiati.
Puoi spostare gli ostacoli selezionati tenendo premuto il tasto shift di sinistra mentre sposti gli ostacoli in giro. Tuttavia, ciò potrebbe risultare piuttosto impreciso.

            Posizionamento di oggetti nelle casse

Basta selezionare la cassa desiderata e fare clic sul pulsante più a sinistra nella riga superiore.
Sarai reindirizzato ad una schermata che appare come nella schermata del negozio.
Verrà visualizzato un coltello (che in realtà non è posizionato nel petto a proposito) selezionalo e clicca sul pulsante 'vendi'.
Seleziona gli oggetti che desideri eliminare quando il giocatore apre la cassa.
Questi elementi vengono visualizzati nella barra superiore del negozio.
Per rimuovere uno di questi elementi, semplicemente selezionalo e schiaccia su 'compra'.
La croce rossa ti fa fuori dallo schermo.

            Aggiungere un testo a un segno

Seleziona il segno e aggiungi un'etichetta ostacolo con il segno di testo. Salva la mappa e esci.
Apri il file del livello (map/levels.dat) e trova la nuova etichetta di ostacolo. Cambia la linea sopra al testo da type=30' a 'type=32' e salva.
Ora quando cliccherai sull'insegna nel gioco apparirà il tuo breve messaggio.

            Aggiungere un dialogo a un terminale

Seleziona il terminale e aggiungi un'etichetta di ostacolo con il nome della finestra di dialogo che desideri utilizzare. Salva la mappa ed esci.
Apri il file del livello (map/levels.dat) e trova la nuova etichetta di ostacolo.
Cambia la linea sopra al testo da 'type=30' a 'type=32' e salva. Ora quando cliccherai sul terminale nel gioco darà inizio al dialogo da te selezionato.

        Modalità di modifica pavimento:

La modalità di modifica funziona in maniera piuttosto simile alla modalità di modifica di ostacoli. Puoi selezionare diversi tipi di pavimento nel selezionatore di oggetto.
Per riempire una regione con una singola piastrella di pavimento, prima seleziona la piastrella da utilizzare, poi clicca il pulsante sinistro e trascina il mouse fino a ricoprire la regione desiderata. Le piastrelle del pavimento vengono posizionate sullo strato di pavimento attuale.
Non ci sono pavimenti che sono speciali in alcuna maniera, sono puramente una decorazione.

La visibilità degli strati di pavimento può essere controllata da un pulsante con l'icona degli strati. Il pulsante viene mostrato solo per livelli con pavimenti multistrato.
Con un click del pulsante sinistro sul pulsante si passa dalla visualizzazione di un singolo strato di pavimento alla visualizzazione di tutti gli strati di pavimento. Schiacciando il pulsante destro sul pulsante si cambia l'attuale strato di pavimento.

            Seleziona il tipo di pavimento

La selezione è facile come nella modalità ostacolo. Le piastrelle del pavimento possono essere spostate con il metodo descritto sopra.
Per i livelli con pavimenti multistrato vengono selezionati solamente gli strati di pavimento visibili. Quando è visibile un solo strato di pavimento, vengono selezionate solamente le piastrelle nell'attuale strato di pavimento.

Per guardare solamente il pavimento, clicca l'icona della lampada per non visualizzare alcun ostacolo. Un altro clic farà apparire nuovamente gli ostacoli.
L'icona con il rettangolo turchese visualizza i rettangoli di collissione. Questi rettangoli indicano l'area di blocco di un ostacolo. Tux non può camminare in tale area.
Se lo accendi e fai un test di gioco (spiegato in seguito) della tua mappa, i rettangoli vengono ancora visualizzati se attivati il che è piuttosto utile per testare se il giocatore può oltrepassare una fessura o meno.

        Modalità di modifica item

Puoi inoltre posizionare elementi sulla mappa che il giocatore può utilizzare.
Gli elementi sono oggetti che il giocatore può raccogliere. Possono essere trasportati, alcuni possono anche essere utilizzati o equipaggiati.
Alcuni elementi vengono utilizzati per portare avanti la trama, altri forniscono bonus al giocatore, mentre altri ancora non fanno assolutamente niente.
Seleziona la modalità elementi e clicca su un elemento visualizzato nel selezionatore di oggetti. Per alcuni elementi, devi specificare la quantità prima del loro posizionamento.
La puoi impostare cliccando i pulsanti a freccia o trascinando la sfera blu a sinistra o destra.
Premi 'g' per avere una migliore panoramica degli elementi disponibili (possono essere utilizzati anche per il dropping, gli elementi verranno droppati in corrispondenza del mirino
Per fare ciò puoi selezionare anche l'icona con gli stivali con la croce sopra.


        modalità di modifica WayPoint:

Attualmente, i droidi (con questo si intende tutti i personaggi non giocanti) si spostano sui livelli utilizzando punti di passaggio predefiniti
Per impostare un punto di passaggio, premi il tasto 'w'. Questo attiverà il punto di passaggio sul rettangolo sotto al mirino.
Puoi anche cliccare sulla mappa in corrispondenza di una posizione in cui vuoi avere un punto di passaggio avendo attivata questa modalità. Con un altro clic altrove imposterai un altro punto di passaggio e collegherai ad esso quello precedentemente selezionato.
Cliccando su un punto di passaggio preesistente ti permetterà di collegarlo con un altro (per fare ciò, ti basterà cliccare sull'altro).
Tuttavia, c'è una differenza tra questi due metodi di impostazione. Quando collegherai due punti di passaggio utilizzando la tastiera, le connessioni saranno unidirezionali.
Questo vuol dire che quando crei una connessione tra un punto di passaggio A e un punto di passaggio B, il bot sarà in grado soltanto di camminare dal punto A al punto B, ma senza tornare indietro
Puoi rimuovere una connessione unidirezionae 'sovrapponendo' ad essa un'altra connessione che va nella stessa direzione di quella che vuoi cancellare (ciò non funziona con le connessioni bidirezionali!)
Le connessioni bidirezionali sono tuttavia realizzate in maniera automatica utilizzando il metodo del clic per collegare i punti di passaggio.
Nota importante: non è possibile collegare tra loro punti di passaggio su mappe diverse!
I punti di passaggio vengono anche utilizzati per posizionare bot generati in maniera casuale. Tuttavia ciò potrebbe risultare inadeguato per alcuni punti di passaggio.
Vi sono quelli 'normali' che sono bianchi, per rigenerare bot e quelli 'speciali', quelli viola che devono essere utilizzati per gli NPC. Quelli normali vengono utilizzati per i bot generati, quelli viola devono essere utilizzati per gli NPC.
Puoi selezionare questi diversi tipi di punti di passaggio nella barra di selezione superiore. Per trasformare un punto di passaggio normale in uno viola o tornare sui propri passi, premere shift+w.
Per favore assicurati che i percorsi tra i punti di passaggio non siano bloccati da un ostacolo in mezzo ai due punti di passaggio.
Per controllare la mappa intera per questo, puoi utilizzare il validatore di livello della mappa che verrà spiegato in seguito.


        Impostazione le Etichette:

Ci sono due tipi di tavole: etichette della mappa e etichette degli ostacoli.
Per favore assicurati che ogni ID di etichetta sia unico.
Fornire una stringa vuota cancellerà la rispettiva etichetta.


            Impostazione delle etichette della mappa

Le etichette della mappa vengono utilizzate per definire le posizioni iniziali degli NPC (see ReturnOfTux.droids), eventi che si verificano quando TUX si muove su di esse (vedere events.dat), o posizioni utilizzate per lo spostamento degli NPC attraverso i file script lua (eventi, missioni, e i dialoghi).
Per definire una nuova etichetta di mappa, premere il pulsante 'm' sulla tastiera o cliccare il pulsante con la M sull'insegna su di esso. Ti verrà richiesta l'etichetta della mappa. Si noti che apparirà un cerchio colorato su una qualsiasi piastrella della mappa in cui è stata inserita un'etichetta di mappa.
L'etichetta della mappa verrà impostata in maniera automatica sulla piastrella al centro dello schermo.
Puoi impostare la visualizzazione dei droidi/NPC su attivata o disattivata premendo il puslante con il bot 302 su di esso

            Impostazione delle etichette degli ostacoli

Le etichette degli ostacoli sono importanti per poter contrassegnare alcuni ostacoli per eventi che accadranno (ad esempio durante una missione). Se ad esempio si presuppone che un evento rimuova un ostacolo di parete speciale, allora a questo ostacolo deve essere prima dato un nome o un ID, in modo da potervi fare riferimento in seguito nella definizione dell'evento.
Puoi usarli per aggiungere dialoghi agli ostacoli, così puoi parlare agli ostacoli come se fossero NPC.
Per impostare un'etichetta su un ostacolo, devi prima contrassegnare questo ostacolo (vedere la spiegazione della modalità ostacolo di cui sopra).
Cliccando l'icona con l'insegna e la O su di essa ti verrà richiesta la nuova etichetta da attaccare a questo ostacolo.

Puoi disattivare/attivare la visualizzazione delle etichette della mappa utilizzando la piccola icona con il cerchio-etichetta su di essa.

        Salvataggio delle mappe:

Per salvare una mappa, clicca sulla piccola icona del disco nell'area in alto a destra della schermata dell'editor. L'icona a forma di porta ti farà uscire dall'editor.
Puoi fare questo anche attraverso il menu che si apre schiacciando il tasto 'Esc'.


Consigli generali:

	Vedi una panoramica
Per cambiare il fattore dello zoom, premi il tasto 'o' oppure clicca l'icona con la lente su di essa.
Prova a schiacciare il tasto sinistro e quello destro per accedere a diversi fattori di zoom.


	Il menu dell'editor

Può accedere a questo menu premendo ESC.

		"Level:"
Qui puoi navigare facilmente ad altri livelli. Puoi o usare i tasti freccia avendo questa opzione selezionata
per passare al prossimo o al precedente livello (facendo riferimento ai numeri dei livelli), oppure, cliccando su di esso, inserisci il numero del livello desiderato e premi enter.

		Opzioni del livello
				Livello:	Vedere sopra per la spiegazione
				Nome:	Il nome della mappa mostrato nel GPS nell'angolo in alto a destra della schermata di gioco. Puoi disabilitare il GPS durante il gioco utilizzando il menu delle opzioni.
				Dimendioni:	Puoi aumentare o ridurre le dimensioni del tuo livello. Selezionare il bordo desiderato in cui vuoi aggiungere/rimuovere una linea di piastrelle e cliccare i tasti freccia <- or ->.
				Strati del pavimento: per cambiare il numero di strati del pavimento nell'attuale livello, utilizzare i tasti freccia <- or ->.
				Interfaccia di bordo:	Qui puoi selezionare i livelli che devono essere limitrofi all'attuale livello. Immetti il numero del livello per ogni rispoettivo bordo.
								Un livello può avere solamente un livello adiacente (uno con cui i bordi si toccano) in ognuno dei quattro punti cardinali (Nord, Sud, Ovest, Est).
				Dungeon casuale:	se imposti questa opzione su 'Si', la mappa genererà in maniuera automatica un dungeon. Cliccando sull'opzione tu puoi impostare il numero dei teletrasporti di entrata e uscita da questa mappa.
								I dungeon generati in maniera casuale avranno tutto il necessario impostato, come punti di passaggio, bot, e ostacoli.
				Classe degli elementi droppati per gli ostacoli:	Impostazione della classe che gli elementi droppati dai barili/casse/scatole devono avere.
				Blocco del teletrasporto:	Rende (im)possibile teletrasportarsi via da un livello.
				Accoppiamento del Teletrasporto:	Questo è importante se crei un dungeon che non è direttamente collegato ad un'altra mappa. Qui puoi impostare il numero di uscite ed entrate per un dungeon generato casualmente.
				Luce:			Quanta luce ti piacerebbe avere? Premi la barra spaziatrice per passare da ambiente (la luce generale della mappa attuale) e le modalità bonus (la luce emessa da alcuni ostacoli, quali lampade, o funghi).
				Musica di sottofondo:	Qui puoi impostare una traccia musicale da suonare mentre il giocatore cammina in giro per la mappa. Le possibili tracce puoi trovarle in ./sound/music/ .
									Basta inserire il nome del file con l'estensione .ogg inclusa.
				Stamina per la corsa infinita:	Se hai impostato ciò su "yes", la stamina di Tux non diminuirà mentre corre per la mappa. ciò deve essere utilizzato solamente se il livello non ha NPC ostili in esso, come ad esempio nel livello 0, la Città.
				add/rem livellol:		Ti permette di aggiungere un nuovo livello o di rimuovere l'attuale livello.

		Opzioni avanzate
Qui puoi eseguire il validatore di livello di mappa.
Il validatore di livello di mappa controlla tutti i percorsi tra i punti di passaggio per assicurarsi che non siano bloccati da ostacoli. Nel terminale puoi trovare un risultato più dettagliato che spiega quali percorsi sono bloccati, nel caso in cui il gioco venga eseguito utilizzandolo, oppure un file di output di errore globale.
Può anche controllare se hai ostacoli vicini ai bordi della mappa in una maniera critica.
Questo deve essere SEMPRE eseguito prima di poter dire che una mappa è finita.
Anche "freedroidRPG -b leveltest" esegue questo controllo.

		Test di gioco del file di mappa
Ti permette di testare nel gioco le tue modifiche facilmente.
Se abbandoni questa modalità, i cambiamenti negli ostacoli che sono stati fatti durante il gioco, ad esempio la distruzione di casse, verranno ripristinati al momento in cui hai iniziato il test di gioco.




Tasti
spazio					attiva disattiva la modalità di impostazione/selezione
w						imposta un punto di passaggio
shift+w					attiva la modalità per i punti di passaggio a 'bot casuale' o 'NPC'
escape					accedi al menu
cifre del tastierino numerico 1-9	utilizzate per impostare gli ostacoli in corrispondenza delle posizioni della griglia
n						cicla tra gli ostacoli selezionati (successivo)
z						annulla l'ultima azione
y						ripete l'ultima azione annullata
c						imposta percorsi tra punti di passaggio
ctrl+x oppure backspace		taglia gli oggetti selezionati, può essere utilizzato per eliminare gli oggetti non incollandoli in seguito
ctrl+c					copia gli oggetti selezionati
ctrl+v					incolla gli oggetti tagliati/copiati
alt+shift				trascina/sposta gli oggetti selezionati utilizzando il mosue
tasti freccia				scorrono in giro per la mappa
ctrl+tasti freccia			scorrono in giro con intervalli maggiori
ruota del mouse				scorre attraverso gli ostacoli del selezionatore di oggetti
ctrl+pagina su/pagina giù	scorre attraverso gli ostacoli del selezionatore di oggetti
g						accede alla schermata degli elementi di drop
t						attiva disattiva la trasparenza 3x3 attorno al mirino
m						aggiunge/modifica un'etichetta di mappa nella posizione del mirino o della piastrella selezionata
o						zoom
tab						passa alla successiva modalità di editing
shift+tab				passa alla precedente modalità di editing
f						passa alla successiva scheda oggetto
shift+f					passa alla precedente scheda oggetto


Se incontri problemi con l'editor, per favore contattaci.
Inoltre, non aver timore di inviarci mappe se hai fatto qualcosa di valido, non mordiamo :)
]]
}
