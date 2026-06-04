#ifndef CONSOLE_H
#define CONSOLE_H

void Console_Setup(void);
int Console_IsAttached(void);

void AppNotify(const char *title, const char *message);

#endif