#ifndef CONFIGS_H
#define CONFIGS_H

// globals
String customScript = "";

// DNS Port
const byte DNS_PORT = 53;

// Wi-Fi AP settings
String  inputSSID   = "USB Ware";                                 // SSID
String  inputPASS   = "HID1000#";                                 // Password (leave empty for open network)
int     inputCH     = 6;                                          // Channel
bool    inputHidden = false;                                      // Hidden
int     maxClients  = 1;                                          // MAX devices allowed
uint8_t MAC[6]      = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};       // Custom MAC
IPAddress local_IP(10, 0, 0, 1);                                  // IP
IPAddress gateway(10, 0, 0, 1);                                   // Gateway
IPAddress subnet(255, 255, 255, 0);                               // Subnet mask

// Optional shared secret. Leave empty to disable. When set, the firmware
String  accessKey   = "";

//--------------------------------------------------

const char MainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>USB://WARE</title>
<style>
  :root{
    --bg:#04070a; --panel:#070d0c; --panel2:#0a1311; --edge:#103324;
    --fg:#4dffb0; --fg-dim:#1f8a5c; --ghost:#0e3a28;
    --amber:#ffb454; --err:#ff5d6c; --info:#5cd0ff;
    --glow:0 0 6px rgba(77,255,176,.35);
    --mono:ui-monospace,"SFMono-Regular",Menlo,Consolas,"DejaVu Sans Mono",monospace;
  }
  *{box-sizing:border-box}
  html,body{height:100%}
  body{
    margin:0; background:var(--bg); color:var(--fg);
    font-family:var(--mono); font-size:14px; line-height:1.5;
    padding:18px; min-height:100vh;
    background-image:radial-gradient(120% 90% at 50% -10%,rgba(77,255,176,.06),transparent 60%);
  }
  /* CRT overlays */
  body::before{content:"";position:fixed;inset:0;pointer-events:none;z-index:50;
    background:repeating-linear-gradient(0deg,rgba(0,0,0,0) 0 2px,rgba(0,0,0,.18) 2px 3px);
    mix-blend-mode:multiply;animation:flick 6s infinite steps(60)}
  body::after{content:"";position:fixed;inset:0;pointer-events:none;z-index:51;
    background:radial-gradient(130% 120% at 50% 50%,transparent 62%,rgba(0,0,0,.55) 100%)}
  @keyframes flick{0%,97%{opacity:1}98%{opacity:.86}100%{opacity:1}}

  .wrap{max-width:1040px;margin:0 auto;position:relative;z-index:1;opacity:0;animation:boot .5s .15s forwards}
  @keyframes boot{to{opacity:1}}

  header{display:flex;align-items:center;gap:14px;flex-wrap:wrap;
    border:1px solid var(--edge);background:var(--panel);
    padding:12px 16px;border-radius:10px;box-shadow:inset 0 0 30px rgba(77,255,176,.04)}
  .brand{font-weight:700;letter-spacing:2px;text-shadow:0 0 10px var(--fg),0 0 22px var(--fg)}
  .brand b{color:var(--amber);text-shadow:0 0 12px var(--amber)}
  .chips{display:flex;gap:8px;flex-wrap:wrap;margin-left:auto}
  .chip{border:1px solid var(--ghost);border-radius:999px;padding:3px 10px;color:var(--fg-dim);font-size:12px}
  .chip i{color:var(--fg);font-style:normal}
  .dot{width:8px;height:8px;border-radius:50%;background:var(--fg);display:inline-block;
    box-shadow:0 0 8px var(--fg);animation:pulse 1.6s infinite}
  @keyframes pulse{50%{opacity:.35}}

  .grid{display:grid;grid-template-columns:1.5fr 1fr;gap:14px;margin-top:14px}
  @media(max-width:860px){.grid{grid-template-columns:1fr}}
  .card{border:1px solid var(--edge);background:var(--panel);border-radius:10px;overflow:hidden}
  .card>.bar{display:flex;align-items:center;gap:8px;flex-wrap:wrap;
    padding:8px 10px;border-bottom:1px solid var(--edge);background:var(--panel2)}
  .bar .tag{color:var(--fg-dim);font-size:12px;letter-spacing:1px;margin-right:auto}

  button,select,.fbtn{font-family:var(--mono);font-size:13px;cursor:pointer;
    background:transparent;color:var(--fg);border:1px solid var(--ghost);
    border-radius:6px;padding:6px 11px;transition:.15s;white-space:nowrap}
  button:hover,select:hover,.fbtn:hover{border-color:var(--fg);box-shadow:var(--glow);background:rgba(77,255,176,.06)}
  button.run{border-color:var(--fg);color:#04110b;background:var(--fg);font-weight:700;
    text-shadow:none;box-shadow:0 0 14px rgba(77,255,176,.5)}
  button.run:hover{background:#7dffc7}
  button:disabled{opacity:.4;cursor:not-allowed;box-shadow:none;border-color:var(--ghost)}
  select{padding-right:6px}
  .key{width:108px;border:1px solid var(--ghost);background:var(--bg);color:var(--amber);
    border-radius:6px;padding:6px 8px;font-family:var(--mono);font-size:12px}
  .key::placeholder{color:#5a4a20}

  /* editor */
  .edit{display:flex;background:var(--bg);max-height:52vh;min-height:300px}
  .gutter{padding:10px 8px;text-align:right;color:var(--ghost);user-select:none;
    background:var(--panel2);border-right:1px solid var(--edge);overflow:hidden;min-width:42px}
  .gutter span{display:block}
  #editor{flex:1;border:0;outline:0;resize:none;background:transparent;color:var(--fg);
    font-family:var(--mono);font-size:14px;line-height:1.5;padding:10px 12px;
    caret-color:var(--amber);white-space:pre;overflow:auto;tab-size:2}
  #editor::selection{background:rgba(77,255,176,.25)}
  .efoot{display:flex;gap:14px;padding:6px 12px;border-top:1px solid var(--edge);
    background:var(--panel2);color:var(--fg-dim);font-size:12px}
  .efoot .sp{margin-left:auto}

  /* console */
  #log{height:200px;overflow:auto;padding:10px 12px;background:var(--bg);font-size:13px}
  #log .l{white-space:pre-wrap;word-break:break-word}
  #log .t{color:var(--ghost)} .ok{color:var(--fg)} .er{color:var(--err)} .nf{color:var(--info)} .wn{color:var(--amber)}
  #log .cursor{color:var(--amber);animation:bl 1s steps(2) infinite}
  @keyframes bl{50%{opacity:0}}

  /* saved slots */
  .slots{padding:10px 12px;background:var(--bg)}
  .slotrow{display:flex;gap:8px;margin-bottom:8px}
  .slotrow input{flex:1;background:var(--bg);border:1px solid var(--ghost);border-radius:6px;
    color:var(--fg);font-family:var(--mono);font-size:12px;padding:6px 8px}
  ul.list{list-style:none;margin:0;padding:0;max-height:120px;overflow:auto}
  ul.list li{display:flex;align-items:center;gap:8px;border:1px solid var(--edge);
    border-radius:6px;padding:5px 8px;margin-bottom:6px;font-size:12px}
  ul.list .nm{flex:1;color:var(--fg);cursor:pointer;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
  ul.list .nm:hover{text-decoration:underline}
  ul.list .x{color:var(--err);border-color:transparent;padding:2px 7px}
  .empty{color:var(--ghost);font-size:12px;padding:4px 2px}

  /* reference */
  details.ref{margin-top:14px;border:1px solid var(--edge);border-radius:10px;background:var(--panel)}
  details.ref>summary{cursor:pointer;padding:10px 14px;color:var(--fg-dim);letter-spacing:1px;list-style:none}
  details.ref>summary::-webkit-details-marker{display:none}
  details.ref>summary::before{content:"[+] ";color:var(--fg)}
  details.ref[open]>summary::before{content:"[-] "}
  .refgrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(230px,1fr));gap:14px;padding:0 14px 14px}
  .refgrid h4{margin:10px 0 4px;color:var(--amber);font-size:12px;letter-spacing:1px}
  .refgrid code{display:block;color:var(--fg);font-size:12px;padding:1px 0}
  .refgrid code b{color:var(--fg-dim);font-weight:400}
  input[type=file]{display:none}
</style>
</head>
<body>
<div class="wrap">

  <header>
    <span class="brand">USB://<b>WARE</b></span>
    <span class="chips">
      <span class="chip">SSID <i id="cSsid">--</i></span>
      <span class="chip">CH <i id="cCh">--</i></span>
      <span class="chip"><span class="dot"></span> <i id="cLink">READY</i></span>
    </span>
  </header>

  <div class="grid">
    <!-- EDITOR -->
    <section class="card">
      <div class="bar">
        <span class="tag">PAYLOAD.duck</span>
        <button class="run" onclick="run()" id="runBtn">&#9654; RUN</button>
        <button onclick="clearEditor()">CLEAR</button>
        <label class="fbtn" for="file">IMPORT</label>
        <input type="file" id="file" accept=".txt,.duck,.dd" onchange="importFile(event)">
        <button onclick="exportFile()">EXPORT</button>
        <select id="examples" onchange="loadExample()">
          <option value="">examples&#9662;</option>
          <option value="hello">hello loop</option>
          <option value="run">open + type</option>
          <option value="count">counter (MATH/WHILE)</option>
          <option value="jiggle">mouse anti-idle</option>
          <option value="func">function demo (3.0)</option>
        </select>
        <input class="key" id="authKey" placeholder="key (opt)" autocomplete="off">
      </div>
      <div class="edit">
        <div class="gutter" id="gutter"><span>1</span></div>
        <textarea id="editor" spellcheck="false" autocapitalize="off" autocomplete="off"
          placeholder="REM type DuckyScript here&#10;STRING hello world&#10;ENTER"></textarea>
      </div>
      <div class="efoot">
        <span id="ln">LN 1</span>
        <span id="ch">CH 0</span>
        <span class="sp">tab = 2 spaces &middot; STRING_DELAY for slow hosts</span>
      </div>
    </section>

    <!-- SIDE -->
    <section class="card">
      <div class="bar"><span class="tag">SAVED SLOTS</span></div>
      <div class="slots">
        <div class="slotrow">
          <input id="slotName" placeholder="slot name" autocomplete="off">
          <button onclick="saveSlot()">SAVE</button>
        </div>
        <ul class="list" id="slotList"></ul>
      </div>
      <div class="bar"><span class="tag">CONSOLE</span><button onclick="clearLog()">CLR</button></div>
      <div id="log"></div>
    </section>
  </div>

  <!-- REFERENCE -->
  <details class="ref">
    <summary>COMMAND REFERENCE</summary>
    <div class="refgrid">
      <div>
        <h4>TEXT</h4>
        <code>STRING <b>text</b></code>
        <code>STRINGLN <b>text</b></code>
        <code>PRINT <b>text</b> <b>(serial)</b></code>
      </div>
      <div>
        <h4>KEYS</h4>
        <code>ENTER TAB ESC SPACE</code>
        <code>UP DOWN LEFT RIGHT</code>
        <code>HOME END DELETE F1..F12</code>
        <code>CTRL c &middot; CTRL ALT DELETE</code>
        <code>GUI r &middot; HOLD/RELEASE <b>key</b></code>
      </div>
      <div>
        <h4>TIMING</h4>
        <code>DELAY <b>ms</b> &middot; WAIT <b>ms</b></code>
        <code>DEFAULT_DELAY <b>ms</b></code>
        <code>STRING_DELAY <b>ms</b></code>
        <code>REPEAT <b>n</b></code>
      </div>
      <div>
        <h4>VARIABLES</h4>
        <code>VAR <b>x</b> = 5  (or VAR $x = 5)</code>
        <code>MATH <b>x</b> = (x + 1) * 2</code>
        <code>DEFINE <b>#URL</b> example.com</code>
        <code>STRING ${x}  &middot;  STRING #URL</code>
      </div>
      <div>
        <h4>FLOW</h4>
        <code>IF x &gt; 3 ... ELSE ... ENDIF</code>
        <code>IF (x &gt; 3) THEN ... END_IF</code>
        <code>WHILE x &lt; 9 ... ENDWHILE</code>
        <code>LOOP <b>n</b> ... ENDLOOP</code>
        <code>== != &gt; &lt; &gt;= &lt;=</code>
      </div>
      <div>
        <h4>FUNCTIONS</h4>
        <code>FUNCTION NAME ... ENDFUNCTION</code>
        <code>CALL NAME  &middot;  NAME()</code>
        <code>RETURN</code>
      </div>
      <div>
        <h4>MOUSE</h4>
        <code>MOUSE MOVE <b>x</b> <b>y</b></code>
        <code>MOUSE CLICK <b>LEFT|RIGHT|MIDDLE</b></code>
        <code>MOUSE SCROLL <b>n</b></code>
      </div>
    </div>
  </details>

  <!-- SETTINGS -->
  <details class="ref" id="setDrawer" ontoggle="if(this.open)loadSettings()">
    <summary>DEVICE SETTINGS</summary>
    <div style="padding:0 14px 14px">
      <div class="setstat" id="setStat">clients: -- &middot; free heap: -- B</div>
      <div class="setgrid">
        <label>SSID<input id="s_ssid" autocomplete="off"></label>
        <label>PASSWORD<input id="s_pass" autocomplete="off"></label>
        <label>CHANNEL (1-13)<input id="s_ch" type="number" min="1" max="13"></label>
        <label>MAX CLIENTS (1-8)<input id="s_max" type="number" min="1" max="8"></label>
        <label class="cbx"><input id="s_hidden" type="checkbox"> HIDDEN SSID</label>
        <label>ACCESS KEY (blank=off)<input id="s_akey" autocomplete="off"></label>
        <label>MAC<input id="s_mac" autocomplete="off" placeholder="DE:AD:BE:EF:01:02"></label>
        <label>IP<input id="s_ip" autocomplete="off"></label>
        <label>GATEWAY<input id="s_gw" autocomplete="off"></label>
        <label>SUBNET<input id="s_sn" autocomplete="off"></label>
        <label>DEFAULT_DELAY ms<input id="s_dd" type="number" min="0"></label>
        <label>STRING_DELAY ms<input id="s_sd" type="number" min="0"></label>
      </div>
      <div class="setrow">
        <label class="cbx"><input id="s_reboot" type="checkbox" checked> reboot to apply network changes</label>
        <button onclick="loadSettings()">RELOAD</button>
        <button class="run" onclick="saveSettings()">SAVE</button>
      </div>
      <div class="warn" id="setWarn"></div>
    </div>
  </details>
</div>

<style>
  .setstat{color:var(--fg-dim);font-size:12px;margin:4px 0 12px}
  .setgrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:10px}
  .setgrid label{display:flex;flex-direction:column;gap:4px;font-size:11px;color:var(--fg-dim);letter-spacing:1px}
  .setgrid input[type=text],.setgrid input[type=number],.setgrid input:not([type]){
    background:var(--bg);border:1px solid var(--ghost);border-radius:6px;color:var(--fg);
    font-family:var(--mono);font-size:13px;padding:7px 9px}
  .setgrid input:focus{outline:0;border-color:var(--fg);box-shadow:var(--glow)}
  label.cbx{flex-direction:row !important;align-items:center;gap:8px;color:var(--fg)}
  label.cbx input{width:auto}
  .setrow{display:flex;align-items:center;gap:12px;flex-wrap:wrap;margin-top:14px}
  .setrow .cbx{margin-right:auto}
  .warn{margin-top:10px;color:var(--amber);font-size:12px;min-height:16px}
</style>

<script>
var EX = {
  hello:"REM print a line a few times\nDEFAULT_DELAY 40\nLOOP 3\n  STRINGLN hello from usb ware\nENDLOOP",
  run:"REM open the run dialog and type (does NOT press enter)\nGUI r\nDELAY 400\nSTRING notepad\nREM remove the REM on the next line to launch:\nREM ENTER",
  count:"REM count down using arithmetic + a while loop\nVAR n = 5\nWHILE n > 0\n  STRING tick \nMATH n = n - 1\nENDWHILE\nSTRINGLN done",
  jiggle:"REM nudge the cursor to stay active\nLOOP 10\n  MOUSE MOVE 8 0\n  DELAY 500\n  MOUSE MOVE -8 0\n  DELAY 500\nENDLOOP",
  func:"REM DuckyScript 3.0 style function\nVAR count = 3\nbanner()\nFUNCTION banner()\n  WHILE count > 0\n    STRING *\nMATH count = count - 1\n  ENDWHILE\n  STRINGLN  ok\nENDFUNCTION"
};
var SKEY = "usbware.scripts";
var ed = document.getElementById('editor');
var gutter = document.getElementById('gutter');

function now(){var d=new Date();return d.toTimeString().slice(0,8);}
function log(msg,cls){
  var box=document.getElementById('log');
  var line=document.createElement('div'); line.className='l';
  line.innerHTML='<span class="t">'+now()+' </span><span class="'+(cls||'ok')+'">'+msg+'</span>';
  box.appendChild(line); box.scrollTop=box.scrollHeight;
}
function clearLog(){document.getElementById('log').innerHTML='';}
function clearEditor(){ed.value='';syncGutter();}

function syncGutter(){
  var lines=ed.value.split('\n').length;
  var html=''; for(var i=1;i<=lines;i++) html+='<span>'+i+'</span>';
  gutter.innerHTML=html||'<span>1</span>';
  document.getElementById('ln').textContent='LN '+lines;
  document.getElementById('ch').textContent='CH '+ed.value.length;
}
ed.addEventListener('input',syncGutter);
ed.addEventListener('scroll',function(){gutter.scrollTop=ed.scrollTop;});
ed.addEventListener('keydown',function(e){
  if(e.key==='Tab'){e.preventDefault();
    var s=ed.selectionStart,en=ed.selectionEnd;
    ed.value=ed.value.slice(0,s)+'  '+ed.value.slice(en);
    ed.selectionStart=ed.selectionEnd=s+2; syncGutter();}
});

function loadExample(){
  var sel=document.getElementById('examples'); var k=sel.value;
  if(k&&EX[k]){ed.value=EX[k];syncGutter();log('loaded example: '+k,'nf');}
  sel.value='';
}
function importFile(e){
  var f=e.target.files[0]; if(!f)return;
  var r=new FileReader();
  r.onload=function(ev){ed.value=ev.target.result;syncGutter();log('imported '+f.name+' ('+f.size+'B)','nf');};
  r.readAsText(f); e.target.value='';
}
function exportFile(){
  var blob=new Blob([ed.value],{type:'text/plain'});
  var a=document.createElement('a');
  a.href=URL.createObjectURL(blob); a.download='payload.duck'; a.click();
  URL.revokeObjectURL(a.href); log('exported payload.duck','nf');
}

/* ---- saved slots (browser localStorage) ---- */
function getSlots(){try{return JSON.parse(localStorage.getItem(SKEY))||{};}catch(e){return {};}}
function setSlots(o){localStorage.setItem(SKEY,JSON.stringify(o));}
function renderSlots(){
  var o=getSlots(),ul=document.getElementById('slotList'),keys=Object.keys(o);
  ul.innerHTML='';
  if(!keys.length){ul.innerHTML='<div class="empty">no saved scripts</div>';return;}
  keys.forEach(function(k){
    var li=document.createElement('li');
    var nm=document.createElement('span'); nm.className='nm'; nm.textContent=k;
    nm.onclick=function(){ed.value=o[k];syncGutter();log('loaded slot: '+k,'nf');};
    var x=document.createElement('button'); x.className='x'; x.textContent='DEL';
    x.onclick=function(){var s=getSlots();delete s[k];setSlots(s);renderSlots();log('deleted slot: '+k,'wn');};
    li.appendChild(nm); li.appendChild(x); ul.appendChild(li);
  });
}
function saveSlot(){
  var n=document.getElementById('slotName').value.trim();
  if(!n){log('slot name required','wn');return;}
  if(!ed.value.trim()){log('nothing to save','wn');return;}
  var o=getSlots(); o[n]=ed.value; setSlots(o);
  document.getElementById('slotName').value=''; renderSlots();
  log('saved slot: '+n,'ok');
}

/* ---- run ---- */
async function run(){
  var body=ed.value;
  if(!body.trim()){log('editor is empty','wn');return;}
  var btn=document.getElementById('runBtn'); btn.disabled=true;
  document.getElementById('cLink').textContent='SENDING';
  log('&gt; transmitting '+body.split('\n').length+' lines ('+body.length+'B)...','nf');
  var hdr={'Content-Type':'application/x-www-form-urlencoded'};
  var k=document.getElementById('authKey').value.trim(); if(k) hdr['X-USBWARE-KEY']=k;
  try{
    var res=await fetch('/set',{method:'POST',headers:hdr,
      body:'customScript='+encodeURIComponent(body)});
    var txt=await res.text();
    if(res.ok){log('&lt; '+(txt||'OK')+' <span class="cursor">_</span>','ok');
      document.getElementById('cLink').textContent='READY';}
    else{log('&lt; HTTP '+res.status+': '+txt,'er');
      document.getElementById('cLink').textContent='ERR '+res.status;}
  }catch(err){
    log('&lt; link failure: '+err,'er');
    document.getElementById('cLink').textContent='OFFLINE';
  }
  btn.disabled=false;
}

/* ---- device settings ---- */
function authHeaders(base){
  var h=base||{}; var k=document.getElementById('authKey').value.trim();
  if(k) h['X-USBWARE-KEY']=k; return h;
}
async function loadSettings(){
  document.getElementById('setWarn').textContent='';
  try{
    var r=await fetch('/api/settings',{headers:authHeaders({})});
    if(!r.ok){document.getElementById('setWarn').textContent='load failed: HTTP '+r.status;return;}
    var s=await r.json();
    var set=function(id,v){document.getElementById(id).value=v;};
    set('s_ssid',s.ssid); set('s_pass',s.pass); set('s_ch',s.ch); set('s_max',s.max);
    set('s_akey',s.akey); set('s_mac',s.mac); set('s_ip',s.ip); set('s_gw',s.gw);
    set('s_sn',s.sn); set('s_dd',s.ddelay); set('s_sd',s.sdelay);
    document.getElementById('s_hidden').checked=!!s.hidden;
    document.getElementById('setStat').innerHTML='clients: '+s.clients+' &middot; free heap: '+s.heap+' B';
    log('settings loaded','nf');
  }catch(e){document.getElementById('setWarn').textContent='load error: '+e;}
}
async function saveSettings(){
  var g=function(id){return document.getElementById(id).value;};
  var p=new URLSearchParams();
  p.set('ssid',g('s_ssid')); p.set('pass',g('s_pass')); p.set('ch',g('s_ch'));
  p.set('max',g('s_max')); p.set('akey',g('s_akey')); p.set('mac',g('s_mac'));
  p.set('ip',g('s_ip')); p.set('gw',g('s_gw')); p.set('sn',g('s_sn'));
  p.set('ddelay',g('s_dd')); p.set('sdelay',g('s_sd'));
  p.set('hidden',document.getElementById('s_hidden').checked?'1':'0');
  var reboot=document.getElementById('s_reboot').checked; p.set('reboot',reboot?'1':'0');
  document.getElementById('setWarn').textContent='';
  try{
    var r=await fetch('/api/settings',{method:'POST',
      headers:authHeaders({'Content-Type':'application/x-www-form-urlencoded'}),body:p.toString()});
    var t=await r.text();
    if(r.ok){ log('settings: '+t,'ok');
      if(reboot) document.getElementById('setWarn').textContent='Device rebooting. Reconnect to the (possibly renamed) AP, then reopen this page.';
    } else { log('settings save: HTTP '+r.status+' '+t,'er');
      document.getElementById('setWarn').textContent='save failed: HTTP '+r.status; }
  }catch(e){
    if(reboot){ log('settings saved; device dropped link on reboot','wn');
      document.getElementById('setWarn').textContent='Saved. Device is rebooting; reconnect to the AP.'; }
    else { document.getElementById('setWarn').textContent='save error: '+e; }
  }
}

/* ---- boot ---- */
(function(){
  document.getElementById('cSsid').textContent='USB Ware';
  document.getElementById('cCh').textContent='6';
  syncGutter(); renderSlots();
  var seq=['initializing HID bridge...','USB descriptor: KEYBOARD+MOUSE','AP up @ 10.0.0.1','interpreter v3-compat ready'];
  var i=0;(function step(){if(i<seq.length){log(seq[i++],'nf');setTimeout(step,160);}})();
})();
</script>
</body>
</html>
)rawliteral";

#endif