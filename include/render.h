#ifndef RENDER_H
#define RENDER_H

extern void render(struct System systemSnapshot, struct System prevSystemSnapshot);
extern void help();
extern char *getBars(float val);
extern char* getColor(float value);

extern char *RED;
extern char *GREEN;
extern char *YELLOW;
extern char *BLUE;
extern char *WHITE;

extern char *BOLD;
extern char *DIM;
extern char *ITALIC;
extern char *UNDERLINE;

extern char *RESET;

#endif