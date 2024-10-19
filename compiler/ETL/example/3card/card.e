#include<libetl/etl.e>
#include<libetl/sdl.e>

#define CARD_BACK 0
#define CARD_ACE 1
#define CARD_KING 2
#define NUM_CARDS 3

#define CARD1 0
#define CARD2 5
#define CARD3 10

proc @alloc_deck() {
    let deck = malloc(15 * 8);  
    memclr(deck, 15 * 8);
    return deck;
}

proc set_deck(@deck) {
    memstorel(deck, 0, CARD_KING);
    memstorel(deck, 1, 320/2); // X
    memstorel(deck, 2, (720-450)/2); // Y
    memstorel(deck, 3, rand()%2); // DIR
    memstorel(deck, 4, 0); // CARD_INDEX

    memstorel(deck, 5, CARD_KING);
    memstorel(deck, 6, 500); // X
    memstorel(deck, 7, (720-450)/2); // Y
    memstorel(deck, 8, rand()%2); // DIR
    memstorel(deck, 9, 1); // CARD_INDEX
    
    memstorel(deck, 10, CARD_KING);
    memstorel(deck, 11, 840); // X
    memstorel(deck, 12, (720-450)/2); // Y
    memstorel(deck, 13, rand()%2); // DIR
    memstorel(deck, 14, 2); // CARD_INDEX

    let r = rand()%3;
    if(r == 0) {
        memstorel(deck, CARD1, CARD_ACE);
    }
    if(r == 1) {
        memstorel(deck, CARD2, CARD_ACE);
    }
    if(r == 2) {
        memstorel(deck, CARD3, CARD_ACE);
    }
    return 0;
}

proc proc_cards(@deck) {
    let cards = malloc ( 8 * 3 );
    memclr(cards, 8 * 3);
    memstorel(cards, 0, CARD1+1);
    memstorel(cards, 1, CARD2+1);
    memstorel(cards, 2, CARD3+1);
    for(let i = 0; i < 3; i += 1) {
        let pos_x = mematl(deck, mematl(cards, i));
        let pos_dir = mematl(deck, mematl(cards, i)+2);
        if(pos_dir == 0) {
            pos_x -= 50;
            if(pos_x < -320) {
                pos_x = 1280+320;
            }
        } else {
            pos_x += 50;
            if(pos_x > 1280+320) {
                pos_x = -320;
            }
        }
        memstorel(deck, mematl(cards, i), pos_x );
    }
    free(cards);
    return 0;
}

proc set_cards(@deck) {
    let pos_x1 = mematl(deck, CARD1 + 1);  
    let pos_x2 = mematl(deck, CARD2 + 1);  
    let pos_x3 = mematl(deck, CARD3 + 1);  
    
    if (pos_x1 < pos_x2 && pos_x1 < pos_x3) {
        memstorel(deck, CARD1 + 1, 320/2);  
    } else {
        if (pos_x1 > pos_x2 && pos_x1 > pos_x3) {
            memstorel(deck, CARD1 + 1, 800);  
        } else {
            memstorel(deck, CARD1 + 1, 480);  
        }
    }

    if (pos_x2 < pos_x1 && pos_x2 < pos_x3) {
        memstorel(deck, CARD2 + 1, 320/2);  
    } else { 
        if (pos_x2 > pos_x1 && pos_x2 > pos_x3) {
            memstorel(deck, CARD2 + 1, 800);  
        } else {
            memstorel(deck, CARD2 + 1, 480);  
        }
    }
    
    if (pos_x3 < pos_x1 && pos_x3 < pos_x2) {
        memstorel(deck, CARD3 + 1, 320/2);  
    } else {
        if (pos_x3 > pos_x1 && pos_x3 > pos_x2) {
            memstorel(deck, CARD3 + 1, 800);  
        } else {
            memstorel(deck, CARD3 + 1, 480);  
        }
    }
    return 0;
}

proc init() {
    srand(time(0));
    sdl_init();
    sdl_create("3 Card Shuffle", 1280, 720, 1280, 720);

    let deck = alloc_deck();
    let textures = malloc(8 * NUM_CARDS); 

    set_deck(deck);

    let card_back_tex = sdl_loadtex("data/cardback.bmp");
    let card_ace_tex = sdl_loadtex("data/cardace.bmp");
    
    let active_intro = 1;
    let active_game = 1;
    let active_win = 0;

    while (sdl_pump() && active_intro == 1) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        if(mematl(deck, CARD1) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        }
        if(mematl(deck, CARD2) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        }
        if(mematl(deck, CARD3) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        }
        sdl_printtext(15, 15, "Press Enter to Shuffle the Cards");
        if (sdl_keydown(40)) {
            active_intro = 0;
        }
        sdl_flip();
    }

    set_deck(deck);

    while (sdl_pump() && active_game == 1) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();

        sdl_copytex(card_back_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        sdl_copytex(card_back_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        sdl_copytex(card_back_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        
        sdl_printtext(15, 15, "Press Space to Stop Shuffle");
        sdl_flip();
        proc_cards(deck);
        if(sdl_keydown(44)) {
            active_game = 0;
        }
    }
    set_cards(deck);

    let selected_card = 0;

    while (sdl_pump() && active_game == 0) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        sdl_copytex(card_back_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        sdl_copytex(card_back_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        sdl_copytex(card_back_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        sdl_printtext(15, 15, "Press 1, 2, or 3 to Select Ace");
        sdl_printtext(mematl(deck, CARD1+1)+320/2, 50, "1");
        sdl_printtext(mematl(deck, CARD2+1)+320/2, 50, "2");
        sdl_printtext(mematl(deck, CARD3+1)+320/2, 50, "3");

        if(sdl_keydown(30)) {
            selected_card = 0;
            active_game = 1;
        }
        if(sdl_keydown(31)) {
            selected_card = 1
            active_game = 1;
        }
        if(sdl_keydown(32)) {
            selected_card = 2;
            active_game = 1;
        }
        sdl_flip();
    }

    let correct_card = 0;

    while(sdl_pump() && active_game == 1) {
        sdl_setcolor(0,0,0,255);
        sdl_clear();
        if(mematl(deck, CARD1) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD1+1), mematl(deck,CARD1+2), 320, 450);
        }
        
        if(mematl(deck, CARD2) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD2+1), mematl(deck,CARD2+2), 320, 450);
        }
        if(mematl(deck, CARD3) == 2) {
            sdl_copytex(card_back_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        } else {
            sdl_copytex(card_ace_tex, mematl(deck, CARD3+1), mematl(deck,CARD3+2), 320, 450);
        }

        if(mematl(deck, CARD1) == CARD_ACE) {
            correct_card = 0;
        }
        if(mematl(deck, CARD2) == CARD_ACE) {
            correct_card = 1;
        }
        if(mematl(deck, CARD3) == CARD_ACE) {
            correct_card = 2;
        }

        sdl_printtext(mematl(deck, CARD1+1)+320/2, 50, "1");
        sdl_printtext(mematl(deck, CARD2+1)+320/2, 50, "2");
        sdl_printtext(mematl(deck, CARD3+1)+320/2, 50, "3");

        if(selected_card == correct_card) {
            sdl_printtext(15, 15, "You are correct! You Selcted: " + str(selected_card+1) + " card is #" + str(correct_card+1) + " [Press Ecape to Quit]");
        } else {
            sdl_printtext(15, 15, "You are incorrect! Selected: " + str(selected_card+1) + " card is # " + str(correct_card+1) + " [Press Escape to Quit]");
        }
        sdl_flip();
    }

    sdl_destroytex(card_back_tex);
    sdl_destroytex(card_ace_tex);
    free(deck);
    sdl_release();
    sdl_quit();
    return 0;
}