#ifndef USB_TOOLS_H
#define USB_TOOLS_H

// =============================================================================
//  USB_Tools.h  -  DuckyScript interpreter for ESP32-S2/S3 native USB HID
// -----------------------------------------------------------------------------
//  Architecture
//    1) PARSE pass  : source text -> flat std::vector<Instr> (compiled once).
//                     Block structure (IF/ELSE/ENDIF, LOOP/ENDLOOP,
//                     WHILE/ENDWHILE, FUNCTION/ENDFUNCTION) is resolved into
//                     jump targets at parse time. No string is re-lexed later.
//    2) EXEC  pass  : a program counter walks the instruction array using a
//                     small control stack. Loops reset the PC instead of
//                     rebuilding + re-parsing source -> no recursion, no
//                     O(n*m) blow-up, far less heap churn on the ESP32.
// =============================================================================

#include <map>
#include <vector>
#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

// ----------------- Global HID objects -----------------
static USBHIDMouse    Mouse;
static USBHIDKeyboard Keyboard;

// ----------------- Tunables -----------------
static int  defaultDelay   = 30;                     // ms after each emitted action
static int  stringDelay    = 0;                      // ms between chars in STRING (host reliability)
static const long  MAX_DELAY_MS   = 60UL * 1000UL;   // clamp DELAY/WAIT
static const int   MAX_CALL_DEPTH = 32;              // CALL recursion guard
static const long  MAX_STEPS      = 2000000L;        // instruction budget

// ----------------- Variables & constants -----------------
static std::map<String, String> vars;
static std::map<String, String> defines;

// ----------------- Instruction set -----------------
enum Op {
  OP_NOP,
  OP_STRING, OP_STRINGLN, OP_PRINT,
  OP_KEY,
  OP_COMBO,
  OP_HOLD, OP_RELEASE, OP_RELEASE_ALL,
  OP_DELAY, OP_DEFAULT_DELAY, OP_STRING_DELAY, OP_WAIT,
  OP_MOUSE_MOVE, OP_MOUSE_CLICK, OP_MOUSE_SCROLL,
  OP_VAR, OP_MATH,
  OP_IF, OP_ELSE, OP_ENDIF,
  OP_LOOP, OP_ENDLOOP,
  OP_WHILE, OP_ENDWHILE,
  OP_FUNC, OP_ENDFUNC, OP_CALL, OP_RETURN,
  OP_REPEAT
};

struct Instr {
  Op     op   = OP_NOP;
  String a;
  String b;
  String c;
  long   n    = 0;
  long   m    = 0;
  int    j1   = -1;
  int    line = 0;
};

static std::vector<Instr>          program;
static std::map<String, int>       funcStart;

// ----------------- String helpers -----------------
static String clean(const String &in) {
  String s = in; s.replace("\r", ""); s.trim(); return s;
}

static int splitTokens(const String &s, String out[], int maxTok) {
  int count = 0, i = 0, len = s.length();
  while (i < len && count < maxTok) {
    while (i < len && s[i] == ' ') i++;
    if (i >= len) break;
    int start = i;
    while (i < len && s[i] != ' ') i++;
    out[count++] = s.substring(start, i);
  }
  return count;
}

static String expand(const String &in) {
  String out; out.reserve(in.length());
  int i = 0, len = in.length();
  while (i < len) {
    char ch = in[i];
    if (ch == '$' || ch == '#') {
      int j = i + 1; bool brace = false;
      if (ch == '$' && j < len && in[j] == '{') { brace = true; j++; }
      int start = j;
      while (j < len && (isAlphaNumeric(in[j]) || in[j] == '_')) j++;
      String name = in.substring(start, j);
      if (brace && j < len && in[j] == '}') j++;
      std::map<String, String> &tbl = (ch == '$') ? vars : defines;
      if (name.length() && tbl.count(name)) out += tbl[name];
      else                                  out += in.substring(i, j);
      i = j;
    } else {
      out += in[i++];
    }
  }
  return out;
}

// ----------------- Recursive-descent integer evaluator -----------------
struct ExprParser {
  const String &s; int i;
  ExprParser(const String &str) : s(str), i(0) {}
  void skip() { while (i < (int)s.length() && s[i] == ' ') i++; }

  long factor() {
    skip();
    if (i < (int)s.length() && s[i] == '(') { i++; long v = expr(); skip(); if (i < (int)s.length() && s[i] == ')') i++; return v; }
    if (i < (int)s.length() && s[i] == '-') { i++; return -factor(); }
    if (i < (int)s.length() && isDigit(s[i])) {
      int start = i;
      while (i < (int)s.length() && isDigit(s[i])) i++;
      return s.substring(start, i).toInt();
    }
    if (i < (int)s.length() && (s[i] == '$' || isAlpha(s[i]) || s[i] == '_')) {
      if (s[i] == '$') i++;
      int start = i;
      while (i < (int)s.length() && (isAlphaNumeric(s[i]) || s[i] == '_')) i++;
      String name = s.substring(start, i);
      return vars.count(name) ? vars[name].toInt() : 0;
    }
    return 0;
  }
  long term() {
    long v = factor();
    for (;;) { skip();
      if      (i < (int)s.length() && s[i] == '*') { i++; v *= factor(); }
      else if (i < (int)s.length() && s[i] == '/') { i++; long d = factor(); v = d ? v / d : 0; }
      else if (i < (int)s.length() && s[i] == '%') { i++; long d = factor(); v = d ? v % d : 0; }
      else break;
    }
    return v;
  }
  long expr() {
    long v = term();
    for (;;) { skip();
      if      (i < (int)s.length() && s[i] == '+') { i++; v += term(); }
      else if (i < (int)s.length() && s[i] == '-') { i++; v -= term(); }
      else break;
    }
    return v;
  }
};
static long evalExpr(const String &raw) { String e = expand(raw); ExprParser p(e); return p.expr(); }

static bool isNumeric(const String &s) {
  if (!s.length()) return false;
  int i = (s[0] == '-') ? 1 : 0;
  if (i >= (int)s.length()) return false;
  for (; i < (int)s.length(); i++) if (!isDigit(s[i])) return false;
  return true;
}

static String resolveOperand(const String &raw) {
  String v = expand(raw);
  if (v == raw && vars.count(raw)) return vars[raw];
  return v;
}

static bool evalCond(const String &lraw, const String &op, const String &rraw) {
  String left = resolveOperand(lraw), right = resolveOperand(rraw);
  bool num = isNumeric(left) && isNumeric(right);
  long L = left.toInt(), R = right.toInt();
  if (op == "==") return num ? (L == R) : (left == right);
  if (op == "!=") return num ? (L != R) : (left != right);
  if (op == ">")  return L >  R;
  if (op == "<")  return L <  R;
  if (op == ">=") return L >= R;
  if (op == "<=") return L <= R;
  return false;
}

// ----------------- Key tables -----------------
static bool modKey(const String &t, uint8_t &out) {
  String u = t; u.toUpperCase();
  if (u == "CTRL" || u == "CONTROL") { out = KEY_LEFT_CTRL;  return true; }
  if (u == "ALT")                    { out = KEY_LEFT_ALT;   return true; }
  if (u == "SHIFT")                  { out = KEY_LEFT_SHIFT;  return true; }
  if (u == "GUI" || u == "WINDOWS" || u == "WIN" || u == "CMD" || u == "META")
                                     { out = KEY_LEFT_GUI;   return true; }
  return false;
}

static bool namedKey(const String &t, uint8_t &out) {
  String u = t; u.toUpperCase();
  if (u == "ENTER" || u == "RETURN") { out = KEY_RETURN;       return true; }
  if (u == "TAB")                    { out = KEY_TAB;          return true; }
  if (u == "ESC" || u == "ESCAPE")   { out = KEY_ESC;          return true; }
  if (u == "SPACE")                  { out = ' ';              return true; }
  if (u == "BACKSPACE" || u == "BKSP"){ out = KEY_BACKSPACE;   return true; }
  if (u == "DELETE" || u == "DEL")   { out = KEY_DELETE;       return true; }
  if (u == "INSERT" || u == "INS")   { out = KEY_INSERT;       return true; }
  if (u == "HOME")                   { out = KEY_HOME;         return true; }
  if (u == "END")                    { out = KEY_END;          return true; }
  if (u == "PAGEUP")                 { out = KEY_PAGE_UP;      return true; }
  if (u == "PAGEDOWN")               { out = KEY_PAGE_DOWN;    return true; }
  if (u == "UP")                     { out = KEY_UP_ARROW;     return true; }
  if (u == "DOWN")                   { out = KEY_DOWN_ARROW;   return true; }
  if (u == "LEFT")                   { out = KEY_LEFT_ARROW;   return true; }
  if (u == "RIGHT")                  { out = KEY_RIGHT_ARROW;  return true; }
  if (u == "CAPSLOCK")               { out = KEY_CAPS_LOCK;    return true; }
#ifdef KEY_MENU
  if (u == "MENU" || u == "APP")     { out = KEY_MENU;         return true; }
#endif
  if (u.length() > 1 && u[0] == 'F' && isDigit(u[1])) {
    int f = u.substring(1).toInt();
    if (f >= 1 && f <= 12) { out = KEY_F1 + (f - 1); return true; }
  }
  return false;
}

static bool resolveKey(const String &t, uint8_t &out) {
  if (namedKey(t, out)) return true;
  if (t.length() == 1)  { out = (uint8_t)t[0]; return true; }
  return false;
}

// ----------------- PARSE pass -----------------
struct Frame { Op op; int idx; };

static void parse(const String &src) {
  program.clear();
  funcStart.clear();
  defines.clear();
  std::vector<Frame> stk;
  int lineNo = 0, pos = 0, len = src.length();

  while (pos < len) {
    int e = src.indexOf('\n', pos);
    if (e == -1) e = len;
    String line = clean(src.substring(pos, e));
    pos = e + 1;
    lineNo++;
    if (line.isEmpty() || line.startsWith("REM")) continue;

    int sp = line.indexOf(' ');
    String head = (sp == -1) ? line : line.substring(0, sp);
    String rest = (sp == -1) ? ""   : line.substring(sp + 1);
    String HEAD = head; HEAD.toUpperCase();

    if      (HEAD == "END_IF")       HEAD = "ENDIF";
    else if (HEAD == "END_WHILE")    HEAD = "ENDWHILE";
    else if (HEAD == "END_LOOP")     HEAD = "ENDLOOP";
    else if (HEAD == "END_FUNCTION") HEAD = "ENDFUNCTION";

    Instr in; in.line = lineNo;

    if      (HEAD == "STRING")   { in.op = OP_STRING;    in.a = rest; }
    else if (HEAD == "STRINGLN") { in.op = OP_STRINGLN;  in.a = rest; }
    else if (HEAD == "PRINT")    { in.op = OP_PRINT;     in.a = rest; }

    else if (HEAD == "DELAY")         { in.op = OP_DELAY;         in.n = rest.toInt(); }
    else if (HEAD == "DEFAULT_DELAY") { in.op = OP_DEFAULT_DELAY; in.n = rest.toInt(); }
    else if (HEAD == "STRING_DELAY")  { in.op = OP_STRING_DELAY;  in.n = rest.toInt(); }
    else if (HEAD == "WAIT")          { in.op = OP_WAIT;          in.n = rest.toInt(); }
    else if (HEAD == "REPEAT")        { in.op = OP_REPEAT;        in.n = rest.toInt(); }

    else if (HEAD == "HOLD")     { in.op = OP_HOLD;    in.a = clean(rest); }
    else if (HEAD == "RELEASE")  { String r = clean(rest);
                                   if (r.isEmpty()) in.op = OP_RELEASE_ALL;
                                   else { in.op = OP_RELEASE; in.a = r; } }

    else if (HEAD == "MOUSE") {
      String t[3]; int nt = splitTokens(rest, t, 3);
      String sub = nt ? t[0] : ""; sub.toUpperCase();
      if      (sub == "MOVE")   { in.op = OP_MOUSE_MOVE;   in.n = (nt>1?t[1].toInt():0); in.m = (nt>2?t[2].toInt():0); }
      else if (sub == "CLICK")  { in.op = OP_MOUSE_CLICK;  in.a = (nt>1?t[1]:"LEFT"); }
      else if (sub == "SCROLL") { in.op = OP_MOUSE_SCROLL; in.n = (nt>1?t[1].toInt():0); }
      else continue;
    }

    else if (HEAD == "DEFINE") {
      String r = clean(rest);
      int s2 = r.indexOf(' ');
      if (s2 < 0) continue;
      String name = r.substring(0, s2);
      if (name.startsWith("#")) name = name.substring(1);
      defines[name] = clean(r.substring(s2 + 1));
      continue;
    }
    else if (HEAD == "VAR") {
      int eq = rest.indexOf('=');
      if (eq < 0) continue;
      String name = clean(rest.substring(0, eq));
      if (name.startsWith("$")) name = name.substring(1);
      in.op = OP_VAR; in.a = name; in.b = clean(rest.substring(eq + 1));
    }
    else if (HEAD == "MATH") {
      int eq = rest.indexOf('=');
      if (eq < 0) continue;
      String name = clean(rest.substring(0, eq));
      if (name.startsWith("$")) name = name.substring(1);
      in.op = OP_MATH; in.a = name; in.b = clean(rest.substring(eq + 1));
    }

    else if (HEAD == "IF" || HEAD == "WHILE") {
      String cond = rest;
      int thenPos = cond.indexOf(" THEN");
      if (thenPos < 0) { String U = cond; U.toUpperCase(); if (U.endsWith(" THEN")) thenPos = cond.length() - 5; }
      if (thenPos >= 0) cond = cond.substring(0, thenPos);
      cond.replace("(", " "); cond.replace(")", " ");
      cond = clean(cond);
      String t[3]; int nt = splitTokens(cond, t, 3);
      if (nt < 3) continue;
      in.op = (HEAD == "IF") ? OP_IF : OP_WHILE;
      in.a = t[0]; in.b = t[1]; in.c = t[2];
      stk.push_back({in.op, (int)program.size()});
    }
    else if (HEAD == "ELSE")     { in.op = OP_ELSE; }
    else if (HEAD == "ENDIF")    { in.op = OP_ENDIF; }
    else if (HEAD == "ENDWHILE") { in.op = OP_ENDWHILE; }
    else if (HEAD == "LOOP")     { in.op = OP_LOOP; in.n = rest.toInt();
                                   stk.push_back({OP_LOOP, (int)program.size()}); }
    else if (HEAD == "ENDLOOP")  { in.op = OP_ENDLOOP; }

    else if (HEAD == "FUNCTION") { in.op = OP_FUNC;
                                   String nm = clean(rest); nm.replace("(", ""); nm.replace(")", "");
                                   in.a = nm;
                                   funcStart[nm] = (int)program.size() + 1;
                                   stk.push_back({OP_FUNC, (int)program.size()}); }
    else if (HEAD == "ENDFUNCTION") { in.op = OP_ENDFUNC; }
    else if (HEAD == "RETURN")      { in.op = OP_RETURN; }
    else if (HEAD == "CALL")        { in.op = OP_CALL; in.a = clean(rest); }

    else if (head.endsWith("()")) {
      in.op = OP_CALL; in.a = head.substring(0, head.length() - 2);
    }
    else { uint8_t mk;
           if (modKey(head, mk)) { in.op = OP_COMBO; in.a = line; }
           else                  { in.op = OP_KEY;   in.a = head; }
    }

    int idx = program.size();
    program.push_back(in);

    // ---- resolve block jumps as closers are seen ----
    if (in.op == OP_ELSE) {
      if (stk.empty() || stk.back().op != OP_IF) { Serial.printf("[PARSE] ELSE without IF @%d\n", lineNo); }
      else { program[stk.back().idx].j1 = idx + 1;
             stk.back() = {OP_ELSE, idx}; }
    }
    else if (in.op == OP_ENDIF) {
      if (stk.empty()) { Serial.printf("[PARSE] ENDIF without IF @%d\n", lineNo); }
      else { Frame f = stk.back(); stk.pop_back();
             if (f.op == OP_ELSE)     program[f.idx].j1 = idx;
             else /* OP_IF */         program[f.idx].j1 = idx; }
    }
    else if (in.op == OP_ENDLOOP) {
      if (stk.empty() || stk.back().op != OP_LOOP) { Serial.printf("[PARSE] ENDLOOP without LOOP @%d\n", lineNo); }
      else { program[stk.back().idx].j1 = idx; in.j1 = stk.back().idx; program[idx].j1 = stk.back().idx; stk.pop_back(); }
    }
    else if (in.op == OP_ENDWHILE) {
      if (stk.empty() || stk.back().op != OP_WHILE) { Serial.printf("[PARSE] ENDWHILE without WHILE @%d\n", lineNo); }
      else { program[stk.back().idx].j1 = idx; program[idx].j1 = stk.back().idx; stk.pop_back(); }
    }
    else if (in.op == OP_ENDFUNC) {
      if (stk.empty() || stk.back().op != OP_FUNC) { Serial.printf("[PARSE] ENDFUNCTION without FUNCTION @%d\n", lineNo); }
      else { program[stk.back().idx].j1 = idx; stk.pop_back(); }
    }
  }
  if (!stk.empty()) Serial.printf("[PARSE] %d unterminated block(s)\n", (int)stk.size());
}

// ----------------- chord / key emit -----------------
static void emitCombo(const String &line) {
  String t[6]; int nt = splitTokens(line, t, 6);
  uint8_t code;
  for (int k = 0; k < nt; k++) {
    if (modKey(t[k], code))      Keyboard.press(code);
    else if (resolveKey(t[k], code)) Keyboard.press(code);
  }
  delay(5);
  Keyboard.releaseAll();
}

static void emitKey(const String &tok) {
  uint8_t code;
  if (resolveKey(tok, code)) Keyboard.write(code);
  else Serial.println("[WARN] Unknown key: " + tok);
}

// ----------------- single instruction (used by EXEC + REPEAT) -----------------
static void runOne(const Instr &in) {
  switch (in.op) {
    case OP_STRING: {
      String s = expand(in.a);
      if (stringDelay <= 0) Keyboard.print(s);
      else for (int k = 0; k < (int)s.length(); k++) { Keyboard.print(s[k]); delay(stringDelay); }
      break;
    }
    case OP_STRINGLN:   { String s = expand(in.a); Keyboard.print(s); Keyboard.write(KEY_RETURN); break; }
    case OP_PRINT:        Serial.println("[PRINT] " + expand(in.a)); break;
    case OP_KEY:          emitKey(in.a); break;
    case OP_COMBO:        emitCombo(in.a); break;
    case OP_HOLD:        { uint8_t c; if (modKey(in.a, c) || resolveKey(in.a, c)) Keyboard.press(c); break; }
    case OP_RELEASE:     { uint8_t c; if (modKey(in.a, c) || resolveKey(in.a, c)) Keyboard.release(c); break; }
    case OP_RELEASE_ALL:  Keyboard.releaseAll(); break;
    case OP_DELAY:        delay(constrain(in.n, 0, MAX_DELAY_MS)); break;
    case OP_WAIT:         delay(constrain(in.n, 0, MAX_DELAY_MS)); break;
    case OP_DEFAULT_DELAY: defaultDelay = max(0L, in.n); break;
    case OP_STRING_DELAY:  stringDelay  = max(0L, in.n); break;
    case OP_MOUSE_MOVE:    Mouse.move((signed char)in.n, (signed char)in.m); break;
    case OP_MOUSE_CLICK:  { String b = in.a; b.toUpperCase();
                            if      (b == "RIGHT")  Mouse.click(MOUSE_RIGHT);
                            else if (b == "MIDDLE") Mouse.click(MOUSE_MIDDLE);
                            else                    Mouse.click(MOUSE_LEFT); break; }
    case OP_MOUSE_SCROLL:  Mouse.move(0, 0, (signed char)in.n); break;
    case OP_VAR:           vars[in.a] = expand(in.b); break;
    case OP_MATH:          vars[in.a] = String(evalExpr(in.b)); break;
    default: break;
  }
}

static bool isAction(const Instr &in) {
  switch (in.op) {
    case OP_STRING: case OP_STRINGLN: case OP_KEY: case OP_COMBO:
    case OP_HOLD: case OP_RELEASE: case OP_RELEASE_ALL:
    case OP_MOUSE_MOVE: case OP_MOUSE_CLICK: case OP_MOUSE_SCROLL:
      return true;
    default: return false;
  }
}

// ----------------- EXEC pass -----------------
struct LoopRT { int back; long remaining; };

static void execute() {
  std::vector<LoopRT> loops;
  std::vector<int>    calls;
  int  pc = 0;
  int  lastAction = -1;
  long steps = 0;

  while (pc >= 0 && pc < (int)program.size()) {
    if (++steps > MAX_STEPS) { Serial.println("[ERR] instruction budget exceeded"); break; }
    const Instr &in = program[pc];

    switch (in.op) {
      case OP_IF:
        if (evalCond(in.a, in.b, in.c)) pc++;
        else pc = (in.j1 >= 0) ? in.j1 : pc + 1;
        break;

      case OP_ELSE:
        pc = (in.j1 >= 0) ? in.j1 : pc + 1;
        break;

      case OP_ENDIF:
        pc++;
        break;

      case OP_LOOP:
        if (in.n <= 0) pc = in.j1 + 1;
        else { loops.push_back({pc + 1, in.n}); pc++; }
        break;

      case OP_ENDLOOP:
        if (!loops.empty()) {
          if (--loops.back().remaining > 0) pc = loops.back().back;
          else { loops.pop_back(); pc++; }
        } else pc++;
        break;

      case OP_WHILE:
        if (evalCond(in.a, in.b, in.c)) pc++;
        else pc = in.j1 + 1;
        break;

      case OP_ENDWHILE:
        pc = in.j1;
        break;

      case OP_FUNC:
        pc = in.j1 + 1;
        break;

      case OP_CALL:
        if (funcStart.count(in.a)) {
          if ((int)calls.size() >= MAX_CALL_DEPTH) { Serial.printf("[ERR] call depth @line %d\n", in.line); pc++; }
          else { calls.push_back(pc + 1); pc = funcStart[in.a]; }
        } else { Serial.println("[ERR] unknown function: " + in.a); pc++; }
        break;

      case OP_RETURN:
      case OP_ENDFUNC:
        if (!calls.empty()) { pc = calls.back(); calls.pop_back(); } else pc++;
        break;

      case OP_REPEAT:
        if (lastAction >= 0)
          for (long r = 0; r < in.n; r++) { runOne(program[lastAction]); delay(defaultDelay); }
        pc++;
        break;

      default:
        runOne(in);
        if (isAction(in)) { lastAction = pc; delay(defaultDelay); }
        pc++;
        break;
    }
  }
  Keyboard.releaseAll();
}

// ----------------- USB start -----------------
static void startUSB(const String &dev) {
  USB.begin();
  if      (dev.equalsIgnoreCase("Keyboard")) Keyboard.begin();
  else if (dev.equalsIgnoreCase("Mouse"))    Mouse.begin();
  else                                       { Keyboard.begin(); Mouse.begin(); }
  delay(900);
}

// ---- Public entry (compat with old name) ----
static void DuckyScript(const String &script) {
  parse(script);
  execute();
}

#endif