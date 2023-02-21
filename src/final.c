#include <stdio.h>
#include <stdlib.h>
#include <MLV/MLV_all.h>
#include <unistd.h>

#define TAILLE_X 1200
#define TAILLE_Y 800

#define NB_NIVEAU 5
#define LONGEUR_MOT 32

#define NB_LINES 7
#define NB_POSITIONS 24
#define NB_TYPES 5
#define NB_INFOS 3


typedef struct chips{
    int type;
    int life;
    int line;       /* Y */
    int position;   /* X */
    int price;
    struct chips* next;
} Chips;

typedef struct virus{
    int type;
    int life;
    int line; // Y
    int position; // X
    int speed;
    int turn;
    struct virus* next;
    struct virus* next_line;
    struct virus* prev_line;

} Virus;

typedef struct{
    Virus* virus;
    Chips* chips;
    int turn;
    int money;
} Game;




/****************************************/
/*INITIALISATION D'UNE PARTIE*/

void infoLifeVirus(int type, int *life, int typesV[][NB_INFOS]){
    int i;
    for(i=0; i<NB_TYPES; i++){
        if (typesV[i][0] == type){
            *life = typesV[i][1];
        }
    }
}

void infoSpeedVirus(int type, int *speed, int typesV[][NB_INFOS]){
    int i;
    for(i=0; i<NB_TYPES; i++){
        if (typesV[i][0] == type){
            *speed = typesV[i][2];
        }
    }
}


Virus* alloueVirus(int type, int life, int line, int position, int speed, int turn){
    Virus *virus = (Virus *) malloc(sizeof(Virus));
    if (virus != NULL){
        virus->type = type;
        virus->life = life;
        virus->line = line;
        virus->position = position;
        virus->speed = speed;
        virus->turn = turn;
        virus->next = NULL;
    }
    return virus;
}

void insereVirus(Game *g, Virus *virus){
    if (g->virus == NULL){
        g->virus = virus;
    }
    else{
        Virus *tmp = g->virus;
        while (tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = virus;
    }
}


void debutPartie(Game* g, FILE* file, int *nbtour, int typesV[][NB_INFOS]){
    char type;
    int turn, line, position = NB_POSITIONS+1, life, speed;
    g->virus = NULL;

    //on recup d'abord l'argent qui sera a disposition du J
    fscanf(file, "%d", &g->money);
    //printf("%d\n", g->money);

    while (fscanf(file, "%d %d %c", &turn, &line, &type) == 3){ 
        infoLifeVirus((int) type, &life, typesV);
        infoSpeedVirus((int) type, &speed, typesV);
        //printf("%d %d %c, %d %d\n",turn, line, type, life, speed);

        Virus *virus = alloueVirus((int) type, life, line, position, speed, turn);
        insereVirus(&(*g), virus);
        
    }
    fclose(file);
    *nbtour = turn;
    //printf("%d", *nbtour);
}



/******************************************/
/*VISUALISATION DE LA VAGUE EN ASCII*/

void visuaVagueASCII(Game g, int nbtour){
    int i, j;
    int vilain = 0;
    
    printf("Here is the wave of problems approaching...\n");
    for(i=1; i<NB_LINES+1; i++){ //lignes
        printf("%d| ", i);
        for (j=1; j<nbtour+1; j++){ //tours

            Virus *tmp = g.virus;
            while(tmp != NULL){
                if (tmp->line == i && tmp->turn == j){
                    printf("%c ", tmp->type);
                    vilain = 1;
                }
                tmp = tmp->next;
            }
            if (vilain == 0){
                printf("  ");
            }
            vilain = 0;
        }
        printf("\n");
    }
}



/**************************************/
/*POSAGE DES TOURELLES SUR LE PLATEAU*/

void infoChips(int type, int *life, int *price, int typesC[][NB_INFOS]){
    int i;
    for(i=0; i<NB_TYPES; i++){
        if (typesC[i][0] == type){
            *life = typesC[i][1];
            *price = typesC[i][2];
        }
    }
}


int poseValide(Game g, int line , int position){
    if ((line >= 1 && line <= NB_LINES) && (position >= 1 && position <= NB_POSITIONS)){
        Chips *tmp = g.chips;
        while(tmp != NULL){
            //si la case(line, position) est déjà occupé par un chips 
            if (tmp->line == line && tmp->position == position){
                return 0;
            }
            tmp = tmp->next;
        }
        return 1;
    }
    return 0;
}

void saisieCoord(Game g, int *line, int *position){
    do {
        printf("Line Position: ");
        scanf("%d %d", &(*line), &(*position));
    } while(poseValide(g, *line, *position) != 1);
}


Chips* alloueChips(int type, int life, int line, int position, int price){
    Chips *chips = (Chips *) malloc(sizeof(Chips));
    if (chips != NULL){
        chips->type = type;
        chips->life = life;
        chips->line = line;
        chips->position = position;
        chips->price = price;
        chips->next = NULL;
    }
    return chips;
}

void insereChips(Game *g, Chips *chips){
    if (g->chips == NULL){
        g->chips = chips;
    }
    else{
        Chips *tmp = g->chips;
        while (tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = chips;
    }
}


void afficheMenu(){
    printf("Do you want to see the wave?\n");
    printf("v : show the wave\n\n");

    printf("What chips do you want to deploy ?\n");
    printf("F : firewall\n");
    printf("P : PMMU\n");
    printf("A : ALU\n");
    printf("R : RAM\n");
    printf("X : Patch\n");
    printf("q : no more\n");
}

void choixJoueur(Game g, char *choix){
    printf("\n-------------------------------------------------------\n");
    printf("Your money : %d\n", g.money);
    afficheMenu();
    do {
        printf("\nYour choice ?\n");
        scanf(" %c", &(*choix));
    } while(*choix != 'F' && *choix != 'P' && *choix != 'A' && *choix != 'R' && *choix != 'X' && *choix != 'q' && *choix != 'v');
}


void poseChips(Game *g, int nbtour, int typesC[][NB_INFOS]){
    char choix;
    int life, line, position, price;
    g->chips = NULL;

    while (1){
        if (g->money <= 0){
            printf("You have no more money\n");
            break;
        }

        choixJoueur(*g, &choix);
        if (choix == 'q'){
            break;
        }
        else if (choix == 'v'){
            visuaVagueASCII(*g, nbtour);
        }
        else{
            infoChips((int) choix, &life, &price, typesC); //une fois qu'on a le type, on determine life et price
            if (g->money < price){
                printf("You don't have enough money\n");
            }
            else{
                g->money -= price;
                saisieCoord(*g, &line, &position);
                Chips *chips = alloueChips((int) choix, life, line, position, price);
                insereChips(&(*g), chips);
            }
        }
    }
}



/*******************************/
/*DEROULE D'1 TOUR*/

/*Entrée des virus sur le plateau en position opposé à l'ordinateur central*/
void entrePlateau(Game *g, int turn){
    Virus *tmp = g->virus;
    while (tmp != NULL){
        if (tmp->turn == turn){
            //tmp->position =-1;
            tmp->position = NB_POSITIONS;
        }
        tmp = tmp->next;
    }
}



/*Gere la suppression d'un virus et d'un chip*/
void supprimeVirus(Game *g, Virus *virus){
    if (g->virus == virus){
        g->virus = g->virus->next;
        free(virus);
        return;
    }

    Virus *tmp = g->virus;
    while (tmp->next != virus){
        tmp = tmp->next;
    }
    tmp->next = virus->next;
    free(virus);
}

void supprimeChips(Game *g, Chips *chip){
    if (g->chips == chip){
        g->chips = g->chips->next;
        free(chip);
        return;
    }

    Chips *tmp = g->chips;
    while (tmp->next != chip){
        tmp = tmp->next;
    }
    tmp->next = chip->next;
    free(chip);
}



/*Gere l'action des Chips*/
Virus* premierVirus(Game g, int line){
    Virus *first = g.virus;
    while(first != NULL){
        if (first->line == line && first->position <= NB_POSITIONS){
            return first;
        }
        first = first->next;
    }
    return NULL;
}

Virus* contactVirus(Game g, int line, int position){
    while(g.virus != NULL){
        if (g.virus->line == line && g.virus->position == position){
            return g.virus;
        }
        g.virus = g.virus->next;
    }
    return NULL;
}


void tueVirus(Game *g, Virus *virus){
    if (virus->life <= 0){ //si le virus n'a plus de vie
        supprimeVirus(&(*g), virus);
    }
}

void tire(Game *g, Virus *cible, int type, int line){
    if (type == (int)'A'){
        cible->life -= 1;
    }
    else if (type == (int)'R'){
        cible->speed = 1;
    }
    else if (type == (int)'P'){
        /*tourelle faisant de lourds dommages sur sa ligne 
        et les 2 lignes voisines*/
        cible->life -= 3;
        Virus *voisinHaut = premierVirus(*g, line-1);
        Virus *voisinBas = premierVirus(*g, line+1);

        if (voisinHaut != NULL){
            voisinHaut->life -= 3;
            tueVirus(&(*g), voisinHaut);
        }
        if (voisinBas != NULL){
            voisinBas->life -= 3;
            tueVirus(&(*g), voisinBas);
        }
    }
}


void actionChips(Game *g){
    if (g->chips == NULL){ //si le joueur n'a pas posé de tourelle
        return;
    }

    int type, line, position;
    Chips *chip = g->chips;
    while (chip != NULL){
        type = chip->type, line = chip->line, position = chip->position;

        Virus *cible = premierVirus(*g, line);
        Virus *contact = contactVirus(*g, line, position);

        if (cible != NULL){ //s'il y a une cible 
            if (cible == contact){
                if (type == (int)'X'){ //mine explosant au contact d'un vilain
                    supprimeVirus(&(*g), cible); //one shot
                    supprimeChips(&(*g), chip);
                }
                else{
                    tire(&(*g), cible, type, line);
                    tueVirus(&(*g), cible);
                }
            }
            else{
                tire(&(*g), cible, type, line);
                tueVirus(&(*g), cible);
            }
        }

        chip = chip->next;
    }
}



/*************************************/
/*CHAMPS next_line ET prev_line*/

void insereVirusNext(Virus *first, Virus *virus){
    Virus *tmp = first;
    while (tmp->next_line != NULL){
        tmp = tmp->next_line;
    }
    tmp->next_line = virus;
}

Virus* next_line(Game g, int line){
    Virus *first = premierVirus(g, line);
    first->next_line = NULL;

    Virus *tmp = g.virus;
    while(tmp != NULL){
        if ((tmp->line == line && tmp->position <= NB_POSITIONS) && tmp != first){
            Virus *next = tmp;
            next->next_line = NULL;
            insereVirusNext(first, next);
        } 
        tmp = tmp->next;
    }
    return first;
}


Virus* prev_line(Game g, int line){
    Virus *nextLine = next_line(g, line);

    Virus *tete = NULL;
    Virus *virus = NULL;

    Virus *tmp = nextLine;
    while (tmp != NULL){
        virus = tmp;
        virus->prev_line = tete;
        tete = virus;

        tmp = tmp->next_line;
    }
    return tete;
}


Virus* devant(Game g, int line, Virus *virus){
    Virus *prevLine = prev_line(g, line);

    Virus *tmp = prevLine;
    while (tmp != virus){
        tmp = tmp->prev_line;
    }
    return tmp->prev_line;
}

Virus* derriere(Game g, int line, Virus *virus){
    Virus *nextLine = next_line(g, line);

    Virus *tmp = nextLine;
    while (tmp != virus){
        tmp = tmp->next_line;
    }
    return tmp->next_line;
}



/*Gere l'action des Virus*/
void soigneVirus(Virus *virus, int typesV[][NB_INFOS]){
    int viedeBase;

    infoLifeVirus(virus->type, &viedeBase, typesV);
    if (virus->life < viedeBase){
        virus->life += 1;
    }
}

void accelereVirus(Virus *virus, int typesV[][NB_INFOS]){
    int speedBase;

    infoSpeedVirus(virus->type, &speedBase, typesV);
    if (virus->speed <= speedBase){
        virus->speed += 1;
    }
}

void effet(Game g, Virus *virus, int line, int type, int typesV[][NB_INFOS]){
    if (type == (int)'M'){ //docteur qui soigne devant et derriere
        Virus *dev = devant(g, line, virus);
        Virus *der = derriere(g, line , virus);

        if (dev != NULL){ //s'il y a un virus devant lui
            soigneVirus(dev, typesV);
        }
        if (der != NULL){ //s'il y a un virus derriere lui
            soigneVirus(der, typesV);
        }
    }
    else if (type == (int)'B'){
        /*vilain qui accélère le 1er de sa ligne 
        ainsi que les 2 premiers virus des lignes voisines à la sienne*/
        Virus *first = premierVirus(g, line);
        Virus *voisinHaut = premierVirus(g, line-1);
        Virus *voisinBas = premierVirus(g, line+1);

        if (first != virus){ 
            accelereVirus(first, typesV);
        }
        if (voisinHaut != NULL){
            accelereVirus(voisinHaut, typesV);
        }
        if (voisinBas != NULL){
            accelereVirus(voisinBas, typesV);
        }
    }
}


Chips* contactChips(Game g, int line, int position){
    if (g.chips == NULL){
        return NULL;
    }

    while(g.chips != NULL){
        if (g.chips->line == line && g.chips->position == position){
            return g.chips;
        }
        g.chips = g.chips->next;
    }
    return NULL;
}


void degat(Chips* contact, int type){
    if (type == (int)'E'){
        contact->life -= 1;
    }
    else if (type == (int)'D'){
        contact->life -= 1;
    }
    else if (type == (int)'S'){
        contact->life -= 1;
    }
}


void actionVirus(Game *g, int typesV[][NB_INFOS]){
    int type, line, position; 

    Virus *virus = g->virus;
    while (virus != NULL){
        if (virus->position <= NB_POSITIONS){
            type = virus->type, line = virus->line, position = virus->position;

            effet(*g, virus, line, type, typesV);

            Chips *contact = contactChips(*g, line, position);
            if (contact != NULL){ //si le virus est au contact d'un chip
                degat(contact, type);

                if (contact->life <= 0){
                    supprimeChips(&(*g), contact);
                }
            }
        }
        
        virus = virus->next;
    }
}



/*Déplacement des virus sur le plateau*/
int positionMax(Game g, int line){
    Chips *posMax = g.chips;

    while (g.chips != NULL){
        if (g.chips->position > posMax->position){
            posMax = g.chips;
        }
        g.chips = g.chips->next;
    }
    return posMax->position;
}

Chips* premierChip(Game g, int line){
    if (g.chips == NULL){
        return NULL;
    }
    int posMax = positionMax(g, line);

    while (g.chips != NULL){
        if (g.chips->line == line && g.chips->position == posMax){
            return g.chips;
        }
        g.chips = g.chips->next;
    }
    return NULL;
}


void deplacementVirus(Game *g){
    int line, position; 

    Virus *virus = g->virus;
    while (virus != NULL){
        if (virus->position <= NB_POSITIONS){ //si le virus est dans le plateau
            line = virus->line, position = virus->position;

            Chips *contact = contactChips(*g, line, position);
            if (contact == NULL){ //si le virus n'est pas au contact d'un chip

                Virus *firstVirus = premierVirus(*g, line);
                if (virus == firstVirus){ //si c'est le 1er virus de sa ligne

                    Chips *firstChip = premierChip(*g, line);
                    if (firstChip != NULL){ //s'il y a au moins une tourelle sur sa ligne
                        int dist = virus->position - firstChip->position;

                        if (dist >= virus->speed){
                            virus->position -= virus->speed;
                        }
                        else{
                            virus->position = firstChip->position;
                        }
                    }
                    else{
                        virus->position -= virus->speed;
                    }
                }

                Virus* dev = devant(*g, line, virus);
                if (dev != NULL){ //si c'est pas le 1er virus de sa ligne
                    int dist = virus->position - dev->position;


                    if (dist > virus->speed){
                        virus->position -= virus->speed;
                    }
                    if (dist <= virus->speed){
                        virus->position = dev->position + 1;
                    }
                }
            }
        }

        virus = virus->next;
    }
}



/*Condition de fin de partie*/
int partiePerdu(Game g){
    Virus *virus = g.virus;
    while (virus != NULL){
        if (virus->position < 1){
            return 1;
        }
        virus = virus->next;
    }
    return 0;
}

int PartieGagne(Game g){
    if (g.virus == NULL){
        return 1;
    }
    return 0;
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////// MODE CONSOLE //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



int asciiChoixFile(char niveau[][LONGEUR_MOT]){
    int i;
    int choix;
    printf("Chose your level : \n");

    for(i=0; i<NB_NIVEAU; i++){
        printf("%s : %d \n", niveau[i], i);
    }
    printf("\n");

    do{
        printf("Your choice : ");
        scanf("%d", &choix);
    } while(choix < 0 || choix > NB_NIVEAU-1);

    return choix;
}



/*Permet de visualiser les tourelles et les virus dans le mode Console*/
void visuaObjet(Game g, int line, int position){
    int pose = 0;

    while (g.chips != NULL){
        if (g.chips->line == line && g.chips->position == position){
            printf("%2d%c ", g.chips->life, g.chips->type);
            pose = 1;
        }
        g.chips = g.chips->next;
    }

    while (g.virus != NULL){
        if (g.virus->line == line && g.virus->position == position){
            printf("%2d%c ", g.virus->life, g.virus->type);
            pose = 1;
        }
        g.virus = g.virus->next;
    }

    if (pose == 0){
        printf("    ");
    }
    pose = 0;
}

void modeConsole(Game g){
    int i, j;

    printf("\e[1;1H\e[2J");
    printf("\nGame turn %d\n", g.turn);

    for(i=1; i<NB_LINES+1; i++){
        printf("%d| ", i);
        for(j=1; j<NB_POSITIONS+1; j++){
            visuaObjet(g, i, j);
        }
        printf("\n");
    }
}



/*Deroule d'1 tour*/
void derouleTour(Game *g, int typesV[][NB_INFOS]){
    g->turn = 1;

    while (1){
        entrePlateau(&(*g), g->turn);
        modeConsole(*g);
        
        if (partiePerdu(*g) == 1){
            printf("You lose !!");
            break;
        }
        if (PartieGagne(*g) == 1){
            printf("You win !!");
            break;
        }

        actionChips(&(*g));
        actionVirus(&(*g), typesV);
        deplacementVirus(&(*g));
        g->turn += 1;
        sleep(1);
    }
}

















/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////// MODE GRAPHIQUE ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////











// ----------------------------------------------------------------------------
// ----------------------------- FONCTION D'AFFICHAGE -------------------------
// ----------------------------------------------------------------------------

void drawTourelle(){
    MLV_draw_rectangle(20, 15, 50, 50, MLV_COLOR_BLACK);
    MLV_draw_image(MLV_load_image("img/alu.png"), 20,15);
    MLV_draw_text(30, 65, "100", MLV_COLOR_BLACK);

    MLV_draw_rectangle(80, 15, 50, 50, MLV_COLOR_BLACK);
    MLV_draw_image(MLV_load_image("img/ram.png"), 80,15);
    MLV_draw_text(90, 65, "150", MLV_COLOR_BLACK);

    MLV_draw_rectangle(140, 15, 50, 50, MLV_COLOR_BLACK);
    MLV_draw_image(MLV_load_image("img/pmmu.png"), 140,15);
    MLV_draw_text(155, 65, "400", MLV_COLOR_BLACK);

    MLV_draw_rectangle(200, 15, 50, 50, MLV_COLOR_BLACK);
    MLV_draw_image(MLV_load_image("img/patch.png"), 200,15);
    MLV_draw_text(215, 65, "50", MLV_COLOR_BLACK);

    MLV_draw_rectangle(260, 15, 50, 50, MLV_COLOR_BLACK);
    MLV_draw_image(MLV_load_image("img/firewall.png"), 260,10);
    MLV_draw_text(275, 65, "125", MLV_COLOR_BLACK);

    MLV_actualise_window();
}

void drawUI(Game *g){
    MLV_Image* image;
    MLV_Image* computer;
    char money[16];

    // Background
    image = MLV_load_image("img/background.png");
    MLV_resize_image(image, TAILLE_X, TAILLE_Y);
    MLV_draw_image(image, 0,0);

    // Ordinateur
    computer = MLV_load_image("img/computer.png");
    MLV_resize_image(computer, 1280/10, 925/10);
    MLV_draw_image(computer, 10, TAILLE_Y/2);

    // Tour image
    MLV_draw_filled_rectangle(10, 10, 500, 75, MLV_COLOR_WHITE);

    // Argent
    sprintf(money, "%d", g->money);
    MLV_draw_text(400, 30, "Argent :", MLV_COLOR_BLACK);
    MLV_draw_text(400, 40, money, MLV_COLOR_BLACK);

    // Voir vague
    MLV_draw_filled_rectangle(520, 10, 100, 75, MLV_COLOR_WHITE);
    MLV_draw_text(522, 40, "Voir la vague !", MLV_COLOR_BLACK);

    // Commencer
    MLV_draw_filled_rectangle(630, 10, 100, 75, MLV_COLOR_WHITE);
    MLV_draw_text(635, 40, "Commencer !", MLV_COLOR_BLACK);

    // informations
    MLV_draw_filled_rectangle(740, 10, 100, 75, MLV_COLOR_WHITE);
    MLV_draw_text(745, 40, "Informations", MLV_COLOR_BLACK);

    // Quitter
    MLV_draw_filled_rectangle(TAILLE_X-110, 10, 90, 75, MLV_COLOR_WHITE);
    MLV_draw_text(TAILLE_X-90, 40, "Quitter !", MLV_COLOR_BLACK);

    // Tourelles
    drawTourelle();
}

void drawEmplacement(){
    int x, y;
    int sizeX = 900/NB_POSITIONS;
    int sizeY = 600/NB_LINES;
    int x1, y1;
    for(x=0; x<NB_POSITIONS; x++){
        for(y=0; y<NB_LINES; y++){
            x1 = x*sizeX;
            x1 += x*(sizeX/10);

            y1 = y*sizeY;
            y1 += y*(sizeY/10);
            MLV_draw_filled_rectangle(x1+150, y1+150, sizeY/5, sizeY/5, MLV_COLOR_LIGHT_BLUE);
        }

    }
}


// Permet de réafficher tout l'affichage du jeu en une fonction
void actualise(Game *g){
    MLV_draw_filled_rectangle(0,0, TAILLE_X, TAILLE_Y, MLV_COLOR_WHITE);
    drawUI(g);
    drawEmplacement();
    MLV_actualise_window();
}



/*MENU DE BASE ET MENU CHOIX DES NIVEAUX*/
void drawMenu(){
    MLV_Image* image = MLV_load_image("img/menu.png");
    MLV_resize_image(image, 800, 800);
    MLV_draw_image(image, 0,0);

    MLV_Image* jouer = MLV_load_image("img/jouer.png");
    MLV_resize_image(jouer, 300, 50);
    MLV_draw_image(jouer, 10,500);

    MLV_Image* survie = MLV_load_image("img/survie.png");
    MLV_resize_image(survie, 300, 50);
    MLV_draw_image(survie, 10,570);

    MLV_Image* quitter = MLV_load_image("img/quitter.png");
    MLV_resize_image(quitter, 300, 50);
    MLV_draw_image(quitter, 10,640);

    MLV_actualise_window();

}

void drawChoixNiveau(char niveau[][LONGEUR_MOT]){
    // Fond
    MLV_Image* image = MLV_load_image("img/menu2.png");
    MLV_resize_image(image, 800, 800);
    MLV_draw_image(image, 0,0);

    // Titre
    MLV_Image* title = MLV_load_image("img/choix.png");
    MLV_draw_image(title, 200,50);

    // Affichage niveau
    int i=0;
    for(; i<NB_NIVEAU; i++){
        MLV_draw_rectangle(30, 400 + 50 * (i+1), 100, 25, MLV_COLOR_RED);
        MLV_draw_text(30, 400 + 50 * (i+1), niveau[i], MLV_COLOR_WHITE);
    }
    MLV_actualise_window();
}




void drawObjet(Game g, int line, int position){
    MLV_Image* chip;
    MLV_Image* virus;

    while (g.chips != NULL){
        if (g.chips->line == line && g.chips->position == position){
            if(g.chips->type == (int)'A'){
                chip = MLV_load_image("img/alu.png");
            }
            else if(g.chips->type == (int)'R'){
                chip = MLV_load_image("img/ram.png");
            }
            else if(g.chips->type == (int)'P'){
                chip = MLV_load_image("img/pmmu.png");
            }
            else if(g.chips->type == (int)'X'){
                chip = MLV_load_image("img/patch.png");
            }
            else if(g.chips->type == (int)'F'){
                chip = MLV_load_image("img/firewall.png");
            }

            MLV_resize_image_with_proportions(chip, 50, 50);
            int sizeX = 900/NB_POSITIONS;
            int sizeY = 600/NB_LINES;

            int resX = position*sizeX + position*(sizeX/10);
            int resY = line*sizeY + line*(sizeY/10);
            MLV_draw_image(chip, resX+100, resY+40);
            MLV_actualise_window();
        }
        g.chips = g.chips->next;
    }

    if (g.virus != NULL){
        while (g.virus != NULL){
            if (g.virus->line == line && g.virus->position == position){
                if(g.virus->type == (int)'E'){
                    virus = MLV_load_image("img/fpe.png");
                }
                else if(g.virus->type  == (int)'D'){
                    virus = MLV_load_image("img/ddos.png");
                }
                else if(g.virus->type  == (int)'S'){
                    virus = MLV_load_image("img/segfault.png");
                }
                else if(g.virus->type  == (int)'M'){
                    virus = MLV_load_image("img/corruption.png");
                }
                else if(g.virus->type  == (int)'B'){
                    virus = MLV_load_image("img/bug.png");
                }

                MLV_resize_image_with_proportions(virus, 50, 50);
                int sizeX = 900/NB_POSITIONS;
                int sizeY = 600/NB_LINES;

                int resX = position*sizeX + position*(sizeX/10);
                int resY = line*sizeY + line*(sizeY/10);
                MLV_draw_image(virus, resX+100, resY+40);
                MLV_actualise_window();
            }
            g.virus = g.virus->next;
        }
    }
}

void draw(Game g){
    int i, j;

    for(i=1; i<NB_LINES+1; i++){
        for(j=1; j<NB_POSITIONS+1; j++){
            drawObjet(g, i, j);
        }
    }
}



// ----------------------------------------------------------------------------
// ----------------------------- SAISIE DE SOURIS -----------------------------
// ----------------------------------------------------------------------------

void visuaVagueGraph(Game g, int nbtour){
    MLV_Image* virus;
    int i, j;
    printf("NBBB %d \n", nbtour);
    for(i=1; i<NB_LINES+1; i++){
        for (j=1; j<nbtour+1; j++){ 

            if (j <= NB_POSITIONS){
                Virus *tmp = g.virus;
                while(tmp != NULL){
                    if (tmp->line == i && tmp->turn == j){

                        if(g.virus->type == (int)'E'){
                        virus = MLV_load_image("img/fpe.png");
                        }
                        else if(g.virus->type  == (int)'D'){
                            virus = MLV_load_image("img/ddos.png");
                        }
                        else if(g.virus->type  == (int)'S'){
                            virus = MLV_load_image("img/segfault.png");
                        }
                        else if(g.virus->type  == (int)'M'){
                            virus = MLV_load_image("img/corruption.png");
                        }
                        else if(g.virus->type  == (int)'B'){
                            virus = MLV_load_image("img/bug.png");
                        }

                        MLV_resize_image_with_proportions(virus, 50, 50);
                        int sizeX = 900/NB_POSITIONS;
                        int sizeY = 600/NB_LINES;
 
                        int resX = j*sizeX + j*(sizeX/10);
                        int resY = i*sizeY + i*(sizeY/10);
                        MLV_draw_image(virus, resX+100, resY+40);
                        MLV_actualise_window();

                    }
                    tmp = tmp->next;
                }
            }
            else{
                //img 
                MLV_draw_text(TAILLE_X-100, TAILLE_Y/2, "et plus encore ! ", MLV_COLOR_RED);
            }
        }
    }
}

// Permet de connaitre les choix du joueur
void saisieSourisUI(Game *g, int x, int y, int nbtour, int typesV[][NB_INFOS]){

    if(10<y && y<85){
        if(1090 < x && x < 1180){
            printf("QUITTER \n");
            exit(0);
        }

        if(740 < x && x < 850){
            MLV_Image* infos = MLV_load_image("img/infos.png");
            MLV_draw_image(infos, 300, 100);
            MLV_actualise_window();
            
            MLV_wait_seconds(5);
            actualise(&(*g));
            draw(*g);
            MLV_free_image(infos);

            printf("INFORMATIONS \n");
        }


        if(630 < x && x < 730){

            g->turn = 1;

            while (1){
                entrePlateau(&(*g), g->turn);
                draw(*g);
                
                if (partiePerdu(*g) == 1){
                    printf("You lose !!");
                    MLV_draw_text(TAILLE_X/2, TAILLE_Y/2, "YOU LOSE !!", MLV_COLOR_RED);
                    MLV_actualise_window();
                    break;
                }
                if (PartieGagne(*g) == 1){
                    printf("You win !!");
                    MLV_draw_text(TAILLE_X/2, TAILLE_Y/2, "YOU WIN !!", MLV_COLOR_RED);
                    MLV_actualise_window();
                    break;
                }
                actionChips(&(*g));
                actionVirus(&(*g), typesV);
                deplacementVirus(&(*g));

                g->turn += 1;
                MLV_wait_seconds(1);
                actualise(&(*g));
            }


            printf("COMMENCER\n");
            

        }

        if(520 < x && x < 620){
            actualise(&(*g));
            visuaVagueGraph(*g, nbtour);
            printf("VOIR LA VAGUE\n");
        }
    }
}


void saisieSourisEmplacement(int a, int b, int *position, int *line){
    int x, y;
    int sizeX = 900/NB_POSITIONS;
    int sizeY = 600/NB_LINES;
    int x1, y1;

    for(x=0; x<NB_POSITIONS; x++){
        for(y=0; y<NB_LINES; y++){
            x1 = x*sizeX;
            x1 += x*(sizeX/10);

            y1 = y*sizeY;
            y1 += y*(sizeY/10);
            if(x1+150<a && a< x1+150+sizeX/2){
                if(y1+150<b && b< y1+150+sizeY/5){

                    MLV_draw_filled_rectangle(x1+150, y1+150, sizeX/2, sizeY/5, MLV_COLOR_RED);
                    MLV_actualise_window();
                    printf("\nCoordonnees : %d %d \n", x+1, y+1);
                    *line = y+1;
                    *position = x+1;
                }
            }

        }
    }

}

void saisieTourelle(int a, int b, int *typeChip){
    int choix;
    int types[5] = {(int)'A', (int)'R', (int)'P', (int)'X', (int)'F'};

    int x;
    int x1=20, x2=70;

    for(x=0; x<5; x++){
        if(15<b && b<65){
            if(x1<a && a<x2){
                printf("\nTourelle : %d\n", x+1);
                choix =  x+1;

                *typeChip = types[choix-1];
            }
        }
        x1 += 60;
        x2 += 60;
    }
}

void poseChipsSouris(Game *g, int type, int line, int position, int typesC[][NB_INFOS]){
    int life, price;
    
    infoChips(type, &life, &price, typesC); //une fois qu'on a le type, on determine life et price
    if (g->money >= price){
        g->money -= price;
        Chips *chips = alloueChips(type, life, line, position, price);
        insereChips(&(*g), chips);
    }
}



void modeSurvie(){
    printf("Coming soon !\n");
    exit(0);
}



/*MENU ET CHOIX DES NIVEAUX*/
int saisieMenu(){
    int y1, y2;
    int i;
    int choix=0;

    while(choix == 0){
        int a;
        int b;
        MLV_wait_mouse(&a, &b);

        y1=500, y2=550;

        for(i = 1; i < 4; i++){
            if(10<a && a<310){
                if(y1<b && b<y2){
                    choix = i;
                }
            }
            y1 += 70;
            y2 += 70;
        }
    }

    return choix;
}

int saisieChoixNiveau(){
    int a, b, i;
    int y1, y2;
    int choixNiv = 0;
    
    while(choixNiv == 0){
        MLV_wait_mouse(&a, &b);

        y1 = 450, y2 = 475;
        for(i = 1; i < NB_NIVEAU+1; i++){
            if(30 < a && a < 130){
                if(y1 < b && b < y2)
                    choixNiv = i;
            }
            y1 += 50;
            y2 += 50;
        }
    }

    return choixNiv - 1;
}


/*CHOIX DU MODE D'AFFICHAGE*/
void choixModeAffichage(int argc, char *argv[], int *mode){
    int i;
    for (i = 1; i<argc; i++){
        if (argv[i][0] == '-'){
            if (argv[i][1] == 'a'){
                *mode = 1;
            }
            else if (argv[i][1] == 'g'){
                *mode = 2;
            }
        }
    }
}



int main(int argc, char *argv[]){
    Game g;
    int choixFile, choix;
    FILE* file = NULL;
    int nbtour = 0;

    int type = 0;
    int position = 0, line = 0;
    int modeAffichage = 0;
    g.chips = NULL;

    /* type, life, speed */
    int typesV[NB_TYPES][NB_INFOS] = {
        { (int)'E', 3, 2},
        { (int)'D', 10, 1},
        { (int)'S', 3, 3},
        { (int)'M', 5, 2},
        { (int)'B', 4, 2}
    };

    /* type, life, price */
    int typesC[NB_TYPES][NB_INFOS] = {{(int)'A', 2, 100}, {(int)'R', 2, 115}, {(int)'P', 4, 400}, {(int)'X', 1, 50}, {(int)'F', 10, 150}};

    char niveauPossibles[NB_NIVEAU][LONGEUR_MOT] = {
        "levels/level1.txt",
        "levels/level2.txt",
        "levels/level3.txt",
        "levels/level4.txt",
        "levels/level5.txt"
    };

    choixModeAffichage(argc, &(*argv), &modeAffichage);
    if (modeAffichage == 0){
        printf("ERROR !, no display mode was entered\n");
        printf("Enter -a for : ASCII\n");
        printf("Enter -g for : Graphique\n");
        exit(0);
    }
    
    else if (modeAffichage == 1){ //mode Console
        int choixFile = asciiChoixFile(niveauPossibles);
        file = fopen(niveauPossibles[choixFile], "r");

        debutPartie(&g, file, &nbtour, typesV);
        poseChips(&g, nbtour, typesC);
        
        derouleTour(&g, typesV);
    }
    else{ /* mode Graphique */
        MLV_create_window("Tower defense", "Menu", 800, 800);
        drawMenu();
        choix = saisieMenu();
        printf("CHOIX : %d \n", choix);
        MLV_free_window();


        MLV_create_window("Tower defense", "Choix", 800, 800);
        if(choix == 1){
            drawChoixNiveau(niveauPossibles);

            choixFile = saisieChoixNiveau(); //entre 0 et 4
            printf("CHOIX NIVEAU: %d \n", choixFile);
            MLV_free_window();

            file = fopen(niveauPossibles[choixFile], "r");
            debutPartie(&g, file, &nbtour, typesV);

        } else if(choix == 2){
            modeSurvie();
        } else {
            exit(0);
        }

        MLV_create_window("Tower defense", "Tower defense", TAILLE_X, TAILLE_Y);
        actualise(&g);
        
        while(1){
            int a, b;
            MLV_wait_mouse(&a, &b);

            saisieTourelle(a, b, &type);
            if (type != 0){
                saisieSourisEmplacement(a, b, &position, &line);
                
                if (line != 0 && position != 0){
                    poseChipsSouris(&g, type, line, position, typesC);
                    line = 0, position = 0;
                }
            }
            actualise(&g);
            draw(g);
            MLV_actualise_window();

            saisieSourisUI(&g, a, b, nbtour, typesV); // rien

        }


        MLV_actualise_window();
        MLV_wait_seconds(2);
    }

    return 0;
}
