// snakes.c
// gcc serpent5.c -o serpent5.out -lpthread -lncurses
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <pthread.h>

struct coor {                   //inisialisasi  koordinat curses
 	int x;
 	int y;
};

//variabel global
struct coor snake1[2000];       //koordinat snake player 1
struct coor snake2[2000];       //koordinat snake player 2
struct coor food;               //koordinat objek makanan
struct coor head;               //koordinat kepala ular
int panjang1;                   //variabel menyimpan panjang snake player 1
int panjang2;                   //variabel menyimpan panjang snake player 2
char ch;                        //menyimpan character getch
char ch1;                       //menyimpan arah snake player 1
char ch2;                       //menyimpan arah snake player 2
char inputkb_1;                 //menyimpan hasil thread input keyboard untuk snake player 1
char inputkb_2;                 //menyimpan hasil thread input keyboard untuk snake player 2
int screen_width = 100;         //lebar layar permainan
int screen_height = 20;         //tinggi layar permainan
int screen_x = 0;               //awal print layar koor x
int screen_y = 0;               //awal print layar koor y
int game_state = 1;             //status game sekarang
int game_state_start = 1;       //status game untuk menu awal
int game_state_ongoing = 2;     //status game ketika permainan sedang berlangsung
int game_state_over = 0;        //status game ketika permainan berakhir
int result1;                    //menyimpan hasil permainan
int result2;
pthread_mutex_t lock;
WINDOW* win;

//fungsi 
void init();                    //inisialisasi variabel
void snakeMove1(char ch1);      //mengupdate lokasi snake player 1
void snakeMove2(char ch2);      //mengupdate lokasi snake player 2
void snakeHits();               //mengecek apakah snake terkena dinding atau badan 
char kbhit1(char ch1);          //mengecek input untuk snake player 1
char kbhit2(char ch2);          //mengecek input untuk snake player 2
int curs_set(int visibility);   //cursor visibility

void *Thread_1(void *vargp){
    pthread_mutex_lock(&lock);
    snakeMove1(ch1);
    pthread_mutex_unlock(&lock);
} 
void *Thread_2(void *vargp){
    pthread_mutex_lock(&lock);
    snakeMove2(ch2);
    pthread_mutex_unlock(&lock);
} 

void *inputkb(void *vargp){
    while(game_state == game_state_ongoing){
        int i;
        char string[10];
        for(i=0;i<10;i++){
            string[i] = getch();
            string[9] = '\n';
        }
        usleep(100000);
        for(i=0;i<10;i++){
            if(string[i] == 'w' || string[i] == 'a' || string[i] == 's' || string[i] == 'd'){
                inputkb_1 = string[i];
                i = 10;
            }
        }
        for(i=0;i<10;i++){
            if(string[i] == 'i' || string[i] == 'j' || string[i] == 'k' || string[i] == 'l'){
                inputkb_2 = string[i];
                i = 10;
            }
        }
    }
}

int main(){
    pthread_t snake1, snake2, input;
    refresh();  //merefresh perubahan

    while(1){
        if(game_state == game_state_start){ //menu awal
            nodelay(stdscr, FALSE);
            init();     
            mvwprintw(win, screen_height/2, (screen_width/2)-9, "Welcome to SERPENT");
            mvwprintw(win, (screen_height/2)+2, (screen_width/2)-8, "Press r to start");
            mvwprintw(win, (screen_height/2)+3, (screen_width/2)-8, "Press h for help");
            wrefresh(win);
            ch = getch();
            if(ch == 'r'){
                wclear(win);
                mvwprintw(win, screen_height/2, (screen_width/2)-7, "Game start in");
                mvwprintw(win, (screen_height/2)+1, (screen_width/2)-7, "3...");
                wrefresh(win);
                sleep(1);
                wclear(win);
                mvwprintw(win, screen_height/2, (screen_width/2)-7, "Game start in");
                mvwprintw(win, (screen_height/2)+1, (screen_width/2)-7, "2...");
                wrefresh(win);
                sleep(1);
                wclear(win);
                mvwprintw(win, screen_height/2, (screen_width/2)-7, "Game start in");
                mvwprintw(win, (screen_height/2)+1, (screen_width/2)-7, "1...");
                wrefresh(win);
                sleep(1);
                wclear(win);
                win = newwin(screen_height, screen_width, screen_y, screen_x);  //membuat window baru
                game_state = 2;
                pthread_create(&input, NULL, inputkb, NULL);
                init();     //menginisialisasi variabel
            }
            if(ch == 'h'){
                nodelay(stdscr, FALSE);
                wclear(win);
                mvwprintw(win, 2, 7, "Player 1");
                mvwprintw(win, 4, 7, "snake  : oooo>");
                mvwprintw(win, 6, 7, "Control");
                mvwprintw(win, 7, 7, "Up     : w");
                mvwprintw(win, 8, 7, "Down   : s");
                mvwprintw(win, 9, 7, "Left   : a");
                mvwprintw(win, 10, 7, "Right  : d");
                mvwprintw(win, 2, 40, "Player 2");
                mvwprintw(win, 4, 40, "snake  : ++++>");
                mvwprintw(win, 6, 40, "Control");
                mvwprintw(win, 7, 40, "Up     : i");
                mvwprintw(win, 8, 40, "Down   : k");
                mvwprintw(win, 9, 40, "Left   : j");
                mvwprintw(win, 10, 40, "Right  : l");
                mvwprintw(win, 13, 17, "Press anything to exit");
                wrefresh(win);
                ch = getch();
            }
        }
        if(game_state == game_state_ongoing) { //tahap permainan
            ch1 = kbhit1(ch1);
            ch2 = kbhit2(ch2);
            box(win, 0, 0);
            print();
            wrefresh(win);
            wclear(win);   
            usleep(200000);     //1000000 micro detik = 1 detik
            pthread_create(&snake1, NULL, Thread_1, NULL);
            pthread_create(&snake2, NULL, Thread_2, NULL);
            snakeHits(); 
        }
        else if(game_state == game_state_over){ //game selesai
            nodelay(stdscr, FALSE);
            clear();
            if(result1 == 0 && result2 == 0){
                mvprintw(screen_height/2-5, screen_width/2-1, "TIE");
            }
            else if(result1 == 0){
                mvprintw(screen_height/2-5, screen_width/2-5, "PLAYER 2 WIN");
            }
            else if(result2 == 0){
                mvprintw(screen_height/2-5, screen_width/2-5, "PLAYER 1 WIN");
            }
            mvprintw(screen_height/2+1, screen_width/2-7, "Press q to quit");
            mvprintw(screen_height/2, screen_width/2-9, "Press r to play again");
            ch = getch();
            if(ch == 'q'){
                break;
            }
            if(ch == 'r'){
                game_state = game_state_start;
            }
        }
    }
    clear();
    endwin();   //mengakhiri program

    return 0;
}

void foodPlacement() {
    srand(time(0));
    int i;
    int check = 0;

    do {
        /* random koordinat untuk food */
        food.x = ((rand() % ((screen_width-4)/2)) * 2) + 2;
        food.y = rand() % (screen_height-4) + 2;

        /* memastikan koordinat food tidak sama dengan snake */
        for (i = 0; i < panjang1; i++) {
            if ((food.x == snake1[i].x) && (food.y == snake1[i].y)) {
                check = 1;
            }
        }
        for (i = 0; i < panjang2; i++) {
            if ((food.x == snake2[i].x) && (food.y == snake2[i].y)) {
                check = 1;
            }
        }
    } while (check);
    
}

void init(){
    initscr();                                                      // memulai mode curse
    curs_set(0);                                                    // invisible cursor
    noecho();	                                                    // tidak ada karakter inputan yang tampil
    win = newwin(screen_height, screen_width, screen_y, screen_x);  // membuat window baru
    nodelay(stdscr, TRUE);                                          // getch tidak menunggu inputan key
    result1 = 1;
    result2 = 1;

    /* inisialisasi snake */
    ch1 = 's';
    ch2 = 'i';
    inputkb_1 = 's';
    inputkb_2 = 'i';
    panjang1 = 4;
    panjang2 = 4;
    int i;
	for (i = 3; i <= 6; i ++) {
		snake1[6-i].x = 4;
		snake1[6-i].y = i;
 	}
    for (i = 3; i <= 6; i ++) {
		snake2[6-i].x = 96;
		snake2[6-i].y = 18-i;
 	}

    foodPlacement();
}

void snakeMove1(char ch1){
    head.x = snake1[0].x;
    head.y = snake1[0].y;
    /* memeriksa posisi kepala ular */
    if(ch1 == 'w' || ch1 == 'W'){
        head.y = snake1[0].y - 1;
    }
    if(ch1 == 'a' || ch1 == 'A'){
        head.x = snake1[0].x - 2;
    }
    if(ch1 == 's' || ch1 == 'S'){
        head.y = snake1[0].y + 1;
    }
    if(ch1 == 'd' || ch1 == 'D'){
        head.x = snake1[0].x + 2;
    }
    
    /* jika dapat makanan panjang ditambah 1 */
    if (food.x == head.x && food.y == head.y) {
        panjang1++;
        foodPlacement();
    }

    /* menggeser body ular */
    int i;
    for (i = panjang1-1; i > 0; i--) {
        snake1[i].x = snake1[i-1].x;
        snake1[i].y = snake1[i-1].y;
    }
    // menggeser kepala ular
    snake1[0].x = head.x;
    snake1[0].y = head.y;
}

void snakeMove2(char ch2){
    head.x = snake2[0].x;
    head.y = snake2[0].y;
    /* memeriksa posisi kepala ular */
    if(ch2 == 'i' || ch2 == 'I'){
        head.y = snake2[0].y - 1;
    }
    if(ch2 == 'j' || ch2 == 'J'){
        head.x = snake2[0].x - 2;
    }
    if(ch2 == 'k' || ch2 == 'K'){
        head.y = snake2[0].y + 1;
    }
    if(ch2 == 'l' || ch2 == 'L'){
        head.x = snake2[0].x + 2;
    }
    
    /* jika dapat makanan panjang ditambah 1 */
    if (food.x == head.x && food.y == head.y) {
        panjang2++;
        foodPlacement();
    }

    /* menggeser body ular */
    int i;
    for (i = panjang2-1; i > 0; i--) {
        snake2[i].x = snake2[i-1].x;
        snake2[i].y = snake2[i-1].y;
    }
    // menggeser kepala ular
    snake2[0].x = head.x;
    snake2[0].y = head.y;
}

void snakeHits(){
    /* mati jika menabrak dinding */
    if(snake1[0].x <= 1 || snake1[0].y <= 0  || snake1[0].x >= screen_width || 
        snake1[0].y >= screen_height-1){
        result1 = 0;
        game_state = game_state_over;
    }
    if(snake2[0].x <= 1 || snake2[0].y <= 0  || snake2[0].x >= screen_width || 
        snake2[0].y >= screen_height-1){
        result2 = 0;
        game_state = game_state_over;
    }

    /* mati jika menabrak tubuh sendiri */
    int i;
    for (i = 4; i < panjang1; i++){
        if ((snake1[0].x == snake1[i].x) && (snake1[0].y == snake1[i].y)) {
            result1 = 0;
            game_state = game_state_over;
        }
    }
    for (i = 4; i < panjang2; i++){
        if ((snake2[0].x == snake2[i].x) && (snake2[0].y == snake2[i].y)) {
            result2 = 0;
            game_state = game_state_over;
        }
    }


    /* mati jika menabrak tubuh lawan */
    for (i = 0; i < panjang1; i++){
        if ((snake2[0].x == snake1[i].x) && (snake2[0].y == snake1[i].y)) {
            result2 = 0;
            game_state = game_state_over;
        }
    }
    for (i = 0; i < panjang2; i++){
        if ((snake1[0].x == snake2[i].x) && (snake1[0].y == snake2[i].y)) {
            result1 = 0;
            game_state = game_state_over;
        }
    }

}

char kbhit1(char ch1){
    usleep(1000);
    char chx = inputkb_1; 
    switch(chx){
        case 'w':
            if(ch1 != 's' && ch1 != 'S'){
                ch1 = chx;
            }
            break;
        case 'a':
            if(ch1 != 'd' && ch1 != 'D'){
                ch1 = chx;
            }
            break;
        case 's':
            if(ch1 != 'w' && ch1 != 'W'){
                ch1 = chx;
            }
            break;
        case 'd':
            if(ch1 != 'a' && ch1 != 'A'){
                ch1 = chx;
            }
            break;
        default:
            break;
    }
    return ch1;
}

char kbhit2(char ch2){
    usleep(1000);
    char chx = inputkb_2; 
    switch(chx){
        case 'i':
            if(ch2 != 'k' && ch2 != 'K'){
                ch2 = chx;
            }
            break;
        case 'j':
            if(ch2 != 'l' && ch2 != 'L'){
                ch2 = chx;
            }
            break;
        case 'k':
            if(ch2 != 'i' && ch2 != 'I'){
                ch2 = chx;
            }
            break;
        case 'l':
            if(ch2 != 'j' && ch2 != 'J'){
                ch2 = chx;
            }
            break;
        default:
            break;
    }
    return ch2;
}

void print() {
    /* cetak snake */
    int i;

    // cetak tubuh
    for (i = 1; i < panjang1; i ++) {
        mvwprintw(win, snake1[i].y, snake1[i].x, "o");
	}
    for (i = 1; i < panjang2; i ++) {
        mvwprintw(win, snake2[i].y, snake2[i].x, "+");
	}


    // cetak kepala 
    switch(ch1){
        case 'w':
            mvwprintw(win, snake1[0].y, snake1[0].x, "^");
            break;
        case 'a':
            mvwprintw(win, snake1[0].y, snake1[0].x, "<");
            break;
        case 's':
            mvwprintw(win, snake1[0].y, snake1[0].x, "v");
            break;
        case 'd':
            mvwprintw(win, snake1[0].y, snake1[0].x, ">");
            break;
        default:
            break;
    }
    switch(ch2){
        case 'i':
            mvwprintw(win, snake2[0].y, snake2[0].x, "^");
            break;
        case 'j':
            mvwprintw(win, snake2[0].y, snake2[0].x, "<");
            break;
        case 'k':
            mvwprintw(win, snake2[0].y, snake2[0].x, "v");
            break;
        case 'l':
            mvwprintw(win, snake2[0].y, snake2[0].x, ">");
            break;
        default:
            break;
    }

    /* cetak food */
    mvwprintw(win, food.y, food.x, "!");
}