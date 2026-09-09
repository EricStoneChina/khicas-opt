#include <stdio.h>
#include <fxcg/keyboard.h>
#include "giacPCH.h"
#include <fxcg/display.h>
#include <fxcg/file.h>
#include <fxcg/keyboard.h>
#include <fxcg/system.h>
#include <fxcg/misc.h>
#include <fxcg/app.h>
#include <fxcg/serial.h>
extern "C" {
#include <fxcg/rtc.h>
}
#include <fxcg/heap.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alloca.h>
#include <ctype.h>

#include "history.h"
#include "dConsole.h"
#include "main.h"
  //#include "memmgr.h"
#include "catalogGUI.hpp"
#include "fileGUI.hpp"
#include "textGUI.hpp"
#include "graphicsProvider.hpp"
#include "constantsProvider.hpp"
#include "kdisplay.h"
#include "input_lexer.h"
#include "input_parser.h"
#define STATUS_AREA_PX 24

#define CAT_CATEGORY_ALL 0
#define CAT_CATEGORY_ALGEBRA 1
#define CAT_CATEGORY_LINALG 2
#define CAT_CATEGORY_CALCULUS 3
#define CAT_CATEGORY_ARIT 4
#define CAT_CATEGORY_COMPLEXNUM 5
#define CAT_CATEGORY_PLOT 6
#define CAT_CATEGORY_POLYNOMIAL 7
#define CAT_CATEGORY_PROBA 8
#define CAT_CATEGORY_PROGCMD 9
#define CAT_CATEGORY_REAL 10
#define CAT_CATEGORY_SOLVE 11
#define CAT_CATEGORY_STATS 12
#define CAT_CATEGORY_TRIG 13
#define CAT_CATEGORY_OPTIONS 14
#define CAT_CATEGORY_LIST 15
#define CAT_CATEGORY_MATRIX 16
#define CAT_CATEGORY_PROG 17
#define CAT_CATEGORY_SOFUS 18
#define CAT_CATEGORY_PHYS 19
#define CAT_CATEGORY_UNIT 20
#define CAT_CATEGORY_2D 21
#define CAT_CATEGORY_3D 22
#define CAT_CATEGORY_LOGO 23 // should be the last one
#define XCAS_ONLY 0x80000000

using namespace giac;
extern giac::context * contextptr;
void reset_alpha(){
  SetSetupSetting( (unsigned int)0x14, 0);	
  DisplayStatusArea();
}

int lang=0;
int zh_ui_enabled=1; /* Chinese UI strings on (see zhui.cc) */
const char ram_filename[]="\\\\fls0\\khicaszh.8c2";
const catalogFunc completeCat[] = { // list of all functions (including some not in any category)
  {" loop for", "for ", "Defined loop.", "#\nfor ", 0, CAT_CATEGORY_PROG},
  {" loop in list", "for in", "Loop on all elements of a list.", "#\nfor in", 0, CAT_CATEGORY_PROG},
  {" loop while", "while ", "Undefined loop.", "#\nwhile ", 0, CAT_CATEGORY_PROG},
  {" test if", "if ", "Test", "#\nif ", 0, CAT_CATEGORY_PROG},
  {" test else", "else ", "Test false case", 0, 0, CAT_CATEGORY_PROG},
  {" function def", "f(x):=", "Definition of function.", "#\nf(x):=", 0, CAT_CATEGORY_PROG},
  {" local j,k;", "local ", "Local variables declaration (Xcas)", 0, 0, CAT_CATEGORY_PROG},
  {" range(a,b)", 0, "In range [a,b) (a included, b excluded)", "# in range(1,10)", 0, CAT_CATEGORY_PROG},
  {" return res", "return ", "Leaves current function and returns res.", 0, 0, CAT_CATEGORY_PROG},
  {" edit list ", "list ", "List creation wizzard.", 0, 0, CAT_CATEGORY_LIST},
  {" edit matrix ", "matrix ", "Matrix creation wizzard.", 0, 0, CAT_CATEGORY_MATRIX},
    {" mksa(x)", 0, "Conversion to MKSA units", 0, 0, CAT_CATEGORY_PHYS | (CAT_CATEGORY_UNIT << 8) | XCAS_ONLY},
    {" ufactor(a,b)", 0, "Factorize unit b in a", "100_J,1_kW", 0, CAT_CATEGORY_PHYS | (CAT_CATEGORY_UNIT << 8) | XCAS_ONLY},
    {" usimplify(a)", 0, "Simplify unit", "100_l/10_cm^2", 0, CAT_CATEGORY_PHYS | (CAT_CATEGORY_UNIT << 8) | XCAS_ONLY},
  {"!", "!", "Logical not (prefix) or factorial of n (suffix).", "#7!", "~!b", CAT_CATEGORY_PROGCMD},
  {"#", "#", "Python comment, for Xcas comment type //. Shortcut ALPHA F2", 0, 0, CAT_CATEGORY_PROG},
  {"%", "%", "a % b means a modulo b", 0, 0, CAT_CATEGORY_ARIT | (CAT_CATEGORY_PROGCMD << 8)},
  {"&", "&", "Logical and or +", "#1&2", 0, CAT_CATEGORY_PROGCMD},
  {":=", ":=", "Set variable value. Shortcut SHIFT F1", "#a:=3", 0, CAT_CATEGORY_PROGCMD|(CAT_CATEGORY_SOFUS<<8)|XCAS_ONLY},
  {"<", "<", "Shortcut SHIFT F2", 0, 0, CAT_CATEGORY_PROGCMD},
  {"=>", "=>", "Store value in variable or conversion (touche ->). For example 5=>a or x^4-1=>* or (x+1)^2=>+ or sin(x)^2=>cos.", "#5=>a", "#15_ft=>_cm", CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_PHYS <<8) | (CAT_CATEGORY_UNIT << 16) | XCAS_ONLY},
  {">", ">", "Shortcut F2.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"\\", "\\", "\\ char", 0, 0, CAT_CATEGORY_PROGCMD},
  {"_", "_", "_ char, shortcut (-).", 0, 0, CAT_CATEGORY_PROGCMD},
    {"_(km/h)", "_(km/h)", "Speed kilometer per hour", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_(m/s)", "_(m/s)", "Speed meter/second", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_(m/s^2)", "_(m/s^2)", "Acceleration", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_(m^2/s)", "_(m^2/s)", "Viscosity", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_A", 0, "Ampere", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Bq", 0, "Becquerel", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_C", 0, "Coulomb", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Ci", 0, "Curie", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_F", 0, "Farad", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_F_", 0, "Faraday constant", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_G_", 0, "Gravitation force=_G_*m1*m2/r^2", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_H", 0, "Henry", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Hz", 0, "Hertz", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_J", 0, "Joule=kg*m^2/s^2", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_K", 0, "Temperature in Kelvin", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Kcal", 0, "Energy kilo-calorie", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_MeV", 0, "Energy mega-electron-Volt", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_N", 0, "Force Newton=kg*m/s^2", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_NA_", 0, "Avogadro constant", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_Ohm", 0, "Ohm", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_PSun_", 0, "Sun power", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_Pa", 0, "Pressure in Pascal=kg/m/s^2", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_REarth_", 0, "Earth radius", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_RSun_", 0, "Sun radius", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_R_", 0, "Boltzmann constant (per mol)", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_S", 0, "", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_StdP_", 0, "Standard pressure", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_StdT_", 0, "Standard temperature (0 degre Celsius in Kelvins)", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_Sv", 0, "Sievert", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_T", 0, "Tesla", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_V", 0, "Volt", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Vm_", 0, "Volume molaire", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_W", 0, "Watt=kg*m^2/s^3", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_Wb", 0, "Weber", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_alpha_", 0, "fine structure constant", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_c_", 0, "speed of light", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_cd", 0, "candela", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
  {"_cdf", "_cdf", "Suffix to get a cumulative distribution function. Type F2 for inverse cumulative distribution function _icdf suffix.", "#_icdf", 0, CAT_CATEGORY_PROBA|XCAS_ONLY},
    {"_d", 0, "day", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_deg", 0, "degree", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_eV", 0, "electron-Volt", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_epsilon0_", 0, "vacuum permittivity", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_ft", 0, "feet", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_g_", 0, "Earth gravity (ground)", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_grad", 0, "grades (angle unit(", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_h", 0, "Hour", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_h_", 0, "Planck constant", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_ha", 0, "hectare", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_hbar_", 0, "Planck constant/(2*pi)", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_inch", 0, "inches", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_kWh", 0, "kWh", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_k_", 0, "Boltzmann constant", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_kg", 0, "kilogram", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_l", 0, "liter", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_m", 0, "meter", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_mEarth_", 0, "Earth mass", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_m^2", 0, "Area in m^2", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_m^3", 0, "Volume in m^3", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_me_", 0, "electron mass", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_miUS", 0, "US miles", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_mn", 0, "minute", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_mp_", 0, "proton mass", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_mpme_", 0, "proton/electron mass-ratio", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_mu0_", 0, "", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_phi_", 0, "magnetic flux quantum", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_plot", "_plot", "Suffix for a regression graph.", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];polynomial_regression_plot(X,Y,2);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
    {"_qe_", 0, "electron charge", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_qme_", 0, "_q_/_me_", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_rad", 0, "radians", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_rem", 0, "rem", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_s", 0, "second", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_sd_", 0, "Sideral day", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_syr_", 0, "Siderale year", 0, 0, CAT_CATEGORY_PHYS | XCAS_ONLY},
    {"_tr", 0, "tour (angle unit)", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
    {"_yd", 0, "yards", 0, 0, CAT_CATEGORY_UNIT | XCAS_ONLY},
  {"a and b", " and ", "Logical and", 0, 0, CAT_CATEGORY_PROGCMD},
  {"a or b", " or ", "Logical or", 0, 0, CAT_CATEGORY_PROGCMD},
  {"a2q(A,[vars])", 0, "Matrix to quadratic form", "[[1,2],[2,3]]","[[1,2],[2,3]],[x,y]", CAT_CATEGORY_LINALG},
  {"abcuv(a,b,c)", 0, "Find 2 polynomial u,v such that a*u+b*v=c","x+1,x^2-2,x", 0, CAT_CATEGORY_POLYNOMIAL},
  {"abs(x)", 0, "\x01""\xbe""""\xf8""""\xb6""""\xd4""""\xd6""""\xb5"" / ""\xb8""""\xb4""""\xca""""\xfd""""\xb5""""\xc4""""\xc4""""\xa3"" / ""\xcf""""\xf2""""\xc1""""\xbf""""\xb7""""\xb6""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":abs(x)", "#abs(-3)", "#abs(3+4i)", CAT_CATEGORY_COMPLEXNUM | (CAT_CATEGORY_REAL<<8)},
  {"altitude(A,B,C)", 0, "Altitude in triangle ABC from A", "1,i,2+i", 0,CAT_CATEGORY_2D},
  {"append", 0, "Adds an element at the end of a list","#l.append(x)", 0, CAT_CATEGORY_LIST},
  {"approx(x)", 0, "\x01""\xca""""\xfd""""\xd6""""\xb5""""\xbd""""\xfc""""\xcb""""\xc6""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":approx(expr) ""\xbb""""\xf2"" approx(expr,n)", "#approx(pi)", "#approx(sqrt(2),20)", CAT_CATEGORY_REAL},
  {"area(objet)", 0, "Algebric area", "circle(0,1)", "triangle(-1,1+i,3)", CAT_CATEGORY_2D  },
  {"arg(z)", 0, "\x01""\xb8""""\xb4""""\xca""""\xfd""""\xb5""""\xc4""""\xb7""""\xf8""""\xbd""""\xc7""(""\xbb""""\xa1""""\xb6""""\xc8"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":arg(z)", "#arg(1+i)", "#arg(-1)", CAT_CATEGORY_COMPLEXNUM},
  {"asc(string)", 0, "List of ASCII codes os a string", "\"Hello\"", 0, CAT_CATEGORY_ARIT},
  {"assume(hyp)", 0, "Assumption on variable.", "x>1", "x>-1 and x<1", CAT_CATEGORY_PROGCMD|(CAT_CATEGORY_SOFUS<<8)},
  {"avance(n)", "avance ", "Turtle forward n steps, default n=10", "#avance 30", 0, CAT_CATEGORY_LOGO},
  {"axes", "axes", "Axes visible or not axes=1 or 0", "#axes=0", 0, CAT_CATEGORY_PROGCMD << 8|XCAS_ONLY},
  {"baisse_crayon ", "baisse_crayon ", "Turtle moves with the pen writing.", 0, 0, CAT_CATEGORY_LOGO},
  {"barplot(list)", 0, "Bar plot of 1-d statistic series data in list.", "[3/2,2,1,1/2,3,2,3/2]", 0, CAT_CATEGORY_STATS},
  {"barycenter([pnt,coeff],...)", 0, "Barycenter of a sequence of [point,coefficient]. Run isobarycenter if all coefficients are equal", "[1,1],[i,1],[2,3]", 0, CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"binomial(n,p,k)", 0, "\x01""\xb6""""\xfe""""\xcf""""\xee""""\xca""""\xbd""""\xcf""""\xb5""""\xca""""\xfd"" C(n,k)""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":binomial(n,k)", "#binomial(10,3)", "#binomial(n,2)", CAT_CATEGORY_PROBA},
    {"bisector(A,B,C)", 0, "Bisector of angle AB,AC", "1,i,2+i", 0,CAT_CATEGORY_2D},
  {"bitxor", "bitxor", "Exclusive or", "#bitxor(1,2)", 0, CAT_CATEGORY_PROGCMD},
  {"black", "black", "Display option", "#display=black", 0, CAT_CATEGORY_PROGCMD},
  {"blue", "blue", "Display option", "#display=blue", 0, CAT_CATEGORY_PROGCMD},
  {"camembert(list)", 0, "Camembert pie-chart of a 1-d statistical series.", "[[\"France\",6],[\"Germany\",12],[\"Switzerland\",5]]", 0, CAT_CATEGORY_STATS},
  {"cache_tortue ", "cache_tortue ", "Hide turtle (once the picture has been drawn).", 0, 0, CAT_CATEGORY_LOGO},
  {"ceil(x)", 0, "\x01""\xcf""""\xf2""""\xc9""""\xcf""""\xc8""""\xa1""""\xd5""""\xfb""(""\xb2""""\xbb""""\xd0""""\xa1""""\xd3""""\xda"" x ""\xb5""""\xc4""""\xd7""""\xee""""\xd0""""\xa1""""\xd5""""\xfb""""\xca""""\xfd"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":ceil(x)", "#ceil(1.2)", "#ceil(-2.7)", CAT_CATEGORY_REAL},
  {"center(objet)", 0, "Circle or sphere center. For ellipse or hyperbola, returns center, one focus and a point on the conic. For a parabola, returns focus and vertex.", "circle(0,1)", "sphere([0,0,0],[1,1,1])", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"cfactor(p)", 0, "Factorization over C.", "x^4-1", 0, CAT_CATEGORY_ALGEBRA | (CAT_CATEGORY_COMPLEXNUM << 8)},
  {"char(liste)", 0, "Converts a list of ASCII codes to a string.", "[97,98,99]", 0, CAT_CATEGORY_ARIT},
  {"charpoly(M,x)", 0, "Characteristic polynomial of matrix M in variable x.", "[[1,2],[3,4]],x", 0, CAT_CATEGORY_MATRIX},
  {"circle(center,radius)", 0, "Circle", "2+i,3", "1-i,1+i", CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_2D << 8)},
  {"circumcircle(A,B,C)", 0, "Circumcircle", "-1,2+i,3", 0, CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_2D << 8) | XCAS_ONLY},
  {"clearscreen()", "clearscreen()", "Clear screen.", 0, 0, CAT_CATEGORY_PROGCMD|XCAS_ONLY},
  {"coeff(p,x,n)", 0, "Coefficient of x^n in polynomial p.", 0, 0, CAT_CATEGORY_POLYNOMIAL},
  {"comb(n,k)", 0, "Returns nCk", "10,4", 0, CAT_CATEGORY_PROBA},
  {"cond(A,[1,2,inf])", 0, "Nombre de condition d'une matrice par rapport a la norme specifiee (par defaut 1)", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX},
  {"cone(A,v,theta,[h])", 0, " cone with vertex A, direction v, and with half_angle t [and with altitudes h and -h]", "[0,0,0],[0,0,1],pi/6", "[0,0,0],[0,0,1],pi/6,4", CAT_CATEGORY_3D},
  {"conic(expression)", 0, "Conic given by a polynomial equation of degree 2 or by 5 vertices", "x^2+x*y+y^2=5", "1,i,2+i,3-i,4+2i", CAT_CATEGORY_2D},
  {"coordinates(object)", 0, "Coordonnees (cartesian))", "point(1,2)", "point(1,2,3)", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"conj(z)", 0, "\x01""\xb9""""\xb2""""\xe9""""\xee""""\xb8""""\xb4""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":conj(z)", "#conj(3+4i)", "#conj(1-i)", CAT_CATEGORY_COMPLEXNUM},
  {"correlation(l1,l2)", 0, "\x01""\xcf""""\xe0""""\xb9""""\xd8""""\xcf""""\xb5""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":correlation(L1,L2)", "#correlation([1,2,3],[2,4,6])", "#correlation([1,2,3],[3,1,2])", CAT_CATEGORY_STATS},
  {"covariance(l1,l2)", 0, "Covariance of lists l1 and l2", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"cpartfrac(p,x)", 0, "Partial fraction decomposition over C.", "1/(x^4-1)", 0, CAT_CATEGORY_ALGEBRA | (CAT_CATEGORY_COMPLEXNUM << 8)},
  {"crayon ", "crayon ", "Turtle drawing color", "#crayon red", 0, CAT_CATEGORY_LOGO},
  {"cross(u,v)", 0, "Cross product of vectors u and v.","[1,2,3],[0,1,3]", 0, CAT_CATEGORY_LINALG},
  {"csolve(equation,x)", 0, "Solve equation (or polynomial system) in exact mode over the complex numbers.","x^2+x+1=0", 0, CAT_CATEGORY_SOLVE| (CAT_CATEGORY_COMPLEXNUM << 8)},
  {"cube(A,B,C)", 0, "Cube of edge AB with one face in plane ABC", "[0,0,0],[1,0,0],[0,1,0]","[0,0,0],[0,2,sqrt(5)/2+3/2],[0,0,1]", CAT_CATEGORY_3D},
  {"curl(u,vars)", 0, "Curl of vector u.", "[2*x*y,x*z,y*z],[x,y,z]", 0, CAT_CATEGORY_LINALG},
  {"cyan", "cyan", "Display option", "#display=cyan", 0, CAT_CATEGORY_PROGCMD},
  {"cylinder(A,v,r,[h])", 0, "Cylinder of axis A,v and radius r [and optional altitude h]", "[0,0,0],[0,1,0],2", "[0,0,0],[0,1,0],2,3", CAT_CATEGORY_3D},
  {"debug(f(args))", 0, "Runs user function f in step by step mode.", 0, 0, CAT_CATEGORY_PROG},
  {"degree(p,x)", 0, "Degre of polynomial p in x.", "x^4-1", 0, CAT_CATEGORY_POLYNOMIAL},
  {"denom(x)", 0, "Denominator of expression x.", "3/4", 0, CAT_CATEGORY_POLYNOMIAL},
  {"desolve(equation,t,y)", 0, "Exact differential equation solving.", "desolve([y'+y=exp(x),y(0)=1])", "[y'=[[1,2],[2,1]]*y+[x,x+1],y(0)=[1,2]]", CAT_CATEGORY_SOLVE | (CAT_CATEGORY_CALCULUS << 8)},
  {"det(A)", 0, "\x01""\xb7""""\xbd""""\xd5""""\xf3""""\xb5""""\xc4""""\xd0""""\xd0""""\xc1""""\xd0""""\xca""""\xbd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":det(A)", "#det([[1,2],[3,4]])", "#det([[1,2,3],[4,5,6],[7,8,10]])", CAT_CATEGORY_MATRIX | (CAT_CATEGORY_LINALG << 8)},
  {"diff(f,var,[n])", 0, "\x01""\xc7""""\xf3""""\xb5""""\xbc""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":diff(f,x) ""\xd2""""\xbb""""\xbd""""\xd7""""\xb5""""\xbc""""\xca""""\xfd"";diff(f,x,n) n ""\xbd""""\xd7""""\xb5""""\xbc""""\xca""""\xfd""", "#diff(sin(x),x)", "#diff(x^3,x,2)", CAT_CATEGORY_CALCULUS},
  {"discriminant(p,x)", 0, "Discriminant of polynomial p wrt x", "x^2+x+1,x", 0, CAT_CATEGORY_POLYNOMIAL},
  {"display", "display", "Display option", "#display=red", 0, CAT_CATEGORY_PROGCMD},
  {"disque n", "disque ", "Filled circle tangent to the turtle, radius n. Run disque n,theta for a filled arc of circle, theta in degrees, or disque n,theta,segment for a segment of circle.", "#disque 30", "#disque(30,90)", CAT_CATEGORY_LOGO},
  {"dodecahedron(A,B,C)", 0, "Dodecahedron of edge AB with one face in plane ABC", "[0,0,0],[0,2,sqrt(5)/2+3/2],[0,0,1]", 0, CAT_CATEGORY_3D},
  {"dot(a,b)", 0, "Dot product of 2 vectors. Shortcut: *", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_LINALG},
  {"draw_arc(x1,y1,rx,ry,theta1,theta2,c)", 0, "Pixelised arc of ellipse.", "100,100,60,80,0,pi,magenta", 0, CAT_CATEGORY_PROGCMD},
  {"draw_circle(x1,y1,r,c)", 0, "Pixelised circle. Option: filled", "100,100,60,cyan+filled", 0, CAT_CATEGORY_PROGCMD},
  {"draw_line(x1,y1,x2,y2,c)", 0, "Pixelised line.", "100,50,300,200,blue", 0, CAT_CATEGORY_PROGCMD},
  {"draw_pixel(x,y,color)", 0, "Colors pixel x,y. Run draw_pixel() to synchronise screen.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"draw_polygon([[x1,y1],...],c)", 0, "Pixelised polygon.", "[[100,50],[30,20],[60,70]],red+filled", 0, CAT_CATEGORY_PROGCMD},
  {"draw_rectangle(x,y,w,h,c)", 0, "Rectangle.", "100,50,30,20,red+filled", 0, CAT_CATEGORY_PROGCMD},
  {"draw_string(s,x,y,c)", 0, "Draw string s at pixel x,y", "\"Bonjour\",80,60", 0, CAT_CATEGORY_PROGCMD},
#ifndef TURTLETAB
  {"ecris ", "ecris ", "Write at turtle position", "#ecris \"hello\"", 0, CAT_CATEGORY_LOGO},
#endif
  {"efface", "efface", "Reset turtle", 0, 0, CAT_CATEGORY_LOGO},
  {"egcd(A,B)", 0, "\x01""\xc0""""\xa9""""\xd5""""\xb9""""\xc5""""\xb7""""\xbc""""\xb8""""\xc0""""\xef""""\xb5""""\xc3""""\xcb""""\xe3""""\xb7""""\xa8"":""\xc7""""\xf3"" U,V,D ""\xca""""\xb9"" A*U+B*V=D=gcd(A,B)""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":egcd(A,B)", "#egcd(x^2+3x+1,x^2-5x-1)", "#egcd(23,13)", CAT_CATEGORY_POLYNOMIAL},
  {"elif test", "elif ", "Test cascade", 0, 0, CAT_CATEGORY_PROG},
  {"ellipse(F1,F2,M)", 0, "Ellipse given by 2 focus and one point", "-1,1,2", 0, CAT_CATEGORY_2D},
  {"eigenvals(A)", 0, "Eigenvalues of matrix  A.", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX | (CAT_CATEGORY_LINALG << 8) | XCAS_ONLY},
  {"eigenvects(A)", 0, "Eigenvectors of matrix A.", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX  | (CAT_CATEGORY_LINALG << 8) | XCAS_ONLY},
  {"equation(object)", 0, "Cartesian equation. Run parameq for parametric equation", "circle(0,1)", "ellipse(-1,1,3)", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"erf(x)", 0, "Error function of x.", "1.2", 0, CAT_CATEGORY_PROBA},
  {"erfc(x)", 0, "Complementary error function of x.", "1.2", 0, CAT_CATEGORY_PROBA},
  {"euler(n)", 0, "\x01""\xc5""""\xb7""""\xc0""""\xad""""\xba""""\xaf""""\xca""""\xfd"" ""\xa6""""\xd5""(n):""\xd0""""\xa1""""\xd3""""\xda"" n ""\xc7""""\xd2""""\xd3""""\xeb"" n ""\xbb""""\xa5""""\xcb""""\xd8""""\xb5""""\xc4""""\xd5""""\xfb""""\xca""""\xfd""""\xb8""""\xf6""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":euler(n)", "#euler(25)", "#euler(100)", CAT_CATEGORY_ARIT},
  {"eval(f)", 0, "Evals f.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"evalc(z)", 0, "Write z=x+i*y.", "1/(1+i*sqrt(3))", 0, CAT_CATEGORY_COMPLEXNUM},
  {"exact(x)", 0, "Converts x to a rational. Shortcut shift S-D", "1.2", 0, CAT_CATEGORY_REAL},
  {"exp2trig(expr)", 0, "Convert complex exponentials to sin/cos", "exp(i*x)", 0, CAT_CATEGORY_TRIG},
  {"exponential_regression(Xlist,Ylist)", 0, "Exponential regression.", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"exponential_regression_plot(Xlist,Ylist)", 0, "Exponential regression plot.", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];exponential_regression_plot(X,Y);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
  {"exponentiald(lambda,x)", 0, "Exponential distribution law of  parameter lambda. exponentiald_cdf(lambda,x) probability that \"exponential distribution <=x\" e.g. exponentiald_cdf(2,3). exponentiald_icdf(lambda,t) returns x such that \"exponential distribution <=x\" has probability t, e.g, exponentiald_icdf(2,0.95) ", "5.1,3.4", 0, CAT_CATEGORY_PROBA},
  {"extend", 0, "Merge 2 lists. Note that + does not merge lists, it adds vectors","#l1.extend(l2)", 0, CAT_CATEGORY_LIST},
  {"factor(p,[x])", 0, "\x01""\xd2""""\xf2""""\xca""""\xbd""""\xb7""""\xd6""""\xbd""""\xe2""""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""(""\xd5""""\xfb""""\xca""""\xfd""""\xb7""""\xd6""""\xbd""""\xe2""""\xd3""""\xc3"" ifactor)""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":factor(p) ""\xbb""""\xf2"" factor(p,x)", "#factor(x^4-1)", "#factor(x^6+1,sqrt(3))", CAT_CATEGORY_ALGEBRA| (CAT_CATEGORY_POLYNOMIAL << 8)},
  {"filled", "filled", "Display option", 0, 0, CAT_CATEGORY_PROGCMD},
  {"float(x)", 0, "Converts x to a floating point value.", "pi", 0, CAT_CATEGORY_REAL},
  {"floor(x)", 0, "\x01""\xcf""""\xf2""""\xcf""""\xc2""""\xc8""""\xa1""""\xd5""""\xfb""(""\xb2""""\xbb""""\xb3""""\xac""""\xb9""""\xfd"" x ""\xb5""""\xc4""""\xd7""""\xee""""\xb4""""\xf3""""\xd5""""\xfb""""\xca""""\xfd"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":floor(x)", "#floor(pi)", "#floor(-2.7)", CAT_CATEGORY_REAL},
  {"fourier_an(f,x,T,n,a)", 0, "Cosine Fourier coefficients of f", "x^2,x,2*pi,n,-pi", 0, CAT_CATEGORY_CALCULUS},
  {"fourier_bn(f,x,T,n,a)", 0, "Sine Fourier coefficients of f", "x^2,x,2*pi,n,-pi", 0, CAT_CATEGORY_CALCULUS},
  {"fourier_cn(f,x,T,n,a)", 0, "Exponential Fourier coefficients of f", "x^2,x,2*pi,n,-pi", 0, CAT_CATEGORY_CALCULUS},
  {"from math/... import *", "from math import *", "Access to math or to random functions ([random]) or turtle with English commandnames [turtle]. Math import is not required in KhiCAS", "#from random import *", "#from turtle import *", CAT_CATEGORY_PROG},
  {"fsolve(equation,x=a..b)", 0, "\x01""\xca""""\xfd""""\xd6""""\xb5""""\xbd""""\xe2""""\xb7""""\xbd""""\xb3""""\xcc""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":fsolve(""\xb7""""\xbd""""\xb3""""\xcc"",x=""\xb3""""\xf5""""\xd6""""\xb5"")", "#fsolve(cos(x)=x,x=1)", "#fsolve(x^2-2=0,x=1)", CAT_CATEGORY_SOLVE},
  // {"function f(x):...", "function f(x) local y;   ffunction:;", "Function definition.", "#function f(x) local y; y:=x^2; return y; ffunction:;", 0, CAT_CATEGORY_PROG},
  {"gauss(q)", 0, "Quadratic form reduction", "x^2+x*y+x*z+y^2+z^2,[x,y,z]", 0, CAT_CATEGORY_LINALG},
  {"gcd(a,b,...)", 0, "\x01""\xd7""""\xee""""\xb4""""\xf3""""\xb9""""\xab""""\xd2""""\xf2""""\xca""""\xfd"" / ""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xd7""""\xee""""\xb4""""\xf3""""\xb9""""\xab""""\xd2""""\xf2""""\xca""""\xbd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":gcd(a,b,...)", "#gcd(23,13)", "#gcd(x^2-1,x^3-1)", CAT_CATEGORY_ARIT | (CAT_CATEGORY_POLYNOMIAL << 8)},
  {"gl_x", "gl_x", "Display settings X gl_x=xmin..xmax", "#gl_x=0..2", 0, CAT_CATEGORY_PROGCMD},
  {"gl_y", "gl_y", "Display settings Y gl_y=ymin..ymax", "#gl_y=-1..1", 0, CAT_CATEGORY_PROGCMD},
  {"gramschmidt(M)", 0, "Gram-Schmidt orthonormalization (line vectors or linearly independent set of vectors)", "[[1,2,3],[4,5,6]]", "[1,1+x],(p,q)->integrate(p*q,x,-1,1)", CAT_CATEGORY_LINALG},
  {"green", "green", "Display option", "#display=green", 0, CAT_CATEGORY_PROGCMD},
  {"halftan(expr)", 0, "Convert cos, sin, tan with tan(angle/2).","cos(x)", 0, CAT_CATEGORY_TRIG},
  {"hermite(n)", 0, "n-th Hermite polynomial", "10", 0, CAT_CATEGORY_POLYNOMIAL},
  {"hilbert(n)", 0, "Hilbert matrix of order n.", "4", 0, CAT_CATEGORY_MATRIX},
  {"histogram(list,min,size)", 0, "Histogram of data in list, classes begin at min of size size.","ranv(100,uniformd,0,1),0,0.1", 0, CAT_CATEGORY_STATS},
  {"homothety(center,ratio,object)", 0, "Image of object by homothety of ratio", "0,2,circle(1,1)", 0, CAT_CATEGORY_2D },
  {"hyperbola(F1,F2,M)", 0, "Hyperbola given by 2 focus and one point", "-2-i,2+i,1", 0, CAT_CATEGORY_2D},
  {"iabcuv(a,b,c)", 0, "Find 2 integers u,v such that a*u+b*v=c","23,13,15", 0, CAT_CATEGORY_ARIT},
  {"ichinrem([a,m],[b,n])", 0,"Integer chinese remainder of a mod m and b mod n.", "[3,13],[2,7]", 0, CAT_CATEGORY_ARIT},
  {"icosahedron(A,B,C)", 0, "Icosahedron with center A, vertex B and such that the plane ABC contains one vertex among the 5 nearest vertices from B ", "[0,0,0],[sqrt(5),0,0],[1,2,0]", 0, CAT_CATEGORY_3D},
   {"idivis(n)", 0, "Returns the list of divisors of an integer n.", "10", 0, CAT_CATEGORY_ARIT},
  {"idn(n)", 0, "\x01n ""\xbd""""\xd7""""\xb5""""\xa5""""\xce""""\xbb""""\xbe""""\xd8""""\xd5""""\xf3""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":idn(n)", "#idn(3)", "#idn(3)*[[1,2,3],[4,5,6],[7,8,9]]", CAT_CATEGORY_MATRIX},
  {"iegcd(a,b)", 0, "Find integers u,v,d such that a*u+b*v=d=gcd(a,b)","23,13", 0, CAT_CATEGORY_ARIT},
  {"ifactor(n)", 0, "\x01""\xd5""""\xfb""""\xca""""\xfd""""\xd6""""\xca""""\xd2""""\xf2""""\xca""""\xfd""""\xb7""""\xd6""""\xbd""""\xe2""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":ifactor(n)", "#ifactor(1234)", "#ifactor(2^32+1)", CAT_CATEGORY_ARIT},
  {"ilaplace(f,s,x)", 0, "Inverse Laplace transform of f", "s/(s^2+1),s,x", 0, CAT_CATEGORY_CALCULUS},
  {"im(z)", 0, "\x01""\xb8""""\xb4""""\xca""""\xfd""""\xb5""""\xc4""""\xd0""""\xe9""""\xb2""""\xbf""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":im(z)", "#im(3+4i)", "#im(2-5i)", CAT_CATEGORY_COMPLEXNUM},
  {"incircle(A,B,C)", 0, "Incircle", "-1,2+i,3", 0, CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_2D << 8) | XCAS_ONLY},
  {"inf", "inf", "Plus infinity. -inf for minus infinity and infinity for unsigned/complex infinity. Shortcut shift INS.", "oo", 0, CAT_CATEGORY_CALCULUS},
  {"input()", "input()", "Read a string from keyboard", 0, 0, CAT_CATEGORY_PROG},
  {"integrate(f,x,[a,b])", 0, "\x01""\xc7""""\xf3""""\xbb""""\xfd""""\xb7""""\xd6""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":integrate(f,x) ""\xc7""""\xf3""""\xb2""""\xbb""""\xb6""""\xa8""""\xbb""""\xfd""""\xb7""""\xd6"";integrate(f,x,a,b) ""\xc7""""\xf3""""\xb6""""\xa8""""\xbb""""\xfd""""\xb7""""\xd6""", "#integrate(x*sin(x),x)", "#integrate(1/(1+x^2),x,0,1)", CAT_CATEGORY_CALCULUS},
  {"interp(X,Y)", 0, "Lagrange interpolation at points (xi,yi) where X is the list of xi and Y of yi. If interp is passed as 3rd argument, returns the divided differences list.", "[1,2,3,4,5],[0,1,3,4,4]", "[1,2,3,4,5],[0,1,3,4,4],interp", CAT_CATEGORY_POLYNOMIAL},
  {"inter(A,B)", 0, "Intersections list. Run single_inter if intersection is unique.", "line(y=x),circle(0,1)", 0, CAT_CATEGORY_3D | (CAT_CATEGORY_2D << 8) | XCAS_ONLY},
  {"inv(A)", 0, "\x01""\xbe""""\xd8""""\xd5""""\xf3""""\xc7""""\xf3""""\xc4""""\xe6""(""\xbb""""\xf2""""\xb1""""\xed""""\xb4""""\xef""""\xca""""\xbd""""\xb5""""\xc4""""\xb5""""\xb9""""\xca""""\xfd"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":inv(A)", "#inv([[1,2],[3,4]])", "#inv(2)", CAT_CATEGORY_MATRIX  | (CAT_CATEGORY_LINALG << 8)},
  {"iquo(a,b)", 0, "\x01""\xd5""""\xfb""""\xca""""\xfd""""\xd5""""\xfb""""\xb3""""\xfd""""\xb5""""\xc4""""\xc9""""\xcc""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":iquo(a,b)", "#iquo(23,13)", "#iquo(-23,13)", CAT_CATEGORY_ARIT},
  {"irem(a,b)", 0, "\x01""\xd5""""\xfb""""\xca""""\xfd""""\xd5""""\xfb""""\xb3""""\xfd""""\xb5""""\xc4""""\xd3""""\xe0""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":irem(a,b)", "#irem(23,13)", "#irem(-23,13)", CAT_CATEGORY_ARIT},
  {"isprime(n)", 0, "\x01""\xc5""""\xd0""""\xb6""""\xcf""""\xd5""""\xfb""""\xca""""\xfd""""\xca""""\xc7""""\xb7""""\xf1""""\xce""""\xaa""""\xcb""""\xd8""""\xca""""\xfd"":""\xca""""\xc7""""\xb7""""\xb5""""\xbb""""\xd8"" 1,""\xb7""""\xf1""""\xd4""""\xf2""""\xb7""""\xb5""""\xbb""""\xd8"" 0""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":isprime(n)", "#isprime(1999)", "#isprime(2^61-1)", CAT_CATEGORY_ARIT},
  {"is_collinear(A,B,C)", 0, "Returns 1 if A, B, C are collinear, 0 otherwise", "1,i,-1", "i,0,-i", CAT_CATEGORY_2D | XCAS_ONLY },
  {"is_concyclic(A,B,C,D)", 0, "Returns 1 if A, B, C, D are concyclic, 0 otherwise", "1,i,-1,-i", "1,i,0,-i", CAT_CATEGORY_2D | XCAS_ONLY },
  {"is_element(A,G)", 0, "Returns 1 if A belongs to G, 0 otherwise.", "point(0),circle(0,1)", "point(i),square(0,1)", CAT_CATEGORY_2D | XCAS_ONLY },
  {"is_orthogonal(D,E)", 0, "Returns 1 if D and E are perpendicular, 0 otherwise", "line(y=x),line(y=-x)", "line(y=x),line(y=x+1)", CAT_CATEGORY_2D | XCAS_ONLY },
  {"is_parallel(D,E)", 0, "Returns 1 if D and E are parallel, 0 otherwise", "line(y=x),line(y=-x)", "line(y=x),line(y=x+1)", CAT_CATEGORY_2D | XCAS_ONLY },
  {"jordan(A)", 0, "Jordan normal form of matrix A, returns P and D such that P^-1*A*P=D", "[[1,2],[3,4]]", "[[1,1,-1,2,-1],[2,0,1,-4,-1],[0,1,1,1,1],[0,1,2,0,1],[0,0,-3,3,-1]]", CAT_CATEGORY_MATRIX},
  {"laguerre(n,a,x)", 0, "n-ieme Laguerre polynomial (default a=0).", "10", 0, CAT_CATEGORY_POLYNOMIAL},
  {"laplace(f,x,s)", 0, "Laplace transform of f","sin(x),x,s", 0, CAT_CATEGORY_CALCULUS},
  {"lcm(a,b,...)", 0, "\x01""\xd7""""\xee""""\xd0""""\xa1""""\xb9""""\xab""""\xb1""""\xb6""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":lcm(a,b,...)", "#lcm(23,13)", "#lcm(x^2-1,x^3-1)", CAT_CATEGORY_ARIT | (CAT_CATEGORY_POLYNOMIAL << 8)},
  {"lcoeff(p,x)", 0, "Leading coefficient of polynomial p in x.", "x^4-1", 0, CAT_CATEGORY_POLYNOMIAL},
  {"legendre(n)", 0, "n-the Legendre polynomial.", "10", "10,t", CAT_CATEGORY_POLYNOMIAL},
#ifdef RELEASE
  {"len(l)", 0, "Size of a list.", "[3/2,2,1,1/2,3,2,3/2]", 0, CAT_CATEGORY_LIST},
#endif
  {"leve_crayon ", "leve_crayon ", "Turtle moves without trace.", 0, 0, CAT_CATEGORY_LOGO},
  {"limit(f,x=a)", 0, "\x01""\xc7""""\xf3""""\xbc""""\xab""""\xcf""""\xde""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":limit(f,x=a);""\xb5""""\xa5""""\xb2""""\xe0""""\xbc""""\xab""""\xcf""""\xde"":limit(f,x=a,1) ""\xd3""""\xd2""""\xbc""""\xab""""\xcf""""\xde""""\xa1""""\xa2""limit(f,x=a,-1) ""\xd7""""\xf3""""\xbc""""\xab""""\xcf""""\xde""", "#limit(sin(x)/x,x=0)", "#limit(abs(x)/x,x=0,1)", CAT_CATEGORY_CALCULUS},
  {"line(equation)", 0, "Line of equation", "y=2x+1", "[0,0,0],[1,-2,3]", CAT_CATEGORY_PROGCMD |(CAT_CATEGORY_2D << 8)|(CAT_CATEGORY_2D << 16)},
  {"line_width_", "line_width_", "Width prefix (2 to 8)", 0, 0, CAT_CATEGORY_PROGCMD},
  {"linear_regression(Xlist,Ylist)", 0, "Linear regression.", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"linear_regression_plot(Xlist,Ylist)", 0, "Linear regression plot.", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];linear_regression_plot(X,Y);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
  {"linetan(expr,x,x0)", 0, "Tangent to the graph at x=x0.", "sin(x),x,pi/2", 0, CAT_CATEGORY_PLOT},
  {"linsolve([eq1,eq2,..],[x,y,..])", 0, "\x01""\xbd""""\xe2""""\xcf""""\xdf""""\xd0""""\xd4""""\xb7""""\xbd""""\xb3""""\xcc""""\xd7""""\xe9""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":linsolve([eq1,eq2],[x,y]) ""\xbb""""\xf2"" linsolve(A,b)", "#linsolve([x+y=3,x-y=1],[x,y])", "#linsolve([[1,2],[3,4]],[5,6])", CAT_CATEGORY_SOLVE | (CAT_CATEGORY_LINALG <<8) | (CAT_CATEGORY_MATRIX << 16)},
  {"logarithmic_regression(Xlist,Ylist)", 0, "Logarithmic egression.", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"logarithmic_regression_plot(Xlist,Ylist)", 0, "Logarithmic regression plot.", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];logarithmic_regression_plot(X,Y);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
  {"lu(A)", 0, "LU decomposition LU of matrix A, P*A=L*U", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX},
  {"magenta", "magenta", "Display option", "#display=magenta", 0, CAT_CATEGORY_PROGCMD},
  {"map(f,l)", 0, "\x01""\xb6""""\xd4""""\xc1""""\xd0""""\xb1""""\xed""""\xc3""""\xbf""""\xb8""""\xf6""""\xd4""""\xaa""""\xcb""""\xd8""""\xd3""""\xa6""""\xd3""""\xc3""""\xba""""\xaf""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":map(L,f)", "#map([1,2,3],x->x^2)", "#map([1,2,3],sin)", CAT_CATEGORY_LIST},
  {"matpow(A,n)", 0, "Returns matrix A^n", "[[1,2],[3,4]],n","#assume(n>=1);matpow([[0,2],[0,4]],n)",  CAT_CATEGORY_MATRIX},
  {"matrix(r,c,func)", 0, "Matrix from a defining function.", "2,3,(j,k)->j^k", 0, CAT_CATEGORY_MATRIX},
  {"mean(l)", 0, "\x01""\xcb""""\xe3""""\xca""""\xf5""""\xc6""""\xbd""""\xbe""""\xf9""""\xd6""""\xb5""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":mean(L)", "#mean([3/2,2,1,1/2,3,2,3/2])", "#mean([1,2,3,4,5])", CAT_CATEGORY_STATS},
  {"median(l)", 0, "\x01""\xd6""""\xd0""""\xce""""\xbb""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":median(L)", "#median([3/2,2,1,1/2,3,2,3/2])", "#median([1,2,3,4])", CAT_CATEGORY_STATS},
  {"median_line(A,B,C)", 0, "Median line of triangle ABC from vertex A", "1,i,2+i", 0,CAT_CATEGORY_2D},
  {"midpoint(A,B)", 0, "Midpoint of segment AB", "1,i", 0,CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8)},
  {"montre_tortue ", "montre_tortue ", "Displays the turtle", 0, 0, CAT_CATEGORY_LOGO},
  {"mult_c_conjugate", 0, "Multiplier par le conjugue complexe.", "1+2*i", 0,  (CAT_CATEGORY_COMPLEXNUM << 8)},
  {"mult_conjugate", 0, "Multiplier par le conjugue (sqrt).", "sqrt(2)-sqrt(3)", 0, CAT_CATEGORY_ALGEBRA},
  {"normald([mu,sigma],x)", 0, "Normal distribution probability density, by default mu=0 and sigma=1. normald_cdf([mu,sigma],x) probability that \"normal distribution <=x\" e.g. normald_cdf(1.96). normald_icdf([mu,sigma],t) returns x such that \"normal distribution <=x\" has probability t, e.g. normald_icdf(0.975) ", "1.2", 0, CAT_CATEGORY_PROBA},
  {"not(x)", 0, "Logical not.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"numer(x)", 0, "Numerator of x.", "3/4", 0, CAT_CATEGORY_POLYNOMIAL},
  {"octahedron(A,B,C)", 0, "Octahedron of edge AB with one face in plane ABC", "[0,0,0],[3,0,0],[0,1,0]", 0, CAT_CATEGORY_3D},
  {"odesolve(f(t,y),[t,y],[t0,y0],t1)", 0, "Approx. solution of differential equation y'=f(t,y) and y(t0)=y0, value for t=t1 (add curve to get intermediate values of y)", "sin(t*y),[t,y],[0,1],2", "0..pi,(t,v)->{[-v[1],v[0]]},[0,1]", CAT_CATEGORY_SOLVE},
  {"parabola(F,A)", 0, "Parabola given by focus and vertex", "-2-i,2+i", 0, CAT_CATEGORY_2D},
  {"parameq(object)", 0, "Parametric equations. Run equation for cartesian equation", "circle(0,1)", "ellipse(-1,1,3)", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"partfrac(p,x)", 0, "\x01""\xb2""""\xbf""""\xb7""""\xd6""""\xb7""""\xd6""""\xca""""\xbd""""\xb7""""\xd6""""\xbd""""\xe2""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":partfrac(p,x)", "#partfrac(1/(x^4-1),x)", "#partfrac((x^2+1)/(x-1),x)", CAT_CATEGORY_ALGEBRA},
  {"pas_de_cote n", "pas_de_cote ", "Turtle side jump from n steps, by default n=10", "#pas_de_cote 30", 0, CAT_CATEGORY_LOGO},
  {"perpen_bisector(A,B)", 0, "Perpendicular bisector of segment AB", "1,i", 0,CAT_CATEGORY_2D},
  {"plane(equation)", 0, "Plane given by equation or by 3 points", "z=x+y-1", "[0,0,0],[1,0,0],[0,1,0]", CAT_CATEGORY_3D | XCAS_ONLY},
  {"plot(expr,x)", 0, "\x01""\xbb""""\xe6""""\xd6""""\xc6""""\xba""""\xaf""""\xca""""\xfd""""\xcd""""\xbc""""\xcf""""\xf1""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":plot(f) ""\xbb""""\xf2"" plot(f,x=a..b)", "#plot(sin(x))", "#plot(ln(x),x=0.5..5)", (CAT_CATEGORY_PLOT << 8) | (CAT_CATEGORY_3D)},
#ifdef RELEASE
  {"plotarea(expr,x=a..b,[n,meth])", 0, "Area under curve with specified quadrature.", "1/x,x=1..3,2,trapezoid", 0, CAT_CATEGORY_PLOT},
#endif
  {"plotcontour(expr,[x=xm..xM,y=ym..yM],levels)", 0, "Levels of expr.", "x^2+2y^2,[x=-2..2,y=-2..2],[1,2]", 0, CAT_CATEGORY_PLOT},
  {"plotfield(f(t,y),[t=tmin..tmax,y=ymin..ymax])", 0, "Plot field of differential equation y'=f(t,y), an optionally one solution by adding plotode=[t0,y0]", "sin(t*y),[t=-3..3,y=-3..3],plotode=[0,1]", 0, CAT_CATEGORY_PLOT},
  {"plotfunc(expr,[x,y])", 0, "\x01""\xbb""""\xe6""""\xd6""""\xc6""""\xba""""\xaf""""\xca""""\xfd""""\xcd""""\xbc""""\xcf""""\xf1""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":plotfunc(f,x)", "#plotfunc(sin(x),x)", "#plotfunc(x^2,x=-2..2)", CAT_CATEGORY_PLOT | (CAT_CATEGORY_3D << 8) | XCAS_ONLY },
  {"plotlist(list)", 0, "Plot a list", "[3/2,2,1,1/2,3,2,3/2]", 0, CAT_CATEGORY_PLOT},
  {"plotode(f(t,y),[t=tmin..tmax,y],[t0,y0])", 0, "Plot solution of differential equation y'=f(t,y), y(t0)=y0.", "sin(t*y),[t=-3..3,y],[0,1]", 0, CAT_CATEGORY_PLOT},
  {"plotparam([x,y],t)", 0, "\x01""\xbb""""\xe6""""\xd6""""\xc6""""\xb2""""\xce""""\xca""""\xfd""""\xc7""""\xfa""""\xcf""""\xdf""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":plotparam([x(t),y(t)],t)", "#plotparam([cos(t),sin(t)],t)", "#plotparam([t,t^2],t=-2..2)", CAT_CATEGORY_PLOT},
  {"plotpolar(r,theta)", 0, "Polar plot.","cos(3*x),x,0,pi", "1/(1+cos(x)),x=0..pi,tstep=0.05", CAT_CATEGORY_PLOT},
  {"plotseq(f(x),x=[u0,m,M],n)", 0, "Plot f(x) on [m,M] and n terms of the sequence defined by u_{n+1}=f(u_n) and u0.","sqrt(2+x),x=[6,0,7],5", 0, CAT_CATEGORY_PLOT},
  {"plus_point", "plus_point", "Display option", "#display=blue+plus_point", 0, CAT_CATEGORY_PROGCMD},
  {"point(x,y[,z])", 0, "Point", "1,2", "1,2,3", CAT_CATEGORY_PLOT | (CAT_CATEGORY_2D << 8)},
  {"polygon(list)", 0, "Closed polygon given by a list of vertices.", "1-i,2+i,3,3-2i", 0, CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_2D << 8) },
  {"polygonscatterplot(Xlist,Ylist)", 0, "Plot points and polygonal line.", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"polyhedron(A,B,C,D,...)", 0, "Convex polyhedron of vertices in A,B,C,D,...", "[0,0,0],[0,5,0],[0,0,5],[1,2,6]", 0, CAT_CATEGORY_3D},
  {"polynomial_regression(Xlist,Ylist,n)", 0, "Polynomial regression, degree <= n.", "[1,2,3,4,5],[0,1,3,4,4],2", 0, CAT_CATEGORY_STATS},
  {"polynomial_regression_plot(Xlist,Ylist,n)", 0, "Polynomial regression plot, degree <= n.", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];polynomial_regression_plot(X,Y,2);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
  //{"pour", "pour j de 1 jusque  faire  fpour;", "For loop.","#pour j de 1 jusque 10 faire print(j,j^2); fpour;", 0, CAT_CATEGORY_PROG},
  {"power_regression(Xlist,Ylist,n)", 0, "Power regression.", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"power_regression_plot(Xlist,Ylist,n)", 0, "Power regression graph", "#X,Y:=[1,2,3,4,5],[0,1,3,4,4];power_regression_plot(X,Y);scatterplot(X,Y)", 0, CAT_CATEGORY_STATS},
  {"powmod(a,n,p)", 0, "Returns a^n mod p.","123,456,789", 0, CAT_CATEGORY_ARIT},
  {"print(expr)", 0, "Print expr in console", 0, 0, CAT_CATEGORY_PROG},
  {"projection(obj1,obj2)", 0, "Projection on obj1 of obj2", "line(y=x),point(2,3)", 0, CAT_CATEGORY_2D },
  {"proot(p)", 0, "\x01""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xca""""\xfd""""\xd6""""\xb5""""\xc7""""\xf3""""\xb8""""\xf9""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":proot(P)", "#proot(x^3-1)", "#proot([1,0,0,-1])", CAT_CATEGORY_POLYNOMIAL},
  {"purge(x)", 0, "Clear assigned variable x. Shortcut SHIFT-FORMAT", 0, 0, CAT_CATEGORY_PROGCMD|(CAT_CATEGORY_SOFUS<<8)},
  {"python(f)", 0, "Displays f in Python syntax.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"python_compat(0|1|2)", 0, "python_compat(0) Xcas syntax, python_compat(1) Python syntax with ^ interpreted as power, python_compat(2) ^ as bit xor", "0", "1", CAT_CATEGORY_PROG},
  {"q2a(expr,vars)", 0, "Matrix of a quadratic form", "x^2+3*x*y","x^2+3*x*y,[x,y]", CAT_CATEGORY_LINALG},
  {"qr(A)", 0, "A=Q*R factorization with Q orthogonal and R upper triangular", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX},
  {"quadric(equation)", 0, "Quadric given by equation (or 9 points)", "x^2-y^2+z^2", "x^2+x*y+y^2+z^2-3", CAT_CATEGORY_3D},
  {"quartile1(l)", 0, "1st quartile", "[3/2,2,1,1/2,3,2,3/2]", 0, CAT_CATEGORY_STATS},
  {"quartile3(l)", 0, "3rd quartile", "[3/2,2,1,1/2,3,2,3/2]", 0, CAT_CATEGORY_STATS},
  {"quo(p,q,x)", 0, "\x01""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xd5""""\xfb""""\xb3""""\xfd""""\xb5""""\xc4""""\xc9""""\xcc""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":quo(p,q,x);""\xd5""""\xfb""""\xca""""\xfd""""\xb5""""\xc4""""\xc9""""\xcc""""\xd3""""\xc3"" iquo(a,b)", "#quo(x^3-1,x-1,x)", "#iquo(23,13)", CAT_CATEGORY_POLYNOMIAL},
  {"quote(x)", 0, "Returns expression x unevaluated.", 0, 0, CAT_CATEGORY_ALGEBRA},
  {"radius(objet)", 0, "Radius of a circle or sphere", "circle(0,1)", "sphere([0,0,0],[1,1,1])", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"rand()", "rand()", "\x01""\xc9""""\xfa""""\xb3""""\xc9"" 0 ""\xb5""""\xbd"" 1 ""\xd6""""\xae""""\xbc""""\xe4""""\xb5""""\xc4""""\xcb""""\xe6""""\xbb""""\xfa""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":rand()", "#rand()", "#1+100*rand()", CAT_CATEGORY_PROBA},
  {"randint(a,b)", 0, "Random integer between a and b. With 1 argument in Xcas, random integer between 1 and n.", "5,25", "6", CAT_CATEGORY_PROBA},
  {"ranm(n,m,[loi,parametres])", 0, "Random matrix with integer coefficients or according to a probability law (ranv for a vector). Examples ranm(2,3), ranm(3,2,binomial,20,.3), ranm(4,2,normald,0,1)", "3,3","4,2,normald,0,1",  CAT_CATEGORY_MATRIX},
  {"ranv(n,[loi,parametres])", 0, "Random vector.", "10","4,normald,0,1", CAT_CATEGORY_LINALG},
  {"ratnormal(x)", 0, "Puts everything over a common denominator.", 0, 0, CAT_CATEGORY_ALGEBRA},
  {"re(z)", 0, "\x01""\xb8""""\xb4""""\xca""""\xfd""""\xb5""""\xc4""""\xca""""\xb5""""\xb2""""\xbf""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":re(z)", "#re(3+4i)", "#re(2-5i)", CAT_CATEGORY_COMPLEXNUM},
  {"read(\"filename\")", "read(\"", "Read a file.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"rectangle_plein a,b", "rectangle_plein ", "Direct filled rectangle from turtle position, if b is omitted b==a", "#rectangle_plein 30","#rectangle_plein 20,40", CAT_CATEGORY_LOGO},
  {"recule n", "recule ", "Turtle backward n steps, n=10 by default", "#recule 30", 0, CAT_CATEGORY_LOGO},
  {"red", "red", "Display option", "#display=red", 0, CAT_CATEGORY_PROGCMD},
  {"reflection(obj1,obj2)", 0, "Reflection or symmetrical of obj2", "line(y=x),cercle(1,1)", 0, CAT_CATEGORY_2D },
  {"rem(p,q,x)", 0, "\x01""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""""\xd5""""\xfb""""\xb3""""\xfd""""\xb5""""\xc4""""\xd3""""\xe0""""\xca""""\xfd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":rem(p,q,x);""\xd5""""\xfb""""\xca""""\xfd""""\xb5""""\xc4""""\xd3""""\xe0""""\xca""""\xfd""""\xd3""""\xc3"" irem(a,b)", "#rem(x^3-1,x-1,x)", "#irem(23,13)", CAT_CATEGORY_POLYNOMIAL},
#ifdef RELEASE
  {"residue(f(z),z,z0)", 0, "Residue of an expression at z0.", "1/(x^2+1),x,i", 0, CAT_CATEGORY_COMPLEXNUM},
#endif
  {"resultant(p,q,x)", 0, "Resultant in x of polynomials p and q.", "#P:=x^3+p*x+q;resultant(P,P',x);", 0, CAT_CATEGORY_POLYNOMIAL},
  {"revert(p[,x])", 0, "Revert Taylor series","x+x^2+x^4", 0, CAT_CATEGORY_CALCULUS},
  {"rgb(r,g,b)", 0, "color defined from red, green, blue from 0 to 255", "255,0,255", 0, CAT_CATEGORY_PROGCMD},
  {"rhombus_point", "rhombus_point", "Display option", "#display=magenta+rhombus_point", 0, CAT_CATEGORY_PROGCMD},
  {"rond n", "rond ", "Circle tangent to the turtle, radius n. Run rond n,theta for an arc of circle of theta degrees", 0, 0, CAT_CATEGORY_LOGO},
  {"rotation(center,angle,objcet)", 0, "Image of object by rotation", "2-i,pi/2,circle(0,1)", "sphere([0,0,0],[1,1,1])", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  {"rref(A)", 0, "\x01""\xbe""""\xd8""""\xd5""""\xf3""""\xb5""""\xc4""""\xd0""""\xd0""""\xbc""""\xf2""""\xbb""""\xaf""""\xbd""""\xd7""""\xcc""""\xdd""""\xd0""""\xce""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":rref(A)", "#rref([[1,2],[3,4]])", "#rref([[1,2,3],[4,5,6]])", CAT_CATEGORY_MATRIX|  (CAT_CATEGORY_LINALG<<8)},
  {"rsolve(equation,u(n),[init])", 0, "Solve a recurrence relation.","u(n+1)=2*u(n)+3,u(n),u(0)=1", "([u(n+1)=3*v(n)+u(n),v(n+1)=v(n)+u(n)],[u(n),v(n)],[u(0)=1,v(0)=2]", CAT_CATEGORY_SOLVE},
  {"saute n", "saute ", "Turtle jumps n steps, by default n=10", "#saute 30", 0, CAT_CATEGORY_LOGO},
  {"scatterplot(Xlist,Ylist)", 0, "Draws points", "[1,2,3,4,5],[0,1,3,4,4]", 0, CAT_CATEGORY_STATS},
  {"segment(A,B)", 0, "Segment", "1,2+i", "[1,2,1],[-1,3,2]", CAT_CATEGORY_PROGCMD | (CAT_CATEGORY_2D << 8) | XCAS_ONLY},
  {"seq(expr,var,a,b)", 0, "\x01""\xc9""""\xfa""""\xb3""""\xc9""""\xc1""""\xd0""""\xb1""""\xed""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":seq(f,k,a,b) ""\xbb""""\xf2"" seq(f,k,a,b,""\xb2""""\xbd""""\xb3""""\xa4"")", "#seq(j^2,j,1,5)", "#seq(j^2,j,1,10,2)", CAT_CATEGORY_PROGCMD},
  //{"si", "si  alors  sinon  fsi;", "Test.", "#f(x):=si x>0 alors x; sinon -x; fsi;// valeur absolue", 0, CAT_CATEGORY_PROG},
  {"sign(x)", 0, "\x01""\xb7""""\xfb""""\xba""""\xc5""""\xba""""\xaf""""\xca""""\xfd"":""\xd5""""\xfd""""\xca""""\xfd""""\xb7""""\xb5""""\xbb""""\xd8"" 1,""\xb8""""\xba""""\xca""""\xfd""""\xb7""""\xb5""""\xbb""""\xd8"" -1,0 ""\xb7""""\xb5""""\xbb""""\xd8"" 0""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":sign(x)", "#sign(-3)", "#sign(2)", CAT_CATEGORY_REAL|XCAS_ONLY},
  {"similarity(center,ratio,angle,object)", 0, "Image of object by similarity", "0,2,pi/2,circle(1,1)", 0, CAT_CATEGORY_2D },
  {"simplify(expr)", 0, "\x01""\xbb""""\xaf""""\xbc""""\xf2""""\xb1""""\xed""""\xb4""""\xef""""\xca""""\xbd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":simplify(expr)", "#simplify(sin(3x)/sin(x))", "#simplify((x^2-1)/(x-1))", CAT_CATEGORY_ALGEBRA},
  {"single_inter(A,B)", 0, "First intersection. Run inter for a list of intersections.", "line(y=x),line(x+y=3)", 0, CAT_CATEGORY_3D | (CAT_CATEGORY_2D << 8) | XCAS_ONLY},
  {"solve(equation,x)", 0, "\x01""\xbd""""\xe2""""\xb7""""\xbd""""\xb3""""\xcc""(""\xbe""""\xab""""\xc8""""\xb7""""\xbd""""\xe2"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":solve(""\xb7""""\xbd""""\xb3""""\xcc"",x);""\xb7""""\xbd""""\xb3""""\xcc""""\xd7""""\xe9"":solve([eq1,eq2],[x,y])", "#solve(x^2-3x+2=0,x)", "#solve([x+y=3,x-y=1],[x,y])", CAT_CATEGORY_SOLVE},
  {"sorted(l)", 0, "Sorts a list.","[3/2,2,1,1/2,3,2,3/2]", "[[1,2],[2,3],[4,3]],(x,y)->when(x[1]==y[1],x[0]>y[0],x[1]>y[1]", CAT_CATEGORY_LIST},
  {"sphere(A,r)", 0, "Sphere of center A and radius r or diameter AB", "[0,0,0],1", "[0,0,0],[1,1,1]", CAT_CATEGORY_3D},
  {"square_point", "square_point", "Display option", "#display=cyan+square_point", 0, CAT_CATEGORY_PROGCMD},
  {"star_point", "star_point", "Display option", "#display=magenta+star_point", 0, CAT_CATEGORY_PROGCMD},
  {"stddev(l)", 0, "\x01""\xb1""""\xea""""\xd7""""\xbc""""\xb2""""\xee""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":stddev(L)", "#stddev([1,2,3,4,5])", "#stddev([3/2,2,1,1/2,3,2,3/2])", CAT_CATEGORY_STATS},
  {"subst(a,b=c)", 0, "\x01""\xb4""""\xfa""""\xc8""""\xeb""""\xcc""""\xe6""""\xbb""""\xbb"":""\xb0""""\xd1"" a ""\xd6""""\xd0""""\xb5""""\xc4"" b ""\xbb""""\xbb""""\xb3""""\xc9"" c""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":subst(a,b=c),""\xd2""""\xb2""""\xbf""""\xc9""""\xbc""""\xf2""""\xd0""""\xb4"" a(b=c)", "#subst(x^2+1,x=3)", "#sin(x)(x=pi/2)", CAT_CATEGORY_ALGEBRA},
  {"sum(f,k,m,M)", 0, "\x01""\xc7""""\xf3""""\xba""""\xcd""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":sum(f,k,m,M) ""\xb1""""\xed""""\xca""""\xbe"" k ""\xb4""""\xd3"" m ""\xb5""""\xbd"" M ""\xb6""""\xd4"" f ""\xc7""""\xf3""""\xba""""\xcd""", "#sum(k^2,k,1,10)", "#sum(1/k^2,k,1,inf)", CAT_CATEGORY_CALCULUS},
  {"svd(A)", 0, "Singular Value Decomposition, returns U orthogonal, S vector of singular values, Q orthogonal such that A=U*diag(S)*tran(Q).", "[[1,2],[3,4]]", 0, CAT_CATEGORY_MATRIX},
  {"tabvar(f,[x=a..b])", 0, "Table of variations of expression f, optional arguments variable x in interval a..b", "sqrt(x^2+x+1)",  "[cos(t),sin(3t)],t", CAT_CATEGORY_CALCULUS},
  //{"tantque", "tantque  faire   ftantque;", "While loop.", "#j:=13; tantque j!=1 faire j:=when(even(j),j/2,3j+1); print(j); ftantque;", 0, CAT_CATEGORY_PROG},
  {"taylor(f,x=a,n,[polynom])", 0, "\x01""\xcc""""\xa9""""\xc0""""\xd5""""\xd5""""\xb9""""\xbf""""\xaa""""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":taylor(f,x=a,n);""\xbc""""\xd3"" polynom ""\xb2""""\xce""""\xca""""\xfd""""\xbf""""\xc9""""\xc8""""\xa5""""\xb5""""\xf4""""\xd3""""\xe0""""\xcf""""\xee""", "#taylor(sin(x),x=0,5)", "#taylor(sin(x),x=0,5,polynom)", CAT_CATEGORY_CALCULUS},
  {"tchebyshev1(n)", 0, "Tchebyshev polynomial 1st kind: cos(n*x)=T_n(cos(x))", "10", 0, CAT_CATEGORY_POLYNOMIAL},
  {"tchebyshev2(n)", 0, "Tchebyshev polynomial 2nd kind: sin((n+1)*x)=sin(x)*U_n(cos(x))", "10", 0, CAT_CATEGORY_POLYNOMIAL},
  {"tcollect(expr)", 0, "Linearize and collect trig functions.","sin(x)+cos(x)", 0, CAT_CATEGORY_TRIG},
  {"texpand(expr)", 0, "Expand trigonometric, exp and ln functions.","sin(3x)", 0, CAT_CATEGORY_TRIG},
  {"time(cmd)", 0, "Time to run a command or set the clock","int(1/(x^4+1),x)","8,0", CAT_CATEGORY_PROG},
  {"tlin(expr)", 0, "Trigonometric linearization of expr.","sin(x)^3", 0, CAT_CATEGORY_TRIG},
  {"tourne_droite n", "tourne_droite ", "Turtle turns right n degrees, n=90 by default", 0, 0, CAT_CATEGORY_LOGO},
  {"tourne_gauche n", "tourne_gauche ", "Turtle turns left n degrees, n=90 by default", 0, 0, CAT_CATEGORY_LOGO},
  {"trace(A)", 0, "\x01""\xbe""""\xd8""""\xd5""""\xf3""""\xb5""""\xc4""""\xbc""""\xa3""(""\xd6""""\xf7""""\xb6""""\xd4""""\xbd""""\xc7""""\xcf""""\xdf""""\xd4""""\xaa""""\xcb""""\xd8""""\xd6""""\xae""""\xba""""\xcd"")""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":trace(A)", "#trace([[1,2],[3,4]])", "#trace(idn(3))", CAT_CATEGORY_MATRIX},
  {"transpose(A)", 0, "\x01""\xbe""""\xd8""""\xd5""""\xf3""""\xd7""""\xaa""""\xd6""""\xc3""(""\xb9""""\xb2""""\xe9""""\xee""""\xd7""""\xaa""""\xd6""""\xc3""""\xd3""""\xc3"" trn(A))""\xa1""""\xa3""""\xd3""""\xc3""""\xb7""""\xa8"":transpose(A)", "#transpose([[1,2],[3,4]])", "#transpose([[1,2,3]])", CAT_CATEGORY_MATRIX |  (CAT_CATEGORY_LINALG<<8)},
  {"translation(vect,obj)", 0, "Translate by vect obj", "[1,2],cercle(0,1)", 0, CAT_CATEGORY_2D },
  {"triangle(A,B,C)", 0, "Triangle given by 3 vertices", "1+i,1-i,-1", "A,B,C", CAT_CATEGORY_2D},
  {"triangle_point", "triangle_point", "Display option", "#display=yellow+triangle_point", 0, CAT_CATEGORY_PROGCMD},
  {"trig2exp(expr)", 0, "Convert complex exponentials to trigonometric functions","cos(x)^3", 0, CAT_CATEGORY_TRIG},
  {"trigcos(expr)", 0, "Convert sin^2 and tan^2 to cos^2.","sin(x)^4", 0, CAT_CATEGORY_TRIG},
  {"trigsin(expr)", 0, "Convert cos^2 and tan^2 to sin^2.","cos(x)^4", 0, CAT_CATEGORY_TRIG},
  {"trigtan(expr)", 0, "Convert cos^2 and sin^2 to tan^2.","cos(x)^4", 0, CAT_CATEGORY_TRIG},
  {"uniformd(a,b,x)", "uniformd", "uniform law on [a,b] of density 1/(b-a)", 0, 0, CAT_CATEGORY_PROBA},
  {"vertices(objet)", 0, "List of vertices of a polygon or polyhedra", "triangle(1,i,2)", "cube([0,0,0],[1,0,0],[0,1,0])", CAT_CATEGORY_2D | (CAT_CATEGORY_3D << 8) },
  //{"version", "version()", "Khicas 1.5.0, (c) B. Parisse et al. www-fourier.ujf-grenoble.fr/~parisse\nLicense GPL version 2. Interface adapted from Eigenmath for Casio, G. Maia, http://gbl08ma.com. Do not use if CAS calculators are forbidden.", 0, 0, CAT_CATEGORY_PROGCMD},
  {"write(\"filename\",var)", "write(\"", "Save 1 or more variables in a file. For example f(x):=x^2; write(\"func_f\",f).",  0, 0, CAT_CATEGORY_PROGCMD},
  {"yellow", "yellow", "Display option", "#display=yellow", 0, CAT_CATEGORY_PROGCMD},
  {"|", "|", "Logical or", "#1|2", 0, CAT_CATEGORY_PROGCMD},
  {"~", "~", "Complement", "#~7", 0, CAT_CATEGORY_PROGCMD},
};

const char chk_restart_string1[]="Keep variables?";
const char chk_restart_string2[]="F1: keep,   F6: erase";
const char aide_khicas_string[]="Khicas Help";
const char main_string1[]="Clear variables?";
const char main_string2[]="F1: cancel,  F6: confirm";
const char shortcuts_string[]="To set the system clock, run hh,mm=>, for example 13,45=>,\nKeyboard shortcuts (shell and editor)\nF1-F3: according to the legends\nF4: catalog\nF5: lowercase lock or switch lowercase/uppercase\nF6: file menu\n(-): _\nshift-OPTN: programming commands including pixelised graphs\nshift-PRGM: programming characters\nshift-FRAC: plot commands\nOPTN: options\nshift-QUIT: turtle\nshift-Lst: list editor or commands\nshift-Mtr: matrix editor or commands\nVARS: list of variables (shell) or turtle picture (editor)\nshift-FORMAT: purge\n=>+: partfrac\n=>*: factor\n=>sin/cos/tan\n=>=>: solve\n\n*** Shell ***:\ndownkey: completion/help, shift-SETUP: configuration\nF3: 2-d expression editor or graphic view or text editor\nalpha-F3: text editor\n\n*** Expression editor ***\npad: move selection inside expression tree\nshift-left/right exchange selection with right or left argument\nALPHA-left/right inside a sum or product: increase selection adding left or right argument\nF3: Edit selection, shift-F3: increase fontsize, ALPHA-F3: decrease fontsize\nF4: catalog\nF5: lower/uppercase\nF6: Eval selection, shift-F6: approx value, ALPHA-F6: regroup command\nDEL: suppress root operator in selection\n\n*** Script editor: ***\nfraction key (G): indent, shift-fraction: help/completion, shift-CLIP: begins selection, move cursor to the end then DEL to remove or shift-CLIP to copy to clipboard. shift-PASTE to paste.\nF6-6 Search only: enter word then EXE then EXIT. Type EXE for next occurence, AC to cancel.\nF6-6: Replace: enter word then EXE then replacement then EXE. Type EXE or EXIT to replace or skip replacement and go to the next occurence, AC to cancel.\nshift-Ans: check syntax\n\n*** Graphs: ***\n+ - zoom\n(-): zoomout along y\n*: autoscale\n/: orthonormalize\nOPTN: axes on/off";
const char apropos_string[]="\x01Khicas 1.8.0 \xd6\xd0\xce\xc4\xb0\xe6 (c) 2024 B. Parisse et al.\n\x01\xd3\xc9 EricStoneChina \xb9\xb9\xbd\xa8 (2026)\n\x01www-fourier.univ-grenoble-alpes.fr/~parisse\n\x01License GPL version 2.\n\x01Interface adapted from Eigenmath for Casio, by G. Maia, http://gbl08ma.com, Mike Smith, Nemhardy, LePhenixNoir\n\x01Special thanks to LePhenixNoir, planet-casio and tiplanet\n\x01\n\x01Do not use if CAS calculators are forbidden!";

int CAT_COMPLETE_COUNT=sizeof(completeCat)/sizeof(catalogFunc);

ustl::string insert_string(int index){
  ustl::string s;
  if (completeCat[index].insert)
    s=completeCat[index].insert;
  else {
    s=completeCat[index].name;
    int pos=s.find('(');
    if (pos>=0 && pos<s.size())
      s=s.substr(0,pos+1);
  }
  return s;//s+' ';
}

int showCatalog(char* insertText,int preselect,int menupos) {
  // returns 0 on failure (user exit) and 1 on success (user chose a option)
  MenuItem menuitems[CAT_CATEGORY_LOGO+1];
    menuitems[CAT_CATEGORY_ALL].text = (char*)"\x01""\xc8""""\xab""""\xb2""""\xbf""";
    menuitems[CAT_CATEGORY_ALGEBRA].text = (char*)"\x01""\xb4""""\xfa""""\xca""""\xfd""";
    menuitems[CAT_CATEGORY_LINALG].text = (char*)"\x01""\xcf""""\xdf""""\xd0""""\xd4""""\xb4""""\xfa""""\xca""""\xfd""";
    menuitems[CAT_CATEGORY_CALCULUS].text = (char*)"\x01""\xce""""\xa2""""\xbb""""\xfd""""\xb7""""\xd6""";
    menuitems[CAT_CATEGORY_ARIT].text = (char*)"\x01""\xcb""""\xe3""""\xca""""\xf5""/""\xc3""""\xdc""""\xc2""""\xeb""";
    menuitems[CAT_CATEGORY_COMPLEXNUM].text = (char*)"\x01""\xb8""""\xb4""""\xca""""\xfd""";
    menuitems[CAT_CATEGORY_PLOT].text = (char*)"\x01""\xc7""""\xfa""""\xcf""""\xdf""";
    menuitems[CAT_CATEGORY_POLYNOMIAL].text = (char*)"\x01""\xb6""""\xe0""""\xcf""""\xee""""\xca""""\xbd""";
    menuitems[CAT_CATEGORY_PROBA].text = (char*)"\x01""\xb8""""\xc5""""\xc2""""\xca""";
    menuitems[CAT_CATEGORY_PROGCMD].text = (char*)"\x01""\xb3""""\xcc""""\xd0""""\xf2""""\xc3""""\xfc""""\xc1""""\xee"" (0)";
    menuitems[CAT_CATEGORY_REAL].text = (char*)"\x01""\xca""""\xb5""""\xca""""\xfd"" (x,""\xa6""""\xc8"",t)";
    menuitems[CAT_CATEGORY_SOLVE].text = (char*)"\x01""\xbd""""\xe2""""\xb7""""\xbd""""\xb3""""\xcc"" (log)";
    menuitems[CAT_CATEGORY_STATS].text = (char*)"\x01""\xcd""""\xb3""""\xbc""""\xc6"" (ln)";
    menuitems[CAT_CATEGORY_TRIG].text = (char*)"\x01""\xc8""""\xfd""""\xbd""""\xc7"" (sin)";
    menuitems[CAT_CATEGORY_OPTIONS].text = (char*)"\x01""\xd1""""\xa1""""\xcf""""\xee"" (cos)";
    menuitems[CAT_CATEGORY_LIST].text = (char*)"\x01""\xc1""""\xd0""""\xb1""""\xed"" (tan)";
    menuitems[CAT_CATEGORY_MATRIX].text = (char*)"\x01""\xbe""""\xd8""""\xd5""""\xf3""";
    menuitems[CAT_CATEGORY_PROG].text = (char*)"\x01""\xb3""""\xcc""""\xd0""""\xf2"" (S<>D)";
    menuitems[CAT_CATEGORY_SOFUS].text = (char*)"\x01""\xd0""""\xde""""\xb8""""\xc4""""\xb1""""\xe4""""\xc1""""\xbf"" ((";
    menuitems[CAT_CATEGORY_PHYS].text = (char*)"\x01""\xce""""\xef""""\xc0""""\xed""""\xb3""""\xa3""""\xc1""""\xbf"" ())";
    menuitems[CAT_CATEGORY_UNIT].text = (char*)"\x01""\xce""""\xef""""\xc0""""\xed""""\xb5""""\xa5""""\xce""""\xbb"" (,)";
    menuitems[CAT_CATEGORY_2D].text = (char*)"\x01""\xbc""""\xb8""""\xba""""\xce"" (->)";
    menuitems[CAT_CATEGORY_3D].text = (char*)"\x013D (*)";
    menuitems[CAT_CATEGORY_LOGO].text = (char*)"\x01""\xba""""\xa3""""\xb9""""\xea""""\xbb""""\xe6""""\xcd""""\xbc"" (/)";
  
  Menu menu;
  menu.items=menuitems;
  menu.numitems=sizeof(menuitems)/sizeof(MenuItem);
  menu.scrollout=1;
  menu.title = (char*)"Function Catalog";
  
  while(1) {
    if (preselect)
      menu.selection=preselect;
    else {
      if (menupos>0)
	menu.selection=menupos;
      int sres = doMenu(&menu);
      if (sres != MENU_RETURN_SELECTION)
	return 0;
    }
    // puts("catalog 3");
    if(doCatalogMenu(insertText,(char *) menuitems[menu.selection-1].text, menu.selection-1)) {
      const char * ptr=0;
      if (strcmp("matrix ",insertText)==0 && (ptr=input_matrix(false)) )
	return 0;
      if (strcmp("list ",insertText)==0 && (ptr=input_matrix(true)) )
	return 0;
      return 1;
    }
    if (preselect)
      return 0;
  }
  return 0;
}

int find_category(const char *cmdname){
  int l=strlen(cmdname);
  for (int i=0;i<CAT_COMPLETE_COUNT;++i){
    if (!strncmp(cmdname,completeCat[i].name,l))
      return completeCat[i].category;
  }
  return -1;
}

  string remove_accents(const string & s){
    string r;
    for (int i=0;i<s.size();++i){
      unsigned char ch=s[i];
      if (ch==195 && i<s.size()-1){
	++i;
	switch ((unsigned char)s[i]){
	case 160: case 161: case 162:
	  r+='a';
	  continue;
	case 168: case 169: case 170:
	  r+='e';
	  continue;
	case 172: case 173: case 174:
	  r+='i';
	  continue;
	case 178: case 179: case 180:
	  r += 'o';
	  continue;
	case 185: case 186: case 187:
	  r+='u';
	  continue;
	}
	r += '?';
	continue;
      }
      r+=(char)ch;
    }
    return r;
  }

string extract_example(const string & s,int n){
  int l=s.size(),j=0;
  string res;
  for (int i=0;i<l;++i){
    if (s[i]==';'){
      if (j==n)
	break;
      ++j;
      continue;
    }
    if (j==n)
      res+=s[i];
  }
  return res;
}

ustl::string longhelp(const char * s);
int QRdisp(const char * text,const char *msg1,const char * msg2,const char * msg3,const char * msg4);

int doCatalogMenu(char* insertText, char* title, int category,const char * cmdname) {
  int allcmds=builtin_lexer_functions_end()-builtin_lexer_functions_begin();
  int allopts=lexer_tab_int_values_end-lexer_tab_int_values_begin;
  bool isall=category==CAT_CATEGORY_ALL;
  bool isopt=category==CAT_CATEGORY_OPTIONS;
  int nitems = isall? allcmds:(isopt?allopts:CAT_COMPLETE_COUNT);
  MenuItem* menuitems = (MenuItem*)alloca(sizeof(MenuItem)*nitems);
  int cur = 0,curmi = 0,i=0,menusel=-1,cmdl=cmdname?strlen(cmdname):0;
  gen g;
  while(cur<nitems) {
    MenuItem & curmenuitem= menuitems[curmi];
    curmenuitem.type = MENUITEM_NORMAL;
    curmenuitem.color = TEXT_COLOR_BLACK;    
    if (isall || isopt) {
      const char * text=isall?(builtin_lexer_functions_begin()+curmi)->first:(lexer_tab_int_values_begin+curmi)->keyword;
      if (menusel<0 && cmdname && !strncmp(cmdname,text,cmdl))
	menusel=curmi;
      curmenuitem.text = text;
      curmenuitem.isfolder = allcmds; // assumes allcmds>allopts
      curmenuitem.token=isall?((builtin_lexer_functions_begin()+curmi)->second.subtype+256):((lexer_tab_int_values_begin+curmi)->subtype+(lexer_tab_int_values_begin+curmi)->return_value*256);
      // curmenuitem.token=isall?find_or_make_symbol(text,g,0,false,contextptr):((lexer_tab_int_values_begin+curmi)->subtype+(lexer_tab_int_values_begin+curmi)->return_value*256);
      for (;i<CAT_COMPLETE_COUNT;++i){
	const char * catname=completeCat[i].name;
	int tmp=strcmp(catname,text);
	if (tmp>=0){
	  size_t st=strlen(text),j=tmp?0:st;
	  for (;j<st;++j){
	    if (catname[j]!=text[j])
	      break;
	  }
	  if (j==st && (!isalphanum(catname[j]))){
	    curmenuitem.isfolder = i;
	    ++i;
	  }
	  break;
	}
      }
      // compare text with completeCat
      ++curmi;
    }
    else {
      int cat=completeCat[cur].category;
      if ( (xcas_python_eval==0 || cat>0) &&
	   ((cat & 0xff) == category ||
	    (cat & 0xff00) == (category<<8) ||
	    (cat & 0xff0000) == (category <<16)
	    )
	   ){
	curmenuitem.isfolder = cur; // little hack: store index of the command in the full list in the isfolder property (unused by the menu system in this case)
	curmenuitem.text = completeCat[cur].name;
	curmi++;
      }
    }
    cur++;
  }
  if (curmi==0)
    return 0;
  Menu menu;
  if (menusel>=0)
    menu.selection=menusel+1;
  menu.items=menuitems;
  menu.numitems=curmi;
  if (isopt){ menu.selection=5; menu.scroll=4; }
  if (curmi>=100)
    SetSetupSetting( (unsigned int)0x14, 0x88);	
  DisplayStatusArea();
  menu.scrollout=1;
  menu.title = title;
  menu.type = MENUTYPE_FKEYS;
  menu.height = 7;
  while(1) {
    int fkeyw=LCD_WIDTH_PX/6;
    drawRectangle(0,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    Bdisp_MMPrint(0,LCD_HEIGHT_PX-STATUS_AREA_PX-19,"  INPUT",0,0xffffffff,0,0,COLOR_WHITE,COLOR_BLACK,1,0);
    drawRectangle(fkeyw,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    Bdisp_MMPrint(fkeyw,LCD_HEIGHT_PX-STATUS_AREA_PX-19," EXAMPL1",0,0xffffffff,0,0,COLOR_WHITE,COLOR_BLACK,1,0);
    drawRectangle(2*fkeyw,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    Bdisp_MMPrint(2*fkeyw,LCD_HEIGHT_PX-STATUS_AREA_PX-19," EXAMPL2",0,0xffffffff,0,0,COLOR_WHITE,COLOR_BLACK,1,0);
    drawRectangle(3*fkeyw,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    drawRectangle(4*fkeyw,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    Bdisp_MMPrint(4*fkeyw,LCD_HEIGHT_PX-STATUS_AREA_PX-19,"  HELP",0,0xffffffff,0,0,COLOR_WHITE,COLOR_BLACK,1,0);
    drawRectangle(5*fkeyw,LCD_HEIGHT_PX-23, fkeyw, 23, TEXT_COLOR_BLACK);
    Bdisp_MMPrint(5*fkeyw,LCD_HEIGHT_PX-STATUS_AREA_PX-19," QRHELP ",0,0xffffffff,0,0,COLOR_WHITE,COLOR_BLACK,1,0);
    int sres = doMenu(&menu);
    if(sres == MENU_RETURN_EXIT){
      reset_alpha();
      return sres;
    }
    int index=menuitems[menu.selection-1].isfolder;
    const char *fcmdname=menuitems[menu.selection-1].text,* fhowto=0,*fsyntax=0,*fexamples=0,*frelated=0;
    bool stathelp=has_static_help(fcmdname,lang,fhowto,fsyntax,fexamples,frelated);
    if (sres==KEY_CTRL_F6 && fcmdname){
      string url="https://www-fourier.ujf-grenoble.fr/~parisse/giac/doc/en/cascmd_en/"+longhelp(fcmdname);
      QRdisp(url.c_str(),"Xcas doc qrcode",fcmdname,"EXE/EXIT: OK",0);
    }
    if (sres == KEY_CTRL_F5) {
      char * example=index<allcmds?completeCat[index].example:0;
      char * example2=index<allcmds?completeCat[index].example2:0;
      textArea text;
      text.editable=false;
      text.clipline=-1;
      text.title = (char*)"Help on command";
      text.allowF1=true;
      text.python=python_compat(contextptr);
      ustl::vector<textElement> & elem=text.elements;
      elem = ustl::vector<textElement> (example2?4:3);
      elem[0].s = index<allcmds?completeCat[index].name:menuitems[menu.selection-1].text;
      elem[0].color = COLOR_BLUE;
      elem[1].newLine = 1;
      elem[1].lineSpacing = 3;
      ustl::string autoexample;
      if (index<allcmds){
	elem[1].s = completeCat[index].desc;
	// Chinese build: the translated static help carries a 0x01 GB18030 marker,
	// prefer it over the built-in (English) catalog description.
	if (stathelp && fhowto && (unsigned char)fhowto[0]==1)
	  elem[1].s = fhowto;
      }
      else {
	int token=menuitems[menu.selection-1].token;
	elem[1].s="Sorry, no help available...";
	// *logptr(contextptr) << token << endl;
	if (stathelp){
	  elem[1].s=remove_accents(fhowto);
	  //cout << fexamples << '\n';
	  example=(char *)fexamples;
	}
	else {
	  if (isopt){
	    if (token==_INT_PLOT+T_NUMBER*256){
	      autoexample="display="+elem[0].s;
	      elem[1].s ="display option: "+ autoexample;
	    }
	    if (token==_INT_COLOR+T_NUMBER*256){
	      autoexample="display="+elem[0].s;
	      elem[1].s="color option: "+ autoexample;
	    }
	    if (token==_INT_SOLVER+T_NUMBER*256){
	      autoexample=elem[0].s;
	      elem[1].s="fsolve option: " + autoexample;
	    }
	    if (token==_INT_TYPE+T_TYPE_ID*256){
	      autoexample=elem[0].s;
	      elem[1].s="Object type: " + autoexample;
	    }
	  }
	  if (isall){
	    if (token==T_UNARY_OP || token==T_UNARY_OP_38)
	      elem[1].s=elem[0].s+"(args)";
	  }
	}
      }
      ustl::string ex("Ex. (F2): "),ex2("Ex. (F3): ");
      elem[2].newLine = 1;
      elem[2].lineSpacing = 3;
      if (example){
	if (example[0]=='#')
	  ex += example+1;
	else {
	  if (index<allcmds){
	    ex += insert_string(index);
	    ex += example;
	    ex += ")";
	  }
	  else {
	    string allex(example);
	    ex += extract_example(allex,0); 
	    string tmp=extract_example(allex,1);
	    if (tmp.size()>1){
	      elem.push_back(elem[2]);
	      //cout << tmp << '\n' ;
	      elem[3].newLine = 1;
	      elem[3].lineSpacing = 3;
	      elem[3].s=ex2+tmp;
	    }
	  }
	}
	elem[2].s = ex;
	if (example2){
	  if (example2[0]=='#')
	    ex2 += example2+1;
	  else {
	    if (index<allcmds){
	      ex2 += insert_string(index);
	      ex2 += example2;
	      ex2 += ")";
	    }
	  }
	  elem[3].newLine = 1;
	  elem[3].lineSpacing = 3;
	  elem[3].s=ex2;
	}
      }
      else {
	if (autoexample.size())
	  elem[2].s=ex+autoexample;
	else
	  elem.pop_back();
      }
      sres=doTextArea(&text);
    }
    if (sres == KEY_CTRL_F2 || sres==KEY_CTRL_F3) {
      reset_alpha();
      if (isopt){
	int token=menuitems[menu.selection-1].token;
	if (token==_INT_PLOT+T_NUMBER*256 || token==_INT_COLOR+T_NUMBER*256)
	  strcpy(insertText,"display=");
	else
	  *insertText=0;
	strcat(insertText,menuitems[menu.selection-1].text);
	return 1;
      }
      ustl::string s;
      char * example=0;
      string ex;
      if (stathelp){
	ex=extract_example(fexamples,sres==KEY_CTRL_F2?0:1);
	example=(char *)ex.c_str();
      }
      if (index<allcmds && completeCat[index].example){
	s=insert_string(index);
	if (sres==KEY_CTRL_F2)
	  example=completeCat[index].example;
	else
	  example=completeCat[index].example2;
      }
      if (example){
	if (example[0]=='#')
	  s=example+1;
	else {
	  s += example;
	  if (!stathelp)
	    s += ")";
	}
	strcpy(insertText, s.c_str());
	return 1;
      }
      sres=KEY_CTRL_F1;
    }
    if(sres == MENU_RETURN_SELECTION || sres == KEY_CTRL_F1) {
      reset_alpha();
      strcpy(insertText,index<allcmds?insert_string(index).c_str():menuitems[menu.selection-1].text);
      return 1;
    }
  }
  return 0;
}
