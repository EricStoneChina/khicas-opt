#ifndef KHICAS_ZH_UI_H
#define KHICAS_ZH_UI_H

/* Shared UI translation hook. The English build leaves text untouched; the
 * Chinese build returns a GB18030-marked string for known UI labels. */
const char *zh_ui_translate(const char *text);

void zh_ui_define_status(const char *text);

/* Draw a translated label through the low-level CASIO printer.  The Chinese
 * build uses a leading marker to select GB18030; this wrapper switches the
 * character set for the duration of Bdisp_MMPrint so labels do not become
 * mojibake in dialogs that bypass the normal graphics provider. */
void zh_ui_bdisp_mmprint(int x, int y, const char *text, int mode,
                         unsigned int xlimit, int p6, int p7, int color,
                         int background, int writeflag, int p11);

void zh_ui_printmini(int *x, int *y, const char *text, int mode,
                     unsigned int xlimit, int p6, int p7, int color,
                     int background, int writeflag, int p11);

#endif
