/*
* 程序:基于 C语言EasyX库 的文本输入框
* Copyright (c) 2010 - 2025 chwmq. All rights reserved.
* 编译环境:VS2022，EasyX_20220901
* github: https://github.com/chwmq/easyx_Input
* csdn: https://blog.csdn.net/chwmq
*/
#include <graphics.h>
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 结构体定义
typedef struct {
    int left, top, right, bottom;
    wchar_t* text;
    size_t maxlen;
} EasyTextBox;

typedef struct {
    int left, top, right, bottom;
    wchar_t* text;
    void (*userfunc)();
} EasyButton;

// 函数原型声明
void TextBox_Create(EasyTextBox* tb, int x1, int y1, int x2, int y2, int max);
void TextBox_Destroy(EasyTextBox* tb);
wchar_t* TextBox_Text(EasyTextBox* tb);
int TextBox_Check(EasyTextBox* tb, int x, int y);
void TextBox_Show(EasyTextBox* tb);
void TextBox_OnMessage(EasyTextBox* tb);

void Button_Create(EasyButton* btn, int x1, int y1, int x2, int y2, const wchar_t* title, void (*func)());
void Button_Destroy(EasyButton* btn);
int Button_Check(EasyButton* btn, int x, int y);
void Button_Show(EasyButton* btn);
void Button_OnMessage(EasyButton* btn);

int SaveToFile(const wchar_t* username, const wchar_t* password);
void On_btnOk_Click();

// 定义控件
EasyTextBox txtName;
EasyTextBox txtPwd;
EasyButton btnOK;

// 文本框相关函数实现
void TextBox_Create(EasyTextBox* tb, int x1, int y1, int x2, int y2, int max) {
    tb->maxlen = max;
    tb->text = (wchar_t*)malloc(sizeof(wchar_t) * max);
    tb->text[0] = 0;
    tb->left = x1;
    tb->top = y1;
    tb->right = x2;
    tb->bottom = y2;
    TextBox_Show(tb);
}

void TextBox_Destroy(EasyTextBox* tb) {
    if (tb->text != NULL) {
        free(tb->text);
        tb->text = NULL;
    }
}

wchar_t* TextBox_Text(EasyTextBox* tb) {
    return tb->text;
}

int TextBox_Check(EasyTextBox* tb, int x, int y) {
    return (tb->left <= x && x <= tb->right && tb->top <= y && y <= tb->bottom);
}

void TextBox_Show(EasyTextBox* tb) {
    int oldlinecolor = getlinecolor();
    int oldbkcolor = getbkcolor();
    int oldfillcolor = getfillcolor();

    setlinecolor(LIGHTGRAY);
    setbkcolor(0xeeeeee);
    setfillcolor(0xeeeeee);
    fillrectangle(tb->left, tb->top, tb->right, tb->bottom);
    outtextxy(tb->left + 10, tb->top + 5, tb->text);

    setlinecolor(oldlinecolor);
    setbkcolor(oldbkcolor);
    setfillcolor(oldfillcolor);
}

void TextBox_OnMessage(EasyTextBox* tb) {
    int oldlinecolor = getlinecolor();
    int oldbkcolor = getbkcolor();
    int oldfillcolor = getfillcolor();

    setlinecolor(BLACK);
    setbkcolor(WHITE);
    setfillcolor(WHITE);
    fillrectangle(tb->left, tb->top, tb->right, tb->bottom);
    outtextxy(tb->left + 10, tb->top + 5, tb->text);

    int width = textwidth(tb->text);
    int counter = 0;
    int binput = 1;

    ExMessage msg;
    while (binput) {
        while (binput && peekmessage(&msg, EX_MOUSE | EX_CHAR, false)) {
            if (msg.message == WM_LBUTTONDOWN) {
                if (msg.x < tb->left || msg.x > tb->right || msg.y < tb->top || msg.y > tb->bottom) {
                    binput = 0;
                    break;
                }
            }
            else if (msg.message == WM_CHAR) {
                size_t len = wcslen(tb->text);
                switch (msg.ch) {
                case '\b':
                    if (len > 0) {
                        tb->text[len - 1] = 0;
                        width = textwidth(tb->text);
                        counter = 0;
                        clearrectangle(tb->left + 10 + width, tb->top + 1, tb->right - 1, tb->bottom - 1);
                    }
                    break;
                case '\r':
                case '\n':
                    binput = 0;
                    break;
                default:
                    if (len < tb->maxlen - 1) {
                        tb->text[len++] = msg.ch;
                        tb->text[len] = 0;
                        clearrectangle(tb->left + 10 + width + 1, tb->top + 3, tb->left + 10 + width + 1, tb->bottom - 3);
                        width = textwidth(tb->text);
                        counter = 0;
                        outtextxy(tb->left + 10, tb->top + 5, tb->text);
                    }
                }
            }
            peekmessage(NULL, EX_MOUSE | EX_CHAR);
        }

        counter = (counter + 1) % 32;
        if (counter < 16)
            line(tb->left + 10 + width + 1, tb->top + 3, tb->left + 10 + width + 1, tb->bottom - 3);
        else
            clearrectangle(tb->left + 10 + width + 1, tb->top + 3, tb->left + 10 + width + 1, tb->bottom - 3);

        Sleep(20);
    }

    clearrectangle(tb->left + 10 + width + 1, tb->top + 3, tb->left + 10 + width + 1, tb->bottom - 3);

    setlinecolor(oldlinecolor);
    setbkcolor(oldbkcolor);
    setfillcolor(oldfillcolor);

    TextBox_Show(tb);
}

// 按钮相关函数实现
void Button_Create(EasyButton* btn, int x1, int y1, int x2, int y2, const wchar_t* title, void (*func)()) {
    btn->text = (wchar_t*)malloc(sizeof(wchar_t) * (wcslen(title) + 1));
    wcscpy_s(btn->text, wcslen(title) + 1, title);
    btn->left = x1;
    btn->top = y1;
    btn->right = x2;
    btn->bottom = y2;
    btn->userfunc = func;
    Button_Show(btn);
}

void Button_Destroy(EasyButton* btn) {
    if (btn->text != NULL) {
        free(btn->text);
        btn->text = NULL;
    }
}

int Button_Check(EasyButton* btn, int x, int y) {
    return (btn->left <= x && x <= btn->right && btn->top <= y && y <= btn->bottom);
}

void Button_Show(EasyButton* btn) {
    int oldlinecolor = getlinecolor();
    int oldbkcolor = getbkcolor();
    int oldfillcolor = getfillcolor();

    setlinecolor(BLACK);
    setbkcolor(WHITE);
    setfillcolor(WHITE);
    fillrectangle(btn->left, btn->top, btn->right, btn->bottom);
    outtextxy(btn->left + (btn->right - btn->left - textwidth(btn->text) + 1) / 2,
        btn->top + (btn->bottom - btn->top - textheight(btn->text) + 1) / 2,
        btn->text);

    setlinecolor(oldlinecolor);
    setbkcolor(oldbkcolor);
    setfillcolor(oldfillcolor);
}

void Button_OnMessage(EasyButton* btn) {
    if (btn->userfunc != NULL)
        btn->userfunc();
}

// 文件保存函数
int SaveToFile(const wchar_t* username, const wchar_t* password) {
    FILE* file;
    // 使用 "w" 模式打开文件，会覆盖原有内容
    errno_t err = _wfopen_s(&file, L"输入框内容.txt", L"w,ccs=UTF-8");

    if (err != 0 || file == NULL) {
        return 0;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    //时间[%04d-%02d-%02d %02d:%02d:%02d]st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, 
    // 写入文件：输入框1+输入框2
    fwprintf(file, L"输入框1的内容: %s, 输入框2的内容: %s\n", username, password);

    fclose(file);
    return 1;
}

// 按钮点击事件
void On_btnOk_Click() {
    if (SaveToFile(TextBox_Text(&txtName), TextBox_Text(&txtPwd))) {
        MessageBox(GetHWnd(), L"数据已成功保存到文件", L"提示", MB_OK);
    }
    else {
        MessageBox(GetHWnd(), L"保存数据失败", L"错误", MB_OK);
    }
    /*
    // 判断密码是否正确
    if (wcscmp(L"123456", TextBox_Text(&txtPwd)) != 0) {
        MessageBox(GetHWnd(), L"密码错误", L"错误", MB_OK);
    }
    else {
        wchar_t s[100] = L"Hello, ";
        wcscat_s(s, 100, TextBox_Text(&txtName));
        MessageBox(GetHWnd(), s, L"Hello", MB_OK);
    }*/
}

// 主函数
int main() {
    initgraph(640, 480, SHOWCONSOLE);

    setbkcolor(0xeeeeee);
    cleardevice();
    settextcolor(BLACK);
    outtextxy(50, 55, L"用户名：");
    TextBox_Create(&txtName, 120, 50, 400, 75, 10);
    outtextxy(50, 105, L"密　码：");
    TextBox_Create(&txtPwd, 120, 100, 400, 125, 10);
    Button_Create(&btnOK, 320, 150, 400, 175, L"OK", On_btnOk_Click);

    ExMessage msg;
    while (true) {
        msg = getmessage(EX_MOUSE);

        if (msg.message == WM_LBUTTONDOWN) {
            if (TextBox_Check(&txtName, msg.x, msg.y))
                TextBox_OnMessage(&txtName);

            if (TextBox_Check(&txtPwd, msg.x, msg.y))
                TextBox_OnMessage(&txtPwd);

            if (Button_Check(&btnOK, msg.x, msg.y))
                Button_OnMessage(&btnOK);
        }
    }

    closegraph();
    TextBox_Destroy(&txtName);
    TextBox_Destroy(&txtPwd);
    Button_Destroy(&btnOK);

    return 0;
}
