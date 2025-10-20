#ifndef USB_TOOLS_H
#define USB_TOOLS_H

#include <map>
#include <vector>
#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

// ----------------- Global HID objects -----------------
static USBHIDMouse Mouse;
static USBHIDKeyboard Keyboard;

// ----------------- State & storage -----------------
static int defaultDelay = 30;     // ms between commands (can be overridden by DEFAULT_DELAY)
static String lastCmd = "";     // last executed command (for REPEAT)
static std::map<String, String> vars;    // user variables
static std::map<String, String> macros;  // recorded macros

// ----------------- Helpers -----------------
// Trim + normalize a line
static String clean(const String &s_in) {
  String s = s_in;
  s.trim();
  s.replace("\r", "");
  return s;
}

// Expand ${name} placeholders using vars map
static String expandVars(const String &in) {
  String out = in;
  for (auto &p : vars) out.replace("${" + p.first + "}", p.second);
  return out;
}

// Evaluate a simple binary condition with optional variable lookup
static bool evalCond(String left, const String &op, String right) {
  left.trim(); right.trim();
  if (vars.count(left)) left = vars[left];
  if (vars.count(right)) right = vars[right];

  // If both look numeric, compare numerically for > and <
  if (op == "==") return left == right;
  if (op == "!=") return left != right;
  if (op == ">")  return left.toInt() > right.toInt();
  if (op == "<")  return left.toInt() < right.toInt();
  return false;
}

// ----------------- USB start -----------------
// Use: startUSB("Keyboard"), startUSB("Mouse"), startUSB("Both")
static void startUSB(const String &dev) {
  USB.begin();
  if (dev.equalsIgnoreCase("Keyboard")) Keyboard.begin();
  else if (dev.equalsIgnoreCase("Mouse")) Mouse.begin();
  else if (dev.equalsIgnoreCase("Both")) { Keyboard.begin(); Mouse.begin(); }
  delay(defaultDelay * 30); // small pause after USB initialization
}

// ----------------- DuckyScript interpreter -----------------
static void DuckyScript(const String &script);

// Helper: execute a block (used for loops and macros)
static void executeBlock(const String &block) {
  DuckyScript(block);
}

// Main interpreter
static void DuckyScript(const String &script) {
  // Split into lines
  std::vector<String> lines;
  int pos = 0;
  while (pos < script.length()) {
    int e = script.indexOf('\n', pos);
    if (e == -1) e = script.length();
    lines.push_back(clean(script.substring(pos, e)));
    pos = e + 1;
  }

  for (size_t i = 0; i < lines.size(); i++) {
    String line = lines[i];
    if (line.isEmpty()) continue;
    if (line.startsWith("REM")) continue; // comment

    // ====== VARIABLE DECLARATION: VAR name = value
    if (line.startsWith("VAR ")) {
      int eq = line.indexOf('=');
      if (eq > 0) {
        String name = clean(line.substring(4, eq));
        String value = clean(line.substring(eq + 1));
        value = expandVars(value); // support dynamic assignment
        vars[name] = value;
      }
    }

    // ====== MACRO RECORDING: MACRO name / MACROEND
    else if (line.startsWith("MACRO ")) {
      String mname = clean(line.substring(6));
      String block = "";
      int nest = 0;
      while (++i < lines.size()) {
        String l = lines[i];
        if (l.startsWith("MACRO ")) nest++;
        else if (l == "MACROEND" && nest == 0) break;
        else if (l == "MACROEND") nest--;
        block += l + "\n";
      }
      macros[mname] = block;
    }

    // ====== RUNMACRO name [count]
    else if (line.startsWith("RUNMACRO ")) {
      String rest = clean(line.substring(9));
      int sp = rest.indexOf(' ');
      String mname = (sp == -1) ? rest : rest.substring(0, sp);
      int count = 1;
      if (sp != -1) count = rest.substring(sp + 1).toInt();
      if (macros.count(mname)) {
        for (int r = 0; r < count; r++) executeBlock(macros[mname]);
      } else {
        Serial.println("[ERR] Unknown macro: " + mname);
      }
    }

    // ====== IF ... ELSE / ENDIF ======
    else if (line.startsWith("IF ")) {
      String cond = clean(line.substring(3));
      // parse left op right - assume single-space separated tokens
      int p1 = cond.indexOf(' ');
      int p2 = cond.indexOf(' ', p1 + 1);
      if (p1 > 0 && p2 > p1) {
        String left = cond.substring(0, p1);
        String op   = cond.substring(p1 + 1, p2);
        String right = cond.substring(p2 + 1);
        bool result = evalCond(left, op, right);
        if (!result) {
          // skip to ELSE or ENDIF respecting nesting
          int nest = 0;
          while (++i < lines.size()) {
            String l = lines[i];
            if (l.startsWith("IF ")) nest++;
            else if (l == "ENDIF" && nest == 0) break;
            else if (l == "ELSE" && nest == 0) break;
            else if (l == "ENDIF") nest--;
          }
        }
      }
    }
    else if (line == "ELSE") {
      int nest = 0;
      while (++i < lines.size()) {
        String l = lines[i];
        if (l.startsWith("IF ")) nest++;
        else if (l == "ENDIF" && nest == 0) break;
        else if (l == "ENDIF") nest--;
      }
    }
    else if (line == "ENDIF") {
      // no-op: handled by skipping logic
    }

    // ====== LOOP n / ENDLOOP ======
    else if (line.startsWith("LOOP ")) {
      int n = line.substring(5).toInt();
      int loopStart = i + 1;
      int nest = 0, endIdx = -1;
      for (size_t j = loopStart; j < lines.size(); j++) {
        if (lines[j].startsWith("LOOP ")) nest++;
        else if (lines[j] == "ENDLOOP" && nest == 0) { endIdx = j; break; }
        else if (lines[j] == "ENDLOOP") nest--;
      }
      if (endIdx == -1) { Serial.println("[ERR] Missing ENDLOOP"); break; }

      // Build block
      String block = "";
      for (size_t k = loopStart; k < endIdx; k++) block += lines[k] + "\n";

      for (int r = 0; r < n; r++) executeBlock(block);
      i = endIdx; // continue after ENDLOOP
    }

    // ====== STRING / STRINGLN ======
    else if (line.startsWith("STRINGLN ")) {
      String text = expandVars(line.substring(9));
      Keyboard.print(text);
      Keyboard.write(KEY_RETURN);
    }
    else if (line.startsWith("STRING ")) {
      String text = expandVars(line.substring(7));
      Keyboard.print(text);
    }

    // ====== STANDARD KEYS ======
    else if (line.equalsIgnoreCase("ENTER")) Keyboard.write(KEY_RETURN);
    else if (line.equalsIgnoreCase("TAB")) Keyboard.write(KEY_TAB);
    else if (line.equalsIgnoreCase("ESCAPE")) Keyboard.write(KEY_ESC);
    else if (line.equalsIgnoreCase("SPACE")) Keyboard.write(' ');
    else if (line.equalsIgnoreCase("UP")) Keyboard.write(KEY_UP_ARROW);
    else if (line.equalsIgnoreCase("DOWN")) Keyboard.write(KEY_DOWN_ARROW);
    else if (line.equalsIgnoreCase("LEFT")) Keyboard.write(KEY_LEFT_ARROW);
    else if (line.equalsIgnoreCase("RIGHT")) Keyboard.write(KEY_RIGHT_ARROW);
    else if (line.equalsIgnoreCase("CAPSLOCK")) Keyboard.write(KEY_CAPS_LOCK);
    else if (line.equalsIgnoreCase("MENU")) Keyboard.write(KEY_MENU);

    // ====== FUNCTION KEYS F1..F12 (e.g. F5) ======
    else if (line.length() > 1 && (line[0] == 'F' || line[0] == 'f') && isDigit(line[1])) {
      int f = line.substring(1).toInt();
      if (f >= 1 && f <= 12) Keyboard.write(KEY_F1 + (f - 1));
    }

    // ====== DELAY & DEFAULT_DELAY ======
    else if (line.startsWith("DELAY ")) delay(line.substring(6).toInt());
    else if (line.startsWith("DEFAULT_DELAY ")) defaultDelay = line.substring(14).toInt();

    // ====== REPEAT n ======
    else if (line.startsWith("REPEAT ")) {
      int n = line.substring(7).toInt();
      for (int r = 0; r < n; r++) {
        if (!lastCmd.isEmpty()) {
          // execute lastCmd as a single-line script
          DuckyScript(lastCmd + "\n");
        }
        delay(defaultDelay);
      }
    }

    // ====== GUI / WINDOWS <char or KEYNAME> ======
    else if (line.startsWith("GUI ") || line.startsWith("WINDOWS ")) {
      String keypart = clean(line.substring(line.indexOf(' ') + 1));
      // if single char, send that with GUI
      Keyboard.press(KEY_LEFT_GUI);
      if (keypart.length() == 1) {
        Keyboard.press(keypart[0]);
      } else {
        // try named keys like 'r' or 'R' or 'TAB' etc. fallback to sending first char
        Keyboard.press(keypart[0]);
      }
      Keyboard.releaseAll();
    }

    // ====== COMBO KEYS (CTRL/ALT/SHIFT + KEY) ======
    else if (line.indexOf("CTRL") >= 0 || line.indexOf("ALT") >= 0 || line.indexOf("SHIFT") >= 0) {
      if (line.indexOf("CTRL") >= 0) Keyboard.press(KEY_LEFT_CTRL);
      if (line.indexOf("ALT") >= 0) Keyboard.press(KEY_LEFT_ALT);
      if (line.indexOf("SHIFT") >= 0) Keyboard.press(KEY_LEFT_SHIFT);
      if (line.indexOf("DEL") >= 0) Keyboard.press(KEY_DELETE);
      if (line.indexOf("ESC") >= 0) Keyboard.press(KEY_ESC);
      if (line.indexOf("TAB") >= 0) Keyboard.press(KEY_TAB);
      if (line.indexOf("ENTER") >= 0) Keyboard.press(KEY_RETURN);
      Keyboard.releaseAll();
    }

    // ====== PRINT (debug) ======
    else if (line.startsWith("PRINT ")) {
      String msg = expandVars(line.substring(6));
      Serial.println("[PRINT] " + msg);
    }

    // ====== WAIT <ms>  OR WAIT var==value (polling) ======
    else if (line.startsWith("WAIT ")) {
      String arg = clean(line.substring(5));
      int eqpos = arg.indexOf("==");
      if (eqpos > 0) {
        String var = clean(arg.substring(0, eqpos));
        String val = clean(arg.substring(eqpos + 2));
        // poll until equal
        while (!vars.count(var) || vars[var] != val) delay(50);
      } else {
        delay(arg.toInt());
      }
    }

    // ====== MOUSE commands ======
    else if (line.startsWith("MOUSE MOVE ")) {
      // MOUSE MOVE x y
      String rest = clean(line.substring(11));
      int sp = rest.indexOf(' ');
      if (sp > 0) {
        int x = rest.substring(0, sp).toInt();
        int y = rest.substring(sp + 1).toInt();
        Mouse.move(x, y);
      }
    }
    else if (line.startsWith("MOUSE CLICK ")) {
      String btn = clean(line.substring(12));
      if (btn.equalsIgnoreCase("LEFT")) Mouse.click(MOUSE_LEFT);
      else if (btn.equalsIgnoreCase("RIGHT")) Mouse.click(MOUSE_RIGHT);
      else if (btn.equalsIgnoreCase("MIDDLE")) Mouse.click(MOUSE_MIDDLE);
    }
    else if (line.startsWith("MOUSE SCROLL ")) {
      int s = clean(line.substring(13)).toInt();
      Mouse.move(0, 0, s);
    }

    // ====== Fallback: unknown command ======
    else {
      Serial.println("[WARN] Unknown command: " + line);
    }

    // Save last non-empty command for REPEAT
    lastCmd = line;
    delay(defaultDelay);
  }
}

#endif
