/**
 * @author: wuyiccc
 * @date: 2023-01-02 15:52
 */

#include "head.h"
extern WINDOW *msg_win, *sub_msg_win, *info_win, *sub_info_win, *input_win, *sub_input_win;
int msgcnt = 0;

WINDOW *create_newwin(int width, int height, int startx, int starty) {
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

void destroy_win(WINDOW *win) {
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win);
    delwin(win);
}

void gotoxy(int x, int y) {
    move(y, x);
}

void gotoxy_putc(int x, int y, char c) {
    move(y, x);
    addch(c);
    refresh();
}

void gotoxy_puts(int x, int y, char* s) {
    move(y, x);
    addstr(s);
    refresh();
}

void w_gotoxy_putc(WINDOW *win, int x, int y, char c) {
    mvwaddch(win, y, x, c);
    move(LINES - 1, COLS - 1);
    wrefresh(win);
}

void w_gotoxy_puts(WINDOW *win, int x, int y, char* s) {
    mvwaddstr(win, y, x, s);
    move(LINES - 1, COLS - 1);
    wrefresh(win);
}



void init_ui(){
    initscr();
    if (!has_colors() || start_color() == ERR) {
        endwin();
        exit(1);
    }
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_YELLOW);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    msg_win = create_newwin(MSG_WIDTH, MSG_HEIGHT, 2, 1);
    sub_msg_win = subwin(msg_win, MSG_HEIGHT - 2, MSG_WIDTH - 2, 2, 3);
    scrollok(sub_msg_win, 1);

    info_win = create_newwin(INFO_WIDTH, MSG_HEIGHT, 2 + MSG_WIDTH, 1);
    sub_info_win = subwin(info_win, MSG_HEIGHT - 2, INFO_WIDTH - 2, 4, MSG_WIDTH + 1);

    input_win = create_newwin(INFO_WIDTH + MSG_WIDTH, 4, 2, MSG_HEIGHT + 1);
    sub_input_win = subwin(input_win, 2, MSG_WIDTH + INFO_WIDTH - 2, MSG_HEIGHT + 2, 4);

    wattron(msg_win, COLOR_PAIR(1));
    box(msg_win, 0, 0);

    wattron(input_win, COLOR_PAIR(2));
    box(input_win, 0, 0);

    wattron(info_win, COLOR_PAIR(3));
    box(info_win, 0, 0);

    box(sub_info_win, 0, 0);
    //box(sub_input_win, 0, 0);

    wrefresh(msg_win);
    wrefresh(input_win);
    wrefresh(info_win);
    wrefresh(sub_info_win);
    wrefresh(sub_input_win);

}

void show_msg(struct wechat_msg *msg) {
    time_t time_now = time(NULL);
    struct tm *tm = localtime(&time_now);
    char timestr[10] = {0};
    sprintf(timestr, "%02d:%02d:%02d ", tm->tm_hour, tm->tm_min, tm->tm_sec);
    if (msgcnt > MSG_HEIGHT - 3) {
        scroll(sub_msg_win);
        msgcnt = MSG_HEIGHT - 3;
    }
    w_gotoxy_puts(sub_msg_win, 0, msgcnt, timestr);
    if (msg->type & WECHAT_SYS) {
        bzero(msg->from, sizeof(msg->from));
        strcpy(msg->from, "SysInfo");
        wattron(sub_msg_win, COLOR_PAIR(1));
    } else if (msg->type & WECHAT_MSG) {
        char tmp_name[50] = {0};
        strcpy(tmp_name, msg->from);
        bzero(msg->from, sizeof(msg->from));
        sprintf(msg->from, "%s*", tmp_name);
        wattron(sub_msg_win, COLOR_PAIR(2));
    } 
    else {
        wattron(sub_msg_win, COLOR_PAIR(4));
    }
    w_gotoxy_puts(sub_msg_win, 9, msgcnt, msg->from);
    //消除颜色
    wattron(sub_msg_win, COLOR_PAIR(7));
    w_gotoxy_puts(sub_msg_win, 10 + strlen(msg->from), msgcnt, msg->msg);
    int tmp = (strlen(msg->msg) + 11 + strlen(msg->from)) / (MSG_WIDTH - 2) + 1;
    msgcnt += tmp;
    wmove(sub_input_win, 2, 1);
    wrefresh(sub_input_win);
}