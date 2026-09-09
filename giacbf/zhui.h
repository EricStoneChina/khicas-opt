#ifndef KHICAS_ZHUI_H
#define KHICAS_ZHUI_H

/*
 * Chinese UI string lookup for the KhiCAS Chinese build.
 *
 * UI strings in the source are written as  lang ? "<french>" : "<english>".
 * zhui(english, french) returns:
 *   - the Chinese translation when zh_ui_enabled is non-zero (catalogzh.cpp
 *     defines it as 1; catalogen.cpp / catalogfr.cpp define it as 0), else
 *   - french when lang is non-zero, else english (original behaviour).
 *
 * Chinese strings are GB18030 bytes with a leading 0x01 marker (see
 * khicas_gb18030.h); the table is generated from tools/ui-zh.json by
 * tools/ui-zh-gen.ps1, which also rewrites the matching ternaries into
 * zhui(...) calls.
 */

#ifdef __cplusplus
extern "C" {
#endif

extern int zh_ui_enabled;              /* defined in catalogXX.cpp */
const char * zhui(const char * en, const char * fr);

#ifdef __cplusplus
}
#endif

#endif /* KHICAS_ZHUI_H */
